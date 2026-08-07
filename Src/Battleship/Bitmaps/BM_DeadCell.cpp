// Format: page-major (SSD1306-native), LSB = top pixel in page
#include <stdint.h>
// Bitmap 7×7 px (1 pages, 7 bytes)
static const uint8_t __static_BM_DeadCell__[] = {
  0b01111111,  // col 0 (top bits = rows 0..7, etc.)
  0b01011101,  // col 1
  0b01101011,  // col 2
  0b01110111,  // col 3
  0b01101011,  // col 4
  0b01011101,  // col 5
  0b01111111  // col 6
};
#include <Graphics/BitmapData.hpp>
BitmapData BM_DeadCell { 7, 7, __static_BM_DeadCell__ };
