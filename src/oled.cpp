#include "Equinox.h"

Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT);

// 0: boot_count
// 1: wifi
String oled_lines[3] = {"", "", ""};
bool oled_active = true;

void oled_print_lines() {
    oled.clearDisplay();
    oled.setTextSize(2);
    oled.setTextColor(SSD1306_WHITE);
    for (int i = 0; i < 3; i++) {
        oled.setCursor(1, 1 + (i * 15));
        oled.print(oled_lines[i]);
    }
    oled.display();
}

void oled_init() {
    Wire.begin(OLED_SDA_PIN, OLED_SCL_PIN);
    if (!oled.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
        Serial.println("OLED init failed");
        return;
    }
    oled.clearDisplay();
//    oled.setRotation(2);
    oled.invertDisplay(true);
    oled.display();
}
