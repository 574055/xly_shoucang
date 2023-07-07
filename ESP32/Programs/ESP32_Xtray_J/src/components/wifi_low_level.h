#ifndef WIFI_LOW_LEVEL_H
#define WIFI_LOW_LEVEL_H

#include "defines.h"

#include <Arduino.h>
#include <time.h>


class WiFiLowLevel
{
public:
    // Connection settings.
    String ssid = WIFI_DEFAULT_SSID;
    String pwd = WIFI_DEFAULT_PWD;

    // Target server settings.
    String serverName = WIFI_DEFAULT_SERVERNAME;
    String serverIP = WIFI_DEFAULT_SERVERIP;

    /// @brief Load/save wifi settings (ssid/pwd) from the sd card.
    void loadSecrets();
    void saveSecrets();

    void init();
    bool isConnected();
    bool connect(uint32_t timeout = 0);
    void disconnect();

    /// @brief  Configures wifi to be awake/sleeping.
    void wakeUp();
    void sleep();

    /// @brief  Sends data through a http POST request to the specified endpoint.
    bool sendHttpPostRequest(String endpoint, String data, String content_type = "application/json");

    /// @brief  Prints wifi status.
    void printStatus();

    /// @brief  Syncs with the NTP server.
    void syncTime();
    bool getTime(struct tm* timeinfo);
    void printTime(struct tm* timeinfo);
};

extern WiFiLowLevel XTrayWiFi;


#endif