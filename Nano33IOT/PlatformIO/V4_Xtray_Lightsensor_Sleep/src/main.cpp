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
#include "ArduinoLowPower.h"
#include <ArduinoOTA.h>

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
void saveScaleFactor(String scaleFactor);
void saveWifi(String SSID, String PWD);
void weightscaleInit(float SCALEFACTOR);
void eepromInit();
void wifiInit();
String buildData(String ver, String id, String timestamp, String weight, String unitinfo, String t1, String h1);
void httpRequest(String data);
void printWifiStatus();
void oledInit();
void midtermTask(uint8_t numberOfTags, String finalTags);

void SERCOM0_Handler();
void rfidInit2(int set);
void rfidLow2();
String multiscan2_v2(uint8_t *numberOfTags, unsigned long timeout);

void rfidsdread();
void makeVersion();
void httpHB(String data);
void wifiSleep();
String SleepBuild(String ver, String timestamp, String unitinfo);

void OTA_begin();

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

int LightReading=0;//This is the direct reading from xtray
int LightCounting=0;//If lightReading>LightThreshold, this++, and device will turn to sleepmode 
                    //when this reach a given number in Line. 254 
int LightThreshold=700;//This determines what value above is dark, and what value below is 
                       //bright. Please adjust it to fit different environments.
int RFIDLowed = 0;//true if RFID reader were turned to low power mode, used in sleepmode

// HX711
float calibration_factor = 22504; //50kg->22900, 5kg-->250000
float output;
int readIndex;
float total = 0;
float average = 0;
float average_last = 0;
const int cycles = 20;
float readings[cycles];



// basic setting of hardware

// WEIGHT SENSOR
#ifdef WEIGHT_SCALE
#ifdef FOUR_HX711
const int LOADCELL1_DOUT_PIN = 2;
const int LOADCELL1_WEIGHT_SCK_PIN = 3;
const int LOADCELL2_DOUT_PIN = 4;
const int LOADCELL2_WEIGHT_SCK_PIN = 5;
const int LOADCELL3_DOUT_PIN = 6;
const int LOADCELL3_WEIGHT_SCK_PIN = 7;
const int LOADCELL4_DOUT_PIN = 8;
const int LOADCELL4_WEIGHT_SCK_PIN = 9;
HX711 scale1;
HX711 scale2;
HX711 scale3;
HX711 scale4;
#else
const int DT_PIN = A0;
const int WEIGHT_SCK_PIN = A1;
HX711 scale;
#endif
#endif

// e_mea: Measurement Uncertainty - How much do we expect to our measurement vary
// e_est: Estimation Uncertainty - Can be initilized with the same value as e_mea since the kalman filter will adjust its value.
// q: Process Variance - usually a small number between 0.001 and 1 - how fast your measurement moves. Recommended 0.01. Should be tunned to your needs.
// SimpleKalmanFilter kf = SimpleKalmanFilter(e_mea, e_est, q);
SimpleKalmanFilter simpleKalmanFilter(4, 4, .025);

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

Uart mySerial(&sercom0, 5, 6, SERCOM_RX_PAD_1, UART_TX_PAD_0);

// Wifi
///////please enter your sensitive data in the Secret tab/arduino_secrets.h

int keyIndex = 0; // your network key index number (needed only for WEP)

int status = WL_IDLE_STATUS;

// Initialize the WiFi client library
WiFiClient client;
#define url "/lab/hwinfo"
#define urlHB "/lab/hwHB"

#define DHTTYPE DHT11
#define DHTPIN A3 //A3
DHT dht(DHTPIN, DHTTYPE);//DHT11 setup

//User ID filtering
String USERID;

// UID/Serial Number
int UIDtemp;
String UID;

//Heartbeat count:define how many loops to send a heartbeat package
int HBcount = 0;
int HBThread = 450;//Thread = 900, 1800sec, 30min. The time will be longer than the estimation
                   //as other acticities like wifi reconnection



