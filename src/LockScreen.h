#ifndef LOCKSCREEN_H
#define LOCKSCREEN_H

#include "UIScreen.h"
#include "UIDataModel.h"
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include "TFTHelpers.h"

class LockScreen : public UIScreen {
public:
    LockScreen(TFTHelpers::ILI9341_Display* tft, UIDataModel* data);

    void enter() override;
    void exit() override;
    void drawFull() override;
    void update() override;

private:
    TFTHelpers::ILI9341_Display* tft;
    UIDataModel* data;

    // Fade for "Swipe up to start"
    uint8_t fadeValue;
    bool    fadeDirection;

    // Bouncing heart animation
    int16_t heartBaseY;
    int8_t  heartOffset;
    bool    heartGoingUp;
    
    // FIX: Variable to track the heart's vertical position from the previous frame.
    int16_t previousHeartY; 

    void drawHeart();   // draw one frame of heart
};

#endif