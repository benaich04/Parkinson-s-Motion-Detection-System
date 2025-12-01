/*
#include <Arduino.h>
#include "IMU_I2C.h"
#include "SignalProcessor.h"
#include "FFTModule.h"   // for FFT_SIZE and SAMPLING_FREQUENCY

// Sampling interval for the IMU (20 Hz)
static const unsigned long SAMPLE_INTERVAL_MS = 50;

SignalProcessor signalProc;

void printFFTSpectrum(const float* spectrum, uint16_t numBins) {
    Serial.println(F("--- FFT SPECTRUM START ---"));
    Serial.println(F("bin,frequency_Hz,magnitude"));

    const float freqRes = (float)SAMPLING_FREQUENCY / (float)FFT_SIZE;

    for (uint16_t i = 0; i < numBins; i++) {
        float freq = i * freqRes;
        Serial.print(i);
        Serial.print(',');
        Serial.print(freq, 3);
        Serial.print(',');
        Serial.println(spectrum[i], 3);
    }

    Serial.println(F("--- FFT SPECTRUM END ---"));
}

void setup() {
    Serial.begin(115200);
    while (!Serial) {
        ; // wait for USB serial (32u4)
    }

    Serial.println(F("IMU + SignalProcessor test starting..."));

    // Initialize IMU over I2C
    uint8_t status = IMU_I2C_init();
    if (status != IMU_OK) {
        Serial.print(F("IMU init failed, status = "));
        Serial.println(status);
        Serial.println(F("Check wiring / ADXL345. Halting."));
        while (1) {
            delay(1000);
        }
    }

    // Initialize our helper
    signalProc.begin();

    Serial.println(F("IMU initialized OK."));
    Serial.println(F("x,y,z,raw_mag,processed_mag"));
}

void loop() {
    static unsigned long lastSampleTime = 0;
    unsigned long now = millis();

    if (now - lastSampleTime < SAMPLE_INTERVAL_MS) {
        return;
    }
    lastSampleTime += SAMPLE_INTERVAL_MS;

    // 1) Read IMU sample
    int16_t x, y, z;
    IMU_I2C_readAccel(&x, &y, &z);

    // 2) Feed into SignalProcessor
    float rawMag = 0.0f;
    float procMag = 0.0f;
    bool fftReady = false;

    signalProc.update(x, y, z, rawMag, procMag, fftReady);

    // 3) Print per-sample info
    Serial.print(x);
    Serial.print(',');
    Serial.print(y);
    Serial.print(',');
    Serial.print(z);
    Serial.print(',');
    Serial.print(rawMag, 3);
    Serial.print(',');
    Serial.println(procMag, 3);

    // 4) If a new FFT spectrum is ready, print it
    if (fftReady) {
        float spectrum[(FFT_SIZE / 2) + 1];
        uint16_t numBins = 0;

        signalProc.getSpectrum(spectrum, numBins);
        printFFTSpectrum(spectrum, numBins);
    }
}
*/













//Code for UI 
/*
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
*/









#include <Adafruit_ILI9341.h>
#include "UIManager.h"
#include "UIDataModel.h"
#include "TouchInput.h"
#include "SignalUIBridge.h"

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

    // TFT
    tft.begin();
    tft.setRotation(1);
    Serial.println("[1] TFT OK");

    // Touch
    touch.begin();
    Serial.println("[2] Touch OK");

    // UI
    ui.begin();
    Serial.println("[3] UI Manager OK");

    // INTERNAL SIGNAL LOGIC
    SignalUIBridge::begin();

    Serial.println("======== SETUP COMPLETE ========");
}

void loop() {

    // update IMU + SignalProcessor internally
    SignalUIBridge::update();

    // TOUCH INPUT (unchanged)
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

    ui.loop();

    delay(50);
}

