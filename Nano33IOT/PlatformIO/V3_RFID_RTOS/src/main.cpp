#include <Arduino.h>

#include <FreeRTOS_SAMD21.h>
#include "HX711.h"
#include <Servo.h>
#include "UST_RFID.h"
#include "wiring_private.h" //for sercom serial ports
#include <SPI.h>  //for OLED
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


void rfidInit2();
void oledInit();
void weightscaleInit(float SCALEFACTOR);


// Weight Scale 
const int DT_PIN = A0;
const int SCK_PIN = A1;
HX711 scale;
// const String default_scalefactor = "22";
const float default_scalefactor = 22.0;

// Pinouts
#define REDLED 8
#define GREENLED 9     //toggles after each loop
#define BLUELED 10    //is on when not connected to wifi
#define BUZZER A7

//screen
// On an arduino UNO:       A4(SDA), A5(SCL)
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3c ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Servo
Servo myservo1;
Servo myservo2;
volatile int pos = 0;
#define SERVODELAY 50
volatile bool readFlag1 = false, readFlag2 = false;

String finalTags1 = "";
String finalTags2 = "";
//**************************************************************************
// Type Defines and Constants
//**************************************************************************

#define ERROR_LED_PIN 13 // Led Pin: Typical Arduino Board
//#define  ERROR_LED_PIN  2 //Led Pin: samd21 xplained board
#define RFID1_EN 3
#define RFID2_EN 7

#define ERROR_LED_LIGHTUP_STATE HIGH // the state that makes the led light up on your board, either low or high

// Select the serial port the project should use and communicate over
// Some boards use SerialUSB, some use Serial
#define SERIAL SerialUSB // Sparkfun Samd21 Boards
//#define SERIAL          Serial //Adafruit, other Samd21 Boards

//**************************************************************************
// global variables
//**************************************************************************
TaskHandle_t Handle_Servo;
TaskHandle_t Handle_RFID1;
TaskHandle_t Handle_RFID2;
TaskHandle_t Handle_monitorTask;
SemaphoreHandle_t myMutex;

// UART 2
void SERCOM0_Handler();
Uart mySerial2 (&sercom0, 5, 6, SERCOM_RX_PAD_1, UART_TX_PAD_0);
String Readcallback2(String finalTags);
void Sendcommand2(uint8_t com_nub);

void myDelayMsUntil(TickType_t *previousWakeTime, int ms)
{
  vTaskDelayUntil(previousWakeTime, (ms * 1000) / portTICK_PERIOD_US);
}

