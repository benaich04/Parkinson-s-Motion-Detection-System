#include <Arduino.h>
#include "LockScreen.h"
#include "TFTHelpers.h"

LockScreen::LockScreen(Adafruit_ILI9341* tft, UIDataModel* data)
: tft(tft),
  data(data),
  fadeValue(0),
  fadeDirection(true),
  heartBaseY(150),
  heartOffset(0),
  heartGoingUp(true)
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
    TFTHelpers::drawCenteredText(*tft, 40, timeStr, 4, ILI9341_WHITE);

    // --------- DATE (under time) ---------
    char dateStr[16];
    snprintf(dateStr, sizeof(dateStr), "%02u/%02u/%04u",
             data->day, data->month, data->year);
    TFTHelpers::drawCenteredText(*tft, 80, dateStr, 2, ILI9341_DARKGREY);

    // --------- SUBTITLE (pleasant) -------
    TFTHelpers::drawCenteredText(*tft, 110, "Parkinson Monitor", 2, ILI9341_CYAN);

    // --------- BATTERY (top-right) -------
    char battStr[8];
    snprintf(battStr, sizeof(battStr), "%u%%", data->batteryPercent);
    TFTHelpers::drawText(*tft, 250, 10, battStr, 2, ILI9341_YELLOW, -1);

    // First heart frame
    drawHeart();

    // First "Swipe up" frame (so it appears immediately)
    uint16_t fadeColor = tft->color565(fadeValue, fadeValue, fadeValue);
    tft->setTextSize(2);
    tft->setCursor(60, 260);
    tft->setTextColor(fadeColor, ILI9341_BLACK);
    tft->print("Swipe up to start");
}

void LockScreen::drawHeart() {
    // Clear only a small area around the heart
    int16_t hw = 60;  // area width
    int16_t hh = 60;  // area height
    int16_t cx = tft->width() / 2;
    int16_t x  = cx - hw / 2;
    int16_t y  = heartBaseY - hh / 2 - 10;

    tft->fillRect(x, y, hw, hh, ILI9341_BLACK);

    // Heart Y position (bobbing)
    int16_t heartY = heartBaseY + heartOffset;

    // Use "<3" for reliable rendering
    const char *heartText = "<3";

    TFTHelpers::drawCenteredText(
        *tft,
        heartY,
        heartText,
        3,
        tft->color565(255, 60, 120),  // soft pink/red
        -1
    );
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
    uint16_t fadeColor = tft->color565(fadeValue, fadeValue, fadeValue);

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
