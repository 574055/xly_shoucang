#ifndef THREAD_RFID1_h
#define THREAD_RFID1_h

#include "main.h"


TaskHandle_t Handle_RFID1;

static void threadReadRFID1(void *pvParameters)
{
  Serial.println("Thread read RFID1: Started");

  while (1)
  {
    finalTags1 = "";
    // Serial.println((String) "Resume RFID thread: " + finalTags1);
    while(readFlag1 == true){
      // Serial.println((String) "start Readcallback(" + finalTags1 + ")");
      finalTags1 = Readcallback(finalTags1);
      // Serial.println((String) "finalTags1 in RFID thread: " + finalTags1);
      myDelayMs(20);
    }
    // Serial.println((String) "Suspend RFID1");
    vTaskSuspend(Handle_RFID1);

  }

  // delete ourselves.
  Serial.println("Task threadReadRFID1: Deleting");
  vTaskDelete(NULL);

}



#endif