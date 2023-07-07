/**
 * Convenience library for getting data from sensors.
 *
 * Sensei's sensors senses centaurs' scents, and scant cents send ants to San Diego.
 */
#ifndef SENSING_H
#define SENSING_H

#include <DHT.h>
#include <cstdint>


struct SensorsClass
{
    DHT dht;

    SensorsClass(uint8_t dhtPin, uint8_t dhtType);

    void init();

    struct Temperature
    {
        DHT& dht;
        void init();
        float get() const;
        bool isAvailable() const;
    } temp;

    struct Humidity
    {
        DHT& dht;
        void init();
        float get() const;
        bool isAvailable() const;
    } humi;

    struct Light
    {
        void init();
        float get() const;    // Gets normalised [0-1] reading.
        float getRaw() const; // Gets raw reading.
        bool isAvailable() const;
    } light;
};

extern SensorsClass Sensors;


#endif