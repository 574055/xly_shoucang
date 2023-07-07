#ifndef WIFI_h
#define WIFI_h

#include "main.h"
#include <WiFiNINA.h>
#include "arduino_secrets.h"
#include "oled.h"
#include "SD_CARD.h"

#define BLUELED 10   //is on when not wifi connection error
#define GREENLED 9     //toggles after send data

int keyIndex = 0;            // your network key index number (needed only for WEP)

// Initialize the WiFi client library
WiFiClient client;

#define url "/lab/hwinfo"
#define serverno "148.70.180.108" //developing
IPAddress server(148,70,180,108);
// #define serverno "18.162.252.107" //serverno and server have to be the same
// IPAddress server(18,162,252,107);
const String unitinfo = "testxtrayB";

int status = WL_IDLE_STATUS;
bool firstNotConnected = false;
unsigned long looptime, noWifiTime = 0;

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
  //get SSID from SD card
  String wifiSSID = readWifiSSID();
  String wifiPASS = readWifiPASS();
  // Serial.println((String)"wifiSSID: " + wifiSSID);
  // Serial.println((String)"wifiPASS: " + wifiPASS);

  char wifiSSIDchar[20], wifiPasschar[20];
  wifiSSID.toCharArray(wifiSSIDchar, 20);
  // wifiPASS.toCharArray(wifiPasschar, 20);

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  long prev_time = millis();
  long tim = 0;
  bool con = true;
  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    // Serial.print("Attempting to connect to SSID: ");
    // Serial.println(wifiSSID);
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println((String)"trying to connect to: "+ wifiSSID);
    display.print((String)"PWD: "+ wifiPASS);
    display.display();
    // Serial.println((String)"WifiInit SSID: " + wifiSSID);
    // Serial.println((String)"WifiInit PWD: " + wifiPASS);

    status = WiFi.begin(wifiSSIDchar, wifiPasschar);

    // wait 10 seconds for connection:
    delay(10000);
    tim = millis() - prev_time;
    if (tim > 20000){ //0.5 min
      con = false;
      break;
    }
  }
  // you're connected now, so print out the status:
  // printWifiStatus();
  if (con){ //could connect
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(0, 0);
    display.println("Connected!");
    display.display();
    digitalWrite(BLUELED,LOW);    
    //send to GUI
    Serial.println((String)"A"); //succesfully connectto Wifi, send to GUI
  }
  else{ //connection failed
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(0, 0);
    display.println("Could not connect to Wifi");
    display.display();
    Serial.println("could not connect to wifi");
    digitalWrite(BLUELED,HIGH);
    //send to GUI
    Serial.println((String)"a"); //could not connect to Wifi, send to GUI
  }
  firstNotConnected = false;
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

    digitalWrite(BLUELED, LOW);
    
//    client.println(postRequest);
//    client.println("Connection: close");
//    client.println();

  } else {
    // if you couldn't make a connection:
    Serial.println("connection failed");
    status = WL_IDLE_STATUS;
    WiFi.end();
    digitalWrite(BLUELED, HIGH);
  }
}

String prepareDataForHttpReq(String finalTags, uint8_t numberOfTags){
    uint8_t i = 0;
    String scanresult;
    while(i<numberOfTags){
      scanresult += "\"" + finalTags.substring(8 * i,8 + 8 * i) + "\",";
      i++;
    }
    int length = scanresult.length();
    scanresult.remove(length - 1); //remove the last comma
    
    // Serial.println(scanresult); 
    // Serial.println((String)"number of Tags: " + numberOfTags);
    // Serial.println("finished rfid");
    return scanresult;
}

void sendData2Server(String data){
    if (status == WL_CONNECTED){ //only send data if connected to wifi
    httpRequest(data);
    }
    //check connection and connect to wifi if duration has passed
    else{
      if(!firstNotConnected){ //save time when first-time not connected to wifi
        // Serial.println("first time not connected");
        firstNotConnected = true;
        noWifiTime = millis();
      }
      if (millis() - noWifiTime > 1000*10*5){ //5 min
        noWifiTime = millis();
        wifiInit();
      } 
    }

    digitalWrite (GREENLED, !digitalRead(GREENLED)); //toggle led
}
#endif