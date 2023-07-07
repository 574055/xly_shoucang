#include "backup.h"

#include "sdcard.h"
#include "wifi_low_level.h"

bool backup(const String& data)
{
    tm timeinfo;
    XTrayWiFi.getTime(&timeinfo);

    static char path[64];
    snprintf(path, sizeof(path), SD_BACKUP_FMT(timeinfo));

    return sdAppend(path, data);
}