#include <Arduino.h>

#include "myconfig.h"
#include <FlashStorage_SAMD.h>
#include <SPI.h>
#include <Wire.h>
#include "HX711.h"
#include <WiFiNINA.h>
#include "UST_RFID.h"
#include "arduino_secrets.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#ifdef DEBUG
#define debug(x) Serial.print(x)
#define debugln(x) Serial.println(x)
#else
#define debug(x)
#define debugln(x)
#endif

void waitSerial(uint16_t waittime);
void displaySavedData();
void saveScaleFactor(String scaleFactor);
void saveWifi(String SSID, String PWD);
void weightscaleInit(float SCALEFACTOR);
void eepromInit();
void wifiInit();
String buildData(String id, String timestamp, String weight, String unitinfo);
void httpRequest(String data);
void printWifiStatus();
void oledInit();

const int WRITTEN_SIGNATURE = 0xBEEFDEED;

typedef struct
{
  char SSID[30];
  char PWD[30];
  char SCALEFACTOR[10];
} EEPROM_data;

int signature;
uint16_t storedAddress = 0;
EEPROM_data e_data;

String data;
char d1;
String x;
float scalefactor;

//basic setting of hardware

//WEIGHT SENSOR
#ifdef FOUR_HX711
const int LOADCELL1_DOUT_PIN = 2;
const int LOADCELL1_SCK_PIN = 3;
const int LOADCELL2_DOUT_PIN = 4;
const int LOADCELL2_SCK_PIN = 5;
const int LOADCELL3_DOUT_PIN = 6;
const int LOADCELL3_SCK_PIN = 7;
const int LOADCELL4_DOUT_PIN = 8;
const int LOADCELL4_SCK_PIN = 9;
HX711 scale1;
HX711 scale2;
HX711 scale3;
HX711 scale4;
#else
const int DT_PIN = A0;
const int SCK_PIN = A1;
HX711 scale;
#endif

//Timer
unsigned long looptime, noWifiTime = 0;
bool firstNotConnected = false;
//screen
// On an arduino UNO:       A4(SDA), A5(SCL)
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3c ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//Wifi
///////please enter your sensitive data in the Secret tab/arduino_secrets.h

int keyIndex = 0;            // your network key index number (needed only for WEP)

int status = WL_IDLE_STATUS;

// Initialize the WiFi client library
WiFiClient client;
#define url "/lab/hwinfo"


void setup() {
  randomSeed(analogRead(0));

  Serial1.begin(115200); //for RFID
  Serial.begin(115200);
  //Serial.setTimeout(500);
  oledInit();

  pinMode (REDLED, OUTPUT); // Red LED
  pinMode (YELLOWLED, OUTPUT); // Yellow LED
  pinMode (GREENLED, OUTPUT); // Green LED
  pinMode (BUZZER, OUTPUT); 
  digitalWrite(GREENLED, HIGH);
  //pinMode(LED_BUILTIN,OUTPUT);
  //digitalWrite(LED_BUILTIN, LOW);
  //waitSerial(wait_for_Serial);
  debugln("Serial port ready");
  eepromInit();
  
  rfidInit();
  debugln("RFID ready");
  weightscaleInit(atof(e_data.SCALEFACTOR));
  debugln("weight scale ready");
  wifiInit();
  debugln("WeShare RFID reader is ready!");
}

