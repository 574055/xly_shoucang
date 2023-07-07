#ifndef SD_CARD_H
#define SD_CARD_H

#include "main.h"

#include <SPI.h>
#include <SD.h>
//function prototypes
void SDInit();
void writeSD(String data);

#define chipSelect 4

//fucntion defines
void SDInit(){
  if (!SD.begin(chipSelect)) {  //如果从CS口与SD卡通信失败，串口输出信息Card failed, or not present
    Serial.println("Card failed, or not present");
    return;
  }
  // Serial.println("card initialized.");  //与SD卡通信成功，串口输出信息card initialized.
}

void writeSD(String data){
  // START Save data on SD card 
  File dataFile = SD.open("datalog.txt", FILE_WRITE);  
 
  if (dataFile) {
    dataFile.println(data);
    dataFile.close();
    // 数组dataString输出到串口
    // Serial.println("saved to SD card");
    // Serial.println(data);
  }  
  // 如果无法打开文件，串口输出错误信息error opening datalog.txt
  else {
    Serial.println("error opening datalog.txt");
  } 
  // END Save data on SD card
}

void writeSD_reset(String data){
  // START Save data on SD card 
  File dataFile = SD.open("reset.txt", FILE_WRITE);  
 
  if (dataFile) {
    dataFile.println(data);
    dataFile.close();
    // 数组dataString输出到串口
    Serial.println("saved to SD card");
    Serial.println(data);
  }  
  // 如果无法打开文件，串口输出错误信息error opening datalog.txt
  else {
    Serial.println("error opening reset.txt");
  } 
  // END Save data on SD card
}

String readWifiSSID(void){
  File myFile = SD.open("wifi.txt");
  String SdString = "";
  if (myFile) {
    // Serial.println("wifi.txt:");

    while (myFile.available())
    {
      SdString += (char)myFile.read();
    }
    // Serial.println(SdString);

    myFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }
  
  int ind1 = SdString.indexOf(',');
  // Serial.println(SdString.substring(0,ind1));
  // Serial.println(SdString.substring(ind1+1));
  return SdString.substring(0,ind1);
}

String readWifiPASS(void){
  File myFile = SD.open("wifi.txt");
  String SdString = "";
  if (myFile) {
    // Serial.println("wifi.txt:");

    while (myFile.available())
    {
      SdString += (char)myFile.read();
    }
    // Serial.println(SdString);

    myFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }
  
  int ind1 = SdString.indexOf(',');
  // Serial.println(SdString.substring(0,ind1));
  // Serial.println(SdString.substring(ind1+1));
  return SdString.substring(ind1+1);
}

void writeSD_scale(String data){
  // START Save data on SD card 
  File dataFile = SD.open("scalefactor.txt", FILE_WRITE);  
 
  if (dataFile) {
    dataFile.println(data);
    dataFile.close();
    // 数组dataString输出到串口
    Serial.println("saved to SD card");
    Serial.println(data);
  }  
  // 如果无法打开文件，串口输出错误信息error opening datalog.txt
  else {
    Serial.println("error opening scalefactor.txt");
  } 
  // END Save data on SD card
}

String readSDreset(void){
  File myFile = SD.open("reset.txt");
  String SdString = "";
  if (myFile) {
    // Serial.println("wifi.txt:");

    while (myFile.available())
    {
      SdString += (char)myFile.read();
    }
    // Serial.println(SdString);

    myFile.close();
    // SD.remove("reset.txt");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening reset.txt");
  }
  
  return SdString;
}

String readSDscale(void){
  File myFile = SD.open("scalefactor.txt");
  String SdString = "";
  if (myFile) {
    
    while (myFile.available())
    {
      SdString += (char)myFile.read();
    }
    // Serial.println(SdString);

    myFile.close();
    
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening scalefactor.txt");
  }
  
  return SdString;
}

#endif
