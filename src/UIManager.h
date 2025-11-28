#ifndef UIMANAGER_H
#define UIMANAGER_H

#include "UIScreen.h"
#include "UIDataModel.h"
#include "LockScreen.h"
#include "DiagnosticScreen.h"
#include <Adafruit_ILI9341.h>

class UIManager {
public:
    UIManager(Adafruit_ILI9341* tft, UIDataModel* data);

    void begin();
    void loop();

    // gesture handlers (swipe)
    void onSwipeUp();
    void onSwipeLeft();
    void onSwipeRight();

    // touch tracking
    void onTouchDown(int x, int y);
    void onTouchUp(int x, int y);

private:
    Adafruit_ILI9341* tft;
    UIDataModel* data;

    // gesture state
    int  touchStartX;
    int  touchStartY;
    bool touching;

    // screen enums
    enum ScreenID { LOCK, DIAGNOSTIC, PATIENT, HISTORY };
    ScreenID current;

    // screen pointers
    UIScreen* lockScreen;
    UIScreen* diagnosticScreen;
    UIScreen* patientScreen;
    UIScreen* historyScreen;

    void switchTo(ScreenID s);
};

#endif
