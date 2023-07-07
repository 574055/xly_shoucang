1. xtray version
|v1.0    |    Old version developed by Mutian.|
|v2.0    |    Old version built with pcb only have one RFID |reader port.
|v3.0.0    |    Version built with larger pcb, which has two |-RFID reader, weight scale with HX711, SD card, OLED screen, and LED.|
|v3.1.0    |    Add DHT11 based on v3.0.0|
|v3.2.0  |    Add Light sensor based on v3.1.0|
|v3.2.1 | Add serial number, heart beat package|

2. Thing to check during deployment
-Name in OTA init     ArduinoOTA.begin(WiFi.localIP(), "6105-Tray-20", "123456", InternalStorage);
-name in myconfig.h   const String unitinfo = "6105-Tray-20";//6105-Tray-13
-server address         #define serverno "18.162.252.107"//Main server
                        IPAddress server(18,162,252,107);
-Any different pin setting