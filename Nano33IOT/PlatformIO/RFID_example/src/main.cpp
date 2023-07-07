#include <Arduino.h>

#include "myconfig.h"
#include <FlashStorage_SAMD.h>
#include <SPI.h>
#include <Wire.h>
#include "UST_RFID.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>



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
// screen
//  On an arduino UNO:       A4(SDA), A5(SCL)
#define SCREEN_WIDTH 128    // OLED display width, in pixels
#define SCREEN_HEIGHT 32    // OLED display height, in pixels
#define OLED_RESET 4        // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3c ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// #define timeout 900
// uint8_t totaltags[1000];

void setup()
{
  randomSeed(analogRead(0));

  Serial1.begin(115200); // for RFID
  Serial.begin(115200);
  // Serial.setTimeout(500);
  oledInit();

  waitSerial(wait_for_Serial);
  debugln("Serial port ready");
  eepromInit();

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
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.print((String) "Wait " + waittime / 1000 + " s for Serial...");
  display.display();

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

void displaySavedData()
{
  Serial.print("W");
  Serial.println(e_data.SSID);
  Serial.print("F");
  Serial.println(e_data.SCALEFACTOR);
}

void eepromInit()
{ // safes default values to eeprom if empty
  EEPROM.get(storedAddress, signature);

  // If the EEPROM is empty then no WRITTEN_SIGNATURE
  if (signature == WRITTEN_SIGNATURE) // if something is in Flash
  {
    EEPROM.get(storedAddress + sizeof(signature), e_data);
    displaySavedData();
  }
  else // safe default settings to eeprom if empty
  {
    EEPROM.put(storedAddress, WRITTEN_SIGNATURE);

    String SCALEFACTOR = "-200";

    // Fill the "e_data" structure with the data entered by the user...

    SCALEFACTOR.toCharArray(e_data.SCALEFACTOR, 10);

    // ...and finally save everything into emulated-EEPROM
    EEPROM.put(storedAddress + sizeof(signature), e_data);

    if (!EEPROM.getCommitASAP())
    {
      Serial.println("CommitASAP not set. Need commit()");
      EEPROM.commit();
    }
  }
}

void oledInit()
{
  Wire.begin();
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
  {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // Don't proceed, loop forever
  }
  display.clearDisplay();
  display.setTextSize(2);              // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 0);             // Start at top-left corner
  display.println("Wesahre");
  display.println("...loading");
  display.display();
}
