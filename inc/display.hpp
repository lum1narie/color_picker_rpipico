#ifndef DISPLAY_H
#define DISPLAY_H

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <limits>
#include <optional>

#include "LCD.h"
#include "LCD_buffer.hpp"

namespace display {
/// @brief Type definition for display coordinates and dimensions,
///        using float for precision.
#define DISPLAY_UNIT float

/// @brief Mathematical constant pi.
extern const float pi;
/// @brief Inverse of pi (1/pi).
extern const float pi_inv;
/// @brief Inverse of 180 (1/180).
extern const float f180_inv;

/**
 * @brief Structure to hold 2D coordinates using LCD_POINT type.
 */
struct LCDPoint2D {
  /// @brief X coordinate.
  LCD_POINT x;
  /// @brief Y coordinate.
  LCD_POINT y;
};

/**
 * @brief Structure to hold 2D coordinates using float type.
 */
struct Float2D {
  /// @brief X coordinate.
  float x;
  /// @brief Y coordinate.
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
 * @param[in] p0: point 0
 * @param[in] p1: point 1
 * @param[in] p2: point 2
 * @return bool: `true` if sign of cross production of vec(1,0) and vec(2,0)
 *               is positive or zero (within tolerance)
 */
inline bool calc_sign(Float2D p0, Float2D p1, Float2D p2) {
  static float REL_EPS = 1e-6;
  return cross_prod(p0.x, p0.y, p1.x, p1.y, p2.x, p2.y) >= -REL_EPS;
}

/**
 * @brief judge if two float numbers are approximately equal
 * @param[in] x: first number
 * @param[in] y: second number
 * @return bool: `true` if two numbers are approximately equal
 *               within absolute and relative tolerances
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
  return d < REL_EPS * m;
}

/**
 * @brief judge if point is in triangle
 * @param[in] p: point to check
 * @param[in] triangle: array of 3 Float2D points
 *   representing the vertices of the triangle
 * @return bool: `true` if p is inside or on the boundary of the triangle
 */
inline bool is_in_triangle(Float2D p, Float2D *triangle) {
  bool sign_1 = calc_sign(triangle[0], triangle[1], p);
  bool sign_2 = calc_sign(triangle[1], triangle[2], p);
  bool sign_3 = calc_sign(triangle[2], triangle[0], p);

  return (sign_1 == sign_2) && (sign_1 == sign_3);
}

/**
 * @brief Convert a 2D vector to a hue value (0-360 degrees).
 *        Maps angles to hue, adjusting the range.
 * @param[in] v: Input vector (Float2D).
 * @return uint: Hue value (0-360).
 */
inline uint vec_to_h(Float2D v) {
  float th = std::atan2(v.y, v.x);
  // [-180, 180]
  int th_deg = 180 * th * pi_inv;
  // (th_deg + 120) % 360
  return th_deg + (th_deg < -120 ? 480 : 120);
}

/**
 * @brief Convert a hue value (0-360 degrees) to an angle in radians.
 *        Inverse operation of the specific mapping in vec_to_h.
 * @param[in] h: Hue value (0-360).
 * @return float: Angle in radians.
 */
inline float h_to_angle(int h) { return (120.0 - (float)h) * pi * f180_inv; }

/**
 * @brief geometry of color circle
 */
struct ColorCircleGeometry {
  //! x coordinate of left-up-most point of the bounding box
  LCD_POINT area_x_start = 0;
  //! y coordinate of left-up-most point of the bounding box
  LCD_POINT area_y_start = 0;
  //! height of the bounding box
  LCD_LENGTH area_height = 0;
  //! width of the bounding box
  LCD_LENGTH area_width = 0;

  //! center of circle in float coordinates
  Float2D center = {0.0, 0.0};
  //! radius of outer colored circle
  DISPLAY_UNIT outer_r = 0;
  //! radius of inner void circle
  DISPLAY_UNIT inner_r = 0;

  bool operator==(const ColorCircleGeometry &rhs) const;
};

/**
 * @brief geometry of cursor in color selector (hue indicator)
 */
struct ColorCursorGeometry {
  //! x coordinate of left-up-most point of the bounding box
  LCD_POINT area_x_start = 0;
  //! y coordinate of left-up-most point of the bounding box
  LCD_POINT area_y_start = 0;
  //! height of the bounding box
  LCD_LENGTH area_height = 0;
  //! width of the bounding box
  LCD_LENGTH area_width = 0;

