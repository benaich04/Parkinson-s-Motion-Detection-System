#ifndef HISTORYSCREEN_H
#define HISTORYSCREEN_H
#include "UIScreen.h"
#include "UIDataModel.h"
#include "TFTHelpers.h"

class HistoryScreen : public UIScreen {
public:
    HistoryScreen(TFTHelpers::ILI9341_Display* tft, UIDataModel* data);
    void enter() override;
    void exit() override;
    void drawFull() override;
    void update() override;
    void captureEvent();  // PUBLIC - called from main loop

private:
    TFTHelpers::ILI9341_Display* tft;
    UIDataModel* data;
    
    struct Evt {
        uint8_t t:1;  // type: 0=tremor, 1=dysk (1 bit)
        uint8_t m:7;  // magnitude 0-100 (7 bits)
        uint8_t h:5;  // hour 0-23 (5 bits)
        uint8_t n:6;  // minute 0-59 (6 bits)
    };
    
    Evt e[5];  // Only 5 events, 3 bytes each = 15 bytes total
    uint8_t cnt;
    uint8_t idx;
    uint8_t dCnt;
    uint8_t tCnt;  
};
#endif