#include "defines.h"
#include "main.h"
#include "requests.h"
#include "rfid.h"
#include "sdloader.h"
#include "sensing.h"
#include "stringreader.h"
#include "weight.h"
#include "wifi_low_level.h"
#include "wifi_requests.h"

#include <WiFi.h>
#include <base64.hpp>


bool sensorRequests[NUM_REQUESTS];


void requestInfo(RequestAction req) { sensorRequests[req] = true; }


void resetSensorRequests()
{
    for (int i = 0; i < NUM_REQUESTS; i++)
        sensorRequests[i] = false;
}


bool isRequested(RequestAction req) { return sensorRequests[req]; }


#define ATTR_VARIABLE attr
#define CMD_VARIABLE  cmd

// Convenience macros for declaring action and attribute blocks. Some variable names are hard-coded.
#define ALTERNATIVE(VAR, ALT, BLOCK...) \
    if (VAR == ALT) {                   \
        BLOCK;                          \
        return true;                    \
    }
#define ACTION(CMD, BLOCK...) ALTERNATIVE(CMD_VARIABLE, CMD, BLOCK)

#define ATTR(ATTR_STRING, GET_BLOCK, SET_BLOCK...)            \
    ALTERNATIVE(ATTR_VARIABLE, ATTR_STRING, {                 \
        if (!reader.available()) {                            \
            /* No arguments. Just get, print, and bye bye. */ \
            Serial.println("getting...");                     \
            GET_BLOCK;                                        \
        } else {                                              \
            /* There are arguments to parse... */             \
            Serial.println("setting...");                     \
            SET_BLOCK;                                        \
        }                                                     \
    })

