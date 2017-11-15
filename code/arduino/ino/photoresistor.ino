// Control some LEDs with a photoresistor

// Calculated note frequencies
#include "ino/pitches.h"

// Constants
const int num_leds = 3; // 3 LEDS, R/G/B
const int PIN_LEDS[] = {6, 12, 13};
const int PIN_PHOTORESISTOR = A0;
const int PIN_SPEAKER = 5;
const int PIN_MOTOR = 9;

// Pull out variables for speed
byte char_in;
char buffer[128];
int sensor_value = 0;
int led_level    = 0;
bool melody_playing = false;

// notes in the melody:
int melody[] = {
  NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {
  4, 8, 8, 4, 4, 4, 4, 4
};

// Functions
void play_melody();
void serial_port();

void setup() {
  // Enable the serial port
  // A baud rate of 9600 (8-bit with No parity and 1 stop bit)
  Serial.begin(9600, SERIAL_8N1);

  // All LEDs are outputs
  for (int led_number = 0; led_number < num_leds; ++led_number) {
    pinMode(PIN_LEDS[led_number], OUTPUT);
  }

  // Start playing the melody
  melody_playing = true;
}

void loop() {
  // Read the sensor value
  sensor_value = analogRead(PIN_PHOTORESISTOR);

  // Light between 0 and 3 LEDs
  led_level = 0;
  if (sensor_value > 500) {
    led_level = 3;
  }
  else if (sensor_value > 300) {
    led_level = 2;
  }
  else if (sensor_value > 150) {
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

  // If all 3 are lit, buzz a buzzer
  // Generates a square wave of the frequency of 10000 Hz (and 50% duty cycle) on a pin 5 
  if (led_level >= 2) {
    pinMode(PIN_SPEAKER, OUTPUT);
    tone(PIN_SPEAKER, 10000);
    pinMode(PIN_SPEAKER, INPUT);
  }

  // Check the serial port
  serial_port();
}

void play_melody() {
  // iterate over the notes of the melody:
  for (int thisNote = 0; thisNote < 8; thisNote++) {

    // to calculate the note duration, take one second divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(PIN_SPEAKER, melody[thisNote], noteDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(PIN_SPEAKER);
  }
}

// Read from the serial port for debugging
void serial_port() {
  if (Serial.available()) {
    // Read the character
    char_in = Serial.read();

    // Send the character back to the RPi,
    //  along with the brightness
    sprintf(buffer, "[%c] #%d (%d)", char_in, led_level, sensor_value);
    Serial.println(buffer);
  }
}
