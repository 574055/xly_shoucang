#ifndef SDLOADER_H
#define SDLOADER_H

#include "wifi_low_level.h"

#include <Arduino.h>


bool sdLoadOptions();
void sdSaveOptions();

float sdLoadFloat(const String& path, float default_value);
void sdSaveFloat(const String& path, float value);

float sdLoadScale();
void sdSaveCalibration(float value);

float sdLoadOffset();
void sdSaveOffset(float value);

bool sdLoadWifi(WiFiLowLevel&);
void sdSaveWifi(const WiFiLowLevel&);

float sdLoadSleepThresh();
void sdSaveSleepThresh(float value);


#endif
