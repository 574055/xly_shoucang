#ifndef MY_CONFIG_h
#define MY_CONFIG_h


//wifi settings

// #define serverno "148.70.180.108" //developing
// IPAddress server(148,70,180,108);
#define serverno "18.162.252.107"
IPAddress server(18,162,252,107);

const String unitinfo = "testxtray8";

// Pinouts
#define REDLED 7
#define GREENLED 8     //toggles after each loop
#define YELLOWLED 9    //is on when not connected to wifi
#define BUZZER 10

//time to wait for Serial in ms
const uint16_t wait_for_Serial = 15000;

#define FLASH_DEBUG 0 // Use 0-2. Larger for more debugging messages

// Weight scale
// #define FOUR_HX711 //comment to have only one HX711
#ifdef FOUR_HX711
    const String default_scalefactor = "230";
#else
    const String default_scalefactor = "22";
#endif


#endif