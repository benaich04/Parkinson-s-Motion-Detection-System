#include <Arduino.h>
#include "HistoryScreen.h"

HistoryScreen::HistoryScreen(TFTHelpers::ILI9341_Display* tft, UIDataModel* data)
: tft(tft), data(data), cnt(0), idx(0), dCnt(0), tCnt(0)
{}

void HistoryScreen::enter() {
    drawFull();
}

void HistoryScreen::exit() {}

void HistoryScreen::drawFull() {
    TFTHelpers::clear(*tft, ILI9341_BLACK);
    TFTHelpers::drawHeaderBar(*tft, "HISTORY", ILI9341_MAROON, ILI9341_WHITE);
    
    // Stats bar - compact
    char b[4];
    
    // Total
    tft->fillRect(5, 32, 100, 35, ILI9341_NAVY);
    tft->drawRect(5, 32, 100, 35, ILI9341_CYAN);
    TFTHelpers::drawText(*tft, 10, 36, "TOT", 1, ILI9341_LIGHTGREY);
    itoa(tCnt + dCnt, b, 10);  
    TFTHelpers::drawText(*tft, 45, 48, b, 2, ILI9341_WHITE);
    
    // Tremors
    tft->fillRect(110, 32, 100, 35, ILI9341_NAVY);
    tft->drawRect(110, 32, 100, 35, ILI9341_ORANGE);
    TFTHelpers::drawText(*tft, 115, 36, "TRM", 1, ILI9341_LIGHTGREY);
    itoa(tCnt, b, 10); 
    TFTHelpers::drawText(*tft, 150, 48, b, 2, ILI9341_ORANGE);
    
    // Dysk
    tft->fillRect(215, 32, 100, 35, ILI9341_NAVY);
    tft->drawRect(215, 32, 100, 35, ILI9341_MAGENTA);
    TFTHelpers::drawText(*tft, 220, 36, "DSK", 1, ILI9341_LIGHTGREY);
    itoa(dCnt, b, 10);
    TFTHelpers::drawText(*tft, 255, 48, b, 2, ILI9341_MAGENTA);
    
    // Events
    if (cnt == 0) {
        TFTHelpers::drawCenteredText(*tft, 120, "No events", 2, ILI9341_DARKGREY);
        return;
    }
    
    uint8_t vis = cnt < 4 ? cnt : 4;
    for (uint8_t i = 0; i < vis; i++) {
        int8_t ei = idx - 1 - i;
        if (ei < 0) ei += 5;
        
        int16_t y = 75 + (i * 35);
        uint16_t col = e[ei].t ? ILI9341_MAGENTA : ILI9341_ORANGE;
        
        // Accent bar
        tft->fillRect(8, y, 3, 28, col);
        tft->fillRect(11, y, 300, 28, ILI9341_NAVY);
        tft->drawRect(11, y, 300, 28, ILI9341_DARKGREY);
        
        // Type
        char ic = e[ei].t ? 'D' : 'T';
        char s[2] = {ic, 0};
        TFTHelpers::drawText(*tft, 18, y + 8, s, 2, col);
        
        // Time
        char tm[6];
        sprintf(tm, "%02d:%02d", e[ei].h, e[ei].n);
        TFTHelpers::drawText(*tft, 40, y + 10, tm, 1, ILI9341_CYAN);
        
        // Gauge
        uint16_t gc;
        if (e[ei].m < 25) gc = ILI9341_GREEN;
        else if (e[ei].m < 50) gc = ILI9341_YELLOW;
        else if (e[ei].m < 75) gc = ILI9341_ORANGE;
        else gc = ILI9341_RED;
        
        int16_t gw = (e[ei].m * 150) / 100;
        tft->fillRect(110, y + 8, gw, 12, gc);
        tft->drawRect(110, y + 8, 150, 12, ILI9341_DARKGREY);
        
        // %
        char pc[5];
        sprintf(pc, "%d%%", e[ei].m);
        TFTHelpers::drawText(*tft, 270, y + 10, pc, 1, ILI9341_WHITE);
    }
}

void HistoryScreen::update() {
}

void HistoryScreen::captureEvent() {
    // Capture current event from data model
    e[idx].t = (data->state == UIDataModel::DYSKINESIA) ? 1 : 0;
    e[idx].m = (uint8_t)(data->intensity * 100.0f);
    if (e[idx].m > 100) e[idx].m = 100;
    e[idx].h = data->hour;
    e[idx].n = data->minute;
    
    if (e[idx].t) dCnt++;
    else tCnt++;
    
    idx = (idx + 1) % 5;
    if (cnt < 5) cnt++;
}