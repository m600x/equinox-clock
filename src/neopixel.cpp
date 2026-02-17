#include "Equinox.h"

Adafruit_NeoPixel builtin_led(1, BUILTIN_LED_PIN, NEO_RGB + NEO_KHZ800);
Adafruit_NeoPixel strip(STRIP_NUM_PIXELS, STRIP_PIN, NEO_GRB + NEO_KHZ800);

stripStruct stripState;

static void read_time_snapshot(int &h, int &m, int &s, int &ms) {
    portENTER_CRITICAL(&timeMux);
    h = timeState.hours;
    m = timeState.minutes;
    s = timeState.seconds;
    ms = timeState.millis;
    portEXIT_CRITICAL(&timeMux);
}

int get_offset(int pos) {
    String posStr = String(MCU_POSITION);
    if (posStr.equalsIgnoreCase("bottom")) {
        pos += (STRIP_NUM_PIXELS / 2);
        if (pos > STRIP_NUM_PIXELS - 1) pos -= STRIP_NUM_PIXELS;
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

void npx_clear() {
    for (int i = 0; i < STRIP_NUM_PIXELS; i++) {
        set_strip_color(i, STRIP_BLACK);
    }
}

void npx_trailing(int pixel, uint32_t color) {
    if (pixel < 0 || pixel >= STRIP_NUM_PIXELS) return;
    set_strip_color(pixel, color);
    int prev = pixel - 1;
    int next = pixel + 1;
    if (prev < 0)
        prev = STRIP_NUM_PIXELS - 1;
    if (next > STRIP_NUM_PIXELS - 1)
        next = 0;
    if (strip.getPixelColor(prev) == STRIP_BLACK)
        set_strip_color(prev, darken_color(color));
    if (strip.getPixelColor(next) == STRIP_BLACK)
        set_strip_color(next, darken_color(color));
}

void npx_trailing_set_timebased(int npx_p2, int npx_p1, int npx_n1, int npx_n2,
                                int npx_hours, int npx_minutes, int ms) {
    int i = (ms * FADE) / 1000;
    if (i < 0) i = 0;
    if (i >= FADE) i = FADE - 1;

    uint32_t clr_n2 = strip.Color(0, map(i, 0, FADE, 10, 128),  map(i, 0, FADE, 10, 128));
    uint32_t clr_n1 = strip.Color(0, map(i, 0, FADE, 128, 255), map(i, 0, FADE, 128, 255));
    uint32_t clr_p1 = strip.Color(0, map(i, 0, FADE, 255, 128), map(i, 0, FADE, 255, 128));
    uint32_t clr_p2 = strip.Color(0, map(i, 0, FADE, 128, 10),  map(i, 0, FADE, 128, 10));

    if (npx_n2 != npx_hours && npx_n2 != npx_minutes) set_strip_color(npx_n2, clr_n2);
    if (npx_n1 != npx_hours && npx_n1 != npx_minutes) set_strip_color(npx_n1, clr_n1);
    if (npx_p1 != npx_hours && npx_p1 != npx_minutes) set_strip_color(npx_p1, clr_p1);
    if (npx_p2 != npx_hours && npx_p2 != npx_minutes) set_strip_color(npx_p2, clr_p2);
}

void npx_trailing_sec(int npx_seconds, int npx_hours, int npx_minutes, int ms) {
    int npx_n2 = npx_seconds + 2;
    int npx_n1 = npx_seconds + 1;
    int npx_p1 = npx_seconds - 1;
    int npx_p2 = npx_seconds - 2;
    switch (npx_seconds) {
        case STRIP_NUM_PIXELS - 2: { npx_n2 = 0; npx_n1 = STRIP_NUM_PIXELS - 1; break; }
        case STRIP_NUM_PIXELS - 1: { npx_n2 = 1; npx_n1 = 0; break; }
        case 0: {
            npx_n1 = npx_seconds + 1;
            npx_n2 = npx_seconds + 2;
            npx_p1 = STRIP_NUM_PIXELS - 1;
            npx_p2 = STRIP_NUM_PIXELS - 2;
            break;
        }
        case 1: {
            npx_n1 = npx_seconds + 1;
            npx_n2 = npx_seconds + 2;
            npx_p1 = 0;
            npx_p2 = STRIP_NUM_PIXELS - 1;
            break;
        }
    }
    npx_trailing_set_timebased(npx_p2, npx_p1, npx_n1, npx_n2, npx_hours, npx_minutes, ms);
}

void strip_clock() {
    int h, m, s, ms;
    read_time_snapshot(h, m, s, ms);

    int h12 = h % 12;

    npx_clear();

    float hour_pos = (h12 + (float)m / 60.0) * 5;
    int npx_hours = get_offset((int)(hour_pos + 0.5));
    int npx_minutes = get_offset(m);
    int npx_seconds = get_offset(s);
    set_strip_color(npx_minutes, STRIP_COLOR_MINUTES);

    npx_trailing(npx_hours, STRIP_COLOR_HOURS);

    if (npx_seconds != npx_hours && npx_seconds != npx_minutes)
        set_strip_color(npx_seconds, STRIP_COLOR_SECONDS);

    npx_trailing_sec(npx_seconds, npx_hours, npx_minutes, ms);
    strip.show();
}

void strip_rainbow() {
    uint32_t hue_offset = (millis() / 10) % 360;
    for (int i = 0; i < STRIP_NUM_PIXELS; i++) {
        uint16_t hue = (hue_offset + (STRIP_NUM_PIXELS - i) * 360 / STRIP_NUM_PIXELS) % 360;
        set_strip_color(i, strip.ColorHSV(hue * 182, 255, 255));
    }
    strip.show();
}

void strip_loading_spinner() {
    int spinner = 1;
    if (pref_has_value("boot_count") && pref_get_int("boot_count") > 1) {
        spinner = pref_get_int("boot_count");
    }

    npx_clear();
    uint32_t head = (millis() / 80) % STRIP_NUM_PIXELS;
    uint8_t brightness[] = {2, 5, 10, 18, 30, 50, 80, 120, 180, 255};

    for (int s = 0; s < spinner; s++) {
        int spinner_offset = (STRIP_NUM_PIXELS / spinner) * s;
        for (int i = 0; i < 10; i++) {
            int pos = (head + i + spinner_offset) % STRIP_NUM_PIXELS;
            set_strip_color(pos, strip.ColorHSV(0x8000, 255, brightness[i]));
        }
    }
    strip.show();
}

void strip_init() {
    strip.begin();
    strip.setBrightness(255);
    strip.show();
}