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

/**
 * @brief geometry of color circle
 */
struct ColorCircleGeometry {
  //! x coordinate of left-up-most point
  LCD_POINT area_x_start = 0;
  //! y coordinate of left-up-most point
  LCD_POINT area_y_start = 0;
  //! height of area
  LCD_LENGTH area_height = 0;
  //! width of area
  LCD_LENGTH area_width = 0;

  //! center of circle
  Float2D center = {0.0, 0.0};
  //! radius of outer colored circle
  DISPLAY_UNIT outer_r = 0;
  //! radius of inner void circle
  DISPLAY_UNIT inner_r = 0;
};

/**
 * @brief geometry of cursor in color selector
 */
struct ColorCursorGeometry {
  //! x coordinate of left-up-most point
  LCD_POINT area_x_start = 0;
  //! y coordinate of left-up-most point
  LCD_POINT area_y_start = 0;
  //! height of area
  LCD_LENGTH area_height = 0;
  //! width of area
  LCD_LENGTH area_width = 0;

  //! coordinates of vertices of cursor
  Float2D vertices[3] = {{0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}};
};

/**
 * @brief geometry of color selector
 */
struct ColorSelectorGeometry {
  //! geometry of color circle
  ColorCircleGeometry circle;
  //! geometry of cursor
  ColorCursorGeometry cursor;

  //! true if color selector is valid
  bool is_valid = false;
};

/**
 * @brief print ColorSelectorGeometry for debug
 * @param[in] geo ColorSelectorGeometry to print
 */
void print_color_selector_geometry(ColorSelectorGeometry *geo);

/**
 * @brief parameters of color circle
 */
struct ColorCircleParams {
  //! x coordinate of left-up-most point
  LCD_POINT area_x_start;
  //! y coordinate of left-up-most point
  LCD_POINT area_y_start;

  //! radius of outer colored circle
  DISPLAY_UNIT outer_r;
  //! radius of inner void circle
  DISPLAY_UNIT inner_r;
};

/**
 * @brief parameters of cursor
 */
struct ColorCursorParams {
  //! height of cursor
  DISPLAY_UNIT height;
  //! bottom length of cursor
  DISPLAY_UNIT width;
  //! foreground color of cursor
  LCD_COLOR color;
};

/**
 * @brief drawer of color selector
 */
class ColorSelectorDrawer {
protected:
  //! geometry on previous draw
  ColorSelectorGeometry prev_geo;
  ColorCircleParams circle_params;
  ColorCursorParams cursor_params;
  //! LCD object to draw on
  LCD_ST7735SBuffered *LCD;
  //! background color
  LCD_COLOR bg_color = BLACK;
  LCD_COLOR prev_bg_color = BLACK;

  /**
   * @brief draw color circle
   * @param[in] LCD: LCD object
   * @param[in] circle: geometry of color circle
   */
  void draw_color_circle(LCD_ST7735SBuffered *LCD,
                         ColorCircleGeometry circle) const;

  /**
   * @brief calculate geometry of color selector
   * @param[out] LCD: LCD object
   * @param[in] h: hue of color
   * @param[in] x_start: x coordinate of left-up-most point
   * @param[in] y_start: y coordinate of left-up-most point
   * @param[in] circle_outer_r: radius of outer colored circle
   * @param[in] circle_inner_r: radius of inner void circle
   * @param[in] cursor_height: height of cursor
   * @param[in] cursor_width: bottom length of cursor
   * @return ColorSelectorGeometry: geometry of color selector
   */
  ColorSelectorGeometry calc_color_selector_geometry(
      LCD_ST7735S *LCD, int h, DISPLAY_UNIT x_start, DISPLAY_UNIT y_start,
      DISPLAY_UNIT circle_outer_r, DISPLAY_UNIT circle_inner_r,
      DISPLAY_UNIT cursor_height, DISPLAY_UNIT cursor_width) const;

  /**
   * @brief draw cursor for color circle
   * @param[out] LCD: LCD object
   * @param[in] cursor: geometry of cursor
   * @param[in] fg_color: foreground color
   */
  void draw_color_cursor(LCD_ST7735SBuffered *LCD, ColorCursorGeometry cursor,
                         LCD_COLOR fg_color) const;

public:
  ColorSelectorDrawer();
  /**
   * @brief set parameters for circle
   * @param[in] params: parameters of circle
   */
  void set_circle_params(ColorCircleParams params);
  /**
   * @brief set parameters for cursor
   * @param[in] params: parameters of cursor
   */
  void set_cursor_params(ColorCursorParams params);
  /**
   * @brief set background color
   * @param[in] bg_color: background color
   */
  void set_bg_color(LCD_COLOR bg_color);
  /**
   * @brief set LCD object
   * @param[in] LCD: LCD object
   */
  void set_lcd(LCD_ST7735SBuffered *LCD);

  /**
   * @brief draw color selector
   * @param[in] h: hue of color
   */
  void draw_color_selector(int h);
};

} // namespace display
#endif
