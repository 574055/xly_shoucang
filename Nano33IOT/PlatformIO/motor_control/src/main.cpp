#include <Arduino.h>
#include "motor_control.h"

const int buttonPin = 2;    // the number of the pushbutton pin
volatile byte state = LOW;
byte lastState = LOW;
bool rotDir = HIGH;

void blink();


void setup() {
  pinMode(buttonPin, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  motorInit();
  attachInterrupt(digitalPinToInterrupt(buttonPin), blink, RISING);
}

void loop() {

  if (state != lastState) { 
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    stopMotor(3000);
    rotDir = !rotDir; //flip direction
    directionControl(rotDir);
    lastState = state;
    delay(1000);
  }

}

void blink() {
  state = !state;
}
