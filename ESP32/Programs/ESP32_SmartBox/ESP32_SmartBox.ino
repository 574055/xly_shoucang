#include <Arduino.h>
#include "UST_RFID.h"
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define RFID1_EN 5

//RFID
static uint8_t prev_numberOfTags = 0; 
static String prev_finalTags = "";
String finalTags1 = "";
uint8_t numberOfTags =0;


#define DHTTYPE DHT11
#define DHTPIN 4
DHT dht(DHTPIN, DHTTYPE);//DHT11 setup

#define SCREEN_WIDTH 128    // OLED display width, in pixels
#define SCREEN_HEIGHT 32    // OLED display height, in pixels
#define OLED_RESET -1       // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3c ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  randomSeed(analogRead(0));

  Serial.begin(115200);
  Serial1.begin(115200, SERIAL_8N1, 16, 17);//RX TX

  pinMode (RFID1_EN, OUTPUT); 
  digitalWrite(RFID1_EN,HIGH);
  rfidInit();  

  dht.begin();//DHT init

  oledInit();

}

void loop() {
  USERID="";

  finalTags1 = multiscan_v2(&numberOfTags, TIMEOUT);
  numberOfTags = finalTags1.length() / 8;
  Serial.print((String)"Found:" + finalTags1+'\t');
  Serial.println((String)"Number" + numberOfTags+'\t');
  String addedTags = tagsChanged(prev_finalTags, prev_numberOfTags, finalTags1, numberOfTags);
  String removedTags = tagsChanged(finalTags1, numberOfTags, prev_finalTags, prev_numberOfTags);
  prev_finalTags = finalTags1;
  prev_numberOfTags = numberOfTags;

  int h1 = dht.readHumidity();
  float t1 = dht.readTemperature();
  if(isnan(t1))
  {
    t1=-100.00;
  }
  if(isnan(h1))
  {
    h1=-1.00;
  }
  Serial.print((String) "Temp:" + '\t' + t1); // send numberOfTags to GUI
  Serial.print((String) "Humi:" + '\t' + h1); // send numberOfTags to GUI

  // display on screen
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 0);
  //display.println((String)average_last + " kg");

  display.print(t1,1);
  display.print((char)247);
  display.print("C ");
  display.println((String)h1 + "%");

  if(USERID=="")
  {
    display.println("NULL");
  }
  else if(USERID.length()<10)
  {
    display.setTextSize(2);
    display.println((String)USERID);
  }
  else
  {
    display.setTextSize(1);
    display.println((String)USERID);
    
  }

  display.display();


}


//private function
//****************************************************


void oledInit()
{
  Wire.begin();
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
  {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // Don't proceed, loop forever
  }
  display.clearDisplay();
  display.setTextSize(2);              // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 0);             // Start at top-left corner
  display.println("Wesahre");
  display.println("...loading");
  display.display();
}