#include <Arduino.h>

#include "myconfig.h"

#include <SPI.h>
#include <Wire.h>
#include "UST_RFID.h"

SoftwareSerial SerialRFID;  // RX, TX//9600
#define MYPORT_RX 12
#define MYPORT_TX 14



void waitSerial(uint16_t waittime);
void displaySavedData();
void eepromInit();
void oledInit();
void addtotags();

const int WRITTEN_SIGNATURE = 0xBEEFDEED;

typedef struct
{
  char SSID[30];
  char PWD[30];
  char SCALEFACTOR[10];
} EEPROM_data;

int signature;
uint16_t storedAddress = 0;
EEPROM_data e_data;

String data;
char d1;
String x;
float scalefactor;

// basic setting of hardware

// Timer
unsigned long myTime, scanTimeOut = 0;



// #define timeout 900
// uint8_t totaltags[1000];

void setup()
{
  randomSeed(analogRead(0));

  //Serial1.begin(115200); // for RFID
  

  Serial.begin(115200);
  SerialRFID.begin(115200, SWSERIAL_8N1, MYPORT_RX, MYPORT_TX, false);
  // Serial.setTimeout(500);


  waitSerial(wait_for_Serial);
  debugln("Serial port ready");


  rfidInit();
  debugln("RFID ready");

  pinMode(RFID1_EN,OUTPUT);
  pinMode(RFID2_EN,OUTPUT);
  pinMode(RFID3_EN,OUTPUT);
  digitalWrite(RFID1_EN,LOW);
  digitalWrite(RFID2_EN,LOW);
  digitalWrite(RFID3_EN,LOW);
}

void loop()
{
  debugln();
  debugln();
  debugln("beginn of main loop");
  debugln();

  myTime = millis();

  // RFID
  // Scan RFID
  // enabel pin2 high

  uint8_t numberOfTags1 = 0;
  static uint8_t prev_numberOfTags1 = 0;
  digitalWrite(RFID1_EN,HIGH);
  String finalTags1 = multiscan_v2(&numberOfTags1, TIMEOUT);
  Serial.println((String)"n1:" + numberOfTags1); //send numberOfTags to GUI
  //Serial.println((String)"f1:" + finalTags1); //send finalTags to GUI
  static String prev_finalTags1 = "";
  //Serial.println((String)"finalTags: " + finalTags); 
  // Serial.println((String)"prev_finalTags: " + prev_finalTags);
  //Serial.println((String)"numberOfTags: " + numberOfTags); 
  // Serial.println((String)"prev_numberOfTags: " + prev_numberOfTags);
  String addedTags1 = tagsChanged(prev_finalTags1, prev_numberOfTags1, finalTags1, numberOfTags1);
  String removedTags1 = tagsChanged(finalTags1, numberOfTags1, prev_finalTags1, prev_numberOfTags1);
  //Serial.println((String)"d1:" + addedTags1); //send numberOfTags to GUI
  //Serial.println((String)"r1:" + removedTags1); //send finalTags to GUI 
  prev_finalTags1 = finalTags1;
  prev_numberOfTags1 = numberOfTags1;

  uint8_t numberOfTags2 = 0;
  static uint8_t prev_numberOfTags2 = 0;
  digitalWrite(RFID1_EN,LOW);
  digitalWrite(RFID2_EN,HIGH);
  String finalTags2 = multiscan_v2(&numberOfTags2, TIMEOUT);
  Serial.println((String)"n2:" + numberOfTags2); //send numberOfTags to GUI
  //Serial.println((String)"f2:" + finalTags2); //send finalTags to GUI
  static String prev_finalTags2 = "";
  //Serial.println((String)"finalTags: " + finalTags); 
  // Serial.println((String)"prev_finalTags: " + prev_finalTags);
  //Serial.println((String)"numberOfTags: " + numberOfTags); 
  // Serial.println((String)"prev_numberOfTags: " + prev_numberOfTags);
  String addedTags2 = tagsChanged(prev_finalTags2, prev_numberOfTags2, finalTags2, numberOfTags2);
  String removedTags2 = tagsChanged(finalTags2, numberOfTags1, prev_finalTags2, prev_numberOfTags2);
  //Serial.println((String)"d2:" + addedTags2); //send numberOfTags to GUI
  //Serial.println((String)"r2:" + removedTags2); //send finalTags to GUI 

  uint8_t numberOfTags3 = 0;
  static uint8_t prev_numberOfTags3 = 0;
  digitalWrite(RFID2_EN,LOW);
  digitalWrite(RFID3_EN,HIGH);
  String finalTags3 = multiscan_v2(&numberOfTags3, TIMEOUT);
  Serial.println((String)"n3:" + numberOfTags3); //send numberOfTags to GUI
  //Serial.println((String)"f2:" + finalTags2); //send finalTags to GUI
  static String prev_finalTags3 = "";
  //Serial.println((String)"finalTags: " + finalTags); 
  // Serial.println((String)"prev_finalTags: " + prev_finalTags);
  //Serial.println((String)"numberOfTags: " + numberOfTags); 
  // Serial.println((String)"prev_numberOfTags: " + prev_numberOfTags);
  String addedTags3 = tagsChanged(prev_finalTags3, prev_numberOfTags3, finalTags3, numberOfTags3);
  String removedTags3 = tagsChanged(finalTags3, numberOfTags3, prev_finalTags3, prev_numberOfTags3);
  //Serial.println((String)"d2:" + addedTags2); //send numberOfTags to GUI
  //Serial.println((String)"r2:" + removedTags2); //send finalTags to GUI 

  String combined = keepIndividuals(finalTags1, numberOfTags1, finalTags2, numberOfTags2);
  uint8_t numberOfTags0 = combined.length() / 8;
  String combined23 = keepIndividuals(combined, numberOfTags0, finalTags3, numberOfTags3);
  uint8_t numberOfTags88 = combined23.length() / 8;
  static uint8_t prev_numberOfTags = 0;
  debugln("TOTAL:");
  //Serial.println((String) "w" + average);       // send weight to GUI
  Serial.println((String) "n*2:" + numberOfTags0); // send numberOfTags to GUI
  Serial.println((String) "f*2:" + combined);    // send finalTags to GUI
  Serial.println((String) "n*3:" + numberOfTags88); // send numberOfTags to GUI
  Serial.println((String) "f*3:" + combined23);    // send finalTags to GUI


  delay(1000);
}