  //! coordinates of vertices of the triangular cursor
  Float2D vertices[3] = {{0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}};
};

/**
 * @brief Geometry of the Saturation/Value selection rectangle.
 */
struct ColorSVRectangleGeometry {
  //! x coordinate of left-up-most point of the bounding box
  LCD_POINT area_x_start = 0;
  //! y coordinate of left-up-most point of the bounding box
  LCD_POINT area_y_start = 0;
  //! height of the bounding box
  LCD_LENGTH area_height = 0;
  //! width of the bounding box
  LCD_LENGTH area_width = 0;

  //! Minimum x coordinate (float) of the rectangle area
  DISPLAY_UNIT x_min = 0;
  //! Maximum x coordinate (float) of the rectangle area
  DISPLAY_UNIT x_max = 0;
  //! Minimum y coordinate (float) of the rectangle area
  DISPLAY_UNIT y_min = 0;
  //! Maximum y coordinate (float) of the rectangle area
  DISPLAY_UNIT y_max = 0;
};

/**
 * @brief Geometry of the cursor within the Saturation/Value rectangle.
 */
struct ColorSVCursorGeometry {
  //! x coordinate of left-up-most point of the bounding box
  LCD_POINT area_x_start = 0;
  //! y coordinate of left-up-most point of the bounding box
  LCD_POINT area_y_start = 0;
  //! height of the bounding box
  LCD_LENGTH area_height = 0;
  //! width of the bounding box
  LCD_LENGTH area_width = 0;

  //! Center x coordinate (integer) of the cursor
  LCD_POINT x;
  //! Center y coordinate (integer) of the cursor
  LCD_POINT y;

  //! Radius of the circular cursor
  LCD_LENGTH radius;
  //! Space in the middle of the cursor crosshair (if applicable)
  LCD_LENGTH mid_space;
};

/**
 * @brief geometric definitions for the hue circle, saturation/value rectangle,
 *        and their respective cursors
 */
struct ColorSelectorGeometry {
  //! geometry of color circle (hue ring)
  ColorCircleGeometry circle;
  //! geometry of cursor (hue indicator)
  ColorCursorGeometry cursor;
  //! geometry of SV rectangle
  ColorSVRectangleGeometry sv;
  //! geometry of the cursor within the SV rectangle
  ColorSVCursorGeometry sv_cursor;
};

/**
 * @brief print ColorSelectorGeometry for debug
 * @param[in] geo: Pointer to the ColorSelectorGeometry to print
 */
void print_color_selector_geometry(ColorSelectorGeometry *geo);

/**
 * @brief parameters of color circle
 */
struct ColorCircleParams {
  //! x coordinate of left-up-most point of the circle's drawing area
  LCD_POINT area_x_start;
  //! y coordinate of left-up-most point of the circle's drawing area
  LCD_POINT area_y_start;

  //! radius of outer colored circle
  DISPLAY_UNIT outer_r;
  //! radius of inner void circle
  DISPLAY_UNIT inner_r;
};

/**
 * @brief parameters of cursor (hue indicator)
 */
struct ColorCursorParams {
  //! height of the triangular cursor
  DISPLAY_UNIT height;
  //! bottom length (width) of the triangular cursor
  DISPLAY_UNIT width;
  //! foreground color of the cursor
  LCD_COLOR color;
};

/**
 * @brief Parameters for the Saturation/Value cursor.
 */
struct ColorSVCursorParams {
  //! Radius of the circular cursor.
  LCD_LENGTH radius;
  //! Space in the middle of the cursor crosshair (if applicable).
  LCD_LENGTH mid_space;
};

/**
 * @brief drawer of color selector UI element.
 *        Manages the drawing of the hue circle, SV rectangle, and cursors.
 */
class ColorSelectorDrawer {
protected:
  //! geometry on previous draw, used for optimization
  //! (clearing previous state)
  std::optional<ColorSelectorGeometry> prev_geo = std::nullopt;
  //! Parameters defining the hue circle geometry and position.
  ColorCircleParams circle_params;
  //! Parameters defining the hue cursor appearance.
  ColorCursorParams cursor_params;
  //! Parameters defining the SV cursor appearance.
  ColorSVCursorParams sv_cursor_params;
  //! Reference to the buffered LCD object to draw on.
  LCD_ST7735SBuffered &lcd;
  //! Background color used for clearing areas.
  LCD_COLOR bg_color = BLACK;
  //! Previous background color, used for optimization.
  std::optional<LCD_COLOR> prev_bg_color = std::nullopt;

