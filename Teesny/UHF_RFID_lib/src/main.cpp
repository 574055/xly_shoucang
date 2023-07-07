#include <Arduino.h>

#include "UHF_RFID.h"
/*
  UST_RFID
  
  Basic library to read from the JRD-100 UHF-RFID Module

  Author: gstoettnerjo@gmail.com
 */

class Test
{
  private:
    Stream *_streamRef;
    const byte readMulti[10] = {0xBB, 0x00, 0x27, 0x00, 0x03, 0x22, 0x00, 0x14, 0x60, 0x7E}; // x20
  
  public:
    void begin(Stream *streamObject)
    {
      _streamRef=streamObject;
    }
    
    void println(char *someText)
    {
      _streamRef->println(someText);
    }

    char read(){
      return _streamRef->read();
    }

    void cmd(){
      _streamRef->write(readMulti, sizeof(readMulti));
    }
};

Test test;

void setup() {
  Serial.begin(9600);
  // Serial2.begin(9600);
  // test.begin(&Serial2); // tell the class which serial object to use
}

void loop() {
  // test.println("Hello, world!");
  if (Serial.available()){
    char var = test.read();
    Serial.println(var);
    // test.cmd();
    // Serial.println(var);
  }

}