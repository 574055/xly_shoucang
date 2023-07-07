#include <Arduino.h>

#include "myconfig.h"
#include <SPI.h>
#include <Wire.h>
#include "UST_RFID.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "wiring_private.h"
#include <Servo.h>

Servo myservo;


void oledInit();

void SERCOM0_Handler();
void rfidInit2();
String multiscan2_v2(uint8_t* numberOfTags, unsigned long timeout);



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

Uart mySerial (&sercom0, 5, 6, SERCOM_RX_PAD_1, UART_TX_PAD_0);


void setup()
{
  randomSeed(analogRead(0));

  // Reassign pins 5 and 6 to SERCOM alt
  pinPeripheral(5, PIO_SERCOM_ALT);
  pinPeripheral(6, PIO_SERCOM_ALT);
  mySerial.begin(115200); // for RFID2

  Serial1.begin(115200); // for RFID
  Serial.begin(115200);
  oledInit();

  myservo.attach(9);

  rfidInit();
  rfidInit2();
  debugln("RFID ready");
}

void loop()
{
  debugln();
  debugln();
  debugln("beginn of main loop");
  debugln();

  myTime = millis();

  // RFID1
  uint8_t numberOfTags1 = 0;
  String finalTags1 = multiscan_v2(&numberOfTags1, TIMEOUT);
  // Serial.println("RFID1:");
  // Serial.println((String)"n" + numberOfTags1); //send numberOfTags to GUI
  // Serial.println((String)"f" + finalTags1); //send finalTags1 to GUI

  //mySerial RFID2
  uint8_t numberOfTags2 = 0;
  String finalTags2 = multiscan2_v2(&numberOfTags2, TIMEOUT);
  // Serial.println("RFID2:");
  // Serial.println((String)"n" + numberOfTags2); //send numberOfTags to GUI
  // Serial.println((String)"f" + finalTags2); //send finalTags to GUI

  // data processing
  String finalTags = keepIndividuals(finalTags1, numberOfTags1, finalTags2, numberOfTags2);
  uint8_t numberOfTags = finalTags.length() / 8;
  Serial.println("TOTAL:");
  Serial.println((String)"n" + numberOfTags); //send numberOfTags to GUI
  Serial.println((String)"f" + finalTags); //send finalTags to GUI

  // delay(1000);
}

// private functions ******************************************************************


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

void SERCOM0_Handler()
{
    mySerial.IrqHandler();
}


// RFID
void rfidInit2()
{
  const byte setPower[] = {0xBB, 0x00, 0xB6, 0x00, 0x02, 0x0A, 0x28, 0xEA, 0x7E}; // 26dBm
  // const byte setPower[] = {0xBB, 0x00, 0xB6, 0x00, 0x02, 0x07, 0xD0, 0x8F, 0x7E}; //20dBm
  // const byte setPower[] = {0xBB, 0x00, 0xB6, 0x00, 0x02, 0x07, 0x3A, 0xF9, 0x7E}; //18.5dBm
  //  const byte setPower[] = {0xBB, 0x00, 0xB6, 0x00, 0x02, 0x06, 0x0E, 0xCC, 0x7E}; //15.5dBm
  mySerial.write(setPower, sizeof(setPower));
  // Serial.println("set power to 26 dBm");
}


String multiscan2_v2(uint8_t* numberOfTags, unsigned long timeout)
{
  // const byte readMulti[] = {0xBB, 0x00, 0x22, 0x00, 0x00, 0x22, 0x7E}; //x1
  // const byte readMulti[] = {0xBB, 0x00, 0x27, 0x00, 0x03, 0x22, 0x00, 0x03, 0x4F, 0x7E}; // x3
  // const byte readMulti[] = {0xBB, 0x00, 0x27, 0x00, 0x03, 0x22, 0x00, 0x14, 0x60, 0x7E}; // x20
  const byte readMulti[] = {0xBB, 0x00, 0x27, 0x00, 0x03, 0x22, 0x00, 0x1E, 0x6A, 0x7E}; // x30
  bool finished_scan = false;
  uint8_t tags = 0;
  uint16_t i = 0;
  unsigned long currentMillis = 0; // millis() returns an unsigned long.
  mySerial.write(readMulti, sizeof(readMulti));
  currentMillis = millis(); // reset timer;
  uint8_t result[100] ={0};
  String finalTags = "";
  
  while (!finished_scan)
  {
    while (mySerial.available())
    {

      result[i] = mySerial.read();
      // Serial.print(result[i], HEX);
      // Serial.print(",");

      if (result[i] == 0x7e)
      {
        // Serial.println((String)"i: " + i);
        // if (i < 8)
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