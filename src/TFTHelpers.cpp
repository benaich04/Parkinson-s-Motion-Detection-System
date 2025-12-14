#include "TFTHelpers.h"
#include <string.h>
#include <math.h>

// 6x8 font (ASCII 32-127) - More readable alternative
static const uint8_t font6x8[][6] PROGMEM = {
  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // Space (32)
  {0x00, 0x00, 0x5F, 0x5F, 0x00, 0x00}, // ! (33)
  {0x00, 0x07, 0x07, 0x00, 0x07, 0x07}, // " (34)
  {0x14, 0x7F, 0x7F, 0x14, 0x7F, 0x7F}, // # (35)
  {0x24, 0x2E, 0x6B, 0x6B, 0x3A, 0x12}, // $ (36)
  {0x46, 0x66, 0x30, 0x18, 0x0C, 0x66}, // % (37)
  {0x30, 0x7A, 0x4F, 0x5D, 0x37, 0x7A}, // & (38)
  {0x00, 0x00, 0x07, 0x07, 0x00, 0x00}, // ' (39)
  {0x00, 0x1C, 0x3E, 0x63, 0x41, 0x00}, // ( (40)
  {0x00, 0x41, 0x63, 0x3E, 0x1C, 0x00}, // ) (41)
  {0x08, 0x2A, 0x3E, 0x1C, 0x3E, 0x2A}, // * (42)
  {0x08, 0x08, 0x3E, 0x3E, 0x08, 0x08}, // + (43)
  {0x00, 0x80, 0xE0, 0x60, 0x00, 0x00}, // , (44)
  {0x08, 0x08, 0x08, 0x08, 0x08, 0x08}, // - (45)
  {0x00, 0x00, 0x60, 0x60, 0x00, 0x00}, // . (46)
  {0x60, 0x30, 0x18, 0x0C, 0x06, 0x03}, // / (47)
  
  // Numbers 0-9 with subtle serifs (48-57)
  {0x3E, 0x7F, 0x59, 0x4D, 0x7F, 0x3E}, // 0 (48)
  {0x42, 0x42, 0x7F, 0x7F, 0x40, 0x40}, // 1 (49)
  {0x62, 0x73, 0x59, 0x49, 0x6F, 0x66}, // 2 (50)
  {0x22, 0x63, 0x49, 0x49, 0x7F, 0x36}, // 3 (51)
  {0x18, 0x1C, 0x16, 0x7F, 0x7F, 0x10}, // 4 (52)
  {0x27, 0x67, 0x45, 0x45, 0x7D, 0x39}, // 5 (53)
  {0x3C, 0x7E, 0x4B, 0x49, 0x79, 0x30}, // 6 (54)
  {0x03, 0x03, 0x71, 0x79, 0x0F, 0x07}, // 7 (55)
  {0x36, 0x7F, 0x49, 0x49, 0x7F, 0x36}, // 8 (56)
  {0x06, 0x4F, 0x49, 0x69, 0x3F, 0x1E}, // 9 (57)
  
  {0x00, 0x00, 0x66, 0x66, 0x00, 0x00}, // : (58)
  {0x00, 0x80, 0xE6, 0x66, 0x00, 0x00}, // ; (59)
  {0x08, 0x1C, 0x36, 0x63, 0x41, 0x00}, // < (60)
  {0x14, 0x14, 0x14, 0x14, 0x14, 0x14}, // = (61)
  {0x00, 0x41, 0x63, 0x36, 0x1C, 0x08}, // > (62)
  {0x02, 0x03, 0x51, 0x59, 0x0F, 0x06}, // ? (63)
  {0x3E, 0x7F, 0x41, 0x5D, 0x5D, 0x1F}, // @ (64)
  
  // Uppercase A-Z with serifs (65-90)
  {0x7C, 0x7E, 0x13, 0x13, 0x7E, 0x7C}, // A (65)
  {0x41, 0x7F, 0x7F, 0x49, 0x49, 0x7F}, // B (66)
  {0x1C, 0x3E, 0x63, 0x41, 0x41, 0x63}, // C (67)
  {0x41, 0x7F, 0x7F, 0x41, 0x63, 0x3E}, // D (68)
  {0x41, 0x7F, 0x7F, 0x49, 0x5D, 0x41}, // E (69)
  {0x41, 0x7F, 0x7F, 0x49, 0x1D, 0x01}, // F (70)
  {0x1C, 0x3E, 0x63, 0x41, 0x51, 0x73}, // G (71)
  {0x7F, 0x7F, 0x08, 0x08, 0x7F, 0x7F}, // H (72)
  {0x00, 0x41, 0x7F, 0x7F, 0x41, 0x00}, // I (73)
  {0x30, 0x70, 0x40, 0x41, 0x7F, 0x3F}, // J (74)
  {0x41, 0x7F, 0x7F, 0x08, 0x1C, 0x77}, // K (75)
  {0x41, 0x7F, 0x7F, 0x41, 0x40, 0x60}, // L (76)
  {0x7F, 0x7F, 0x06, 0x0C, 0x06, 0x7F}, // M (77)
  {0x7F, 0x7F, 0x06, 0x0C, 0x18, 0x7F}, // N (78)
  {0x1C, 0x3E, 0x63, 0x41, 0x63, 0x3E}, // O (79)
  {0x41, 0x7F, 0x7F, 0x49, 0x09, 0x0F}, // P (80)
  {0x1E, 0x3F, 0x21, 0x71, 0x7F, 0x5E}, // Q (81)
  {0x41, 0x7F, 0x7F, 0x09, 0x19, 0x7F}, // R (82)
  {0x26, 0x6F, 0x4D, 0x59, 0x73, 0x32}, // S (83)
  {0x03, 0x41, 0x7F, 0x7F, 0x41, 0x03}, // T (84)
  {0x7F, 0x7F, 0x40, 0x40, 0x7F, 0x7F}, // U (85)
  {0x1F, 0x3F, 0x60, 0x60, 0x3F, 0x1F}, // V (86)
  {0x7F, 0x7F, 0x30, 0x18, 0x30, 0x7F}, // W (87)
  {0x63, 0x77, 0x1C, 0x08, 0x1C, 0x77}, // X (88)
  {0x07, 0x4F, 0x78, 0x78, 0x4F, 0x07}, // Y (89)
  {0x47, 0x63, 0x71, 0x59, 0x4D, 0x67}, // Z (90)
  
  {0x00, 0x7F, 0x7F, 0x41, 0x41, 0x00}, // [ (91)
  {0x03, 0x06, 0x0C, 0x18, 0x30, 0x60}, // \ (92)
  {0x00, 0x41, 0x41, 0x7F, 0x7F, 0x00}, // ] (93)
  {0x08, 0x0C, 0x06, 0x03, 0x06, 0x0C}, // ^ (94)
  {0x80, 0x80, 0x80, 0x80, 0x80, 0x80}, // _ (95)
  {0x00, 0x00, 0x03, 0x07, 0x04, 0x00}, // ` (96)
  
  // Lowercase a-z with subtle serifs (97-122)
  {0x20, 0x74, 0x54, 0x54, 0x3C, 0x78}, // a (97)
  {0x41, 0x7F, 0x3F, 0x48, 0x48, 0x78}, // b (98)
  {0x38, 0x7C, 0x44, 0x44, 0x6C, 0x28}, // c (99)
  {0x30, 0x78, 0x48, 0x49, 0x3F, 0x7F}, // d (100)
  {0x38, 0x7C, 0x54, 0x54, 0x5C, 0x18}, // e (101)
  {0x48, 0x7E, 0x7F, 0x49, 0x03, 0x02}, // f (102)
  {0x98, 0xBC, 0xA4, 0xA4, 0xF8, 0x7C}, // g (103)
  {0x41, 0x7F, 0x7F, 0x08, 0x04, 0x7C}, // h (104)
  {0x00, 0x44, 0x7D, 0x7D, 0x40, 0x00}, // i (105)
  {0x60, 0xE0, 0x80, 0x84, 0xFD, 0x7D}, // j (106)
  {0x41, 0x7F, 0x7F, 0x10, 0x38, 0x6C}, // k (107)
  {0x00, 0x41, 0x7F, 0x7F, 0x40, 0x00}, // l (108)
  {0x7C, 0x7C, 0x18, 0x38, 0x1C, 0x7C}, // m (109)
  {0x7C, 0x7C, 0x04, 0x04, 0x7C, 0x78}, // n (110)
  {0x38, 0x7C, 0x44, 0x44, 0x7C, 0x38}, // o (111)
  {0x84, 0xFC, 0xF8, 0xA4, 0x24, 0x3C}, // p (112)
  {0x18, 0x3C, 0x24, 0xA4, 0xF8, 0xFC}, // q (113)
  {0x44, 0x7C, 0x78, 0x4C, 0x04, 0x1C}, // r (114)
  {0x48, 0x5C, 0x54, 0x54, 0x74, 0x24}, // s (115)
  {0x00, 0x04, 0x3E, 0x7F, 0x44, 0x24}, // t (116)
  {0x3C, 0x7C, 0x40, 0x40, 0x3C, 0x7C}, // u (117)
  {0x1C, 0x3C, 0x60, 0x60, 0x3C, 0x1C}, // v (118)
  {0x3C, 0x7C, 0x70, 0x38, 0x70, 0x7C}, // w (119)
  {0x44, 0x6C, 0x38, 0x10, 0x38, 0x6C}, // x (120)
  {0x9C, 0xBC, 0xA0, 0xA0, 0xFC, 0x7C}, // y (121)
  {0x4C, 0x64, 0x74, 0x5C, 0x4C, 0x64}, // z (122)
  
  {0x00, 0x08, 0x08, 0x3E, 0x77, 0x41}, // { (123)
  {0x00, 0x00, 0x00, 0x77, 0x77, 0x00}, // | (124)
  {0x41, 0x77, 0x3E, 0x08, 0x08, 0x00}, // } (125)
  {0x02, 0x03, 0x01, 0x03, 0x02, 0x03}, // ~ (126)
  {0x70, 0x78, 0x4C, 0x46, 0x4C, 0x78}, // DEL (127)
};

