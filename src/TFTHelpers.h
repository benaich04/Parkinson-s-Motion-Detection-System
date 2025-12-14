#ifndef TFT_HELPERS_H
#define TFT_HELPERS_H

#include <Arduino.h>
#include <SPI.h>
#include <stddef.h>

// ILI9341 Color definitions (RGB565 format)
#define ILI9341_BLACK       0x0000
#define ILI9341_NAVY        0x000F
#define ILI9341_DARKGREEN   0x03E0
#define ILI9341_DARKCYAN    0x03EF
#define ILI9341_MAROON      0x7800
#define ILI9341_PURPLE      0x780F
#define ILI9341_OLIVE       0x7BE0
#define ILI9341_LIGHTGREY   0xC618
#define ILI9341_DARKGREY    0x7BEF
#define ILI9341_BLUE        0x001F
#define ILI9341_GREEN       0x07E0
#define ILI9341_CYAN        0x07FF
#define ILI9341_RED         0xF800
#define ILI9341_MAGENTA     0xF81F
#define ILI9341_YELLOW      0xFFE0
#define ILI9341_WHITE       0xFFFF
#define ILI9341_ORANGE      0xFD20
#define ILI9341_GREENYELLOW 0xAFE5
#define ILI9341_PINK        0xF81F

// ILI9341 Commands
#define ILI9341_NOP         0x00
#define ILI9341_SWRESET     0x01
#define ILI9341_SLPOUT      0x11
#define ILI9341_DISPON      0x29
#define ILI9341_CASET       0x2A
#define ILI9341_PASET       0x2B
#define ILI9341_RAMWR       0x2C
#define ILI9341_MADCTL      0x36
#define ILI9341_PIXFMT      0x3A

namespace TFTHelpers {

  // Direct SPI Display class for ILI9341
  class ILI9341_Display {
  public:
    ILI9341_Display(int8_t cs_pin, int8_t dc_pin, int8_t rst_pin = -1);
    
    // Initialize SPI and display
    void begin();
    
