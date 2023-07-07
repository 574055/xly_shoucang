#ifndef THREAD_HX711_h
#define THREAD_HX711_h

#include "main.h"
#include "HX711.h"
#include <SimpleKalmanFilter.h>


#define SCALE_FACTOR 20000

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = A0;
const int LOADCELL_SCK_PIN = A1;


HX711 scale;
SimpleKalmanFilter simpleKalmanFilter(2, 2, 10);

void weightscaleInit(float SCALEFACTOR);

TaskHandle_t Handle_HX711;

static void threadHX711(void *pvParameters)
{
  Serial.println("Thread read HX711: Started");

  while (1)
  {
    if (scale.wait_ready_retry(10)) {
      measured_weight = scale.get_units();
      estimated_weight = simpleKalmanFilter.updateEstimate(measured_weight);
      // SERIAL.println((String)"measured weight: " + measured_weight);
      Serial.println((String)"estimated weight: " + estimated_weight);
    } else {
      Serial.println("HX711 not found. or is currently taring");
    }
    myDelayMs(100);

  }

  // delete ourselves.
  Serial.println("Task threadReadHX711: Deleting");
  vTaskDelete(NULL);

}

void weightscaleInit(float SCALEFACTOR){
    scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
    scale.set_scale(SCALEFACTOR);
    String offsetstr = readSDreset();

    if(offsetstr != ""){ //if there is a value saved on sd card
      int offsetint = offsetstr.toInt();
      debugln((String)"offset: " + offsetint);
      scale.set_offset(offsetint);
    }
    else{
      Serial.println((String)"tare ");
      scale.tare(10);
    }
    
}

#endif