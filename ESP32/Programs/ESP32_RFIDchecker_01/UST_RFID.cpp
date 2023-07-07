#include "UST_RFID.h"

boolean isValidNumber(String str) {
  for (uint8_t i = 0; i < str.length(); i++)
  {
    if (!isDigit(str.charAt(i))) return false;
  }
  
  return true;
}