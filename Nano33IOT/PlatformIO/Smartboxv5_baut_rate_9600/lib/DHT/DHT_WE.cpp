#include "DHT_WE.h"

DHT dht(DHTPIN, DHTTYPE); // DHT11 setup

void initializeDHT()
{
    dht.begin();
}

void dhtReading(int h1, float t1)
{
    if (isnan(t1)) // if reading = NA, correct to error code
    {
        t1 = -100.00;
    }
    if (isnan(h1)) {
        h1 = -1.00;
    }
    Serial.print((String) "Temp:" + '\t' + t1); // show temperature data on the screen
    Serial.print('\t');
    Serial.print((String) "Humi:" + '\t' + h1); // show temperature data on the screen
    Serial.print('\t');
}

float temperatureReading()
{
    return dht.readTemperature();
}

int humdityReading()
{
    return dht.readHumidity();
}
