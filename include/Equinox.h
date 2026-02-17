#ifndef EQUINOX_H
#define EQUINOX_H

#include <Arduino.h>
#include <HardwareSerial.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>
#include <HTTPClient.h>
#include <time.h>
#include <ArduinoOTA.h>
#include "RemoteDebug.h"
#include <Adafruit_NeoPixel.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <Wire.h>
#include <SPI.h>
#include <freertos/FreeRTOS.h>
#include <freertos/portmacro.h>

#define NAME "Equinox"
#define BUILTIN_LED_PIN 21
#define STRIP_PIN 3
#define STRIP_NUM_PIXELS 60
#define FADE 20
#define MCU_POSITION "bottom"
#define CONNECT_TIMEOUT_MS 300000

#define BTN_LEFT_PIN 4
#define BTN_RIGHT_PIN 5

#define STRIP_RED 0xFF0000
#define STRIP_GREEN 0x00FF00
#define STRIP_BLUE 0x0000FF
#define STRIP_WHITE 0xFFFFFF
#define STRIP_BLACK 0x000000
#define STRIP_ORANGE 0xFF8000
#define STRIP_PURPLE 0xFF00FF
#define STRIP_YELLOW 0xFFFF00
#define STRIP_CYAN 0x00FFFF
#define STRIP_MAGENTA 0xFF00FF

#define STRIP_COLOR_HOURS        STRIP_YELLOW
#define STRIP_COLOR_MINUTES      STRIP_MAGENTA
#define STRIP_COLOR_SECONDS      STRIP_CYAN

extern Adafruit_NeoPixel builtin_led;
extern Adafruit_NeoPixel strip;
extern Adafruit_SSD1306 oled;
extern Preferences preferences;
extern WebServer server;
extern RemoteDebug Debug;
extern bool debugger_available;

struct timeStruct {
    bool synced = false;
    unsigned int last_sync = 0;
    String tz_name = "";
    int tz_offset = 0;

    int hours = 0;
    int minutes = 0;
    int seconds = 0;
    int millis = 0;

    char time_str[9] = "00:00:00";
    int refresh_rate = 0;
};
extern timeStruct timeState;
extern portMUX_TYPE timeMux;


struct stripStruct {
    String mode = "";
};
extern stripStruct stripState;


struct oledStruct {
    bool active = true;
    String lines[3] = {"", "", ""};
    unsigned long sleep = 0;
};
extern oledStruct oledState;


// neopixel.cpp
void set_builtin_led(uint8_t r, uint8_t g, uint8_t b);
void strip_clock();
void strip_rainbow();
void strip_loading_spinner();
void strip_init();

// network.cpp
void start_ap();
void debugger_init();
void ota_init();
void wifi_init();

// oled.cpp
void oled_main();
void oled_print_lines();
void oled_init();

// preferences.cpp
bool pref_has_value(String key);
String pref_get_string(String key);
bool pref_set_string(String key, String value) ;
int pref_get_int(String key);
bool pref_set_int(String key, int value);
void pref_clear_memory();
void pref_boot_count();

// time.cpp
void sync_remote_time();

// utils.cpp
uint32_t darken_color(uint32_t c);
void format_time_str_unsafe();
void logger(String message);

// web.cpp
void handle_root();
void handle_save();

#endif