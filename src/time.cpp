#include "Equinox.h"

timeStruct timeState;

void update_time() {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo))
        return;
    timeState.hours = timeinfo.tm_hour;
    timeState.minutes = timeinfo.tm_min;
    timeState.seconds = timeinfo.tm_sec;

    stripState.mode = "clock";
    timeState.time_str = (timeState.hours < 10 ? "0" : "") + String(timeState.hours) + ":"
                        + (timeState.minutes < 10 ? "0" : "") + String(timeState.minutes) + ":"
                        + (timeState.seconds < 10 ? "0" : "") + String(timeState.seconds);
}

int parse_utc_offset(const String &utc_offset) {
    if (utc_offset.length() < 5) return 0;
    int sign = (utc_offset[0] == '-') ? -1 : 1;
    int hours = utc_offset.substring(1, 3).toInt();
    return sign * hours * 3600;
}

void sync_time(int timezone_offset) {
    configTime(timezone_offset, 0, "pool.ntp.org", "time.google.com");
    struct tm timeinfo;
    if (getLocalTime(&timeinfo)) {
        Serial.println("Time synced");
        timeState.synced = true;
        timeState.hours = timeinfo.tm_hour;
        timeState.minutes = timeinfo.tm_min;
        timeState.seconds = timeinfo.tm_sec;
        set_builtin_led(0, 255, 0);
    }
}

void time_init() {
    if (WiFi.status() == WL_CONNECTED) {
        String tz = pref_get_string("timezone");
        Serial.print("Timezone: ");
        Serial.println(tz);
        timeState.tz_offset = parse_utc_offset(tz);
        Serial.print("Offset: ");
        Serial.println(timeState.tz_offset);

        sync_time(timeState.tz_offset);
        set_builtin_led(0, 255, 0);
    }
}