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
void saveScaleFactor(String scaleFactor);
void saveWifi(String SSID, String PWD);
void weightscaleInit(float SCALEFACTOR);
void eepromInit();
void wifiInit();
String buildData(String id, String timestamp, String weight, String unitinfo, String t1, String h1);
void httpRequest(String data);
void printWifiStatus();
void oledInit();
void midtermTask(uint8_t numberOfTags, String finalTags);

void SERCOM0_Handler();
void rfidInit2();
String multiscan2_v2(uint8_t *numberOfTags, unsigned long timeout);

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

// HX711
float calibration_factor = 20900; //=100g
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
const int RESET_PIN = A2;
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

#define DHTTYPE DHT11
#define DHTPIN 17
DHT dht(DHTPIN, DHTTYPE);//DHT11 setup

int UID[100];

void resetfunc()
{
  analogWrite(RESET_PIN,255);
  delay(100);
  analogWrite(RESET_PIN,0);
  return;
}


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

  rfidInit();
  rfidInit2();
  debugln("RFID ready");
  wifiInit();
  debugln("WeShare RFID reader is ready!");
#ifdef WEIGHT_SCALE
  weightscaleInit(atof(e_data.SCALEFACTOR));
#endif
  debugln("weight scale ready");

    dht.begin();//DHT init
}

void loop()
{
  debugln("beginn of main loop");
  looptime = millis();

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
  

  // Smoothing the results
  //  subtract the last reading:
  // total = total - readings[readIndex];
  // // read from the scale
  // readings[readIndex] = scale.get_units(), 2;
  // // add the reading to the total:
  // total = total + readings[readIndex];
  // // advance to the next position in the array:
  // readIndex = readIndex + 1;

  // // if we're at the end of the array...
  // if (readIndex >= cycles)
  // {
  //   // ...wrap around to the beginning:
  //   readIndex = 0;
  // }
  // calculate the average:
  //average = total / cycles;

  average = scale.get_units(15),4;
    //unit convertion
  Serial.print(average,4);
  int ave=average*1000;

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

  // simple Kalmanfilter
  // float measured_value, weight;
  // for (uint8_t i = 0; i < 30; i++)
  // { // get weight
  //   measured_value = scale.get_units();
  //   weight = simpleKalmanFilter.updateEstimate(measured_value);
  // }

#endif

  // float weight = random(300); //dummy weight data
  debugln((String) "weight: " + average_last);
#else
  float weight = 0.0;
#endif
  digitalWrite(BUZZER, LOW); // turn off Buzzer

  float h1 = dht.readHumidity();
  float t1 = dht.readTemperature();
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
  String finalTags1 = multiscan_v2(&numberOfTags1, TIMEOUT);
  debugln("RFID1:");
  debugln((String)numberOfTags1); // send numberOfTags to GUI
  debugln((String)finalTags1);    // send finalTags to GUI

  

  // mySerial RFID2
  uint8_t numberOfTags2 = 0;
  String finalTags2 = multiscan2_v2(&numberOfTags2, TIMEOUT);
  debugln("RFID2:");
  debugln((String)numberOfTags2); // send numberOfTags to GUI
  debugln((String)finalTags2);    // send finalTags to GUI

  // data processing
  String finalTags = keepIndividuals(finalTags1, numberOfTags1, finalTags2, numberOfTags2);
  uint8_t numberOfTags = finalTags.length() / 8;
  static uint8_t prev_numberOfTags = 0;
  debugln("TOTAL:");
  Serial.println((String) "w" + average);       // send weight to GUI
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

  // display on screen
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.println((String)average_last + " kg");
  display.println((String)numberOfTags + " Tags");
  display.display();

  //midtermTask(numberOfTags,finalTags);


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
  String data = buildData(scanresult, timestamp, (String)ave, unitinfo, (String)t1, (String)h1);//
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
      digitalWrite(BUZZER, LOW); // turn off Buzzer
      wifiInit();
    }
  }

