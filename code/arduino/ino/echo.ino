// Simple test script to light up an LED when a character is entered

// Constants
const int PIN_LED = 6; // Red LED with PWM
const static int led_modes[6] = {0, 1, 8, 32, 100, 255};

// Variables
byte charIn;
int led_mode_num = -1;
int brightness = 0;
char buffer[128];

// called once on start up
void setup() {
  // A baud rate of 115200 (8-bit with No parity and 1 stop bit)
  Serial.begin(115200, SERIAL_8N1);
  //pinMode(PIN_LED, OUTPUT);         // the LED is an output
  analogWrite(PIN_LED, 0);
}

// Loops forever!
void loop() {
  if (Serial.available()) {
    // Read the character
    charIn = Serial.read();

    // Set the brightness of the LED 
    ++led_mode_num;
    if (led_mode_num > 5) {
      led_mode_num = 0;
    }
    brightness = led_modes[led_mode_num];
    analogWrite(PIN_LED, brightness);

    // Send the character back to the RPi
    sprintf(buffer, "[%c] #%d (%d)", charIn, led_mode_num, brightness);
    Serial.println(buffer);

    // Delay slightly
    delay(100);
  }
}
