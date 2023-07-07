#ifndef BUZZER_H
#define BUZZER_H

#include "defines.h"

#include <Arduino.h>


inline void buzzer_init()
{
    pinMode(BUZZER_PIN, OUTPUT);
}

inline void buzzer_on()
{
    digitalWrite(BUZZER_PIN, HIGH);
}

inline void buzzer_off()
{
    digitalWrite(BUZZER_PIN, LOW);
}


#endif