namespace TFTHelpers {

// ============================================================================
//                         ILI9341_Display Implementation
// ============================================================================

ILI9341_Display::ILI9341_Display(int8_t cs_pin, int8_t dc_pin, int8_t rst_pin)
  : _cs_pin(cs_pin),
    _dc_pin(dc_pin),
    _rst_pin(rst_pin),
    _width(240),
    _height(320),
    _rotation(0),
    _cursor_x(0),
    _cursor_y(0),
    _text_size(1),
    _text_color(ILI9341_WHITE),
    _text_bg_color(ILI9341_BLACK),
    _text_has_bg(false) {
}

void ILI9341_Display::spiBegin() {
  digitalWrite(_cs_pin, LOW);
}

void ILI9341_Display::spiEnd() {
  digitalWrite(_cs_pin, HIGH);
}

void ILI9341_Display::writeCommand(uint8_t cmd) {
  digitalWrite(_dc_pin, LOW);  // Command mode
  spiBegin();
  SPI.transfer(cmd);
  spiEnd();
}

void ILI9341_Display::writeData(uint8_t data) {
  digitalWrite(_dc_pin, HIGH);  // Data mode
  spiBegin();
  SPI.transfer(data);
  spiEnd();
}

void ILI9341_Display::writeData16(uint16_t data) {
  digitalWrite(_dc_pin, HIGH);
  spiBegin();
  SPI.transfer(data >> 8);
  SPI.transfer(data & 0xFF);
  spiEnd();
}

void ILI9341_Display::writeData32(uint32_t data) {
  digitalWrite(_dc_pin, HIGH);
  spiBegin();
  SPI.transfer(data >> 24);
  SPI.transfer(data >> 16);
  SPI.transfer(data >> 8);
  SPI.transfer(data);
  spiEnd();
}

void ILI9341_Display::hardwareReset() {
  if (_rst_pin >= 0) {
    pinMode(_rst_pin, OUTPUT);
    digitalWrite(_rst_pin, HIGH);
    delay(5);
    digitalWrite(_rst_pin, LOW);
    delay(20);
    digitalWrite(_rst_pin, HIGH);
    delay(150);
  }
}

void ILI9341_Display::begin() {
  // Setup pins
  pinMode(_cs_pin, OUTPUT);
  pinMode(_dc_pin, OUTPUT);
  digitalWrite(_cs_pin, HIGH);
  digitalWrite(_dc_pin, HIGH);
  
  // Initialize SPI
  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV2); // 8MHz for 32u4 @ 16MHz
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE0);
  
  // Hardware reset
  hardwareReset();
  
  // ILI9341 Initialization sequence
  writeCommand(0xEF);
  writeData(0x03);
  writeData(0x80);
  writeData(0x02);
  
  writeCommand(0xCF);
  writeData(0x00);
  writeData(0xC1);
  writeData(0x30);
  
  writeCommand(0xED);
  writeData(0x64);
  writeData(0x03);
  writeData(0x12);
  writeData(0x81);
  
  writeCommand(0xE8);
  writeData(0x85);
  writeData(0x00);
  writeData(0x78);
  
  writeCommand(0xCB);
  writeData(0x39);
  writeData(0x2C);
  writeData(0x00);
  writeData(0x34);
  writeData(0x02);
  
  writeCommand(0xF7);
  writeData(0x20);
  
  writeCommand(0xEA);
  writeData(0x00);
  writeData(0x00);
  
  writeCommand(0xC0);    // Power control
  writeData(0x23);       // VRH[5:0]
  
  writeCommand(0xC1);    // Power control
  writeData(0x10);       // SAP[2:0];BT[3:0]
  
  writeCommand(0xC5);    // VCM control
  writeData(0x3E);
  writeData(0x28);
  
  writeCommand(0xC7);    // VCM control2
  writeData(0x86);
  
  writeCommand(ILI9341_MADCTL);
  writeData(0x48);
  
  writeCommand(ILI9341_PIXFMT);
  writeData(0x55);
  
  writeCommand(0xB1);
  writeData(0x00);
  writeData(0x18);
  
  writeCommand(0xB6);    // Display Function Control
  writeData(0x08);
  writeData(0x82);
  writeData(0x27);
  
  writeCommand(0xF2);    // 3Gamma Function Disable
  writeData(0x00);
  
  writeCommand(0x26);    // Gamma curve selected
  writeData(0x01);
  
  writeCommand(0xE0);    // Set Gamma
  writeData(0x0F);
  writeData(0x31);
  writeData(0x2B);
  writeData(0x0C);
  writeData(0x0E);
  writeData(0x08);
  writeData(0x4E);
  writeData(0xF1);
  writeData(0x37);
  writeData(0x07);
  writeData(0x10);
  writeData(0x03);
  writeData(0x0E);
  writeData(0x09);
  writeData(0x00);
  
  writeCommand(0xE1);    // Set Gamma
  writeData(0x00);
  writeData(0x0E);
  writeData(0x14);
  writeData(0x03);
  writeData(0x11);
  writeData(0x07);
  writeData(0x31);
  writeData(0xC1);
  writeData(0x48);
  writeData(0x08);
  writeData(0x0F);
  writeData(0x0C);
  writeData(0x31);
  writeData(0x36);
  writeData(0x0F);
  
  writeCommand(ILI9341_SLPOUT);    // Exit Sleep
  delay(120);
  
  writeCommand(ILI9341_DISPON);    // Display on
  delay(20);
}

