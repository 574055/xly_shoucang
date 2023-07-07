#include <Arduino.h>

#include "UST_RFID.h"
#include <SoftwareSerial.h>


#ifdef DEBUG
#define debug(x) Serial.print(x)
#define debugln(x) Serial.println(x)
#else
#define debug(x)
#define debugln(x)
#endif

SoftwareSerial mySerial(2, 3); // RX, TX

void setup() {
  mySerial.begin(115200);
  rfidInit();
}

void loop() {
  //RFID
  //Scan RFID
  uint8_t scanResult[NUMBEROFBYTES];
  uint8_t numberOfTags = multiscan(scanResult, TIMEOUT); //the data is stored in scanResult
  debugln((String)"numberOfTags: " + numberOfTags);
  // for (int i = 0; i<100; i++){
  //     Serial.print(scanResult[i],HEX);
  // }
  // Serial.println((String)"numberOfTags: " + numberOfTags);

  //Extract ID numbers from the whole Hex array and convert to char array
  char *tagID;
  tagID = getTagfromHEX(scanResult, numberOfTags);
  debugln((String)"tagID: " + tagID);
  //Serial.println((String)"getTagfromHEX: " + tagID);


  String finalTags = keepIndividualsOfMultiScan ((String) tagID, &numberOfTags);
  //Serial.println((String)"finalTags: " + finalTags);
  //Serial.println((String)"final numberOfTags: " + numberOfTags);

  //send to GUI
  Serial.println((String)"n" + numberOfTags); //send numberOfTags to GUI
  Serial.println((String)"f" + finalTags); //send finalTags to GUI

  //Add comma and <"> between Tags 
  uint8_t i = 0;
  String scanresult;
  while(i<numberOfTags){
    scanresult += "\"" + finalTags.substring(8 * i,8 + 8 * i) + "\",";
    i++;
  }
  int length = scanresult.length();
  scanresult.remove(length - 1); //remove the last comma
  
  debugln(scanresult);

  
  debugln((String)"number of Tags: " + numberOfTags);
  debugln("finished rfid");
  //END of RFID
}



void rfidInit(){
  const byte setPower[] = {0xBB, 0x00, 0xB6, 0x00, 0x02, 0x0A, 0x28, 0xEA, 0x7E}; //26dBm
  //const byte setPower[] = {0xBB, 0x00, 0xB6, 0x00, 0x02, 0x07, 0xD0, 0x8F, 0x7E}; //20dBm
  //const byte setPower[] = {0xBB, 0x00, 0xB6, 0x00, 0x02, 0x07, 0x3A, 0xF9, 0x7E}; //18.5dBm
  mySerial.write(setPower, sizeof(setPower));
  //Serial.println("set power to 26 dBm");
}


uint8_t multiscan(uint8_t result[], unsigned long timeout){
  //returns the number of tags 
  //resutl ... the resulting scan as result array in HEX
  //timeout ... max time to wait after each Serial read
  //const byte readMulti[] = {0xBB, 0x00, 0x27, 0x00, 0x03, 0x22, 0x00, 0x0A, 0x56, 0x7E}; //x10
  //const byte readMulti[] = {0xBB, 0x00, 0x27, 0x00, 0x03, 0x22, 0x00, 0x0F, 0x5B, 0x7E}; //x15
  const byte readMulti[] = {0xBB, 0x00, 0x27, 0x00, 0x03, 0x22, 0x00, 0x14, 0x60, 0x7E}; //x20
  bool finished_scan = false; 
  uint8_t tags = 0;
  uint16_t i = 0;
  unsigned long currentMillis=0; // millis() returns an unsigned long.
  mySerial.write(readMulti, sizeof(readMulti));
  currentMillis = millis(); //reset timer;
     
  while (!finished_scan)
  {
    while (mySerial.available()) 
    {
      result[i] = mySerial.read();
      // Serial.print(result[i],HEX); 
      // Serial.print(",");
      
      if (result[i] == 0x7e){
        // Serial.print(" ");
        tags++; 
      }
      i++;
      currentMillis = millis(); //reset timer;
    }
  
    
    if(millis() - currentMillis > timeout){ //if timeout is reached then finish reading
      // Serial.println("finished Read");

      //DEBUGGING
      //Serial.println((String)"number of Tags: " + tags);
      //print result
      //for(uint16_t j = 0; j < i; j++)
      //{
      //        Serial.print(result[j],HEX);
      //        Serial.print(",");
      //      }
      //Serial.println((String)"i: " + i);
      //END DEBUGGING
      
      finished_scan = true;
      return tags;
    }
  }
  return 0;
}