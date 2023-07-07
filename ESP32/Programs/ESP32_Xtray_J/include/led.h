#ifndef LED_H
#define LED_H

#include "defines.h"

#include <Arduino.h>
#include <cstdint>


struct LED
{
    uint8_t pin;
    uint8_t channel;
    static uint8_t counter;

    LED(uint8_t pin) : pin{pin}, channel{counter++} {}

    void init()
    {
        ledcSetup(channel, LEDC_BASE_FREQ, LEDC_TIMER_BIT_PRECISION);
        ledcAttachPin(pin, channel);
    }

    void write(uint8_t value)
    {
        // calculate duty, 8191 from 2 ^ 13 - 1
        uint32_t duty = min(value, uint8_t(255)) * 8191 / 255;

        // write duty to LEDC
        ledcWrite(channel, duty);
    }
};

struct RGBLED
{
    LED* led[3];
    RGBLED(LED* rled, LED* gled, LED* bled) : led{rled, gled, bled} {}

    void init()
    {
        led[0]->init();
        led[1]->init();
        led[2]->init();
    }

    void setRGB(uint8_t r, uint8_t g, uint8_t b)
    {
        led[0]->write(r);
        led[1]->write(g);
        led[2]->write(b);
    }

    void setRGB(uint32_t rgb)
    {
        setRGB((rgb >> 16) & 0xFF, (rgb >> 8) & 0xFF, rgb & 0xFF);
    }
};


#endif