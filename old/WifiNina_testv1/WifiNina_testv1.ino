/*
  Repeating WiFi Web Client

 This sketch connects to a a web server and makes a request
 using a WiFi equipped Arduino board.

 created 23 April 2012
 modified 31 May 2012
 by Tom Igoe
 modified 13 Jan 2014
 by Federico Vanzati

 http://www.arduino.cc/en/Tutorial/WifiWebClientRepeating
 This code is in the public domain.
 */

#include <SPI.h>
#include <WiFiNINA.h>

#include "arduino_secrets.h" 


///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;            // your network key index number (needed only for WEP)
String data = "{\"Targetid\":\"000001123\",\"Timestamp\":\"1\",\"Weight\":50,\"Unitinfo\":\"1\"}";

int status = WL_IDLE_STATUS;

// Initialize the WiFi client library
WiFiClient client;

// server address:
//char server[] = "example.org";
//char server[] = "http://api.thingspeak.com/update";


#define url "/hwinfo"
#define serverno "148.70.180.108"
IPAddress server(148,70,180,108);

unsigned long lastConnectionTime = 0;            // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 300L; //1L * 1000L; // delay between updates, in milliseconds

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

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
  // if there's incoming data from the net connection.
  // send it out the serial port.  This is for debugging
  // purposes only:
  while (client.available()) {
    char c = client.read();
    Serial.write(c);
  }

  // if ten seconds have passed since your last connection,
  // then connect again and send data:
  if (millis() - lastConnectionTime > postingInterval) {
    httpRequest();
  }

}

// this method makes a HTTP connection to the server:
void httpRequest() {
  // close any connection before send a new request.
  // This will free the socket on the NINA module
  client.stop();

  // if there's a successful connection:
  if (client.connect(server, 5000)) {
    Serial.println("connecting...");
    // send the HTTP GET request:
//    client.println((String)"POST " + url + " HTTP/1.1");
//    client.println((String)"Host: " + serverno);
//    client.println("Accept: */*");
//    client.println((String)"Content_Length: " + data.length());
//    client.println((String)"Contnent_Type: application/json");
//    client.println();
//    client.println(data);
//    client.println("Connection: close");
//    client.println();


    
//Only to check wehter the send string is ident
//    Serial.println((String)"POST " + url + " HTTP/1.0");
//    Serial.println((String)"Host: " + serverno);
//    Serial.println("Accept: */*");
//    Serial.println((String)"Content_Length: " + data.length());
//    Serial.println((String)"Contnent_Type: application/json");
//    Serial.println();
//    Serial.println(data);
//    Serial.println("Connection: close");
//    Serial.println();
//
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


    // note the time that the connection was made:
    lastConnectionTime = millis();
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
