#ifndef UST_RFID_h
#define UST_RFID_h

#include "Arduino.h"

#define NUMBEROFBYTES 100 //max length for result array in getTagfromHEX()

//function prototypes
char *getTagfromHEX(unsigned char HexArr[], uint8_t nTags = 6);
char *getTagfromHEX_check(unsigned char HexArr[]);
String keepIndividuals(String scan1, uint8_t nTags1, String scan2, uint8_t nTags2);

#endif
