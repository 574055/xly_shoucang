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
#include "wiring_private.h"
#include "SD_CARD.h"
#include <SimpleKalmanFilter.h>

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <ArduinoUniqueID.h>

// #define DEBUG //uncoment when to enable debugg messages
#ifdef DEBUG
#define debug(x) Serial.print(x)
#define debugln(x) Serial.println(x)
#else
#define debug(x)
#define debugln(x)
#endif

void waitSerial(uint16_t waittime);
void displaySavedData();
//void saveScaleFactor(String scaleFactor);
void saveWifi(String SSID, String PWD);
//void weightscaleInit(float SCALEFACTOR);
void eepromInit();
void wifiInit();
String buildData(String id, String timestamp, String unitinfo,String temp, String Humi);
void httpRequest(String data);
void printWifiStatus();
void oledInit();
void midtermTask(uint8_t numberOfTags, String finalTags);

void SERCOM0_Handler();
//void rfidInit2();
//String multiscan2_v2(uint8_t *numberOfTags, unsigned long timeout);

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
char d1;//terminal input
String x;
float scalefactor;

String USERID;
String LastUSER;




// Timer
unsigned long looptime, noWifiTime = 0;
bool firstNotConnected = false;
// screen
//  On an arduino UNO:       A4(SDA), A5(SCL)
#define SCREEN_WIDTH 128    // OLED display width, in pixels
#define SCREEN_HEIGHT 32    // OLED display height, in pixels
#define OLED_RESET -1       // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3c ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//Uart mySerial(&sercom0, 5, 6, SERCOM_RX_PAD_1, UART_TX_PAD_0);

// Wifi
///////please enter your sensitive data in the Secret tab/arduino_secrets.h

int keyIndex = 0; // your network key index number (needed only for WEP)

int status = WL_IDLE_STATUS;

// Initialize the WiFi client library
WiFiClient client;
#define url "/lab/hwinfo"

#define DHTTYPE DHT11
#define DHTPIN 11
DHT dht(DHTPIN, DHTTYPE);//DHT11 setup

int UID[100];//UniqueID

//NFC
String nfcresult;
String NFC_ID;
//const byte SetSerial[]={0x03, 0x08, 0xC7, 0x20, 0x07, 0x00, 0x00, 0x14}; set baud rate to 115200
const byte ReadingCommand[]={0x01, 0x08, 0xA1, 0x20, 0x00, 0x01, 0x00, 0x76};
Uart mySerial(&sercom0, 5, 6, SERCOM_RX_PAD_1, UART_TX_PAD_0);//NFC pin RX, TX

//Human sensor
int HumanRead = 0;
int NFCshow = 0;
int loopCount = 0;

void setup()
{
  randomSeed(analogRead(0));

  pinMode(4, INPUT);//Human sensor
  pinMode(3,OUTPUT);//Buzzer
  digitalWrite(3,LOW);

  pinPeripheral(5, PIO_SERCOM_ALT);
  pinPeripheral(6, PIO_SERCOM_ALT);

  mySerial.begin(9600);//for NFC

  Serial1.begin(115200); // for RFID
  Serial.begin(115200);
  delay(1000);
  //mySerial.write(SetSerial,sizeof(SetSerial));
  // while (!Serial) {
  //   ; // wait for Serial port to connect. Needed for native USB port only. ONLY FOR DEBUGGING PURPOSE!!!!!
  // }

  oledInit();

  pinMode(RFID1_EN, OUTPUT);
  //pinMode(RFID2_EN, OUTPUT);
  digitalWrite(RFID1_EN, HIGH); // turn on RFID

  waitSerial(wait_for_Serial);
  debugln("Serial port ready");

  //SDInit();
  eepromInit();

  rfidInit();
  //rfidInit2();
  debugln("RFID ready");
  wifiInit();
  debugln("WeShare RFID reader is ready!");
// #ifdef WEIGHT_SCALE
//   weightscaleInit(atof(e_data.SCALEFACTOR));
// #endif
  debugln("weight scale ready");

    dht.begin();//DHT init
}

