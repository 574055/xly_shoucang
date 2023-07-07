#include<SoftwareSerial.h>
SoftwareSerial esp(10, 9);  // RX, TX

//link setting//fix in initWifi()
//#define wifi_ssid "iottest"//ap address
//#define wifi_pwd "11111111"//ap password
#define wifi_ssid "LSK 7028"//ap address
#define wifi_pwd "wesharetechnology"//ap password

//post setting
#define url "/hwinfo"
#define server "148.70.180.108"

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    ;
  }
  Serial.println("hardware serial!");
  esp.begin(9600);
  esp.println("software serial");


  String cmd1 = "AT+CWJAP=\"";
  cmd1 += wifi_ssid;
  cmd1 += "\",\"";
  cmd1 += wifi_pwd;
  cmd1 += "\"";
  esp.println(cmd1);
  delay(5000);

  if (esp.find("OK")) {//获得连接ok
    esp.write("AT+CIFSR\r\n");//打IP地址
    Serial.println("ESP OK");
  } else {
    Serial.println("wifi reconnect");
  }//建立对远程服务器的TCP连接
//  String cmd2 = "AT+CIPSTART=\"TCP\",\"";
//  cmd2 += server;
//  cmd2 += "\",5000";
//  esp.println(cmd2);//start a TCP connection.
//  if (esp.find("OK")) {
//    Serial.println("server connected");
//  }

}

void loop() {
//  Serial.println("inloop");
//  if (esp.available())
//    Serial.write(esp.read());
//
//  if (Serial.available())
//    esp.write(Serial.read());


  httpPost();
  delay(5000);

}
void httpPost () {//post data
  Serial.println("inpost");
  String cmd2 = "AT+CIPSTART=\"TCP\",\"";
  cmd2 += server;
  cmd2 += "\",5000";
  esp.println(cmd2);//start a TCP connection.
  if (esp.find("OK")) {
    Serial.println("server connected");
  }
  String data = "{\"Targetid\": 1, \"Timestamp\":\"00\", \"Weight\":20, \"Unitinfo\": 1}";
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
//  Serial.println(sendCmd);
  esp.println(postRequest.length() );
  delay(500);

  
  if (esp.find(">")) {
    Serial.println("Sending.."); esp.print(postRequest);
    if ( esp.find("SEND OK")) {
      Serial.println("Packet sent");
      while (esp.available()) {
        String tmpResp = esp.readString();
        Serial.println(tmpResp);
      }
      //       close the connection
//      esp.println("AT+CIPCLOSE");
    }
  }
}