    // Basic operations
    void setRotation(uint8_t rotation);
    void fillScreen(uint16_t color);
    void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
    void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
    void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
    void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
    void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
    void fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color);
    void drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color);
    void drawPixel(int16_t x, int16_t y, uint16_t color);
    // Text rendering
    void drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size);
    
    // Text operations
    void setCursor(int16_t x, int16_t y);
    void setTextSize(uint8_t size);
    void setTextColor(uint16_t color);
    void setTextColor(uint16_t color, uint16_t bg);
    void print(const char* text);
    void print(const __FlashStringHelper* text);
    
    // Getters
    int16_t width() const { return _width; }
    int16_t height() const { return _height; }
    
  private:
    int8_t _cs_pin;
    int8_t _dc_pin;
    int8_t _rst_pin;
    int16_t _width;
    int16_t _height;
    uint8_t _rotation;
    int16_t _cursor_x;
    int16_t _cursor_y;
    uint8_t _text_size;
    uint16_t _text_color;
    uint16_t _text_bg_color;
    bool _text_has_bg;
    
    // Low-level SPI communication
    void writeCommand(uint8_t cmd);
    void writeData(uint8_t data);
    void writeData16(uint16_t data);
    void writeData32(uint32_t data);
    void setAddrWindow(int16_t x0, int16_t y0, int16_t x1, int16_t y1);
    void pushColor(uint16_t color);
    void pushColors(uint16_t* colors, uint16_t len);
    
    // Hardware control
    void spiBegin();
    void spiEnd();
    void hardwareReset();
    
    
  };

  // Helper functions - same API as original, but using ILI9341_Display
  void initScreen(ILI9341_Display &tft,
                  uint8_t rotation = 1,
                  uint16_t bgColor = ILI9341_BLACK);

  void clear(ILI9341_Display &tft,
             uint16_t color = ILI9341_BLACK);

  void drawText(ILI9341_Display &tft,
                int16_t x, int16_t y,
                const char *text,
                uint8_t textSize = 2,
                uint16_t textColor = ILI9341_WHITE,
                int16_t bgColor = -1);

  void drawCenteredText(ILI9341_Display &tft,
                        int16_t y,
                        const char *text,
                        uint8_t textSize = 2,
                        uint16_t textColor = ILI9341_WHITE,
                        int16_t bgColor = -1);

  void drawBox(ILI9341_Display &tft,
               int16_t x, int16_t y,
               int16_t w, int16_t h,
               uint16_t fillColor,
               uint16_t borderColor);

  void drawRoundedBox(ILI9341_Display &tft,
                      int16_t x, int16_t y,
                      int16_t w, int16_t h,
                      int16_t radius,
                      uint16_t fillColor,
                      uint16_t borderColor);

  void drawBoxWithText(ILI9341_Display &tft,
                       int16_t x, int16_t y,
                       int16_t w, int16_t h,
                       const char *label,
                       uint8_t textSize,
                       uint16_t fillColor,
                       uint16_t borderColor,
                       uint16_t textColor);

  void drawRoundedBoxWithText(ILI9341_Display &tft,
                              int16_t x, int16_t y,
                              int16_t w, int16_t h,
                              int16_t radius,
                              const char *label,
                              uint8_t textSize,
                              uint16_t fillColor,
                              uint16_t borderColor,
                              uint16_t textColor);

  void drawButton(ILI9341_Display &tft,
                  int16_t x, int16_t y,
                  int16_t w, int16_t h,
                  const char *label,
                  bool pressed,
                  uint8_t textSize = 2);

  void drawHeaderBar(ILI9341_Display &tft,
                     const char *title,
                     uint16_t bgColor = ILI9341_BLUE,
                     uint16_t textColor = ILI9341_WHITE);

  void drawFooterBar(ILI9341_Display &tft,
                     const char *text,
                     uint16_t bgColor = ILI9341_DARKGREY,
                     uint16_t textColor = ILI9341_WHITE);

  void drawHorizontalGauge(ILI9341_Display &tft,
                           int16_t x, int16_t y,
                           int16_t w, int16_t h,
                           float value01,
                           uint16_t fillColor = ILI9341_GREEN,
                           uint16_t bgColor = ILI9341_DARKGREY,
                           uint16_t borderColor = ILI9341_WHITE);

  void drawGrid(ILI9341_Display &tft,
                int16_t x, int16_t y,
                int16_t w, int16_t h,
                int16_t xStep, int16_t yStep,
                uint16_t lineColor = ILI9341_DARKGREY);

  void drawCrosshair(ILI9341_Display &tft,
                     int16_t x, int16_t y,
                     int16_t size,
                     uint16_t color = ILI9341_YELLOW);

  void drawTimeBox(ILI9341_Display &tft,
                   int16_t x, int16_t y,
                   int16_t w, int16_t h);

  // Plot configuration structure
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
    int16_t curX;
    int16_t prevY;
    bool    hasPrev;
  };

  void initPlot(ILI9341_Display &tft,
                const PlotConfig &cfg,
                int16_t xStep = 10,
                int16_t yStep = 10);

  void plotBuffer(ILI9341_Display &tft,
                  const PlotConfig &cfg,
                  const float *buffer,
                  size_t N,
                  uint16_t lineColor = ILI9341_GREEN);

  void plotNextSample(ILI9341_Display &tft,
                      PlotConfig &cfg,
                      float newValue,
                      uint16_t lineColor = ILI9341_GREEN);

  void drawSpectrumBars(ILI9341_Display &tft,
                        const PlotConfig &cfg,
                        const float *spectrum,
                        uint16_t numBins,
                        uint16_t maxFreqHz,
                        float yScale);

  void uiInitSignalScreen(ILI9341_Display &tft,
                          PlotConfig &plotCfg,
                          float valMin,
                          float valMax,
                          bool isRunning,
                          const char *modeLabel,
                          const char *title,
                          const char *plotLabel);

  void uiUpdateStatusBar(ILI9341_Display &tft,
                         bool isRunning);

  void uiUpdateTime(ILI9341_Display &tft,
                    const char *timeStr);

  // Utility: Convert RGB888 to RGB565 color
  inline uint16_t color565(uint8_t r, uint8_t g, uint8_t b) {
    return ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
  }

} // namespace TFTHelpers

#endif // TFT_HELPERS_H