//
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
      calibration_factor += 200;
      saveScaleFactor((String)calibration_factor);
      displaySavedData();
    }
    else if(d1 == '-' || d1 == 'z'){
      calibration_factor -= 200;
      saveScaleFactor((String)calibration_factor);
      displaySavedData(); 
    }
    
    else if(d1 == 'r')//use computer to reset
    {
      resetfunc();
    }

    else if (d1 == 't')
    { // tare was clicked
      debugln("tare ");
      scale.tare(10);
      SD.remove("RESET.txt"); // delete reset.txt, so only last offset will be saved
      writeSD_reset((String)scale.get_offset());

    }
  }
  // END of GUI

  // Print ellapsed time per loop
  looptime = millis() - looptime;
  debugln((String) "Elapsed time per loop: " + looptime);

  // delay(1000);
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

#ifdef WEIGHT_SCALE
void weightscaleInit(float SCALEFACTOR)
{

  scale.begin(DT_PIN, WEIGHT_SCK_PIN);
  scale.set_scale(SCALEFACTOR);
  String offsetstr = readSDreset();

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

String buildData(String id, String timestamp, String weight, String unitinfo, String temp, String Humi) { //组建发送的json字符串
  String payload = "{\"unitinfo\":\"";
  payload += unitinfo;

  // for(size_t i = 0; i < UniqueIDsize; i++)
  // {
  // UID[i]=UniqueID[i],HEX;
  // payload += String(UID[i]);
  // payload += " ";
  // }
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
  //  char attributes[100];
  //  payload.toCharArray( attributes, 100 );
}

// this method makes a HTTP connection to the server:
void httpRequest(String data)
{
  Serial.println("httpRequest");
  // Serial.println((String)"data" + data);
  //  close any connection before send a new request.
  //  This will free the socket on the NINA module
  client.stop();

  // if there's a successful connection:
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
    digitalWrite(BLUELED, HIGH);
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
    display.print((String) "PWD: " + e_data.PWD);
    display.display();
    debugln((String) "WifiInit SSID: " + e_data.SSID);
    debugln((String) "WifiInit PWD: " + e_data.PWD);

    status = WiFi.begin(e_data.SSID, e_data.PWD);

    // wait 10 seconds for connection:
    delay(10000);
    tim = millis() - prev_time;
    if (tim > 20000)
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
void rfidInit2()
{
  const byte setPower[] = {0xBB, 0x00, 0xB6, 0x00, 0x02, 0x0A, 0x28, 0xEA, 0x7E}; // 26dBm
  // const byte setPower[] = {0xBB, 0x00, 0xB6, 0x00, 0x02, 0x07, 0xD0, 0x8F, 0x7E}; //20dBm
  // const byte setPower[] = {0xBB, 0x00, 0xB6, 0x00, 0x02, 0x07, 0x3A, 0xF9, 0x7E}; //18.5dBm
  //  const byte setPower[] = {0xBB, 0x00, 0xB6, 0x00, 0x02, 0x06, 0x0E, 0xCC, 0x7E}; //15.5dBm
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
  uint8_t result[100] = {0};
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
        if (!isValidNumber(tagID)) // check if tagID consists of only numbers
        {
          // Serial.println("no number");
          continue;
        }
        if (!first4DiggitsZero(tagID)) // check if the first 4 digits are all zeros, if not its a wrong read
        {
          // Serial.println("not all are zeros");
          continue;
        }

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
        Serial.println("Two chemicals cannot be put together!!!");
      }
    }
  }

  if (warning == 1)
  {
    digitalWrite(REDLED, HIGH); // Red On
    digitalWrite(BUZZER, HIGH); // Red On
                                // digitalWrite (11, HIGH); // Yellow On
  }
  if (warning == 0)
  {
    digitalWrite(REDLED, LOW); // Red Off
    digitalWrite(BUZZER, LOW); // Red On
    // digitalWrite (11, LOW); // Yellow Off
  }
}