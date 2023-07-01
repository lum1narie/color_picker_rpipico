#include "color.hpp"
#include "pico/stdlib.h"

#include "LCD_buffer.hpp"

namespace color {
RGB::RGB() {}
RGB::RGB(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b) {}

uint16_t RGB::to_565() {
  return (((uint16_t)r & 0xF8) << 8) | (((uint16_t)g & 0xFC) << 3) |
         ((uint16_t)b >> 3);
}

HSV::HSV() {}
HSV::HSV(uint16_t h, uint8_t s, uint8_t v) : h(h), s(s), v(v) {}

RGB HSV::to_rgb() const {
  uint8_t val_max = v;
  uint8_t val_min = (uint16_t)val_max * (uint16_t)(255 - s) / 255;
  uint8_t val_mid;

  if (h > 360) {
    return RGB(val_max, val_max, val_max);
  }

  if (h < 60) {
    val_mid = (uint16_t)(val_max - val_min) * h / 60 + val_min;
    return RGB(val_max, val_mid, val_min);
  } else if (h < 120) {
    val_mid = (uint16_t)(val_max - val_min) * (120 - h) / 60 + val_min;
    return RGB(val_mid, val_max, val_min);
  } else if (h < 180) {
    val_mid = (uint16_t)(val_max - val_min) * (h - 120) / 60 + val_min;
    return RGB(val_min, val_max, val_mid);
  } else if (h < 240) {
    val_mid = (uint16_t)(val_max - val_min) * (240 - h) / 60 + val_min;
    return RGB(val_min, val_mid, val_max);
  } else if (h < 300) {
    val_mid = (uint16_t)(val_max - val_min) * (h - 240) / 60 + val_min;
    return RGB(val_mid, val_min, val_max);
  } else {
    val_mid = (uint16_t)(val_max - val_min) * (360 - h) / 60 + val_min;
    return RGB(val_max, val_min, val_mid);
  }
};
} // namespace color
