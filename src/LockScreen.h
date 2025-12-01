#ifndef LOCKSCREEN_H
#define LOCKSCREEN_H

#include "UIScreen.h"
#include "UIDataModel.h"
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include "TFTHelpers.h"

class LockScreen : public UIScreen {
public:
    LockScreen(Adafruit_ILI9341* tft, UIDataModel* data);

    void enter() override;
    void exit() override;
    void drawFull() override;
    void update() override;

private:
    Adafruit_ILI9341* tft;
    UIDataModel* data;

    // Fade for "Swipe up to start"
    uint8_t fadeValue;
    bool    fadeDirection;

    // Bouncing heart animation
    int  heartBaseY;
    int  heartOffset;
    bool heartGoingUp;

    void drawHeart();   // draw one frame of heart
};

#endif










/*

#ifndef LOCKSCREEN_H
#define LOCKSCREEN_H

#include "UIScreen.h"
#include "UIDataModel.h"
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include "TFTHelpers.h"

class LockScreen : public UIScreen {
public:
    LockScreen(Adafruit_ILI9341* tft, UIDataModel* data);

    void enter() override;
    void exit() override;
    void drawFull() override;
    void update() override;

private:
    Adafruit_ILI9341* tft;
    UIDataModel* data;

    // Fade animation for "Swipe up to start"
    uint8_t fadeValue;
    bool    fadeDirection;
};

#endif
*/