void ILI9341_Display::setAddrWindow(int16_t x0, int16_t y0, int16_t x1, int16_t y1) {
  writeCommand(ILI9341_CASET); // Column addr set
  writeData16(x0);
  writeData16(x1);
  
  writeCommand(ILI9341_PASET); // Row addr set
  writeData16(y0);
  writeData16(y1);
  
  writeCommand(ILI9341_RAMWR); // Write to RAM
}

void ILI9341_Display::pushColor(uint16_t color) {
  digitalWrite(_dc_pin, HIGH);
  spiBegin();
  SPI.transfer(color >> 8);
  SPI.transfer(color & 0xFF);
  spiEnd();
}

void ILI9341_Display::pushColors(uint16_t* colors, uint16_t len) {
  digitalWrite(_dc_pin, HIGH);
  spiBegin();
  for (uint16_t i = 0; i < len; i++) {
    SPI.transfer(colors[i] >> 8);
    SPI.transfer(colors[i] & 0xFF);
  }
  spiEnd();
}

void ILI9341_Display::setRotation(uint8_t rotation) {
  _rotation = rotation % 4;
  
  writeCommand(ILI9341_MADCTL);
  
  switch (_rotation) {
    case 0:
      writeData(0x48);
      _width = 240;
      _height = 320;
      break;
    case 1:
      writeData(0x28);
      _width = 320;
      _height = 240;
      break;
    case 2:
      writeData(0x88);
      _width = 240;
      _height = 320;
      break;
    case 3:
      writeData(0xE8);
      _width = 320;
      _height = 240;
      break;
  }
}

