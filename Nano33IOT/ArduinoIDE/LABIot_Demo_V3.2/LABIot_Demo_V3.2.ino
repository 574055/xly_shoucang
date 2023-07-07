/**
   lab IoT demo on Arduino Nano
   LMT 2021.5.19
   Function description:
   initWifi(): start wifi connection
   initCIP(): start CIPSEND
   httpPost(): POST data
   getWeightData(): get data of weight sensor
   RFID_scan(): get data of RFID sensor, buzzer control is under this function
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



//basic setting of hardware
const String unitinfo = "testxtray3";
//WEIGHT SENSOR
const int DT_PIN = A0;
const int SCK_PIN = A1;
//Timer
unsigned long myTime, scanTimeOut = 0;



//Wifi
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;            // your network key index number (needed only for WEP)

int status = WL_IDLE_STATUS;

// Initialize the WiFi client library
WiFiClient client;

#define url "/lab/hwinfo"
// #define serverno "148.70.180.108"
// IPAddress server(148,70,180,108);
#define serverno "18.162.252.107"
IPAddress server(18,162,252,107);



HX711 scale;
#define SCALEFACTOR -223.f

// #define NUMBEROFBYTES (256)
// #define NUMBERSCANS   3

#define REDLED 12
#define GREENLED 13
#define YELLOWLED 11
#define BUZZER 10

void setup() {
  Serial1.begin(115200); //for RFID
  Serial.begin(115200); //USB debugging

  pinMode (REDLED, OUTPUT); // Red LED
  pinMode (YELLOWLED, OUTPUT); // Yellow LED
  pinMode (GREENLED, OUTPUT); // Green LED
  pinMode (BUZZER, OUTPUT); 
  digitalWrite(GREENLED, HIGH);

  rfidInit();
  weightscaleInit();
  wifiInit();
  
  //while (!Serial) {}
  Serial.println("WeShare RFID reader is ready!");
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
  //Scan RFID
  uint8_t scanResult[NUMBEROFBYTES];
  uint8_t numberOfTags = multiscan(scanResult, TIMEOUT); //the data is stored in scanResult
  // for (int i = 0; i<100; i++){
  //     Serial.print(scanResult[i],HEX);
  // }
  //Serial.println((String)"numberOfTags: " + numberOfTags);

  //Extract ID numbers from the whole Hex array and convert to char array
  char *tagID;
  tagID = getTagfromHEX(scanResult, numberOfTags);
  //Serial.println((String)"getTagfromHEX: " + tagID);


  String finalTags = keepIndividualsOfMultiScan ((String) tagID, &numberOfTags);
  //Serial.println((String)"finalTags: " + finalTags);
  //Serial.println((String)"final numberOfTags: " + numberOfTags);

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
  
  Serial.println(scanresult);

  
  Serial.println((String)"number of Tags: " + numberOfTags);
  Serial.println("finished rfid");
  //END of RFID


  //

  // get response form wifi
  //while (!client.available()){}
  while (client.available()) {
    char c = client.read();
    Serial.write(c);
  }


  
  //Send Data through Wifi  
  String timestamp = "1";
  String data = buildData(scanresult, timestamp, (String)weight, unitinfo);
  //Serial.println((String)"payload: " + data);
  
  httpRequest(data);

  digitalWrite (GREENLED, !digitalRead(GREENLED));

  // Print ellapsed time per loop
  myTime = millis()-myTime;
  Serial.println((String)"Elapsed time per loop: " + myTime); 

  scale.power_up();
}



//private fucntions *******************************************************




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
    
//    client.println(postRequest);
//    client.println("Connection: close");
//    client.println();

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

void weightscaleInit(){
  scale.begin(DT_PIN, SCK_PIN);
  scale.set_scale(SCALEFACTOR);  // 開始取得比例參數                      // this value is obtained by calibrating the scale with known weights; see the README for details
  scale.tare();//////////程序崩坏点！！！！！！！

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
