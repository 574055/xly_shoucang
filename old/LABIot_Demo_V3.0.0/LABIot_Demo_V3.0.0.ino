/**
   lab IoT demo on Arduino Nano
   LMT 2021.5.19
   Function description:
   initWifi(): start wifi connection
   initCIP(): start CIPSEND
   httpPost(): POST data
   getWeightData(): get data of weight sensor
   getRFIDData(): get data of RFID sensor, buzzer control is under this function
   buildData(): build JSON data string
   reset(): ESP reset

   new update on 2021.5.19
   UHF RFID part
   change buzzer into LED light
*/


#include "HX711.h"
#include <SPI.h>
#include "UST_RFID.h"
#include <WiFiNINA.h>
#include "arduino_secrets.h"
#include "wiring_private.h" 

//basic setting of hardware
const String unitinfo = "1";
//WEIGHT SENSOR
const int DT_PIN = A0;
const int SCK_PIN = A1;
//Timer
unsigned long myTime;

//UHF Commands
const byte readSingle[] = {0xBB, 0x00, 0x22, 0x00, 0x00, 0x22, 0x7E};
const byte readMulti[] = {0xBB, 0x00, 0x27, 0x00, 0x03, 0x22, 0x00, 0x0A, 0x56, 0x7E};//10 times rotation
const byte stopRead[] = {0xBB, 0x00, 0x28, 0x00, 0x00, 0x28, 0x7E};
const byte setBaudrate[] = {0xBB, 0x00, 0x11, 0x00, 0x02, 0x00, 0x60, 0x73, 0x7E};


String idList = "";

//Wifi
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;            // your network key index number (needed only for WEP)

int status = WL_IDLE_STATUS;

// Initialize the WiFi client library
WiFiClient client;

#define url "/hwinfo"
#define serverno "148.70.180.108"
IPAddress server(148,70,180,108);


HX711 scale;

#define NUMBEROFBYTES (20*6)
#define NUMBERSCANS   5

void setup() {
  Serial1.begin(115200); //for RFID
  Serial.begin(115200); //USB debugging
  while (!Serial) {
    ;
  }
  Serial.println("WeShare RFID reader is ready!");
  
 
  //SPI.begin();

  
  //weight scale init
  scale.begin(DT_PIN, SCK_PIN);
  scale.set_scale(2280.f);  // 開始取得比例參數                      // this value is obtained by calibrating the scale with known weights; see the README for details
  scale.tare();//////////程序崩坏点！！！！！！！

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

  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }
  // you're connected now, so print out the status:
  printWifiStatus();
}

void loop() {
  Serial.println("beginn of main loop");
  myTime = millis();

  
  
  //Weight scale
  float weight = scale.get_units(5);
  scale.power_down();
  Serial.println((String)"Weight: " + weight);
  //String weight = String(getWeightData());
  Serial.println("finweight");



  //RFID
  uint8_t hexArr[NUMBEROFBYTES], numberOfTags[6], i = 0;
  String scanString[6];
  char *p; 
  do{ //do-while loops run at least one time
    numberOfTags[i]  = getRFIDData(hexArr); //scan
    //check if there is a Tag
    p = getTagfromHEX_check(hexArr);
    if ((String)p == "1516"){
      //Serial.println("no Tag");
      scanString[i] = "";
    }
    else{
      p = getTagfromHEX(hexArr, numberOfTags[i]);
      //Serial.println((String)"ids: " + p);
      scanString[i] = (String)p;
      if (i>0){ //check for double
        scanString[0] = keepIndividuals(scanString[0],numberOfTags[0],scanString[i],numberOfTags[i]);
        numberOfTags[0] = scanString[0].length()/8;
        //Serial.println(scanString[0]);
        //Serial.println((String)"number of Tags: " + numberOfTags[0]);
      }
    }
    i++;
  }while (i < NUMBERSCANS);
  Serial.println(scanString[0]);
  Serial.println((String)"number of Tags: " + numberOfTags[0]);
  Serial.println("finished rfid");
  //END of RFID



  // get response form wifi
  //while (!client.available()){}
  while (client.available()) {
    char c = client.read();
    Serial.write(c);
  }


  
  //Send Data through Wifi  
  String timestamp = "1";
  String data = buildData(scanString[0], timestamp, (String)weight, unitinfo);
  httpRequest(data);



// Print ellapsed time per loop
  myTime = millis()-myTime;
  Serial.println((String)"Elapsed time per loop: " + myTime); 

  scale.power_up();
}



//private fucntions *******************************************************

uint8_t getRFIDData(uint8_t result[NUMBEROFBYTES]) {
//print RFID reply
  //Serial1.listen();
  Serial.println("read RFID tag"); 
  Serial1.write(readSingle, sizeof(readSingle));
//  Serial1.write(readMulti, sizeof(readMulti));
  while (!Serial1.available()){} //wait for RFID response 
  delay(500); //additional dealy, since RFID string can be quite long and take a while to arrive
  uint16_t i = 0;
  uint8_t tags = 0; 
  while (Serial1.available()) {    
    result[i] = Serial1.read();    
    //Serial.print(result[i],HEX);      
    if (result[i] == 0x7E){
      tags++; 
    }
    i++;
    if (!Serial1.available()){//wait till last HEX message has arrived
      return tags;
    }
  }
}



String buildData(String id, String timestamp, String weight, String unitinfo) { //组建发送的json字符串
  String payload = "{";
  payload += "\"Targetid\":\"";
  payload += id;
  payload += "\",";
  payload += "\"Timestamp\":\"";
  payload += timestamp;
  payload += "\",";
  payload += "\"Weight\":";
  payload += weight;
  payload += ",";
  payload += "\"Unitinfo\":\"";
  payload += unitinfo;
  payload += "\"}";
  Serial.println(payload);
  return payload;
  //  char attributes[100];
  //  payload.toCharArray( attributes, 100 );
}


// this method makes a HTTP connection to the server:
void httpRequest(String data) { 
  // close any connection before send a new request.
  // This will free the socket on the NINA module
  client.stop();

  // if there's a successful connection:
  if (client.connect(server, 5000)) {
    Serial.println("connecting...");

    String postRequest = "POST ";
    postRequest += url;
    postRequest += " HTTP/1.0\r\n";
    postRequest += "Host: " ;
    postRequest += serverno;
    postRequest += "\r\n";
    postRequest += "Accept: *" ;
    postRequest += "/";
    postRequest += "*\r\n" ;
    postRequest += "Content-Length: " ;
    postRequest += data.length();
    postRequest += "\r\n" ;
    postRequest += "Content-Type: application/json\r\n" ;
    postRequest += "\r\n" ;
    postRequest += data;
    
    //Serial.println(postRequest);
    Serial.println((String)"Data: " + data);
    
    client.println(postRequest);
    client.println("Connection: close");
    client.println();

  } else {
    // if you couldn't make a connection:
    Serial.println("connection failed");
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
