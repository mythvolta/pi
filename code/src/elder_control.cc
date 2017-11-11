// Perform a variety of functions on the raspberry pi 3

#include "elder_control.h"

// Cheap global variables
static uint16_t led_brightness = 0; // Range is [0, 4]
static bool fan_override = false;
const static int brightness_modes[5] = {0, 1, 5, 20, 128};

// Run forever, requires root privileges
int main() {
  // Make sure the user is root
  if (!user_is_root()) {
    return 1;
  }

  // Set up the GPIO ports
  gpio_setup();

  // Turn off the LED to start
  led_off();

  // Watch for rising edge of the button
  wiringPiISR(GPIO_BTN, INT_EDGE_RISING, &button_action);

  // Monitor temperature to control the fan in a new thread
  std::thread thread_fan(fan_control);
  std::thread thread_led(led_blink);

  // Join all the threads to exit
  std::cout << "Waiting in the main thread" << std::endl;
  thread_fan.join();
  thread_led.join();

  std::cout << "Finished, exiting from the main thread" << std::endl;
  return 0;
}

// Check to ensure that the user is root
bool user_is_root() {
  if (geteuid() != 0)
  {
    std::cout << "Error, must be root to execute." << std::endl;
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
  pinMode(GPIO_LED, PWM_OUTPUT);
  pinMode(GPIO_BTN, INPUT);
  pinMode(GPIO_FAN, OUTPUT);
}

// Turn the LED off
int led_off() {
  led_brightness = 0;
  std::cout << "LED brightness reset to 0" << std::endl;
  pwmWrite(GPIO_LED, 0);
}

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
  std::cout << "LED brightness cycled to #" << led_brightness
    << " (" << brightness_modes[led_brightness] << ")" << std::endl;

  // Set the PWM mark from range [0, 128]
  pwmWrite(GPIO_LED, brightness_modes[led_brightness]);

  return 0;
}

// Blink the LED to show that the fan is stuck on
void led_blink() {
  while (true) {
    // Only blink when we're in fan_override mode
    if (fan_override) {
      pwmWrite(GPIO_LED, 32);
      usleep(150000);
      pwmWrite(GPIO_LED, 0);
      usleep(150000);
    }
    else {
      usleep(300000);
    }
  }

  return;
}

// Pulse the LED a few times before powering down
int led_pulse() {
  std::cout << "Pulsing the LED before powering off" << std::endl;

  // Cycle it from off to on and back again, 3 times
  for (unsigned int pulse_number = 0; pulse_number < 3; ++pulse_number) {
    for (int b = 0; b < 128; ++b) {
      pwmWrite(GPIO_LED, b);
      usleep(5000);
    }
    usleep(300000);
    for (int b = 127; b > 0; --b) {
      pwmWrite(GPIO_LED, b);
      usleep(5000);
    }
  }

  // Actually power off the system
  std::cout << "Powering off" << std::endl;
  sync();
  const int val = reboot(LINUX_REBOOT_CMD_POWER_OFF);
  if (val != 0) {
    std::cout << "Error shutting down, returned " << val << std::endl;
  }

  // Blink the led in error
  fan_override = true;

  // Return
  return val;
}

// Power the fan on or off
void fan_power(bool on) {
  std::cout << "Turning fan " << (on ? "on" : "off") << std::endl;
  const int state = (on ? HIGH : LOW);
  digitalWrite(GPIO_FAN, state);
}

// Turn the fan on or off based on CPU/GPU temperature
void fan_control() {
  FILE *temperature_file;
  double T;
  bool fan_is_on = false;

  // Start with the fan off
  fan_power(fan_is_on);

  // Read the CPU temperature only every 10 seconds
  while (true) {
    temperature_file = fopen("/sys/class/thermal/thermal_zone0/temp", "r");
    if (temperature_file == NULL) {
      std::cout << "Failed to read temperature" << std::endl;
    }
    else {
      fscanf(temperature_file, "%lf", &T);
      T /= 1000;
      printf("The temperature is %6.3f C.\n", T);
      fclose(temperature_file);

      // Holding the button for 1 second will force the fan to be on
      if (!fan_override) {
        // Run the fan to cool >50C to < 45C
        if (!fan_is_on && T >= 50) {
          fan_is_on = true;
          fan_power(fan_is_on);
        }
        else if (fan_is_on && T < 45) {
          fan_is_on = false;
          fan_power(fan_is_on);
        }
      }
    }

    // Sleep for a while
    sleep(60);
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

  if (time_current - time_previous > DEBOUNCE_TIME) {
    // Immediately cycle the brightness of the LED,
    //  unless the fan is stuck on
    if (!fan_override) {
      led_cycle_brightness();
    }

    // Check every 100 ms until it is no longer held
    unsigned int hold_intervals = 0;
    bool button_held = true;
    while (button_held) {
      usleep(DEBOUNCE_TIME * 1000);
      if (digitalRead(GPIO_BTN)) {
        ++hold_intervals;

        // At 3 seconds, pulse the LED, then exit
        if (hold_intervals * DEBOUNCE_TIME >= 3000) {
          led_pulse();
          button_held = false;
        }
        // At 2.4 seconds, turn off fan_override mode again
        else if (hold_intervals * DEBOUNCE_TIME == 2400) {
          fan_override = !fan_override;
          fan_power(fan_override);
        }
        // At 1 second, switch between fan_override mode
        else if (hold_intervals * DEBOUNCE_TIME == 1000) {
          led_off();
          fan_override = !fan_override;
          fan_power(fan_override);
        }
      }
      else {
        button_held = false;
      }
    }
    std::cout << "Button was held for " << hold_intervals * DEBOUNCE_TIME << " ms" << std::endl;
  }

  // Update the old time and return
  time_previous = millis();
  return;
}
