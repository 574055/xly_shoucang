#ifndef RTOS_HELPER_h
#define RTOS_HELPER_h

#include <Arduino.h>
#include <FreeRTOS_SAMD21.h>

// #define SERIAL          SerialUSB

void myDelayMs(int ms)
{
  vTaskDelay( (ms * 1000) / portTICK_PERIOD_US );  
}

void myDelayMsUntil(TickType_t *previousWakeTime, int ms)
{
  vTaskDelayUntil( previousWakeTime, (ms * 1000) / portTICK_PERIOD_US );  
}


#endif