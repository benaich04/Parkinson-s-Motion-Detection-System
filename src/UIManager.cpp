#include <Arduino.h>
#include "UIManager.h"

//this file implements the UIManager class, which handles screen management and touch gestures
//swipe gestures are mapped to screen transitions : up->Diagnostic, down->Lock, left->Patient, right->History

UIManager::UIManager(TFTHelpers::ILI9341_Display* tft, UIDataModel* data): 
  tft(tft),
  data(data),
  touchStartX(-1),
  touchStartY(-1),
  touching(false),
  current(LOCK)
{
    // Instantiate all screens
    lockScreen       = new LockScreen(tft, data);
    diagnosticScreen = new DiagnosticScreen(tft, data);
    patientScreen    = new PatientScreen(tft, data);
    historyScreen    = new HistoryScreen(tft, data);
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
// SCREEN SWITCH (Finite State Machine)
// --------------------------------
void UIManager::switchTo(ScreenID s) {
    // Optimization: Don't switch if already there
    if (current == s) return;

    // 1. Exit the CURRENT screen (Stop animations/timers)
    switch (current) {
        case LOCK:       lockScreen->exit();       break;
        case DIAGNOSTIC: diagnosticScreen->exit(); break;
        case PATIENT:    patientScreen->exit();    break;
        case HISTORY:    historyScreen->exit();    break;
    }

    // 2. Update State
    current = s;

    // 3. Enter the NEW screen (Draw UI)
    switch (current) {
        case LOCK:       lockScreen->enter();       break;
        case DIAGNOSTIC: diagnosticScreen->enter(); break;
        case PATIENT:    patientScreen->enter();    break;
        case HISTORY:    historyScreen->enter();    break;
    }
}

// --------------------------------
// GESTURES
// --------------------------------

void UIManager::onSwipeUp() {
    // UP -> Diagnostic (Analysis)
    switchTo(DIAGNOSTIC);
}

void UIManager::onSwipeDown() {
    // DOWN -> Lock Screen (Home)
    switchTo(LOCK);
}

void UIManager::onSwipeLeft() {
    // LEFT -> Patient Status (Advice/Status)
    switchTo(PATIENT);
}

void UIManager::onSwipeRight() {
    // RIGHT -> History (Stats/Logs)
    switchTo(HISTORY);
}

// --------------------------------
// TOUCH HANDLING
// --------------------------------

void UIManager::onTouchDown(int x, int y) {
    touching    = true;
    touchStartX = x;
    touchStartY = y;
}

void UIManager::onTouchUp(int x, int y) {
    if (!touching) return;
    touching = false;

    // Calculate distance moved
    int dx = x - touchStartX;   // Vertical axis in portrait
    int dy = y - touchStartY;   // Horizontal axis in portrait

    const int threshold = 35;   // Sensitivity threshold

    // Determine dominant axis (Vertical vs Horizontal)
    if (abs(dx) > abs(dy)) {
        // --- VERTICAL MOVEMENT ---
        if (dx > threshold) {
            onSwipeUp();         
        } else if (dx < -threshold) {
            onSwipeDown();
        }
    } else {
        // --- HORIZONTAL MOVEMENT ---
        if (dy > threshold) {
            onSwipeRight();      
        } else if (dy < -threshold) {
            onSwipeLeft();       
        }
    }
}


void UIManager::captureHistoryEvent() {
    ((HistoryScreen*)historyScreen)->captureEvent();
}