void loop() {
  debugln("beginn of main loop");
  looptime = millis();
  
  //Weight scale
  #ifdef FOUR_HX711
    float weight1 = scale1.get_units(5);
    float weight2 = scale2.get_units(5);
    float weight3 = scale3.get_units(5);
    float weight4 = scale4.get_units(5);
    float weight = weight1 + weight2 + weight3 + weight4;
  #else
    float weight = scale.get_units(5);
  #endif


  //float weight = random(300); //dummy weight data
  debugln((String)"weight: " + weight);
  //delay(500);

  //RFID
  //Scan RFID
  uint8_t numberOfTags = 0;
  String finalTags = multiscan_v2(&numberOfTags, TIMEOUT);
  //RFID END

  //send to GUI
  Serial.println((String)"w" + weight); //send weight to GUI
  Serial.println((String)"n" + numberOfTags); //send numberOfTags to GUI
  Serial.println((String)"f" + finalTags); //send finalTags to GUI

    // check for added/removed tags
  static uint8_t prev_numberOfTags = 0;
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


//Midterm Task case1
//if any tag has different first two digit values with others, warning == 1 and red light on
//e.g. 0100111d and 02001123 will turn on the red light, 0000111d and 00001123 will not
  int warning = 0;
  for (int i=0 ; i<numberOfTags*8 ; i+=8)
  {
   String group = finalTags.substring(i,i+2);
  for (int j=0 ; j<numberOfTags*8 ; j+=8)
  {
   String temp_group = finalTags.substring(j,j+2);
     if (group != temp_group)
     {
      warning = 1;
      //Serial.println("Two chemicals cannot be put together!!!");
     }
   }
   }

  if (warning == 1)
  {
  digitalWrite (REDLED, HIGH); // Red On
  digitalWrite (BUZZER, HIGH); // Red On
  // digitalWrite (11, HIGH); // Yellow On
 }
  if (warning == 0)
 {
  digitalWrite (REDLED, LOW); // Red Off
  digitalWrite (BUZZER, LOW); // Red On
  // digitalWrite (11, LOW); // Yellow Off
  }
//END Midterm Task case1


  //Add comma and <"> between Tags 
  uint8_t i = 0;
  String scanresult;
  while(i<numberOfTags){
    scanresult += "\"" + finalTags.substring(8 * i,8 + 8 * i) + "\",";
    i++;
  }
  int length = scanresult.length();
  scanresult.remove(length - 1); //remove the last comma
  
  debugln(scanresult);

  
  debugln((String)"number of Tags: " + numberOfTags);
  debugln("finished rfid");
  //END of RFID


  // get response form wifi
  //while (!client.available()){}
  #ifdef DEBUG
  while (client.available()) {
    char c = client.read();
    Serial.write(c);
  }
  #endif

  
  //Send Data through Wifi  
  String timestamp = "1";
  String data = buildData(scanresult, timestamp, (String)weight, unitinfo);
  //Serial.println((String)"payload: " + data);
  
  //check connection and connect to wifi if duration has passed
  if (status == WL_CONNECTED){ //only send data if connected to wifi
  httpRequest(data);
  }
  else{
    if(!firstNotConnected){ //save time when first-time not connected to wifi
      debugln("fist time not connected");
      firstNotConnected = true;
      noWifiTime = millis();
    }
    if (millis() - noWifiTime > 1000*10*4){ //4 min
      noWifiTime = millis();
      wifiInit();
    } 
  }
  

  digitalWrite (GREENLED, !digitalRead(GREENLED));

   
  //END of Send Data through Wifi  



  //GUI
  if(Serial.available()){
    String incomingString = Serial.readStringUntil('\n');
    d1 = incomingString.charAt(0);
    debugln(d1);
    debugln(incomingString);

    if(d1 == 'A'){
      digitalWrite(LED_BUILTIN, HIGH);
    }
    else if(d1 == 'a'){
      digitalWrite(LED_BUILTIN, LOW);
    }
    else if(d1 == 'C'){ //connect was clicked
      digitalWrite(LED_BUILTIN, HIGH);     
      displaySavedData();      
    }

    else if(d1 == 'S'){//save wifi was clicked
      x = incomingString.substring(1);
      uint8_t xlen = x.length();
      for(uint8_t i = 0; i<xlen; i++){
        if(x.substring(i,i+2) == ":;"){
          for(uint8_t j = i+2; j<xlen; j++){
            if(x.substring(j,j+2) == ":;"){
            String pwd = x.substring(i+2, j); //"x.length()-1"  removes the "new line character"
            String ssid = x.substring(0,i);
            debugln((String)"save wifi ssid: " + ssid);
            debugln((String)"save wifi pwd: " + pwd);
            saveWifi(ssid, pwd);
            displaySavedData();
            //change to other wifi
            WiFi.end(); //todo: test
            status = WL_IDLE_STATUS;
            debugln((String)"WL_IDLE_STATUS: " + WL_IDLE_STATUS);
            wifiInit();
            }
          }
        }
      }
    }  

    else if(d1 == 'F'){//save scalefactor was clicked
      x = incomingString.substring(1);
      scalefactor = x.toFloat();
      #ifdef FOUR_HX711
        scale1.set_scale(scalefactor);
        scale2.set_scale(scalefactor);
        scale3.set_scale(scalefactor);
        scale4.set_scale(scalefactor);
      #else
        scale.set_scale(scalefactor);
      #endif
      saveScaleFactor(x);
      displaySavedData();
    }
   
    else if(d1 == 't'){//tare was clicked
    #ifdef FOUR_HX711
      scale1.tare();
      scale2.tare();
      scale3.tare();
      scale4.tare();
    #else
      scale.tare();
    #endif
    }
    
  }
  //END of GUI


  // Print ellapsed time per loop
  looptime = millis()-looptime;
  debugln((String)"Elapsed time per loop: " + looptime);

  //delay(1000);
}