static void threadServo(void *pvParameters)
{

  SERIAL.println("Thread Servo: Started");
  while (1)
  {
    //SERIAL.println("reached Servo start");
    readFlag1 = true;
    digitalWrite(RFID1_EN,HIGH); //turn on RFID
    // digitalWrite(RFID2_EN,HIGH);
    myDelayMs(70); //RFID need some time to be ready

    vTaskResume(Handle_RFID1);
    // vTaskResume(Handle_RFID2);
    Sendcommand(4); //start RFID scan
    // Sendcommand2(4);
    for (pos = 0; pos <= 180; pos += 1)
    { // goes from 0 degrees to 180 degrees
      // in steps of 1 degree
      myservo1.write(pos); // tell servo to go to position in variable 'pos'
      myservo2.write(pos);
      myDelayMs(SERVODELAY);          // waits 15ms for the servo to reach the position
    }
    //SERIAL.println("reached Servo end");
    readFlag1 = false;
    // Sendcommand(5); //stop RFID scan
    // Sendcommand2(5);
    digitalWrite(RFID1_EN,LOW); //turn off RFID
    // digitalWrite(RFID2_EN,LOW);



    
    //Rotate back
    //SERIAL.println("reached Servo start");
    readFlag2 = true;
    digitalWrite(RFID2_EN,HIGH);
    myDelayMs(70); //RFID need some time to be ready

    vTaskResume(Handle_RFID2);
    Sendcommand2(4);

    //Rotate back
    for (pos = 180; pos >= 0; pos -= 1)
    {                     // goes from 180 degrees to 0 degrees
      myservo1.write(pos); // tell servo to go to position in variable 'pos'
      myservo2.write(pos);
      myDelayMs(SERVODELAY);          // waits 15ms for the servo to reach the position
    }

    //SERIAL.println("reached Servo end");
    readFlag2 = false;
    digitalWrite(RFID2_EN,LOW); //turn off RFID


    //RFID
    //RFID 1
    uint8_t numberOfTags1 = finalTags1.length() / 8;
    Serial.println((String) "finalTags1: " + finalTags1);
    Serial.println((String) "numberOfTags1: " + numberOfTags1);
    //RFID 2
    uint8_t numberOfTags2 = finalTags2.length() / 8;
    Serial.println((String) "finalTags2: " + finalTags2);
    Serial.println((String) "numberOfTags2: " + numberOfTags2);
    // Weight
    scale.power_up();
    myDelayMs(500);
    float weight = scale.get_units(5);
    scale.power_down();
    // data processing
    String finalTags = keepIndividuals(finalTags1, numberOfTags1, finalTags2, numberOfTags2);
    uint8_t numberOfTags = finalTags.length() / 8;
    static uint8_t prev_numberOfTags = 0;
    // Serial.println("TOTAL:");
    Serial.println((String)"w" + weight); //send weight to GUI
    Serial.println((String)"n" + numberOfTags); //send numberOfTags to GUI
    Serial.println((String)"f" + finalTags); //send finalTags to GUI

    // check for added/removed tags
    static String prev_finalTags = "";
    String addedTags = tagsChanged(prev_finalTags, prev_numberOfTags, finalTags, numberOfTags);
    String removedTags = tagsChanged(finalTags, numberOfTags, prev_finalTags, prev_numberOfTags);
    Serial.println((String)"d" + addedTags); //send numberOfTags to GUI
    Serial.println((String)"r" + removedTags); //send finalTags to GUI 
    prev_finalTags = finalTags;
    prev_numberOfTags = numberOfTags;
    
    //display on screen
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(0, 0);
    display.println((String) weight + " g");
    display.println((String) numberOfTags + " Tags");
    display.display();
  }
}

static void threadReadRFID1(void *pvParameters)
{
  SERIAL.println("Thread read RFID1: Started");

  while (1)
  {
    finalTags1 = "";
    //Serial.println((String) "Resume RFID thread: " + finalTags1);
    while(readFlag1 == true){
      //Serial.println((String) "start Readcallback(" + finalTags + ")");
      finalTags1 = Readcallback(finalTags1);
      //Serial.println((String) "finalTags1 in RFID thread: " + finalTags1);
      myDelayMs(20);
    }
    
    vTaskSuspend(Handle_RFID1);

  }

  // delete ourselves.
  SERIAL.println("Task threadReadRFID1: Deleting");
  vTaskDelete(NULL);

}


static void threadReadRFID2(void *pvParameters)
{
  SERIAL.println("Thread read RFID2: Started");

  while (1)
  {
    finalTags2 = "";
    // Serial.println((String) "Resume RFID thread2: " + finalTags2);
    while(readFlag2 == true){
      //Serial.println((String) "start Readcallback(" + finalTags2 + ")");
      finalTags2 = Readcallback2(finalTags2);
      // Serial.println((String) "finalTags2 in RFID thread: " + finalTags2);
      myDelayMs(20);
    }
    
    vTaskSuspend(Handle_RFID2);

  }

  // delete ourselves.
  SERIAL.println("Task threadReadRFID2: Deleting");
  vTaskDelete(NULL);

}


//*****************************************************************
// Task will periodically print out useful information about the tasks running
// Is a useful tool to help figure out stack sizes being used
// Run time stats are generated from all task timing collected since startup
// No easy way yet to clear the run time stats yet
//*****************************************************************
static char ptrTaskList[400]; // temporary string buffer for task stats

