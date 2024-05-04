#ifndef COLOR_HPP
#define COLOR_HPP

#include "pico/stdlib.h"

namespace color {
/**
 * @brief 24-bit RGB color
 */
class RGB {
public:
  uint8_t r;
  uint8_t g;
  uint8_t b;

  RGB();
  RGB(uint8_t r, uint8_t g, uint8_t b);

  /**
   * @brief Convert to 16-bit 565 color
   */
  uint16_t to_565();
};

/**
 * @brief HSV color
 */
class HSV {
public:
  uint16_t h;
  uint8_t s;
  uint8_t v;

  HSV();
  HSV(uint16_t h, uint8_t s, uint8_t v);

  /**
   * @brief Convert to RGB color
   */
  RGB to_rgb() const;
};

} // namespace color
#endif