void ILI9341_Display::drawPixel(int16_t x, int16_t y, uint16_t color) {
  if (x < 0 || x >= _width || y < 0 || y >= _height) return;
  
  setAddrWindow(x, y, x, y);
  pushColor(color);
}

void ILI9341_Display::fillScreen(uint16_t color) {
  fillRect(0, 0, _width, _height, color);
}

void ILI9341_Display::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
  // Clipping
  if (x < 0) { w += x; x = 0; }
  if (y < 0) { h += y; y = 0; }
  if (x + w > _width) w = _width - x;
  if (y + h > _height) h = _height - y;
  if (w <= 0 || h <= 0) return;
  
  setAddrWindow(x, y, x + w - 1, y + h - 1);
  
  uint8_t hi = color >> 8;
  uint8_t lo = color & 0xFF;
  
  digitalWrite(_dc_pin, HIGH);
  spiBegin();
  
  uint32_t pixels = (uint32_t)w * (uint32_t)h;
  for (uint32_t i = 0; i < pixels; i++) {
    SPI.transfer(hi);
    SPI.transfer(lo);
  }
  
  spiEnd();
}

void ILI9341_Display::drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {
  fillRect(x, y, 1, h, color);
}

void ILI9341_Display::drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {
  fillRect(x, y, w, 1, color);
}

