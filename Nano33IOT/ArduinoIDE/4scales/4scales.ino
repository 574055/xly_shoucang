/**
 *
 * HX711 library for Arduino - example file
 * https://github.com/bogde/HX711
 *
 * MIT License
 * (c) 2018 Bogdan Necula
 *
**/
#include "HX711.h"


// HX711 circuit wiring
const int LOADCELL1_DOUT_PIN = 2;
const int LOADCELL1_SCK_PIN = 3;
const int LOADCELL2_DOUT_PIN = 4;
const int LOADCELL2_SCK_PIN = 5;
const int LOADCELL3_DOUT_PIN = 6;
const int LOADCELL3_SCK_PIN = 7;
const int LOADCELL4_DOUT_PIN = 8;
const int LOADCELL4_SCK_PIN = 9;

HX711 scale1;
HX711 scale2;
HX711 scale3;
HX711 scale4;

#define SCALEFACTOR 100.f + 5.f

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.println("HX711 Demo");

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

  digitalWrite(LED_BUILTIN, HIGH); 
}

void loop() {
  float weight1 = scale1.get_units(5);
  float weight2 = scale2.get_units(5);
  float weight3 = scale3.get_units(5);
  float weight4 = scale4.get_units(5);
  float sum = weight1 + weight2 + weight3 + weight4;

  Serial.print("scale1:\t");
  Serial.print(weight1, 1);
  Serial.print("\tscale2:\t");
  Serial.print(weight2, 1);
  Serial.print("\tscale3:\t");
  Serial.print(weight3, 1);
  Serial.print("\tscale4:\t");
  Serial.print(weight4, 1);
  Serial.print("\tsum:\t");
  Serial.println(sum, 1);

  // scale1.power_down();			        // put the ADC in sleep mode
  // delay(5000);
  // scale1.power_up();
}
