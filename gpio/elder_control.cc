// Perform a variety of functions on the raspberry pi 3

#include "elder_control.h"

// Run forever, requires root privileges
int main() {
  // Use the GPIO numbers for setup
  wiringPiSetupGpio();
  
  // Set up all the pins
  pinMode(GPIO_LED, OUTPUT); // Eventually PWM
  pinMode(GPIO_BTN, INPUT);
  pinMode(GPIO_FAN, OUTPUT);

  // Turn off the LED to start
  led_off();

  // Watch for rising edge of the button
  wiringPiISR(GPIO_BTN, INT_EDGE_RISING, &button_action);
  
  // Monitor temperature to control the fan in a new thread
  std::thread thread_fan(fan_control);
  
  // Loop for a bit
  std::cout << "Waiting in the main thread" << std::endl;

  // Join all the threads to exit
  thread_fan.join();

  std::cout << "Finished, exiting from the main thread" << std::endl;
  return 0;
}

// Turn the LED off
int led_off() {
  digitalWrite(GPIO_LED, LOW);
}

// Change the LED brightness between 6 possibilities
// Off, 25%, 50%, 75%, 100%, and blink
int led_cycle_brightness() {
  static int brightness = 0;

  // Make sure we don't go above 5
  if (++brightness > 5) {
    brightness = 0;
  }
  std::cout << "LED brightness cycled to " << brightness << std::endl;

  // Actually just alternate between ON and OFF
  //digitalWrite(GPIO_LED, (brightness % 2);
}

// Pulse the LED a few times before powering down
int led_pulse()
{
  std::cout << "Pulsing the LED before powering off" << std::endl;
  return 0;
}

// Turn the fan on or off based on CPU/GPU temperature
void fan_control() {
  FILE *temperature_file;
  double T;
  bool fan_is_on = false;
  
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
      
      // Run the fan to cool >50C to < 45C
      if (!fan_is_on && T >= 50) {
        std::cout << "Too hot, turning fan on" << std::endl;
        digitalWrite(GPIO_LED, HIGH);
      }
      else if (fan_is_on && T < 45) {
        std::cout << "Cool enough, turning fan off" << std::endl;
        digitalWrite(GPIO_LED, LOW);
      }
    }

    // Sleep for a while
    sleep(10);
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
    // Immediately cycle the brightness of the LED
    led_cycle_brightness();

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
        // At 1 second, turn the LED off
        else if (hold_intervals * DEBOUNCE_TIME >= 1000) {
          led_off();
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