void ILI9341_Display::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {
  // Bresenham's line algorithm
  int16_t dx = abs(x1 - x0);
  int16_t dy = abs(y1 - y0);
  int16_t sx = (x0 < x1) ? 1 : -1;
  int16_t sy = (y0 < y1) ? 1 : -1;
  int16_t err = dx - dy;
  
  while (true) {
    drawPixel(x0, y0, color);
    
    if (x0 == x1 && y0 == y1) break;
    
    int16_t e2 = 2 * err;
    if (e2 > -dy) {
      err -= dy;
      x0 += sx;
    }
    if (e2 < dx) {
      err += dx;
      y0 += sy;
    }
  }
}

void ILI9341_Display::drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
  drawFastHLine(x, y, w, color);
  drawFastHLine(x, y + h - 1, w, color);
  drawFastVLine(x, y, h, color);
  drawFastVLine(x + w - 1, y, h, color);
}

void ILI9341_Display::fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color) {
  // Main rectangles
  fillRect(x + r, y, w - 2 * r, h, color);
  fillRect(x, y + r, r, h - 2 * r, color);
  fillRect(x + w - r, y + r, r, h - 2 * r, color);
  
  // Corners (simplified circle approximation)
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t px = 0;
  int16_t py = r;
  
  while (px < py) {
    if (f >= 0) {
      py--;
      ddF_y += 2;
      f += ddF_y;
    }
    px++;
    ddF_x += 2;
    f += ddF_x;
    
    // Draw corner fills
    drawFastHLine(x + r - px, y + r - py, 2 * px, color);
    drawFastHLine(x + w - r - px, y + r - py, 2 * px, color);
    drawFastHLine(x + r - py, y + r - px, 2 * py, color);
    drawFastHLine(x + w - r - py, y + r - px, 2 * py, color);
    
    drawFastHLine(x + r - px, y + h - r + py - 1, 2 * px, color);
    drawFastHLine(x + w - r - px, y + h - r + py - 1, 2 * px, color);
    drawFastHLine(x + r - py, y + h - r + px - 1, 2 * py, color);
    drawFastHLine(x + w - r - py, y + h - r + px - 1, 2 * py, color);
  }
}

void ILI9341_Display::drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color) {
  // Lines
  drawFastHLine(x + r, y, w - 2 * r, color);
  drawFastHLine(x + r, y + h - 1, w - 2 * r, color);
  drawFastVLine(x, y + r, h - 2 * r, color);
  drawFastVLine(x + w - 1, y + r, h - 2 * r, color);
  
  // Corners (simplified)
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t px = 0;
  int16_t py = r;
  
  while (px < py) {
    if (f >= 0) {
      py--;
      ddF_y += 2;
      f += ddF_y;
    }
    px++;
    ddF_x += 2;
    f += ddF_x;
    
    drawPixel(x + r - px, y + r - py, color);
    drawPixel(x + w - r + px - 1, y + r - py, color);
    drawPixel(x + r - py, y + r - px, color);
    drawPixel(x + w - r + py - 1, y + r - px, color);
    
    drawPixel(x + r - px, y + h - r + py - 1, color);
    drawPixel(x + w - r + px - 1, y + h - r + py - 1, color);
    drawPixel(x + r - py, y + h - r + px - 1, color);
    drawPixel(x + w - r + py - 1, y + h - r + px - 1, color);
  }
}

void ILI9341_Display::setCursor(int16_t x, int16_t y) {
  _cursor_x = x;
  _cursor_y = y;
}

void ILI9341_Display::setTextSize(uint8_t size) {
  _text_size = (size > 0) ? size : 1;
}

void ILI9341_Display::setTextColor(uint16_t color) {
  _text_color = color;
  _text_has_bg = false;
}

void ILI9341_Display::setTextColor(uint16_t color, uint16_t bg) {
  _text_color = color;
  _text_bg_color = bg;
  _text_has_bg = true;
}

void ILI9341_Display::drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size) {
  if (c < 32 || c > 127) c = 32; // Limit to printable ASCII
  c -= 32;
  
  for (uint8_t i = 0; i < 6; i++) {
    uint8_t line = pgm_read_byte(&font6x8[c][i]);
    for (uint8_t j = 0; j < 8; j++) {
      if (line & 0x01) {
        if (size == 1) {
          drawPixel(x + i, y + j, color);
        } else {
          fillRect(x + i * size, y + j * size, size, size, color);
        }
      } else if (_text_has_bg) {
        if (size == 1) {
          drawPixel(x + i, y + j, bg);
        } else {
          fillRect(x + i * size, y + j * size, size, size, bg);
        }
      }
      line >>= 1;
    }
  }
}

