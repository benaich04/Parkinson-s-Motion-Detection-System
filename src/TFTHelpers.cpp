
#include "TFTHelpers.h"
#include <string.h> // for strlen()
#include <math.h>
#include "FFTModule.h"

namespace {

  // small internal helper to estimate text width with default GFX font
  int16_t textWidthPixels(const char *text, uint8_t textSize) {
    if (!text) return 0;
    // Default GFX font: 5px wide + 1px spacing = 6px per char
    int16_t len = (int16_t)strlen(text);
    return len * 6 * textSize;
  }

  // Map value in [vmin, vmax] to y pixel in plot area
  int16_t mapValueToY(const TFTHelpers::PlotConfig &cfg, float v) {
    // clamp
    if (v < cfg.valMin) v = cfg.valMin;
    if (v > cfg.valMax) v = cfg.valMax;

    float norm = (v - cfg.valMin) / (cfg.valMax - cfg.valMin); // 0..1
    // invert vertical (0 = bottom, 1 = top)
    int16_t y = cfg.y + cfg.h - (int16_t)(norm * cfg.h);
    return y;
  }

} // anonymous namespace

namespace TFTHelpers {

  void initScreen(Adafruit_ILI9341 &tft,
                  uint8_t rotation,
                  uint16_t bgColor) {
    tft.begin();
    tft.setRotation(rotation);
    tft.fillScreen(bgColor);
  }

  void clear(Adafruit_ILI9341 &tft,
             uint16_t color) {
    tft.fillScreen(color);
  }

  void drawText(Adafruit_ILI9341 &tft,
                int16_t x, int16_t y,
                const char *text,
                uint8_t textSize,
                uint16_t textColor,
                int16_t bgColor) {
    tft.setCursor(x, y);
    tft.setTextSize(textSize);
    if (bgColor < 0) {
      tft.setTextColor(textColor);
    } else {
      tft.setTextColor(textColor, (uint16_t)bgColor);
    }
    tft.print(text);
  }

  void drawCenteredText(Adafruit_ILI9341 &tft,
                        int16_t y,
                        const char *text,
                        uint8_t textSize,
                        uint16_t textColor,
                        int16_t bgColor) {
    int16_t w = textWidthPixels(text, textSize);
    int16_t x = (tft.width() - w) / 2;
    if (x < 0) x = 0;
    drawText(tft, x, y, text, textSize, textColor, bgColor);
  }

  void drawBox(Adafruit_ILI9341 &tft,
               int16_t x, int16_t y,
               int16_t w, int16_t h,
               uint16_t fillColor,
               uint16_t borderColor) {
    tft.fillRect(x, y, w, h, fillColor);
    tft.drawRect(x, y, w, h, borderColor);
  }

  void drawRoundedBox(Adafruit_ILI9341 &tft,
                      int16_t x, int16_t y,
                      int16_t w, int16_t h,
                      int16_t radius,
                      uint16_t fillColor,
                      uint16_t borderColor) {
    tft.fillRoundRect(x, y, w, h, radius, fillColor);
    tft.drawRoundRect(x, y, w, h, radius, borderColor);
  }

  void drawBoxWithText(Adafruit_ILI9341 &tft,
                       int16_t x, int16_t y,
                       int16_t w, int16_t h,
                       const char *label,
                       uint8_t textSize,
                       uint16_t fillColor,
                       uint16_t borderColor,
                       uint16_t textColor) {
    drawBox(tft, x, y, w, h, fillColor, borderColor);

    int16_t textW = textWidthPixels(label, textSize);
    int16_t textX = x + (w - textW) / 2;
    if (textX < x + 2) textX = x + 2;
    int16_t textY = y + (h - (8 * textSize)) / 2; // 7px tall + 1px
    if (textY < y + 2) textY = y + 2;

    drawText(tft, textX, textY, label, textSize, textColor, -1);
  }

  void drawRoundedBoxWithText(Adafruit_ILI9341 &tft,
                              int16_t x, int16_t y,
                              int16_t w, int16_t h,
                              int16_t radius,
                              const char *label,
                              uint8_t textSize,
                              uint16_t fillColor,
                              uint16_t borderColor,
                              uint16_t textColor) {
    drawRoundedBox(tft, x, y, w, h, radius, fillColor, borderColor);

    int16_t textW = textWidthPixels(label, textSize);
    int16_t textX = x + (w - textW) / 2;
    if (textX < x + 2) textX = x + 2;
    int16_t textY = y + (h - (8 * textSize)) / 2;
    if (textY < y + 2) textY = y + 2;

    drawText(tft, textX, textY, label, textSize, textColor, -1);
  }

