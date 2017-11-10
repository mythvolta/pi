// Perform a variety of functions on the raspberry pi 3

#include <iostream>
#include <thread>
#include <unistd.h>
#include <wiringPi.h>

// There is one LED, two buttons, and a fan
#define GPIO_LED    18 // The LED is connected to GPIO18 (pin 12, WPi 1) in ALT5 mode for PWM
#define GPIO_BTN    27 // Button A is connected to GPIO27 (pin 13, WPi 2)
//#define GPIO_BTN_B  22 // Button B is connected to GPIO22 (pin 15, WPi 3)
#define GPIO_FAN    23 // The fan is connected to GPIO23 (pin 16, WPi 4)

// Debounce time is 100ms, the same as the query interval
#define DEBOUNCE_TIME 100

// Run forever, requires root privileges
int main();

// Control the LED
int led_off();
int led_cycle_brightness();
int led_blink();
int led_pulse();

// Turn the fan on or off based on CPU/GPU temperature
void fan_control();

// Handle button presses
void button_action();
