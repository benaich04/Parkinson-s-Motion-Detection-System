#include <Arduino.h>
#include <Adafruit_ILI9341.h>
#include "UIManager.h"
#include "UIDataModel.h"
#include "TouchInput.h"
#include "SignalUIBridge.h"
#include "TFTHelpers.h"

// -------------------------------------------------------------------------
// HARDWARE SETUP
// -------------------------------------------------------------------------
TFTHelpers::ILI9341_Display tft = TFTHelpers::ILI9341_Display(9, 10);
TouchInput touch;
UIDataModel data;
UIManager ui(&tft, &data);

bool wasTouched = false;
int lastTouchX = 0;
int lastTouchY = 0;

// Track state changes for history capture
UIDataModel::MovementState lastState = UIDataModel::STABLE;

void setup() {

    // 1. TFT
    tft.begin();
    tft.setRotation(1);

    // 2. Touch
    touch.begin();

    // 3. UI Manager
    ui.begin();

    // 4. INTERNAL SIGNAL LOGIC
    //Pass the &data pointer so signals can write to the UI Model
    SignalUIBridge::begin(&data);
}

void loop() {

    // 1. Update IMU + SignalProcessor internally
    // (This checks its own timer, so calling it fast is fine)
    SignalUIBridge::update();

    // 2. TOUCH INPUT
    TouchPoint p = touch.read();

    if (p.touched) {
        lastTouchX = p.x;
        lastTouchY = p.y;

        if (!wasTouched) {
            ui.onTouchDown(p.x, p.y);
            wasTouched = true;
        }
    } else {
        if (wasTouched) {
            ui.onTouchUp(lastTouchX, lastTouchY);
            wasTouched = false;
        }
    }

    // 3. Update active screen
    ui.loop();

    // 4. Capture history events on state transitions
    // Only capture when entering TREMOR or DYSKINESIA (not when returning to STABLE)
    if (data.state != lastState && data.state != UIDataModel::STABLE) {
        ui.captureHistoryEvent();
    }
    lastState = data.state;

    // Keep delay small (5ms) for smooth animations and responsive swipes.
    // 50ms is too slow for good touch interaction.
    delay(5);
}