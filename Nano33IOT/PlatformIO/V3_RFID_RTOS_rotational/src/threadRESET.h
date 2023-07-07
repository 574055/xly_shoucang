#ifndef THREAD_RESET_h
#define THREAD_RESET_h

#include "main.h"
#include "SD_CARD.h"
#include "HX711.h"

#define RESET_PIN	3

TaskHandle_t Handle_reset_Task;

void Interrupt_resetHandler_IRQ()
{

	#ifdef ENABLE_DEBUG_OUTPUT_ISR
		Serial.print(F("["));
	#endif

	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	// release task to handle the radios read data
	vTaskNotifyGiveFromISR( Handle_reset_Task, &xHigherPriorityTaskWoken );
	portYIELD_FROM_ISR( xHigherPriorityTaskWoken );

	#ifdef ENABLE_DEBUG_OUTPUT_ISR
		Serial.print(F("]"));
		Serial.flush();
	#endif

}

static void threadRESET( void *pvParameters ) 
{
	Serial.println("Thread RESET: Started");

	// setup the hardware as a input to listen to
	pinMode(RESET_PIN, INPUT_PULLUP);
	attachInterrupt( RESET_PIN, Interrupt_resetHandler_IRQ, RISING );

	while(1)
	{
		// wait for a interrupt to release the task
		while (ulTaskNotifyTake( pdTRUE, portMAX_DELAY ) != pdPASS)
		{
			// do nothing
		}
		SD.remove("reset.txt"); // delete reset.txt, so only last offset will be saved
		writeSD_reset((String)scale.get_offset());
		Serial.print("RESET");
		Serial.flush();
		myDelayMs(500);
		NVIC_SystemReset();  //reset Arduino
		// digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));

	}

}



#endif