// private functions ******************************************************************

void waitSerial(uint16_t waittime)
{


  // waits for Serial Port or the wiattime has elapsed
  long prev_time = millis();
  long tim = 0;
  while (!Serial)
  {
    tim = millis() - prev_time;
    if (tim > waittime)
      break;
  };
}

void rfidInit()
{
  const byte setPower[] = {0xBB, 0x00, 0xB6, 0x00, 0x02, 0x0A, 0x28, 0xEA, 0x7E}; // 26dBm
  // const byte setPower[] = {0xBB, 0x00, 0xB6, 0x00, 0x02, 0x07, 0xD0, 0x8F, 0x7E}; //20dBm
  // const byte setPower[] = {0xBB, 0x00, 0xB6, 0x00, 0x02, 0x07, 0x3A, 0xF9, 0x7E}; //18.5dBm
  //  const byte setPower[] = {0xBB, 0x00, 0xB6, 0x00, 0x02, 0x06, 0x0E, 0xCC, 0x7E}; //15.5dBm
  SerialRFID.write(setPower, sizeof(setPower));
  // Serial.println("set power to 26 dBm");
}


String multiscan_v2(uint8_t* numberOfTags, unsigned long timeout)
{
  // const byte readMulti[] = {0xBB, 0x00, 0x22, 0x00, 0x00, 0x22, 0x7E}; //x1
  // const byte readMulti[] = {0xBB, 0x00, 0x27, 0x00, 0x03, 0x22, 0x00, 0x03, 0x4F, 0x7E}; // x3
   const byte readMulti[] = {0xBB, 0x00, 0x27, 0x00, 0x03, 0x22, 0x00, 0x05, 0x51, 0x7E}; // x5
  // const byte readMulti[] = {0xBB, 0x00, 0x27, 0x00, 0x03, 0x22, 0x00, 0x07, 0x53, 0x7E}; // x7
  // const byte readMulti[] = {0xBB, 0x00, 0x27, 0x00, 0x03, 0x22, 0x00, 0x0A, 0x56, 0x7E}; // x10
  // const byte readMulti[] = {0xBB, 0x00, 0x27, 0x00, 0x03, 0x22, 0x00, 0x14, 0x60, 0x7E}; // x20
  // const byte readMulti[] = {0xBB, 0x00, 0x27, 0x00, 0x03, 0x22, 0x00, 0x1E, 0x6A, 0x7E}; // x30
  bool finished_scan = false;
  uint8_t tags = 0;
  uint16_t i = 0;
  unsigned long currentMillis = 0; // millis() returns an unsigned long.
  SerialRFID.write(readMulti, sizeof(readMulti));
  currentMillis = millis(); // reset timer;
  uint8_t result[100] ={0};
  String finalTags = "";
  
  while (!finished_scan)
  {
    while (SerialRFID.available())
    {

      result[i] = SerialRFID.read();
      // Serial.print(result[i], HEX);
      // Serial.print(",");

      if (result[i] == 0x7e)
      {
        // Serial.println((String)"i: " + i);
        //if (i < 8)
        if ((i == 23) == (i == 19))
        { // if not enough characters, skipp this tag
          i = 0;
          // Serial.println("Skipped tag");
          continue;
        }
        tags++;
        i = 0;

        // Serial.println("do some data processing");

        
        String tagID = getTagfromHEX(result, 1);
        // Serial.println((String) "tagID: " + tagID);

        *numberOfTags = finalTags.length() / 8;
        // Serial.println((String) "numberOfTags: " + *numberOfTags);
        finalTags = keepIndividuals(finalTags, *numberOfTags, tagID, 1);
        // Serial.println((String) "finalTags: " + finalTags);

        continue;
      }
      i++;
      
      currentMillis = millis(); // reset timer;
    }

    if (millis() - currentMillis > timeout)
    { // if timeout is reached then finish reading
      // Serial.println((String) "numberOfTags: " + numberOfTags);
      // Serial.println((String) "finalTags: " + finalTags);

      // send to GUI
      // Serial.println((String) "n" + *numberOfTags); // send numberOfTags to GUI
      // Serial.println((String) "f" + finalTags);    // send finalTags to GUI
      
      //Serial.println.println("finished Read");

      finished_scan = true;
    }
  }
  return finalTags;
}

