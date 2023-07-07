#include "main.h"

#include "threadRFID.h"
#include "threadHX711.h"
#include "threadRESET.h"
#include "threadGUI.h"


//**************************************************************************
// Type Defines and Constants
//**************************************************************************

#define ERROR_LED_PIN 13 // Led Pin: Typical Arduino Board
// #define ERROR_LED_PIN  2 //Led Pin: samd21 xplained board
#define RFID1_EN 4
#define BUTTON_PIN	2

#define ERROR_LED_LIGHTUP_STATE HIGH // the state that makes the led light up on your board, either low or high


//**************************************************************************
// global variables
//**************************************************************************
TaskHandle_t Handle_main;
TaskHandle_t Handle_monitorTask;
SemaphoreHandle_t myMutex;



static void mainTread(void *pvParameters)
{
  debugln("Thread MAIN: Started");

	// setup the hardware as a input to listen to
	pinMode(BUTTON_PIN, INPUT_PULLUP);
	attachInterrupt( BUTTON_PIN, Interrupt_MyHandler_IRQ, RISING );

	while(1)
	{
		static bool rotDir = HIGH;
		// wait for a interrupt to release the task
		while (ulTaskNotifyTake( pdTRUE, portMAX_DELAY ) != pdPASS)
		{
			// do nothing
		}

		// digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));

		stopMotor(1);

		readFlag1 = false;
		Sendcommand(5); //stop RFID scan
		digitalWrite(RFID1_EN,LOW); //turn off RFID

    debugln((String) "finalTags1: " + finalTags1);
    debugln((String) "numberOfTags1: " + numberOfTags1);

    uint8_t numberOfTags = finalTags1.length() / 8;
    static uint8_t prev_numberOfTags = 0;
    // debugln("TOTAL:");
    Serial.println((String)"w" + estimated_weight); //send weight to GUI
    Serial.println((String)"n" + numberOfTags); //send numberOfTags to GUI
    Serial.println((String)"f" + finalTags1); //send finalTags to GUI

    // check for added/removed tags
    static String prev_finalTags = "";
    String addedTags = tagsChanged(prev_finalTags, prev_numberOfTags, finalTags1, numberOfTags);
    String removedTags = tagsChanged(finalTags1, numberOfTags, prev_finalTags, prev_numberOfTags);
    Serial.println((String)"d" + addedTags); //send numberOfTags to GUI
    Serial.println((String)"r" + removedTags); //send finalTags to GUI 
    prev_finalTags = finalTags1;
    prev_numberOfTags = numberOfTags;
    
    oledDisplayData(estimated_weight, numberOfTags);
    
    
    //Send Data through Wifi  
    String scanresult = prepareDataForHttpReq(finalTags1, numberOfTags);
    String timestamp = "1";
    data = buildData(scanresult, timestamp, (String)estimated_weight, unitinfo);
    //debugln((String)"payload: " + data);
    sendData2Server(data);
    //END of Send Data through Wifi 

    if(status != WL_CONNECTED){
      writeSD(data);
    }
    myDelayMs(3000);

		readFlag1 = true;
		digitalWrite(RFID1_EN,HIGH); //turn on RFID
		myDelayMs(70); //RFID need some time to be ready
		vTaskResume(Handle_RFID1); //reads RFID
    Sendcommand(4); //start RFID scan

    rotDir = !rotDir; //flip direction
    directionControl(rotDir);

		// myDelayMs(2000); // help debounce the interrupt handling
	}
}


