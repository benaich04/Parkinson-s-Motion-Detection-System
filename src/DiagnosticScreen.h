#ifndef DIAGNOSTICSCREEN_H
#define DIAGNOSTICSCREEN_H

#include "UIScreen.h"
#include "UIDataModel.h"
#include <Adafruit_ILI9341.h>
#include "TFTHelpers.h"

class DiagnosticScreen : public UIScreen {
public:
    DiagnosticScreen(Adafruit_ILI9341 *tft, UIDataModel *data);

    void enter() override;
    void exit() override;
    void drawFull() override;
    void update() override;

private:
    Adafruit_ILI9341 *tft;
    UIDataModel *data;

    TFTHelpers::PlotConfig accelPlot;
    TFTHelpers::PlotConfig fftPlot;

    unsigned long lastUpdateMs;
};

#endif
