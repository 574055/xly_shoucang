#include "data.h"

#include "main.h"
#include "sdloader.h"

#include <ArduinoJson.h>


// TODO: unit info how?
String UNIT_INFO = "3125-Test-42"; // 6105-Tray-13

unsigned logLevel = 0;


const String& getUnitInfo()
{
    return UNIT_INFO;
}

unsigned getLogLevel()
{
    return logLevel;
}

void setLogLevel(unsigned level)
{
    logLevel = level;
}

String getActiveComponents()
{
    String s;
    if (option::get(WEIGHT_SENSING))
        s += "W";
    if (option::get(RFID_SENSING))
        s += "R";
    if (option::get(TEMP_HUMI_SENSING))
        s += "D";
    if (option::get(SLEEP_SENSING))
        s += "L";
    return s;
}

MetaData MetaData::get()
{
    String version = (String)VERSION_STRING + "-" + getActiveComponents();
    String serial = "deadbeefdeadbeefdeadbeefdeadbeefdeadbeef";
    return MetaData{UNIT_INFO, version, serial};
}

String buildJSON(const MetaData& metadata, const TrayData& data)
{
    StaticJsonDocument<1024> doc;
    doc["unitinfo"] = metadata.unitinfo;
    doc["version"] = metadata.version;
    doc["sn"] = metadata.serial;
    doc["sleeping"] = false;

    char buffer[16];
    snprintf(buffer, sizeof(buffer), "%011.3f", data.weight);
    doc["data"]["weight"] = String(buffer);

    size_t i = 0;
    if (!data.tags || data.tags->empty()) {
        // If no tags, create an empty array.
        doc["data"].createNestedArray("uids");
    } else {
        for (const RFIDIdentifier& rfid : *data.tags) {
            snprintf(buffer, sizeof(buffer), "%08x", rfid.id);
            doc["data"]["uids"][i] = String(buffer);
            i++;
        }
    }

    snprintf(buffer, sizeof(buffer), "%.2f", data.temp);
    doc["data"]["temp"] = String(buffer);

    snprintf(buffer, sizeof(buffer), "%.2f", float(int(data.humi)));
    doc["data"]["humi"] = String(buffer);

    String out;
    serializeJson(doc, out);
    return out;
}

String buildJSON(const MetaData& metadata, const MinimalTrayData& data)
{
    StaticJsonDocument<1024> doc;
    doc["unitinfo"] = metadata.unitinfo;
    doc["version"] = metadata.version;
    doc["sn"] = metadata.serial;
    doc["sleeping"] = true;

    char buffer[16];

    snprintf(buffer, sizeof(buffer), "%.2f", data.temp);
    doc["data"]["temp"] = String(buffer);

    snprintf(buffer, sizeof(buffer), "%.2f", float(int(data.humi)));
    doc["data"]["humi"] = String(buffer);

    String out;
    serializeJson(doc, out);
    return out;
}