//private functions ******************************************************************

void waitSerial(uint16_t waittime)
{
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.print((String)"Wait "+ waittime/1000 + " s for Serial...");
  display.display();

  //waits for Serial Port or the wiattime has elapsed
  long prev_time = millis();
  long tim = 0;
  while (!Serial)
  {
    tim = millis() - prev_time;
    if (tim > waittime)
      break;
  };
}

void displaySavedData(){
  Serial.print("W");
  Serial.println(e_data.SSID);
  Serial.print("F");
  Serial.println(e_data.SCALEFACTOR);
}

void saveScaleFactor(String scaleFactor){
  EEPROM.put(storedAddress, WRITTEN_SIGNATURE);

  String SCALEFACTOR = scaleFactor;

  // Fill the "e_data" structure with the data entered by the user...
  SCALEFACTOR.toCharArray(e_data.SCALEFACTOR, 10);

  // ...and finally save everything into emulated-EEPROM
  EEPROM.put(storedAddress + sizeof(signature), e_data);

  if (!EEPROM.getCommitASAP())
  {
    Serial.println("CommitASAP not set. Need commit()");
    EEPROM.commit();
  }    
}

void saveWifi(String SSID, String PWD){
  EEPROM.put(storedAddress, WRITTEN_SIGNATURE);

  //String SSID = "LSK 7028";
  //String PWD = "wesharetechnology";

  // Fill the "e_data" structure with the data entered by the user...
  SSID.toCharArray(e_data.SSID, 30);
  PWD.toCharArray(e_data.PWD, 30);

  // ...and finally save everything into emulated-EEPROM
  EEPROM.put(storedAddress + sizeof(signature), e_data);

  if (!EEPROM.getCommitASAP())
  {
    Serial.println("CommitASAP not set. Need commit()");
    EEPROM.commit();
  }
}

void weightscaleInit(float SCALEFACTOR){
  #ifdef FOUR_HX711
    scale1.begin(LOADCELL1_DOUT_PIN, LOADCELL1_SCK_PIN);
    scale2.begin(LOADCELL2_DOUT_PIN, LOADCELL2_SCK_PIN);
    scale3.begin(LOADCELL3_DOUT_PIN, LOADCELL3_SCK_PIN);
    scale4.begin(LOADCELL4_DOUT_PIN, LOADCELL4_SCK_PIN);
    scale1.set_scale(SCALEFACTOR);                      // this value is obtained by calibrating the scale with known weights; see the README for details
    scale1.tare();				                              // reset the scale to 0
    scale2.set_scale(SCALEFACTOR);                      
    scale2.tare();				                            
    scale3.set_scale(SCALEFACTOR);                     
    scale3.tare();				                                
    scale4.set_scale(SCALEFACTOR);                      
    scale4.tare();
  #else
    scale.begin(DT_PIN, SCK_PIN);
    scale.set_scale(SCALEFACTOR);
    scale.tare();
  #endif

}