void setup()
{
  randomSeed(analogRead(0));

  // Reassign pins 5 and 6 to SERCOM alt
  pinPeripheral(5, PIO_SERCOM_ALT);
  pinPeripheral(6, PIO_SERCOM_ALT);
  mySerial.begin(115200);

  Serial1.begin(115200); // for RFID
  Serial.begin(115200);
  // while (!Serial) {
  //   ; // wait for Serial port to connect. Needed for native USB port only. ONLY FOR DEBUGGING PURPOSE!!!!!
  // }

  oledInit();

  pinMode(RFID1_EN, OUTPUT);
  pinMode(RFID2_EN, OUTPUT);
  pinMode(A2,INPUT);
  digitalWrite(RFID1_EN, HIGH); // turn on RFID
  digitalWrite(RFID2_EN, HIGH); // turn on RFID
  pinMode(REDLED, OUTPUT);      // Red LED
  pinMode(BLUELED, OUTPUT);     // Yellow LED
  pinMode(GREENLED, OUTPUT);    // Green LED
  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, LOW);
  digitalWrite(GREENLED, HIGH);
  digitalWrite(REDLED, LOW);
  // pinMode(LED_BUILTIN,OUTPUT);
  // digitalWrite(LED_BUILTIN, LOW);
  // waitSerial(wait_for_Serial);
  debugln("Serial port ready");

  SDInit();
  eepromInit();


  //rfidInit(powerset);
  //rfidInit2(powerset);

  debugln("RFID ready");
  wifiInit();
  debugln("WeShare RFID reader is ready!");
#ifdef WEIGHT_SCALE
  weightscaleInit(atof(e_data.SCALEFACTOR));
#endif
  debugln("weight scale ready");

    dht.begin();//DHT init

    SD_Lowpower();//Use to reduce the power of SD module in the sleepmode
    rfidsdread();


    //Get UID
    UniqueIDdump(Serial);
    delay(2000);
    Serial.print("UniqueID: ");
    for (size_t i = 0; i < UniqueIDsize; i++)
    {
      if (UniqueID[i] < 0x10)
        Serial.print("0");
      Serial.print(UniqueID[i], HEX);
      Serial.print(" ");
      UIDtemp=UniqueID[i],HEX;
      UID+=String(UIDtemp,HEX);
    }
    Serial.println();
    Serial.println((String)"U" + UID);
    makeVersion();

  // OTA init
  OTA_begin();
  Serial.println("Finished setup");
}
  