void taskMonitor(void *pvParameters)
{
  //int x;
  int measurement;

  SERIAL.println("Task Monitor: Started");

  // run this task afew times before exiting forever
  while (1)
  {
    myDelayMs(10000); // print every 10 seconds

    SERIAL.flush();
    SERIAL.println("");
    SERIAL.println("****************************************************");
    SERIAL.print("Free Heap: ");
    SERIAL.print(xPortGetFreeHeapSize());
    SERIAL.println(" bytes");

    SERIAL.print("Min Heap: ");
    SERIAL.print(xPortGetMinimumEverFreeHeapSize());
    SERIAL.println(" bytes");
    SERIAL.flush();

    SERIAL.println("****************************************************");
    SERIAL.println("Task            ABS             %Util");
    SERIAL.println("****************************************************");

    vTaskGetRunTimeStats(ptrTaskList); // save stats to char array
    SERIAL.println(ptrTaskList);       // prints out already formatted stats
    SERIAL.flush();

    SERIAL.println("****************************************************");
    SERIAL.println("Task            State   Prio    Stack   Num     Core");
    SERIAL.println("****************************************************");

    vTaskList(ptrTaskList);      // save stats to char array
    SERIAL.println(ptrTaskList); // prints out already formatted stats
    SERIAL.flush();

    SERIAL.println("****************************************************");
    SERIAL.println("[Stacks Free Bytes Remaining] ");

    measurement = uxTaskGetStackHighWaterMark(Handle_Servo);
    SERIAL.print("Thread Servo: ");
    SERIAL.println(measurement);

    measurement = uxTaskGetStackHighWaterMark(Handle_RFID1);
    SERIAL.print("Thread RFID1: ");
    SERIAL.println(measurement);

    measurement = uxTaskGetStackHighWaterMark(Handle_RFID2);
    SERIAL.print("Thread RFID2: ");
    SERIAL.println(measurement);

    measurement = uxTaskGetStackHighWaterMark(Handle_monitorTask);
    SERIAL.print("Monitor Stack: ");
    SERIAL.println(measurement);

    SERIAL.println("****************************************************");
    SERIAL.flush();
  }

  // delete ourselves.
  // Have to call this or the system crashes when you reach the end bracket and then get scheduled.
  SERIAL.println("Task Monitor: Deleting");
  vTaskDelete(NULL);
}

//*****************************************************************

