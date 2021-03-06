// Perform a variety of functions on the raspberry pi 3
// Commands:
//   Single click: toggle led
//   Double click: start/stop fan
//   Long press:   power off

#include "gpio_button_led_fan.h"

// Cheap global variables
static uint16_t led_brightness = 0; // Range is [0, 4]
static bool fan_override = false;
const static int brightness_modes[5] = {0, 1, 8, 32, 128};

// Run forever, requires root privileges
int main() {
  // Make sure the user is root
  if (!user_is_root()) {
    return 1;
  }

  // Set up the GPIO ports
  gpio_setup();

  // Watch for rising edge of the button
  wiringPiISR(GPIO_BTN, INT_EDGE_RISING, &button_action);

  // Keep track of how many other threads are running
  uint16_t num_threads = 0;

  // Use a new thread to see if we should be blinking the LED
  std::thread thread_led(led_blink);

  std::cout << date_time() << "Starting application by pulsing the LED" << std::endl;
  usleep(100);

  // Monitor temperature to control the fan in a new thread
  std::thread thread_fan(fan_control);

#ifdef HAS_LED
  led_pulse(5);
#endif // HAS_LED

  // Join all the threads to exit
  thread_led.join();
  thread_fan.join();

  std::cout << date_time() << "Finished, exiting from the main thread" << std::endl;
  return 0;
}

// Check to ensure that the user is root
bool user_is_root() {
  if (geteuid() != 0)
  {
    std::cout << date_time() << "Error, must be root to execute." << std::endl;
    return false;
  }
  return true;
}

// Set up the GPIO pins and clocks
void gpio_setup() {
  // Use the GPIO numbers for setup
  wiringPiSetupGpio();

  // Use mark/space instead of balanced for PWM
  pwmSetMode(PWM_MODE_MS);
  // Run a 10 KHz clock with range 0-128
  pwmSetClock(15);
  pwmSetRange(128);

  // Set up all the pins
  pinMode(GPIO_BTN, INPUT);
#ifdef HAS_LED
  pinMode(GPIO_LED, PWM_OUTPUT);
#endif // HAS_LED
#ifdef HAS_FAN
  pinMode(GPIO_FAN, OUTPUT);
#endif // HAS_FAN
}

// Write directly to the /sys/ filesystesm
void sysfs_write(const std::string &path, const std::string &filename,
                 const std::string &value) {
  std::cout << date_time() << "Writing " << value << " to " << path << "/" << filename << std::endl;
  std::ofstream fs;
  fs.open((path + "/" + filename).c_str());
  fs << value;
  fs.close();
}

// Actually power off the system
int power_off() {
  std::cout << date_time() << "Powering off" << std::endl;
  sync();
  const int val = reboot(LINUX_REBOOT_CMD_POWER_OFF);
  if (val != 0) {
    std::cout << date_time() << "Error shutting down, returned " << val << std::endl;
  }

  // Blink the led in error if we get this far
  fan_override = true;

  // Return
  return val;
}

// Turn the LED off
int led_off() {
  led_brightness = 0;
  std::cout << date_time() << "LED brightness reset to 0" << std::endl;
#ifdef HAS_LED
  pwmWrite(GPIO_LED, 0);
#else // Internal LED
  sysfs_write(SYSFS_LED, "trigger", "none");
  sysfs_write(SYSFS_LED, "brightness", "0");
#endif // HAS_LED
}

#ifdef HAS_LED
// Change the LED brightness between 5 possibilities
// Off, 25%, 50%, 75%, and full on
int led_cycle_brightness() {
  // In fan override mode, don't do anything
  if (fan_override) {
    return 1;
  }

  // Make sure we don't go above 4
  if (++led_brightness > 4) {
    led_brightness = 0;
  }
  std::cout << date_time() << "LED brightness cycled to #" << led_brightness
    << " (" << brightness_modes[led_brightness] << ")" << std::endl;

  // Set the PWM mark from range [0, 128]
  pwmWrite(GPIO_LED, brightness_modes[led_brightness]);

  return 0;
}

// Pulse the LED a number of times
void led_pulse(const unsigned int num_pulses) {
  std::cout << date_time() << "Pulsing the LED " << num_pulses
    << " time" << (num_pulses == 1 ? "" : "s") << std::endl;

  // Run the alloted number of times
  for (int pulse_num = 1; pulse_num <= num_pulses; ++pulse_num) {
    // Cycle it from off to on and back again, then sleep for half a second
    for (int b = 0; b < 128; ++b) {
      pwmWrite(GPIO_LED, b);
      usleep(5000);
    }
    usleep(100000);
    for (int b = 127; b >= 0; --b) {
      pwmWrite(GPIO_LED, b);
      usleep(5000);
    }
 
    // Don't pause after the last pulse
    if (pulse_num < num_pulses) {
      usleep(200000);
    }
  }
}
#endif // HAS_LED