void loop()
{
  if(loopCount == 1000){
    NFCshow = 0;
  }
  HumanRead = digitalRead(4);

  debugln("beginn of main loop");
  looptime = millis();
  USERID="";

  int h1 = dht.readHumidity();
  float t1 = dht.readTemperature();
  if(isnan(t1))//if reading = NA, correct to error code
  {
    t1=-100.00;
  }
  if(isnan(h1))
  {
    h1=-1.00;
  }
  Serial.print((String) "Temp:" + '\t' + t1); // send numberOfTags to GUI
  Serial.print('\t');
  Serial.print((String) "Humi:" + '\t' + h1); // send numberOfTags to GUI
  Serial.print('\t');

    //NFC
  nfcresult="";
  NFC_ID="";
  char reading[25]={'\0'};

  mySerial.write(ReadingCommand,sizeof(ReadingCommand));
  //delay(2);

  if(mySerial.available())
  {
    for(int i = 0 ; i<25 ; i++)
    {
      //Serial.readString()
      reading[i]=mySerial.read();
      //Serial.print(reading[i],HEX);
      //Serial.print('\t');
      nfcresult += String(reading[i],HEX);
      
    }
    nfcresult[19]='\0';
  }
  Serial.print((String)"NFC reading:");
  //Serial.print(nfcresult);

  if(nfcresult.substring(0,4)=="1ca1")
  {
    NFC_ID=nfcresult.substring(9,17);
    Serial.print(NFC_ID);
  }
  // end of NFC

      // display on screen 1
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 0);
  //display.println((String)average_last + " kg");

  if(NFC_ID=="")
  {
  display.print(t1,1);
  display.print((char)247);
  display.print("C ");
  display.println((String)h1 + "%");
  }
  else{
    display.println((String)"Card Found");
  }

  if(NFC_ID != ""){//human warning
    NFCshow = 1;
    loopCount = 0;
  }

  if ( LastUSER == "" && NFCshow == 0){//human warning
    digitalWrite(3, LOW);
  }
  else if (LastUSER != "" && NFCshow == 1){
    digitalWrite(3, LOW);
  }
  else {
    Serial.println("\nWarning!!!" + (String)HumanRead);
    digitalWrite(3, HIGH);
  }

    if (LastUSER == "")
    {
      display.println("NULL");
    }
    else if (LastUSER.length() < 10)
    {
      display.setTextSize(2);
      display.println((String)LastUSER);
    }
    else
    {
      display.setTextSize(1);
      display.println((String)LastUSER);
    }

  display.display();




  // RFID
  // Scan RFID1
  uint8_t numberOfTags1 = 0;
  String finalTags = multiscan_v2(&numberOfTags1, TIMEOUT);
  debugln("RFID1:");
  debugln((String)numberOfTags1); // send numberOfTags to GUI
  debugln((String)finalTags);    // send finalTags to GUI

  

  // mySerial RFID2
  // uint8_t numberOfTags2 = 0;
  // String finalTags2 = multiscan2_v2(&numberOfTags2, TIMEOUT);
  // debugln("RFID2:");
  // debugln((String)numberOfTags2); // send numberOfTags to GUI
  // debugln((String)finalTags2);    // send finalTags to GUI

  // data processing
  //String finalTags = keepIndividuals(finalTags1, numberOfTags1, finalTags2, numberOfTags2);
  uint8_t numberOfTags = finalTags.length() / 8;
  static uint8_t prev_numberOfTags = 0;
  debugln("TOTAL:");
  //Serial.println((String) "w" + average);       // send weight to GUI
  Serial.print('\t'+ (String)"RFID reading:");
  Serial.print((String) "n" + numberOfTags); // send numberOfTags to GUI
  Serial.print('\t');
  Serial.print((String) "f" + finalTags);    // send finalTags to GUI
  Serial.print('\t');

  // check for added/removed tags
  static String prev_finalTags = "";
  String addedTags = tagsChanged(prev_finalTags, prev_numberOfTags, finalTags, numberOfTags);
  String removedTags = tagsChanged(finalTags, numberOfTags, prev_finalTags, prev_numberOfTags);
  //Serial.println((String) "d" + addedTags);   // send numberOfTags to GUI
  //Serial.println((String) "r" + removedTags); // send finalTags to GUI

  prev_finalTags = finalTags;
  prev_numberOfTags = numberOfTags;
  //end of RFID reader

  // //NFC
  // nfcresult="";
  // NFC_ID="";
  // char reading[20]={'\0'};

  // mySerial.write(ReadingCommand,sizeof(ReadingCommand));

  // if(mySerial.available())
  // {
  //   for(int i = 0 ; i<20 ; i++)
  //   {
  //     //Serial.readString()
  //     reading[i]=mySerial.read();
  //     //Serial.print(reading[i],HEX);
  //     //Serial.print('\t');
  //     nfcresult += String(reading[i],HEX);
      
  //   }
  //   nfcresult[19]='\0';
  // }
  // Serial.print("NFC reading:");
  // //Serial.print(nfcresult);

  // if(nfcresult.substring(0,4)=="1ca1")
  // {
  //   NFC_ID=nfcresult.substring(9,17);
  //   Serial.print(NFC_ID);
  // }

  Serial.println("");


    // display on screen 2
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 0);
  //display.println((String)average_last + " kg");

  if(NFC_ID=="")
  {
  display.print(t1,1);
  display.print((char)247);
  display.print("C ");
  display.println((String)h1 + "%");
  }
  else{display.println((String)"Card Found");}

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
  LastUSER=USERID;





  // Add comma and <"> between Tags
  uint8_t i = 0;
  String scanresult;
  while (i < numberOfTags)
  {
    scanresult += "\"" + finalTags.substring(8 * i, 8 + 8 * i) + "\",";
    i++;
  }
  int length = scanresult.length();
  scanresult.remove(length - 1); // remove the last comma

  debugln(scanresult);

  debugln((String) "number of Tags: " + numberOfTags);
  debugln("finished rfid");
