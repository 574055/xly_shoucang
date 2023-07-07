#include "arduino_secrets.h" //self on src folder
#include "wiring_private.h"

#include <Adafruit_Sensor.h>
#include <Arduino.h>
#include <ArduinoUniqueID.h>
#include <FlashStorage_SAMD.h>
#include <SPI.h>
#include <SimpleKalmanFilter.h>
#include <WiFiNINA.h>
#include <Wire.h>

// SELF LIBRARY HEADER FILE
#include "DISPLAY.h"
#include "SD_CARD.h"
// #include "UST_RFID.h"
#include "DHT_WE.h"
#include "NFC_WE.h"
#include "rfid.h"
#include "rfid_low_level.h"

// #define DEBUG //uncoment when to enable debugg messages
#ifdef DEBUG
#define debug(x)   Serial.print(x)
#define debugln(x) Serial.println(x)
#else
#define debug(x)
#define debugln(x)
#endif

// Define the function [instead of setup and loop]
void displaySavedData();
void saveWifi(String SSID, String PWD);
void eepromInit();
void wifiInit();
String buildData(String id, String timestamp, String unitinfo, String temp, String Humi, String USERID, String chemID);
void httpRequest(String data);
void printWifiStatus();
void NFCreadinginLoop();
void printforCounting();
void nfcAvailable(String reading);

String data;
char d1; // terminal input
String x;
float scalefactor;

// RFID VARIABLE
String USERID;
String LastUSER;

// RFID STRING SENDING TO JSON
String rfidtags;

// for rfid and nfc both tapped [disable those detecting function within 30 mins]
int allright = 0;         // act like bool to show the result about rfid and nfc are detected
int allrightcounting = 0; // counting time for 30 mins
int screenCounting = 0;   // counting time for 30 mins on screen

// Timer
unsigned long looptime, noWifiTime = 0;
bool firstNotConnected = false;
int checkinTime = 0; // for ensuring there won't be the bug that checkin and checkout occur at the same second

// DHT TEMP AND HUMIDITY
int h1 = 0;
float t1 = 0;

// Wifi
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
int keyIndex = 0; // your network key index number (needed only for WEP)
int status = WL_IDLE_STATUS;

// Initialize the WiFi client library
WiFiClient client;
#define url "/lab/hwinfo"

// data received from server
String receivedMsg = "";

// eeprom
typedef struct
{
    char SSID[30];
    char PWD[30];
    char SCALEFACTOR[10];
} EEPROM_data;

const int WRITTEN_SIGNATURE = 0xBEEFDEED;

int signature;
uint16_t storedAddress = 0;
EEPROM_data e_data;

int UID[100]; // UniqueID

// NFC
String nfcresult;
String NFC_ID;
// const byte SetSerial[]={0x03, 0x08, 0xC7, 0x20, 0x07, 0x00, 0x00, 0x14}; //set baud rate to 115200
const byte SetSerial[] = {0x03, 0x08, 0xC7, 0x20, 0x07, 0x00, 0x00, 0x14}; // set baud rate to 9600
const byte ReadingCommand[] = {0x01, 0x08, 0xA1, 0x20, 0x00, 0x00, 0x00, 0x77};

int NFCshow = 0;

// Check if the user is new user or not within 30 mins variable
int newuserCome = 0; // Check if the user is new user or not within 30 mins [act as bool]
String currentuser = "";

// storing NFCID
String lastNFCID = "";
String currentNFCID = "";

// storing the chemical rfid tap number
String chemID = "";
String scanresult;
// Server detection bool
int consistent = 0;

// checkin checking
int checkinED = 0;

// store all the chemical rfid tag before check out
String chemicalrfidNoSendtoServer = "";

// display rfid tag
String tagDisplayF = "";

// user band rfid
String realUserBand = "";
String lastrealUserBand = "";
RFIDIdentifier userTag, lastUserTag;

// one minute for rfid scan result storage
int oneminScan = 0;
int numberOfTagsOneMin = 0;
int cansend = 0;

// set prev-finalTags
static String prev_finalTags = "";

RFIDReader rfidReader(Serial1);
RFIDAggregator rfid;

void displayTag(const RFIDIdentifier& tag)
{
    char buffer[9];
    snprintf(buffer, sizeof(buffer), "%08x", tag.id);
    display.print(buffer);
}


