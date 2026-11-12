/*
  SetupCheck - tests every part of our Zumo before we write the race code.

  Hardware: Arduino Uno + Pololu Zumo Shield v1.2 + Zumo reflectance sensor array.
  Library:  ZumoShield (Pololu), install it from the Library Manager.

  What it does:
    1. On start: plays a short beep (buzzer works) and the yellow LED (pin 13)
       starts blinking (the program is running).
    2. Every 0.2 s: prints the 6 raw sensor readings to the Serial Monitor
       (9600 baud). Low numbers = white, high numbers = black.
       Readings only make sense with batteries in and the power switch ON.
    3. Short press of the user button (pin 12, NOT the Reset button):
       runs a short motor test. Needs batteries and the power switch ON.
       Lift the robot first so the tracks touch nothing.
    4. Hold the user button about 1 second, then let go: STRAIGHT-DRIVE TEST.
       Put the robot on the floor with 1.5 m of free space in front. After
       1 s it drives straight ahead for 2 s. If it curves, one motor is weaker
       (curves left = left side slower).
*/

#include <Wire.h>
#include <ZumoShield.h>

// ---- Parts of the robot ----
ZumoReflectanceSensorArray sensors;  // sensor pins 4, A3, 11, A0, A2, 5; LEDON on pin 2
ZumoMotors motors;                   // motor pins 7, 8, 9, 10; speeds from -400 to 400
ZumoBuzzer buzzer;                   // pin 3 on an Uno ("Bz." jumper on 328P)
Pushbutton button(ZUMO_BUTTON);      // pin 12

// ---- Settings ----
const int LED_PIN = 13;
const int NUM_SENSORS = 6;
const int TEST_SPEED = 150;          // motor test speed, out of 400 (gentle)
const int STEP_TIME_MS = 600;        // how long each motor step runs
const int STRAIGHT_SPEED = 200;      // straight-drive test speed, out of 400
const int STRAIGHT_TIME_MS = 2000;   // straight-drive test duration
const unsigned long LONG_PRESS_MS = 800;  // hold longer than this = straight test

unsigned int sensorValues[NUM_SENSORS];

void setup()
{
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
  sensors.init();

  buzzer.play(">g32>>c32");  // short "hello" beep

  Serial.println(F("=== Zumo setup check ==="));
  Serial.println(F("Sensor readings (left to right): S0 S1 S2 S3 S4 S5"));
  Serial.println(F("White = low numbers, black = high numbers."));
  Serial.println(F("Press the user button for the motor test (lift the robot first)."));
  Serial.println();
}

// Runs one motor step, prints what should happen, then stops.
void motorStep(const __FlashStringHelper *what, int leftSpeed, int rightSpeed)
{
  Serial.println(what);
  buzzer.play("c32");
  motors.setSpeeds(leftSpeed, rightSpeed);
  delay(STEP_TIME_MS);
  motors.setSpeeds(0, 0);
  delay(400);
}

void runMotorTest()
{
  Serial.println(F("--- Motor test ---"));
  delay(500);
  motorStep(F("LEFT track FORWARD"), TEST_SPEED, 0);
  motorStep(F("LEFT track BACKWARD"), -TEST_SPEED, 0);
  motorStep(F("RIGHT track FORWARD"), 0, TEST_SPEED);
  motorStep(F("RIGHT track BACKWARD"), 0, -TEST_SPEED);
  motorStep(F("BOTH tracks FORWARD"), TEST_SPEED, TEST_SPEED);
  motorStep(F("SPIN on the spot (left back, right forward)"), -TEST_SPEED, TEST_SPEED);
  Serial.println(F("--- Motor test done. Did every step match? ---"));
  Serial.println();
  buzzer.play(">g32>>c32");
}

// Drives straight for STRAIGHT_TIME_MS so we can see if the robot curves.
void runStraightTest()
{
  Serial.println(F("--- Straight-drive test: starting in 1 s ---"));
  delay(1000);  // time to take your hand away
  buzzer.play("c32");
  motors.setSpeeds(STRAIGHT_SPEED, STRAIGHT_SPEED);
  delay(STRAIGHT_TIME_MS);
  motors.setSpeeds(0, 0);
  Serial.println(F("--- Done. Did it go straight, or curve left/right? ---"));
  Serial.println();
  buzzer.play(">g32>>c32");
}

void loop()
{
  static unsigned long lastPrint = 0;

  if (millis() - lastPrint >= 200)
  {
    lastPrint = millis();

    sensors.read(sensorValues);  // raw values, 0 (very white) to 2000 (very black)

    char line[48];
    snprintf(line, sizeof(line), "%4u %4u %4u %4u %4u %4u",
             sensorValues[0], sensorValues[1], sensorValues[2],
             sensorValues[3], sensorValues[4], sensorValues[5]);
    Serial.println(line);

    digitalWrite(LED_PIN, !digitalRead(LED_PIN));  // heartbeat blink
  }

  // Short press = motor test, hold about 1 s = straight-drive test.
  if (button.isPressed())
  {
    unsigned long pressedAt = millis();
    button.waitForRelease();
    if (millis() - pressedAt >= LONG_PRESS_MS)
      runStraightTest();
    else
      runMotorTest();
  }
}
