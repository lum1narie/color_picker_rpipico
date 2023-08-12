#ifndef COLOR_HPP
#define COLOR_HPP

#include "pico/stdlib.h"

namespace color {
class RGB {
public:
  uint8_t r;
  uint8_t g;
  uint8_t b;

  RGB();
  RGB(uint8_t r, uint8_t g, uint8_t b);

  uint16_t to_565();
};

class HSV {
public:
  uint16_t h;
  uint8_t s;
  uint8_t v;

  HSV();
  HSV(uint16_t h, uint8_t s, uint8_t v);

  RGB to_rgb() const;
};

} // namespace color
#endif