// Specific type of action which requests for sensor data.
#define ACTION_REQUEST(CMD, REQ, BLOCK)     \
    ALTERNATIVE(CMD_VARIABLE, CMD, {        \
        Serial.println("requesting " #CMD); \
        BLOCK;                              \
        requestInfo(REQ);                   \
    })

// Helper macro for printing attributes, to reply attr requests.
#define PRINT_ATTR(NAME, FMT, ...)  Serial.printf(NAME "=" FMT "\n", __VA_ARGS__);
#define PRINT_ATTR_ON_GET(FMT, ...) PRINT_ATTR("%s", FMT, toString(ATTR_VARIABLE).c_str(), __VA_ARGS__);

// OK macros. Use this when a request completes successfully.
#define OK_LOG_PREFIX "%s=OK"
#define ATTR_OK()                                                           \
    {                                                                       \
        Serial.printf(OK_LOG_PREFIX "\n", toString(ATTR_VARIABLE).c_str()); \
    }
#define ATTR_OKf(fmt, ...)                                                                       \
    {                                                                                            \
        Serial.printf(OK_LOG_PREFIX ";" fmt "\n", toString(ATTR_VARIABLE).c_str(), __VA_ARGS__); \
    }
#define ACTION_OK()                                                        \
    {                                                                      \
        Serial.printf(OK_LOG_PREFIX "\n", toString(CMD_VARIABLE).c_str()); \
    }
#define ACTION_OKf(fmt, ...)                                                                    \
    {                                                                                           \
        Serial.printf(OK_LOG_PREFIX ";" fmt "\n", toString(CMD_VARIABLE).c_str(), __VA_ARGS__); \
    }

// ERROR macros. Use this to indicate a request could not be parsed or fulfilled.
#define ERROR_LOG_PREFIX "[ERROR]"
#define ERROR()          return false
#define ERRORm(MSG)                                \
    {                                              \
        Serial.println(ERROR_LOG_PREFIX ": " MSG); \
        ERROR();                                   \
    }


/**
 * @brief   Parse and execute Actions.
 * @return  True if action was successfully executed, False if not (e.g. missing argument,
 *          failure, emotional damage, etc).
 */
bool parseAction(const etl::string_view& CMD_VARIABLE, StringReader& reader)
{
    // TODO: instead of checking actions using an if-statement, use a switch
    // statement and jump table, or use a map<string, function<void(StringReader)>>.
    // Same goes for parseAttributes.

    ACTION("wifi.conn", {
        Serial.println("connecting wifi...");
        XTrayWiFi.connect(5000);
        ACTION_OK();
    })


    ACTION("wifi.disconn", {
        Serial.println("disconnecting wifi...");
        XTrayWiFi.disconnect();
        ACTION_OK();
    })


    ACTION("weight.tare", {
        Serial.println("performing tare...");

        long oldOffset = weightGetOffset();
        weightTare();
        long newOffset = weightGetOffset();

        ACTION_OKf("updated offset: %ld --> %ld", oldOffset, newOffset);
    })


    ACTION("weight.calib", {
        // Recalibrate according to the actual weight provided from an external source.
        etl::optional<float> actualWeight = reader.readSafe<float>();
        if (!actualWeight || *actualWeight < 1e-6) {
            ERRORm("missing argument - actual weight");
        }

        float currWeight = weightGetFiltered();
        if (currWeight < 1e-8) {
            // We don't want to divide by a near-zero value...
            ERRORm("no weight reading, please check weight sensors");
        }

        float adjust = (*actualWeight / currWeight);
        float scale = weightGetScale();
        float newScale = scale / adjust;
        weightSetScale(newScale);

        ACTION_OKf("updated scale: %.3f --> %.3f (adjusted %.3f)", scale, newScale, adjust);
    })


    ACTION("weight.scale", {
        // Set the scale of HX711.
        etl::optional<float> newScale = reader.readSafe<float>();
        if (!newScale || newScale < 1e-4) {
            ERRORm("missing argument - new scale");
        }

        float oldScale = weightGetScale();
        weightSetScale(newScale.value());

        ACTION_OKf("updated scale: %.3f --> %.3f", oldScale, newScale);
    })


    ACTION("sleep.adjust", {
        // Calibrate the sleepthreshold according to the current light strength.
        // TODO: use an averaging filter or something on the light data to get more stable reading.

        // Multiply by a factor to give some leeway for the threshold.
        float factor = 1.05;

        float oldThresh = getSleepLightThreshold();
        float newThresh = Sensors.light.get() * factor;
        updateSleepLightThreshold(newThresh);
        ACTION_OKf("updated sleep threshold: %.3f --> %.3f", oldThresh, newThresh);
    })


    ACTION("sleep.threshold", {
        // Set the sleep threshold.
        etl::optional<float> newThresh = reader.readSafe<float>();
        if (!newThresh) {
            ERRORm("missing argument - sleep threshold");
        }
        if (*newThresh < 0 || newThresh > 1) {
            ERRORm("out of range - expected threshold between 0 - 1");
        }

        float oldThresh = getSleepLightThreshold();
        updateSleepLightThreshold(newThresh.value());
        ACTION_OKf("updated sleep threshold: %.3f --> %.3f", oldThresh, newThresh);
    })


    // Special requests to be returned after the logic loop.

    // Why are these designed like this? You can ask Dayan about it. I would
    // suggest breaking these apart into two sets of commands: one set to
    // request updateRFID(), updateWeight() to be called; another set to output
    // the current values stored in `TrayData data`.
    ACTION_REQUEST("humi.value", RequestHumidity, )
    ACTION_REQUEST("temp.value", RequestTemperature, )
    ACTION_REQUEST("rfid.inventory", RequestRFID, )
    ACTION_REQUEST("weight.value", RequestWeight, )
    return false;
}


char buffer[256];

String b64_encode(const char* str)
{
    encode_base64((const unsigned char*)str, strlen(str), (unsigned char*)buffer);
    return String(buffer);
}

String b64_encode(const String& str)
{
    encode_base64((const unsigned char*)str.c_str(), str.length(), (unsigned char*)buffer);
    return String(buffer);
}

String b64_decode(const char* str)
{
    size_t len = decode_base64((const unsigned char*)str, (unsigned char*)buffer);
    buffer[len] = '\0';
    return String(buffer);
}

String b64_decode(etl::string_view str)
{
    size_t len = decode_base64((const unsigned char*)str.data(), str.length(), (unsigned char*)buffer);
    buffer[len] = '\0';
    return String(buffer);
}

String b64_decode(const String& str) { return b64_decode(str.c_str()); }


/**
 * @brief   Parse and retrieve Attributes.
 * @return  True if the attribute was successfully retrieved, False if not.
 */
bool parseAttribute(const etl::string_view& ATTR_VARIABLE, StringReader& reader)
{
    ATTR("weight.scale", PRINT_ATTR_ON_GET("%.6f", weightGetScale()), {
        etl::optional<float> arg = reader.readSafe<float>();
        if (!arg || arg < 1e-8) {
            ERRORm("expected a non-zero float");
        }

        float old = weightGetScale();
        weightSetScale(*arg);
        ATTR_OKf("%.6f --> %.6f\n", old, *arg);
    })

    ATTR("weight.offset", PRINT_ATTR_ON_GET("%.6f", weightGetOffset()), {
        etl::optional<float> arg = reader.readSafe<float>();
        if (!arg) {
            ERRORm("expected a float");
        }

        float old = weightGetOffset();
        weightSetOffset(*arg);
        ATTR_OKf("%.6f --> %.6f\n", old, *arg);
    })

    ATTR("sys.cpuid", PRINT_ATTR_ON_GET("%llX", ESP.getEfuseMac()), )
    ATTR("wifi.mac", PRINT_ATTR_ON_GET("%s", WiFi.macAddress().c_str()), )
    ATTR("wifi.ip", PRINT_ATTR_ON_GET("%s", WiFi.localIP().toString().c_str()), )

    ATTR(
        "wifi.config",
        PRINT_ATTR_ON_GET("<%s,%s>", b64_encode(XTrayWiFi.ssid).c_str(), b64_encode(XTrayWiFi.pwd).c_str()), {
            auto pr_res = reader.readSafe<etl::string_view, etl::string_view>();
            if (!pr_res) {
                ERRORm("expected a pair <a,b>")
            }
            const auto& pr = *pr_res;
            if (std::get<0>(pr).empty()) {
                ERRORm("expected an ssid. How do you expect me to connect to WiFi without something to connect to? >.<")
            }
            if (std::get<1>(pr).empty()) {
                // No password. Technically not an error.
            }
            XTrayWiFi.ssid = b64_decode(std::get<0>(pr));
            XTrayWiFi.pwd = b64_decode(std::get<1>(pr));
            XTrayWiFi.saveSecrets();
            ATTR_OK();
        })

    ATTR("wifi.rssi", PRINT_ATTR_ON_GET("%d", WiFi.RSSI()), )

    ATTR("server.ip", PRINT_ATTR_ON_GET("%s", XTrayWiFi.serverIP.c_str()), {
        auto ip = toString(reader.rest());
        Serial.printf("setting ip: %s\n", ip.c_str());
        XTrayWiFi.serverIP = ip;
        XTrayWiFi.saveSecrets();
        ATTR_OK();
    })
    ATTR("server.name", PRINT_ATTR_ON_GET("%s", XTrayWiFi.serverName.c_str()), {
        auto name = toString(reader.rest());
        Serial.printf("setting name: %s\n", name.c_str());
        XTrayWiFi.serverName = name;
        XTrayWiFi.saveSecrets();
        ATTR_OK();
    })

    // TODO: directly get/set the log level of the logger (once we've decided on one...)
    ATTR("sys.loglevel", PRINT_ATTR_ON_GET("%u", getLogLevel()), {
        etl::optional<unsigned> level = reader.readSafe<unsigned>();
        if (!level) {
            ERRORm("expected a log level.");
        }
        setLogLevel(*level);
        ATTR_OK();
    })
    return false;
}


void parseSerialRequest(const String& line)
{
    StringReader reader{line};

    // Handle Serial input.
    auto category = reader.readToken(':');
    if (category == "cmd") {
        auto cmd = reader.readToken('=');
        if (!parseAction(cmd, reader)) {
            Serial.printf("unable to fulfil action: '%s'\n", toString(cmd).c_str());
        }
    } else if (category == "attr") {
        auto attr = reader.readToken('=');
        if (!parseAttribute(attr, reader)) {
            Serial.printf("unable to get attribute: '%s'\n", toString(attr).c_str());
        }
    } else {
        tuneset.read(toString(reader.rest()));
    }
}


void handleSensorRequests(const TrayData& data)
{
    if (isRequested(RequestHumidity)) {
        PRINT_ATTR("humi.value", "%.6f", Sensors.humi.get());
    }
    if (isRequested(RequestTemperature)) {
        PRINT_ATTR("temp.value", "%.6f", Sensors.temp.get());
    }
    if (isRequested(RequestRFID)) {
        String s;
        s += "[";
        if (data.tags) {
            size_t left = data.tags->size();
            char buffer[20];
            for (auto it = data.tags->begin(); it != data.tags->end(); ++it, --left) {
                snprintf(buffer, sizeof(buffer), "(%d,%08x)", rfid.rssi(*it), it->id);
                s += buffer;
                if (left > 1)
                    s += ",";
            }
        }
        s += "]";
        PRINT_ATTR("rfid.inventory", "%s", s.c_str());
    }
    if (isRequested(RequestWeight)) {
        PRINT_ATTR("weight.value", "%.6f", data.weight);
    }
}
