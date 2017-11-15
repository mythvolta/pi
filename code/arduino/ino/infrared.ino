// Read from an IR remote

#include <IRremote.h>    // For infrared receivers

// Constants
const int num_leds = 3; // 3 LEDS, R/G/B
const int PIN_LEDS[] = {6, 12, 13};
const int PIN_PHOTORESISTOR = A0;
const int PIN_SPEAKER = 5;
const int PIN_IR = 2;

// Pull out variables for speed
int sensor_value = 0;
int led_level    = 0;
bool melody_playing = false;
char buffer[128];
byte char_in;

// Functions
void ir_read();
void serial_respond();

// Set up the IR receiver
IRrecv irrecv(PIN_IR);
decode_results results;

void setup() {
  // Start the serial port
  Serial.begin(9600);
  
  // Start the receiver
  Serial.println("Enabling IRin");
  irrecv.enableIRIn();
  Serial.println("Arduino is ready");

  // All LEDs are outputs
  for (int led_number = 0; led_number < num_leds; ++led_number) {
    pinMode(PIN_LEDS[led_number], OUTPUT);
  }

  // Start playing the melody
  melody_playing = false;
}

void loop() {
  // Read the sensor value
  sensor_value = analogRead(PIN_PHOTORESISTOR);

  // Light between 0 and 3 LEDs
  led_level = 0;
  if (sensor_value > 800) {
    led_level = 3;
  }
  else if (sensor_value > 500) {
    led_level = 2;
  }
  else if (sensor_value > 200) {
    led_level = 1;
  }

  for (int led_number = 0; led_number < num_leds; ++led_number) {
    // When led_number is smaller than led_level, run the following code.
    if (led_level > led_number) {
      digitalWrite(PIN_LEDS[led_number], HIGH);     // turn on pins less than the level
    }
    else {
      digitalWrite(PIN_LEDS[led_number], LOW);      // turn off pins higher than 
    }
  }

  // Check for IR messages
  ir_read();

  // Respond to serial messages
  serial_respond();
}

// Read from the IR remote
void ir_read() {
  // Only do things if we receive a pulse
  if (irrecv.decode(&results)) {
    // Send the character back to the RPi
    sprintf(buffer, "hex value is [%lu] : #%d (%d)", results.value, led_level, sensor_value);
    Serial.println(buffer);

    // Receive the next value
    irrecv.resume();
  }
  delay(100);
}

// Respond to characters from the serial port
void serial_respond() {
  if (Serial.available()) {
    // Read the character
    char_in = Serial.read();

    // Send the character back to the RPi, along with the brightness
    sprintf(buffer, "[%c] #%d (%d)", char_in, led_level, sensor_value);
    Serial.println(buffer);
  }
}
