#include <Arduino.h>
#include "UIManager.h"

UIManager::UIManager(Adafruit_ILI9341* tft, UIDataModel* data)
: tft(tft),
  data(data),
  touchStartX(-1),
  touchStartY(-1),
  touching(false),
  current(LOCK)
{
    // real screens
    lockScreen       = new LockScreen(tft, data);
    diagnosticScreen = new DiagnosticScreen(tft, data);

    // placeholders for now
    patientScreen    = lockScreen;
    historyScreen    = lockScreen;
}

void UIManager::begin() {
    lockScreen->enter();
}

void UIManager::loop() {
    switch (current) {
        case LOCK:
            lockScreen->update();
            break;
        case DIAGNOSTIC:
            diagnosticScreen->update();
            break;
        case PATIENT:
            patientScreen->update();
            break;
        case HISTORY:
            historyScreen->update();
            break;
    }
}

// --------------------------------
// SCREEN SWITCH
// --------------------------------
void UIManager::switchTo(ScreenID s) {
    current = s;

    switch (current) {
        case LOCK:
            lockScreen->enter();
            break;
        case DIAGNOSTIC:
            diagnosticScreen->enter();
            break;
        case PATIENT:
            patientScreen->enter();
            break;
        case HISTORY:
            historyScreen->enter();
            break;
    }
}

// --------------------------------
// GESTURES
// --------------------------------
void UIManager::onSwipeUp() {
    Serial.println("[GESTURE] Swipe UP detected");
    if (current == LOCK) switchTo(DIAGNOSTIC);
}

void UIManager::onSwipeLeft() {
    Serial.println("[GESTURE] Swipe LEFT detected");
    if (current == DIAGNOSTIC) switchTo(PATIENT);
}

void UIManager::onSwipeRight() {
    Serial.println("[GESTURE] Swipe RIGHT detected");
    if (current == DIAGNOSTIC) switchTo(HISTORY);
}

void UIManager::onTouchDown(int x, int y) {
    touching    = true;
    touchStartX = x;
    touchStartY = y;
}

void UIManager::onTouchUp(int x, int y) {
    if (!touching) return;
    touching = false;

    int dx = x - touchStartX;   // behaves like vertical movement in your orientation
    int dy = y - touchStartY;   // behaves like horizontal movement

    const int threshold = 30;

    // In your portrait orientation:
    //  - big |dx| = vertical swipe
    //  - big |dy| = horizontal swipe
    if (abs(dx) > abs(dy)) {
        // VERTICAL SWIPES (bottom ↔ top)
        if (dx > threshold) {
            onSwipeUp();         // bottom -> up
        } else if (dx < -threshold) {
            Serial.println("[GESTURE] Swipe DOWN detected");
        }
    } else {
        // HORIZONTAL SWIPES (left ↔ right)
        if (dy > threshold) {
            onSwipeRight();      // left -> right
        } else if (dy < -threshold) {
            onSwipeLeft();       // right -> left
        }
    }
}
