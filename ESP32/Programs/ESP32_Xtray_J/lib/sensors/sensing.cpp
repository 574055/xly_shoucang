#include "sensing.h"

#include "defines.h"

#include <Adafruit_Sensor.h>
#include <cmath>


SensorsClass Sensors{DHT_SENSOR_PIN, DHT_TYPE};


SensorsClass::SensorsClass(uint8_t dhtPin, uint8_t dhtType) : dht{dhtPin, dhtType}, temp{dht}, humi{dht} {}


void SensorsClass::init()
{
    temp.init();
    // humi.init();
    light.init();
}

void SensorsClass::Temperature::init()
{
    dht.begin();
}

float SensorsClass::Temperature::get() const
{
    float t = dht.readTemperature();
    return isnan(t) ? 0.0 : t;
}

bool SensorsClass::Temperature::isAvailable() const
{
    return !isnan(dht.readTemperature());
}

void SensorsClass::Humidity::init()
{
    // Keep temp init and humi init separated, in case some future sensor separates the two...
    // ...in which case we just need to change the implementation.
    dht.begin();
}

float SensorsClass::Humidity::get() const
{
    float t = dht.readHumidity();
    return isnan(t) ? 0.0 : t;
}

bool SensorsClass::Humidity::isAvailable() const
{
    return !isnan(dht.readHumidity());
}

void SensorsClass::Light::init()
{
    pinMode(LIGHT_SENSOR_PIN, INPUT);
}

float SensorsClass::Light::get() const
{
    return 1.0f
           - float(min(max(uint32_t(getRaw()), LIGHT_MIN_VALUE), LIGHT_MAX_VALUE) - LIGHT_MIN_VALUE)
                 / (LIGHT_MAX_VALUE - LIGHT_MIN_VALUE);
}

float SensorsClass::Light::getRaw() const
{
    return analogRead(LIGHT_SENSOR_PIN);
}

bool SensorsClass::Light::isAvailable() const
{
    return analogRead(LIGHT_SENSOR_PIN) != 0;
}
