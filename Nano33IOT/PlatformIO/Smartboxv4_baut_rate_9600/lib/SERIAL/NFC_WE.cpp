#include "NFC_WE.h"

Uart mySerial(&sercom0, 5, 6, SERCOM_RX_PAD_1, UART_TX_PAD_0);//NFC pin RX, TX

void SERCOM0_Handler()
{
  mySerial.IrqHandler();
}



