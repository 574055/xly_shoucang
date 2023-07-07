#ifndef WIFIE_h
#define WIFIE_h

#include "Arduino.h"
#include "Main.h"
#include "Oled.h"

//please add two characters after the SSID and PASS. e.g. "passwordXX". It is due to a bug.
#define SECRET_SSID "35Fqq"
#define SECRET_PASS "44444444qq"

// #define SECRET_SSID "WeShare-6105qq"
// #define SECRET_PASS "1234567890qq" 

WiFiClient client;
int status = WL_IDLE_STATUS;

void wifiInit()
{
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
  // if (WiFi.status() == WL_NO_MODULE)
  // {
  //   Serial.println("Communication with WiFi module failed!");
  // }

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
    Serial.println("length" + (String)namelen +(String)passlen);

    Serial.print(wifipass);Serial.println();
    status = WiFi.begin(wifiname, wifipass);
    delay(3000);

    tim = millis() - prev_time;
    if (tim > 20000)
    { // 0.5 min
      con = false;
      break;
    }
  }
  // you're connected now, so print out the status:
  //printWifiStatus();
  if (con)
  { // could connect
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(0, 0);
    display.println("Connected!");
    display.display();
    // digitalWrite(BLUELED, LOW);
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
    // digitalWrite(BLUELED, HIGH);
    // send to GUI
    Serial.println((String) "a"); // could not connect to Wifi, send to GUI
  }
  firstNotConnected = false;
}


#endif
