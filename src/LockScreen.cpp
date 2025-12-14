#include <Arduino.h>
#include "LockScreen.h"
#include "TFTHelpers.h"

// 1. CONSTRUCTOR FIX: Initialize the new previousHeartY member
LockScreen::LockScreen(TFTHelpers::ILI9341_Display* tft, UIDataModel* data)
: tft(tft),
  data(data),
  fadeValue(0),
  fadeDirection(true),
  heartBaseY(150),
  heartOffset(0),
  heartGoingUp(true),
  previousHeartY(150) // <--- INITIALIZE previousHeartY
{}

void LockScreen::enter() {
    Serial.println("[LOCKSCREEN] enter()");
    drawFull();
}

void LockScreen::exit() {
    // nothing yet
}

void LockScreen::drawFull() {
    TFTHelpers::clear(*tft, ILI9341_BLACK);

    // --------- CLOCK (centered) ----------
    char timeStr[6];
    snprintf(timeStr, sizeof(timeStr), "%02u:%02u", data->hour, data->minute);
TFTHelpers::drawCenteredText(*tft, 40, timeStr, 4, ILI9341_WHITE, -1);

// --------- DATE (under time) ---------
char dateStr[16];
snprintf(dateStr, sizeof(dateStr), "%02u/%02u/%04u",
         data->day, data->month, data->year);
TFTHelpers::drawCenteredText(*tft, 80, dateStr, 2, ILI9341_DARKGREY, -1);

// --------- SUBTITLE (pleasant) -------
TFTHelpers::drawCenteredText(*tft, 110, "PARKINSON MONITOR", 2, ILI9341_CYAN, -1);


    // --------- BATTERY (top-right) -------
    char battStr[8];
    snprintf(battStr, sizeof(battStr), "%u%%", data->batteryPercent);
    TFTHelpers::drawText(*tft, 250, 10, battStr, 2, ILI9341_YELLOW, -1);

    // First heart frame
    drawHeart();

    // First "Swipe up" frame (so it appears immediately)
    uint16_t fadeColor = TFTHelpers::color565(fadeValue, fadeValue, fadeValue);
    tft->setTextSize(2);
    tft->setCursor(60, 260);
    tft->setTextColor(fadeColor, ILI9341_BLACK);
    tft->print("Swipe up to start");
}

// 2. drawHeart FIX: Correctly clear the previous position
void LockScreen::drawHeart() {
    const int16_t textSize = 3;
    
    // Heart dimensions: "<3" is 2 characters wide, 7 pixels tall, scaled by 3.
    // Width calculation: (2 chars * 6 pixels/char * 3 size) - (1 * 3 size) = 33 pixels wide
    const int16_t char_full_width = (5 + 1) * textSize;
    const int16_t text_width = 2 * char_full_width - textSize; 
    const int16_t text_height = 7 * textSize; // 21 pixels high

    // Calculate X for clearing (centered position)
    // The X calculation must match the one inside drawCenteredText for accurate clearing
    int16_t x = (tft->width() - text_width) / 2;
    
    // --- 1. CLEAR THE PREVIOUS HEART POSITION ---
    // The y-coordinate is the *top* of the text, stored from the last frame.
    // This clears only the area the heart occupied in the last frame.
    tft->fillRect(x, previousHeartY, text_width, text_height, ILI9341_BLACK); 


    // --- 2. DRAW THE NEW HEART POSITION ---
    
    // Calculate new Heart Y position (bobbing)
    int16_t heartY = heartBaseY + heartOffset;

    const char *heartText = "<3";

    TFTHelpers::drawCenteredText(
        *tft,
        heartY,
        heartText,
        textSize,
        TFTHelpers::color565(255, 60, 120),  // soft pink/red
        -1 // Transparent background
    );
    
    // --- 3. SAVE THE NEW POSITION FOR THE NEXT CLEAR OPERATION ---
    previousHeartY = heartY;
}

void LockScreen::update() {
    // ---- Heart bobbing animation ----
    if (heartGoingUp) {
        if (heartOffset > -6) heartOffset--;
        else heartGoingUp = false;
    } else {
        if (heartOffset < 6) heartOffset++;
        else heartGoingUp = true;
    }
    drawHeart();

    // ---- Fading "Swipe up to start" ----
    uint16_t fadeColor = TFTHelpers::color565(fadeValue, fadeValue, fadeValue);

    tft->setTextSize(2);
    tft->setCursor(60, 260);
    tft->setTextColor(fadeColor, ILI9341_BLACK);
    tft->print("Swipe up to start");

    if (fadeDirection) {
        if (fadeValue < 200) fadeValue += 4;
        else fadeDirection = false;
    } else {
        if (fadeValue > 20) fadeValue -= 4;
        else fadeDirection = true;
    }
}