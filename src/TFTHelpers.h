#ifndef TFT_HELPERS_H
#define TFT_HELPERS_H

#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <stddef.h>


namespace TFTHelpers {

  // Basic setup / screen utilities
  void initScreen(Adafruit_ILI9341 &tft, // TFT object
                  uint8_t rotation = 1, // 0-3
                  uint16_t bgColor = ILI9341_BLACK); // background color

  void clear(Adafruit_ILI9341 &tft,
             uint16_t color = ILI9341_BLACK);

  // Text helpers
  void drawText(Adafruit_ILI9341 &tft,
                int16_t x, int16_t y,
                const char *text,
                uint8_t textSize = 2,
                uint16_t textColor = ILI9341_WHITE,
                int16_t bgColor = -1);   // bgColor < 0 = transparent

  void drawCenteredText(Adafruit_ILI9341 &tft,
                        int16_t y,
                        const char *text,
                        uint8_t textSize = 2,
                        uint16_t textColor = ILI9341_WHITE,
                        int16_t bgColor = -1);

  // Simple rectangular boxes
  void drawBox(Adafruit_ILI9341 &tft,
               int16_t x, int16_t y,
               int16_t w, int16_t h,
               uint16_t fillColor,
               uint16_t borderColor);

  // Rounded rectangle box
  void drawRoundedBox(Adafruit_ILI9341 &tft,
                      int16_t x, int16_t y,
                      int16_t w, int16_t h,
                      int16_t radius,
                      uint16_t fillColor,
                      uint16_t borderColor);

  // Box with text centered inside (rectangle)
  void drawBoxWithText(Adafruit_ILI9341 &tft,
                       int16_t x, int16_t y,
                       int16_t w, int16_t h,
                       const char *label,
                       uint8_t textSize,
                       uint16_t fillColor,
                       uint16_t borderColor,
                       uint16_t textColor);

  // Rounded box with text centered inside
  void drawRoundedBoxWithText(Adafruit_ILI9341 &tft,
                              int16_t x, int16_t y,
                              int16_t w, int16_t h,
                              int16_t radius,
                              const char *label,
                              uint8_t textSize,
                              uint16_t fillColor,
                              uint16_t borderColor,
                              uint16_t textColor);

  // Button helper (changes style when "pressed")
  void drawButton(Adafruit_ILI9341 &tft,
                  int16_t x, int16_t y,
                  int16_t w, int16_t h,
                  const char *label,
                  bool pressed,
                  uint8_t textSize = 2);

  // Header / status bars
  void drawHeaderBar(Adafruit_ILI9341 &tft,
                     const char *title,
                     uint16_t bgColor = ILI9341_BLUE,
                     uint16_t textColor = ILI9341_WHITE);

  void drawFooterBar(Adafruit_ILI9341 &tft,
                     const char *text,
                     uint16_t bgColor = ILI9341_DARKGREY,
                     uint16_t textColor = ILI9341_WHITE);

  // Horizontal gauge / bar (value in [0,1])
  void drawHorizontalGauge(Adafruit_ILI9341 &tft,
                           int16_t x, int16_t y,
                           int16_t w, int16_t h,
                           float value01,
                           uint16_t fillColor = ILI9341_GREEN,
                           uint16_t bgColor   = ILI9341_DARKGREY,
                           uint16_t borderColor = ILI9341_WHITE);

  // Simple grid background (for plots)
  void drawGrid(Adafruit_ILI9341 &tft,
                int16_t x, int16_t y,
                int16_t w, int16_t h,
                int16_t xStep, int16_t yStep,
                uint16_t lineColor = ILI9341_DARKGREY);

  // Crosshair / marker
  void drawCrosshair(Adafruit_ILI9341 &tft,
                     int16_t x, int16_t y,
                     int16_t size,
                     uint16_t color = ILI9341_YELLOW);

  // Quick “time area” placeholder (you will fill text yourself)
  void drawTimeBox(Adafruit_ILI9341 &tft,
                   int16_t x, int16_t y,
                   int16_t w, int16_t h);


   // ---- Simple plot helper ----
    struct PlotConfig {
    int16_t x;
    int16_t y;
    int16_t w;
    int16_t h;

    float   valMin;
    float   valMax;

    uint16_t bgColor;
    uint16_t borderColor;
    uint16_t gridColor;

    // NEW: runtime state for smooth plotting
    int16_t curX;      // current x position inside the plot (0 .. w-1)
    int16_t prevY;     // previous y position
    bool    hasPrev;   // do we already have a previous point?
  };


