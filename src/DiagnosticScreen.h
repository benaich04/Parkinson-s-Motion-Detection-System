#ifndef DIAGNOSTICSCREEN_H
#define DIAGNOSTICSCREEN_H

#include "UIScreen.h"
#include "UIDataModel.h"
#include <Adafruit_ILI9341.h>
#include "TFTHelpers.h"

class DiagnosticScreen : public UIScreen {
public:
    DiagnosticScreen(TFTHelpers::ILI9341_Display *tft, UIDataModel *data);

    void enter() override;
    void exit() override;
    void drawFull() override;
    void update() override;

private:
    TFTHelpers::ILI9341_Display *tft;
    UIDataModel *data;

    // Top plot: processed acceleration magnitude vs time
    TFTHelpers::PlotConfig accelPlot;

    // Bottom plot: FFT spectrum (frequency on x-axis, magnitude on y-axis)
    TFTHelpers::PlotConfig fftPlot;

    unsigned long lastUpdateMs;
};

#endif
