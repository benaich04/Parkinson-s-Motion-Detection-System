#include "TouchInput.h"
#include <Arduino.h>
#include <Adafruit_ILI9341.h>
#include <Wire.h>

// calibration values
#define TSC_TS_MINX 300
#define TSC_TS_MAXX 3800
#define TSC_TS_MINY 185
#define TSC_TS_MAXY 3700

// TFT is defined in main.cpp
extern Adafruit_ILI9341 tft;

TouchInput::TouchInput()
: ts(Adafruit_TSC2007()) {}

void TouchInput::begin() {
    ts.begin(0x48, &Wire);

    min_x = TSC_TS_MINX;
    max_x = TSC_TS_MAXX;
    min_y = TSC_TS_MINY;
    max_y = TSC_TS_MAXY;
}

TouchPoint TouchInput::read() {
    TouchPoint p;

    TS_Point raw = ts.getPoint();

    if ((raw.x == 0 && raw.y == 0) || raw.z < 10) {
        p.touched = false;
        return p;
    }

    p.touched = true;

    // map to screen coords
    p.x = map(raw.x, min_x, max_x, 0, tft.width());
    p.y = map(raw.y, min_y, max_y, 0, tft.height());

    return p;
}
