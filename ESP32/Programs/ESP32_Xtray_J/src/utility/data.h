#ifndef DATA_H
#define DATA_H

#include "rfid.h"


// Define the payloads and structs to be sent to the server.

/// @brief  TrayData: contains all primary sensor data.
struct TrayData
{
    const rfidset* tags;
    float weight;
    float temp;
    float humi;
};

/// @brief  MinimalTrayData: contains minimal data to be sent when asleep.
struct MinimalTrayData
{
    float temp;
    float humi;
};

/// @brief  MetaData: information about this board and unit.
struct MetaData
{
    String unitinfo;
    String version;
    String serial;

    static MetaData get();
};


const String& getUnitInfo();

unsigned getLogLevel();
void setLogLevel(unsigned level);

String getActiveComponents();

String buildJSON(const MetaData& metadata, const MinimalTrayData& data);
String buildJSON(const MetaData& metadata, const TrayData& data);


#endif
