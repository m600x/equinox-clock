#include "Equinox.h"

RemoteDebug Debug;

int hours;
int minutes;
int seconds;

void loop_ux(void *pvParameters) {
    strip_init();
    oled_init();

    while (1) {
        if (stripState.mode.equals("spinner"))
            strip_loading_spinner();
        if (stripState.mode.equals("rainbow"))
            strip_rainbow();
        if (stripState.mode.equals("clock"))
            strip_clock();
        if (oledState.active) {
            oled_main();
//            oled_print_lines();
        }
        else {
            oled.clearDisplay();
            oled.display();
        }
        delay(20);
    }
}

void loop_cron(void *pvParameters) {
    oledState.sleep = millis() + 30000000; //TODO DECREASE 1000
    while (1) {
        if (oledState.active && millis() > oledState.sleep) {
            oledState.active = false;
        }
        delay(100);
    }
}

void btn_left_interrupt() {
    oledState.active = true;
    oledState.sleep = millis() + 10000; 
}

void btn_right_interrupt() {
    stripState.mode = (stripState.mode.equals("rainbow")) ? "clock" : "rainbow";
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
    delay(500);

    pinMode(BTN_LEFT_PIN, INPUT);
    attachInterrupt(digitalPinToInterrupt(BTN_LEFT_PIN), btn_left_interrupt, FALLING);
    pinMode(BTN_RIGHT_PIN, INPUT);
    attachInterrupt(digitalPinToInterrupt(BTN_RIGHT_PIN), btn_right_interrupt, FALLING);

    xTaskCreatePinnedToCore(loop_ux, "loop_ux", 4096, NULL, 1, NULL, 0);
    xTaskCreatePinnedToCore(loop_cron, "loop_cron", 8192, NULL, 1, NULL, 1);

    if (!pref_has_value("ssid") || pref_get_string("ssid").length() == 0) {
        
        Serial.println("No credentials");
        start_ap();
        set_builtin_led(255, 0, 0);
    }
    else {
        oledState.lines[0] = "Connecting";
        oledState.lines[2] = pref_get_string("ssid");
        stripState.mode = "spinner";

        Serial.println("Credentials found");
        String ssid = pref_get_string("ssid");
        String pass = pref_get_string("pass");
        Serial.print("SSID: ");
        Serial.println(ssid);

        WiFi.disconnect(true);
        WiFi.mode(WIFI_STA);
        WiFi.setHostname(NAME);
        unsigned int connect_start_time = millis();

        if (pass.length() > 0)
            WiFi.begin(ssid.c_str(), pass.c_str());
        else
            WiFi.begin(ssid.c_str());


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

        oledState.lines[0] = "";

        init_ota();

        Debug.begin(NAME);
        Debug.setResetCmdEnabled(true);
        Debug.showProfiler(true);
        Debug.showColors(true);
        Serial.println("RemoteDebug ready");

        pref_set_int("boot_count", 0);

        time_init();
    }
}

void loop() {
    ArduinoOTA.handle();
    Debug.handle();
    server.handleClient();

    if (WiFi.status() == WL_CONNECTED) {
        update_time();
    }

    delay(20);
}