  void drawButton(Adafruit_ILI9341 &tft,
                  int16_t x, int16_t y,
                  int16_t w, int16_t h,
                  const char *label,
                  bool pressed,
                  uint8_t textSize) {
    uint16_t fill = pressed ? ILI9341_DARKGREY : ILI9341_NAVY;
    uint16_t border = pressed ? ILI9341_WHITE : ILI9341_CYAN;
    uint16_t textColor = ILI9341_WHITE;

    drawRoundedBoxWithText(tft, x, y, w, h, 6,
                           label, textSize,
                           fill, border, textColor);
  }

  void drawHeaderBar(Adafruit_ILI9341 &tft,
                     const char *title,
                     uint16_t bgColor,
                     uint16_t textColor) {
    int16_t h = 20;
    tft.fillRect(0, 0, tft.width(), h, bgColor);
    drawCenteredText(tft, 4, title, 2, textColor, bgColor);
  }

  void drawFooterBar(Adafruit_ILI9341 &tft,
                     const char *text,
                     uint16_t bgColor,
                     uint16_t textColor) {
    int16_t h = 18;
    int16_t y = tft.height() - h;
    tft.fillRect(0, y, tft.width(), h, bgColor);
    drawCenteredText(tft, y + 2, text, 1, textColor, bgColor);
  }

  void drawHorizontalGauge(Adafruit_ILI9341 &tft,
                           int16_t x, int16_t y,
                           int16_t w, int16_t h,
                           float value01,
                           uint16_t fillColor,
                           uint16_t bgColor,
                           uint16_t borderColor) {
    if (value01 < 0.0f) value01 = 0.0f;
    if (value01 > 1.0f) value01 = 1.0f;
    int16_t fillW = (int16_t)(w * value01);

    // background
    tft.fillRect(x, y, w, h, bgColor);
    // filled portion
    tft.fillRect(x, y, fillW, h, fillColor);
    // border
    tft.drawRect(x, y, w, h, borderColor);
  }

  void drawGrid(Adafruit_ILI9341 &tft,
                int16_t x, int16_t y,
                int16_t w, int16_t h,
                int16_t xStep, int16_t yStep,
                uint16_t lineColor) {
    // vertical lines
    for (int16_t xx = x; xx <= x + w; xx += xStep) {
      tft.drawLine(xx, y, xx, y + h, lineColor);
    }
    // horizontal lines
    for (int16_t yy = y; yy <= y + h; yy += yStep) {
      tft.drawLine(x, yy, x + w, yy, lineColor);
    }
  }

  void drawCrosshair(Adafruit_ILI9341 &tft,
                     int16_t x, int16_t y,
                     int16_t size,
                     uint16_t color) {
    tft.drawLine(x - size, y, x + size, y, color);
    tft.drawLine(x, y - size, x, y + size, color);
  }

  void drawTimeBox(Adafruit_ILI9341 &tft,
                   int16_t x, int16_t y,
                   int16_t w, int16_t h) {
    drawRoundedBox(tft, x, y, w, h, 4,
                   ILI9341_BLACK, ILI9341_WHITE);
    // You will later call drawText() to print "HH:MM" / date inside
  }


  void initPlot(Adafruit_ILI9341 &tft,
                const PlotConfig &cfg,
                int16_t xStep,
                int16_t yStep) {
    // Clear background of plot area
    tft.fillRect(cfg.x, cfg.y, cfg.w, cfg.h, cfg.bgColor);

    // Border
    tft.drawRect(cfg.x, cfg.y, cfg.w, cfg.h, cfg.borderColor);

    // Grid (optional; if step <= 0, skip)
    if (xStep > 0) {
      for (int16_t xx = cfg.x + xStep; xx < cfg.x + cfg.w; xx += xStep) {
        tft.drawLine(xx, cfg.y, xx, cfg.y + cfg.h, cfg.gridColor);
      }
    }
    if (yStep > 0) {
      for (int16_t yy = cfg.y + yStep; yy < cfg.y + cfg.h; yy += yStep) {
        tft.drawLine(cfg.x, yy, cfg.x + cfg.w, yy, cfg.gridColor);
      }
    }

    // init state for smooth plotting
    // start at left edge, no previous point yet
    // (curX is 0..w-1, relative to cfg.x)
    const_cast<PlotConfig&>(cfg).curX = 0;
    const_cast<PlotConfig&>(cfg).hasPrev = false;
    const_cast<PlotConfig&>(cfg).prevY = 0;
  }

