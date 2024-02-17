#ifndef DISPLAY_H
#define DISPLAY_H

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <limits>

#include "LCD_buffer.hpp"
#include "color.hpp"

namespace display {
#define DISPLAY_UNIT float

struct LCDPoint2D {
  LCD_POINT x;
  LCD_POINT y;
};

struct Float2D {
  float x;
  float y;
};

/**
 * @brief calculate cross production of vec(1,0) and vec(2,0)
 * @param[in] x0: x coordinate of point 0
 * @param[in] y0: y coordinate of point 0
 * @param[in] x1: x coordinate of point 1
 * @param[in] y1: y coordinate of point 1
 * @param[in] x2: x coordinate of point 2
 * @param[in] y2: y coordinate of point 2
 * @return float: cross production of vec(1,0) and vec(2,0)
 */
inline float cross_prod(float x0, float y0, float x1, float y1, float x2,
                        float y2) {
  return (x1 - x0) * (y2 - y0) - (x2 - x0) * (y1 - y0);
}

/**
 * @brief calculate L2 norm of the point
 * @param[in] x: x coordinate of the point
 * @param[in] y: y coordinate of the point
 * @return float: L2 norm of the point
 */
inline float norm(float x, float y) { return std::sqrt(x * x + y * y); }

/**
 * @brief calculate L2 norm of vec(1,0)
 * @param[in] x0: x coordinate of point 0
 * @param[in] y0: y coordinate of point 0
 * @param[in] x1: x coordinate of point 1
 * @param[in] y1: y coordinate of point 1
 * @return float: L2 norm of vec(1,0)
 */
inline float distance(float x0, float y0, float x1, float y1) {
  return norm(x1 - x0, y1 - y0);
}

/**
 * @brief calculate sign of cross production of vec(1,0) and vec(2,0)
 * @param[in] x0: x coordinate of point 0
 * @param[in] y0: y coordinate of point 0
 * @param[in] x1: x coordinate of point 1
 * @param[in] y1: y coordinate of point 1
 * @param[in] x2: x coordinate of point 2
 * @param[in] y2: y coordinate of point 2
 * @return bool: true if sign of cross production of vec(1,0) and vec(2,0) is
 * positive
 */
inline bool calc_sign(Float2D p0, Float2D p1, Float2D p2) {
  static float REL_EPS = 1e-6;
  return cross_prod(p0.x, p0.y, p1.x, p1.y, p2.x, p2.y) >= -REL_EPS;
}

/**
 * @brief judge if two float numbers are approximately equal
 * @param[in] x: first number
 * @param[in] y: second number
 * @return bool: true if two numbers are approximately equal
 */
inline bool float_eq(float x, float y) {
  static float ABS_EPS = 1e-5;
  static float REL_EPS = 1e-5;
  float d = std::abs(x - y);
  if (d < ABS_EPS) {
    return true;
  }

  float m = std::max(std::min(std::abs(x), std::abs(y)),
                     std::numeric_limits<float>::min());
  return d / m < REL_EPS;
}

/**
 * @brief judge if point is in triangle
 * @param[in] p: point
 * @param[in] triangle: vertices of triangle
 * @return bool: true if p is in triangle
 */
inline bool is_in_triangle(Float2D p, Float2D *triangle) {
  bool sign_1 = calc_sign(triangle[0], triangle[1], p);
  bool sign_2 = calc_sign(triangle[1], triangle[2], p);
  bool sign_3 = calc_sign(triangle[2], triangle[0], p);

  return (sign_1 == sign_2) && (sign_1 == sign_3);
}

struct ColorCircleGeometry {
  LCD_POINT area_x_start = 0;
  LCD_POINT area_y_start = 0;
  LCD_LENGTH area_height = 0;
  LCD_LENGTH area_width = 0;

  Float2D center = {0.0, 0.0};
  DISPLAY_UNIT outer_r = 0;
  DISPLAY_UNIT inner_r = 0;
};

struct ColorCursorGeometry {
  LCD_POINT area_x_start = 0;
  LCD_POINT area_y_start = 0;
  LCD_LENGTH area_height = 0;
  LCD_LENGTH area_width = 0;

  Float2D vertices[3] = {{0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}};
};

struct ColorSelectorGeometry {
  ColorCircleGeometry circle;
  ColorCursorGeometry cursor;

  bool is_valid = false;
};

void print_color_selector_geometry(ColorSelectorGeometry *geo);

struct ColorCircleParams {
  LCD_POINT area_x_start;
  LCD_POINT area_y_start;

  DISPLAY_UNIT outer_r;
  DISPLAY_UNIT inner_r;
};
struct ColorCursorParams {
  DISPLAY_UNIT height;
  DISPLAY_UNIT width;
  LCD_COLOR color;
};

class ColorSelectorDrawer {
protected:
  ColorSelectorGeometry prev_geo;
  ColorCircleParams circle_params;
  ColorCursorParams cursor_params;
  LCD_ST7735SBuffered *LCD;
  LCD_COLOR bg_color = BLACK;

  /**
   * TODO: Fill Documentation
   * @brief draw color circle
   * @param[in] LCD: LCD object
   * @param[in] center: center coordinate of color circle
   * @param[in] outer_r: radius of outer color circle
   * @param[in] inner_r: radius of inner void circle
   */
  void draw_color_circle(LCD_ST7735SBuffered *LCD,
                         ColorCircleGeometry circle) const;

  /**
   * TODO: Fill Documentation
   */
  ColorSelectorGeometry calc_color_selector_geometry(
      LCD_ST7735S *LCD, int h, DISPLAY_UNIT x_start, DISPLAY_UNIT y_start,
      DISPLAY_UNIT circle_outer_r, DISPLAY_UNIT circle_inner_r,
      DISPLAY_UNIT cursor_height, DISPLAY_UNIT cursor_width) const;

  /**
   * @brief draw cursor for color circle
   * TODO: Fill Documentation
   */
  void draw_color_cursor(LCD_ST7735SBuffered *LCD,
                         ColorCursorGeometry cursor,
                         LCD_COLOR fg_color) const;

public:
  ColorSelectorDrawer();
  void set_circle_params(ColorCircleParams params);
  void set_cursor_params(ColorCursorParams params);
  void set_bg_color(LCD_COLOR bg_color);
  void set_lcd(LCD_ST7735SBuffered *LCD);

  /**
   * @brief draw color selector
   * TODO: Fill Documentation
   */
  void draw_color_selector(int h);
};

} // namespace display
#endif