void setup()
{
  pinMode (chipSelect, OUTPUT);
  pinMode (REDLED, OUTPUT); 
  pinMode (BLUELED, OUTPUT); 
  pinMode (GREENLED, OUTPUT); 
  pinMode (BUZZER, OUTPUT); 
  // pinMode(LED_BUILTIN, OUTPUT);
  pinMode(RFID1_EN, OUTPUT);
  digitalWrite(GREENLED, HIGH);
  digitalWrite(RFID1_EN, HIGH);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(RESET_PIN, INPUT_PULLUP);

  Serial.begin(115200);
  Serial1.begin(115200); // RFID 1
  // while (!Serial) {
  //   ; // wait for Serial port to connect. Needed for native USB port only. ONLY FOR DEBUGGING PURPOSE!!!!!
  // }
  
  oledInit();
  SDInit();
  weightscaleInit(SCALE_FACTOR);
  wifiInit();
  motorInit();
  

  delay(3000); // prevents usb driver crash on startup, do not omit this
  // weightscaleInit(default_scalefactor);
  debugln("");
  debugln("******************************");
  debugln("        Program start         ");
  debugln("******************************");
  Serial.flush();

  // Set the led the rtos will blink when we have a fatal rtos error
  // RTOS also Needs to know if high/low is the state that turns on the led.
  // Error Blink Codes:
  //    3 blinks - Fatal Rtos Error, something bad happened. Think really hard about what you just changed.
  //    2 blinks - Malloc Failed, Happens when you couldn't create a rtos object.
  //               Probably ran out of heap.
  //    1 blink  - Stack overflow, Task needs more bytes defined for its stack!
  //               Use the taskMonitor thread to help gauge how much more you need
  vSetErrorLed(ERROR_LED_PIN, ERROR_LED_LIGHTUP_STATE);

  // sets the Serial port to print errors to when the rtos crashes
  // if this is not set, Serial information is not printed by default
  vSetErrorSerial(&Serial);

  // Create the threads that will be managed by the rtos
  // Sets the stack size and priority of each task
  // Also initializes a handler pointer to each task, which are important to communicate with and retrieve info from tasks
  xTaskCreate(mainTread, "Task Main", 256, NULL, tskIDLE_PRIORITY + 3, &Handle_main);
  xTaskCreate(threadReadRFID1, "Task RFID1", 256, NULL, tskIDLE_PRIORITY + 2, &Handle_RFID1);
  xTaskCreate(threadHX711, "Task HX711", 256, NULL, tskIDLE_PRIORITY + 1, &Handle_HX711);
  xTaskCreate(threadRESET,     "Task RESET",       256, NULL, tskIDLE_PRIORITY + 2, &Handle_reset_Task);
  xTaskCreate(threadGUI,     "Task GUI",       256, NULL, tskIDLE_PRIORITY + 1, &Handle_GUI);
  // xTaskCreate(threadA,     "Task A",       256, NULL, tskIDLE_PRIORITY + 3, &Handle_aTask);
  // xTaskCreate(threadMotor, "Task RFID1", 256, NULL, tskIDLE_PRIORITY + 2, &Handle_Motor);

  // xTaskCreate(taskMonitor, "Task Monitor", 256, NULL, tskIDLE_PRIORITY + 1, &Handle_monitorTask);

  // Start the RTOS, this function will never return and will schedule the tasks.
  vTaskStartScheduler();

  // error scheduler failed to start
  // should never get here
  while (1)
  {
    Serial.println("Scheduler Failed! \n");
    Serial.flush();
    delay(1000);
  }
}

//*****************************************************************
// This is now the rtos idle loop
// No rtos blocking functions allowed!
//*****************************************************************
void loop()
{
  // Optional commands, can comment/uncomment below
  debug("."); // print out dots in terminal, we only do this when the RTOS is in the idle state
  // digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  delay(1000); // delay is interrupt friendly, unlike vNopDelayMS
}

//*****************************************************************


static char ptrTaskList[400]; // temporary string buffer for task stats

void taskMonitor(void *pvParameters)
{
  //int x;
  int measurement;

  Serial.println("Task Monitor: Started");

  // run this task afew times before exiting forever
  while (1)
  {
    myDelayMs(10000); // print every 10 seconds

    Serial.flush();
    Serial.println("");
    Serial.println("****************************************************");
    Serial.print("Free Heap: ");
    Serial.print(xPortGetFreeHeapSize());
    Serial.println(" bytes");

    Serial.print("Min Heap: ");
    Serial.print(xPortGetMinimumEverFreeHeapSize());
    Serial.println(" bytes");
    Serial.flush();

    Serial.println("****************************************************");
    Serial.println("Task            ABS             %Util");
    Serial.println("****************************************************");

    vTaskGetRunTimeStats(ptrTaskList); // save stats to char array
    Serial.println(ptrTaskList);       // prints out already formatted stats
    Serial.flush();

    Serial.println("****************************************************");
    Serial.println("Task            State   Prio    Stack   Num     Core");
    Serial.println("****************************************************");

    vTaskList(ptrTaskList);      // save stats to char array
    Serial.println(ptrTaskList); // prints out already formatted stats
    Serial.flush();

    Serial.println("****************************************************");
    Serial.println("[Stacks Free Bytes Remaining] ");

    measurement = uxTaskGetStackHighWaterMark(Handle_main);
    Serial.print("Thread Servo: ");
    Serial.println(measurement);

    measurement = uxTaskGetStackHighWaterMark(Handle_RFID1);
    Serial.print("Thread RFID1: ");
    Serial.println(measurement);

    measurement = uxTaskGetStackHighWaterMark(Handle_monitorTask);
    Serial.print("Monitor Stack: ");
    Serial.println(measurement);

    Serial.println("****************************************************");
    Serial.flush();
  }

  // delete ourselves.
  // Have to call this or the system crashes when you reach the end bracket and then get scheduled.
  Serial.println("Task Monitor: Deleting");
  vTaskDelete(NULL);
}


void Interrupt_MyHandler_IRQ()
{

	#ifdef ENABLE_DEBUG_OUTPUT_ISR
		Serial.print(F("["));
	#endif

	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	// release task to handle the radios read data
	vTaskNotifyGiveFromISR( Handle_main, &xHigherPriorityTaskWoken );
	portYIELD_FROM_ISR( xHigherPriorityTaskWoken );

	#ifdef ENABLE_DEBUG_OUTPUT_ISR
		Serial.print(F("]"));
		Serial.flush();
	#endif

}