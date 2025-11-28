#include <Arduino.h>
#include "DiagnosticScreen.h"
#include "TFTHelpers.h"

DiagnosticScreen::DiagnosticScreen(Adafruit_ILI9341 *tft, UIDataModel *data)
: tft(tft),
  data(data),
  lastUpdateMs(0)
{
    // zero the configs
    memset(&accelPlot, 0, sizeof(accelPlot));
    memset(&fftPlot,   0, sizeof(fftPlot));
}

void DiagnosticScreen::enter() {
    drawFull();
}

void DiagnosticScreen::exit() {
    // nothing yet
}

void DiagnosticScreen::drawFull() {
    TFTHelpers::clear(*tft, ILI9341_BLACK);

    // Top header bar
    TFTHelpers::drawHeaderBar(*tft, "Diagnostics", ILI9341_DARKGREY, ILI9341_WHITE);

    // Accel plot (top half)
    accelPlot.x = 10;
    accelPlot.y = 40;
    accelPlot.w = tft->width() - 20;
    accelPlot.h = 80;
    accelPlot.valMin = -2.0f;
    accelPlot.valMax =  2.0f;
    accelPlot.bgColor = ILI9341_BLACK;
    accelPlot.borderColor = ILI9341_DARKGREY;
    accelPlot.gridColor = ILI9341_DARKGREY;

    TFTHelpers::initPlot(*tft, accelPlot, 20, 10);
    TFTHelpers::drawText(*tft, accelPlot.x + 2, accelPlot.y - 12,
                         "Acceleration (demo)", 1, ILI9341_CYAN, -1);

    // FFT plot (bottom half)
    fftPlot.x = 10;
    fftPlot.y = 140;
    fftPlot.w = tft->width() - 20;
    fftPlot.h = 80;
    fftPlot.valMin = 0.0f;
    fftPlot.valMax = 1.0f;
    fftPlot.bgColor = ILI9341_BLACK;
    fftPlot.borderColor = ILI9341_DARKGREY;
    fftPlot.gridColor = ILI9341_DARKGREY;

    TFTHelpers::initPlot(*tft, fftPlot, 20, 10);
    TFTHelpers::drawText(*tft, fftPlot.x + 2, fftPlot.y - 12,
                         "Spectrum (demo)", 1, ILI9341_CYAN, -1);

    lastUpdateMs = millis();
}

void DiagnosticScreen::update() {
    // update ~every 40 ms
    if (millis() - lastUpdateMs < 40) return;
    lastUpdateMs = millis();

    // fake accel value: random between -1.5 and +1.5
    float accelVal = (random(-150, 150)) / 100.0f;
    TFTHelpers::plotNextSample(*tft, accelPlot, accelVal, ILI9341_GREEN);

    // fake FFT magnitude: random between 0 and 1
    float fftVal = (random(0, 100)) / 100.0f;
    TFTHelpers::plotNextSample(*tft, fftPlot, fftVal, ILI9341_YELLOW);
}
