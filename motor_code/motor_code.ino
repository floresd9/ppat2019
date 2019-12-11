#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include <Adafruit_PWMServoDriver.h>
#include <Servo.h>

// Initialize variables
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
Adafruit_DCMotor *motor = AFMS.getMotor(2);
#define SERVOMIN  150 // This is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  600 // This is the 'maximum' pulse length count (out of 4096)
#define USMIN  600 // This is the rounded 'minimum' microsecond length based on the minimum pulse of 150
#define USMAX  2400 // This is the rounded 'maximum' microsecond length based on the maximum pulse of 600
#define SERVO_FREQ 60 // Analog servos run at ~60 Hz updates

// Initialize Servo, DC, and button pins
uint8_t arm1 = 0;
uint8_t arm2 = 4;
uint8_t page_flip = 2;
uint8_t book_holder1 = 3;
uint8_t book_holder2 = 7;
const int buttonPin = A1;

// Angle variables: base positions
int arm1_up = 60;
int page_flip_start = 180;
int book_holder_up = 130;

// Angle variables: intermediate positions
int arm1_down = 140;
int book_holder_down = 0;
int arm1_slightly_raised = 110;
int page_flip_end = 45;

// Wheel variables
int turn_speed = 225;
int turn_duration = 400;

// Helper funciton(s):
int convert(int degrees){
  /*
   * Convert given degrees to valid pwm units for Servo motor use
   * 
   * degrees: integer between 0 and 360 representing degree desired
   * 
   * return map to the corresponding pwm output
   */
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
  pwm.setPWM(arm1, 0, convert(arm1_up));
  pwm.setPWM(arm2, 0, convert(180-arm1_up));
  pwm.setPWM(page_flip, 0, convert(page_flip_start));
  pwm.setPWM(book_holder1, 0, convert(book_holder_up));
  pwm.setPWM(book_holder2, 0, convert(book_holder_up));
}

void loop() {
  Serial.println(analogRead(buttonPin));
  if (analogRead(buttonPin) == 0) {
    
    // Step 1: lower wheel onto book
    for (int degree = arm1_up; degree < arm1_down; degree++) {
      pwm.setPWM(arm1, 0, convert(degree));
      pwm.setPWM(arm2, 0, convert(180-degree));
    }
    delay(1000);

    // Step 2: remove book holder from turning page
    for (int degree = book_holder_up; degree > book_holder_down; degree--) {
      pwm.setPWM(book_holder1, 0, convert(degree));
    }
    delay(1000);

    // Step 3: Turn wheel
    motor->setSpeed(turn_speed);
    motor->run(FORWARD);
    delay(turn_duration);
    motor->setSpeed(0);
    delay(1000);
    
    // Step 4: slightly raise wheel to allow page to flip with less resistance
    for (int degree = arm1_down; degree > arm1_slightly_raised; degree--) {
      pwm.setPWM(arm1, 0, convert(degree));
      pwm.setPWM(arm2, 0, convert(180-degree));
    }
    delay(1000);

    // Step 5: flip page turer
    for (int degree = page_flip_start; degree > page_flip_end; degree--) {
      pwm.setPWM(page_flip, 0, convert(degree));
    }
    delay(300);

    // Step 6: remove book holder from side catching turned page
    for (int degree = book_holder_up; degree > book_holder_down; degree--) {
      pwm.setPWM(book_holder2, 0, convert(degree));
    }
    delay(500);
    
    // Step 7: re-place both book holders, having caught the turned page
    for (int degree = book_holder_down; degree < book_holder_up; degree++) {
      pwm.setPWM(book_holder1, 0, convert(degree));
      pwm.setPWM(book_holder2, 0, convert(degree));
    }
    delay(1000);
    
    // Step 8: Flip page turner back to reset
    for (int degree = page_flip_end; degree < page_flip_start; degree++) {
      pwm.setPWM(page_flip, 0, convert(degree));
    }
    delay(1000);

    // Step 9: raise wheel:
    for (int degree = arm1_slightly_raised; degree > arm1_up; degree--) {
      pwm.setPWM(arm1, 0, convert(degree));
      pwm.setPWM(arm2, 0, convert(180-degree));
    }
    delay(1000);
  }
}
