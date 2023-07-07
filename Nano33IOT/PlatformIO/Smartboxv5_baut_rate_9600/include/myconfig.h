#ifndef MY_CONFIG_h
#define MY_CONFIG_h


// wifi settings
char servername[] = "debug.wesharetechnology.com";
#define serverno "16.162.101.124" // developing
// IPAddress server(148,70,180,108);

//#define serverno "18.162.199.13" //Smartbox
// IPAddress server(18,162,199,13);

// #define serverno "18.162.252.107"
// IPAddress server(18,162,252,107);

const String unitinfo = "SmartBox-v5"; // 6105-Tray-13

// Pinouts
#define chipSelect 5
#define BUZZER     3

// time to wait for Serial in ms
const uint16_t wait_for_Serial = 5000;

#define FLASH_DEBUG 0 // Use 0-2. Larger for more debugging messages

// Weight scale

#define WEIGHT_SCALE
// #define FOUR_HX711 //comment to have only one HX711
#ifdef FOUR_HX711
const String default_scalefactor = "230";
#else
const String default_scalefactor = "22"; // unused
#endif


#endif