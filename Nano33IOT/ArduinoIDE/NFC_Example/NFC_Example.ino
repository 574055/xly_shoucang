#include "Arduino.h"
#include "wiring_private.h"
#include "nfc_ws.h"


String result;
String NFC_ID;
const byte SetSerial[]={0x03, 0x08, 0xC7, 0x20, 0x07, 0x00, 0x00, 0x14};
const byte ReadCommand[]={0x01, 0x08, 0xA1, 0x20, 0x00, 0x01, 0x00, 0x76};
Uart mySerial(&sercom0, 5, 6, SERCOM_RX_PAD_1, UART_TX_PAD_0);

void setup() {
  // put your setup code here, to run once:
  pinPeripheral(5, PIO_SERCOM_ALT);
  pinPeripheral(6, PIO_SERCOM_ALT);

  mySerial.begin(115200);
  Serial.begin(115200);
  delay(1000);
  mySerial.write(SetSerial,sizeof(SetSerial));
}

void SERCOM0_Handler()
{
  mySerial.IrqHandler();
}

void loop() {
  // put your main code here, to run repeatedly:
  result="";
  NFC_ID="";
  char reading[20]={'\0'};

  mySerial.write(ReadCommand,sizeof(ReadCommand));

  if(mySerial.available())
  {
    for(int i = 0 ; i<20 ; i++)
    {
      //Serial.readString()
      reading[i]=mySerial.read();
      //Serial.print(reading[i],HEX);
      //Serial.print('\t');
      result += String(reading[i],HEX);
      
    }
    result[19]='\0';
  }

  Serial.print(result);

  if(result.substring(0,4)=="1ca1")
  {
    NFC_ID=result.substring(9,17);
    Serial.print('\t'+NFC_ID);
  }

  Serial.println("");
  delay(500);
  
}












