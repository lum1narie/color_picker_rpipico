#ifndef COLOR_HPP
#define COLOR_HPP

#include "pico/stdlib.h"

namespace color {
/**
 * @brief Represents a color in the RGB (Red, Green, Blue) color space
 *        using 8 bits per channel.
 */
class RGB {
public:
  /** @brief Red component (0-255). */
  uint8_t r;
  /** @brief Green component (0-255). */
  uint8_t g;
  /** @brief Blue component (0-255). */
  uint8_t b;

  /**
   * @brief Default constructor. Initializes components to 0 (black).
   */
  RGB();
  /**
   * @brief Construct a new RGB object with specified component values.
   * @param r Red component (0-255).
   * @param g Green component (0-255).
   * @param b Blue component (0-255).
   */
  RGB(uint8_t r, uint8_t g, uint8_t b);

  /**
   * @brief Convert the RGB color to a 16-bit RGB565 format.
   *        (5 bits for Red, 6 bits for Green, 5 bits for Blue).
   * @return uint16_t The color value in RGB565 format.
   */
  uint16_t to_565();
};

/**
 * @brief Represents a color in the HSV (Hue, Saturation, Value) color space.
 */
class HSV {
public:
  /** @brief Hue component (0-360 degrees). */
  uint16_t h;
  /** @brief Saturation component (0-255). */
  uint8_t s;
  /** @brief Value (brightness) component (0-255). */
  uint8_t v;

  /**
   * @brief Default constructor. Initializes components to 0.
   */
  HSV();
  /**
   * @brief Construct a new HSV object with specified component values.
   * @param h Hue component (0-360).
   * @param s Saturation component (0-255).
   * @param v Value component (0-255).
   */
  HSV(uint16_t h, uint8_t s, uint8_t v);

  /**
   * @brief Convert the HSV color to its equivalent RGB representation
   *        (using floating-point calculations).
   * @return RGB The equivalent color in the RGB color space.
   */
  RGB to_rgb() const;

  /**
   * @brief Convert the HSV color to its equivalent RGB representation
   *        (using approximate integer calculations).
   *        This version might be faster but less accurate than to_rgb().
   * @return RGB The approximately equivalent color in the RGB color space.
   */
  RGB to_rgb_approx() const;
};

} // namespace color
#endif
