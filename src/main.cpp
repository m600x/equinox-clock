#include "Equinox.h"

RemoteDebug Debug;

int hours;
int minutes;
int seconds;

void loop_ux(void *pvParameters) {
    init_colors();
    strip.begin();
    strip.setBrightness(255);
    strip.show();
    npx_clear();

    oled_init();

    while (1) {
        if (strip_mode.equals("spinner"))
            strip_loading_spinner();
        if (strip_mode.equals("rainbow"))
            strip_rainbow();
        if (strip_mode.equals("clock"))
            npx_clock(hours, minutes, seconds);
        if (oled_active) {
            oled_print_lines();
        }
        else {
            oled.clearDisplay();
            oled.display();
        }
        delay(20);
    }
}

void loop_oled(void *pvParameters) {
    unsigned int oled_sleep = 0;
    while (1) {
        if (digitalRead(BTN_LEFT_PIN) == HIGH) {
            oled_active = true;
            oled.invertDisplay(true);
            oled_sleep = millis() + 10000; // auto-sleep after 10s
        }
        if (digitalRead(BTN_RIGHT_PIN) == HIGH) {
            oled_active = false;
            oled.invertDisplay(false);
        }
        if (oled_active && millis() > oled_sleep) {
            oled_active = false;
            oled.invertDisplay(false);
        }
        delay(100);
    }
}

void setup() {
    Serial.begin(115200);
    pref_boot_count();

    builtin_led.begin();
    builtin_led.setBrightness(32);
    set_builtin_led(255, 255, 255);
    delay(500);
    set_builtin_led(255, 0, 0);
    delay(500);
    set_builtin_led(0, 255, 0);
    delay(500);
    set_builtin_led(0, 0, 255);
    delay(500);
    set_builtin_led(0, 0, 0);
    delay(3000);

    pinMode(BTN_LEFT_PIN, INPUT);
    pinMode(BTN_RIGHT_PIN, INPUT);

    xTaskCreatePinnedToCore(loop_ux, "loop_ux", 4096, NULL, 1, NULL, 0);
    xTaskCreatePinnedToCore(loop_oled, "loop_oled", 8192, NULL, 1, NULL, 1);

    if (!pref_has_value("ssid") || pref_get_string("ssid").length() == 0) {
        strip_mode = "rainbow";
        Serial.println("No credentials");
        start_ap();
        set_builtin_led(255, 0, 0);
    }
    else {
        Serial.println("Credentials found");
        String ssid = pref_get_string("ssid");
        String pass = pref_get_string("pass");
        Serial.print("SSID: ");
        Serial.println(ssid);

        WiFi.disconnect(true);
        WiFi.mode(WIFI_STA);
        WiFi.setHostname("Equinox");
        connect_start_time = millis();

        if (pass.length() > 0)
            WiFi.begin(ssid.c_str(), pass.c_str());
        else
            WiFi.begin(ssid.c_str());

        strip_mode = "spinner";
        while (WiFi.status() != WL_CONNECTED) {
            if (millis() - connect_start_time > CONNECT_TIMEOUT_MS) {
                Serial.println("Connection timeout, switching to AP mode");
                start_ap();
                set_builtin_led(255, 0, 0);
                return;
            }
            set_builtin_led(255, 165, 0);
            Serial.print(".");
            delay(10);
        }

        init_ota();

        Debug.begin("Equinox");
        Debug.setResetCmdEnabled(true);
        Debug.showProfiler(true);
        Debug.showColors(true);
        Serial.println("RemoteDebug ready");

        pref_set_int("boot_count", 0);

        String tz = pref_get_string("timezone");
        Serial.print("Timezone: ");
        Serial.println(tz);
        timezone_offset = parse_utc_offset(tz);
        Serial.print("Offset: ");
        Serial.println(timezone_offset);

        configTime(timezone_offset, 0, "pool.ntp.org", "time.google.com");
        sync_time(timezone_offset);
        set_builtin_led(0, 255, 0);
    }
}

void loop() {
    ArduinoOTA.handle();
    Debug.handle();
    server.handleClient();

    if (WiFi.status() == WL_CONNECTED) {
        if (!time_synced)
            sync_time(timezone_offset);
        display_time();
    } else {
        if (credentials_saved) {
            npx_clear();
            set_builtin_led(0, 0, 0);
            delay(100);
            ESP.restart();
            while (1);
        }
    }
    delay(20);
}