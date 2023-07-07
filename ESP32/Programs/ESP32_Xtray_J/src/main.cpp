#include "buzzer.h"
#include "display.h"
#include "filters.h"
#include "led.h"
#include "main.h"
#include "requests.h"
#include "rfid.h"
#include "sdcard.h"
#include "sdloader.h"
#include "sensing.h"
#include "stringreader.h"

// #include "tuning_profile.h"
#include "weight.h"
#include "weight_tracking.h"
#include "wifi_low_level.h"
#include "wifi_requests.h"

#include <HX711.h>
#include <WiFi.h>
#include <Wire.h>


// RGB LED.
uint8_t LED::counter = 0;

LED LED_R{LED_R_PIN};
LED LED_G{LED_G_PIN};
LED LED_B{LED_B_PIN};

RGBLED rgbLed{&LED_R, &LED_G, &LED_B};

// RFID readers and controllers.
RFIDReader rfidReader1{Serial1};
RFIDReader rfidReader2{Serial2};
RFIDAggregator rfid;

// Sleep threshold.
float sleepLightThreshold = LIGHT_DEFAULT_SLEEP_THRESHOLD;

// High-level tuning interface.
TuneSet<> tuneset;

// Centralised structure for all tray data.
TrayData data;

// Weight variables.
float weightStableEpsilon = WEIGHT_EPSILON;

// High-level interface to weight-tracking algorithm.
WeightTracker weightTracker{rfid};

// Board status and health.
struct BoardStatus
{
    uint8_t sleeping = false;
    bool wifiConnected = false;
};
static BoardStatus status;

void updateLED()
{
    if (status.sleeping) {
        rgbLed.setRGB(0, 0, 0);
    } else {
        if (status.wifiConnected) {
#ifdef LED_BLINK_WHEN_WIFI_CONNECTED
            const uint32_t interval = 500;

            static uint32_t lt_blink = 0;
            static bool blink_up = true;
            if (millis() > lt_blink + interval) {
                blink_up = !blink_up;
                lt_blink = millis();
            }
            if (blink_up) {
                rgbLed.setRGB(0, 255, 0);
            } else {
                rgbLed.setRGB(0, 40, 0);
            }
#else
            rgbLed.setRGB(0, 255, 0);
#endif
        } else {
            rgbLed.setRGB(0, 0, 255);
        }
    }
}


float getSleepLightThreshold() { return sleepLightThreshold; }

void updateSleepLightThreshold(float val)
{
    sleepLightThreshold = val;
    sdSaveSleepThresh(sleepLightThreshold);
}


/// @brief  Callback to be triggered after a tuning variable is set.
void tuningOnUpdate(void* ptr)
{
    // `ptr` points to the variable that was updated. We can match it against actual variables
    // to check if that variable was already updated. Note that these variables should correspond to
    // the variables added to the tuneset.
    if (ptr == &weightGetScale()) {
        Serial.println("scale updated...");
        weightUpdateScale();
    } else if (ptr == &sleepLightThreshold) {
        Serial.println("light threshold updated...");
        sdSaveSleepThresh(sleepLightThreshold);
    } else if (option::isAddress(ptr)) {
        Serial.println("option updated...");
        sdSaveOptions();
    }
}


void updateWeight()
{
    weightUpdateStable(weightStableEpsilon);
    if (option::get(RFID_WEIGHT_TRACKING)) {
        data.weight = weightGetStableRaw();
    } else {
        data.weight = weightGetFiltered();
    }
}

void updateRFID()
{
    // Update RFID internal state.
    rfid.loop();

    data.tags = &rfid.tags();
}

void updateWeightRFIDTracking()
{
    // Update weight tracking using the stabilised weight.
    static uint32_t last_ticks_check = 0;
    if (millis() - last_ticks_check > WT_UPDATE_INTERVAL) {
        last_ticks_check = millis();

        weightTracker.update(data.weight);
    }
}

void updateDisplay()
{
    displaySetWeight(data.weight);
    displaySetNumTags(data.tags ? data.tags->size() : 0);
    for (const auto& it : *data.tags) {
        Serial.println(it.id);
    }
    displayUpdate();
}

void updateWifiSendUpdate()
{
    // Send wifi update payload.
    static uint32_t last_ticks_wifi = 0;
    if (millis() - last_ticks_wifi > (status.sleeping ? WIFI_SLEEP_UPDATE_INTERVAL : WIFI_UPDATE_INTERVAL)) {
        last_ticks_wifi = millis();
        if (data.tags) {
            if (status.sleeping) {
                MinimalTrayData data{Sensors.temp.get(), Sensors.humi.get()};
                wifiTrySend(data);
            } else {
                TrayData cdata{data.tags, data.weight, Sensors.temp.get(), Sensors.humi.get()};
                wifiTrySend(cdata);
            }
        }
    }
}

