#include <Wire.h>
#include <Adafruit_MotorShield.h>
//#include "utility/Adafruit_MS_PWMServoDriver.h"
#include <Adafruit_PWMServoDriver.h>
#include <Servo.h>


Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
Adafruit_DCMotor *motor = AFMS.getMotor(2);
#define SERVOMIN  150 // This is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  600 // This is the 'maximum' pulse length count (out of 4096)
#define USMIN  600 // This is the rounded 'minimum' microsecond length based on the minimum pulse of 150
#define USMAX  2400 // This is the rounded 'maximum' microsecond length based on the maximum pulse of 600
#define SERVO_FREQ 60 // Analog servos run at ~60 Hz updates

// our servo # counter
uint8_t arm1 = 0;
uint8_t arm2 = 4;
uint8_t page_flip = 2;
uint8_t book_holder = 3;
const int buttonPin = A0;

int pos = 0;

int convert(int degrees){
  return map(degrees, 0, 180, 150, 600);
}

void setup() {
  Serial.begin(9600);
  pinMode(buttonPin, INPUT);
  // put your setup code here, to run once:
  AFMS.begin();
  pwm.begin();
  // In theory the internal oscillator is 25MHz but it really isn't
  // that precise. You can 'calibrate' by tweaking this number till
  // you get the frequency you're expecting!
  pwm.setOscillatorFrequency(27000000);  // The int.osc. is closer to 27MHz  
  pwm.setPWMFreq(SERVO_FREQ);  // Analog servos run at ~60 Hz updates
  pwm.setPWM(arm1, 0, convert(0));
  pwm.setPWM(arm2, 0, convert(0));
  pwm.setPWM(page_flip, 0, convert(0));
  pwm.setPWM(book_holder, 0, convert(0));
}

void loop() {
  Serial.println(analogRead(buttonPin));
  if (analogRead(buttonPin) == 0) {
    // Step 1: lower wheel onto book
    Serial.println("Hello");
    for (int degree = 0; degree < 120; degree++) {
      pwm.setPWM(arm1, 0, convert(degree));
      pwm.setPWM(arm2, 0, convert(180-degree));
    }
    delay(2000);
    Serial.println("Hi");
    // Step 2: raise book holder block
    for (int degree = 0; degree < 180; degree++) {
      pwm.setPWM(book_holder, 0, convert(degree));
    }
    delay(2000);
    Serial.println("Here");
    // Step 3: Turn wheel
    motor->setSpeed(150);
    motor->run(FORWARD);
    delay(1000);
    motor->setSpeed(0);
    delay(1000);
    Serial.println("Why");
    // Step 4: flip page turer
    for (int degree = 0; degree < 180; degree++) {
      pwm.setPWM(page_flip, 0, convert(degree));
    }
    delay(2000);
    // Step 4b: Flip page turner back to reset
    for (int degree = 180; degree > 0; degree--) {
      pwm.setPWM(page_flip, 0, convert(degree));
    }
    delay(2000);
    Serial.println("No");
    // Step 5: lower book holder block
    for (int degree = 180; degree > 0; degree--) {
      pwm.setPWM(book_holder, 0, convert(degree));
    }
    delay(2000);
    Serial.println("Work?");
    // Step 6: raise wheel:
    for (int degree = 120; degree > 0; degree--) {
      pwm.setPWM(arm1, 0, convert(degree));
      pwm.setPWM(arm2, 0, convert(180-degree));
    }
    delay(2000);
  }
}
