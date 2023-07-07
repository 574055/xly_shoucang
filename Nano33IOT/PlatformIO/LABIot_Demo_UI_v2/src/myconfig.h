#ifndef MY_CONFIG_h
#define MY_CONFIG_h

//#define DEBUG //uncoment when to enable debugg messages

//wifi settings

// #define serverno "148.70.180.108"
// IPAddress server(148,70,180,108);
#define serverno "18.162.252.107"
IPAddress server(18,162,252,107);

const String unitinfo = "testxtray1";

// Pinouts
#define REDLED 12
#define GREENLED 13     //toggles after each loop
#define YELLOWLED 11    //is on when not connected to wifi
#define BUZZER 10

//time to wait for Serial in ms
const uint16_t wait_for_Serial = 15000;

#define FLASH_DEBUG 0 // Use 0-2. Larger for more debugging messages

// Weight scale
// #define FOUR_HX711 //comment to have only one HX711
#ifdef FOUR_HX711
    const String default_scalefactor = "190";
#else
    const String default_scalefactor = "22";
#endif


#endif