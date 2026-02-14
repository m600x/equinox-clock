#include "Equinox.h"

bool debugger_available = false;

void logger(String message) {
    String payload = "[" + timeState.time_str + "] " + message;
    if (debugger_available) {
        Debug.printf(payload.c_str());
    }
    Serial.println(message);
}

