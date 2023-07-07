#ifndef MY_CONFIG_h
#define MY_CONFIG_h


//wifi settings

char servername[]="debug.wesharetechnology.com";
#define serverno "148.70.180.108" //developing
//IPAddress server(148,70,180,108);
// #define serverno "18.162.252.107"
// IPAddress server(18,162,252,107);

const String unitinfo = "6105-Tray-23";//6105-Tray-13

// Pinouts
#define chipSelect 4
#define RFID1_EN 3
#define RFID2_EN 7
#define REDLED 8
#define GREENLED 9     //toggles after each loop //9
#define BLUELED 10    //is on when not connected to wifi
#define BUZZER A7

//time to wait for Serial in ms
const uint16_t wait_for_Serial = 15000;

#define FLASH_DEBUG 0 // Use 0-2. Larger for more debugging messages

// Weight scale

#define WEIGHT_SCALE
// #define FOUR_HX711 //comment to have only one HX711
#ifdef FOUR_HX711
    const String default_scalefactor = "230";
#else
    const String default_scalefactor = "22900"; //unused
#endif


#endif