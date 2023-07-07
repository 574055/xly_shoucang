#include <HX711.h>
#include <Arduino.h>
#include <WiFi.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <SPI.h>


#include "Main.h"
#include "SD_CARD.h"
#include "Weight.h"
#include "Wifi.h"
#include "Oled.h"
#include "UST_RFID.h"



void setup() {

  Serial.begin(115200);
  Serial1.begin(115200, SERIAL_8N1, 4, 5);//RX TX
  Serial2.begin(115200, SERIAL_8N2, 26, 27);//RX,TX
  oledInit();
  WeightInit();
  wifiInit();
  rfidInit(rset);

}

void loop() {
  // put your main code here, to run repeatedly:

  int WeightTemp = weightRead(WeightD);
  rfidread();
  dataDisplay();
  

  if (Serial.available())
  {
    String incomingString = Serial.readStringUntil('\n');
    d1 = incomingString.charAt(0);
    
    if (d1 == 't')
    { // tare was clicked
      scale.tare(10);
      // SD.remove("RESET.txt"); // delete reset.txt, so only last offset will be saved
      // writeSD_reset((String)scale.get_offset());
    }
  }

  
}
