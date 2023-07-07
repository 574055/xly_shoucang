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
const String unitinfo = "1";

//WEIGHT SENSOR
const int DT_PIN = A0;
const int SCK_PIN = A1;
const int LOADCELL1_DOUT_PIN = 2;
const int LOADCELL1_SCK_PIN = 3;
const int LOADCELL2_DOUT_PIN = 4;
const int LOADCELL2_SCK_PIN = 5;
const int LOADCELL3_DOUT_PIN = A0; //6;
const int LOADCELL3_SCK_PIN = A1; //7;
const int LOADCELL4_DOUT_PIN = 8;
const int LOADCELL4_SCK_PIN = 9;

HX711 scale1;
HX711 scale2;
HX711 scale3;
HX711 scale4;

#define SCALEFACTOR 100.f + 5.f


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

// #define NUMBEROFBYTES (256)
// #define NUMBERSCANS   3

#define REDLED 12
#define GREENLED 13
#define YELLOWLED 11

void setup() {
  Serial1.begin(115200); //for RFID
  Serial.begin(115200); //USB debugging

  pinMode (REDLED, OUTPUT); // Red LED
  pinMode (YELLOWLED, OUTPUT); // Yellow LED
  pinMode (GREENLED, OUTPUT); // Green LED
  digitalWrite(GREENLED, HIGH);

  weightscaleInit();
  wifiInit();
  
  //while (!Serial) {}
  Serial.println("WeShare RFID reader is ready!");
}

void loop() {
  Serial.println("beginn of main loop");
  myTime = millis();
 
  //Weight scale
  float weight1 = scale1.get_units(5);
  float weight2 = scale2.get_units(5);
  float weight3 = scale3.get_units(5);
  float weight4 = scale4.get_units(5);
  float weight = weight1 + weight2 + weight3 + weight4;

  Serial.print("scale1:\t");
  Serial.print(weight1, 1);
  Serial.print("\tscale2:\t");
  Serial.print(weight2, 1);
  Serial.print("\tscale3:\t");
  Serial.print(weight3, 1);
  Serial.print("\tscale4:\t");
  Serial.print(weight4, 1);
  Serial.print("\tsum:\t");
  Serial.println(weight, 1);



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
  // digitalWrite (11, HIGH); // Yellow On
 }
  if (warning == 0)
 {
  digitalWrite (REDLED, LOW); // Red Off
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

}



//private fucntions *******************************************************




String buildData(String id, String timestamp, String weight, String unitinfo) { //组建发送的json字符串
  String payload = "{\"unitinfo\":\"testxtray4\",\"data\":{\"weight\":\"";
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
  Serial.println("Initializing the scale");

  // Initialize library with data output pin, clock input pin and gain factor.
  // Channel selection is made by passing the appropriate gain:
  // - With a gain factor of 64 or 128, channel A is selected
  // - With a gain factor of 32, channel B is selected
  // By omitting the gain factor parameter, the library
  // default "128" (Channel A) is used here.
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
