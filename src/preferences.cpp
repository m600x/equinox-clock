#include "Equinox.h"

Preferences preferences;

bool pref_has_value(String key) {
    preferences.begin(NAME);
    bool exists = preferences.isKey(key.c_str());
    preferences.end();
    return exists;
}

String pref_get_string(String key) {
    preferences.begin(NAME);
    String value = preferences.getString(key.c_str(), "");
    logger("Retrieved key: [" + key + "] value: [" + value + "]");
    preferences.end();
    return value;
}

bool pref_set_string(String key, String value) {
    logger("Set: [" + key + "] with value: [" + value + "]");
    preferences.begin(NAME);
    size_t result = preferences.putString(key.c_str(), value);
    preferences.end();
    return result > 0;
}

int pref_get_int(String key) {
    preferences.begin(NAME);
    int value = preferences.getInt(key.c_str(), -1);
    logger("Retrieved key: [" + key + "] value: [" + String(value) + "]");
    preferences.end();
    return value;
}

bool pref_set_int(String key, int value) {
    logger("Set: [" + key + "] with value: [" + String(value) + "]");
    preferences.begin(NAME);
    size_t result = preferences.putInt(key.c_str(), value);
    preferences.end();
    return result > 0;
}

void pref_clear_memory() {
    logger("Clearing memory");
    preferences.begin(NAME);
    preferences.clear();
    preferences.end();
}

void pref_boot_count() {
    if (pref_has_value("boot_count")) {
        int count = pref_get_int("boot_count");
        count++;
        bool success = pref_set_int("boot_count", count);
        oledState.lines[0] = "Boot [" + String(count) + "]";
        if (count >= 5) {
            logger("Clearing credentials, 5 consecutive reboot");
            pref_clear_memory();
        }
    } else {
        pref_set_int("boot_count", 0);
    }
}
