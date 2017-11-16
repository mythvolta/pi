// Perform a variety of functions on the raspberry pi 3

#include <iostream>
#include <fstream>
#include <thread>
#include <cstdlib>
#include <unistd.h>
#include <linux/reboot.h>
#include <sys/reboot.h>
#include <wiringPi.h>

// Easily disable the LED or the fan
#define HAS_LED
#define HAS_FAN
const std::string ELEMENT_MAC = "bc:ae:c5:dc:5a:e2";  // For sodium (na)
//const std::string ELEMENT_MAC = "bc:ae:c5:dc:5a:98";  // For magnesium (mg)

// There is one LED, two buttons, and a fan
#define GPIO_LED    18   // The LED is connected to GPIO18 (pin 12, WPi 1) in ALT5 mode for PWM
#define GPIO_BTN    27   // Button A is connected to GPIO27 (pin 13, WPi 2)
//#define GPIO_BTN_B  22 // Button B is connected to GPIO22 (pin 15, WPi 3)
#define GPIO_FAN    23   // The fan is connected to GPIO23 (pin 16, WPi 4)
#define GPIO_IR     23   // The IR receiver is connected to GPIO25 (pin 18, WPi5)

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
#ifdef HAS_LED
int led_cycle_brightness();
void led_pulse();
#endif // HAS_LED
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
