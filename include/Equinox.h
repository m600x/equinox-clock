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

#define NAME "Equinox"
#define BUILTIN_LED_PIN 21
#define STRIP_PIN 3
#define STRIP_NUM_PIXELS 60
#define FADE 20
#define MCU_POSITION "bottom"
#define CONNECT_TIMEOUT_MS 300000

#define OLED_SDA_PIN 2
#define OLED_SCL_PIN 1
#define OLED_ADDR 0x3C
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define BTN_LEFT_PIN 4
#define BTN_RIGHT_PIN 5

extern Adafruit_NeoPixel builtin_led;
extern Adafruit_NeoPixel strip;
extern Adafruit_SSD1306 oled;
extern Preferences preferences;
extern WebServer server;
extern RemoteDebug Debug;
extern bool credentials_saved;
extern bool time_synced;
extern unsigned long connect_start_time;
extern int timezone_offset;
extern String timezone_name;
extern String strip_mode;
extern String oled_lines[3];
extern bool oled_active;
extern int hours;
extern int minutes;
extern int seconds;

void init_colors();
int get_offset(int pos);
void set_builtin_led(uint8_t r, uint8_t g, uint8_t b);
void set_strip_color(uint16_t index, uint32_t color);
void npx_clear();
void npx_trailing(int pixel, uint32_t clr_main, uint32_t clr_fade);
void npx_trailing_set(int npx_p2, int npx_p1, int npx_n1, int npx_n2, int npx_hours, int npx_minutes);
void npx_trailing_sec(int npx_seconds, int npx_hours, int npx_minutes);
void npx_clock(int hours, int minutes, int seconds);
void display_time();
bool retrieve_timezone(int &offset_seconds, String &timezone_name);
void handle_root();
void handle_save();
void sync_time(int timezone_offset);
int parse_utc_offset(const String &utc_offset);

// display.cpp
void strip_loading_spinner();
void strip_rainbow();

// oled.cpp
void oled_init();
void oled_print_lines();

// preferences.cpp
bool pref_has_value(String key);

String pref_get_string(String key);
int pref_get_int(String key);
bool pref_set_int(String key, int value);
bool pref_set_string(String key, String value) ;


void pref_clear_memory();
void pref_boot_count();

// utils.cpp
void logger(String message);

// network.cpp
void start_ap();
void init_ota();

#endif