void loop()
{
  USERID="";//reset USERID



  LightReading=analogRead(A2);//read light sensor data
  Serial.println("L" + (String)LightReading);
  //LightReading=100;/fixed number to disenable sleep mode, comment this line to enable sleep mode


  if( (LightReading<=LightThreshold) || (LightReading>LightThreshold&&LightCounting<4) )
  {
    if(RFIDLowed==1)//run once after wake up
    {
      digitalWrite(RFID1_EN, HIGH); // turn on RFID
      digitalWrite(RFID2_EN, HIGH);
      rfidsdread();
      scale.power_up();
      RFIDLowed=0;
      Serial.begin(115200);
      delay(50);
      //SPI.begin();
      //SDInit();
      WiFi.noLowPowerMode();
      OTA_begin();
    }
    Serial.println("lightCounting is " + (String)LightCounting);
    if(LightReading<=LightThreshold)
    {
      LightCounting=0;
    }
    else if(LightReading>LightThreshold)
    {
      LightCounting++;
    }

  debugln("beginn of main loop");
  looptime = millis();

  
  ArduinoOTA.poll();
  
// Weight scale
#ifdef WEIGHT_SCALE
#ifdef FOUR_HX711
  float weight1 = scale1.get_units(5);
  float weight2 = scale2.get_units(5);
  float weight3 = scale3.get_units(5);
  float weight4 = scale4.get_units(5);
  float weight = weight1 + weight2 + weight3 + weight4;
#else
  scale.set_scale(calibration_factor); // Adjust to this calibration factor

  Serial.print("Reading: ");
  // output = scale.get_units();


  average = scale.get_units(3),4;//read weight, average for oled display
    
  Serial.print(average,4);
  int ave=average*1000;//unit convertion ->g, ave for server

  // Zero drift compensation
  if ((average_last > average + 0.015 || average_last < average - 0.015))
  {
    // Minimum Load defines Zero-Band
    if (average < 0.020)
    {
      average = 0;
    }
    Serial.print("\tFilter: ");
    Serial.print(average);
    average_last = average;
  }
  else
  {
    Serial.print("\tFilter: ");
    Serial.print(average_last, 2);
  }
  Serial.print(" kg"); // Change this to kg and re-adjust the calibration factor if you follow SI units like a sane person
  Serial.print(" calibration_factor: ");
  Serial.print(calibration_factor);
  Serial.println();


#endif

  // float weight = random(300); //dummy weight data
  debugln((String) "weight: " + average_last);
#else
  float weight = 0.0;
#endif
  digitalWrite(BUZZER, LOW); // turn off Buzzer

//DHT reading
  float h1 = dht.readHumidity();//read temperature
  float t1 = dht.readTemperature();//read humidity
  if(isnan(t1))
  {
    t1=-100.00;
  }
  if(isnan(h1))
  {
    h1=-1.00;
  }


  // RFID
  // Scan RFID1
  uint8_t numberOfTags1 = 0;
  String finalTags1 = multiscan_v2(&numberOfTags1, TIMEOUT);//read RFID 1
  debugln("RFID1:");
  debugln((String)numberOfTags1); // send numberOfTags to GUI
  debugln((String)finalTags1);    // send finalTags to GUI

  

  // mySerial RFID2
  uint8_t numberOfTags2 = 0;
  String finalTags2 = multiscan2_v2(&numberOfTags2, TIMEOUT);//read RFID 2
  debugln("RFID2:");
  debugln((String)numberOfTags2); // send numberOfTags to GUI
  debugln((String)finalTags2);    // send finalTags to GUI

  // data processing
  String finalTags = keepIndividuals(finalTags1, numberOfTags1, finalTags2, numberOfTags2);
  uint8_t numberOfTags = finalTags.length() / 8;
  UserIDFind(finalTags);
  static uint8_t prev_numberOfTags = 0;
  debugln("TOTAL:");
  Serial.print((String) "w");       // send weight to GUI
  Serial.println(average,4);
  //Serial.println((String) "w" + average);       // send weight to GUI
  Serial.println((String) "n" + numberOfTags); // send numberOfTags to GUI
  Serial.println((String) "f" + finalTags);    // send finalTags to GUI

  // check for added/removed tags
  static String prev_finalTags = "";
  String addedTags = tagsChanged(prev_finalTags, prev_numberOfTags, finalTags, numberOfTags);
  String removedTags = tagsChanged(finalTags, numberOfTags, prev_finalTags, prev_numberOfTags);
  Serial.println((String) "d" + addedTags);   // send numberOfTags to GUI
  Serial.println((String) "r" + removedTags); // send finalTags to GUI

  prev_finalTags = finalTags;
  prev_numberOfTags = numberOfTags;

  Serial.println((String) "t" + t1);   // send temperature to GUI
  Serial.println((String) "H" + h1); // send humidity to GUI
  Serial.println((String)"u"+ USERID);
  Serial.println((String)"U" + UID);
  Serial.println((String)"V" + version);
  

  // display on screen
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.println((String)average_last + " kg");
  display.println((String)numberOfTags + " Tags");
  display.display();

  midtermTask(numberOfTags,finalTags);


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
  String timestamp = "1";//make data
  String data = buildData(version, scanresult, timestamp, (String)ave, unitinfo, (String)t1, (String)h1);//make data
  Serial.println((String)"payload: " + data);

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
      digitalWrite(BUZZER, LOW); // turn off Buzzer
      wifiInit();
    }
  }

