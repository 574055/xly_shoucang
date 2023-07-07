#ifndef SD_CARD_H
#define SD_CARD_H

#include "Arduino.h"

#include <SD.h>

#define chipSelect 4
//function prototypes
void SDInit();
void writeSD(String data);


//fucntion defines
//init SD card
void SDInit(){
  if (!SD.begin(chipSelect)) {  //如果从CS口与SD卡通信失败，串口输出信息Card failed, or not present
    Serial.println("SD Card failed, or not present");
    return;
  }
  Serial.println("SD card initialized.");  //与SD卡通信成功，串口输出信息card initialized.
}

// write data to SD card
void writeSD(String data){
  // START Save data on SD card 
  File dataFile = SD.open("DATALOG.txt", FILE_WRITE);  
 
  if (dataFile) {
    dataFile.println(data);
    dataFile.close();
    // 数组dataString输出到串口
    Serial.println("saved to SD card");
    Serial.println(data);
  }  
  // 如果无法打开文件，串口输出错误信息error opening datalog.txt
  else {
    Serial.println("error opening datalog.txt");
  } 
  // END Save data on SD card
}

// write offset to SD card
void writeSD_reset(String data){
  // START Save data on SD card 
  File dataFile = SD.open("RESET.txt", FILE_WRITE);  
 
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

// read offset from SD
String readSDreset(void){
  File myFile = SD.open("RESET.txt");
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

void writeSD_autoca(String data){
  // START Save data on SD card 
  File dataFile = SD.open("CALIB.txt", FILE_WRITE);  // "\"filename" +".txt\""
 
  if (dataFile) {
    dataFile.println(data);
    dataFile.close();
    // 数组dataString输出到串口
    Serial.println("saved to SD card");
    Serial.println(data);
  }  
  // 如果无法打开文件，串口输出错误信息error opening datalog.txt
  else {
    Serial.println("error opening califactor.txt");
  } 
  // END Save data on SD card
}

String readSD_aotuca(void){
  File myFile = SD.open("CALIB.txt");
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
    Serial.println("error opening califactor.txt");
  }
  
  return SdString;
}

void SD_Lowpower(){//不得删除
  File dataFile = SD.open("RUBBISH.txt", FILE_WRITE);  
 
  if (dataFile) {
    dataFile.close();
    Serial.println("saved to SD card");
  }  
  else {
    Serial.println("error opening reset.txt");
  } 
  SD.remove("RUBBISH.txt");
}

String readSD_rfidpwr(void){
  File myFile = SD.open("RFIDPWR.txt");
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
    Serial.println("error opening rfidpwr.txt");
  }
  
  return SdString;
}

void writeSD_rdidpwr(String data){
  // START Save data on SD card 
  File dataFile = SD.open("RFIDPWR.txt", FILE_WRITE);  // "\"filename" +".txt\""
 
  if (dataFile) {
    dataFile.println(data);
    dataFile.close();
    // 数组dataString输出到串口
    Serial.println("saved to SD card");
    Serial.println(data);
  }  
  // 如果无法打开文件，串口输出错误信息error opening datalog.txt
  else {
    Serial.println("error opening rfidpwer.txt");
  } 
  // END Save data on SD card
}

String readSD_wifissid(void){
  File myFile = SD.open("WIFISSID.txt");
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
    Serial.println("error opening wifissid.txt");
  }
  
  return SdString;
}

String readSD_wifipwd(void){
  File myFile = SD.open("WIFIPWD.txt");
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
    Serial.println("error opening wifipwd.txt");
  }
  
  return SdString;
}

void writeSD_wifissid(String data){
  // START Save data on SD card 
  File dataFile = SD.open("WIFISSID.txt", FILE_WRITE);  // "\"filename" +".txt\""
 
  if (dataFile) {
    dataFile.println(data);
    dataFile.close();
    // 数组dataString输出到串口
    Serial.println("saved to SD card");
    Serial.println(data);
  }  
  // 如果无法打开文件，串口输出错误信息error opening datalog.txt
  else {
    Serial.println("error opening wifissid.txt");
  } 
  // END Save data on SD card
}

void writeSD_wifipwd(String data){
  // START Save data on SD card 
  File dataFile = SD.open("WIFIPWD.txt", FILE_WRITE);  // "\"filename" +".txt\""
 
  if (dataFile) {
    dataFile.println(data);
    dataFile.close();
    // 数组dataString输出到串口
    Serial.println("saved to SD card");
    Serial.println(data);
  }  
  // 如果无法打开文件，串口输出错误信息error opening datalog.txt
  else {
    Serial.println("error opening wifipwd.txt");
  } 
  // END Save data on SD card
}
#endif
