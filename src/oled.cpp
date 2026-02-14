#include "Equinox.h"

#define OLED_SDA_PIN 2
#define OLED_SCL_PIN 1
#define OLED_ADDR 0x3C
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT);

oledStruct oledState;

void oled_main() {
    oled.clearDisplay();
    oled.setTextColor(SSD1306_WHITE);

    oled.setTextSize(1);
    oled.setCursor(5, 5);
    oled.print(WiFi.localIP());
    oled.drawLine(0, 15, 128, 15, SSD1306_WHITE);
    
    oled.setTextSize(2);
    oled.setCursor(15, 25);
    oled.print(timeState.time_str);


    oled.drawLine(0, 50, 128, 50, SSD1306_WHITE);

    oled.display();
}

void oled_print_lines() {
    oled.clearDisplay();
    oled.setTextSize(2);
    oled.setTextColor(SSD1306_WHITE);
    for (int i = 0; i < 3; i++) {
        oled.setCursor(1, 1 + (i * 15));
        oled.print(oledState.lines[i]);
    }
    oled.display();
}

void oled_init() {
    Wire.begin(OLED_SDA_PIN, OLED_SCL_PIN);
    if (!oled.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
        logger("OLED init failed");
        return;
    }
    oled.clearDisplay();
    oled.display();
}
