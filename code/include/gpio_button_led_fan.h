// Perform a variety of functions on the raspberry pi 3

#ifndef GPIO_BUTTON_LED_FAN_H
#define GPIO_BUTTON_LED_FAN_H

#include <iostream>
#include <fstream>
#include <thread>
#include <string>
#include <cstdlib>
#include <unistd.h>
#include <linux/reboot.h>
#include <sys/reboot.h>
#include <wiringPi.h>
#include "log_date.h"

// Easily disable the LED or the fan
#define HAS_LED1
#define HAS_LED2
#define HAS_FAN
const std::string ELEMENT_MAC = "bc:ae:c5:dc:5a:e2";  // For sodium (na)
//const std::string ELEMENT_MAC = "bc:ae:c5:dc:5a:98";  // For magnesium (mg)

// There is one LED, two buttons, and a fan
#define GPIO_LED1   18   // The first LED is connected to GPIO18 (pin 12, WPi 1) in ALT5 mode for PWM
#define GPIO_LED2   13   // The second LED is connected to GPIO13 (pin 33, WPi 23) in ALT0 mode for PWM
#define GPIO_BTN    27   // Button A is connected to GPIO27 (pin 13/gnd 1, WPi 2)
#define GPIO_FAN    23   // The fan is connected to GPIO23 (pin 16/gnd 14, WPi 4)
#define GPIO_IR     24   // The IR receiver is connected to GPIO24 (pin 18, WPi 5)

// Default to the internal LED if we have to
#define SYSFS_LED   "/sys/class/leds/led0"

// Debounce time is 40ms
#define DEBOUNCE_TIME 40

// Run forever, requires root privileges
int main();

// Check to ensure that the user is root
bool user_is_root();

// Set up the GPIO pins and clocks
void gpio_setup();

// Write directly to the /sys/ filesystesm
void sysfs_write(const std::string &path, const std::string &filename,
                 const std::string &value);

// Actually power off the system
int power_off();

// Control the LED
int led_off();
#ifdef HAS_LED1
int led_cycle_brightness();
void led_pulse(const unsigned int num_pulses);
#endif // HAS_LED1
void led_blink();

// Turn the fan on or off based on CPU temperature
#ifdef HAS_FAN
void fan_power(bool on);
#endif // HAS_FAN
void fan_control();

// Handle button presses
void button_action();

// Wake another machine via LAN
void wake_element();

#endif // GPIO_BUTTON_LED_FAN_H