void setup()
{
    randomSeed(analogRead(0));

    pinMode(BUZZER, OUTPUT); // Buzzer
    digitalWrite(BUZZER, LOW);

    pinPeripheral(5, PIO_SERCOM_ALT);
    pinPeripheral(6, PIO_SERCOM_ALT);

    mySerial.begin(9600); // for NFC

    // Serial1.begin(115200); // for RFID
    Serial.begin(115200);
    delay(1000);

    oledInit();

    rfidReader.init();
    rfid.add(&rfidReader);

    waitSerial(wait_for_Serial);
    debugln("Serial port ready");

    eepromInit();

    // rfidInit();
    debugln("RFID ready");
    wifiInit();
    debugln("WeShare RFID reader is ready!");
    debugln("weight scale ready");

    initializeDHT(); // DHT init
}

void loop()
{
    realUserBand = "";
    USERID = "";
    scanresult = "";
    userTag.clear();
    // tagDisplayF="";

    t1 = temperatureReading();
    h1 = humdityReading();

    // DHT: TEMPERATURE AND HUMDITY READING AND CALIBRATION
    dhtReading(h1, t1);

    // display on screen
    cleanScreenandTextsizeTWO();

    // NFC data reading
    NFCreadinginLoop();

    // // RFID
    // // Scan RFID1
    //   uint8_t numberOfTags1 = 0;
    //   String finalTags = multiscan_v2(&numberOfTags1, TIMEOUT);

    //   // data processing
    //   uint8_t numberOfTags = finalTags.length() / 8;
    //   static uint8_t prev_numberOfTags = 0;
    //   Serial.print('\t'+ (String)"RFID reading:");
    //   Serial.print((String) "n" + numberOfTags); // send numberOfTags to GUI
    //   Serial.print('\t');
    //   Serial.print((String) "f" + finalTags);    // send finalTags to GUI
    //   Serial.print('\t');

    //   // check for added/removed tags
    //   String addedTags = tagsChanged(prev_finalTags, prev_numberOfTags, finalTags, numberOfTags);
    //   String removedTags = tagsChanged(finalTags, numberOfTags, prev_finalTags, prev_numberOfTags);
    //   Serial.println((String)"d" + addedTags); //send numberOfTags to Serial
    //   Serial.println((String)"r" + removedTags); //send finalTags to Serial

    //   if(finalTags != ""){
    //     prev_finalTags = finalTags;
    //     prev_numberOfTags = numberOfTags;
    //   }

    // end of RFID reader


    rfid.loop();
    auto& tags = rfid.tags();

    // Find user id.
    for (const auto& tag : tags) {
        if (((tag.id >> 28) & 0xF) == 0xA) {
            userTag = tag;
            break;
        }
    }

    // check in, out process
    if (checkinED == 1) {
        if (((NFC_ID != "" && NFC_ID == lastNFCID) || (realUserBand != "" && realUserBand == lastrealUserBand)
             || (userTag.has_value() && userTag == lastUserTag))
            && (millis() - checkinTime >= 10000))
        {
            display.setTextSize(2);
            display.println("CHECK-OUT");
            tagDisplayF = "";
            cansend = 1;
            checkinED = 0;
        } else {
            if (millis() - oneminScan >= 60000) {
                oneminScan = millis();
                // tagDisplayF = "";
            } else {
                // Add comma and <"> between Tags
                // if(finalTags != ""){
                //   uint8_t i = 0;
                //     while (i < numberOfTags){
                //       if (finalTags.substring(i*8,i*8+1) != "a")
                //       {
                //         if(tagDisplayF.length() > 0){
                //           String finaltagaddN = finalTags.substring(8 * i, 8 + 8 * i) + "\n";
                //           int canadd = 0;
                //           for(uint8_t j = 0; j < ((tagDisplayF.length())/9); j++){
                //             if(finaltagaddN.substring(9 * i, 9 + 9 * i) == tagDisplayF.substring(9 * j, 9 + 9 * j)){
                //               canadd = 0;
                //               break;
                //             }else{
                //               canadd = 1;
                //             }

                //           }

                //           if(canadd == 1){

                //              tagDisplayF += finalTags.substring(8 * i, 8 + 8 * i) + "\n";
                //               scanresult += "\"" + finalTags.substring(8 * i, 8 + 8 * i) + "\",";
                //               Serial.print("scanresult :" + scanresult);
                //               Serial.print("tagDisplayF :" + tagDisplayF);

                //               numberOfTagsOneMin++;
                //           }


                //         }else{
                //             tagDisplayF += finalTags.substring(8 * i, 8 + 8 * i) + "\n";
                //             scanresult += "\"" + finalTags.substring(8 * i, 8 + 8 * i) + "\",";
                //             numberOfTagsOneMin++;
                //         }
                //       }
                //     i++;
                //   }
                // }
            }

            int length = scanresult.length();
            scanresult.remove(length - 1); // remove the last comma

            // end of rfid

            Serial.println("RFID" + USERID);
            if (tagDisplayF != "" || rfid.count() > 0) {
                display.setTextSize(1);
                // display.println(tagDisplayF);
                for (const auto& tag : tags) {
                    displayTag(tag);
                    display.println("");
                }
                display.setTextSize(2);
                display.setCursor(52, 0);
                display.println("Total#");
                display.setCursor(80, 18);
                // display.println(numberOfTagsOneMin); //////////////////
                display.println(rfid.count());
            } else {
                display.setTextSize(2);
                display.println("no RFID");
            }
        }
    } else {
        printTheTempandHumd(t1, h1);
        display.println("NULL");

        if (NFC_ID != "" || realUserBand != "" || userTag.has_value()) {
            if (checkinED == 0) {
                display.println("CHECK-IN");
                checkinED = 1;
                if (NFC_ID != "") {
                    lastNFCID = NFC_ID;
                } else if (realUserBand != "") {
                    lastrealUserBand = realUserBand;
                } else {
                    lastUserTag = userTag;
                }
                checkinTime = millis();
                oneminScan = millis();
            }
        }
    }

    display.display();

    if (USERID != "") {
        LastUSER = USERID;
    }

    // get response from wifi

    if (client.available()) {
        String payload = client.readString();
        // get JSON body
        int start = payload.indexOf('{');
        int end = payload.lastIndexOf('}');
        String body = payload.substring(start, end + 1);
        Serial.println(body);
    }

    // Send Data through Wifi
    String timestamp = "1";
    String data = buildData(NFC_ID, timestamp, unitinfo, (String)t1, (String)h1, USERID, (String)numberOfTagsOneMin);

    // check connection and connect to wifi if duration has passed
    if (status == WL_CONNECTED) { // only send data if connected to wifi
        if (cansend == 1) {
            httpRequest(data);
            numberOfTagsOneMin = 0;
            cansend = 0;
        }
    }

    if (status != WL_CONNECTED) {
        wifiInit();
    }
}

