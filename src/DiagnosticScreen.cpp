


#include <Arduino.h>
#include "DiagnosticScreen.h"
#include "TFTHelpers.h"
#include "SignalUIBridge.h"

// ------------------------------------------
//  OPTIONAL HEADER BAR
//  Comment this out to remove it
// ------------------------------------------
#define SHOW_HEADER_BAR  1


DiagnosticScreen::DiagnosticScreen(TFTHelpers::ILI9341_Display *tft, UIDataModel *data)
: tft(tft),
  data(data),
  lastUpdateMs(0)
{
    memset(&accelPlot, 0, sizeof(accelPlot));
    memset(&fftPlot,   0, sizeof(fftPlot));
}

void DiagnosticScreen::enter() {
    drawFull();
}

void DiagnosticScreen::exit() {
    // nothing needed
}

void DiagnosticScreen::drawFull() {

    TFTHelpers::clear(*tft, ILI9341_BLACK);

#if SHOW_HEADER_BAR
    TFTHelpers::drawHeaderBar(*tft,
                              "DIAGNOSTICS",
                              ILI9341_DARKGREY,
                              ILI9341_WHITE);
    int headerOffset = 30;
#else
    int headerOffset = 0;
#endif

    // =====================================
    // PROCESSED MAG PLOT
    // =====================================
    accelPlot.x = 10;
    accelPlot.y = headerOffset + 10;
    accelPlot.w = tft->width() - 20;
    accelPlot.h = 70;

    accelPlot.valMin = 0.0f;
    accelPlot.valMax = 200.0f;

    accelPlot.bgColor     = ILI9341_BLACK;
    accelPlot.borderColor = ILI9341_DARKGREY;
    accelPlot.gridColor   = ILI9341_DARKGREY;

    TFTHelpers::initPlot(*tft, accelPlot, 20, 10);

    TFTHelpers::drawText(*tft,
        accelPlot.x + 2,
        accelPlot.y - 12,
        "PROCESSED MAGNITUDE",
        1,
        ILI9341_CYAN,
        -1);

    TFTHelpers::drawText(*tft,
        accelPlot.x - 5,
        accelPlot.y + accelPlot.h/2,
        "MAG",
        1,
        ILI9341_LIGHTGREY,
        -1);

    TFTHelpers::drawText(*tft,
        accelPlot.x,
        accelPlot.y + accelPlot.h + 2,
        "0S", 1, ILI9341_LIGHTGREY, -1);

    TFTHelpers::drawText(*tft,
        accelPlot.x + accelPlot.w/3,
        accelPlot.y + accelPlot.h + 2,
        "30S", 1, ILI9341_LIGHTGREY, -1);

    TFTHelpers::drawText(*tft,
        accelPlot.x + (2 * accelPlot.w)/3,
        accelPlot.y + accelPlot.h + 2,
        "60S", 1, ILI9341_LIGHTGREY, -1);

    TFTHelpers::drawText(*tft,
        accelPlot.x + accelPlot.w - 18,
        accelPlot.y + accelPlot.h + 2,
        "90S", 1, ILI9341_LIGHTGREY, -1);


    // =====================================
    // FFT SPECTRUM PLOT — NOW 0–10 Hz RANGE
    // =====================================
    fftPlot.x = 10;
    fftPlot.y = accelPlot.y + accelPlot.h + 35;
    fftPlot.w = tft->width() - 20;
    fftPlot.h = 70;

    fftPlot.valMin = 0.0f;
    fftPlot.valMax = 200.0f;

    fftPlot.bgColor     = ILI9341_BLACK;
    fftPlot.borderColor = ILI9341_DARKGREY;
    fftPlot.gridColor   = ILI9341_DARKGREY;

    TFTHelpers::initPlot(*tft, fftPlot, 20, 10);

    TFTHelpers::drawText(*tft,
        fftPlot.x + 2,
        fftPlot.y - 12,
        "FFT SPECTRUM (0–10 Hz)",
        1,
        ILI9341_CYAN,
        -1);

    TFTHelpers::drawText(*tft,
        fftPlot.x - 5,
        fftPlot.y + fftPlot.h/2,
        "AMP",
        1,
        ILI9341_LIGHTGREY,
        -1);


    // ---- UPDATED FREQUENCY LABELS (0–10 Hz)
    TFTHelpers::drawText(*tft,
        fftPlot.x,
        fftPlot.y + fftPlot.h + 2,
        "0HZ", 1, ILI9341_LIGHTGREY, -1);

    TFTHelpers::drawText(*tft,
        fftPlot.x + fftPlot.w/2 - 8,
        fftPlot.y + fftPlot.h + 2,
        "5HZ", 1, ILI9341_LIGHTGREY, -1);

    TFTHelpers::drawText(*tft,
        fftPlot.x + fftPlot.w - 25,
        fftPlot.y + fftPlot.h + 2,
        "10HZ", 1, ILI9341_LIGHTGREY, -1);


    lastUpdateMs = millis();
}

