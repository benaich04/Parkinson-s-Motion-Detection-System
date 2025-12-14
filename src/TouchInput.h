#ifndef TOUCHINPUT_H
#define TOUCHINPUT_H

#include <Adafruit_TSC2007.h>

struct TouchPoint {
    bool touched = false;
    int x = 0;
    int y = 0;
};

class TouchInput {
public:
    TouchInput();
    void begin();
    TouchPoint read();

private:
    Adafruit_TSC2007 ts;
    int16_t min_x, max_x, min_y, max_y;
};

#endif
