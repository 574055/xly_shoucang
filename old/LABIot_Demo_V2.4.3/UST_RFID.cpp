#include "UST_RFID.h"

char *getTagfromHEX_check(unsigned char HexArr[]) {
  static char result[2 * NUMBEROFBYTES + 1]; //Note there needs to be 1 extra space for this to work as snprintf null terminates.
  char* myPtr = &result[0];
  
    for (uint8_t i = 0; i < 2; i++){ // loop the tag id
      snprintf(myPtr, 3, "%02x", HexArr[5 + i]); //start from 12th to 16th and add 20 per tag scanned //convert a byte to character string, and save 2 characters (+null) to charArr;
      myPtr += 2; //increment the pointer by two characters in charArr so that next time the null from the previous go is overwritten.
    }
  
  return result;
}

char *getTagfromHEX(unsigned char HexArr[], uint8_t nTags = 6) {
  static char result[2 * NUMBEROFBYTES + 1]; //Note there needs to be 1 extra space for this to work as snprintf null terminates.
  char* myPtr = &result[0];
  for (uint8_t j = 0; j < nTags; j++){ //loop each tag
    for (uint8_t i = 0; i < 4; i++){ // loop the tag id
      snprintf(myPtr, 3, "%02x", HexArr[12 + i + (20 * j)]); //start from 12th to 16th and add 20 per tag scanned //convert a byte to character string, and save 2 characters (+null) to charArr;
      myPtr += 2; //increment the pointer by two characters in charArr so that next time the null from the previous go is overwritten.
    }
  } 
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
