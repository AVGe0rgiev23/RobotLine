/*
  ZumoLineFollower - our race program for the Robo League line following.

  Robot: Arduino Uno + Pololu Zumo Shield v1.2 + Zumo reflectance sensor array.
  Library: ZumoShield (Pololu).
  Based on Pololu's LineFollower example, with these changes for racing:
    - 4 speed modes (Safe / Medium / Fast / Max) chosen with the button,
      each with its own tuning
    - soft start: motors build up to full speed in the first 0.15 s
      (smaller current spike, fewer Arduino restarts, less track slip)
    - optional battery check and battery compensation (needs the shield's
      "battery level" holes joined with a soldered wire, see BATTERY settings)
    - calibration and start are separate steps; the run starts the moment the
      button is released (no music, no delay)
    - the inside track may run backwards in tight turns (faster hairpins)
    - the robot slows down in curves and runs full speed on straights
    - drives straight over crossings
    - if the line is lost in a curve, spins towards the side where it was last seen
    - stops by itself at the end of the line
    - adjustable sensor timeout for a faster control loop

  HOW TO USE (the "user button" is the small button next to the power switch).
    Crossing practice: set CROSSING_PRACTICE to true and upload. Use Safe mode.
    The robot takes right, left, left at its first three detected crossings,
    then drives straight at later crossings. Set the flag to false for racing.
  Every beep is also shown on the yellow LED, so it works without the buzzer:
  a fast flicker = "hello/done", slow blinks = the mode number.
    1. Switch ON. Hello beep + flicker. The robot is in mode 1 (Safe): 1 beep/blink.
    2. CHOOSE MODE: short press = next mode. The robot beeps and blinks the
       mode number (1 = Safe, 2 = Medium, 3 = Fast, 4 = Max, then back to 1).
    3. CALIBRATE: put the robot ON the line, facing along it, so the line runs
       under the middle of the sensor bar (beside the start, not on the start
       line). Hold the button about 1 second, then let go. After 1 s the robot
       spins left and right for about 4 s, then beeps and flickers.
       LED stays on = ready.
       If a sensor did not see both white and black, you hear a low double tone
       and see 3 long blinks: calibration refused, LED stays off. Try again.
    4. Put the robot on the start line. Keep it switched on.
    5. START: on the judge's signal, press and release the button. The robot
       starts the instant you let go.
    6. STOP: the robot stops by itself at the end of the line (the line ends
       straight ahead), or if it loses the line in a curve and cannot find it
       within 1.5 s. You can also press the button while it drives. It stops,
       beeps, and is ready again (LED on). Short press = run again with the same
       calibration. Hold 1 s = calibrate again.
    7. CHANGE MODE after calibrating: type "mode N" in the Serial Monitor
       (see below), or press Reset, choose the mode and calibrate again.

  TUNING OVER USB (no upload, no recalibration):
    Keep the USB cable in and open the Serial Monitor at 9600 baud, with the
    line-ending menu (bottom right) set to "Newline". Type a command + Enter:
      help              list the commands
      show              show all 4 modes and the current one
      mode 3            switch to mode 3 (1 Safe, 2 Medium, 3 Fast, 4 Max)
      speed 380         maxSpeed of the current mode      (0..400)
      kp 1/4            Kp of the current mode, as N/D    (also "kp 1 4")
      kd 16             Kd of the current mode            (0..200)
      slow 180          turnSlowdown of the current mode  (0..maxSpeed)
      rev 250           reverseLimit of the current mode  (0..400)
      timeout 1000      sensor read timeout in us (1000..3000); faster loop
                        when lower. Calibrate again afterwards.
      save              remember all modes after switch-off (EEPROM)
      reset             forget saved values, use the numbers in this file
    A change works on the very next run. Without "save" it is lost at
    switch-off. If you edit the MODES table below and upload, the new code
    numbers are used and old saved values are ignored.
*/

#include <Wire.h>
#include <ZumoShield.h>
#include <EEPROM.h>

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

