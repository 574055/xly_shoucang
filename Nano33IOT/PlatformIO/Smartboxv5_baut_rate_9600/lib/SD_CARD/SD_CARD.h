#ifndef SD_CARD_H
#define SD_CARD_H

#include "Arduino.h"
#include "myconfig.h" //storing the pinout and basic setting

#include <SD.h>
// function prototypes
void SDInit();
void writeSD(String data);


// fucntion defines
// init SD card
void SDInit()
{
    if (!SD.begin(chipSelect)) { //如果从CS口与SD卡通信失败，串口输出信息Card failed, or not present
        Serial.println("Card failed, or not present");
        return;
    }
    Serial.println("card initialized."); //与SD卡通信成功，串口输出信息card initialized.
}

// write data to SD card
void writeSD(String data)
{
    // START Save data on SD card
    File dataFile = SD.open("datalog.txt", FILE_WRITE);

    if (dataFile) {
        dataFile.println(data);
        dataFile.close();
        // 数组dataString输出到串口
        Serial.println("saved to SD card");
        Serial.println(data);
    }
    // 如果无法打开文件，串口输出错误信息error opening datalog.txt
    else
    {
        Serial.println("error opening datalog.txt");
    }
    // END Save data on SD card
}

// write offset to SD card
void writeSD_reset(String data)
{
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
    else
    {
        Serial.println("error opening reset.txt");
    }
    // END Save data on SD card
}

// read offset from SD
String readSDreset(void)
{
    File myFile = SD.open("RESET.txt");
    String SdString = "";
    if (myFile) {
        // Serial.println("wifi.txt:");

        while (myFile.available()) {
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

#endif