uint8_t multiscan(uint8_t result[], unsigned long timeout)
{
  // returns the number of tags
  // resutl ... the resulting scan as result array in HEX
  // timeout ... max time to wait after each Serial read
  // const byte readMulti[] = {0xBB, 0x00, 0x27, 0x00, 0x03, 0x22, 0x00, 0x0A, 0x56, 0x7E}; //x10
  // const byte readMulti[] = {0xBB, 0x00, 0x27, 0x00, 0x03, 0x22, 0x00, 0x0F, 0x5B, 0x7E}; //x15
  const byte readMulti[] = {0xBB, 0x00, 0x27, 0x00, 0x03, 0x22, 0x00, 0x14, 0x60, 0x7E}; // x20
  bool finished_scan = false;
  uint8_t tags = 0;
  uint16_t i = 0;
  unsigned long currentMillis = 0; // millis() returns an unsigned long.
  SerialRFID.write(readMulti, sizeof(readMulti));
  currentMillis = millis(); // reset timer;

  while (!finished_scan)
  {
    while (SerialRFID.available())
    {
      result[i] = SerialRFID.read();
      // Serial.print(result[i],HEX);
      // Serial.print(",");

      if (result[i] == 0x7e)
      {
        // Serial.print(" ");
        tags++;
      }
      i++;
      currentMillis = millis(); // reset timer;
    }

    if (millis() - currentMillis > timeout)
    { // if timeout is reached then finish reading

      finished_scan = true;
      return tags;
    }
  }
  return 0;
}

