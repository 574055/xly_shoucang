#ifndef DISPLAY_H
#define DISPLAY_H

#include <cstdint>


enum Page {
    PAGE_TEMP_HUM,
    PAGE_WEIGHT_RFID,
    NUM_PAGES,
};


void displayInit();

void displaySetWeight(float weight);
void displaySetNumTags(uint32_t tags);

void displayWakeUp();
void displaySleep();

void displayUpdate();

void displayf(uint8_t text_size, const char* fmt, ...) __attribute__((format(printf, 2, 3)));


#endif
