#include <Arduino.h>
#include "PatientScreen.h"

// ---------------- States ----------------
#define ST_NORMAL 0
#define ST_TREMOR 1
#define ST_DYSK   2

// ---------------- Timing ----------------
#define SAMPLE_MS   200
#define ADVICE_MS  6000
#define ANIM_MS      40    // Faster animation refresh (40ms = 25fps)

// ---------------- Colors ----------------
#define BG_COLOR   TFTHelpers::color565(18,18,18)
#define BLUE_SOFT  TFTHelpers::color565(80,170,255)

// ---------------- Advice ----------------
static const char* adviceNormal[] = {
    "Movement is normal",
    "Keep steady breathing"
};

static const char* adviceTremor[] = {
    "Focus on the circle",
    "Slow your breathing"
};

static const char* adviceDysk[] = {
    "Focus on the circle",
    "Reduce movement"
};

// ---------------- Constructor ----------------
PatientScreen::PatientScreen(TFTHelpers::ILI9341_Display* tft, UIDataModel* data)
: tft(tft),
  data(data),
  stableState(-1),
  candidateState(-1),
  streak(0),
  lastSampleMs(0),
  adviceIndex(0),
  lastAdviceMs(0),
  ringOffset(0),
  ringDir(1),
  lastAnimMs(0)
{}

// ---------------- UIScreen ----------------
void PatientScreen::enter() {
    drawFull();
}

void PatientScreen::exit() {
    // required override
}

void PatientScreen::drawFull() {
    TFTHelpers::clear(*tft, ILI9341_BLACK);

    TFTHelpers::drawHeaderBar(
        *tft,
        "PATIENT",
        TFTHelpers::color565(30,30,30),
        ILI9341_WHITE
    );

    cx = tft->width() / 2;
    
    textY   = 40; 
    
    // Circle centered below text
    cy      = textY + 85; 
    baseR   = 40; 
    
    adviceY = tft->height() - 46;

    // Advice box
    tft->fillRect(10, adviceY, tft->width() - 20, 36, BG_COLOR);
    tft->drawRect(10, adviceY, tft->width() - 20, 36, ILI9341_DARKGREY);

    stableState = -1;
    candidateState = -1;
    streak = 0;
    adviceIndex = 0;

    // Force immediate stable draw
    stableState = readLiveState();
    
    // Reset animation state
    if (stableState == ST_NORMAL) {
        ringOffset = 0;
        ringDir = 1;
    } else {
        ringOffset = 15; // Start shrunk out
        ringDir = -1;    // Shrink in
    }

    drawStateText();
    drawBaseCircle();
    updateAdvice();
}

void PatientScreen::update() {
    unsigned long now = millis();

    // ---- Stability filter (5 in a row) ----
    if (now - lastSampleMs >= SAMPLE_MS) {
        lastSampleMs = now;

        int live = readLiveState();
        if (live != candidateState) {
            candidateState = live;
            streak = 1;
        } else if (streak < 5) {
            streak++;
        }

        if (streak >= 5 && stableState != candidateState) {
            stableState = candidateState;
            
            // Reset Animation logic on state change
            if (stableState == ST_NORMAL) {
                ringOffset = 0;
                ringDir = 1;
            } else {
                ringOffset = 15; // Start wide
                ringDir = -1;    // Shrink inward
            }

            drawStateText();
            drawBaseCircle();
            updateAdvice();
        }
    }

    // ---- Animation Logic ----
    if (now - lastAnimMs >= ANIM_MS) {
        lastAnimMs = now;

        // 1. Erase previous ring
        // Note: ringOffset is relative to baseR
        drawRing(ringOffset, ILI9341_BLACK);

        // 2. Update Physics
        if (stableState == ST_NORMAL) {
            // BREATHING: Expands out and back, never goes inside baseR
            // Range: 0 to 8
            ringOffset += ringDir;
            if (ringOffset > 8) {
                ringOffset = 8;
                ringDir = -1;
            } else if (ringOffset < 0) {
                ringOffset = 0;
                ringDir = 1;
            }
        } else {
            // SHRINKING: Starts wide (15), shrinks fast to 0, resets
            // Simulates "Focusing"
            ringOffset -= 10; // Fast shrink speed
            if (ringOffset < 0) {
                ringOffset = 15; // Reset to wide
            }
        }

        // 3. Draw new ring
        drawRing(ringOffset, stateColor(stableState));
    }

    // ---- Advice ----
    if (now - lastAdviceMs >= ADVICE_MS) {
        lastAdviceMs = now;
        adviceIndex++;
        updateAdvice();
    }
}

// ---------------- Helpers ----------------
int PatientScreen::readLiveState() const {
    if (data->state == UIDataModel::TREMOR)     return ST_TREMOR;
    if (data->state == UIDataModel::DYSKINESIA) return ST_DYSK;
    return ST_NORMAL;
}

uint16_t PatientScreen::stateColor(int st) const {
    if (st == ST_TREMOR) return ILI9341_RED;
    if (st == ST_DYSK)   return ILI9341_ORANGE;
    return BLUE_SOFT;
}

const char* PatientScreen::stateText(int st) const {
    if (st == ST_TREMOR) return "TREMOR DETECTED";
    if (st == ST_DYSK)   return "DYSKINESIA";
    return "NORMAL STATE";
}

// ---------------- Drawing ----------------
void PatientScreen::drawStateText() {
    tft->fillRect(0, textY - 10, tft->width(), 30, ILI9341_BLACK);

    TFTHelpers::drawCenteredText(
        *tft,
        textY,
        stateText(stableState),
        2,
        stateColor(stableState),
        ILI9341_BLACK
    );
}

void PatientScreen::drawBaseCircle() {
    // Clear the center area first
    tft->fillRect(cx - (baseR + 20), cy - (baseR + 20), (baseR + 20)*2, (baseR + 20)*2, ILI9341_BLACK);

    // Draw FULL solid circle at base size
    // Using fillRoundRect to simulate circle
    tft->fillRoundRect(
        cx - baseR,
        cy - baseR,
        baseR * 2,
        baseR * 2,
        baseR, // radius = width/2 makes it a circle
        stateColor(stableState)
    );
}

void PatientScreen::drawRing(int offset, uint16_t color) {
    // Determine radius of the outer ring
    int r = baseR + offset;

    // Optimization: Don't draw if it overlaps the solid base circle too much
    if (offset <= 0) return; 

    // Draw outline ring
    tft->drawRoundRect(
        cx - r,
        cy - r,
        r * 2,
        r * 2,
        r,
        color
    );
    // Double thickness for visibility
    tft->drawRoundRect(
        cx - r + 1,
        cy - r + 1,
        (r - 1) * 2,
        (r - 1) * 2,
        r - 1,
        color
    );
}

void PatientScreen::updateAdvice() {
    tft->fillRect(12, adviceY + 4, tft->width() - 24, 28, BG_COLOR);

    const char* msg;
    if (stableState == ST_TREMOR)
        msg = adviceTremor[adviceIndex % 2];
    else if (stableState == ST_DYSK)
        msg = adviceDysk[adviceIndex % 2];
    else
        msg = adviceNormal[adviceIndex % 2];

    TFTHelpers::drawCenteredText(
        *tft,
        adviceY + 10,
        msg,
        2,
        ILI9341_WHITE,
        BG_COLOR
    );
}