  /**
   * @brief draw color circle (hue ring)
   * @param[out] LCD: LCD object to draw on
   * @param[in] circle: geometry of color circle
   */
  void draw_color_circle(LCD_ST7735SBuffered &LCD,
                         ColorCircleGeometry circle) const;

  /**
   * @brief calculate geometry of the entire color selector UI element
   * @param[in] LCD: LCD object (used for screen dimensions)
   * @param[in] h: hue of color (0-360)
   * @param[in] s: saturation of color (0-255)
   * @param[in] v: value (brightness) of color (0-255)
   * @param[in] x_start: x coordinate of left-up-most point
   *                     the hue circle area
   * @param[in] y_start: y coordinate of left-up-most point
   *                     the hue circle area
   * @param[in] circle_outer_r: radius of outer colored circle
   * @param[in] circle_inner_r: radius of inner void circle
   * @param[in] cursor_height: height of the hue cursor
   * @param[in] cursor_width: bottom length of the hue cursor
   * @param[in] sv_cursor_radius: radius of the SV cursor
   * @return std::optional<ColorSelectorGeometry>: geometry of color selector,
   *                                               or std::nullopt if parameters are invalid
   */
  std::optional<ColorSelectorGeometry> calc_color_selector_geometry(
      const LCD_ST7735S &LCD, int h, int s, int v, DISPLAY_UNIT x_start,
      DISPLAY_UNIT y_start, DISPLAY_UNIT circle_outer_r,
      DISPLAY_UNIT circle_inner_r, DISPLAY_UNIT cursor_height,
      DISPLAY_UNIT cursor_width, LCD_LENGTH sv_cursor_radius) const;

  /**
   * @brief draw cursor for color circle (hue indicator)
   * @param[out] LCD: LCD object to draw on
   * @param[in] cursor: geometry of cursor
   * @param[in] fg_color: foreground color for the cursor outline
   */
  void draw_color_cursor(LCD_ST7735SBuffered &LCD, ColorCursorGeometry cursor,
                         LCD_COLOR fg_color) const;

  /**
   * @brief Draw the Saturation/Value selection rectangle.
   * @param[out] LCD: LCD object to draw on.
   * @param[in] rect: Geometry of the SV rectangle.
   * @param[in] h: Current hue value (0-360), used to determine
   *               the rectangle's gradient.
   */
  void draw_color_svrect(LCD_ST7735SBuffered &LCD,
                         ColorSVRectangleGeometry rect, uint h) const;

  /**
   * @brief Draw the cursor within the Saturation/Value rectangle.
   * @param[out] LCD: LCD object to draw on.
   * @param[in] cursor: Geometry and position of the SV cursor.
   */
  void draw_color_svrect_cursor(LCD_ST7735SBuffered &LCD,
                                ColorSVCursorGeometry cursor) const;

public:
  /**
   * @brief Construct a new Color Selector Drawer object.
   * @param[out] lcd: Reference to the buffered LCD object.
   */
  ColorSelectorDrawer(LCD_ST7735SBuffered &lcd);
  /**
   * @brief set parameters for circle
   * @param[in] params: parameters of circle
   */
  void set_circle_params(ColorCircleParams params);
  /**
   * @brief set parameters for cursor (hue indicator)
   * @param[in] params: parameters of cursor
   */
  void set_cursor_params(ColorCursorParams params);
  /**
   * @brief Set parameters for the Saturation/Value cursor.
   * @param[in] params: Parameters defining the SV cursor appearance.
   */
  void set_sv_cursor_params(ColorSVCursorParams params);
  /**
   * @brief set background color and clear the LCD if the color changes.
   * @param[in] bg_color: background color
   */
  void set_bg_color(LCD_COLOR bg_color);

  /**
   * @brief draw the complete color selector UI element based on current HSV
   * values. Calculates geometry, clears previous drawing areas if needed, and
   * draws the hue circle, SV rectangle, and cursors.
   * @param[in] h: hue of color (0-360)
   * @param[in] s: saturation of color (0-255)
   * @param[in] v: value (brightness) of color (0-255)
   */
  void draw_color_selector(int h, int s, int v);
};

} // namespace display
#endif
