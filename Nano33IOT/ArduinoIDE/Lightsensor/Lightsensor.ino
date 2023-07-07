#include "ArduinoLowPower.h"

int reading = 0;

void setup() {
  pinMode(A2, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
}

void loop() {
  reading = analogRead(A2);
  if (reading <= 2000)//Light intensity threshold
  {
    Serial.println("Cabinet Opened, LED on!");
    Serial.println("The Light Intensity is" + (String)reading);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
  }
  else
  {
    Serial.println("Cabinet Closed!Going to Sleep for 8s! LED off!");
    Serial.end();
    digitalWrite(LED_BUILTIN, LOW);
    LowPower.deepSleep(8000);
    Serial.begin(115200);
    delay(500);
    Serial.println("Wake up!");
  }


}