// END of RFID

// get response form wifi
// while (!client.available()){}
#ifdef DEBUG
  while (client.available())
  {
    char c = client.read();
    Serial.write(c);
  }
#endif

  // Send Data through Wifi
  String timestamp = "1";
  String data = buildData(scanresult, timestamp, unitinfo,(String)t1, (String)h1);//
  // Serial.println((String)"payload: " + data);

  // check connection and connect to wifi if duration has passed
  if (status == WL_CONNECTED)
  { // only send data if connected to wifi
    httpRequest(data);
  }
  else
  {
    if (!firstNotConnected)
    { // save time when first-time not connected to wifi
      debugln("fist time not connected");
      firstNotConnected = true;
      noWifiTime = millis();
    }
    if (millis() - noWifiTime > 1000 * 10 * 5)
    { // 5 min
      noWifiTime = millis();

      wifiInit();
    }
  }

//
  //digitalWrite(GREENLED, !digitalRead(GREENLED));

  if (status != WL_CONNECTED)
  {
    writeSD(data);
  }

  // GUI
  if (Serial.available())
  {
    String incomingString = Serial.readStringUntil('\n');
    d1 = incomingString.charAt(0);
    debugln(d1);
    debugln(incomingString);

    if (d1 == 'A')
    {
      digitalWrite(LED_BUILTIN, HIGH);
      debugln("led on");
    }
    else if (d1 == 'a')
    {
      digitalWrite(LED_BUILTIN, LOW);
      debugln("led off");
    }
    else if (d1 == 'C')
    { // connect was clicked
      digitalWrite(LED_BUILTIN, HIGH);
      displaySavedData();
    }

    else if (d1 == 'S')
    { // save wifi was clicked
      debugln("save wifi");
      x = incomingString.substring(1);
      debugln((String) "incoming String: " + x);
      uint8_t xlen = x.length();
      debugln((String) "xlen: " + xlen);
      for (uint8_t i = 0; i < xlen; i++)
      {
        if (x.substring(i, i + 2) == ":;")
        {
          // for(uint8_t j = i+2; j<xlen; j++){

          // if(x.substring(j,j+2) == ":;"){
          String pwd = x.substring(i + 2); //"x.length()-1"  removes the "new line character"
          pwd.trim();
          String ssid = x.substring(0, i);
          ssid.trim();
          debugln((String) "save wifi ssid: " + ssid);
          debugln((String) "save wifi pwd: " + pwd);
          saveWifi(ssid, pwd);
          displaySavedData();
          // change to other wifi
          WiFi.end(); // todo: test
          status = WL_IDLE_STATUS;
          debugln((String) "WL_IDLE_STATUS: " + WL_IDLE_STATUS);
          wifiInit();
          //}
          //}
        }
      }
    }

    // else if(d1 == '+' || d1 == 'a'){
    //   calibration_factor += 50;
    //   saveScaleFactor((String)calibration_factor);
    //   displaySavedData();
    // }
    // else if(d1 == '-' || d1 == 'z'){
    //   calibration_factor -= 50;
    //   saveScaleFactor((String)calibration_factor);
    //   displaySavedData(); 
    // }
    
    // else if(d1 == 'r')//use computer to reset
    // {
    //   resetfunc();
    // }

    // else if (d1 == 't')
    // { // tare was clicked
    //   debugln("tare ");
    //   scale.tare(10);
    //   SD.remove("reset.txt"); // delete reset.txt, so only last offset will be saved
    //   writeSD_reset((String)scale.get_offset());

    // }
  }
  // END of GUI

  // Print ellapsed time per loop
  looptime = millis() - looptime;
  debugln((String) "Elapsed time per loop: " + looptime);

  loopCount++;
}

// private functions ******************************************************************



void waitSerial(uint16_t waittime)
{
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.print((String) "Wait " + waittime / 1000 + " s for Serial...");
  display.display();

  // waits for Serial Port or the wiattime has elapsed
  long prev_time = millis();
  long tim = 0;
  while (!Serial)
  {
    tim = millis() - prev_time;
    if (tim > waittime)
      break;
  };
}

void displaySavedData()
{
  Serial.print("W");
  Serial.println(e_data.SSID);
  Serial.print("F");
  Serial.println(e_data.SCALEFACTOR);
}

