#include "Arduino.h"
#include "HX711.h"


#ifndef MAIN_h
#define MAIN_h

//pinout
const int DT_PIN = 35;
const int WEIGHT_SCK_PIN = 32;

//Other pinout
//SDA -> 21
//SCL -> 22
//RFID1 -> 4，5
//RFID2 -> 26,27

//wifi ssid:35F
//      pwd:44444444  

//Module declaration
HX711 scale;


//variable declaration
//Weight scale
float calibration_factor = 105000;
float average_last = 0;
char d1;
float WeightKg = 0;
float WeightD = 0;
//RFD+ID reader
static uint8_t prev_numberOfTags = 0; 
static String prev_finalTags = "";

String finalTags1 = "";
uint8_t numberOfTags1 =0;
String finalTags2 = "";
uint8_t numberOfTags2 =0;
String finalTags = "";
uint8_t numberOfTags = 0;

String addedTags = "";
String removedTags = "";
//General used
int rset = 0;//for set up power
String USERID;//to filter userid out


#define url "/lab/hwinfo"
#define urlHB "/lab/hwHB"
unsigned long looptime, noWifiTime = 0;
bool firstNotConnected = false;

int counter = 0;
#endif
