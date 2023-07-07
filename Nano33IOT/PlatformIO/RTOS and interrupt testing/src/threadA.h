#ifndef THREAD_A_h
#define THREAD_A_h

#include <Arduino.h>
#include "RTOS_helper.h"

TaskHandle_t Handle_aTask;

static void threadA( void *pvParameters ) 
{
  
  SERIAL.println("Thread A: Started");
  while(1)
  {
    SERIAL.println("A");
    SERIAL.flush();
    myDelayMs(2000);
  }
}



#endif