#include "DISPLAY.h"

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void oledInit()
{
  Wire.begin();
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
  {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // Don't proceed, loop forever
  }
  display.setRotation(2);
  display.clearDisplay();
  display.setTextSize(2);              // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 0);             // Start at top-left corner
  display.println("Weshare");
  display.println("...loading");
  display.display();
}

//clean the screen
void cleanScreenandTextsizeTWO(){
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(0, 0);
}


void waitSerial(uint16_t waittime)
{
  cleanScreenandTextsizeTWO();
  display.print((String) "Wait " + waittime / 1000 + " s for Serial...");
  display.display();

  // waits for Serial Port or the waittime has elapsed
  long prev_time = millis();
  long tim = 0;

  while (!Serial)
  {
    tim = millis() - prev_time;
    if (tim > waittime)
      break;
  };
}