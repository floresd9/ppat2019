#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"
#include <Servo.h>


Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor *motor = AFMS.getMotor(2);
Servo arm1;
Servo arm2;
const int buttonPin = A0;

int pos = 0;

void setup() {
  pinMode(buttonPin, INPUT);
  // put your setup code here, to run once:
  AFMS.begin();
  pinMode(9, OUTPUT);  
  pinMode(10, OUTPUT);
  arm1.attach(9);
  arm2.attach(10);
  arm1.write(0);
  arm2.write(180);
}

void loop() {
  
  if (analogRead(buttonPin) == 1) {
    for (pos = 0; pos <= 120; pos += 1) { // goes from 0 degrees to 180 degrees
      // in steps of 1 degree
        arm1.write(pos);              // tell servo to go to position in variable 'pos'
        arm2.write(180-pos);              // tell servo to go to position in variable 'pos'
        delay(5);                 // waits 15ms for the servo to reach the position
    }
    // put your main code here, to run repeatedly:
    motor->setSpeed(150);
    motor->run(FORWARD);
    delay(1000);
    motor->setSpeed(0);
    delay(1000);
    for (pos = 120; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
        arm1.write(pos);              // tell servo to go to position in variable 'pos'
        arm2.write(180-pos);              // tell servo to go to position in variable 'pos'                       // waits 15ms for the servo to reach the position
        delay(5);
    }  
  }
}
