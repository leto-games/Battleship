// Format: page-major (SSD1306-native), LSB = top pixel in page
#include <stdint.h>
// Bitmap 14×12 px (2 pages, 28 bytes)
static const uint8_t __static_BM_Death__[] = {
  0b11111100, 0b00000000,  // col 0 (top bits = rows 0..7, etc.)
  0b11111110, 0b00000001,  // col 1
  0b11011111, 0b00000001,  // col 2
  0b10001111, 0b00000001,  // col 3
  0b10001111, 0b00001111,  // col 4
  0b11011111, 0b00000011,  // col 5
  0b01111111, 0b00001110,  // col 6
  0b01111111, 0b00001110,  // col 7
  0b11011111, 0b00000011,  // col 8
  0b10001111, 0b00001111,  // col 9
  0b10001111, 0b00000001,  // col 10
  0b11011111, 0b00000001,  // col 11
  0b11111110, 0b00000001,  // col 12
  0b11111100, 0b00000000  // col 13
};
#include <Graphics/BitmapData.hpp>
BitmapData BM_Death { 14, 12, __static_BM_Death__ };
