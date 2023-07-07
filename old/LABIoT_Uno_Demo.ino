/**
   lab IoT demo on Arduino Uno
   LMT 2021.1.6
   Function description:
   initWifi(): start wifi connection
   initCIP(): start CIPSEND
   httpPost(): POST data
   getWeightData(): get data of weight sensor
   getRFIDData(): get data of RFID sensor, buzzer control is under this function
   buildData(): build JSON data string
   reset(): ESP reset
*/



#include<SoftwareSerial.h>
#include "HX711.h"
#include <SPI.h>
#include <MFRC522.h>
//basic setting of hardware
const String unitinfo = "1";
//WEIGHT SENSOR
const int DT_PIN = 5;
const int SCK_PIN = 6;
//BUZZER
const int BEEP = 4;
//RFID
#define RST_PIN      A0        // Reset the RFID reader
#define SS_PIN       10       // Tag selection

SoftwareSerial esp(3, 2);  // RX, TX
HX711 scale;
MFRC522 mfrc522(SS_PIN, RST_PIN);  // Setup the MFRC522

//link setting//fix in initWifi()
#define wifi_ssid "iottest"//ap address
#define wifi_pwd "11111111"//ap password

//post setting
#define url "/hwinfo"
#define server "148.70.180.108"

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ;
  }
  Serial.println("WeShare RFID reader is ready!");
  esp.begin(9600);
  //  esp.write("AT\r\n");
  //  delay(2000);

  pinMode(BEEP, OUTPUT);
  digitalWrite(BEEP, HIGH);

  SPI.begin();

  mfrc522.PCD_Init();   // Initiate the MFRC522 reader module
  scale.begin(DT_PIN, SCK_PIN);
  scale.set_scale();  // 開始取得比例參數
  scale.tare();

  initWifi();
}

void loop() {
  //串口读取互显，调试用
  //  if (esp.available())
  //    Serial.write(esp.read());
  //  if (Serial.available())
  //    esp.write(Serial.read());

  String weight = String(getWeightData());
  String id = getRFIDData();
  String timestamp = "1";
  String data = buildData(id, timestamp, weight, unitinfo);

  httpPost(data);
  delay(5000);
}

float getWeightData() {
  delay(2000);
  float current_weight = scale.get_units(10);
  float true_weight = (current_weight) / -200;
  Serial.print("Weight: ");
  Serial.println((current_weight) / -200, 0.0);
  return true_weight;
}

String getRFIDData() {
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    digitalWrite(BEEP, LOW);
    delay(300);
    digitalWrite(BEEP, HIGH);

    byte *id = mfrc522.uid.uidByte;   // Obtain the UID of Tag
    byte idSize = mfrc522.uid.size;   // Obtain the size of UID

    //    Serial.print("PICC type: ");      // Show the type of Tag
    // Judge the type by returning SAK value（mfrc522.uid.sak）
    MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
    //    Serial.println(mfrc522.PICC_GetTypeName(piccType));

    //    Serial.print("UID Size: ");       // Show the size of UID
    //    Serial.println(idSize);

    //    for (byte i = 0; i < idSize; i++) {  // Check the UIDs one by one
    //      Serial.print("id[");
    //      Serial.print(i);
    //      Serial.print("]: ");
    //      Serial.println(id[i], HEX);       // Show UID in Hex mode
    //    }
    //    Serial.println();

    char HEXid[32] = "";
    array_to_string(id, 4, HEXid);
    Serial.println(HEXid);

    mfrc522.PICC_HaltA();  // Stop repeated reading
    return HEXid;
  } else {
    return "";
  }
}

void initWifi() {
  Serial.println("initWifi");
  String cmd1 = "AT+CWJAP=\"";
  cmd1 += wifi_ssid;
  cmd1 += "\",\"";
  cmd1 += wifi_pwd;
  cmd1 += "\"";
  esp.println(cmd1);
  delay(3000);

  if (esp.find("OK")) {//获得连接ok
    Serial.println("wifi initialized.");
  } else {
    initWifi();
    Serial.println("wifi reconnect");
  }//建立对远程服务器的TCP连接
}

void initCIP() {
  Serial.println("initCIP()");
  String cmd2 = "AT+CIPSTART=\"TCP\",\"";
  cmd2 += server;
  cmd2 += "\",5000";
  esp.println(cmd2);//start a TCP connection.
  if (esp.find("OK")) {
    Serial.println("server connected");
  }
}

void httpPost (String data) {//post data
  Serial.println("httpPost():" + data);
  initCIP();
  String postRequest = "POST ";
  postRequest += url;
  postRequest += " HTTP/1.0\r\n";
  postRequest += "Host: " ;
  postRequest += server;
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

  String sendCmd = "AT+CIPSEND=";//determine the number of caracters to be sent.
  esp.print(sendCmd);
  esp.println(postRequest.length() );
  delay(500);
  if (esp.find(">")) {
    Serial.println("Sending..");
    esp.print(postRequest);
    if ( esp.find("SEND OK")) {
      Serial.println("Packet sent");
      while (esp.available()) {
        String tmpResp = esp.readString();
        Serial.println(tmpResp);
      }
      // close the connection
//            esp.println("AT+CIPCLOSE");
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


void reset() {//重置函数

  esp.println("AT+RST");

  delay(1000);

  if (esp.find("OK") ) Serial.println("Module Reset");

}

void array_to_string(byte array[], unsigned int len, char buffer[]) { //byte array to HEX string
  for (unsigned int i = 0; i < len; i++)
  {
    byte nib1 = (array[i] >> 4) & 0x0F;
    byte nib2 = (array[i] >> 0) & 0x0F;
    buffer[i * 2 + 0] = nib1  < 0xA ? '0' + nib1  : 'A' + nib1  - 0xA;
    buffer[i * 2 + 1] = nib2  < 0xA ? '0' + nib2  : 'A' + nib2  - 0xA;
  }
  buffer[len * 2] = '\0';
}