void eepromInit(){ //safes default values to eeprom if empty
  EEPROM.get(storedAddress, signature);

  // If the EEPROM is empty then no WRITTEN_SIGNATURE
  if (signature == WRITTEN_SIGNATURE) //if something is in Flash
  {
    EEPROM.get(storedAddress + sizeof(signature), e_data);
    displaySavedData();  
  }
  else //safe default settings to eeprom if empty
  {
    EEPROM.put(storedAddress, WRITTEN_SIGNATURE);

    String SSID = SECRET_SSID; //"LSK 7028";
    String PWD = SECRET_PASS; //"wesharetechnology";
    String SCALEFACTOR = default_scalefactor;

    // Fill the "e_data" structure with the data entered by the user...
    SSID.toCharArray(e_data.SSID, 30);
    PWD.toCharArray(e_data.PWD, 30);
    SCALEFACTOR.toCharArray(e_data.SCALEFACTOR, 10);

    // ...and finally save everything into emulated-EEPROM
    EEPROM.put(storedAddress + sizeof(signature), e_data);

    if (!EEPROM.getCommitASAP())
    {
      Serial.println("CommitASAP not set. Need commit()");
      EEPROM.commit();
    }
  }
}


String buildData(String id, String timestamp, String weight, String unitinfo) { //组建发送的json字符串
  String payload = "{\"unitinfo\":\"";
  payload += unitinfo;
  payload += "\",\"data\":{\"weight\":\"";
  payload += weight;
  payload += "\",\"uids\":[";
  payload += id;
  payload += String("]}}");
  //Serial.println((String)"payload: " + payload);
  return payload;
  //  char attributes[100];
  //  payload.toCharArray( attributes, 100 );
}




// this method makes a HTTP connection to the server:
void httpRequest(String data) { 
  Serial.println("httpRequest");
  //Serial.println((String)"data" + data);
  // close any connection before send a new request.
  // This will free the socket on the NINA module
  client.stop();

  // if there's a successful connection:
  if (client.connect(server, 8000)) {
    Serial.println("connecting...");

    // send the HTTP GET request:
    client.println((String)"POST " + url + " HTTP/1.0");
    client.println((String)"Host: " + serverno);
    client.println((String)"Accept: */*");
    client.println((String)"Content-Length: " + data.length());
    client.println("Content-Type: application/json");
    client.println();
    client.println(data);
    
    //Serial.println((String)"postRequest: " + postRequest);
    Serial.println((String)"Data:" + data);
    Serial.flush();

    digitalWrite(YELLOWLED, LOW);
    
//    client.println(postRequest);
//    client.println("Connection: close");
//    client.println();

  } else {
    // if you couldn't make a connection:
    Serial.println("connection failed");
    status = WL_IDLE_STATUS;
    WiFi.end();
    digitalWrite(YELLOWLED, HIGH);
  }
}


void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}


void wifiInit(){
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

// check for firmware update
//  String fv = WiFi.firmwareVersion();
//  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
//    Serial.println("Please upgrade the firmware");
//  }

  long prev_time = millis();
  long tim = 0;
  bool con = true;
  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(e_data.SSID);
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println((String)"trying to connect to: "+ e_data.SSID);
    display.print((String)"PWD: "+ e_data.PWD);
    display.display();
    debugln((String)"WifiInit SSID: " + e_data.SSID);
    debugln((String)"WifiInit PWD: " + e_data.PWD);

    status = WiFi.begin(e_data.SSID, e_data.PWD);

    // wait 10 seconds for connection:
    delay(10000);
    tim = millis() - prev_time;
    if (tim > 20000){ //0.5 min
      con = false;
      break;
    }
  }
  // you're connected now, so print out the status:
  printWifiStatus();
  if (con){ //could connect
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(0, 0);
    display.println("Connected!");
    display.display();
    digitalWrite(YELLOWLED,LOW);    
    //send to GUI
    Serial.println((String)"A"); //succesfully connectto Wifi, send to GUI
  }
  else{ //connection failed
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(0, 0);
    display.println("Could not connect to Wifi");
    display.display();
    debugln("could not connect to wifi");
    digitalWrite(YELLOWLED,HIGH);
    //send to GUI
    Serial.println((String)"a"); //could not connect to Wifi, send to GUI
  }
  firstNotConnected = false;
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