// Blink the LED to show that the fan is stuck on
void led_blink() {

  // Keep track of previous state
  bool fan_override_previous = false;

  // Run forever
  while (true) {
    // Only blink when we're in fan_override mode
#ifdef HAS_LED
    if (fan_override) {
      pwmWrite(GPIO_LED, 32);
      usleep(200000);
      pwmWrite(GPIO_LED, 0);
      usleep(200000);
    }
    else {
      usleep(400000);
    }
#else // Internal LED
    // Start blinking
    if (fan_override && !fan_override_previous) {
      sysfs_write(SYSFS_LED, "trigger", "timer");
      sysfs_write(SYSFS_LED, "delay_on", "200");
      sysfs_write(SYSFS_LED, "delay_off", "200");
      fan_override_previous = fan_override;
    }
    else if (!fan_override && fan_override_previous) {
      led_off();
      fan_override_previous = fan_override;
    }
    else {
      usleep(400000);
    }
#endif // HAS_LED
  }

  return;
}

#ifdef HAS_FAN
// Power the fan on or off
void fan_power(bool on) {
  std::cout << date_time() << "Turning fan " << (on ? "on" : "off") << std::endl;
  const int state = (on ? HIGH : LOW);
  digitalWrite(GPIO_FAN, state);
}
#endif // HAS_FAN

// Turn the fan on or off based on CPU/GPU temperature
void fan_control() {
  FILE *temperature_file;
  double T;
  bool fan_is_on = false;

#ifdef HAS_FAN
  // Start with the fan off
  fan_power(fan_is_on);
#endif // HAS_FAN

  // Read the CPU temperature only every 10 seconds
  while (true) {
    temperature_file = fopen("/sys/class/thermal/thermal_zone0/temp", "r");
    if (temperature_file == NULL) {
      std::cout << date_time() << "Failed to read temperature" << std::endl;
    }
    else {
      fscanf(temperature_file, "%lf", &T);
      fclose(temperature_file);
      T /= 1000;
      char temp_cstr[16];
      sprintf(temp_cstr, "%02.2f", T);
      std::cout << date_time() << "The temperature is " << temp_cstr << " C" << std::endl;

      // Holding the button for 1 second will force the fan to be on
      if (!fan_override) {
        // Run the fan once the temperature hits 65C
        if (T >= 65) {
          // Pulse the fan twice every 30 seconds when the fan is on
          if (!fan_is_on) {
            fan_is_on = true;
#ifdef HAS_FAN
            fan_power(fan_is_on);
#endif // HAS_FAN
          }
#ifdef HAS_LED
          led_pulse(2);
#endif // HAS_LED
        }
        // Stop the fan when we get down to 60C
        else if (fan_is_on && T < 60) {
          fan_is_on = false;
#ifdef HAS_FAN
          fan_power(fan_is_on);
#endif // HAS_FAN
        }
      }
    }

    // Sleep for a while
    sleep(30);
  }

  // Return, though this should never happen
  return;
}

// Debounce the button, and check to see how long it was held
void button_action() {
  // Keep track of the time, make sure
  //  it is not within 100ms of the previous press
  static unsigned long time_previous = 0;
  const unsigned long time_current = millis();
  const long time_delta = (time_current - time_previous);
  if (time_delta > DEBOUNCE_TIME) {
    // Immediately cycle the brightness of the LED,
    //  unless the fan is stuck on
#ifdef HAS_LED
    if (!fan_override) {
      led_cycle_brightness();
    }
#endif // HAS_LED

    // A double click switches between fan_override mode
    if (time_delta < 200) {
      std::cout << date_time() << "Button was double-pressed, time was " << time_delta << std::endl;
      led_off();
      fan_override = !fan_override;
#ifdef HAS_FAN
      fan_power(fan_override);
#endif // HAS_FAN
    }
    // Normal press
    else {
      // Check every 100 ms until it is no longer held
      unsigned int hold_intervals = 0;
      bool button_held = true;
      while (button_held) {
        usleep(100000); // 100 ms
        if (digitalRead(GPIO_BTN)) {
          ++hold_intervals;

          // At 3 seconds, pulse the LED a few times, then power off
          if (hold_intervals >= 30) {
#ifdef HAS_LED
            led_pulse(3);
#endif // HAS_LED
            power_off();
            button_held = false;
          }
        }
        else {
          button_held = false;
        }
      }
      std::cout << date_time() << "Button was held for " << hold_intervals * 100 << " ms" << std::endl;
    }

    // Update the old time and return
    time_previous = millis();
  }

  return;
}

// Wake another machine via LAN
void wake_element() {
  const std::string command = "wakeonlan " + ELEMENT_MAC;
  std::cout << date_time() << command << std::endl;
  std::system(command.c_str());
}