void ILI9341_Display::print(const char* text) {
  if (!text) return;
  
  while (*text) {
    if (*text == '\n') {
      _cursor_x = 0;
      _cursor_y += _text_size * 8;
    } else if (*text == '\r') {
      // Skip
    } else {
      if (_text_has_bg) {
        drawChar(_cursor_x, _cursor_y, *text, _text_color, _text_bg_color, _text_size);
      } else {
        drawChar(_cursor_x, _cursor_y, *text, _text_color, 0, _text_size);
      }
      _cursor_x += _text_size * 7;
    }
    text++;
  }
}

void ILI9341_Display::print(const __FlashStringHelper* text) {
  if (!text) return;
  
  PGM_P p = reinterpret_cast<PGM_P>(text);
  char c;
  while ((c = pgm_read_byte(p++))) {
    if (c == '\n') {
      _cursor_x = 0;
      _cursor_y += _text_size * 8;
    } else if (c == '\r') {
      // Skip
    } else {
      if (_text_has_bg) {
        drawChar(_cursor_x, _cursor_y, c, _text_color, _text_bg_color, _text_size);
      } else {
        drawChar(_cursor_x, _cursor_y, c, _text_color, 0, _text_size);
      }
      _cursor_x += _text_size * 6;
    }
  }
}

// ============================================================================
//                         Helper Functions
// ============================================================================

namespace {
  int16_t textWidthPixels(const char *text, uint8_t textSize) {
    if (!text) return 0;
    int16_t len = (int16_t)strlen(text);
    return len * 7 * textSize;
  }

  int16_t mapValueToY(const PlotConfig &cfg, float v) {
    if (v < cfg.valMin) v = cfg.valMin;
    if (v > cfg.valMax) v = cfg.valMax;
    float norm = (v - cfg.valMin) / (cfg.valMax - cfg.valMin);
    int16_t y = cfg.y + cfg.h - (int16_t)(norm * cfg.h);
    return y;
  }
}

// ============================================================================
//                         Public API Functions
// ============================================================================

void initScreen(ILI9341_Display &tft, uint8_t rotation, uint16_t bgColor) {
  tft.begin();
  tft.setRotation(rotation);
  tft.fillScreen(bgColor);
}

void clear(ILI9341_Display &tft, uint16_t color) {
  tft.fillScreen(color);
}

void drawText(ILI9341_Display &tft, int16_t x, int16_t y, const char *text,
              uint8_t textSize, uint16_t textColor, int16_t bgColor) {
  tft.setCursor(x, y);
  tft.setTextSize(textSize);
  if (bgColor < 0) {
    tft.setTextColor(textColor);
  } else {
    tft.setTextColor(textColor, (uint16_t)bgColor);
  }
  tft.print(text);
}

void drawCenteredText(ILI9341_Display &tft, int16_t y, const char *text,
                      uint8_t textSize, uint16_t textColor, int16_t bgColor) {
  int16_t w = textWidthPixels(text, textSize);
  int16_t x = (tft.width() - w) / 2;
  if (x < 0) x = 0;
  drawText(tft, x, y, text, textSize, textColor, bgColor);
}

void drawBox(ILI9341_Display &tft, int16_t x, int16_t y, int16_t w, int16_t h,
             uint16_t fillColor, uint16_t borderColor) {
  tft.fillRect(x, y, w, h, fillColor);
  tft.drawRect(x, y, w, h, borderColor);
}

void drawRoundedBox(ILI9341_Display &tft, int16_t x, int16_t y, int16_t w, int16_t h,
                    int16_t radius, uint16_t fillColor, uint16_t borderColor) {
  tft.fillRoundRect(x, y, w, h, radius, fillColor);
  tft.drawRoundRect(x, y, w, h, radius, borderColor);
}

void drawBoxWithText(ILI9341_Display &tft, int16_t x, int16_t y, int16_t w, int16_t h,
                    const char *label, uint8_t textSize,
                    uint16_t fillColor, uint16_t borderColor, uint16_t textColor) {
  drawBox(tft, x, y, w, h, fillColor, borderColor);
  int16_t textW = textWidthPixels(label, textSize);
  int16_t textX = x + (w - textW) / 2;
  if (textX < x + 2) textX = x + 2;
  int16_t textY = y + (h - (8 * textSize)) / 2;
  if (textY < y + 2) textY = y + 2;
  drawText(tft, textX, textY, label, textSize, textColor, -1);
}

void drawRoundedBoxWithText(ILI9341_Display &tft, int16_t x, int16_t y, int16_t w, int16_t h,
                            int16_t radius, const char *label, uint8_t textSize,
                            uint16_t fillColor, uint16_t borderColor, uint16_t textColor) {
  drawRoundedBox(tft, x, y, w, h, radius, fillColor, borderColor);
  int16_t textW = textWidthPixels(label, textSize);
  int16_t textX = x + (w - textW) / 2;
  if (textX < x + 2) textX = x + 2;
  int16_t textY = y + (h - (8 * textSize)) / 2;
  if (textY < y + 2) textY = y + 2;
  drawText(tft, textX, textY, label, textSize, textColor, -1);
}

