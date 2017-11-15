/***********************************************
 * name:Servo
 * function:you can see the servo motor rotate 90 degrees (rotate once every 15 degrees).
 * And then rotate in the opposite direction.
 ************************************************/
 //Email: support@sunfounder.com
//Website: www.sunfounder.com

#include <Servo.h>

// Constants
const int PIN_POT = A1;
const int PIN_LED = 6;
const int PIN_MOTOR = 5;

// Variables
uint16_t pot_value = 0;
Servo myservo; // create servo object to control a servo

// Functions
void flash_led();
void spin_motor();
void set_motor_speed(int speed);

/*******************/
void setup()
{
  //Serial.begin(9600);

  //pinMode(PIN_POT, INPUT);
  //pinMode(PIN_LED, OUTPUT);
  //pinMode(PIN_MOTOR, OUTPUT);

  // Turn off the motor
  //set_motor_speed(0); // Range is [0, 255]

  //attachs the servo on pin 9 to servo object
  myservo.attach(PIN_MOTOR);
  myservo.write(0);

  // Flash the LED once
  flash_led();
}
/**************************************************/

/*******************/
void loop() { 
  // Check to see if we should flash the LED
  //flash_led();

  // Read from the potentiometer
  //pot_value = analogRead(PIN_POT);

  // Spin the motor
  spin_motor();
}

// Spin the motor fast then slow
void spin_motor() {
  // Drive the motor, in a loop
  for (int speed = 0; speed <= 255; speed += 10) {
    set_motor_speed(speed);
    delay(500);
  }
  for (int speed = 255; speed >= 0; speed -= 10) {
    set_motor_speed(speed);
    delay(500);
  }
  set_motor_speed(0);
  delay(3000);
}

// Flash the LED once
void flash_led() {
  // fade in from min to max in increments of 5 points:
  for(int fadeValue = 0 ; fadeValue <= 255; fadeValue +=5) { 
    analogWrite(PIN_LED, fadeValue);         
    delay(30);                            
  } 

  // fade out from max to min in increments of 5 points:
  for(int fadeValue = 255 ; fadeValue >= 0; fadeValue -=5) { 
    analogWrite(PIN_LED, fadeValue);         
    delay(30);                            
  }
}

// Function to set the speed of the motor
void set_motor_speed(int speed) {
  int motorSpeed = constrain(abs(speed), 0, 255);
  myservo.write(motorSpeed);
  analogWrite(PIN_LED, motorSpeed);
}
