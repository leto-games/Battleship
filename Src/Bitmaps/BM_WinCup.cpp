// Format: page-major (SSD1306-native), LSB = top pixel in page
#include <stdint.h>
// Bitmap 17×13 px (2 pages, 34 bytes)
static const uint8_t __static_BM_WinCup__[] = {
  0b00011100, 0b00000000,  // col 0 (top bits = rows 0..7, etc.)
  0b00100010, 0b00000000,  // col 1
  0b01000100, 0b00000000,  // col 2
  0b10000101, 0b00000000,  // col 3
  0b11111111, 0b00000000,  // col 4
  0b11111111, 0b00010000,  // col 5
  0b11111111, 0b00011001,  // col 6
  0b11111111, 0b00011111,  // col 7
  0b11111111, 0b00011111,  // col 8
  0b11111111, 0b00011111,  // col 9
  0b11111111, 0b00011001,  // col 10
  0b11111111, 0b00010000,  // col 11
  0b11111111, 0b00000000,  // col 12
  0b10000101, 0b00000000,  // col 13
  0b01000100, 0b00000000,  // col 14
  0b00100010, 0b00000000,  // col 15
  0b00011100, 0b00000000  // col 16
};
#include <Graphics/BitmapData.hpp>
BitmapData BM_WinCup { 17, 13, __static_BM_WinCup__ };