// Safe is the row we have tested on the robot. Medium, Fast and Max are
// starting guesses: tune each one on the practice track before racing with it.
// (These are the "code values". Values changed over USB and saved with "save"
// replace them until you edit this table and upload again, or type "reset".)
const Mode DEFAULT_MODES[] = {
  //  maxSpeed  kpNum  kpDen  kd  turnSlowdown  reverseLimit
  {   250,      1,     4,     6,  50,           100 },   // mode 1: Safe (tested)
  {   320,      1,     4,     8,  80,           150 },   // mode 2: Medium (untested)
  {   370,      1,     4,     10, 110,          200 },   // mode 3: Fast (untested)
  {   400,      1,     4,     12, 140,          250 },   // mode 4: Max (untested)
};
const int NUM_MODES = sizeof(DEFAULT_MODES) / sizeof(DEFAULT_MODES[0]);

// The settings the robot actually uses (start as the code values).
Mode modes[NUM_MODES];

// Sensor read timeout in microseconds. Lower = faster loop. Pololu recommends
// 1000..3000. Try 2000, then 1500, then 1000; keep the lowest value where the
// line still reads as clearly black after calibration.
const unsigned int SENSOR_TIMEOUT_US = 2000;

// Calibrated sensor values go from 0 (white) to 1000 (black).
const int BLACK_THRESHOLD = 700;     // above this, a sensor "sees black"
const int WHITE_THRESHOLD = 200;     // below this, a sensor "sees white"
const int CROSSING_MIN_BLACK = 5;    // this many black sensors = a crossing
const bool CROSSING_PRACTICE = true;  // enable the practice route
const unsigned long CROSSING_CLEAR_MS = 30; // ignore brief gaps in the crossing reading
enum TurnDirection { LEFT = -1, RIGHT = 1 };
const TurnDirection CROSSING_TURNS[] = {RIGHT, LEFT, LEFT};
const int CROSSING_TURN_COUNT = sizeof(CROSSING_TURNS) / sizeof(CROSSING_TURNS[0]);
const int CROSSING_TURN_SPEED = 150;
const unsigned long CROSSING_ADVANCE_MS = 120; // move the pivot toward the crossing
const unsigned long CROSSING_MIN_SPIN_MS = 150; // give the old line time to leave
const unsigned long CROSSING_LEFT_ADVANCE_MS = 180;
const unsigned long CROSSING_LEFT_MIN_SPIN_MS = 220;
const unsigned long CROSSING_MAX_SPIN_MS = 900;

const int LOST_SPIN_SPEED = 300;     // spin speed when the line is lost

// End of the track. If the line disappears while the robot is centred on it
// (|error| below END_CENTER_ERROR), the line has ended straight ahead: the
// robot keeps going straight at half speed for END_CONFIRM_MS to make sure,
// then stops. If the line disappears off one side (overshot a curve), it spins
// back towards it and stops only if it cannot find it within LOST_STOP_MS.
const int END_CENTER_ERROR = 1000;         // 0..2500
const unsigned long END_CONFIRM_MS = 100;
const unsigned long LOST_STOP_MS = 1500;
const int CALIBRATION_SPEED = 200;   // spin speed while calibrating
// After calibrating, every sensor's black value must be at least this much
// higher than its white value, or the calibration is refused (no line seen).
const unsigned int MIN_CALIBRATION_RANGE = 300;
const unsigned long LONG_PRESS_MS = 800;  // hold longer than this = calibrate

// Soft start: top speed grows from 0 to maxSpeed over this many ms.
const unsigned long START_RAMP_MS = 150;

// ---- BATTERY ----
// The Zumo Shield can measure the battery only if its "battery level" holes
// (next to "A1") are joined with a short soldered wire. Until then, keep
// BATTERY_WIRE_SOLDERED = false: the program then skips everything below.
const bool BATTERY_WIRE_SOLDERED = false;
const unsigned int LOW_BATTERY_MV = 4500;  // warn below this (4 NiMH AA ~ 4.8 V)
// Battery compensation: on a fuller battery the motors are fast, so we scale
// them down to behave like a NOMINAL_BATTERY_MV battery. Tuning then holds
// from fresh to half-empty batteries. Only works with the wire soldered.
const bool BATTERY_COMPENSATION = false;
const unsigned int NOMINAL_BATTERY_MV = 4800;

