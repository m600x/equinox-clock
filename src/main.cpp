#include <Arduino.h>
#include <WiFi.h>
#include <time.h>
#include <Adafruit_NeoPixel.h>

#define WIFI_SSID
#define WIFI_PASS

#define BUILTIN_LED_PIN 21
#define STRIP_PIN 1
#define STRIP_NUM_PIXELS 60

#define TIMEZONE 2
#define FADE 20
#define BOTTOM_WIRE 0

Adafruit_NeoPixel builtin_led(1, BUILTIN_LED_PIN, NEO_RGB + NEO_KHZ800);
Adafruit_NeoPixel strip(STRIP_NUM_PIXELS, STRIP_PIN, NEO_GRB + NEO_KHZ800);

int last_hour = -1;
int last_minute = -1;
int last_second = -1;

uint32_t CLR_HRS_MAIN;
uint32_t CLR_HRS_FADE;
uint32_t CLR_MIN_BLK1;
uint32_t CLR_MIN_BLK2;
uint32_t CLR_MIN_FADE;
uint32_t CLR_SEC_MAIN;
uint32_t CLR_BLACK;

void init_colors() {
    CLR_HRS_MAIN = strip.Color(255, 255, 0);
    CLR_HRS_FADE = strip.Color(50, 50, 0);
    CLR_MIN_BLK1 = strip.Color(255, 0, 255);
    CLR_MIN_BLK2 = strip.Color(128, 0, 128);
    CLR_MIN_FADE = strip.Color(50, 0, 50);
    CLR_SEC_MAIN = strip.Color(0, 255, 255);
    CLR_BLACK = strip.Color(0, 0, 0);
}

int get_offset(int pos) {
    if (BOTTOM_WIRE) {
        pos += (STRIP_NUM_PIXELS / 2);
        if (pos > STRIP_NUM_PIXELS - 1) {
            pos -= STRIP_NUM_PIXELS;
        }
    }
    return pos;
}

void set_builtin_led(uint8_t r, uint8_t g, uint8_t b) {
    builtin_led.setPixelColor(0, builtin_led.Color(r, g, b));
    builtin_led.show();
}

void set_strip_color(uint16_t index, uint32_t color) {
    strip.setPixelColor(index, color);
}

void strip_show() {
    strip.show();
}

void connect_wifi() {
    Serial.println("Connecting to WiFi...");
    Serial.print("SSID: ");
    Serial.println(WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    Serial.print("WiFi status: ");
    Serial.println(WiFi.status());

    int i = 0;
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        Serial.println(WiFi.status());
        set_builtin_led(255, 0, 0);
        delay(250);
        set_builtin_led(0, 0, 0);
        delay(250);
        if (i++ > 59) {
            i = 0;
            for (int x = 0; x < STRIP_NUM_PIXELS; x++) {
                set_strip_color(x, CLR_BLACK);
            }
            strip_show();
        }
        set_strip_color(i, strip.Color(255, 0, 0));
        strip_show();
    }
    Serial.println("Connected!");

    set_builtin_led(0, 255, 0);
    for (int x = 0; x < STRIP_NUM_PIXELS; x++) {
        set_strip_color(x, CLR_BLACK);
    }
    strip_show();

    configTime(TIMEZONE * 3600, 0, "pool.ntp.org", "time.google.com");
    struct tm timeinfo;
    if (getLocalTime(&timeinfo)) {
        Serial.println("Time synced");
        Serial.println(&timeinfo, "%Y-%m-%d %H:%M:%S");
    }
}

void npx_clear() {
    for (int i = 0; i < STRIP_NUM_PIXELS; i++) {
        set_strip_color(i, CLR_BLACK);
    }
}

void npx_trailing(int pixel, uint32_t clr_main, uint32_t clr_fade) {
    if (pixel < 0 || pixel >= STRIP_NUM_PIXELS) return;
    set_strip_color(pixel, clr_main);

    int prev = pixel - 1;
    int next = pixel + 1;
    if (prev < 0) prev = STRIP_NUM_PIXELS - 1;
    if (next > STRIP_NUM_PIXELS - 1) next = 0;

    if (strip.getPixelColor(prev) == CLR_BLACK) {
        set_strip_color(prev, clr_fade);
    }
    if (strip.getPixelColor(next) == CLR_BLACK) {
        set_strip_color(next, clr_fade);
    }
}