void drawButton(ILI9341_Display &tft, int16_t x, int16_t y, int16_t w, int16_t h,
              const char *label, bool pressed, uint8_t textSize) {
  uint16_t fill = pressed ? ILI9341_DARKGREY : ILI9341_NAVY;
  uint16_t border = pressed ? ILI9341_WHITE : ILI9341_CYAN;
  uint16_t textColor = ILI9341_WHITE;
  drawRoundedBoxWithText(tft, x, y, w, h, 6, label, textSize, fill, border, textColor);
}

void drawHeaderBar(ILI9341_Display &tft, const char *title,
                  uint16_t bgColor, uint16_t textColor) {
  int16_t h = 20;
  tft.fillRect(0, 0, tft.width(), h, bgColor);
  drawCenteredText(tft, 4, title, 2, textColor, bgColor);
}

void drawHorizontalGauge(ILI9341_Display &tft, int16_t x, int16_t y, int16_t w, int16_t h,
                        float value01, uint16_t fillColor,
                        uint16_t bgColor, uint16_t borderColor) {
  if (value01 < 0.0f) value01 = 0.0f;
  if (value01 > 1.0f) value01 = 1.0f;
  int16_t fillW = (int16_t)(w * value01);
  tft.fillRect(x, y, w, h, bgColor);
  tft.fillRect(x, y, fillW, h, fillColor);
  tft.drawRect(x, y, w, h, borderColor);
}

void drawGrid(ILI9341_Display &tft, int16_t x, int16_t y, int16_t w, int16_t h,
            int16_t xStep, int16_t yStep, uint16_t lineColor) {
  for (int16_t xx = x; xx <= x + w; xx += xStep) {
    tft.drawLine(xx, y, xx, y + h, lineColor);
  }
  for (int16_t yy = y; yy <= y + h; yy += yStep) {
    tft.drawLine(x, yy, x + w, yy, lineColor);
  }
}
void drawCrosshair(ILI9341_Display &tft, int16_t x, int16_t y,
                  int16_t size, uint16_t color) {
  tft.drawLine(x - size, y, x + size, y, color);
  tft.drawLine(x, y - size, x, y + size, color);
}

void drawTimeBox(ILI9341_Display &tft, int16_t x, int16_t y, int16_t w, int16_t h) {
  drawRoundedBox(tft, x, y, w, h, 4, ILI9341_BLACK, ILI9341_WHITE);
}

void initPlot(ILI9341_Display &tft, const PlotConfig &cfg,
            int16_t xStep, int16_t yStep) {
  tft.fillRect(cfg.x, cfg.y, cfg.w, cfg.h, cfg.bgColor);
  tft.drawRect(cfg.x, cfg.y, cfg.w, cfg.h, cfg.borderColor);
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
  const_cast<PlotConfig&>(cfg).curX = 0;
  const_cast<PlotConfig&>(cfg).hasPrev = false;
  const_cast<PlotConfig&>(cfg).prevY = 0;
}

