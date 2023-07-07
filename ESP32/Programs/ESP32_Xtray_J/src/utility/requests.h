#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "data.h"
#include <Arduino.h>


enum RequestAction
{
    RequestHumidity,
    RequestTemperature,
    RequestRFID,
    RequestWeight,
    NUM_REQUESTS,
};


void parseSerialRequest(const String& line);

void resetSensorRequests();
bool isRequested(RequestAction req);
void handleSensorRequests(const TrayData& data);

#endif