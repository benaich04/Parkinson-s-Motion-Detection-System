#include <Adafruit_ILI9341.h>
#include "UIManager.h"
#include "UIDataModel.h"
#include "TouchInput.h"

// ----------------------------------
// GLOBALS
// ----------------------------------
Adafruit_ILI9341 tft = Adafruit_ILI9341(9, 10);
TouchInput touch;
UIDataModel data;
UIManager ui(&tft, &data);

bool wasTouched = false;
int lastTouchX = 0;
int lastTouchY = 0;

void setup() {
    Serial.begin(115200);
    delay(300);

    Serial.println("======== START SETUP =========");

    Serial.println("[1] Initializing TFT...");
    tft.begin();
    tft.setRotation(1);
    Serial.println("[1] TFT OK");

    Serial.println("[2] Initializing Touch...");
    touch.begin();
    Serial.println("[2] Touch OK");

    Serial.println("[3] Initializing UI Manager...");
    ui.begin();
    Serial.println("[3] UI Manager OK");

    Serial.println("======== SETUP COMPLETE ========");
}

void loop() {
    // Fake data (time & battery)
    data.hour = 12;
    data.minute = 34;
    data.day = 24;
    data.month = 11;
    data.year = 2025;
    data.batteryPercent = 95;

    // -------------------------------
    // TOUCH READING
    // -------------------------------
    TouchPoint p = touch.read();

    if (p.touched) {
        // store last coordinates
        lastTouchX = p.x;
        lastTouchY = p.y;

        if (!wasTouched) {
            Serial.print("[TOUCH] DOWN  x=");
            Serial.print(p.x);
            Serial.print("  y=");
            Serial.println(p.y);

            ui.onTouchDown(p.x, p.y);
            wasTouched = true;
        }
    } else {
        if (wasTouched) {
            Serial.println("[TOUCH] UP");
            // use last known coords for release
            ui.onTouchUp(lastTouchX, lastTouchY);
            wasTouched = false;
        }
    }

    // -------------------------------
    // UI LOOP
    // -------------------------------
    ui.loop();

    delay(50);  // small delay for stability
}
