/*
  ZumoLineFollower - our race program for the Robo League line following.

  Robot: Arduino Uno + Pololu Zumo Shield v1.2 + Zumo reflectance sensor array.
  Library: ZumoShield (Pololu).
  Based on Pololu's LineFollower example, with these changes for racing:
    - speed modes (Safe / Fast) chosen with the button, each with its own tuning
    - calibration and start are separate steps; the run starts the moment the
      button is released (no music, no delay)
    - the inside track may run backwards in tight turns (faster hairpins)
    - the robot slows down in curves and runs full speed on straights
    - drives straight over crossings
    - if the line is lost, spins towards the side where it was last seen
    - adjustable sensor timeout for a faster control loop

  HOW TO USE (the "user button" is the small button next to the power switch).
  Every beep is also shown on the yellow LED, so it works without the buzzer:
  a fast flicker = "hello/done", slow blinks = the mode number.
    1. Switch ON. Hello beep + flicker. The robot is in mode 1 (Safe): 1 beep/blink.
    2. CHOOSE MODE: short press = next mode. The robot beeps and blinks the
       mode number (1 = Safe, 2 = Fast).
    3. CALIBRATE: put the robot across the line (beside the start, not on the
       start line). Hold the button about 1 second, then let go. After 1 s the
       robot spins left and right for about 4 s, then beeps and flickers.
       LED stays on = ready.
    4. Put the robot on the start line. Keep it switched on.
    5. START: on the judge's signal, press and release the button. The robot
       starts the instant you let go.
    6. STOP (testing only): press the button while it drives. It stops, beeps,
       and is ready again (LED on). Short press = run again with the same
       calibration. Hold 1 s = calibrate again.
    7. CHANGE MODE after calibrating: press the Reset button, choose the mode
       again (step 2) and calibrate again (step 3).
*/

#include <Wire.h>
#include <ZumoShield.h>

// =====================================================================
// TUNING - change these numbers while testing. Write every change in the
// logbook (docs/logbook.md). Change ONE number at a time.
// =====================================================================

struct Mode {
  int maxSpeed;      // top speed on straights, 0..400
  int kpNum;         // Kp = kpNum / kpDen  (steering towards the line)
  int kpDen;
  int kd;            // Kd (stops the wobble; raise it when speed goes up)
  int turnSlowdown;  // speed taken away when the line is at the far edge, 0..maxSpeed
  int reverseLimit;  // how fast the inside track may run backwards in turns, 0..400
};

const Mode MODES[] = {
  //  maxSpeed  kpNum  kpDen  kd  turnSlowdown  reverseLimit
  {   250,      1,     4,     6,  50,           100 },   // mode 1: Safe
  {   400,      1,     4,     8,  120,          200 },   // mode 2: Fast
};
const int NUM_MODES = sizeof(MODES) / sizeof(MODES[0]);

// Sensor read timeout in microseconds. Lower = faster loop. Pololu recommends
// 1000..3000. Try 2000, then 1500, then 1000; keep the lowest value where the
// line still reads as clearly black after calibration.
const unsigned int SENSOR_TIMEOUT_US = 2000;

// Calibrated sensor values go from 0 (white) to 1000 (black).
const int BLACK_THRESHOLD = 700;     // above this, a sensor "sees black"
const int WHITE_THRESHOLD = 200;     // below this, a sensor "sees white"
const int CROSSING_MIN_BLACK = 5;    // this many black sensors = a crossing

const int LOST_SPIN_SPEED = 300;     // spin speed when the line is lost
const int CALIBRATION_SPEED = 200;   // spin speed while calibrating
const unsigned long LONG_PRESS_MS = 800;  // hold longer than this = calibrate

// =====================================================================
// Robot parts (pins are fixed by the Zumo Shield)
// =====================================================================

const int LED_PIN = 13;
const int NUM_SENSORS = 6;
unsigned char sensorPins[NUM_SENSORS] = { 4, A3, 11, A0, A2, 5 };  // left to right

ZumoReflectanceSensorArray sensors;
ZumoMotors motors;
ZumoBuzzer buzzer;
Pushbutton button(ZUMO_BUTTON);  // pin 12

unsigned int sensorValues[NUM_SENSORS];
int currentMode = 0;   // index into MODES (0 = Safe)
int lastError = 0;     // error from the previous loop (for Kd)
bool calibrated = false;

// =====================================================================
// Small helpers
// =====================================================================

// Beeps AND blinks the yellow LED n times (shows the mode number).
// The LED blinks work even if the buzzer is silent.
void beepTimes(int n)
{
  digitalWrite(LED_PIN, LOW);
  delay(300);
  for (int i = 0; i < n; i++)
  {
    digitalWrite(LED_PIN, HIGH);
    buzzer.playNote(NOTE_C(6), 150, 15);
    delay(250);
    digitalWrite(LED_PIN, LOW);
    delay(250);
  }
}

// Fast flicker of the LED: "done" signal that works without the buzzer.
void flickerLed()
{
  for (int i = 0; i < 6; i++)
  {
    digitalWrite(LED_PIN, HIGH);
    delay(50);
    digitalWrite(LED_PIN, LOW);
    delay(50);
  }
}

// Waits for one press of the button and returns how long it was held (ms).
// Returns when the button is released.
unsigned long waitForPressDuration()
{
  button.waitForPress();
  unsigned long pressedAt = millis();
  button.waitForRelease();
  return millis() - pressedAt;
}

