#include "UST_RFID.h"

uint8_t multiscan(uint8_t result[], unsigned long timeout){
  //returns the number of tags 
  //resutl = the resulting scan as result array in HEX
  //timeout = max time to wait after each Serial read
  const byte readMulti[] = {0xBB, 0x00, 0x27, 0x00, 0x03, 0x22, 0x00, 0x0A, 0x56, 0x7E}; //x10
  bool finished_scan = false; 
  uint8_t tags = 0;
  uint16_t i = 0;
  unsigned long currentMillis=0; // millis() returns an unsigned long.
  Serial1.write(readMulti, sizeof(readMulti));
  currentMillis = millis(); //reset timer;
     
  while (!finished_scan)
  {
    while (Serial1.available()) 
    {
      result[i] = Serial1.read();
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
      Serial.println("finished Read");

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
}

char *getTagfromHEX_check(uint8_t HexArr[]) 
{
  static char result[2 * NUMBEROFBYTES + 1]; //Note there needs to be 1 extra space for this to work as snprintf null terminates.
  char* myPtr = &result[0];
  
    for (uint8_t i = 0; i < 2; i++){ // loop the tag id
      snprintf(myPtr, 3, "%02x", HexArr[5 + i]); //start from 12th to 16th and add 20 per tag scanned //convert a byte to character string, and save 2 characters (+null) to charArr;
      myPtr += 2; //increment the pointer by two characters in charArr so that next time the null from the previous go is overwritten.
    }
  
  return result;
}

char *getTagfromHEX(uint8_t HexArr[], uint8_t nTags) {
  //returns only the Tag numbers of interset
  static char result[2 * NUMBEROFBYTES + 1] = {0}; //Note there needs to be 1 extra space for this to work as snprintf null terminates.
  char* myPtr = &result[0];
  uint16_t j = 0;
  uint8_t tags = 0;
  while (j < NUMBEROFBYTES && tags < nTags){ //loop through the whole HexArr till all Tags are checked
    if (HexArr[j] == 0x7e){
      tags++;
      j = j - 7;
      //Serial.println((String)"j: " + j);
      for (uint8_t i = 0; i < 4; i++){ // loop the tag id   
        //Serial.println((String)"HexArr: " + HexArr[j]);    
        snprintf(myPtr, 3, "%02x", HexArr[j]); //start from 12th to 16th and add 20 per tag scanned //convert a byte to character string, and save 2 characters (+null) to charArr;
        myPtr += 2; //increment the pointer by two characters in charArr so that next time the null from the previous go is overwritten.
        j++;
      }
      j += 7;
      //Serial.println((String)"j: " + j);
    }
    j++;
  } 
  //Serial.println((String)"result: " + result);
  return result;
}

String keepIndividuals (String scan1, uint8_t nTags1, String scan2, uint8_t nTags2){
  uint8_t i = 0, j = 0;
  for (j = 0; j < nTags2; j++) {
    bool alreadyHere = false;
    for (i = 0; i<nTags1; i++){
      if(scan1.substring(8 * i ,8 + 8 * i) == scan2.substring(8 * j ,8 + 8 * j)){
        alreadyHere = true;
        break;
      }
    }
    if (!alreadyHere) {
      scan1 += scan2.substring(8 * j ,8 + 8 * j);
    }
  }  
  return scan1;
}

String keepIndividualsOfMultiScan (String scan, uint8_t *ntotalTags){
  uint8_t j = 0, nTags = 0; 
  String result = "";
  //Serial.println((String)"result: " + result);
  for (uint8_t i = 0; i<*ntotalTags; i++){
    if(scan.substring(8 * i ,8 + 8 * i) != "bb01ff00"){ //do the checking only if there is a valid tag
      j = 0;
      bool alreadyHere = false;
      while(j<nTags){
        //Serial.println((String)"Compare: " + scan.substring(8 * i ,8 + 8 * i) + "with: " + result.substring(8 * j ,8 + 8 * j));
        if(scan.substring(8 * i ,8 + 8 * i) == result.substring(8 * j ,8 + 8 * j)){
          alreadyHere = true;
          break;
        }   
        j++;
      }
      if (!alreadyHere) {
        result += scan.substring(8 * i ,8 + 8 * i); //Add ID to result
        nTags++; 
      }    
    }
    
  }
  *ntotalTags = nTags;
  return result;
}