//light atlering
  digitalWrite(GREENLED, !digitalRead(GREENLED));

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
          SD.remove("WIFIPWD.txt");
          SD.remove("WIFISSID.txt");
          writeSD_wifissid(ssid);
          writeSD_wifipwd(pwd);

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

    else if(d1 == '+' || d1 == 'a'){
      calibration_factor += 50;
      saveScaleFactor((String)calibration_factor);
      displaySavedData();
    }
    else if(d1 == '-' || d1 == 'z'){
      calibration_factor -= 50;
      saveScaleFactor((String)calibration_factor);
      displaySavedData(); 
    }

    else if(d1 == 's')//auto-calibration send "s200" to auto calibration at 200g
    {
      x = incomingString.substring(1);
      Serial.println("Auto-calibration!");

      double difference=scale.get_value();

      float Target_weight=x.toInt();
      int cali_result=(difference/Target_weight)*1000;
      calibration_factor = cali_result;
      scale.set_scale(cali_result);
      Serial.println(cali_result);
      SD.remove("CALIB.txt");
      writeSD_autoca((String)calibration_factor);
    }

    else if (d1 == 't')
    { // tare was clicked
      debugln("tare ");
      scale.tare(10);
      SD.remove("RESET.txt"); // delete reset.txt, so only last offset will be saved
      writeSD_reset((String)scale.get_offset());
    }

    else if(d1 == 'p')//set power, "p0" to set power to 26dBm
    {
      String powers = incomingString.substring(1);
      int powersetting = powers.toInt();
      
      rfidInit(powersetting);
      rfidInit2(powersetting);
      SD.remove("RFIDPWR.txt");
      writeSD_rdidpwr(powers);
    }
  }
  // END of GUI

  // Print ellapsed time per loop
  looptime = millis() - looptime;
  debugln((String) "Elapsed time per loop: " + looptime);

  // data12 = readSD_rfidpwr();
  // dataint = data12.toInt();
  // Serial.println("RFID power reading is " + (String)dataint + " and " + data12);//rfid power testing

  // delay(1000);
  }
  else//enter Sleep mode
  {
    Serial.println("Sleep now!");
    if(RFIDLowed == 0)//only work for once
    {
      digitalWrite(BLUELED, LOW);
      digitalWrite(GREENLED, LOW);
      digitalWrite(REDLED, LOW);
      digitalWrite(RFID1_EN, LOW); 
      digitalWrite(RFID2_EN, LOW);
      digitalWrite(BUZZER, LOW);
      display.clearDisplay();
      display.display();
      rfidLow();
      rfidLow2();
      RFIDLowed=1;
      scale.power_down();
      //WiFi.lowPowerMode();
      Serial.end();
      ArduinoOTA.end();
    }
    

    HBcount++;

    if(HBcount>=HBThread)//send Heartbeat package to server in every <HBThread> times
    {
      HBcount = 0;
      if (WiFi.status() != WL_CONNECTED)
      {
        wifiSleep();
      }//connect WiFi during sleep
      if (WiFi.status() == WL_CONNECTED)
      {
      String SleepData = SleepBuild(version,"1",unitinfo);//make sleep data   
      httpHB(SleepData);//send data
      }
    }
    LightCounting=5;
   
    LowPower.sleep(1000);
  }
}

// private functions ******************************************************************

void OTA_begin()
{
  ArduinoOTA.begin(WiFi.localIP(), "G031A-Tray-26", "123456", InternalStorage); // 6105-Tray-24
}

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
  Serial.print("%");
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

#ifdef WEIGHT_SCALE
void weightscaleInit(float SCALEFACTOR)
{

  scale.begin(DT_PIN, WEIGHT_SCK_PIN);
  scale.set_scale(SCALEFACTOR);
  String offsetstr = readSDreset();
  String calistr = readSD_aotuca();

  if (offsetstr != "")
  { // if there is a value saved on sd card
    int offsetint = offsetstr.toInt();
    debugln((String) "offset: " + offsetint);
    scale.set_offset(offsetint);
  }
  else
  {
    debugln("tare ");
    scale.tare(10);
    SD.remove("RESET.txt"); // delete reset.txt, so only last offset will be saved
    writeSD_reset((String)scale.get_offset());
  }
    if (calistr != "")
  { // if there is a value saved on sd card
    int caliint = calistr.toInt();
    calibration_factor=caliint;
    scale.set_scale(caliint);
  }
  else
  {
    scale.set_scale(calibration_factor);
    SD.remove("CALIB.txt"); // delete reset.txt, so only last offset will be saved
    writeSD_autoca((String)calibration_factor);
  }
}
#endif

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

String buildData(String ver, String id, String timestamp, String weight, String unitinfo, String temp, String Humi) { //组建发送的json字符串
  String payload; 

  payload = "{\"unitinfo\":\"";
  payload += unitinfo;

  payload += "\",\"version\":\"";
  payload += ver;

  payload += "\",\"sn\":\"";
  payload += UID;

  payload += "\",\"data\":{\"weight\":\"";
  payload += weight;
  payload += "\",\"uids\":[";
  payload += id;
  payload += "],\"temp\":\"";
  payload += temp;
  payload += "\",\"humi\":\"";
  payload += Humi;
  payload += String("\"}}");
  //Serial.println((String)"payload: " + payload);
  return payload;

}