  void plotBuffer(Adafruit_ILI9341 &tft,
                  const PlotConfig &cfg,
                  const float *buffer,
                  size_t N,
                  uint16_t lineColor) {
    if (!buffer || N < 2) return;

    // Clear interior (keep border + grid)
    tft.fillRect(cfg.x + 1,
                 cfg.y + 1,
                 cfg.w - 2,
                 cfg.h - 2,
                 cfg.bgColor);

    // Re-draw grid lightly if you like (optional):
    // (comment out if too slow)
    // for (int16_t xx = cfg.x + 10; xx < cfg.x + cfg.w; xx += 10) {
    //   tft.drawLine(xx, cfg.y + 1, xx, cfg.y + cfg.h - 2, cfg.gridColor);
    // }
    // for (int16_t yy = cfg.y + 10; yy < cfg.y + cfg.h; yy += 10) {
    //   tft.drawLine(cfg.x + 1, yy, cfg.x + cfg.w - 2, yy, cfg.gridColor);
    // }

    // X step in pixels (spread N samples across width)
    float stepX;
    if (N <= 1) {
      stepX = 0;
    } else {
      stepX = (float)(cfg.w - 1) / (float)(N - 1);
    }

    int16_t prevX = cfg.x;
    int16_t prevY = mapValueToY(cfg, buffer[0]);

    for (size_t i = 1; i < N; ++i) {
      int16_t x = cfg.x + (int16_t)(stepX * i);
      int16_t y = mapValueToY(cfg, buffer[i]);

      tft.drawLine(prevX, prevY, x, y, lineColor);

      prevX = x;
      prevY = y;
    }

    // Re-draw border (in case we overdraw at edges)
    tft.drawRect(cfg.x, cfg.y, cfg.w, cfg.h, cfg.borderColor);
  }

  void plotNextSample(Adafruit_ILI9341 &tft,
                    PlotConfig &cfg,
                    float newValue,
                    uint16_t lineColor) {
  // Map new sample
  int16_t y = mapValueToY(cfg, newValue);

  // Convert curX (0..w-1) to screen X coordinate
  int16_t x = cfg.x + cfg.curX;

  // If we are at the start of a sweep, clear the interior once
  if (cfg.curX == 0) {
    tft.fillRect(cfg.x + 1,
                 cfg.y + 1,
                 cfg.w - 2,
                 cfg.h - 2,
                 cfg.bgColor);
    cfg.hasPrev = false;
  }

  // Optional: erase a thin vertical strip before drawing (no flicker)
  tft.drawFastVLine(x, cfg.y + 1, cfg.h - 2, cfg.bgColor);

  // Draw line from previous point to current (if we have a previous)
  if (cfg.hasPrev) {
    int16_t prevX = x - 1;
    tft.drawLine(prevX, cfg.prevY, x, y, lineColor);
  }

  cfg.prevY = y;
  cfg.hasPrev = true;

  // Move to next x, wrap at end of plot area
  cfg.curX++;
  if (cfg.curX >= cfg.w) {
    cfg.curX = 0;
  }

  // Re-draw border in case edges got overwritten
  tft.drawRect(cfg.x, cfg.y, cfg.w, cfg.h, cfg.borderColor);
}

    void uiUpdateStatusBar(Adafruit_ILI9341 &tft,
                         bool isRunning) {
    const char *status = isRunning ? "Status: RUNNING" : "Status: PAUSED";
    // reuse drawFooterBar (we know footer is bottom 18px)
    drawFooterBar(tft, status, ILI9341_DARKGREY, ILI9341_WHITE);
  }


void drawSpectrumBars(Adafruit_ILI9341 &tft,
                      const PlotConfig &cfg,
                      const float *spectrum,
                      uint16_t numBins,
                      uint16_t maxFreqHz,
                      float yScale)     // <-- NEW PARAMETER
{
    if (!spectrum || numBins == 0 || maxFreqHz == 0) {
        return;
    }

    if (yScale <= 0.0f) yScale = 1.0f;  // safety clamp

    // Clear the plot area (keep border)
    tft.fillRect(cfg.x + 1,
                 cfg.y + 1,
                 cfg.w - 2,
                 cfg.h - 2,
                 cfg.bgColor);

    // FFT frequency resolution (Hz per bin)
    const float freqRes = (float)SAMPLING_FREQUENCY / (float)FFT_SIZE;

    // Number of 1 Hz bars
    uint16_t numBands = maxFreqHz;

    // Width of each 1 Hz bar
    int16_t barWidth = cfg.w / (int16_t)numBands;
    if (barWidth < 1) barWidth = 1;

    // Loop through frequency bands
    for (uint16_t f = 0; f < numBands; ++f)
    {
        float fLow  = (float)f;
        float fHigh = (float)(f + 1);

        // Convert frequency → FFT bins
        uint16_t binStart = (uint16_t)(fLow  / freqRes);
        uint16_t binEnd   = (uint16_t)(fHigh / freqRes);

        if (binStart >= numBins) continue;
        if (binEnd >= numBins)   binEnd = numBins - 1;
        if (binEnd < binStart)   binEnd = binStart;

        // Take MAX magnitude in this 1 Hz band
        float mag = 0.0f;
        for (uint16_t b = binStart; b <= binEnd; ++b) {
            if (spectrum[b] > mag) mag = spectrum[b];
        }

        // Apply vertical compression
        mag *= yScale;   // <-- THE IMPORTANT LINE

        // Convert magnitude → screen Y coordinate
        int16_t topY = mapValueToY(cfg, mag);
        int16_t bottomY = cfg.y + cfg.h - 1;

        if (topY < cfg.y + 1) topY = cfg.y + 1;
        if (bottomY < topY)   continue;

        // X coordinates for this bar
        int16_t x0 = cfg.x + (int16_t)f * barWidth;
        int16_t x1 = x0 + barWidth - 1;
        if (x1 > cfg.x + cfg.w - 2)
            x1 = cfg.x + cfg.w - 2;

        // Draw the bar
        tft.fillRect(x0,
                     topY,
                     x1 - x0 + 1,
                     bottomY - topY + 1,
                     ILI9341_YELLOW);
    }

    // Re-draw border
    tft.drawRect(cfg.x, cfg.y, cfg.w, cfg.h, cfg.borderColor);
}




