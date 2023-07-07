#include "HX711.h"
#include <SimpleKalmanFilter.h>

//e_mea: Measurement Uncertainty - How much do we expect to our measurement vary
//e_est: Estimation Uncertainty - Can be initilized with the same value as e_mea since the kalman filter will adjust its value.
//q: Process Variance - usually a small number between 0.001 and 1 - how fast your measurement moves. Recommended 0.01. Should be tunned to your needs.
//SimpleKalmanFilter kf = SimpleKalmanFilter(e_mea, e_est, q);
SimpleKalmanFilter simpleKalmanFilter(.004, .004, 0.025);

// Serial output refresh time
const long SERIAL_REFRESH_TIME = 100;
long refresh_time;

float baseline; // baseline weight

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = D7; //D7;
const int LOADCELL_SCK_PIN = D8; //D8;

float calibration_factor = 22250; //=100g

HX711 scale;

void setup() {
  Serial.begin(115200);
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);

  scale.set_scale(calibration_factor);                      // this value is obtained by calibrating the scale with known weights; see the README for details
  scale.tare();                // reset the scale to 0

  baseline = scale.get_units();
}

void loop() {

  float measured_value = scale.get_units();
  float estimated_value = simpleKalmanFilter.updateEstimate(measured_value);

  // send to Serial output every 100ms
  // use the Serial Ploter for a good visualization
  if (millis() > refresh_time) {
//    Serial.print("measured_value:");
    Serial.print(measured_value,2);
//    Serial.print("\testimated_value:");
    Serial.print("\t");
    Serial.print(estimated_value,2);
//    Serial.print("\tcalibration_factor:");
//    Serial.print(calibration_factor,3);
    Serial.println();
    
    refresh_time = millis() + SERIAL_REFRESH_TIME;

  }

  if(Serial.available())
  {
    char temp = Serial.read();
    if(temp == '+' || temp == 'a'){
      calibration_factor += 100;
      scale.set_scale(calibration_factor);
    }
    else if(temp == '-' || temp == 'z'){
      calibration_factor -= 100;
      scale.set_scale(calibration_factor);
    }
    else if(temp == 't'){
      scale.tare(20);
    }
  }
  
}
