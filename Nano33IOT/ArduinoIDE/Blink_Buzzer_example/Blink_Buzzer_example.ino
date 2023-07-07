/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.

  This example code is in the public domain.
 */

// Pin 13 has an LED connected on most Arduino boards.
// Pin 11 has the LED on Teensy 2.0
// Pin 6  has the LED on Teensy++ 2.0
// Pin 13 has the LED on Teensy 3.0
// give it a name:
int buzzer = A7;
int rled = 7;
int gled = 8;
int bled = 9;

// the setup routine runs once when you press reset:
void setup() {
  // initialize the digital pin as an output.
  pinMode(buzzer, OUTPUT);
  pinMode(rled, OUTPUT);
  pinMode(gled, OUTPUT);
  pinMode(bled, OUTPUT);
}

// the loop routine runs over and over again forever:
void loop() {
  digitalWrite(buzzer, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);               // wait for a second
  digitalWrite(buzzer, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);               // wait for a second
  digitalWrite(rled, HIGH);
  delay(1000); 
  digitalWrite(rled, LOW);
  digitalWrite(gled, HIGH);
  delay(1000); 
  digitalWrite(gled, LOW);
  digitalWrite(bled, HIGH);
  delay(1000); 
  digitalWrite(bled, LOW);
}
