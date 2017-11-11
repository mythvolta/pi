// Perform a variety of functions on the raspberry pi 3

#include <iostream>
#include <thread>
#include <unistd.h>
#include <linux/reboot.h>
#include <sys/reboot.h>
#include <wiringPi.h>

// Easily disable the LED or the fan
#define HAS_LED
#define HAS_FAN

// There is one LED, two buttons, and a fan
#define GPIO_LED    18 // The LED is connected to GPIO18 (pin 12, WPi 1) in ALT5 mode for PWM
#define GPIO_BTN    27 // Button A is connected to GPIO27 (pin 13, WPi 2)
//#define GPIO_BTN_B  22 // Button B is connected to GPIO22 (pin 15, WPi 3)
#define GPIO_FAN    23 // The fan is connected to GPIO23 (pin 16, WPi 4)

// Debounce time is 100ms, the same as the query interval
#define DEBOUNCE_TIME 100

// Run forever, requires root privileges
int main();

// Check to ensure that the user is root
bool user_is_root();

// Set up the GPIO pins and clocks
void gpio_setup();

// Control the LED
#ifdef HAS_LED
int led_off();
int led_cycle_brightness();
void led_blink();
int led_pulse();
#endif // HAS_LED

// Turn the fan on or off based on CPU temperature
#ifdef HAS_FAN
void fan_power(bool on);
void fan_control();
#endif // HAS_FAN

// Handle button presses
void button_action();
