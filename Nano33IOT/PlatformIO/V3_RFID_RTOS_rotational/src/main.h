#ifndef MAIN_h
#define MAIN_h

#include <Arduino.h>

#include "RTOS_helper.h"
#include <FreeRTOS_SAMD21.h>


#include "wifi.h"

#include "UST_RFID.h"
#include "motor_control.h"
#include "oled.h"
#include "SD_CARD.h"

// #define DEBUG //uncoment to enable debugg messages
#ifdef DEBUG
#define debug(x) Serial.print(x)
#define debugln(x) Serial.println(x)
#else
#define debug(x)
#define debugln(x)
#endif

// Pinouts
#define REDLED 8
#define BUZZER A7

//GLOBAL VARIABLES
volatile float measured_weight = 0, estimated_weight = 0;
volatile bool readFlag1 = false, readFlag2 = false;
String finalTags1 = "";
String data = "";


void taskMonitor(void *pvParameters);
void Interrupt_MyHandler_IRQ(); //for the limit switch

#endif