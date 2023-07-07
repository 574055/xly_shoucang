#ifndef OLED_h
#define OLED_h

#include "main.h"
#include <SPI.h>  //for OLED
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// On an arduino UNO:       A4(SDA), A5(SCL)
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3c ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void oledInit(){
  Wire.begin();
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.clearDisplay();
  display.setTextSize(2);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner
  display.println("Wesahre");
  display.println("...loading");
  display.display();
}

void oledDisplayData(float estimated_weight, uint8_t numberOfTags){
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(0, 0);
    display.println((String) estimated_weight + " g");
    display.println((String) numberOfTags + " Tags");
    display.display();
}

#endif