  // Draw initial plot frame + grid (call once in setup)
  void initPlot(Adafruit_ILI9341 &tft,
                const PlotConfig &cfg,
                int16_t xStep = 10,
                int16_t yStep = 10);

  // Plot the whole buffer in the given plot region
  // buffer: array of size N with float samples
  void plotBuffer(Adafruit_ILI9341 &tft,
                  const PlotConfig &cfg,
                  const float *buffer,
                  size_t N,
                  uint16_t lineColor = ILI9341_GREEN);

// Smooth real-time plot: draw only the new sample (no flicker)
  void plotNextSample(Adafruit_ILI9341 &tft,
                      PlotConfig &cfg,
                      float newValue,
                      uint16_t lineColor = ILI9341_GREEN);

void drawSpectrumBars(Adafruit_ILI9341 &tft,
                      const PlotConfig &cfg,
                      const float *spectrum,
                      uint16_t numBins,
                      uint16_t maxFreqHz,
                      float yScale);   // <-- NEW PARAMETER


void uiInitSignalScreen(Adafruit_ILI9341 &tft, // TFT object
                          PlotConfig &plotCfg, // plot configuration (will be filled)
                          float valMin, // value range
                          float valMax, // value range
                          bool isRunning, // initial running state
                          const char *modeLabel, // mode label on button
                          const char *title, // title in top bar
                          const char *plotLabel); // optional label inside plot area

  // Update only the bottom status bar when run/pause changes
  void uiUpdateStatusBar(Adafruit_ILI9341 &tft,
                         bool isRunning);

  // Update only the time text in the top-right time box
  // timeStr example: "12:34" or "12:34:56"
  void uiUpdateTime(Adafruit_ILI9341 &tft,
                    const char *timeStr);



} // namespace TFTHelpers

#endif // TFT_HELPERS_H





















