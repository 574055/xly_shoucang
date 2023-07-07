#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <HX711.h>
#include <UST_RFID.h>
#include <SoftwareSerial.h>

SoftwareSerial SerialRFID(12, 14);  // RX, TX//9600



// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 10;
const int LOADCELL_SCK_PIN = 9;
HX711 scale;
#define SCALEFACTOR 192.f

//Timer
unsigned long myTime, scanTimeOut = 0;

//WiFi
const char* ssid = "35F";
const char* password = "44444444";
// const char* ssid = "Wi-Fi Repeater";
// const char* password = "charlietheunicorn";

const char* serverName = "http://148.70.180.108:8000/lab/hwinfo";

//LED pins
#define GREENLED 2 
#define REDLED 4
#define YELLOWLED 5 //onboard LED
#define SPEAKER 15

//fucntion prototypes
void weightscaleInit(uint8_t DT_PIN, uint8_t SCK_PIN);
void wifiInit();
String buildData(String id, String timestamp, String weight, String unitinfo);
uint8_t multiscan(uint8_t result[], unsigned long timeout);


void setup() {
  // Serial1.begin(115200); //for RFID
  SerialRFID.begin(115200);
  Serial.begin(115200); //USB debugging

  pinMode (REDLED, OUTPUT); // Red LED
  pinMode (YELLOWLED, OUTPUT); // Yellow LED
  pinMode (GREENLED, OUTPUT); // Green LED
  pinMode (SPEAKER, OUTPUT); 
  digitalWrite(GREENLED, HIGH);

  weightscaleInit(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  wifiInit();
  
  Serial.println("WeShare RFID reader is ready!");
}


void loop() {
  Serial.println("beginn of main loop");
  myTime = millis();


  //Weight scale
  float weight = scale.get_units(5);
  //scale.power_down();
  Serial.println((String)"Weight: " + weight);
  Serial.println("finweight");


  //RFID
  //Scan RFID
  //delay(500); //software serial needs some time to switch?
  uint8_t scanResult[NUMBEROFBYTES];
  uint8_t numberOfTags = multiscan(scanResult, TIMEOUT); //the data is stored in scanResult
  // for (int i = 0; i<100; i++){
  //     Serial.print(scanResult[i],HEX);
  // }
  //Serial.println((String)"numberOfTags: " + numberOfTags);

  //Extract ID numbers from the whole Hex array and convert to char array
  char *tagID;
  tagID = getTagfromHEX(scanResult, numberOfTags);
  // Serial.println((String)"getTagfromHEX: " + tagID);


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
  digitalWrite (SPEAKER, HIGH);
  //digitalWrite (YELLOWLED, HIGH); // Yellow On
 }
  if (warning == 0)
 {
  digitalWrite (REDLED, LOW); // Red Off
  digitalWrite (SPEAKER, LOW);
  //digitalWrite (YELLOWLED, LOW); // Yellow Off
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
  digitalWrite (GREENLED, !digitalRead(GREENLED));
  
  Serial.println((String)"number of Tags: " + numberOfTags);
  Serial.println("finished rfid");
  //END of RFID


  //Send an HTTP POST request **************************************************************************************
  //Check WiFi connection status
  if(WiFi.status()== WL_CONNECTED){
    WiFiClient client;
    HTTPClient http;
    
    // Your Domain name with URL path or IP address with path
    http.begin(client, serverName);

    // // Specify content-type header
    // http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    // // Data to send with HTTP POST
    // String httpRequestData = "api_key=tPmAT5Ab3j7F9&sensor=BME280&value1=24.25&value2=49.54&value3=1005.14";           
    // // Send HTTP POST request
    // int httpResponseCode = http.POST(httpRequestData);
    
    // If you need an HTTP request with a content type: application/json, use the following:
    http.addHeader("Content-Type", "application/json");
    String data = buildData(scanresult, "timestamp", (String)weight, "textxtray1");
    int httpResponseCode = http.POST(data);

    // If you need an HTTP request with a content type: text/plain
    //http.addHeader("Content-Type", "text/plain");
    //int httpResponseCode = http.POST("Hello, World!");
    
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
      
    // Free resources
    http.end();
  }
  else {
    Serial.println("WiFi Disconnected");
  }
  //END of Send an HTTP POST request **************************************************************************************

  // Print ellapsed time per loop
  myTime = millis()-myTime;
  Serial.println((String)"Elapsed time per loop: " + myTime);

  //delay(5000);

}


//funcitons def

String buildData(String id, String timestamp, String weight, String unitinfo) { //组建发送的json字符串
  String payload = "{\"unitinfo\":\"";
  payload += unitinfo;
  payload += "\",\"data\":{\"weight\":\"";
  payload += weight;
  payload += "\",\"uids\":[";
  payload += id;
  payload += "]}}";
  //Serial.println((String)"payload: " + payload);
  return payload;
  //  char attributes[100];
  //  payload.toCharArray( attributes, 100 );
}

void weightscaleInit(uint8_t DT_PIN, uint8_t SCK_PIN){
  scale.begin(DT_PIN, SCK_PIN);
  scale.set_scale(SCALEFACTOR);  // 開始取得比例參數                      // this value is obtained by calibrating the scale with known weights; see the README for details

}

void wifiInit(){
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
}

uint8_t multiscan(uint8_t result[], unsigned long timeout){
  //returns the number of tags 
  //resutl = the resulting scan as result array in HEX
  //timeout = max time to wait after each Serial read
  const byte readMulti[] = {0xBB, 0x00, 0x27, 0x00, 0x03, 0x22, 0x00, 0x0A, 0x56, 0x7E}; //x10
  bool finished_scan = false; 
  uint8_t tags = 0;
  uint16_t i = 0;
  unsigned long currentMillis=0; // millis() returns an unsigned long.
  SerialRFID.write(readMulti, sizeof(readMulti));
  currentMillis = millis(); //reset timer;
     
  while (!finished_scan){
    while (SerialRFID.available()) {
      result[i] = SerialRFID.read();
      // Serial.print(result[i],HEX); 
      // Serial.print(",");
      
      if (result[i] == 0x7e){
        // Serial.print(" ");
        tags++; 
      }
      i++;
      currentMillis = millis(); //reset timer;
    }
  
    
    if(millis() - currentMillis > timeout){ //if timeout is reached than finish reading
      Serial.println("finished Read");

      //DEBUGGING
      //Serial.println((String)"number of Tags: " + tags);
      //print result
  //      for(uint16_t j = 0; j < i; j++){
  //        Serial.print(result[j],HEX);
  //        Serial.print(",");
  //      }
      //Serial.println((String)"i: " + i);
      //END DEBUGGING
      
      finished_scan = true;
      return tags;
    }
  }
  return 0;
}