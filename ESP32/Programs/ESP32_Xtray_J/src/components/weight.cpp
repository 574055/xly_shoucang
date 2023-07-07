#include "defines.h"
#include "filters.h"
#include "main.h"
#include "sdloader.h"
#include "weight.h"

#include <CircularBuffer.h>
#include <HX711.h>


HX711 hx711;
float hx711Scale;
float hx711Offset;

float rawWeight = 0;
float filteredWeight = 0;
float stableRawWeight = 0;
float stableFilteredWeight = 0;

// Object to filter a stream of values.
// Filter::Average<DATA_BUFFER_SIZE> filter;
Filter::Kalman filter{0.004, 0.004, 0.025};
// Filter::Decaying<DATA_BUFFER_SIZE> filter2{0.9};
// Filter::Decaying<DATA_BUFFER_SIZE> filter3{0.5};

#define DEBUG_OPTION DEBUG_WEIGHT_DATA
#include "debug.h"


Filter::Stable<> stableRawFilter;
Filter::Stable<> stableFilteredFilter;


void weightInit()
{
    hx711.begin(WEIGHT_SDA_PIN, WEIGHT_SCK_PIN);
    // weightTare();
}

void weightTare(size_t n)
{
    hx711.tare(n);
    hx711Offset = hx711.get_offset();
    weightUpdateOffset();

    // Reset weight values.
    filter.reset();
    stableFilteredFilter.reset();
    stableRawFilter.reset();
}

void weightUpdate()
{
    // Read one sample.
    float data = rawWeight = hx711.get_units(1);

    // Add filter data and get the filtered value.
    filteredWeight = filter.apply(data);

    // float filtered2 = filter2.apply(data);
    // float filtered3 = filter3.apply(data);

    debugf("raw:%.5f,", rawWeight);
    debugf("filt:%.5fkg,", filteredWeight);
    // debugf("cal:%.3f,", HX711_CALIBRATION_FACTOR);
    debugln();
}

void weightUpdateStable(float epsilon)
{
    weightUpdate();

    stableRawWeight = stableRawFilter.apply(rawWeight, epsilon);
    stableFilteredWeight = stableFilteredFilter.apply(filteredWeight, epsilon);

    debugf("stabr:%.5f,", stableRawWeight);
    debugf("stabf:%.5f,", stableFilteredWeight);
    // debugf("stabf:%.5f,", stableFilteredWeight);
    // debugf("epsilon:%.3f,", epsilon);
    debugln();
}

float weightGetRaw() { return rawWeight; }
float weightGetFiltered() { return filteredWeight; }
float weightGetStableRaw() { return stableRawWeight; }
float weightGetStableFiltered() { return stableFilteredWeight; }


void weightLoadData()
{
    hx711Scale = sdLoadScale();
    hx711Offset = sdLoadOffset();
    weightUpdateScale();
    weightUpdateOffset();
}

const float& weightGetScale() { return hx711Scale; }

const float& weightGetOffset() { return hx711Offset; }

void weightSetScale(float scale)
{
    hx711Scale = scale;
    weightUpdateScale();
}

void weightSetOffset(float offset)
{
    hx711Offset = offset;
    weightUpdateOffset();
}

void weightUpdateScale()
{
    hx711.set_scale(hx711Scale);
    sdSaveCalibration(hx711Scale);
}

void weightUpdateOffset()
{
    hx711.set_offset(hx711Offset);
    sdSaveOffset(hx711Offset);
}


void weightDisplay() { oled.printf("%6.3fkg\n", stableFilteredWeight); }