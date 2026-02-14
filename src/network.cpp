#include "Equinox.h"

void start_ap() {
    WiFi.disconnect(true);
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(IPAddress(10, 0, 0, 1), IPAddress(10, 0, 0, 1), IPAddress(255, 255, 255, 0));
    WiFi.softAP(NAME);
    server.on("/", HTTP_GET, handle_root);
    server.on("/save", HTTP_POST, handle_save);
    server.begin();

    stripState.mode = "rainbow";
    oledState.active = true;
    oledState.lines[1] = NAME;
    oledState.lines[2] = "10.0.0.1";
}

void debugger_init() {
    Debug.begin(NAME);
    Debug.setResetCmdEnabled(true);
    Debug.showTime(true);
    Debug.showProfiler(true);
    Debug.showColors(true);
    debugger_available = true;
    logger("RemoteDebug ready");
}

void ota_init() {
    ArduinoOTA.setHostname(NAME);
    ArduinoOTA
        .onStart([]() {
            String type;
            if (ArduinoOTA.getCommand() == U_FLASH)
                type = "sketch";
            else
                type = "filesystem";
            logger("Start updating " + type);
        })
        .onEnd([]() {
            logger("\nEnd");
        })
        .onProgress([](unsigned int progress, unsigned int total) {
            Debug.printf("Progress: %u%%\r", (progress / (total / 100)));
        })
        .onError([](ota_error_t error) {
            Debug.printf("Error[%u]: ", error);
            if (error == OTA_AUTH_ERROR) logger("Auth Failed");
            else if (error == OTA_BEGIN_ERROR) logger("Begin Failed");
            else if (error == OTA_CONNECT_ERROR) logger("Connect Failed");
            else if (error == OTA_RECEIVE_ERROR) logger("Receive Failed");
            else if (error == OTA_END_ERROR) logger("End Failed");
        });
    ArduinoOTA.begin();
}
