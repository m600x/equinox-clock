#include "Equinox.h"

void logger(String message) {
    Serial.println(message);
}

int parse_utc_offset(const String &utc_offset) {
    if (utc_offset.length() < 5) return 0;
    int sign = (utc_offset[0] == '-') ? -1 : 1;
    int hours = utc_offset.substring(1, 3).toInt();
    return sign * hours * 3600;
}