// private functions ******************************************************************

// WIFI
void printWifiStatus()
{
    // print the SSID of the network you're attached to:
    Serial.print("SSID: ");
    Serial.println(WiFi.SSID());

    // print your board's IP address:
    IPAddress ip = WiFi.localIP();
    Serial.print("IP Address: ");
    Serial.println(ip);

    // print the received signal strength:
    long rssi = WiFi.RSSI();
    Serial.print("signal strength (RSSI):");
    Serial.print(rssi);
    Serial.println(" dBm");
}

void wifiInit()
{
    // check for the WiFi module:
    if (WiFi.status() == WL_NO_MODULE) {
        Serial.println("Communication with WiFi module failed!");
        // don't continue
        while (true)
            ;
    }

    long prev_time = millis();
    long tim = 0;
    bool con = true;

    // attempt to connect to WiFi network:
    while (status != WL_CONNECTED) {
        Serial.print("Attempting to connect to SSID: ");
        Serial.println(e_data.SSID);
        display.clearDisplay();
        display.setTextSize(1);
        display.setCursor(0, 0);
        display.println((String) "trying to connect to: " + e_data.SSID);
        display.display();
        debugln((String) "WifiInit SSID: " + e_data.SSID);
        debugln((String) "WifiInit PWD: " + e_data.PWD);

        status = WiFi.begin(e_data.SSID, e_data.PWD);

        tim = millis() - prev_time;
        if (tim > 20000) { // 0.5 min
            con = false;
            break;
        }
    }
    // you're connected now, so print out the status:
    printWifiStatus();
    if (WiFi.RSSI()) { // could connect
        cleanScreenandTextsizeTWO();
        display.println("Connected!");
        display.display();

        // send to GUI
        Serial.println((String) "A"); // succesfully connect to Wifi, send to GUI
    } else {                          // connection failed
        cleanScreenandTextsizeTWO();
        display.println("Could not connect to Wifi");
        display.display();
        debugln("could not connect to wifi");
        // send to GUI
        Serial.println((String) "a"); // could not connect to Wifi, send to GUI
    }
    firstNotConnected = false;
}