/*
#ifndef TFT_HELPERS_H
#define TFT_HELPERS_H

#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <stddef.h>


namespace TFTHelpers {

  // --- Basic Screen ---
  void initScreen(Adafruit_ILI9341 &tft,
                  uint8_t rotation = 1,
                  uint16_t bgColor = ILI9341_BLACK);

  void clear(Adafruit_ILI9341 &tft,
             uint16_t color = ILI9341_BLACK);

  // --- Text Helpers (char*) ---
  void drawText(Adafruit_ILI9341 &tft,
                int16_t x, int16_t y,
                const char *text,
                uint8_t textSize = 2,
                uint16_t textColor = ILI9341_WHITE,
                int16_t bgColor = -1);

  void drawCenteredText(Adafruit_ILI9341 &tft,
                        int16_t y,
                        const char *text,
                        uint8_t textSize = 2,
                        uint16_t textColor = ILI9341_WHITE,
                        int16_t bgColor = -1);

  // --- NEW: FlashString versions (F("text")) ---
  void drawText(Adafruit_ILI9341 &tft,
                int16_t x, int16_t y,
                const __FlashStringHelper *text,     // NEW
                uint8_t textSize = 2,
                uint16_t textColor = ILI9341_WHITE,
                int16_t bgColor = -1);               // NEW

  void drawCenteredText(Adafruit_ILI9341 &tft,
                        int16_t y,
                        const __FlashStringHelper *text,  // NEW
                        uint8_t textSize = 2,
                        uint16_t textColor = ILI9341_WHITE,
                        int16_t bgColor = -1);            // NEW

  // --- Simple rectangular boxes ---
  void drawBox(Adafruit_ILI9341 &tft,
               int16_t x, int16_t y,
               int16_t w, int16_t h,
               uint16_t fillColor,
               uint16_t borderColor);

  void drawRoundedBox(Adafruit_ILI9341 &tft,
                      int16_t x, int16_t y,
                      int16_t w, int16_t h,
                      int16_t radius,
                      uint16_t fillColor,
                      uint16_t borderColor);

  // --- Boxes with Text ---
  void drawBoxWithText(Adafruit_ILI9341 &tft,
                       int16_t x, int16_t y,
                       int16_t w, int16_t h,
                       const char *label,
                       uint8_t textSize,
                       uint16_t fillColor,
                       uint16_t borderColor,
                       uint16_t textColor);

  // NEW FlashString overload
  void drawBoxWithText(Adafruit_ILI9341 &tft,
                       int16_t x, int16_t y,
                       int16_t w, int16_t h,
                       const __FlashStringHelper *label,  // NEW
                       uint8_t textSize,
                       uint16_t fillColor,
                       uint16_t borderColor,
                       uint16_t textColor);               // NEW

  void drawRoundedBoxWithText(Adafruit_ILI9341 &tft,
                              int16_t x, int16_t y,
                              int16_t w, int16_t h,
                              int16_t radius,
                              const char *label,
                              uint8_t textSize,
                              uint16_t fillColor,
                              uint16_t borderColor,
                              uint16_t textColor);

  // NEW FlashString overload
  void drawRoundedBoxWithText(Adafruit_ILI9341 &tft,
                              int16_t x, int16_t y,
                              int16_t w, int16_t h,
                              int16_t radius,
                              const __FlashStringHelper *label, // NEW
                              uint8_t textSize,
                              uint16_t fillColor,
                              uint16_t borderColor,
                              uint16_t textColor);              // NEW

  // --- Buttons ---
  void drawButton(Adafruit_ILI9341 &tft,
                  int16_t x, int16_t y,
                  int16_t w, int16_t h,
                  const char *label,
                  bool pressed,
                  uint8_t textSize = 2);

  // NEW FlashString overload
  void drawButton(Adafruit_ILI9341 &tft,
                  int16_t x, int16_t y,
                  int16_t w, int16_t h,
                  const __FlashStringHelper *label,   // NEW
                  bool pressed,
                  uint8_t textSize = 2);              // NEW

  // --- Header/Footer bars ---
  void drawHeaderBar(Adafruit_ILI9341 &tft,
                     const char *title,
                     uint16_t bgColor = ILI9341_BLUE,
                     uint16_t textColor = ILI9341_WHITE);

  void drawHeaderBar(Adafruit_ILI9341 &tft,
                     const __FlashStringHelper *title, // NEW
                     uint16_t bgColor = ILI9341_BLUE,
                     uint16_t textColor = ILI9341_WHITE); // NEW

  void drawFooterBar(Adafruit_ILI9341 &tft,
                     const char *text,
                     uint16_t bgColor = ILI9341_DARKGREY,
                     uint16_t textColor = ILI9341_WHITE);

  void drawFooterBar(Adafruit_ILI9341 &tft,
                     const __FlashStringHelper *text,  // NEW
                     uint16_t bgColor = ILI9341_DARKGREY,
                     uint16_t textColor = ILI9341_WHITE); // NEW

  // --- Gauges, grids, plots, etc. (unchanged) ---
  void drawHorizontalGauge(Adafruit_ILI9341 &tft,
                           int16_t x, int16_t y,
                           int16_t w, int16_t h,
                           float value01,
                           uint16_t fillColor = ILI9341_GREEN,
                           uint16_t bgColor = ILI9341_DARKGREY,
                           uint16_t borderColor = ILI9341_WHITE);

  void drawGrid(Adafruit_ILI9341 &tft,
                int16_t x, int16_t y,
                int16_t w, int16_t h,
                int16_t xStep, int16_t yStep,
                uint16_t lineColor = ILI9341_DARKGREY);

  void drawCrosshair(Adafruit_ILI9341 &tft,
                     int16_t x, int16_t y,
                     int16_t size,
                     uint16_t color = ILI9341_YELLOW);

  void drawTimeBox(Adafruit_ILI9341 &tft,
                   int16_t x, int16_t y,
                   int16_t w, int16_t h);

  // ---- Plotting ----
  struct PlotConfig {
    int16_t x, y, w, h;
    float valMin, valMax;
    uint16_t bgColor, borderColor, gridColor;
    int16_t curX, prevY;
    bool hasPrev;
  };

  void initPlot(Adafruit_ILI9341 &tft,
                const PlotConfig &cfg,
                int16_t xStep = 10,
                int16_t yStep = 10);

  void plotBuffer(Adafruit_ILI9341 &tft,
                  const PlotConfig &cfg,
                  const float *buffer,
                  size_t N,
                  uint16_t lineColor = ILI9341_GREEN);

  void plotNextSample(Adafruit_ILI9341 &tft,
                      PlotConfig &cfg,
                      float newValue,
                      uint16_t lineColor = ILI9341_GREEN);

  // UI helpers
  void uiInitSignalScreen(Adafruit_ILI9341 &tft,
                          PlotConfig &plotCfg,
                          float valMin,
                          float valMax,
                          bool isRunning,
                          const char *modeLabel,
                          const char *title,
                          const char *plotLabel);

  void uiUpdateStatusBar(Adafruit_ILI9341 &tft,
                         bool isRunning);

  void uiUpdateTime(Adafruit_ILI9341 &tft,
                    const char *timeStr);

} // namespace TFTHelpers

#endif // TFT_HELPERS_H
*/