/*      !!!!! AUTO SCALING FOR MAGNITUDE PLOT !!!!!  
void DiagnosticScreen::update() {

    if (millis() - lastUpdateMs < 40)
        return;

    lastUpdateMs = millis();

    // ==========================================
    // PROCESS MAGNITUDE (with dynamic Y-scaling)
    // ==========================================
    float mag = SignalUIBridge::getProcessedMagnitude();

    // --- Auto-scale Y-axis for magnitude plot ---
    static float dynMaxMag = 50.0f;          // starting range
    if (mag > dynMaxMag * 0.90f) {           // if approaching 90% of max
        dynMaxMag = mag * 1.20f;             // expand range w/ headroom
        accelPlot.valMax = dynMaxMag;

        // Reinitialize plot area with new scaling
        TFTHelpers::initPlot(*tft, accelPlot, 20, 10);

        // Redraw label (erased by initPlot)
        TFTHelpers::drawText(*tft,
                             accelPlot.x + 2,
                             accelPlot.y - 12,
                             "Processed Magnitude",
                             1,
                             ILI9341_CYAN,
                             -1);
    }

    // Plot magnitude sample smoothly
    TFTHelpers::plotNextSample(*tft, accelPlot, mag, ILI9341_GREEN);


    // ==========================================
    // FFT SPECTRUM (0–10 Hz bar graph)
    // ==========================================
    if (SignalUIBridge::hasNewFFT()) {

        static const uint16_t MAX_FFT_BINS = 64;
        float spectrum[MAX_FFT_BINS];
        uint16_t numBins = 0;

        bool ok = SignalUIBridge::getFFTSpectrum(spectrum, MAX_FFT_BINS, numBins);
        if (ok && numBins > 0) {

            // Draw bars over 0–10 Hz
            TFTHelpers::drawSpectrumBars(*tft,
                                         fftPlot,
                                         spectrum,
                                         numBins,
                                         10,       // max freq = 10 Hz
                                         0.30f);   // y-axis scale factor
        }
    }
}*/

void DiagnosticScreen::update() {

    if (millis() - lastUpdateMs < 40)
        return;

    lastUpdateMs = millis();

    // ==========================================
    // PROCESS MAGNITUDE (fixed Y-range)
    // ==========================================
    float mag = SignalUIBridge::getProcessedMagnitude();

    // Just plot — fixed accelPlot.valMax from drawFull()
    TFTHelpers::plotNextSample(*tft, accelPlot, mag, ILI9341_GREEN);


    // ==========================================
    // FFT SPECTRUM (0–10 Hz bar graph)
    // ==========================================
    if (SignalUIBridge::hasNewFFT()) {

        static const uint16_t MAX_FFT_BINS = 64;
        float spectrum[MAX_FFT_BINS];
        uint16_t numBins = 0;

        bool ok = SignalUIBridge::getFFTSpectrum(spectrum, MAX_FFT_BINS, numBins);
        if (ok && numBins > 0) {

            // Fixed max freq = 10 Hz, fixed y compression factor = 0.3
            TFTHelpers::drawSpectrumBars(*tft,
                                         fftPlot,
                                         spectrum,
                                         numBins,
                                         10,        // 0–10 Hz
                                         0.3f);     // fixed y scaling
        }
    }
}

