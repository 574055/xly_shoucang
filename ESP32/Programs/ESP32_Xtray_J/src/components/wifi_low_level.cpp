#include "wifi_low_level.h"

#include "defines.h"
#include "sdloader.h"

#include <WiFi.h>
#include <esp_wifi_types.h>


WiFiLowLevel XTrayWiFi;

// NTP settings.
#define HHMMSS(H, M, S) ((H)*60 * 60 + (M)*60 + (S))

const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = HHMMSS(8, 0, 0); // TODO: make timezone configurable.
const int daylightOffset_sec = 0;

static int status = WL_IDLE_STATUS;

void WiFiLowLevel::loadSecrets()
{
    if (!sdLoadWifi(*this))
        sdSaveWifi(*this); // Save defaults.
}

void WiFiLowLevel::saveSecrets()
{
    sdSaveWifi(*this);
}


void WiFiLowLevel::init()
{
    WiFi.mode(WIFI_STA);
    connect(3000);
}

bool WiFiLowLevel::isConnected()
{
    return WiFi.isConnected();
}

bool WiFiLowLevel::connect(uint32_t timeout)
{
    const char* ssid = XTrayWiFi.ssid.c_str();
    const char* pwd = XTrayWiFi.pwd.c_str();
    WiFi.begin(ssid, pwd);
    Serial.printf("Connecting to WiFi (ssid=%s, pwd=%s)..", ssid, pwd);
    if (timeout == 0) {
        // Connect to wifi in non-blocking mode.
        Serial.println(". in non-blocking mode.");
        return true;
    }
    uint32_t start = millis();
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print('.');
        delay(200);
        if (millis() - start > timeout) {
            Serial.println(" timed out.");
            return false;
        }
    }
    Serial.printf(" connected to %s after %dms.\n", WiFi.localIP().toString().c_str(), millis() - start);
    return true;
}

void WiFiLowLevel::disconnect()
{
    bool res = WiFi.disconnect();
    if (res) {
        Serial.println("wifi disconnected");
    } else {
        Serial.println("wifi failed to disconnect");
    }
}

void WiFiLowLevel::syncTime()
{
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    struct tm timeinfo;
    if (!getTime(&timeinfo)) {
        // Couldn't sync time. Set to t=0.
        time_t epoch_ts = 0;
        struct timeval tv_ts = {.tv_sec = epoch_ts};
        settimeofday(&tv_ts, NULL);
    } else {
        Serial.print("Successfully synced datetime: ");
        Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
    }
}

bool WiFiLowLevel::getTime(struct tm* timeinfo)
{
    return getLocalTime(timeinfo);
}

void WiFiLowLevel::printTime(struct tm* timeinfo)
{
    // https://en.cppreference.com/w/cpp/chrono/c/strftime#Format_string
    Serial.println(timeinfo, "%A, %B %d %Y %H:%M:%S");
}

void WiFiLowLevel::wakeUp()
{
    WiFi.setSleep(WIFI_PS_MIN_MODEM);
}

void WiFiLowLevel::sleep()
{
    WiFi.setSleep(WIFI_PS_MAX_MODEM);
}

uint32_t getResponseCode(WiFiClient& client)
{
    // TODO: use some HTTP parser/client instead of manually parsing.
    int count = 0; // Stopping mechanism.
    do {
        String temp = client.readStringUntil(' ');
        if (temp.startsWith("HTTP")) {
            return client.readStringUntil(' ').toInt();
        }
    } while (count++ < 10);
    return 0;
}

bool WiFiLowLevel::sendHttpPostRequest(String endpoint, String data, String content_type)
{
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("Unable to send HTTP request: wifi not connected.");
        return false;
    }

    // TODO: replace with asynchronous client? Connection takes 80-500ms, which is quite a long time..
    WiFiClient client;

    uint32_t port = 8000;
    if (!client.connect(serverName.c_str(), port)) {
        Serial.printf("Unable to send HTTP request: could not connect to server %s:%d\n", serverName.c_str(), port);
        return false;
    }

    Serial.println("Sending HTTP request...");

    client.printf("POST %s HTTP/1.0\n", endpoint.c_str());
    client.printf("Host: %s\n", serverIP.c_str());
    client.printf("Accept: */*\n");
    client.printf("Content-Length: %d\n", data.length());
    client.printf("Content-Type: %s\n", content_type.c_str());
    client.println();
    client.println(data);

    Serial.println("Sending data: " + data);
    Serial.flush();

#if ENABLE_WIFI_VERIFY_200
    uint32_t resp = getResponseCode(client);
    if (resp != 200) {
        Serial.printf("Something went wrong: received HTTP response code %d.\n", resp);
        return false;
    }
#endif
    return true;
}

void WiFiLowLevel::printStatus()
{
    Serial.println("WiFi:");
    Serial.printf("  SSID: %s\n", WiFi.SSID());
    Serial.printf("  IP Address: %s\n", WiFi.localIP().toString());
    Serial.printf("  RSSI: %d\n", WiFi.RSSI());
}
