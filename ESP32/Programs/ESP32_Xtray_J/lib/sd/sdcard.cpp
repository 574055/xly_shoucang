#include "defines.h"
#include "sdcard.h"

#include <SD.h>

SPIClass spi{VSPI};


void sdInit()
{
    spi.begin(SD_CLK_PIN, SD_MISO_PIN, SD_MOSI_PIN, SD_CS_PIN);
    if (!SD.begin(SD_CS_PIN, spi)) {
        Serial.println("SD Card failed, or not present");
        return;
    }
    Serial.println("SD card initialized.");
    SD.mkdir(SD_BACKUP_DIR);
    // SD.mkdir(SD_LOG_DIR);
    SD.mkdir(SD_SETTINGS_DIR);
}


bool sdRead(String* out, String path, bool create)
{
    File file = SD.open(path, FILE_READ, create);
    if (!file)
        return false;

    static char buffer[256];
    while (file.available()) {
        size_t len = file.readBytes(buffer, sizeof(buffer) - 1);
        buffer[len] = '\0';
        *out += buffer;
    }

    file.close();
    return true;
}

bool sdWrite(String path, String data, bool create, bool append)
{
    File file = SD.open(path, append ? FILE_APPEND : FILE_WRITE, create);

    if (!file)
        return false;

    file.println(data);
    file.close();
    return true;
}
