#include "Equinox.h"

Preferences preferences;

static const char* NAMESPACE = "Equinox";

bool pref_has_value(String key) {
    preferences.begin(NAMESPACE);
    bool exists = preferences.isKey(key.c_str());
    preferences.end();
    return exists;
}

String pref_get_string(String key) {
    preferences.begin(NAMESPACE);
    String value = preferences.getString(key.c_str(), "");
    preferences.end();
    return value;
}

int pref_get_int(String key) {
    preferences.begin(NAMESPACE);
    int value = preferences.getInt(key.c_str(), -1);
    preferences.end();
    return value;
}

bool pref_set_int(String key, int value) {
    preferences.begin(NAMESPACE);
    size_t result = preferences.putInt(key.c_str(), value);
    preferences.end();
    return result > 0;
}

bool pref_set_string(String key, String value) {
    preferences.begin(NAMESPACE);
    size_t result = preferences.putString(key.c_str(), value);
    preferences.end();
    return result > 0;
}


void pref_clear_memory() {
    preferences.begin(NAMESPACE);
    preferences.clear();
    preferences.end();
}

void pref_boot_count() {
    if (pref_has_value("boot_count")) {
        int count = pref_get_int("boot_count");
        count++;
        bool success = pref_set_int("boot_count", count);
        oled_lines[0] = "Boot [" + String(count) + "]";
        if (count >= 5) {
            Serial.println("Clearing credentials...");
            pref_clear_memory();
        }
    } else {
        pref_set_int("boot_count", 0);
    }
}