void wakeUp()
{
    Serial.println("waking...");
    rfidReader1.enable();
    rfidReader2.enable();
    XTrayWiFi.wakeUp();
    displayWakeUp();
    setCpuFrequencyMhz(AWAKE_MCU_CLOCK_SPEED);
    status.sleeping = false;
}

void sleep()
{
    Serial.println("sleeping...");
    rfidReader1.disable();
    rfidReader2.disable();
    XTrayWiFi.sleep();
    displaySleep();
    setCpuFrequencyMhz(SLEEP_MCU_CLOCK_SPEED);
    status.sleeping = true;
}

void updateSleeping()
{
    float light = Sensors.light.get();

    if (option::get(SLEEP_SENSING)) {
        if (!status.sleeping && light <= sleepLightThreshold) {
            sleep();
        } else if (status.sleeping && light > sleepLightThreshold) {
            wakeUp();
        }
    } else {
        // Light sensing disabled. Always awake.
        if (status.sleeping)
            wakeUp();
    }

    if (option::get(DEBUG_LIGHT_SENSOR)) {
        static uint32_t lt;
        if (millis() - lt > LIGHT_DEBUG_INTERVAL) {
            lt = millis();
            Serial.printf("light:%.3f\n", light);
        }
    }
}

void updateOnBoardLED()
{
    static uint32_t last_ticks_led = 0;
    static bool on = false;
    if (millis() - last_ticks_led > (status.sleeping ? 1000 : 100)) {
        last_ticks_led = millis();
        on = !on;
        digitalWrite(BOARD_LED_PIN, on);
    }
}


void setup()
{
    buzzer_init();
    buzzer_off();

    Serial.begin(115200);

    tuneset.add("sleep", status.sleeping);
    tuneset.add("wstabe", weightStableEpsilon);
    tuneset.add("uinfo", const_cast<String&>(getUnitInfo()));

    // Initialise SD first; we'll want to load settings from it.
    sdInit();

    for (int i = 0; i < NUM_OPTIONS; i++) {
        auto opt = static_cast<EnableOption>(i);
        tuneset.add(option::getLabel(opt), const_cast<uint8_t&>(option::get(opt)));
    }

    // Load peripheral and execution settings.
    if (!sdLoadOptions()) {
        option::loadDefaults();
        sdSaveOptions(); // Save the defaults.
    }

    // Important: add callback AFTER loading options.
    tuneset.onUpdate(tuningOnUpdate);

#if ENABLE_ONBOARD_LED
    pinMode(BOARD_LED_PIN, OUTPUT);
#endif

    // Start weight sensors.
    weightInit();

    // Load data from SD memory.
    if (option::get(SDCARD_LOADING)) {
        XTrayWiFi.loadSecrets();
        weightLoadData();
    }

    rgbLed.init();
    displayInit();
    Sensors.init();
    if (option::get(SDCARD_LOADING)) {
        sleepLightThreshold = sdLoadSleepThresh();
    }

    rfidReader1.init(RFID_1_RX_PIN, RFID_1_TX_PIN, RFID_1_EN_PIN);
    rfidReader2.init(RFID_2_RX_PIN, RFID_2_TX_PIN, RFID_2_EN_PIN);
    rfid.add(&rfidReader1);
    rfid.add(&rfidReader2);

    XTrayWiFi.init();

    XTrayWiFi.syncTime();
}


void loop()
{
    // Background tasks.
    status.wifiConnected = XTrayWiFi.isConnected();
    updateLED();

    updateSleeping();

    if (option::get(ONBOARD_LED))
        updateOnBoardLED();

    // Handle requests.
    resetSensorRequests();
    while (Serial.available()) {
        String line = Serial.readStringUntil('\n');
        parseSerialRequest(line);
    }


    // Run sensor and component tasks.
    // TODO: replace with RTOS.

    // Some components are run according to `status.sleeping`. But if the tray
    // is sleeping and a request is made, the component is updated regardless.
#define shouldWeRun(OPTION, REQUEST) ((!status.sleeping && (OPTION)) || isRequested(REQUEST))

    if (shouldWeRun(option::get(WEIGHT_SENSING), RequestWeight))
        updateWeight();

    if (shouldWeRun(option::get(RFID_SENSING), RequestRFID))
        updateRFID();

    if (!status.sleeping) {
        if (option::get(RFID_WEIGHT_TRACKING) && option::get(RFID_SENSING) && option::get(WEIGHT_SENSING))
            updateWeightRFIDTracking();

        if (option::get(OLED_DISPLAY))
            updateDisplay();
    }

    if (option::get(WIFI_UPDATES))
        updateWifiSendUpdate();

    handleSensorRequests(data);
}
