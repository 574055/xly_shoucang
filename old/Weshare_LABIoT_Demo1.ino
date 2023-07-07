#include "HX711.h"
#include <SPI.h>
#include <MFRC522.h>     // Invoke the libraries

const int DT_PIN = 5;
const int SCK_PIN = 6;

#define RST_PIN      A0        // Reset the RFID reader
#define SS_PIN       10       // Tag selection
HX711 scale;
MFRC522 mfrc522(SS_PIN, RST_PIN);  // Setup the MFRC522
 
void setup() {
  Serial.begin(9600);
  Serial.println("WeShare RFID reader is ready!");
 
  SPI.begin();
  mfrc522.PCD_Init();   // Initiate the MFRC522 reader module
  scale.begin(DT_PIN, SCK_PIN);
  scale.set_scale();  // 開始取得比例參數
  scale.tare();
//  Serial.println("Nothing on it.");
//  Serial.println(scale.get_units(10));
//  Serial.println("Please put sample object on it..."); //提示放上基準物品
}
 
void loop() {

  //HX711
  delay(1000);
  float current_weight=scale.get_units(10);
  Serial.print("Weight:  ");
  Serial.println((current_weight)/-200,0.0);  // 顯示比例參數，記起來，以便用在正式的程式中
  
    // Check if there is any new Tag
    if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
      byte *id = mfrc522.uid.uidByte;   // Obtain the UID of Tag
      byte idSize = mfrc522.uid.size;   // Obtain the size of UID
 
      Serial.print("PICC type: ");      // Show the type of Tag
      // Judge the type by returning SAK value（mfrc522.uid.sak）
      MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
      Serial.println(mfrc522.PICC_GetTypeName(piccType));
 
      Serial.print("UID Size: ");       // Show the size of UID
      Serial.println(idSize);
 
      for (byte i = 0; i < idSize; i++) {  // Check the UIDs one by one
        Serial.print("id[");
        Serial.print(i);
        Serial.print("]: ");
        Serial.println(id[i], HEX);       // Show UID in Hex mode
      }
      Serial.println();
 
      mfrc522.PICC_HaltA();  // Stop repeated reading
    } 
}
