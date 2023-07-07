#include "wifi_requests.h"

#include "backup.h"
#include "data.h"
#include "defines.h"
#include "wifi_low_level.h"


bool wifiUpdateTrayData(const TrayData& data)
{
    return XTrayWiFi.sendHttpPostRequest(WIFI_UPDATE_ENDPOINT, buildJSON(MetaData::get(), data));
}

bool wifiUpdateMinimalTrayData(const MinimalTrayData& data)
{
    return XTrayWiFi.sendHttpPostRequest(WIFI_UPDATE_ENDPOINT, buildJSON(MetaData::get(), data));
}

// The code for sending a payload and backing-up data is the same, so we use a template here.
template <typename T>
void trySend(const T& data)
{
    String payload = buildJSON(MetaData::get(), data);
    if (!XTrayWiFi.sendHttpPostRequest(WIFI_UPDATE_ENDPOINT, payload)) {
        // Could not send.
        XTrayWiFi.connect(0); // Try reconnect.

        if (option::get(SDCARD_DATA_BACKUP)) {
            // TODO: currently this backs up metadata as well (waste of space). Only backup important data.
            // TODO: serialise backup data in a space-efficient format. There's only so much space in a mini SD card...

            // Backup data in sd card.
            if (backup(payload)) {
                Serial.println("backed up data to sd card");
            } else {
                Serial.println("could not back up data");
            }
        }
        // TODO: send backed-up data over wifi when reconnected... need to agree on a backup protocol first tho...
    }
}

void wifiTrySend(const TrayData& data)
{
    trySend(data);
}

void wifiTrySend(const MinimalTrayData& data)
{
    trySend(data);
}
