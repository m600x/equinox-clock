#include "Equinox.h"
#include <sys/time.h>

timeStruct timeState;
portMUX_TYPE timeMux = portMUX_INITIALIZER_UNLOCKED;

int parse_utc_offset(const String &utc_offset) {
    if (utc_offset.length() < 5) return 0;
    int sign = (utc_offset[0] == '-') ? -1 : 1;
    int hours = utc_offset.substring(1, 3).toInt();
    return sign * hours * 3600;
}

void sync_remote_time() {
    if (WiFi.status() == WL_CONNECTED) {
        struct timeval tv;
        struct tm tmLocal;

        String tz = pref_get_string("timezone");
        timeState.tz_offset = parse_utc_offset(tz);

        configTime(timeState.tz_offset, 0, "pool.ntp.org", "time.google.com");

        if (gettimeofday(&tv, nullptr) != 0)
            return;
        localtime_r(&tv.tv_sec, &tmLocal);

        portENTER_CRITICAL(&timeMux);
        timeState.synced = true;
        timeState.hours = tmLocal.tm_hour;
        timeState.minutes = tmLocal.tm_min;
        timeState.seconds = tmLocal.tm_sec;
        timeState.millis = (int)(tv.tv_usec / 1000);
        format_time_str_unsafe();
        portEXIT_CRITICAL(&timeMux);

        logger("Time synced");
        stripState.mode = "clock";
        set_builtin_led(0, 255, 0);
    }
}