#include "sdloader.h"

#include "defines.h"
#include "sdcard.h"
#include "stringreader.h"

// TODO: cleanup load/save, save-on-fail-to-load logic. Currently
// save-on-fail-to-load is scattered all over. Consider using sum types
// (optional<float>) and letting user handle the save on fail.


bool sdLoadOptions()
{
    String contents;
    if (!sdRead(&contents, SD_SETTINGS_OPTIONS) || contents.isEmpty()) {
        Serial.println("Couldn't options... using default...");
        return false;
    }
    Serial.printf("Read options:\n%s\n", contents.c_str());
    
    StringReader reader{contents};
    do {
        auto line = reader.readToken('=');
        bool en = reader.readLine() == "1";
        for (int i = 0; i < NUM_OPTIONS; i++)
            if (line == option::getLabel(static_cast<EnableOption>(i))) {
                // Found the option!
                option::set(static_cast<EnableOption>(i), en);
            }
    } while (reader);
    return true;
}

void sdSaveOptions()
{
    String text;
    for (int i = 0; i < NUM_OPTIONS; i++) {
        EnableOption opt = static_cast<EnableOption>(i);
        bool en = option::get(opt);
        const char* label = option::getLabel(opt);
        text += label;
        text += "=";
        text += en ? "1" : "0";
        text += "\n";
    }

    if (!sdWrite(SD_SETTINGS_OPTIONS, text)) {
        Serial.println("Couldn't save options...");
        return;
    }

    Serial.println("Saved options...");
    Serial.println(text);
}


float sdLoadFloat(const String& path, float default_value)
{
    String contents;
    if (!sdRead(&contents, path) || contents.isEmpty()) {
        Serial.printf("Couldn't read float from %s. Saving default for now...\n", path.c_str());
        sdSaveFloat(path, default_value);
        return default_value;
    }
    Serial.printf("Read float: %s\n", contents.c_str());
    return contents.toFloat();
}

void sdSaveFloat(const String& path, float value)
{
    if (!sdWrite(path, String(value, 6))) {
        Serial.printf("Couldn't save float to %s.\n", path.c_str());
        return;
    }
    Serial.printf("Saved float to %s.\n", path.c_str());
}


float sdLoadScale()
{
    return sdLoadFloat(SD_SETTINGS_SCALE, HX711_DEFAULT_CALIBRATION_FACTOR);
}

void sdSaveCalibration(float value)
{
    sdSaveFloat(SD_SETTINGS_SCALE, value);
}

float sdLoadOffset()
{
    return sdLoadFloat(SD_SETTINGS_OFFSET, 0.0);
}

void sdSaveOffset(float value)
{
    sdSaveFloat(SD_SETTINGS_OFFSET, value);
}

bool sdLoadWifi(WiFiLowLevel& wifi)
{
    String contents;
    if (!sdRead(&contents, SD_SETTINGS_WIFI) || contents.isEmpty()) {
        Serial.println("Couldn't read wifi settings...");
        return false;
    }

    StringReader reader{contents};
    wifi.ssid = toString(reader.readToken('\n'));
    wifi.pwd = toString(reader.readToken('\n'));
    wifi.serverName = toString(reader.readToken('\n'));
    wifi.serverIP = toString(reader.readToken('\n'));
    Serial.printf("Read wifi settings: %s\n", contents.c_str());
    return true;
}

void sdSaveWifi(const WiFiLowLevel& wifi)
{
    String contents = wifi.ssid + "\n" + wifi.pwd + "\n" + wifi.serverName + "\n" + wifi.serverIP + "\n";
    if (!sdWrite(SD_SETTINGS_WIFI, std::move(contents))) {
        Serial.println("Couldn't save wifi settings...");
        return;
    }
    Serial.println("Saved wifi settings...");
}


float sdLoadSleepThresh()
{
    return sdLoadFloat(SD_SETTINGS_SLEEPTHRESH, LIGHT_DEFAULT_SLEEP_THRESHOLD);
}

void sdSaveSleepThresh(float value)
{
    sdSaveFloat(SD_SETTINGS_SLEEPTHRESH, value);
}
