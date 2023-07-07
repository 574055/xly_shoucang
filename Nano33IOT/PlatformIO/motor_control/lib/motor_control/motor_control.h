#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

#include "Arduino.h"

void directionControl(bool cw);
// void stopMotor(u_int16_t duration);


const int enA = 9;
const int in1 = 8;
const int in2 = 7;


void motorInit() {
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  directionControl(1);
}


void directionControl(bool cw) {
  // Set motors to maximum speed
  // For PWM maximum possible values are 0 to 255
  analogWrite(enA, 255);

  if (cw){
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
  }
  else{
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
  }
}

void stopMotor(u_int16_t duration) {
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  delay(duration);
}

#endif