// this method makes a HTTP connection to the server:
void httpRequest(String data)
{
  Serial.println("httpRequest");
  //  close any connection before send a new request.
  //  This will free the socket on the NINA module
  client.stop();

  // if there's a successful connection:
  //if (client.connect(servername, 8000))//for debugging
  if (client.connect(server, 8000))
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
    Serial.println((String) "Data:" + data);
    Serial.flush();

    digitalWrite(BLUELED, LOW);
  }
  else
  {
    // if you couldn't make a connection:
    Serial.println("connection failed");
    status = WL_IDLE_STATUS;
    WiFi.end();
    digitalWrite(BLUELED, HIGH);
  }
}

void printWifiStatus()
{
  // print the SSID of the network you're attached to:
  byte mac[6];
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

  WiFi.macAddress(mac);//print mac address
  Serial.print("MAC: ");
  Serial.print(mac[5],HEX);
  Serial.print(":");
  Serial.print(mac[4],HEX);
  Serial.print(":");
  Serial.print(mac[3],HEX);
  Serial.print(":");
  Serial.print(mac[2],HEX);
  Serial.print(":");
  Serial.print(mac[1],HEX);
  Serial.print(":");
  Serial.println(mac[0],HEX);
}

void wifiInit()
{
  digitalWrite(BUZZER, LOW);//close buzzer
  String wifi00 = readSD_wifissid();
  String PWD = readSD_wifipwd();
  if(wifi00 == "" && PWD == "")
  {
    SD.remove("WIFIPWD.txt");
    SD.remove("WIFISSID.txt");
    wifi00 = SECRET_SSID;
    PWD = SECRET_PASS;
    writeSD_wifissid(wifi00);
    writeSD_wifipwd(PWD);
  }
  const int namelen = wifi00.length()-1;const int passlen = PWD.length()-1;

  char wifiname[namelen];char wifipass[passlen];
  wifi00.toCharArray(wifiname,namelen);
  PWD.toCharArray(wifipass,passlen);
  

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE)
  {
    Serial.println("Communication with WiFi module failed!");
  }

  long prev_time = millis();
  long tim = 0;
  bool con = true;
  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED)
  {
    Serial.println("Attempting to connect to SSID: ");
    Serial.println(wifiname);
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print((String) "Connecting to: " + wifiname);
    display.print((String) "PWD: " + wifipass);
    display.display();
      //Serial.println("length" + (String)namelen +(String)passlen);

    Serial.print(wifipass);Serial.println();
    status = WiFi.begin(wifiname, wifipass);
    //status=WiFi.begin(e_data.SSID,e_data.PWD);//Debugging use
    // wait 10 seconds for connection:
    //delay(4000);
    tim = millis() - prev_time;
    if (tim > 10000)
    { // 0.5 min
      con = false;
      break;
    }
  }
  // you're connected now, so print out the status:
  printWifiStatus();
  if (con)
  { // could connect
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(0, 0);
    display.println("Connected!");
    display.display();
    digitalWrite(BLUELED, LOW);
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
    digitalWrite(BLUELED, HIGH);
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

void SERCOM0_Handler()
{
  mySerial.IrqHandler();
}

// RFID
void rfidInit2(int set)
{
    switch (set)
    {
    case 0: {byte setPower1[] = {0xBB, 0x00, 0xB6, 0x00, 0x02, 0x0A, 0x28, 0xEA, 0x7E};mySerial.write(setPower1, sizeof(setPower1));break;}// 26dBm 
    case 1: {byte setPower2[] = {0xBB, 0x00, 0xB6, 0x00, 0x02, 0x07, 0xD0, 0x8F, 0x7E};mySerial.write(setPower2, sizeof(setPower2));break;} //20dBm
    case 2: {byte setPower3[] = {0xBB, 0x00, 0xB6, 0x00, 0x02, 0x07, 0x3A, 0xF9, 0x7E};mySerial.write(setPower3, sizeof(setPower3));break;}//18.5dBm
    case 3: {byte setPower4[] = {0xBB, 0x00, 0xB6, 0x00, 0x02, 0x06, 0x0E, 0xCC, 0x7E};mySerial.write(setPower4, sizeof(setPower4));break;} //15.5dBm

    default: {byte setPower5[] = {0xBB, 0x00, 0xB6, 0x00, 0x02, 0x0A, 0x28, 0xEA, 0x7E};mySerial.write(setPower5, sizeof(setPower5));}// 26dBm 
    

    }
}

void rfidLow2()
{
  //  const byte setPower[] = {0xBB, 0x00, 0xB6, 0x00, 0x02, 0x06, 0x0E, 0xCC, 0x7E}; //15.5dBm
  const byte setPower[] = {0xBB, 0x00, 0x17, 0x00, 0x00, 0x17, 0x7E}; //sleep
  mySerial.write(setPower, sizeof(setPower));
  // Serial.println("set power to 26 dBm");
}

String multiscan2_v2(uint8_t *numberOfTags, unsigned long timeout)
{
  // const byte readMulti[] = {0xBB, 0x00, 0x22, 0x00, 0x00, 0x22, 0x7E}; //x1
  // const byte readMulti[] = {0xBB, 0x00, 0x27, 0x00, 0x03, 0x22, 0x00, 0x03, 0x4F, 0x7E}; // x3
  const byte readMulti[] = {0xBB, 0x00, 0x27, 0x00, 0x03, 0x22, 0x00, 0x0A, 0x56, 0x7E}; //x10
  //const byte readMulti[] = {0xBB, 0x00, 0x27, 0x00, 0x03, 0x22, 0x00, 0x14, 0x60, 0x7E}; // x20
  //const byte readMulti[] = {0xBB, 0x00, 0x27, 0x00, 0x03, 0x22, 0x00, 0x1E, 0x6A, 0x7E}; // x30
  // const byte readMulti[] = {0xBB, 0x00, 0x27, 0x00, 0x03, 0x22, 0x00, 0x32, 0x7E, 0x7E}; // x50
  bool finished_scan = false;
  uint8_t tags = 0;
  uint16_t i = 0;
  unsigned long currentMillis = 0; // millis() returns an unsigned long.
  mySerial.write(readMulti, sizeof(readMulti));
  currentMillis = millis(); // reset timer;
  uint8_t result[200] = {0};
  String finalTags = "";

  while (!finished_scan)
  {
    while (mySerial.available())
    {

      result[i] = mySerial.read();
      // Serial.print(result[i], HEX);
      // Serial.print(",");

      if (result[i] == 0x7e)
      {
        // Serial.println((String)"i: " + i);
        // if (i < 8)
        if ((i == 23) == (i == 19))
        { // if not enough characters, skipp this tag
          i = 0;
          // Serial.println("Skipped tag");
          continue;
        }
        tags++;
        i = 0;

        // Serial.println("do some data processing");

        String tagID = getTagfromHEX(result, 1);
        // Serial.println((String) "tagID: " + tagID);
        // if (!isValidNumber(tagID)) // check if tagID consists of only numbers
        // {
        //   // Serial.println("no number");
        //   continue;
        // }
        // if (!first4DiggitsZero(tagID)) // check if the first 4 digits are all zeros, if not its a wrong read
        // {
        //   // Serial.println("not all are zeros");
        //   continue;
        // }

        *numberOfTags = finalTags.length() / 8;
        // Serial.println((String) "numberOfTags: " + *numberOfTags);
        finalTags = keepIndividuals(finalTags, *numberOfTags, tagID, 1);
        // Serial.println((String) "finalTags: " + finalTags);

        continue;
      }
      i++;

      currentMillis = millis(); // reset timer;
    }

    if (millis() - currentMillis > timeout)
    { // if timeout is reached then finish reading
      // Serial.println((String) "numberOfTags: " + numberOfTags);
      // Serial.println((String) "finalTags: " + finalTags);

      // send to GUI
      // Serial.println((String) "n" + *numberOfTags); // send numberOfTags to GUI
      // Serial.println((String) "f" + finalTags);    // send finalTags to GUI

      // Serial.println.println("finished Read");

      finished_scan = true;
    }
  }

  return finalTags;
}

void midtermTask(uint8_t numberOfTags, String finalTags)
{
  // case1
  // if any tag has different first two digit values with others, warning == 1 and red light on
  // e.g. 0100111d and 02001123 will turn on the red light, 0000111d and 00001123 will not
  int warning = 0;

  for (int i = 0; i < numberOfTags * 8; i += 8)
  {
    String group = finalTags.substring(i, i + 2);
    for (int j = 0; j < numberOfTags * 8; j += 8)
    {
      String temp_group = finalTags.substring(j, j + 2);
      if (group != temp_group)
      {
        warning = 1;
        //Serial.println("Two chemicals cannot be put together!!!");
      }
    }
  }

  if (warning == 1)
  {
    //digitalWrite(REDLED, HIGH); // Red On
    //digitalWrite(BUZZER, HIGH); // buzzer On
                                // digitalWrite (11, HIGH); // Yellow On
  }
  if (warning == 0)
  {
    digitalWrite(REDLED, LOW); // Red Off
    digitalWrite(BUZZER, LOW); // buzzer On
     digitalWrite (11, LOW); // Yellow Off
  }
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

void rfidsdread()
{
  String data12 = readSD_rfidpwr();
  int dataint = data12.toInt();
  Serial.println("RFID power reading is " + (String)dataint + " and " + data12);
  if(data12=="")
  {
    SD.remove("RFIDPWR.txt");
    writeSD_rdidpwr((String)powerset);
    rfidInit(powerset);
    rfidInit2(powerset);
  }
  else
  {
    rfidInit(dataint);
    rfidInit2(dataint);
  }
}

void makeVersion()
{
  version += "-";
  if(WEIGHTSCALE_ENABLE){version += "W";}
  if(RFIDREADER_ENABLE){version += "R";}
  if(LIGHTSENSOR_ENABLE){version += "L";}
  if(DHT11_ENABLE){version += "D";}
}

void httpHB(String data)
{
  client.stop();

  // if there's a successful connection:
  //if (client.connect(servername, 8000))//for debugging
  if (client.connect(server, 8000))
  {
    Serial.println("connecting...");

    // send the HTTP GET request:
    client.println((String) "POST " + urlHB + " HTTP/1.0");
    client.println((String) "Host: " + serverno);
    client.println((String) "Accept: */*");
    client.println((String) "Content-Length: " + data.length());
    client.println("Content-Type: application/json");
    client.println();
    client.println(data);

    // Serial.println((String)"postRequest: " + postRequest);
    //    client.println(postRequest);
    //    client.println("Connection: close");
    //    client.println();
    Serial.println("Sent!");
  }
  else
  {
    // if you couldn't make a connection:
    Serial.println("connection failed");
    status = WL_IDLE_STATUS;
    WiFi.end();
  }
}

void wifiSleep()
{
  WiFi.end();
  String wifi00 = readSD_wifissid();
  String PWD = readSD_wifipwd();
  if(wifi00 == "" && PWD == "")
  {
    SD.remove("WIFIPWD.txt");
    SD.remove("WIFISSID.txt");
    wifi00 = SECRET_SSID;
    PWD = SECRET_PASS;
    writeSD_wifissid(wifi00);
    writeSD_wifipwd(PWD);
  }
  const int namelen = wifi00.length()-1;const int passlen = PWD.length()-1;

  char wifiname[namelen];char wifipass[passlen];
  wifi00.toCharArray(wifiname,namelen);
  PWD.toCharArray(wifipass,passlen);
  
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE)
  {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true)
      ;
  }

  long prev_time = millis();
  long tim = 0;
  bool con = true;

  delay(500);
  status = WiFi.begin(wifiname, wifipass);
  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED)
  {
    status = WiFi.begin(wifiname, wifipass);

    tim = millis() - prev_time;
    if (tim > 10000)
    { // 0.5 min
      con = false;
      break;
    }
  }
}

String SleepBuild(String ver, String timestamp, String unitinfo) { //组建发送的json字符串
  String payload; 

  payload = "{\"unitinfo\":\"";
  payload += unitinfo;

  payload += "\",\"version\":\"";
  payload += ver;

  payload += "\",\"sn\":\"";
  payload += UID;

  //payload += "\",\"data\":{";
  payload += String("\"}");
  //Serial.println((String)"payload: " + payload);
  return payload;
  //  char attributes[100];
  //  payload.toCharArray( attributes, 100 );
}