void plotBuffer(ILI9341_Display &tft, const PlotConfig &cfg,
              const float *buffer, size_t N, uint16_t lineColor) {
  if (!buffer || N < 2) return;
  tft.fillRect(cfg.x + 1, cfg.y + 1, cfg.w - 2, cfg.h - 2, cfg.bgColor);
  float stepX;
  if (N <= 1) {
  stepX = 0;
  } 
  else {
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
  tft.drawRect(cfg.x, cfg.y, cfg.w, cfg.h, cfg.borderColor);
}

void plotNextSample(ILI9341_Display &tft, PlotConfig &cfg,
                  float newValue, uint16_t lineColor) {
  int16_t y = mapValueToY(cfg, newValue);
  int16_t x = cfg.x + cfg.curX;
  if (cfg.curX == 0) {
  tft.fillRect(cfg.x + 1, cfg.y + 1, cfg.w - 2, cfg.h - 2, cfg.bgColor);
  cfg.hasPrev = false;
  }
  tft.drawFastVLine(x, cfg.y + 1, cfg.h - 2, cfg.bgColor);
  if (cfg.hasPrev) {
  int16_t prevX = x - 1;
  tft.drawLine(prevX, cfg.prevY, x, y, lineColor);
  }
  cfg.prevY = y;
  cfg.hasPrev = true;
  cfg.curX++;
  if (cfg.curX >= cfg.w) {
  cfg.curX = 0;
  }
  tft.drawRect(cfg.x, cfg.y, cfg.w, cfg.h, cfg.borderColor);
}

void drawSpectrumBars(ILI9341_Display &tft, const PlotConfig &cfg,
                    const float *spectrum, uint16_t numBins,
                    uint16_t maxFreqHz, float yScale) {
  if (!spectrum || numBins == 0 || maxFreqHz == 0) return;
  if (yScale <= 0.0f) yScale = 1.0f;
  tft.fillRect(cfg.x + 1, cfg.y + 1, cfg.w - 2, cfg.h - 2, cfg.bgColor);
  // You'll need to define these constants or pass them as parameters
  // const float freqRes = (float)SAMPLING_FREQUENCY / (float)FFT_SIZE;
  const float freqRes = 1.0f; // Placeholder - replace with your actual value
  uint16_t numBands = maxFreqHz;
  int16_t barWidth = cfg.w / (int16_t)numBands;
  if (barWidth < 1) barWidth = 1;
  for (uint16_t f = 0; f < numBands; ++f) {
    float fLow = (float)f;
    float fHigh = (float)(f + 1);
    uint16_t binStart = (uint16_t)(fLow / freqRes);
    uint16_t binEnd = (uint16_t)(fHigh / freqRes);

    if (binStart >= numBins) continue;
    if (binEnd >= numBins) binEnd = numBins - 1;
    if (binEnd < binStart) binEnd = binStart;

    float mag = 0.0f;
    for (uint16_t b = binStart; b <= binEnd; ++b) {
      if (spectrum[b] > mag) mag = spectrum[b];
    }

    mag *= yScale;

    int16_t topY = mapValueToY(cfg, mag);
    int16_t bottomY = cfg.y + cfg.h - 1;

    if (topY < cfg.y + 1) topY = cfg.y + 1;
    if (bottomY < topY) continue;

    int16_t x0 = cfg.x + (int16_t)f * barWidth;
    int16_t x1 = x0 + barWidth - 1;
    if (x1 > cfg.x + cfg.w - 2) x1 = cfg.x + cfg.w - 2;

    tft.fillRect(x0, topY, x1 - x0 + 1, bottomY - topY + 1, ILI9341_YELLOW);
  }
  tft.drawRect(cfg.x, cfg.y, cfg.w, cfg.h, cfg.borderColor);
}

void uiUpdateStatusBar(ILI9341_Display &tft, bool isRunning) {
  const char *status = isRunning ? "Status: RUNNING" : "Status: PAUSED";
  drawFooterBar(tft, status, ILI9341_DARKGREY, ILI9341_WHITE);
}

void uiUpdateTime(ILI9341_Display &tft, const char *timeStr) {
  int16_t boxW = 70;
  int16_t boxH = 20;
  int16_t x = tft.width() - boxW;
  int16_t y = 0;
  drawTimeBox(tft, x, y, boxW, boxH);
  drawText(tft, x + 4, y + 4, timeStr, 1, ILI9341_GREEN, -1);
}

void uiInitSignalScreen(ILI9341_Display &tft, PlotConfig &plotCfg,
                      float valMin, float valMax, bool isRunning,
                      const char *modeLabel, const char *title,
                      const char *plotLabel) {
  clear(tft, ILI9341_BLACK);
  drawHeaderBar(tft, title, ILI9341_NAVY, ILI9341_WHITE);
  int16_t boxW = 70;
  int16_t boxH = 20;
  int16_t timeX = tft.width() - boxW;
  int16_t timeY = 0;
  drawTimeBox(tft, timeX, timeY, boxW, boxH);
  drawText(tft, timeX + 4, timeY + 4, "--:--", 1, ILI9341_GREEN, -1);
  int16_t btnY = 22;
  int16_t btnH = 26;
  drawButton(tft, 8, btnY, 90, btnH, "START", isRunning, 2);
  drawButton(tft, 108, btnY, 90, btnH, "STOP", !isRunning, 2);
  drawButton(tft, 208, btnY, 90, btnH, modeLabel, false, 1);
  int16_t top = btnY + btnH + 8;
  int16_t bottomMargin = 20;
  plotCfg.x = 8;
  plotCfg.y = top;
  plotCfg.w = tft.width() - 16;
  plotCfg.h = tft.height() - top - bottomMargin;
  plotCfg.valMin = valMin;
  plotCfg.valMax = valMax;
  plotCfg.bgColor = ILI9341_BLACK;
  plotCfg.borderColor = ILI9341_WHITE;
  plotCfg.gridColor = ILI9341_DARKGREY;
  initPlot(tft, plotCfg, 10, 10);
  if (plotLabel) {
  drawText(tft, plotCfg.x + 4, plotCfg.y + 4, plotLabel, 1, ILI9341_YELLOW, -1);
  }
  uiUpdateStatusBar(tft, isRunning);
}
} // namespace TFTHelpers
