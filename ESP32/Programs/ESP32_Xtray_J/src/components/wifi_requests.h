#ifndef WIFI_REQUEST_H
#define WIFI_REQUEST_H

#include "data.h"

bool wifiUpdateTrayData(const TrayData& data);
bool wifiUpdateMinimalTrayData(const MinimalTrayData& data);

void wifiTrySend(const TrayData& data);
void wifiTrySend(const MinimalTrayData& data);

#endif