int clampSpeed(long speed, int lowest, int highest)
{
  if (speed < lowest) return lowest;
  if (speed > highest) return highest;
  return (int)speed;
}

// Spins left and right over the line while the sensors learn black and white.
void calibrate()
{
  digitalWrite(LED_PIN, LOW);
  delay(1000);  // time to take your hand away

  sensors.resetCalibration();
  for (int i = 0; i < 100; i++)
  {
    // left, right, left, right ... in blocks, like Pololu's example
    if ((i > 12 && i <= 37) || (i > 62 && i <= 87))
      motors.setSpeeds(-CALIBRATION_SPEED, CALIBRATION_SPEED);
    else
      motors.setSpeeds(CALIBRATION_SPEED, -CALIBRATION_SPEED);
    sensors.calibrate();
    delay(20);
  }
  motors.setSpeeds(0, 0);
  calibrated = true;

  printCalibration();
  buzzer.play(">g32>>c32");
  flickerLed();
}

// Prints what each sensor learned (helps when tuning SENSOR_TIMEOUT_US).
// Not used during a run.
void printCalibration()
{
  Serial.println(F("Calibration (min = white, max = black), left to right:"));
  for (int i = 0; i < NUM_SENSORS; i++)
  {
    Serial.print(sensors.calibratedMinimumOn[i]);
    Serial.print('/');
    Serial.print(sensors.calibratedMaximumOn[i]);
    Serial.print(i < NUM_SENSORS - 1 ? "  " : "\n");
  }
}

// =====================================================================
// The race: one fast control loop until the button is pressed
// =====================================================================

void race()
{
  const Mode &m = MODES[currentMode];
  lastError = 0;
  unsigned long loops = 0;
  unsigned long startedAt = millis();

  while (true)
  {
    // 1. Read the sensors. position: 0 (line under left sensor) .. 5000 (right).
    int position = sensors.readLine(sensorValues);

    // 2. Count black and white sensors.
    int blackCount = 0;
    int whiteCount = 0;
    for (int i = 0; i < NUM_SENSORS; i++)
    {
      if (sensorValues[i] > BLACK_THRESHOLD) blackCount++;
      if (sensorValues[i] < WHITE_THRESHOLD) whiteCount++;
    }

    if (blackCount >= CROSSING_MIN_BLACK)
    {
      // 3. Crossing: drive straight, do not steer.
      motors.setSpeeds(m.maxSpeed, m.maxSpeed);
    }
    else if (whiteCount == NUM_SENSORS)
    {
      // 4. Line lost: spin towards the side where we saw it last. Never stop.
      if (lastError < 0)
        motors.setSpeeds(-LOST_SPIN_SPEED, LOST_SPIN_SPEED);  // line was left
      else
        motors.setSpeeds(LOST_SPIN_SPEED, -LOST_SPIN_SPEED);  // line was right
    }
    else
    {
      // 5. Normal: PD steering.
      int error = position - 2500;  // negative = line to the left
      long correction = (long)error * m.kpNum / m.kpDen
                      + (long)m.kd * (error - lastError);
      lastError = error;

      // 6. Slow down in curves (big error), full speed on straights.
      int base = m.maxSpeed - (int)((long)m.turnSlowdown * abs(error) / 2500);

      // 7. Motor speeds. The inside track may go backwards (down to -reverseLimit).
      int left = clampSpeed(base + correction, -m.reverseLimit, m.maxSpeed);
      int right = clampSpeed(base - correction, -m.reverseLimit, m.maxSpeed);
      motors.setSpeeds(left, right);
    }

    loops++;

    // Stop button (for testing at home).
    if (button.getSingleDebouncedPress())
    {
      motors.setSpeeds(0, 0);
      break;
    }
  }

  // After the run: report how fast the loop was (helps with SENSOR_TIMEOUT_US).
  unsigned long ms = millis() - startedAt;
  Serial.print(F("Run stopped after "));
  Serial.print(ms);
  Serial.print(F(" ms, loops per second: "));
  Serial.println(ms > 0 ? loops * 1000UL / ms : 0);

  button.waitForRelease();
  buzzer.play(">g32>>c32");
  flickerLed();
}

// =====================================================================
// Setup and main menu
// =====================================================================

void setup()
{
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
  sensors.init(sensorPins, NUM_SENSORS, SENSOR_TIMEOUT_US, ZUMO_SENSOR_ARRAY_DEFAULT_EMITTER_PIN);

  buzzer.play(">g32>>c32");
  flickerLed();
  beepTimes(currentMode + 1);
  Serial.println(F("ZumoLineFollower ready. Short press = next mode, hold = calibrate."));
}

void loop()
{
  // LED on = calibrated and ready to race.
  digitalWrite(LED_PIN, calibrated ? HIGH : LOW);

  unsigned long held = waitForPressDuration();

  if (held >= LONG_PRESS_MS)
  {
    calibrate();
  }
  else if (!calibrated)
  {
    // Not calibrated yet: a short press changes the mode.
    currentMode = (currentMode + 1) % NUM_MODES;
    beepTimes(currentMode + 1);
  }
  else
  {
    // Calibrated: a short press starts the run the instant it is released.
    digitalWrite(LED_PIN, LOW);
    race();
  }
}
