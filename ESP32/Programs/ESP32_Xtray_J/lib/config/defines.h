// Lightweight definitions and constants file.

#ifndef DEFINES_H
#define DEFINES_H

// ----- Version -----

#define VERSION_MAJOR 0
#define VERSION_MINOR 10
#define VERSION_PATCH 0


// ----- Pindefs -----

#define BUZZER_PIN 15

#define LIGHT_SENSOR_PIN VP_PIN
#define DHT_SENSOR_PIN   25

#define OLED_SDA_PIN 27
#define OLED_SCK_PIN 26

#define LED_R_PIN 14
#define LED_G_PIN 12
#define LED_B_PIN 13

#define WEIGHT_SDA_PIN 23
#define WEIGHT_SCK_PIN 22

// RFID wires: GREEN: TX,  WHITE: RX,  BLUE: EN
#define RFID_1_EN_PIN 18
#define RFID_1_TX_PIN 19
#define RFID_1_RX_PIN 21

#define RFID_2_EN_PIN 33
#define RFID_2_TX_PIN 32
#define RFID_2_RX_PIN 35

#define SD_CS_PIN   4
#define SD_CLK_PIN  UART_RX2_PIN
#define SD_MOSI_PIN UART_TX2_PIN
#define SD_MISO_PIN 5


// ----- Component Settings -----

#define LEDC_TIMER_BIT_PRECISION      13   // LEDC timer precision.
#define LEDC_BASE_FREQ                5000 // LEDC base frequency.
#define LED_BLINK_WHEN_WIFI_CONNECTED      // Set the LED to blink when WiFi is connected.

#define DHT_TYPE DHT11

#define LIGHT_MIN_VALUE               ((uint32_t)100)
#define LIGHT_MAX_VALUE               ((uint32_t)3000)
#define LIGHT_DEFAULT_SLEEP_THRESHOLD 0.1  // Below threshold => SLEEP, Above threshold => AWAKE.
#define LIGHT_DEBUG_INTERVAL          1000 // How often light value is printed.


#define HX711_DEFAULT_CALIBRATION_FACTOR (20890) // ADC-to-real world calibration.
#define HX711_80HZ                       0       // 1 for 80Hz filtering. 0 for 10Hz. Requires hardware change also.

#define RFID_MAX_TAGS           128  // Maximum number of tags we can track.
#define RFID_SCAN_TIMEOUT       10   // Duration to wait before we stop the current scan iteration (in ms).
#define RFID_TAG_TIMEOUT        3000 // Duration to wait before a tag is discarded from the tracked tags (in ms).
#define RFID_MULTISCAN_AMOUNT   20   // Number of scans to perform for multiscan.
#define RFID_MAX_READERS        2    // Maximum number of RFIDReaders.
#define RFID_PACKET_BUFFER_SIZE 64   // Number of bytes for buffering a packet before parsing.

// Weight epsilon: how much leeway do we give to detect a change?
// If difference is less than epsilon => stable. Otherwise => unstable.
// This value only applies for stabilising the weight.
#define WEIGHT_EPSILON 0.08f

// Default number of times to tare the scale.
#define WEIGHT_TARE_N 10

// Weight tracking constants.
#define WT_ONTRAY_EPSILON        9    // ∆RSSI needed to be queued for traying/untraying.
#define WT_ONTRAY_RSSI_THRESHOLD 198  // Items on the tray must be above this absolute RSSI threshold.
#define WT_UPDATE_INTERVAL       1000 // Update interval, in ms.

// OLED resolution.
#define OLED_SCREEN_WIDTH  128 // OLED width,  in pixels.
#define OLED_SCREEN_HEIGHT 32  // OLED height, in pixels.

#define OLED_PAGE_CYCLE_INTERVAL 3000 // Interval to display each page on the OLED, in ms.

// WiFi secrets.
#define WIFI_DEFAULT_SSID "WeShare-G031B"
#define WIFI_DEFAULT_PWD  "1234567890"
// #define WIFI_DEFAULT_SSID       "Room 3606"
// #define WIFI_DEFAULT_PWD        "wesharetech"
#define WIFI_DEFAULT_SERVERNAME    "debug.wesharetechnology.com"
#define WIFI_DEFAULT_SERVERIP      "16.162.101.124"
#define WIFI_UPDATE_ENDPOINT       "/lab/hwinfo"
#define WIFI_UPDATE_INTERVAL       5000  //  Interval between data sends.
#define WIFI_SLEEP_UPDATE_INTERVAL 60000 //  Interval between data sends during sleeping.

