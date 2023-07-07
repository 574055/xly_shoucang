#include <Arduino.h>
#include <ArduinoBLE.h>
#include "UST_RFID.h"
#include <FlashStorage_SAMD.h>
#include <SPI.h>
#include <Wire.h>
#include <WiFiNINA.h>
#include "UST_RFID.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "wiring_private.h"


#define RFID1_EN 3

BLEService RFIDService("19b10000-e8f2-537e-4f6c-d104768a1214");
BLEStringCharacteristic TagsDataCharacteristic("19b17846-e8f2-537e-4f6c-d104768a1214", BLERead | BLEWrite | BLENotify,200);
BLEIntCharacteristic NumbersOfTagsCharacteristic("19b17847-e8f2-537e-4f6c-d104768a1214", BLERead | BLEWrite | BLENotify);
static uint8_t prev_numberOfTags = 0; 
static String prev_finalTags = "";
String finalTags1 = "";
uint8_t numberOfTags1 = 0;
uint8_t numberOfTags =0;

String finalTags233[7]={"11111111","22222222","33333333","1111111122222222","2222222233333333","1111111133333333","111111112222222233333333"};
int Test = 0;
int Testtags = 0;

void setup() {

  Serial.begin(115200);//Open serial
  Serial1.begin(115200);


  pinMode (RFID1_EN, OUTPUT); 
  digitalWrite(RFID1_EN,HIGH);
  rfidInit();

  while (!BLE.begin()) {
    Serial.println("starting Bluetooth® Low Energy module failed!");
  }//Open BLE

  BLE.setLocalName("RFID_Scanner_3");
  BLE.setAdvertisedService(RFIDService);
  RFIDService.addCharacteristic(TagsDataCharacteristic);
  RFIDService.addCharacteristic(NumbersOfTagsCharacteristic);
  BLE.addService(RFIDService);
  TagsDataCharacteristic.writeValue("..");
  NumbersOfTagsCharacteristic.writeValue(66);
  BLE.advertise();
  Serial.println("Finished setup!");
}

void loop() {
  delay(500);

  // if(Test<3)//！！！调试使用
  // {
  //   Testtags=1;
  // }
  // else if(Test==6)
  // {
  //   Testtags=3;
  // }
  // else if(Test==7)
  // {Test=0;Testtags=1;}
  // else{Testtags==2;}


  BLEDevice central = BLE.central();

  // if (central)
  // { 
    BLE.stopAdvertise();
    numberOfTags1 = 0;

      String xxxx=TagsDataCharacteristic.value();

      // if(xxxx=="a")
      // {
        finalTags1 = multiscan_v2(&numberOfTags1, TIMEOUT);
        numberOfTags = finalTags1.length() / 8;
        Serial.println((String)"Found:" + finalTags1);
        Serial.println((String)"Number" + numberOfTags);


        String addedTags = tagsChanged(prev_finalTags, prev_numberOfTags, finalTags1, numberOfTags);
        String removedTags = tagsChanged(finalTags1, numberOfTags, prev_finalTags, prev_numberOfTags);
        Serial.println((String)"d" + addedTags); //send numberOfTags to GUI
        Serial.println((String)"r" + removedTags); //send finalTags to GUI 


        //TagsDataCharacteristic.writeValue("AABB");
        delay(50);

        for(int i=0;i<numberOfTags1;i++)//！！！正常使用
        // for(int i=0;i<Testtags;i++)//！！！调试使用
        {
          if(numberOfTags1!=0)
          {
          String data="";
          // String Tags=finalTags233[Test].substring(i*8,(i+1)*8);//！！！调试使用
          // data+=Tags;//！！！调试使用
          String Tags=finalTags1.substring(i*8,(i+1)*8);//正常使用
          data+=Tags;//正常使用

          TagsDataCharacteristic.writeValue(data);
          delay(50);
          }
        }
        TagsDataCharacteristic.writeValue("    ");
      //}


  // }
  // Serial.println("End!");


  if(!central.connected())
  {
    BLE.advertise();
  }
  
  prev_finalTags = finalTags1;
  prev_numberOfTags = numberOfTags;

  Test++;
}