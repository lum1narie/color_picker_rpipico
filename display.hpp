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

struct LCD_Point_2D {
  LCD_POINT x;
  LCD_POINT y;
};

struct Float_2D {
  float x;
  float y;
};

/**
 * TODO: write documentation
 */
inline float crop(float x, float l, float r) {
  if (x < l) {
    return l;
  } else if (x > r) {
    return r;
  } else {
    return x;
  }
}

/**
 * TODO: write documentation
 */
inline float cross_prod(float x0, float y0, float x1, float y1, float x2,
                        float y2) {
  return (x1 - x0) * (y2 - y0) - (x2 - x0) * (y1 - y0);
}

/**
 * TODO: write documentation
 */
inline float norm(float x, float y) { return std::sqrt(x * x + y * y); }

/**
 * TODO: write documentation
 */
inline float distance(float x0, float y0, float x1, float y1) {
  return norm(x1 - x0, y1 - y0);
}

inline float calc_sign(Float_2D p0, Float_2D p1, Float_2D p2) {
  static float REL_EPS = 1e-3;
  float d0 = distance(p0.x, p0.y, p1.x, p1.y);
  float d1 = distance(p0.x, p0.y, p2.x, p2.y);

  if (d0 == 0 || d1 == 0) {
    return true;
  }

  return cross_prod(p0.x, p0.y, p1.x, p1.y, p2.x, p2.y) >= -REL_EPS * d0 * d1;
}

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
 * TODO: write documentation
 */
inline bool is_in_triangle(Float_2D p, Float_2D *triangle) {
  bool sign_1 = calc_sign(triangle[0], triangle[1], p);
  bool sign_2 = calc_sign(triangle[1], triangle[2], p);
  bool sign_3 = calc_sign(triangle[2], triangle[0], p);

  return (sign_1 == sign_2) && (sign_1 == sign_3);
}

struct Color_selector_geometry {
  LCD_POINT cursor_area_x_start;
  LCD_POINT cursor_area_y_start;
  LCD_LENGTH cursor_area_height;
  LCD_LENGTH cursor_area_width;

  Float_2D cursor_vertices[3];

  LCD_POINT circle_area_x_start;
  LCD_POINT circle_area_y_start;
  LCD_LENGTH circle_area_height;
  LCD_LENGTH circle_area_width;

  Float_2D circle_center;
  DISPLAY_UNIT circle_outer_r;
  DISPLAY_UNIT circle_inner_r;

  bool is_valid;
};

void print_color_selector_geometry(Color_selector_geometry *geo);

/**
 * @brief draw color circle
 * TODO: Fill Documentation
 */
void draw_color_circle(LCD_ST7735S_buffered *LCD, Float_2D center,
                       DISPLAY_UNIT outer_r, DISPLAY_UNIT inner_r,
                       LCD_POINT x_start, LCD_POINT y_start,
                       LCD_LENGTH area_height, LCD_LENGTH area_width);

/**
 * TODO: Fill Documentation
 */
Color_selector_geometry calc_color_selector_geometry(
    LCD_ST7735S *LCD, int h, DISPLAY_UNIT x_start, DISPLAY_UNIT y_start,
    DISPLAY_UNIT circle_outer_r, DISPLAY_UNIT circle_inner_r,
    DISPLAY_UNIT cursor_height, DISPLAY_UNIT cursor_width);

/**
 * @brief draw cursor for color circle
 * TODO: Fill Documentation
 */
void draw_color_cursor(LCD_ST7735S_buffered *LCD, LCD_POINT x_start,
                       LCD_POINT y_start, LCD_LENGTH area_height,
                       LCD_LENGTH area_width, Float_2D *vertices,
                       LCD_COLOR fg_color);

extern Color_selector_geometry prev_geo;

/**
 * @brief draw color selector
 * TODO: Fill Documentation
 */
void draw_color_selector(LCD_ST7735S_buffered *LCD, int h, DISPLAY_UNIT x_start,
                         DISPLAY_UNIT y_start, DISPLAY_UNIT outer_r,
                         DISPLAY_UNIT inner_r, DISPLAY_UNIT cursor_height,
                         DISPLAY_UNIT cursor_width, LCD_COLOR cursor_color,
                         LCD_COLOR bg_color);
} // namespace display
#endif