void npx_trailing_set(int npx_p2, int npx_p1, int npx_n1, int npx_n2, int npx_hours, int npx_minutes) {
    for (int i = 0; i < FADE; i++) {
        uint32_t clr_n2 = strip.Color(0, map(i, 0, FADE, 10, 128), map(i, 0, FADE, 10, 128));
        uint32_t clr_n1 = strip.Color(0, map(i, 0, FADE, 128, 255), map(i, 0, FADE, 128, 255));
        uint32_t clr_p1 = strip.Color(0, map(i, 0, FADE, 255, 128), map(i, 0, FADE, 255, 128));
        uint32_t clr_p2 = strip.Color(0, map(i, 0, FADE, 128, 10), map(i, 0, FADE, 128, 10));

        if (npx_n2 != npx_hours && npx_n2 != npx_minutes) {
            set_strip_color(npx_n2, clr_n2);
        }
        if (npx_n1 != npx_hours && npx_n1 != npx_minutes) {
            set_strip_color(npx_n1, clr_n1);
        }
        if (npx_p1 != npx_hours && npx_p1 != npx_minutes) {
            set_strip_color(npx_p1, clr_p1);
        }
        if (npx_p2 != npx_hours && npx_p2 != npx_minutes) {
            set_strip_color(npx_p2, clr_p2);
        }
        strip_show();
        delay(50);
    }
}

void npx_trailing_sec(int npx_seconds, int npx_hours, int npx_minutes) {
    int npx_n2 = npx_seconds + 2;
    int npx_n1 = npx_seconds + 1;
    int npx_p1 = npx_seconds - 1;
    int npx_p2 = npx_seconds - 2;

    switch (npx_seconds) {
        case STRIP_NUM_PIXELS - 2: {
            npx_n2 = 0;
            npx_n1 = STRIP_NUM_PIXELS - 1;
            break;
        }
        case STRIP_NUM_PIXELS - 1: {
            npx_n2 = 1;
            npx_n1 = 0;
            break;
        }
        case 0: {
            npx_n1 = npx_seconds + 2;
            npx_n2 = npx_seconds + 1;
            npx_p1 = STRIP_NUM_PIXELS - 1;
            npx_p2 = STRIP_NUM_PIXELS - 2;
            break;
        }
        case 1: {
            npx_n1 = npx_seconds + 2;
            npx_n2 = npx_seconds + 1;
            npx_p1 = 0;
            npx_p2 = STRIP_NUM_PIXELS - 1;
            break;
        }
    }
    npx_trailing_set(npx_p2, npx_p1, npx_n1, npx_n2, npx_hours, npx_minutes);
}

void npx_hourly() {
    for (int i = 0; i < STRIP_NUM_PIXELS; i++) {
        strip.setBrightness(map(i, 0, STRIP_NUM_PIXELS, 255, 0));
        strip_show();
        delay(8);
    }
    for (int i = 0; i < STRIP_NUM_PIXELS; i++) {
        set_strip_color(i, CLR_BLACK);
    }
    strip.setBrightness(255);
    strip_show();
}

void setup() {
    init_colors();
    Serial.begin(115200);
    builtin_led.begin();
    strip.begin();
    strip.setBrightness(255);
    connect_wifi();
}

void npx_clock(int hours, int minutes, int seconds) {
    npx_clear();

    int npx_hours = get_offset(hours * 5);
    int npx_minutes = get_offset(minutes);
    int npx_seconds = get_offset(seconds);

    npx_trailing(npx_minutes, (seconds % 2) ? CLR_MIN_BLK1 : CLR_MIN_BLK2, CLR_MIN_FADE);
    npx_trailing(npx_hours, CLR_HRS_MAIN, CLR_HRS_FADE);

    if (npx_seconds != npx_hours && npx_seconds != npx_minutes) {
        set_strip_color(npx_seconds, CLR_SEC_MAIN);
    }
    npx_trailing_sec(npx_seconds, npx_hours, npx_minutes);
}

void display_time() {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        return;
    }

    int hours = timeinfo.tm_hour;
    int minutes = timeinfo.tm_min;
    int seconds = timeinfo.tm_sec;

    if (seconds == 0 && last_second != seconds) {
        npx_hourly();
    }

    npx_clock(hours, minutes, seconds);
    last_second = seconds;

    Serial.print("Time: ");
    Serial.print(hours);
    Serial.print(":");
    Serial.print(minutes);
    Serial.print(":");
    Serial.println(seconds);
}

void loop() {
    if (WiFi.status() != WL_CONNECTED) {
        connect_wifi();
    }
    display_time();
}