void setup()
{
  pinMode (REDLED, OUTPUT); 
  pinMode (BLUELED, OUTPUT); 
  pinMode (GREENLED, OUTPUT); 
  pinMode (BUZZER, OUTPUT); 
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(RFID1_EN, OUTPUT);
  pinMode(RFID2_EN, OUTPUT);
  digitalWrite(GREENLED, HIGH);
  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(RFID1_EN, HIGH);
  digitalWrite(RFID2_EN, HIGH);

  oledInit();
  rfidInit();
  rfidInit2();
  

  // Reassign pins 5 and 6 to SERCOM alt
  pinPeripheral(5, PIO_SERCOM_ALT);
  pinPeripheral(6, PIO_SERCOM_ALT);
  mySerial2.begin(115200); // for RFID2

  SERIAL.begin(115200);
  Serial1.begin(115200); // RFID 1
  myservo1.attach(A2);
  myservo2.attach(A3);

  delay(3000); // prevents usb driver crash on startup, do not omit this
  weightscaleInit(default_scalefactor);
  SERIAL.println("");
  SERIAL.println("******************************");
  SERIAL.println("        Program start         ");
  SERIAL.println("******************************");
  SERIAL.flush();

  // Set the led the rtos will blink when we have a fatal rtos error
  // RTOS also Needs to know if high/low is the state that turns on the led.
  // Error Blink Codes:
  //    3 blinks - Fatal Rtos Error, something bad happened. Think really hard about what you just changed.
  //    2 blinks - Malloc Failed, Happens when you couldn't create a rtos object.
  //               Probably ran out of heap.
  //    1 blink  - Stack overflow, Task needs more bytes defined for its stack!
  //               Use the taskMonitor thread to help gauge how much more you need
  vSetErrorLed(ERROR_LED_PIN, ERROR_LED_LIGHTUP_STATE);

  // sets the serial port to print errors to when the rtos crashes
  // if this is not set, serial information is not printed by default
  vSetErrorSerial(&SERIAL);

  // Create the threads that will be managed by the rtos
  // Sets the stack size and priority of each task
  // Also initializes a handler pointer to each task, which are important to communicate with and retrieve info from tasks
  xTaskCreate(threadServo, "Task Servo", 256, NULL, tskIDLE_PRIORITY + 3, &Handle_Servo);
  xTaskCreate(threadReadRFID1, "Task RFID1", 256, NULL, tskIDLE_PRIORITY + 2, &Handle_RFID1);
  xTaskCreate(threadReadRFID2, "Task RFID2", 256, NULL, tskIDLE_PRIORITY + 2, &Handle_RFID2);
  // xTaskCreate(taskMonitor, "Task Monitor", 256, NULL, tskIDLE_PRIORITY + 1, &Handle_monitorTask);

  // Start the RTOS, this function will never return and will schedule the tasks.
  vTaskStartScheduler();

  // error scheduler failed to start
  // should never get here
  while (1)
  {
    SERIAL.println("Scheduler Failed! \n");
    SERIAL.flush();
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
  //SERIAL.print("."); // print out dots in terminal, we only do this when the RTOS is in the idle state
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  delay(1000); // delay is interrupt friendly, unlike vNopDelayMS
}

//*****************************************************************


void SERCOM0_Handler()
{
    mySerial2.IrqHandler();
}

void Sendcommand2(uint8_t com_nub)
{
  uint8_t b = 0;
  while (RFID_cmdnub[com_nub][b] != 0x7E)
  {
    mySerial2.write(RFID_cmdnub[com_nub][b]);
    // Serial.print(" 0x");
    // Serial.print(RFID_cmdnub[com_nub][b], HEX);
    b++;
  }
  mySerial2.write(0x7E);
  mySerial2.write("\n\r");
}

void rfidInit2()
{
  const byte setPower[] = {0xBB, 0x00, 0xB6, 0x00, 0x02, 0x0A, 0x28, 0xEA, 0x7E}; // 26dBm
  // const byte setPower[] = {0xBB, 0x00, 0xB6, 0x00, 0x02, 0x07, 0xD0, 0x8F, 0x7E}; //20dBm
  // const byte setPower[] = {0xBB, 0x00, 0xB6, 0x00, 0x02, 0x07, 0x3A, 0xF9, 0x7E}; //18.5dBm
  //  const byte setPower[] = {0xBB, 0x00, 0xB6, 0x00, 0x02, 0x06, 0x0E, 0xCC, 0x7E}; //15.5dBm
  mySerial2.write(setPower, sizeof(setPower));
  // Serial.println("set power to 26 dBm");
}

String Readcallback2(String finalTags)
{
  uint8_t DATA_I_NUB = 0, numberOfTags = 0, tags = 0;
  uint8_t DATA_I[256] = {0};

  while (mySerial2.available())
  {
    DATA_I[DATA_I_NUB] = mySerial2.read();
    // Serial.print(DATA_I[DATA_I_NUB], HEX);
    // Serial.print(",");

    if (DATA_I[DATA_I_NUB] == 0x7e)
    {
      if ((DATA_I_NUB == 23) == (DATA_I_NUB == 19)){ // if not enough characters, skipp this tag
          DATA_I_NUB = 0;
          //Serial.println("Skipped tag");
          continue;
      }
      tags++;
      DATA_I_NUB = 0;

      // Serial.println("do some data processing");

      String tagID = getTagfromHEXv2(DATA_I);
      //Serial.println((String) "tagID: " + tagID);

      numberOfTags = finalTags.length() / 8;

      finalTags = keepIndividuals(finalTags, numberOfTags, tagID, 1);
      // Serial.println((String) "finalTags: " + finalTags);

      continue;
    }
    DATA_I_NUB++;
  }
  // myDelayMs(100);
  // Serial.println((String) "nothing in Buffer");

  return finalTags;
}

void oledInit(){
  Wire.begin();
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.clearDisplay();
  display.setTextSize(2);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner
  display.println("Wesahre");
  display.println("...loading");
  display.display();
}


void weightscaleInit(float SCALEFACTOR){
    scale.begin(DT_PIN, SCK_PIN);
    scale.set_scale(SCALEFACTOR);
    scale.tare();
}