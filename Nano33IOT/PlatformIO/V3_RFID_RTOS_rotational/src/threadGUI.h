#ifndef THREAD_GUI_h
#define THREAD_GUI_h

#include "main.h"
// #include "threadHX711.h"
// #include "SD_CARD.h"

TaskHandle_t Handle_GUI;

void displaySavedData();


static void threadGUI(void *pvParameters)
{
  Serial.println("Thread read GUI: Started");

  while (1)
  {
    myDelayMs(300);
    if (Serial.available())
    {
      String incomingString = Serial.readStringUntil('\n');
      char d1 = incomingString.charAt(0);
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

      // else if (d1 == 'S')
      // { // save wifi was clicked
      //   debugln("save wifi");
      //   x = incomingString.substring(1);
      //   debugln((String) "incoming String: " + x);
      //   uint8_t xlen = x.length();
      //   debugln((String) "xlen: " + xlen);
      //   for (uint8_t i = 0; i < xlen; i++)
      //   {
      //     if (x.substring(i, i + 2) == ",")
      //     {
      //       // for(uint8_t j = i+2; j<xlen; j++){

      //       // if(x.substring(j,j+2) == ":;"){
      //       String pwd = x.substring(i + 2); //"x.length()-1"  removes the "new line character"
      //       pwd.trim();
      //       String ssid = x.substring(0, i);
      //       ssid.trim();
      //       debugln((String) "save wifi ssid: " + ssid);
      //       debugln((String) "save wifi pwd: " + pwd);
      //       saveWifi(ssid, pwd);
      //       displaySavedData();
      //       // change to other wifi
      //       WiFi.end(); // todo: test
      //       status = WL_IDLE_STATUS;
      //       debugln((String) "WL_IDLE_STATUS: " + WL_IDLE_STATUS);
      //       wifiInit();
      //       //}
      //       //}
      //     }
      //   }
      // }


      else if (d1 == 'F')
      { // save scalefactor was clicked
        String x = incomingString.substring(1);
        debugln(x);
        float scalefactor = x.toFloat();
        debugln(scalefactor);
        scale.set_scale(scalefactor);
        writeSD_scale(x);
        displaySavedData();
      }

      else if (d1 == 't')
      { // tare was clicked

        scale.tare(10);

      }
      // END of GUI
    }
  }
  // delete ourselves.
  Serial.println("Task threadGUI: Deleting");
  vTaskDelete(NULL);
}


void displaySavedData(void){
  Serial.print("W");
  Serial.println(readWifiSSID());
  Serial.print("F");
  Serial.println(readSDscale());
}

#endif