  void uiUpdateTime(Adafruit_ILI9341 &tft,
                    const char *timeStr) {
    // Same geometry used in uiInitSignalScreen
    int16_t boxW = 70;
    int16_t boxH = 20;
    int16_t x = tft.width() - boxW;
    int16_t y = 0;

    // Redraw time box background
    drawTimeBox(tft, x, y, boxW, boxH);
    // Draw the new time string
    drawText(tft, x + 4, y + 4, timeStr, 1, ILI9341_GREEN, -1);
  }

  void uiInitSignalScreen(Adafruit_ILI9341 &tft,
                          PlotConfig &plotCfg,
                          float valMin,
                          float valMax,
                          bool isRunning,
                          const char *modeLabel,
                          const char *title,
                          const char *plotLabel) {
    // Clear full screen first (in case previous screen existed)
    clear(tft, ILI9341_BLACK);

    // --- Top bar with title ---
    drawHeaderBar(tft, title, ILI9341_NAVY, ILI9341_WHITE);

    // Time box in top-right (placeholder time for now)
    int16_t boxW = 70;
    int16_t boxH = 20;
    int16_t timeX = tft.width() - boxW;
    int16_t timeY = 0;
    drawTimeBox(tft, timeX, timeY, boxW, boxH);
    drawText(tft, timeX + 4, timeY + 4, "--:--", 1, ILI9341_GREEN, -1);

    // --- Buttons row under header ---
    int16_t btnY = 22;
    int16_t btnH = 26;

    // START button
    drawButton(tft,
               8, btnY,
               90, btnH,
               "START",
               isRunning,    // pressed when running
               2);

    // STOP button
    drawButton(tft,
               108, btnY,
               90, btnH,
               "STOP",
               !isRunning,   // pressed when stopped
               2);

    // MODE button
    drawButton(tft,
               208, btnY,
               90, btnH,
               modeLabel,
               false,
               1);

    // --- Plot area configuration ---
    int16_t top = btnY + btnH + 8;   // header (20) + buttons + margin
    int16_t bottomMargin = 20;       // room for footer

    plotCfg.x = 8;  // margin left
    plotCfg.y = top; // below buttons
    plotCfg.w = tft.width() - 16;    // margin left/right
    plotCfg.h = tft.height() - top - bottomMargin;

    plotCfg.valMin = valMin;
    plotCfg.valMax = valMax;

    plotCfg.bgColor     = ILI9341_BLACK;
    plotCfg.borderColor = ILI9341_WHITE;
    plotCfg.gridColor   = ILI9341_DARKGREY;

    // Initialize plot (grid + state)
    initPlot(tft, plotCfg, 10, 10);

    // Optional label inside the plot
    if (plotLabel) {
      drawText(tft,
               plotCfg.x + 4,
               plotCfg.y + 4,
               plotLabel,
               1,
               ILI9341_YELLOW,
               -1);
    }

    // --- Bottom status bar ---
    uiUpdateStatusBar(tft, isRunning);
  }


} // namespace TFTHelpers











