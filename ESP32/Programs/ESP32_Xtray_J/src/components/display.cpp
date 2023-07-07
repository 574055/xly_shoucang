#include "defines.h"
#include "display.h"
#include "sensing.h"

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


Adafruit_SSD1306 oled(OLED_SCREEN_WIDTH, OLED_SCREEN_HEIGHT, &Wire, -1);
float disp_weight;
uint32_t disp_tags = 0;

Page page = PAGE_TEMP_HUM;


void displayInit()
{
    Wire.begin(OLED_SDA_PIN, OLED_SCK_PIN);
    if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println(F("failed to start SSD1306 OLED"));
        while (1)
            ;
    }

    delay(2000);              // wait before initializing
    oled.clearDisplay();      // clear display
    oled.setTextSize(2);      // set text size
    oled.setTextColor(WHITE); // set text color
}

void displaySetWeight(float weight) { disp_weight = weight; }
void displaySetNumTags(uint32_t tags) { disp_tags = tags; }

void displayCyclePage()
{
    static uint32_t last_cycle_update = 0;
    if (millis() - last_cycle_update > OLED_PAGE_CYCLE_INTERVAL) {
        last_cycle_update = millis();

        page = static_cast<Page>((int(page) + 1) % NUM_PAGES);
    }
}

void displayWakeUp() { oled.ssd1306_command(SSD1306_DISPLAYON); }

void displaySleep() { oled.ssd1306_command(SSD1306_DISPLAYOFF); }

void displayUpdate()
{
    static uint32_t last_ticks = 0;
    if (millis() - last_ticks > 500) {
        last_ticks = millis();

        oled.clearDisplay();
        oled.setTextSize(2);
        oled.setCursor(0, 0);
        switch (page) {
            case PAGE_TEMP_HUM:
                oled.printf("T %6.1f C\n", Sensors.temp.get());
                oled.printf("H %6.1f %%\n", Sensors.humi.get());
                break;
            case PAGE_WEIGHT_RFID: {
                oled.printf("%8.2fkg\n", (disp_weight < 0 ? 0 : disp_weight));
                oled.printf("%5d tags\n", disp_tags);
            } break;
            default: break;
        }
        oled.display();
    }

    displayCyclePage();
}

void displayf(uint8_t text_size, const char* fmt, ...)
{
    static char buffer[64];
    oled.clearDisplay();
    oled.setTextSize(text_size);
    oled.setCursor(0, 0);
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    oled.println(buffer);
    oled.display();
    va_end(args);
}
