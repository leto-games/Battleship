// Format: page-major (SSD1306-native), LSB = top pixel in page
#include <stdint.h>
// Bitmap 5×5 px (1 pages, 5 bytes)
static const uint8_t __static_BM_EmptyCell__[] = {
  0b00000000,  // col 0 (top bits = rows 0..7, etc.)
  0b00001010,  // col 1
  0b00000100,  // col 2
  0b00001010,  // col 3
  0b00000000  // col 4
};
#include <Graphics/BitmapData.hpp>
BitmapData BM_EmptyCell { 5, 5, __static_BM_EmptyCell__ };
