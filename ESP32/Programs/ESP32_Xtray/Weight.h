#ifndef WEIGHT_h
#define WEIGHT_h

#include "Arduino.h"
#include "Main.h"

void WeightInit()
{
  scale.begin(DT_PIN, WEIGHT_SCK_PIN);
  scale.set_scale(calibration_factor);
  // String offsetstr = readSDreset();
  // String calistr = readSD_aotuca();

  // if (offsetstr != "")
  // { // if there is a value saved on sd card
  //   int offsetint = offsetstr.toInt();
  //   scale.set_offset(offsetint);
  // }
  // else
  // {
  //   scale.tare(10);
  //   SD.remove("RESET.txt"); // delete reset.txt, so only last offset will be saved
  //   writeSD_reset((String)scale.get_offset());
  // }
  //   if (calistr != "")
  // { // if there is a value saved on sd card
  //   int caliint = calistr.toInt();
  //   calibration_factor=caliint;
  //   scale.set_scale(caliint);
  // }
  // else
  // {
  //   scale.set_scale(calibration_factor);
  //   SD.remove("CALIB.txt"); // delete reset.txt, so only last offset will be saved
  //   writeSD_autoca((String)calibration_factor);
  // }

  scale.tare(10);
}

float weightRead(float& weightD)
{
  float average = 0;
  average = scale.get_units(3),4;
  Serial.print("Reading: ");
  Serial.print(average,4);
  WeightKg = average;
  weightD = average * 1000;

    if ((average_last > average + 0.015 || average_last < average - 0.015))
  {
    // Minimum Load defines Zero-Band
    if (average < 0.020)
    {
      average = 0;
    }
    Serial.print("\tFilter: ");
    Serial.print(average);
    average_last = average;
  }
  else
  {
    Serial.print("\tFilter: ");
    Serial.print(average_last, 2);
  }
  Serial.print(" kg"); // Change this to kg and re-adjust the calibration factor if you follow SI units like a sane person
  Serial.print(" calibration_factor: ");
  Serial.print(calibration_factor);
  Serial.println();

  return average;
}



#endif
