// Controlling led by potentiometer

/******************************************/
// Pin settings
const int PIN_ANALOG_A1  = A1; // Potentiometer to analog input
const int PIN_LED_RED    = 6;  // LED on a PWM output

// Store input/output variables
int input_value = 0;
int output_value = 0;
/******************************************/

/******************************************/
void setup() {
}
/******************************************/

/******************************************/
void loop() {
  // Read the value from the sensor
  input_value = analogRead(PIN_ANALOG_A1);

  // Convert from 0-1023 proportional to the number of a number of from 0 to 255
  output_value = map(input_value, 0, 1023, 0, 255);

  // Turn the led on depend on the output value
  analogWrite(PIN_LED_RED, output_value);
}
/*******************************************/