// =====================================================================
// Robot parts (pins are fixed by the Zumo Shield)
// =====================================================================

const int LED_PIN = 13;
const int NUM_SENSORS = 6;
unsigned char sensorPins[NUM_SENSORS] = { 4, A3, 11, A0, A2, 5 };  // left to right

// The Zumo sensor array, plus a way to change the read timeout while running.
class RaceSensors : public ZumoReflectanceSensorArray
{
public:
  void setTimeout(unsigned int timeoutUs) { _maxValue = timeoutUs; }
};

RaceSensors sensors;
ZumoMotors motors;
ZumoBuzzer buzzer;
Pushbutton button(ZUMO_BUTTON);  // pin 12

unsigned int sensorValues[NUM_SENSORS];
int currentMode = 0;   // index into modes (0 = Safe)
unsigned int sensorTimeout = SENSOR_TIMEOUT_US;  // can be changed over USB
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

// =====================================================================
// Saved settings (EEPROM) and the USB tuning commands
// =====================================================================

// What we store in EEPROM: a marker, the code values the saved values were
// based on (so new code values win after an upload), and the saved values.
struct SavedSettings {
  unsigned int marker;
  Mode basedOn[NUM_MODES];       // the code values when we saved
  unsigned int basedOnTimeout;
  Mode values[NUM_MODES];        // the saved values
  unsigned int timeout;
};
const unsigned int SETTINGS_MARKER = 0x5A32;
const int SETTINGS_ADDRESS = 0;

void loadSettings()
{
  memcpy(modes, DEFAULT_MODES, sizeof(modes));
  sensorTimeout = SENSOR_TIMEOUT_US;
  SavedSettings saved;
  EEPROM.get(SETTINGS_ADDRESS, saved);
  if (saved.marker != SETTINGS_MARKER)
  {
    Serial.println(F("Using the code values (nothing saved)."));
  }
  else if (memcmp(saved.basedOn, DEFAULT_MODES, sizeof(saved.basedOn)) != 0
           || saved.basedOnTimeout != SENSOR_TIMEOUT_US)
  {
    Serial.println(F("Code values changed since the last save: using the code values."));
  }
  else
  {
    memcpy(modes, saved.values, sizeof(modes));
    sensorTimeout = saved.timeout;
    Serial.println(F("Using SAVED values (type \"reset\" for the code values)."));
  }
}

void saveSettings()
{
  SavedSettings saved;
  saved.marker = SETTINGS_MARKER;
  memcpy(saved.basedOn, DEFAULT_MODES, sizeof(saved.basedOn));
  saved.basedOnTimeout = SENSOR_TIMEOUT_US;
  memcpy(saved.values, modes, sizeof(saved.values));
  saved.timeout = sensorTimeout;
  EEPROM.put(SETTINGS_ADDRESS, saved);
  Serial.println(F("Saved. These values are kept after switch-off."));
}

// Changes the sensor read timeout. The calibration must then be done again.
void setSensorTimeout(unsigned int timeoutUs)
{
  if (timeoutUs == sensorTimeout) return;
  sensorTimeout = timeoutUs;
  sensors.setTimeout(sensorTimeout);
  sensors.resetCalibration();
  calibrated = false;
  digitalWrite(LED_PIN, LOW);  // LED off = not ready
  Serial.println(F("Sensor timeout changed: calibrate again (hold the button)."));
}

void resetSettings()
{
  EEPROM.put(SETTINGS_ADDRESS, (unsigned int)0);  // remove the marker
  memcpy(modes, DEFAULT_MODES, sizeof(modes));
  setSensorTimeout(SENSOR_TIMEOUT_US);
  Serial.println(F("Back to the code values. Saved values removed."));
}

void printMode(int i)
{
  const Mode &m = modes[i];
  Serial.print(i == currentMode ? F("> mode ") : F("  mode "));
  Serial.print(i + 1);
  Serial.print(F(": speed "));
  Serial.print(m.maxSpeed);
  Serial.print(F("  kp "));
  Serial.print(m.kpNum);
  Serial.print('/');
  Serial.print(m.kpDen);
  Serial.print(F("  kd "));
  Serial.print(m.kd);
  Serial.print(F("  slow "));
  Serial.print(m.turnSlowdown);
  Serial.print(F("  rev "));
  Serial.println(m.reverseLimit);
}

