#ifndef DISPLAY_h
#define DISPLAY_h

#include "Arduino.h"

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//  On an arduino UNO:       A4(SDA), A5(SCL)
#define SCREEN_WIDTH   128  // OLED display width, in pixels
#define SCREEN_HEIGHT  32   // OLED display height, in pixels
#define OLED_RESET     -1   // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3c ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
extern Adafruit_SSD1306 display;

void oledInit();
void waitSerial(uint16_t waittime);
void cleanScreenandTextsizeTWO();           // clean the screen
void printTheTempandHumd(float t1, int h1); // show the temperature and Humdity data on the screen
#endif
