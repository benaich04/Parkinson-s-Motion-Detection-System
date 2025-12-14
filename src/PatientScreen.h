#ifndef PATIENTSCREEN_H
#define PATIENTSCREEN_H

#include "UIScreen.h"
#include "UIDataModel.h"
#include "TFTHelpers.h"

class PatientScreen : public UIScreen {
public:
    PatientScreen(TFTHelpers::ILI9341_Display* tft, UIDataModel* data);

    void enter() override;
    void exit() override;
    void drawFull() override;
    void update() override;

private:
    TFTHelpers::ILI9341_Display* tft;
    UIDataModel* data;

    // Layout
    int16_t cx, cy;
    int16_t baseR;
    int16_t textY;
    int16_t adviceY;

    // State stability
    int stableState;
    int candidateState;
    uint8_t streak;
    unsigned long lastSampleMs;

    // Advice
    uint8_t adviceIndex;
    unsigned long lastAdviceMs;

    // Animation
    int8_t ringOffset; // Used for animation offset from baseR
    int8_t ringDir;
    unsigned long lastAnimMs;

    // Helpers
    int readLiveState() const;
    uint16_t stateColor(int st) const;
    const char* stateText(int st) const;

    void drawStateText();
    void drawBaseCircle();     // Draw static filled center
    void drawRing(int offset, uint16_t color); // Draw animated outer ring
    void updateAdvice();
};

#endif