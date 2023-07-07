#ifndef SD_CARD_H
#define SD_CARD_H

#include <Arduino.h>


void sdInit();
bool sdRead(String* contents, String path, bool create = true);
bool sdWrite(String path, String data, bool create = true, bool append = false);
inline bool sdAppend(String path, String data, bool create = true)
{
    return sdWrite(std::move(path), std::move(data), create, true);
}


#endif
