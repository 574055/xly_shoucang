#ifndef WEIGHTS_H
#define WEIGHTS_H

#include "defines.h"

#include <cstddef>


// Initialise the HX711, ports, etc.
void weightInit();

// Zero-calibrate the scale.
void weightTare(size_t n = WEIGHT_TARE_N);

// Read one sample.
void weightUpdate();

// Read a stable value.
void weightUpdateStable(float epsilon = WEIGHT_EPSILON);

// Get stored values.
float weightGetRaw();
float weightGetFiltered();
float weightGetStableRaw();
float weightGetStable();


// Handle weight parameters.

// Raw analogue (voltage) readings are mapped to real readings through linear regression.
// There are two parameters involved: scale and offset. The relation is handled the usual way.
//      output = (input - offset) / scale
// Scale can be determined by using a weight and correcting the weight reading according to the actual weight.
// Offset can be computed automatically by tare (weightTare), which zero-calibrates the tray and calculates the offset.

void weightLoadData();

const float& weightGetScale();
void weightSetScale(float scale);
void weightUpdateScale();

const float& weightGetOffset();
void weightSetOffset(float offset);
void weightUpdateOffset();


// Display weights on the OLED.
void weightDisplay();

#endif