void saveScaleFactor(String scaleFactor)
{
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

void saveWifi(String SSID, String PWD)
{
  EEPROM.put(storedAddress, WRITTEN_SIGNATURE);

  // String SSID = "LSK 7028";
  // String PWD = "wesharetechnology";

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


void eepromInit()
{ // safes default values to eeprom if empty
  EEPROM.get(storedAddress, signature);

  // If the EEPROM is empty then no WRITTEN_SIGNATURE
  if (signature == WRITTEN_SIGNATURE) // if something is in Flash
  {
    EEPROM.get(storedAddress + sizeof(signature), e_data);
    displaySavedData();
  }
  else // safe default settings to eeprom if empty
  {
    EEPROM.put(storedAddress, WRITTEN_SIGNATURE);

    String SSID = SECRET_SSID; //"LSK 7028";
    String PWD = SECRET_PASS;  //"wesharetechnology";
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

String buildData(String id, String timestamp, String unitinfo, String temp, String Humi) { //组建发送的json字符串
  String payload = "{\"unitinfo\":\"";
  payload += unitinfo;

  // for(size_t i = 0; i < UniqueIDsize; i++)
  // {
  // UID[i]=UniqueID[i],HEX;
  // payload += String(UID[i]);
  // payload += " ";
  // }
  payload += "\",\"data\":{\"weight\":\"";
  payload += 0;
  payload += "\",\"uids\":[";
  payload += id;
  payload += "],\"temp\":\"";
  payload += temp;
  payload += "\",\"humi\":\"";
  payload += Humi;
  payload += "\",\"NFC\":\"";
  payload += NFC_ID;
  payload += String("\"}}");
  Serial.println((String)"payload: " + payload);
  return payload;
  //  char attributes[100];
  //  payload.toCharArray( attributes, 100 );
}

// this method makes a HTTP connection to the server:
void httpRequest(String data)
{
  Serial.print("httpRequest");
  // Serial.println((String)"data" + data);
  //  close any connection before send a new request.
  //  This will free the socket on the NINA module
  client.stop();

  // if there's a successful connection:
  if (client.connect(servername, 8000))
  //if (client.connect(server, 8000))
  {
    Serial.println("connecting...");

    // send the HTTP GET request:
    client.println((String) "POST " + url + " HTTP/1.0");
    client.println((String) "Host: " + serverno);
    client.println((String) "Accept: */*");
    client.println((String) "Content-Length: " + data.length());
    client.println("Content-Type: application/json");
    client.println();
    client.println(data);

    // Serial.println((String)"postRequest: " + postRequest);

    //Serial.println((String) "Data:" + data);//Print out the sent data
    Serial.flush();



    //    client.println(postRequest);
    //    client.println("Connection: close");
    //    client.println();
  }
  else
  {
    // if you couldn't make a connection:
    Serial.println("connection failed");
    status = WL_IDLE_STATUS;
    WiFi.end();
    // digitalWrite(BLUELED, HIGH);
  }
}

void printWifiStatus()
{
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

void wifiInit()
{
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE)
  {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true)
      ;
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
  while (status != WL_CONNECTED)
  {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(e_data.SSID);
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println((String) "trying to connect to: " + e_data.SSID);
    //display.print((String) "PWD: " + e_data.PWD);
    display.display();
    debugln((String) "WifiInit SSID: " + e_data.SSID);
    debugln((String) "WifiInit PWD: " + e_data.PWD);

    status = WiFi.begin(e_data.SSID, e_data.PWD);

    // wait 10 seconds for connection:
    //delay(10000);
    tim = millis() - prev_time;
    if (tim > 20000)
    { // 0.5 min
      con = false;
      break;
    }
  }
  // you're connected now, so print out the status:
  printWifiStatus();
  if (WiFi.RSSI())
  { // could connect
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(0, 0);
    display.println("Connected!");
    display.display();
    // digitalWrite(BLUELED, LOW);
    // send to GUI
    Serial.println((String) "A"); // succesfully connectto Wifi, send to GUI
  }
  else
  { // connection failed
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(0, 0);
    display.println("Could not connect to Wifi");
    display.display();
    debugln("could not connect to wifi");
    // digitalWrite(BLUELED, HIGH);
    // send to GUI
    Serial.println((String) "a"); // could not connect to Wifi, send to GUI
  }
  firstNotConnected = false;
}

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

void UserIDFind(String str){
  for (uint8_t i = 0; i < (str.length()/8); i++)
  {
    if (str.substring(i*8,i*8+1)=="a")
    {
      USERID+=str.substring(i*8,i*8+8);
    }
  }
}

void SERCOM0_Handler()
{
  mySerial.IrqHandler();
}