/*

#include "TFTHelpers.h"
#include <string.h>
#include <math.h>

namespace {

// ---------- HELPERS ----------

// Measure width of RAM string
int16_t textWidthPixels(const char *text, uint8_t textSize) {
    if (!text) return 0;
    int16_t len = (int16_t)strlen(text);
    return len * 6 * textSize;
}

// Measure width of PROGMEM FlashString
int16_t textWidthPixels(const __FlashStringHelper *ftext, uint8_t textSize) {
    if (!ftext) return 0;

    char buf[40];
    strncpy_P(buf, (PGM_P)ftext, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';

    return strlen(buf) * 6 * textSize;
}

// Map data value → Y pixel
int16_t mapValueToY(const TFTHelpers::PlotConfig &cfg, float v) {
    if (v < cfg.valMin) v = cfg.valMin;
    if (v > cfg.valMax) v = cfg.valMax;

    float norm = (v - cfg.valMin) / (cfg.valMax - cfg.valMin);
    return cfg.y + cfg.h - (int16_t)(norm * cfg.h);
}

} // namespace



// ============================================================================
//                              PUBLIC FUNCTIONS
// ============================================================================

namespace TFTHelpers {

// ---------- SCREEN SETUP ----------

void initScreen(Adafruit_ILI9341 &tft,
                uint8_t rotation,
                uint16_t bgColor) {
    tft.begin();
    tft.setRotation(rotation);
    tft.fillScreen(bgColor);
}

void clear(Adafruit_ILI9341 &tft, uint16_t color) {
    tft.fillScreen(color);
}



// ============================================================================
//                                 TEXT (RAM)
// ============================================================================

void drawText(Adafruit_ILI9341 &tft,
              int16_t x, int16_t y,
              const char *text,
              uint8_t textSize,
              uint16_t textColor,
              int16_t bgColor) {
    tft.setCursor(x, y);
    tft.setTextSize(textSize);

    if (bgColor < 0) tft.setTextColor(textColor);
    else             tft.setTextColor(textColor, (uint16_t)bgColor);

    tft.print(text);
}

void drawCenteredText(Adafruit_ILI9341 &tft,
                      int16_t y,
                      const char *text,
                      uint8_t textSize,
                      uint16_t textColor,
                      int16_t bgColor) {
    int16_t w = textWidthPixels(text, textSize);
    int16_t x = (tft.width() - w) / 2;
    if (x < 0) x = 0;
    drawText(tft, x, y, text, textSize, textColor, bgColor);
}



// ============================================================================
//                          TEXT (FLASH / F("text"))
// ============================================================================

void drawText(Adafruit_ILI9341 &tft,
              int16_t x, int16_t y,
              const __FlashStringHelper *ftext,
              uint8_t textSize,
              uint16_t textColor,
              int16_t bgColor) {
    tft.setCursor(x, y);
    tft.setTextSize(textSize);

    if (bgColor < 0) tft.setTextColor(textColor);
    else             tft.setTextColor(textColor, (uint16_t)bgColor);

    tft.print(ftext);  // Adafruit_GFX supports FlashString directly
}

void drawCenteredText(Adafruit_ILI9341 &tft,
                      int16_t y,
                      const __FlashStringHelper *ftext,
                      uint8_t textSize,
                      uint16_t textColor,
                      int16_t bgColor) {
    int16_t w = textWidthPixels(ftext, textSize);
    int16_t x = (tft.width() - w) / 2;
    if (x < 0) x = 0;
    drawText(tft, x, y, ftext, textSize, textColor, bgColor);
}



// ============================================================================
//                            BOXES / BUTTONS
// ============================================================================

void drawBox(Adafruit_ILI9341 &tft,
             int16_t x, int16_t y,
             int16_t w, int16_t h,
             uint16_t fillColor,
             uint16_t borderColor) {
    tft.fillRect(x, y, w, h, fillColor);
    tft.drawRect(x, y, w, h, borderColor);
}

void drawRoundedBox(Adafruit_ILI9341 &tft,
                    int16_t x, int16_t y,
                    int16_t w, int16_t h,
                    int16_t radius,
                    uint16_t fillColor,
                    uint16_t borderColor) {
    tft.fillRoundRect(x, y, w, h, radius, fillColor);
    tft.drawRoundRect(x, y, w, h, radius, borderColor);
}


// --- RAM string ---
void drawBoxWithText(Adafruit_ILI9341 &tft,
                     int16_t x, int16_t y,
                     int16_t w, int16_t h,
                     const char *label,
                     uint8_t textSize,
                     uint16_t fillColor,
                     uint16_t borderColor,
                     uint16_t textColor) {
    drawBox(tft, x, y, w, h, fillColor, borderColor);

    int16_t textW = textWidthPixels(label, textSize);
    int16_t textX = x + (w - textW) / 2;
    if (textX < x + 2) textX = x + 2;

    int16_t textY = y + (h - 8 * textSize) / 2;
    if (textY < y + 2) textY = y + 2;

    drawText(tft, textX, textY, label, textSize, textColor);
}


// --- FlashString version ---
void drawBoxWithText(Adafruit_ILI9341 &tft,
                     int16_t x, int16_t y,
                     int16_t w, int16_t h,
                     const __FlashStringHelper *ftext,
                     uint8_t textSize,
                     uint16_t fillColor,
                     uint16_t borderColor,
                     uint16_t textColor) {
    drawBox(tft, x, y, w, h, fillColor, borderColor);

    int16_t textW = textWidthPixels(ftext, textSize);
    int16_t textX = x + (w - textW) / 2;
    if (textX < x + 2) textX = x + 2;

    int16_t textY = y + (h - 8 * textSize) / 2;
    if (textY < y + 2) textY = y + 2;

    drawText(tft, textX, textY, ftext, textSize, textColor);
}


// Rounded Box versions (RAM + Flash)

void drawRoundedBoxWithText(Adafruit_ILI9341 &tft, int16_t x,int16_t y,int16_t w,int16_t h,
                            int16_t radius,const char *label,uint8_t textSize,
                            uint16_t fillColor,uint16_t borderColor,uint16_t textColor) {
    drawRoundedBox(tft,x,y,w,h,radius,fillColor,borderColor);
    int16_t textW=textWidthPixels(label,textSize);
    int16_t textX=x+(w-textW)/2; if(textX<x+2) textX=x+2;
    int16_t textY=y+(h-8*textSize)/2; if(textY<y+2) textY=y+2;
    drawText(tft,textX,textY,label,textSize,textColor);
}

void drawRoundedBoxWithText(Adafruit_ILI9341 &tft, int16_t x,int16_t y,int16_t w,int16_t h,
                            int16_t radius,const __FlashStringHelper *ftext,uint8_t textSize,
                            uint16_t fillColor,uint16_t borderColor,uint16_t textColor) {
    drawRoundedBox(tft,x,y,w,h,radius,fillColor,borderColor);
    int16_t textW=textWidthPixels(ftext,textSize);
    int16_t textX=x+(w-textW)/2; if(textX<x+2) textX=x+2;
    int16_t textY=y+(h-8*textSize)/2; if(textY<y+2) textY=y+2;
    drawText(tft,textX,textY,ftext,textSize,textColor);
}


// ============================================================================
//                               BUTTONS
// ============================================================================

void drawButton(Adafruit_ILI9341 &tft,
                int16_t x, int16_t y,
                int16_t w, int16_t h,
                const char *label,
                bool pressed,
                uint8_t textSize) {
    uint16_t fill = pressed ? ILI9341_DARKGREY : ILI9341_NAVY;
    uint16_t border = pressed ? ILI9341_WHITE : ILI9341_CYAN;

    drawRoundedBoxWithText(tft,x,y,w,h,6,label,textSize,fill,border,ILI9341_WHITE);
}

void drawButton(Adafruit_ILI9341 &tft,
                int16_t x, int16_t y,
                int16_t w, int16_t h,
                const __FlashStringHelper *ftext,
                bool pressed,
                uint8_t textSize) {
    uint16_t fill = pressed ? ILI9341_DARKGREY : ILI9341_NAVY;
    uint16_t border = pressed ? ILI9341_WHITE : ILI9341_CYAN;

    drawRoundedBoxWithText(tft,x,y,w,h,6,ftext,textSize,fill,border,ILI9341_WHITE);
}



// ============================================================================
//                         HEADER / FOOTER BARS
// ============================================================================

void drawHeaderBar(Adafruit_ILI9341 &tft,
                   const char *title,
                   uint16_t bgColor,
                   uint16_t textColor) {
    tft.fillRect(0,0,tft.width(),20,bgColor);
    drawCenteredText(tft,4,title,2,textColor,bgColor);
}

void drawHeaderBar(Adafruit_ILI9341 &tft,
                   const __FlashStringHelper *ftext,
                   uint16_t bgColor,
                   uint16_t textColor) {
    tft.fillRect(0,0,tft.width(),20,bgColor);
    drawCenteredText(tft,4,ftext,2,textColor,bgColor);
}


void drawFooterBar(Adafruit_ILI9341 &tft,
                   const char *text,
                   uint16_t bgColor,
                   uint16_t textColor) {
    int16_t h = 18;
    int16_t y = tft.height() - h;
    tft.fillRect(0,y,tft.width(),h,bgColor);
    drawCenteredText(tft,y+2,text,1,textColor,bgColor);
}

void drawFooterBar(Adafruit_ILI9341 &tft,
                   const __FlashStringHelper *ftext,
                   uint16_t bgColor,
                   uint16_t textColor) {
    int16_t h = 18;
    int16_t y = tft.height() - h;
    tft.fillRect(0,y,tft.width(),h,bgColor);
    drawCenteredText(tft,y+2,ftext,1,textColor,bgColor);
}



// ============================================================================
//                              REMAINS UNCHANGED
// ============================================================================
// Includes:
//  - drawHorizontalGauge
  void drawHorizontalGauge(Adafruit_ILI9341 &tft,
                           int16_t x, int16_t y,
                           int16_t w, int16_t h,
                           float value01,
                           uint16_t fillColor,
                           uint16_t bgColor,
                           uint16_t borderColor) {
    if (value01 < 0.0f) value01 = 0.0f;
    if (value01 > 1.0f) value01 = 1.0f;
    int16_t fillW = (int16_t)(w * value01);

    // background
    tft.fillRect(x, y, w, h, bgColor);
    // filled portion
    tft.fillRect(x, y, fillW, h, fillColor);
    // border
    tft.drawRect(x, y, w, h, borderColor);
  }
//  - drawGrid
    void drawGrid(Adafruit_ILI9341 &tft,
                int16_t x, int16_t y,
                int16_t w, int16_t h,
                int16_t xStep, int16_t yStep,
                uint16_t lineColor) {
    // vertical lines
    for (int16_t xx = x; xx <= x + w; xx += xStep) {
      tft.drawLine(xx, y, xx, y + h, lineColor);
    }
    // horizontal lines
    for (int16_t yy = y; yy <= y + h; yy += yStep) {
      tft.drawLine(x, yy, x + w, yy, lineColor);
    }
  }
//  - drawCrosshair
void drawCrosshair(Adafruit_ILI9341 &tft,
                     int16_t x, int16_t y,
                     int16_t size,
                     uint16_t color) {
    tft.drawLine(x - size, y, x + size, y, color);
    tft.drawLine(x, y - size, x, y + size, color);
  }
//  - drawTimeBox
  void drawTimeBox(Adafruit_ILI9341 &tft,
                   int16_t x, int16_t y,
                   int16_t w, int16_t h) {
    drawRoundedBox(tft, x, y, w, h, 4,
                   ILI9341_BLACK, ILI9341_WHITE);
    // You will later call drawText() to print "HH:MM" / date inside
  }

//  - initPlot
//  - plotBuffer
void initPlot(Adafruit_ILI9341 &tft,
                const PlotConfig &cfg,
                int16_t xStep,
                int16_t yStep) {
    // Clear background of plot area
    tft.fillRect(cfg.x, cfg.y, cfg.w, cfg.h, cfg.bgColor);

    // Border
    tft.drawRect(cfg.x, cfg.y, cfg.w, cfg.h, cfg.borderColor);

    // Grid (optional; if step <= 0, skip)
    if (xStep > 0) {
      for (int16_t xx = cfg.x + xStep; xx < cfg.x + cfg.w; xx += xStep) {
        tft.drawLine(xx, cfg.y, xx, cfg.y + cfg.h, cfg.gridColor);
      }
    }
    if (yStep > 0) {
      for (int16_t yy = cfg.y + yStep; yy < cfg.y + cfg.h; yy += yStep) {
        tft.drawLine(cfg.x, yy, cfg.x + cfg.w, yy, cfg.gridColor);
      }
    }

    // init state for smooth plotting
    // start at left edge, no previous point yet
    // (curX is 0..w-1, relative to cfg.x)
    const_cast<PlotConfig&>(cfg).curX = 0;
    const_cast<PlotConfig&>(cfg).hasPrev = false;
    const_cast<PlotConfig&>(cfg).prevY = 0;
  }

  void plotBuffer(Adafruit_ILI9341 &tft,
                  const PlotConfig &cfg,
                  const float *buffer,
                  size_t N,
                  uint16_t lineColor) {
    if (!buffer || N < 2) return;

    // Clear interior (keep border + grid)
    tft.fillRect(cfg.x + 1,
                 cfg.y + 1,
                 cfg.w - 2,
                 cfg.h - 2,
                 cfg.bgColor);

    // Re-draw grid lightly if you like (optional):
    // (comment out if too slow)
    // for (int16_t xx = cfg.x + 10; xx < cfg.x + cfg.w; xx += 10) {
    //   tft.drawLine(xx, cfg.y + 1, xx, cfg.y + cfg.h - 2, cfg.gridColor);
    // }
    // for (int16_t yy = cfg.y + 10; yy < cfg.y + cfg.h; yy += 10) {
    //   tft.drawLine(cfg.x + 1, yy, cfg.x + cfg.w - 2, yy, cfg.gridColor);
    // }

    // X step in pixels (spread N samples across width)
    float stepX;
    if (N <= 1) {
      stepX = 0;
    } else {
      stepX = (float)(cfg.w - 1) / (float)(N - 1);
    }

    int16_t prevX = cfg.x;
    int16_t prevY = mapValueToY(cfg, buffer[0]);

    for (size_t i = 1; i < N; ++i) {
      int16_t x = cfg.x + (int16_t)(stepX * i);
      int16_t y = mapValueToY(cfg, buffer[i]);

      tft.drawLine(prevX, prevY, x, y, lineColor);

      prevX = x;
      prevY = y;
    }

    // Re-draw border (in case we overdraw at edges)
    tft.drawRect(cfg.x, cfg.y, cfg.w, cfg.h, cfg.borderColor);
  }
//  - plotNextSample
void plotNextSample(Adafruit_ILI9341 &tft,
                    PlotConfig &cfg,
                    float newValue,
                    uint16_t lineColor) {
  // Map new sample
  int16_t y = mapValueToY(cfg, newValue);

  // Convert curX (0..w-1) to screen X coordinate
  int16_t x = cfg.x + cfg.curX;

  // If we are at the start of a sweep, clear the interior once
  if (cfg.curX == 0) {
    tft.fillRect(cfg.x + 1,
                 cfg.y + 1,
                 cfg.w - 2,
                 cfg.h - 2,
                 cfg.bgColor);
    cfg.hasPrev = false;
  }

  // Optional: erase a thin vertical strip before drawing (no flicker)
  tft.drawFastVLine(x, cfg.y + 1, cfg.h - 2, cfg.bgColor);

  // Draw line from previous point to current (if we have a previous)
  if (cfg.hasPrev) {
    int16_t prevX = x - 1;
    tft.drawLine(prevX, cfg.prevY, x, y, lineColor);
  }

  cfg.prevY = y;
  cfg.hasPrev = true;

  // Move to next x, wrap at end of plot area
  cfg.curX++;
  if (cfg.curX >= cfg.w) {
    cfg.curX = 0;
  }

  // Re-draw border in case edges got overwritten
  tft.drawRect(cfg.x, cfg.y, cfg.w, cfg.h, cfg.borderColor);
}
//  - uiInitSignalScreen
void uiInitSignalScreen(Adafruit_ILI9341 &tft,
                          PlotConfig &plotCfg,
                          float valMin,
                          float valMax,
                          bool isRunning,
                          const char *modeLabel,
                          const char *title,
                          const char *plotLabel) {
    // Clear full screen first (in case previous screen existed)
    clear(tft, ILI9341_BLACK);

    // --- Top bar with title ---
    drawHeaderBar(tft, title, ILI9341_NAVY, ILI9341_WHITE);

    // Time box in top-right (placeholder time for now)
    int16_t boxW = 70;
    int16_t boxH = 20;
    int16_t timeX = tft.width() - boxW;
    int16_t timeY = 0;
    drawTimeBox(tft, timeX, timeY, boxW, boxH);
    drawText(tft, timeX + 4, timeY + 4, "--:--", 1, ILI9341_GREEN, -1);

    // --- Buttons row under header ---
    int16_t btnY = 22;
    int16_t btnH = 26;

    // START button
    drawButton(tft,
               8, btnY,
               90, btnH,
               "START",
               isRunning,    // pressed when running
               2);

    // STOP button
    drawButton(tft,
               108, btnY,
               90, btnH,
               "STOP",
               !isRunning,   // pressed when stopped
               2);

    // MODE button
    drawButton(tft,
               208, btnY,
               90, btnH,
               modeLabel,
               false,
               1);

    // --- Plot area configuration ---
    int16_t top = btnY + btnH + 8;   // header (20) + buttons + margin
    int16_t bottomMargin = 20;       // room for footer

    plotCfg.x = 8;  // margin left
    plotCfg.y = top; // below buttons
    plotCfg.w = tft.width() - 16;    // margin left/right
    plotCfg.h = tft.height() - top - bottomMargin;

    plotCfg.valMin = valMin;
    plotCfg.valMax = valMax;

    plotCfg.bgColor     = ILI9341_BLACK;
    plotCfg.borderColor = ILI9341_WHITE;
    plotCfg.gridColor   = ILI9341_DARKGREY;

    // Initialize plot (grid + state)
    initPlot(tft, plotCfg, 10, 10);

    // Optional label inside the plot
    if (plotLabel) {
      drawText(tft,
               plotCfg.x + 4,
               plotCfg.y + 4,
               plotLabel,
               1,
               ILI9341_YELLOW,
               -1);
    }

    // --- Bottom status bar ---
    uiUpdateStatusBar(tft, isRunning);
  }
//  - uiUpdateStatusBar
void uiUpdateStatusBar(Adafruit_ILI9341 &tft,
                         bool isRunning) {
    const char *status = isRunning ? "Status: RUNNING" : "Status: PAUSED";
    // reuse drawFooterBar (we know footer is bottom 18px)
    drawFooterBar(tft, status, ILI9341_DARKGREY, ILI9341_WHITE);
  }
//  - uiUpdateTime
void uiUpdateTime(Adafruit_ILI9341 &tft,
                    const char *timeStr) {
    // Same geometry used in uiInitSignalScreen
    int16_t boxW = 70;
    int16_t boxH = 20;
    int16_t x = tft.width() - boxW;
    int16_t y = 0;

    // Redraw time box background
    drawTimeBox(tft, x, y, boxW, boxH);
    // Draw the new time string
    drawText(tft, x + 4, y + 4, timeStr, 1, ILI9341_GREEN, -1);
  }
//
// All unchanged from your version because they do not handle strings.
// You can copy-paste them directly below.
// ============================================================================


// (Paste your unchanged plotting + UI code from previous file here)

} // namespace TFTHelpers
*/