#include "Equinox.h"

RemoteDebug Debug;

int hours;
int minutes;
int seconds;

/*
Keep timeStruct in sync down to the millis level, but only update the time string when seconds change.
This is to ensure accurate time keeping in the strip
*/
void loop_time(void *pvParameters) {
    struct timeval tv;
    struct tm tmLocal;
    time_t lastSec = (time_t)-1;

    sync_time();

    while (1) {
        gettimeofday(&tv, nullptr);
        int ms = (int)(tv.tv_usec / 1000);

        if (tv.tv_sec != lastSec) {
            lastSec = tv.tv_sec;
            localtime_r(&tv.tv_sec, &tmLocal);

            portENTER_CRITICAL(&timeMux);
            timeState.hours = tmLocal.tm_hour;
            timeState.minutes = tmLocal.tm_min;
            timeState.seconds = tmLocal.tm_sec;
            timeState.millis = ms;
            format_time_str_unsafe();
            portEXIT_CRITICAL(&timeMux);
        } else {
            portENTER_CRITICAL(&timeMux);
            timeState.millis = ms;
            portEXIT_CRITICAL(&timeMux);
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

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
        }
        else {
            oled.clearDisplay();
            oled.display();
        }
        delay(20);
    }
}

void loop_cron(void *pvParameters) {
    oledState.sleep = millis() + 10000;
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

    pinMode(BTN_LEFT_PIN, INPUT);
    attachInterrupt(digitalPinToInterrupt(BTN_LEFT_PIN), btn_left_interrupt, FALLING);
    pinMode(BTN_RIGHT_PIN, INPUT);
    attachInterrupt(digitalPinToInterrupt(BTN_RIGHT_PIN), btn_right_interrupt, FALLING);

    xTaskCreatePinnedToCore(loop_ux, "loop_ux", 4096, NULL, 1, NULL, 0);
    xTaskCreatePinnedToCore(loop_cron, "loop_cron", 8192, NULL, 1, NULL, 1);

    if (!pref_has_value("ssid") || pref_get_string("ssid").length() == 0) {
        logger("No credentials found, starting AP");
        start_ap();
        set_builtin_led(255, 0, 0);
    }
    else {
        String ssid = pref_get_string("ssid");
        String pass = pref_get_string("pass");
        logger("Credentials found, connecting to: [" + pass + "]");

        oledState.lines[0] = "Connecting";
        oledState.lines[2] = ssid;
        stripState.mode = "spinner";

        WiFi.disconnect(true);
        WiFi.mode(WIFI_STA);
        WiFi.setHostname(NAME);

        if (pass.length() > 0)
        WiFi.begin(ssid.c_str(), pass.c_str());
        else
        WiFi.begin(ssid.c_str());

        unsigned int connect_start_time = millis();
        while (WiFi.status() != WL_CONNECTED) {
            if (millis() - connect_start_time > CONNECT_TIMEOUT_MS) {
                logger("Connection timeout, switching to AP mode");
                start_ap();
                set_builtin_led(255, 0, 0);
                return;
            }
            set_builtin_led(255, 165, 0);
            delay(10);
        }

        oledState.lines[0] = "";

        ota_init();
        debugger_init();

        pref_set_int("boot_count", 0);

        xTaskCreatePinnedToCore(loop_time, "loop_time", 4096, NULL, 2, NULL, 0);
    }
}

void loop() {
    ArduinoOTA.handle();
    Debug.handle();
    server.handleClient();
    delay(20);
}