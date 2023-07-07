#include <Arduino.h>
#include "UST_RFID.h"

#include <DHT.h>
#include <DHT_U.h>

//DHT11
#define DHTPIN 19
#define DHTTYPE DHT11   
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  randomSeed(analogRead(0));

  Serial.begin(115200);
  Serial2.begin(115200);
  waitSerial(1000);

  dht.begin();
  rfidInit();

}

void loop() {
  //myTime = millis();

  //DHT11
  float h1 = dht.readHumidity();
  float t1 = dht.readTemperature();
  if(isnan(t1))
  {
    t1=-100.00;
  }
  if(isnan(h1))
  {
    h1=-1.00;
  }
  Serial.println(h1);
  Serial.println(t1);

  // RFID
  // Scan RFID1
  uint8_t numberOfTags1 = 0;
  String finalTags = multiscan_v2(&numberOfTags1, TIMEOUT);
  uint8_t numberOfTags = finalTags.length() / 8;
  static uint8_t prev_numberOfTags = 0;
  Serial.print((String) "n" + numberOfTags); // send numberOfTags to GUI
  Serial.print('\t');
  Serial.print((String) "f" + finalTags);    // send finalTags to GUI
  Serial.print('\t');
  static String prev_finalTags = "";
  String addedTags = tagsChanged(prev_finalTags, prev_numberOfTags, finalTags, numberOfTags);
  String removedTags = tagsChanged(finalTags, numberOfTags, prev_finalTags, prev_numberOfTags);
  Serial.print((String) "d" + addedTags);   // send numberOfTags to GUI
  Serial.print((String) "r" + removedTags); // send finalTags to GUI
  prev_finalTags = finalTags;
  prev_numberOfTags = numberOfTags;
}




//****************************************************
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
  Serial2.write(setPower, sizeof(setPower));
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
  Serial2.write(readMulti, sizeof(readMulti));
  currentMillis = millis(); // reset timer;
  uint8_t result[100] ={0};
  String finalTags = "";
  
  while (!finished_scan)
  {
    while (Serial2.available())
    {

      result[i] = Serial2.read();
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
  Serial2.write(readMulti, sizeof(readMulti));
  currentMillis = millis(); // reset timer;

  while (!finished_scan)
  {
    while (Serial2.available())
    {
      result[i] = Serial2.read();
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