String buildData(String id, String timestamp, String unitinfo, String temp, String Humi, String USERID,
                 String numberOfTags)
{ //组建发送的json字符串
    String payload = "{\"unitinfo\":\"";
    payload += unitinfo;
    payload += "\",\"data\":{\"weight\":\"";
    payload += 0;
    payload += "\",\"version\":\"";
    payload += "";
    payload += "\",\"SN\":\"";
    payload += "";
    payload += "\",\"uids\":[";
    payload += scanresult;
    payload += "],\"NumberofTap\":[";
    payload += numberOfTags;
    payload += "],\"temp\":\"";
    payload += temp;
    payload += "\",\"humi\":\"";
    payload += Humi;
    payload += "\",\"rfidband\":\"";
    payload += realUserBand;
    payload += "\",\"nfc\":\"";
    payload += id;
    payload += String("\"}}");
    Serial.println((String) "payload: " + payload);
    return payload;
}

// this method makes a HTTP connection to the server:
void httpRequest(String data)
{
    Serial.print("httpRequest");
    //  close any connection before send a new request.
    //  This will free the socket on the NINA module
    client.stop();

    // if there's a successful connection:
    if (client.connect(servername, 8000)) {
        Serial.println("connecting...");

        // send the HTTP POSTS request:
        client.println((String) "POST " + url + " HTTP/1.0");
        client.println((String) "Host: " + serverno);
        client.println((String) "Accept: */*");
        client.println((String) "Content-Length: " + data.length());
        client.println("Content-Type: application/json");
        client.println();
        client.println(data);
        Serial.flush();

    } else {
        // if you couldn't make a connection:
        Serial.println("connection failed");
        status = WL_IDLE_STATUS;
        WiFi.end();
        // digitalWrite(BLUELED, HIGH);
    }
}


// EEPROM
void eepromInit()
{ // safes default values to eeprom if empty
    EEPROM.get(storedAddress, signature);

    // If the EEPROM is empty then no WRITTEN_SIGNATURE
    if (signature == WRITTEN_SIGNATURE) // if something is in Flash
    {
        EEPROM.get(storedAddress + sizeof(signature), e_data);
        displaySavedData();
    } else // safe default settings to eeprom if empty
    {
        EEPROM.put(storedAddress, WRITTEN_SIGNATURE);

        String SSID = SECRET_SSID; //"LSK 7028";
        String PWD = SECRET_PASS;  //"wesharetechnology";
        String SCALEFACTOR = default_scalefactor;

        // Fill the "e_data" structure with the data entered by the user...
        SSID.toCharArray(e_data.SSID, 30);
        PWD.toCharArray(e_data.PWD, 30);
        SCALEFACTOR.toCharArray(e_data.SCALEFACTOR, 10);

        // ...and finally save everything into emulated-EEPROM
        EEPROM.put(storedAddress + sizeof(signature), e_data);


        if (!EEPROM.getCommitASAP()) {
            Serial.println("CommitASAP not set. Need commit()");
            EEPROM.commit();
        }
    }
}

void saveWifi(String SSID, String PWD)
{
    EEPROM.put(storedAddress, WRITTEN_SIGNATURE);

    // Fill the "e_data" structure with the data entered by the user...
    SSID.toCharArray(e_data.SSID, 30);
    PWD.toCharArray(e_data.PWD, 30);

    // ...and finally save everything into emulated-EEPROM
    EEPROM.put(storedAddress + sizeof(signature), e_data);

    if (!EEPROM.getCommitASAP()) {
        Serial.println("CommitASAP not set. Need commit()");
        EEPROM.commit();
    }
}

void displaySavedData() // display the ssid and scale factor
{
    Serial.print("W");
    Serial.println(e_data.SSID);
    Serial.print("F");
    Serial.println(e_data.SCALEFACTOR);
}

///////////////////////////////////////sensors////////////////////////////////////

// NFC
void NFCreadinginLoop()
{
    nfcresult = "";
    NFC_ID = "";
    char reading[25] = {'\0'};

    static uint32_t last_ticks = 0;
    if (millis() - last_ticks > 500) {
        last_ticks = millis();

        mySerial.write(ReadingCommand, sizeof(ReadingCommand));

        Serial.println("");
    }

    if (mySerial.available()) {
        for (int i = 0; i < 25; i++) {
            reading[i] = mySerial.read();
            nfcresult += String(reading[i], HEX);
        }
        nfcresult[19] = '\0';
    }
    Serial.print((String) "NFC reading:");

    if (nfcresult.substring(0, 4) == "1ca1") {
        NFC_ID = nfcresult.substring(9, 17);
        Serial.print(NFC_ID);
    }
}

// check if the rfid is chem rfid or other thing
void UserIDFind(String str)
{
    for (uint8_t i = 0; i < (str.length() / 8); i++) {
        if (str.substring(i * 8, i * 8 + 1) != "a") {
            USERID = str.substring(i * 8, i * 8 + 8);
        } else {
            realUserBand = str.substring(i * 8, i * 8 + 8);
        }
    }
}
