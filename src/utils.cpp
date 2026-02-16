#include "Equinox.h"

bool debugger_available = false;

void format_time_str_unsafe() {
    snprintf(timeState.time_str, sizeof(timeState.time_str),
             "%02d:%02d:%02d",
             timeState.hours, timeState.minutes, timeState.seconds);
}

void logger(String message) {
    char tbuf[9];
    portENTER_CRITICAL(&timeMux);
    memcpy(tbuf, timeState.time_str, sizeof(tbuf));
    portEXIT_CRITICAL(&timeMux);

    String payload = String("[") + tbuf + "] " + message;
    if (debugger_available) {
        Debug.printf(payload.c_str());
    }
    Serial.println(payload);
}