void printAllModes()
{
  for (int i = 0; i < NUM_MODES; i++) printMode(i);
  Serial.print(F("  sensor timeout: "));
  Serial.print(sensorTimeout);
  Serial.println(F(" us"));
}

void printHelp()
{
  Serial.println(F("Commands (then Enter):"));
  Serial.println(F("  show | mode 1..4 | speed 0..400 | kp N/D | kd N | slow N | rev N"));
  Serial.println(F("  timeout 1000..3000 (then calibrate again)"));
  Serial.println(F("  save (keep after switch-off) | reset (code values) | help"));
}

// Reads a whole number from text; returns false if there is none.
bool readNumber(const char *text, int &value)
{
  if (text == NULL || *text == '\0') return false;
  char *end;
  long v = strtol(text, &end, 10);
  if (end == text) return false;
  value = (int)v;
  return true;
}

// Keeps a value between low and high.
int limit(int value, int low, int high)
{
  if (value < low) return low;
  if (value > high) return high;
  return value;
}

// Runs one typed command, for example "kd 16" or "kp 1/4".
void runCommand(char *line)
{
  char *word = strtok(line, " \t");
  if (word == NULL) return;
  char *arg1 = strtok(NULL, " \t/");
  char *arg2 = strtok(NULL, " \t/");
  for (char *c = word; *c; c++) *c = tolower(*c);

  Mode &m = modes[currentMode];
  int value;

  if (strcmp(word, "help") == 0)        { printHelp(); return; }
  else if (strcmp(word, "show") == 0)   { printAllModes(); return; }
  else if (strcmp(word, "save") == 0)   { saveSettings(); return; }
  else if (strcmp(word, "reset") == 0)  { resetSettings(); printAllModes(); return; }
  else if (strcmp(word, "mode") == 0 && readNumber(arg1, value))
  {
    currentMode = limit(value, 1, NUM_MODES) - 1;
    beepTimes(currentMode + 1);
  }
  else if (strcmp(word, "speed") == 0 && readNumber(arg1, value))
  {
    m.maxSpeed = limit(value, 0, 400);
  }
  else if (strcmp(word, "kp") == 0 && readNumber(arg1, value))
  {
    int den;
    m.kpNum = limit(value, 0, 100);
    if (readNumber(arg2, den)) m.kpDen = limit(den, 1, 100);
  }
  else if (strcmp(word, "kd") == 0 && readNumber(arg1, value))
  {
    m.kd = limit(value, 0, 200);
  }
  else if (strcmp(word, "slow") == 0 && readNumber(arg1, value))
  {
    m.turnSlowdown = limit(value, 0, 400);
  }
  else if (strcmp(word, "rev") == 0 && readNumber(arg1, value))
  {
    m.reverseLimit = limit(value, 0, 400);
  }
  else if (strcmp(word, "timeout") == 0 && readNumber(arg1, value))
  {
    setSensorTimeout(limit(value, 1000, 3000));
    printAllModes();
    return;
  }
  else
  {
    Serial.println(F("Unknown command. Type help."));
    return;
  }
  printMode(currentMode);
}

// Collects typed characters; runs a command when Enter arrives.
void handleSerial()
{
  static char line[32];
  static byte length = 0;
  while (Serial.available() > 0)
  {
    char c = Serial.read();
    if (c == '\n' || c == '\r')
    {
      if (length > 0)
      {
        line[length] = '\0';
        runCommand(line);
        length = 0;
      }
    }
    else if (length < sizeof(line) - 1)
    {
      line[length++] = c;
    }
  }
}

// Waits for one press of the button and returns how long it was held (ms).
// Returns when the button is released. While waiting, it answers USB commands.
unsigned long waitForPressDuration()
{
  while (true)
  {
    handleSerial();
    if (button.isPressed())
    {
      delay(15);                       // ignore contact bounce
      if (button.isPressed()) break;
    }
  }
  unsigned long pressedAt = millis();
  button.waitForRelease();
  return millis() - pressedAt;
}

