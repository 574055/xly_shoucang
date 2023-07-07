#ifndef MAIN_H
#define MAIN_H

#include "defines.h"
#include "led.h"
#include "rfid.h"

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <tuning.h>


// ----- Global Variables -----

extern Adafruit_SSD1306 oled;

extern RFIDAggregator rfid;

using XTrayTuneSet = TuneSet<>;
extern XTrayTuneSet tuneset;


float getSleepLightThreshold();
void updateSleepLightThreshold(float val);

#endif