// SD card paths.
#define SD_BACKUP_DIR     "/backup/"
#define SD_RAW_FMT(tm)    "-%04d-%02d-%02d.txt", tm.tm_year, tm.tm_mon, tm.tm_mday
#define SD_BACKUP_FMT(tm) SD_BACKUP_DIR "backup" SD_RAW_FMT((tm))

#define SD_SETTINGS_DIR         "/settings/"
#define SD_SETTINGS_SCALE       "/settings/scale.txt"
#define SD_SETTINGS_OFFSET      "/settings/offset.txt"
#define SD_SETTINGS_WIFI        "/settings/wifi.txt"
#define SD_SETTINGS_SLEEPTHRESH "/settings/slpthresh.txt"
#define SD_SETTINGS_OPTIONS     "/settings/options.txt"


// ----- Enable/Disable Settings -----

// This is an x-macro containing all our configurable options.
// Format: X(DEFAULT_VALUE, ENUM, LABEL).
// The default value is used if a value can't be loaded from SD.
// Label is used for Serial Tuning/printing.
#define OPTIONS_LIST(X)                                                                                                \
    X(1, RFID_SENSING, "en.rfid") /* Will be auto-disabled if either RFID/Weight sensing are disabled. */              \
    X(1, WEIGHT_SENSING, "en.weight")                                                                                  \
    X(0, RFID_WEIGHT_TRACKING, "en.wtrack")                                                                            \
    X(1, TEMP_HUMI_SENSING, "en.dht")                                                                                  \
    X(1, SLEEP_SENSING, "en.sleep")                                                                                    \
    X(1, WIFI_UPDATES, "en.wifi")       /* Whether to continuously send tag/weight updates over wifi. */               \
    X(0, WIFI_VERIFY_200, "en.wifiver") /* Whether to wait and check for a 200 (success) reply to a POST request. */   \
    X(1, OLED_DISPLAY, "en.oled")       /* Whether OLED should be updated. */                                          \
    X(0, BUZZER, "en.buzz")                                                                                            \
    X(1, ONBOARD_LED, "en.boardled")                                                                                   \
    X(0, SDCARD_DATA_BACKUP, "en.sdbackup") /* Whether data should be saved to SD if it fails to be sent. */           \
    X(1, SDCARD_LOADING, "en.sdload")       /* Whether to load data from SD on init. Options are loaded regardless. */ \
    X(0, DEBUG_RFID, "en.d.rfid")                                                                                      \
    X(0, DEBUG_WEIGHT_DATA, "en.d.weight")                                                                             \
    X(0, DEBUG_WEIGHT_TRACKER, "en.d.wtrack")                                                                          \
    X(0, DEBUG_LIGHT_SENSOR, "en.d.light")


// MCU clock speed (in MHz). Options: 80, 160, 240.
#define SLEEP_MCU_CLOCK_SPEED 80
#define AWAKE_MCU_CLOCK_SPEED 240


// ----- Dependent Settings -----
// These generally don't need to be modified, and depend on certain board characteristics or other macros.

#if HX711_80HZ
// More data? Use a larger buffer to filter the noise.
#define DATA_BUFFER_SIZE 64
#else
#define DATA_BUFFER_SIZE 16
#endif

// ESP32 Aliases:
#define UART_RX2_PIN 16
#define UART_TX2_PIN 17
#define VN_PIN       39
#define VP_PIN       36

#define BOARD_LED_PIN 2


// Version.
#define STRINGIFY2(X)  #X // Second stringify macro necessary for expansion.
#define STRINGIFY(X)   STRINGIFY2(X)
#define VERSION_STRING "V" STRINGIFY(VERSION_MAJOR) "." STRINGIFY(VERSION_MINOR) "." STRINGIFY(VERSION_PATCH)



// ----- Option Declarations -----

// Options.
enum EnableOption
{
#define X_ENUM(D, E, ...) E,
    OPTIONS_LIST(X_ENUM) NUM_OPTIONS,
#undef X_ENUM
};

struct option
{
    static void loadDefaults();
    static const unsigned char& get(EnableOption opt);
    static void set(EnableOption opt, bool value);
    static void toggle(EnableOption opt);

    /// @brief Get a unique string representation of the option.
    static const char* getLabel(EnableOption opt);

    /// @brief True if the pointer is targetting an option variable.
    static bool isAddress(void* ptr);
};

#endif