// Battery voltage in millivolts (Pololu's formula: pin A1 sees 2/3 of it).
// Returns 0 if the battery wire is not soldered.
unsigned int readBatteryMillivolts()
{
  if (!BATTERY_WIRE_SOLDERED) return 0;
  return (unsigned int)((long)analogRead(A1) * 5000L * 3 / 2 / 1023);
}

// Prints the battery voltage and warns (low tone) if it is low.
void checkBattery()
{
  unsigned int mv = readBatteryMillivolts();
  if (mv == 0) return;  // not measured
  Serial.print(F("Battery: "));
  Serial.print(mv);
  Serial.println(F(" mV"));
  if (mv < LOW_BATTERY_MV)
  {
    Serial.println(F("Battery LOW: charge or replace the AA batteries."));
    buzzer.play("<c4");
  }
}

// Motor scale in percent for battery compensation (100 = no change).
int batteryScalePercent()
{
  if (!BATTERY_COMPENSATION) return 100;
  unsigned int mv = readBatteryMillivolts();
  if (mv <= NOMINAL_BATTERY_MV) return 100;   // cannot go faster than full power
  long percent = (long)NOMINAL_BATTERY_MV * 100 / mv;
  if (percent < 70) percent = 70;             // safety limit
  return (int)percent;
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
  printCalibration();

  // Check that every sensor saw both white and black.
  calibrated = true;
  for (int i = 0; i < NUM_SENSORS; i++)
  {
    unsigned int white = sensors.calibratedMinimumOn[i];
    unsigned int black = sensors.calibratedMaximumOn[i];
    if (black < white + MIN_CALIBRATION_RANGE)
    {
      calibrated = false;
      Serial.print(F("Sensor "));
      Serial.print(i);
      Serial.println(F(" did not see both white and black."));
    }
  }

  if (calibrated)
  {
    Serial.println(F("Calibration OK. LED on = ready."));
    buzzer.play(">g32>>c32");
    flickerLed();
  }
  else
  {
    // Low "error" tone + long LED blinks. Not ready: calibrate again.
    Serial.println(F("Calibration FAILED: put the robot on the line, facing along it, and hold the button again."));
    buzzer.play("<c8<c8");
    for (int i = 0; i < 3; i++)
    {
      digitalWrite(LED_PIN, HIGH);
      delay(400);
      digitalWrite(LED_PIN, LOW);
      delay(200);
    }
  }
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
  const Mode &m = modes[currentMode];
  lastError = 0;
  unsigned long loops = 0;
  const int scale = batteryScalePercent();   // measured once, before the motors start
  unsigned long startedAt = millis();
  unsigned long lostSince = 0;   // when the line disappeared (0 = line visible)
  bool lostCentred = false;      // was the robot centred on the line when it vanished?
  int nextTurn = 0;
  int turnDirection = 0;
  unsigned long turnStartedAt = 0;
  bool leftOldLine = false;
  bool crossingHandled = false;
  unsigned long lastCrossingSeen = 0;
  const __FlashStringHelper *stopReason = F("button pressed");

  while (true)
  {
    // 0. Soft start: the top speed grows to maxSpeed during START_RAMP_MS.
    unsigned long sinceStart = millis() - startedAt;
    int top = m.maxSpeed;
    if (sinceStart < START_RAMP_MS)
      top = (int)((long)m.maxSpeed * sinceStart / START_RAMP_MS);
    top = (int)((long)top * scale / 100);
    int reverse = (int)((long)m.reverseLimit * scale / 100);
    int slowdown = (int)((long)m.turnSlowdown * scale / 100);
    int spin = (int)((long)LOST_SPIN_SPEED * scale / 100);

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

    bool atCrossing = blackCount >= CROSSING_MIN_BLACK;
    if (CROSSING_PRACTICE)
    {
      unsigned long now = millis();
      if (atCrossing || turnDirection != 0) lastCrossingSeen = now;
      else if (now - lastCrossingSeen >= CROSSING_CLEAR_MS) crossingHandled = false;

      if (atCrossing && !crossingHandled && nextTurn < CROSSING_TURN_COUNT)
      {
        turnDirection = CROSSING_TURNS[nextTurn++];
        turnStartedAt = now;
        leftOldLine = false;
        crossingHandled = true;
      }
    }

    if (turnDirection != 0)
    {
      unsigned long turnTime = millis() - turnStartedAt;
      unsigned long advanceTime = turnDirection == LEFT ? CROSSING_LEFT_ADVANCE_MS : CROSSING_ADVANCE_MS;
      unsigned long minSpinTime = turnDirection == LEFT ? CROSSING_LEFT_MIN_SPIN_MS : CROSSING_MIN_SPIN_MS;
      int turnSpeed = (int)((long)CROSSING_TURN_SPEED * scale / 100);
      if (turnTime < advanceTime)
      {
        motors.setSpeeds(turnSpeed, turnSpeed);
      }
      else
      {
        // Wait for the middle sensors to leave the old line and find the new one.
        bool centreBlack = sensorValues[2] > BLACK_THRESHOLD || sensorValues[3] > BLACK_THRESHOLD;
        if (!centreBlack) leftOldLine = true;
        unsigned long spinTime = turnTime - advanceTime;
        if ((spinTime >= minSpinTime && leftOldLine && centreBlack && !atCrossing)
            || spinTime >= CROSSING_MAX_SPIN_MS)
        {
          turnDirection = 0;
          lostSince = 0;
          // If the line is missing, keep searching in the same direction.
          lastError = centreBlack ? position - 2500 : turnDirection * END_CENTER_ERROR;
          motors.setSpeeds(turnSpeed, turnSpeed);
        }
        else
        {
          motors.setSpeeds(turnDirection * turnSpeed, -turnDirection * turnSpeed);
        }
      }
    }
    else if (whiteCount == NUM_SENSORS)
    {
      // 4. No line under any sensor.
      if (lostSince == 0)
      {
        lostSince = millis();
        lostCentred = abs(lastError) < END_CENTER_ERROR;
      }
      unsigned long lostFor = millis() - lostSince;

      if (lostCentred)
      {
        // 4a. The line ended straight ahead: end of the track?
        //     Keep going straight briefly; stop if it really is gone.
        if (lostFor >= END_CONFIRM_MS)
        {
          motors.setSpeeds(0, 0);
          stopReason = F("end of the line");
          break;
        }
        motors.setSpeeds(top / 2, top / 2);
      }
      else
      {
        // 4b. The line went off one side (overshot a curve): spin back towards it.
        if (lostFor >= LOST_STOP_MS)
        {
          motors.setSpeeds(0, 0);
          stopReason = F("line lost and not found");
          break;
        }
        if (lastError < 0)
          motors.setSpeeds(-spin, spin);  // line was left
        else
          motors.setSpeeds(spin, -spin);  // line was right
      }
    }
    else if (atCrossing)
    {
      // 3. Crossing: drive straight, do not steer.
      lostSince = 0;
      motors.setSpeeds(top, top);
    }
    else
    {
      lostSince = 0;  // line visible
      // 5. Normal: PD steering.
      int error = position - 2500;  // negative = line to the left
      long correction = (long)error * m.kpNum / m.kpDen
                      + (long)m.kd * (error - lastError);
      lastError = error;

      // 6. Slow down in curves (big error), full speed on straights.
      int base = top - (int)((long)slowdown * abs(error) / 2500);

      // 7. Motor speeds. The inside track may go backwards (down to -reverseLimit).
      int left = clampSpeed(base + correction, -reverse, top);
      int right = clampSpeed(base - correction, -reverse, top);
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
  Serial.print(F("Stopped: "));
  Serial.println(stopReason);
  printMode(currentMode);  // the settings used, for the logbook
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
  loadSettings();
  sensors.init(sensorPins, NUM_SENSORS, sensorTimeout, ZUMO_SENSOR_ARRAY_DEFAULT_EMITTER_PIN);

  buzzer.play(">g32>>c32");
  flickerLed();
  checkBattery();
  beepTimes(currentMode + 1);
  Serial.println(F("ZumoLineFollower ready. Short press = next mode, hold = calibrate."));
  Serial.println(F("Tuning over USB: type help (Serial Monitor set to Newline)."));
  printAllModes();
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
