#ifndef THREAD_B_h
#define THREAD_B_h

#include <Arduino.h>
#include "RTOS_helper.h"

#define BUTTON_PIN	2
//#define ENABLE_DEBUG_OUTPUT_ISR

TaskHandle_t Handle_bTask;

void Interrupt_MyHandler_IRQ()
{

	#ifdef ENABLE_DEBUG_OUTPUT_ISR
		SERIAL.print(F("["));
	#endif

	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	// release task to handle the radios read data
	vTaskNotifyGiveFromISR( Handle_bTask, &xHigherPriorityTaskWoken );
	portYIELD_FROM_ISR( xHigherPriorityTaskWoken );

	#ifdef ENABLE_DEBUG_OUTPUT_ISR
		SERIAL.print(F("]"));
		SERIAL.flush();
	#endif

}

static void threadB( void *pvParameters ) 
{
	SERIAL.println("Thread B: Started");

	// setup the hardware as a input to listen to
	pinMode(BUTTON_PIN, INPUT_PULLUP);
	attachInterrupt( BUTTON_PIN, Interrupt_MyHandler_IRQ, RISING );

	while(1)
	{

		// wait for a interrupt to release the task
		while (ulTaskNotifyTake( pdTRUE, portMAX_DELAY ) != pdPASS)
		{
			// do nothing
		}

		SERIAL.print("B");
		SERIAL.flush();

		myDelayMs(200); // help debounce the interrupt handling
	}

}



#endif