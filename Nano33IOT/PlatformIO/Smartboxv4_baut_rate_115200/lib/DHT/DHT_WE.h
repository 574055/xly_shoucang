#ifndef DHT_WE_h
#define DHT_WE_h

#include "Arduino.h"
#include <DHT.h>
#include <DHT_U.h>
#include <Adafruit_Sensor.h>


//dht setting
#define DHTTYPE DHT11
#define DHTPIN 11
extern DHT dht;

void initializeDHT();
void printTheTempandHumd(float t1, int h1);
void dhtReading(int h1,float t1);
float temperatureReading();
int humdityReading();

#endif