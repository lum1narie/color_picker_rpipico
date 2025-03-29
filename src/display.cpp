#include "display.hpp"
#include "LCD1in8/inc/LCD.h"
#include "LCD_buffer.hpp"
#include "color.hpp"
#include <cstdarg>

namespace display {
const float pi = std::acos(-1.0);
const float pi_inv = 1.0f / pi;
const float f180_inv = 1.0f / 180.f;

bool ColorCircleGeometry::operator==(const ColorCircleGeometry &rhs) const {
  return float_eq(this->center.x, rhs.center.x) &&
         float_eq(this->center.y, rhs.center.y) &&
         float_eq(this->outer_r, rhs.outer_r) &&
         float_eq(this->inner_r, rhs.inner_r);
}

void print_color_selector_geometry(ColorSelectorGeometry *geo) {
  printf("cursor:\n"
         "\tarea: (%d, %d) -> w: %d, h: %d\n"
         "\tvertices:\n"
         "\t\t(%f, %f)\n"
         "\t\t(%f, %f)\n"
         "\t\t(%f, %f)\n"
         "circle:\n"
         "\tarea: (%d, %d) -> w: %d, h: %d\n"
         "\tcenter: (%f, %f)\n"
         "\tr: %f, inner: %f\n",
         geo->cursor.area_x_start, geo->cursor.area_y_start,
         geo->cursor.area_width, geo->cursor.area_height,
         geo->cursor.vertices[0].x, geo->cursor.vertices[0].y,
         geo->cursor.vertices[1].x, geo->cursor.vertices[1].y,
         geo->cursor.vertices[2].x, geo->cursor.vertices[2].y,
         geo->circle.area_x_start, geo->circle.area_y_start,
         geo->circle.area_width, geo->circle.area_height, geo->circle.center.x,
         geo->circle.center.y, geo->circle.outer_r, geo->circle.inner_r);
}

ColorSelectorDrawer::ColorSelectorDrawer(LCD_ST7735SBuffered &lcd) : lcd(lcd) {}

void ColorSelectorDrawer::set_circle_params(ColorCircleParams params) {
  this->circle_params = params;
}

void ColorSelectorDrawer::set_cursor_params(ColorCursorParams params) {
  this->cursor_params = params;
}

void ColorSelectorDrawer::set_sv_cursor_params(ColorSVCursorParams params) {
  this->sv_cursor_params = params;
}

void ColorSelectorDrawer::set_bg_color(LCD_COLOR bg_color) {
  this->bg_color = bg_color;
  this->lcd.LCD_Clear(bg_color);
}

void ColorSelectorDrawer::draw_color_circle(LCD_ST7735SBuffered &LCD,
                                            ColorCircleGeometry circle) const {
  // set inner_r as half as outer_r if invalid value is given
  if (circle.inner_r >= circle.outer_r) {
    circle.inner_r = circle.outer_r * 0.75;
  }

  // cut off area size when circle go out from display
  LCD_LENGTH x_siz =
      std::min(circle.area_width,
               (LCD_LENGTH)(LCD.sLCD_DIS.LCD_Dis_Column - circle.area_x_start));
  LCD_LENGTH y_siz =
      std::min(circle.area_height,
               (LCD_LENGTH)(LCD.sLCD_DIS.LCD_Dis_Page - circle.area_y_start));

  // calculate the color of each point
  for (uint i = 0; i < y_siz; ++i) {
    for (uint j = 0; j < x_siz; ++j) {
      LCD_POINT x = circle.area_x_start + j;
      LCD_POINT y = circle.area_y_start + i;
      // assume the center is origin
      float x_from_center = (float)x - circle.center.x;
      float y_from_center = (float)y - circle.center.y;

      if (float_eq(x_from_center, 0.0f) && float_eq(y_from_center, 0.0f)) {
        continue;
      }

      // deal point in polar coordinates
      float r =
          sqrt(x_from_center * x_from_center + y_from_center * y_from_center);
      if (r < circle.inner_r || r > circle.outer_r) {
        continue;
      }

      uint16_t h = vec_to_h({x_from_center, y_from_center});
      LCD_COLOR c = color::HSV(h, 0xFF, 0xFF).to_rgb_approx().to_565();
      LCD.LCD_SetPointlColor(x, y, c);
    }
  }
}

ColorCircleGeometry calc_color_circle_geometry(const LCD_ST7735S &LCD,
                                               float x_start, float y_start,
                                               DISPLAY_UNIT circle_outer_r,
                                               DISPLAY_UNIT circle_inner_r) {
  ColorCircleGeometry retv;
  constexpr float ROUND_EPS = 1e-4;

  retv.center = {x_start + circle_outer_r, y_start + circle_outer_r};

  retv.inner_r = circle_inner_r;
  retv.outer_r = circle_outer_r;
  retv.area_x_start =
      std::clamp(x_start, 0.0f, (float)LCD.sLCD_DIS.LCD_Dis_Column);
  retv.area_y_start =
      std::clamp(y_start, 0.0f, (float)LCD.sLCD_DIS.LCD_Dis_Column);

  LCD_POINT circle_x_end =
      std::ceil(std::clamp(x_start + circle_outer_r * 2.0f + 1.0f, 0.0f,
                           (float)LCD.sLCD_DIS.LCD_Dis_Column) -
                ROUND_EPS);
  LCD_POINT circle_y_end =
      std::ceil(std::clamp(y_start + circle_outer_r * 2.0f + 1.0f, 0.0f,
                           (float)LCD.sLCD_DIS.LCD_Dis_Page) -
                ROUND_EPS);

  retv.area_width = circle_x_end - retv.area_x_start;
  retv.area_height = circle_y_end - retv.area_y_start;

  return retv;
}

ColorCursorGeometry calc_color_cursor_geometry(const LCD_ST7735S &LCD, int h,
                                               Float2D circle_center,
                                               DISPLAY_UNIT circle_outer_r,
                                               DISPLAY_UNIT cursor_height,
                                               DISPLAY_UNIT cursor_width) {
  ColorCursorGeometry retv;
  constexpr float ROUND_EPS = 1e-4;
  float th = h_to_angle(h);
  float cos_th = std::cos(th);
  float sin_th = std::sin(th);

  float v0_x_init_from_center = circle_outer_r;
  float v0_y_init_from_center = 0;
  float v1_x_init_from_center = circle_outer_r + cursor_height;
  float v1_y_init_from_center = -cursor_width;
  float v2_x_init_from_center = circle_outer_r + cursor_height;
  float v2_y_init_from_center = cursor_width;

  retv.vertices[0].x = circle_center.x + v0_x_init_from_center * cos_th -
                       v0_y_init_from_center * sin_th;
  retv.vertices[0].y = circle_center.y - (v0_x_init_from_center * sin_th +
                                          v0_y_init_from_center * cos_th);
  retv.vertices[1].x = circle_center.x + v1_x_init_from_center * cos_th -
                       v1_y_init_from_center * sin_th;
  retv.vertices[1].y = circle_center.y - (v1_x_init_from_center * sin_th +
                                          v1_y_init_from_center * cos_th);
  retv.vertices[2].x = circle_center.x + v2_x_init_from_center * cos_th -
                       v2_y_init_from_center * sin_th;
  retv.vertices[2].y = circle_center.y - (v2_x_init_from_center * sin_th +
                                          v2_y_init_from_center * cos_th);

  retv.area_x_start =
      std::floor(std::clamp(std::min({retv.vertices[0].x, retv.vertices[1].x,
                                      retv.vertices[2].x}),
                            0.0f, (float)LCD.sLCD_DIS.LCD_Dis_Column) +
                 ROUND_EPS);
  retv.area_y_start =
      std::floor(std::clamp(std::min({retv.vertices[0].y, retv.vertices[1].y,
                                      retv.vertices[2].y}),
                            0.0f, (float)LCD.sLCD_DIS.LCD_Dis_Page) +
                 ROUND_EPS);
  LCD_POINT cursor_x_end =
      std::ceil(std::clamp(std::max({retv.vertices[0].x, retv.vertices[1].x,
                                     retv.vertices[2].x}) +
                               1.0f,
                           0.0f, (float)LCD.sLCD_DIS.LCD_Dis_Column) -
                ROUND_EPS);
  LCD_POINT cursor_y_end =
      std::ceil(std::clamp(std::max({retv.vertices[0].y, retv.vertices[1].y,
                                     retv.vertices[2].y}) +
                               1.0f,
                           0.0f, (float)LCD.sLCD_DIS.LCD_Dis_Page) -
                ROUND_EPS);

  retv.area_width = cursor_x_end - retv.area_x_start;
  retv.area_height = cursor_y_end - retv.area_y_start;

  return retv;
}

ColorSVRectangleGeometry calc_color_sv_geometry(const LCD_ST7735S &LCD,
                                                Float2D circle_center,
                                                DISPLAY_UNIT circle_inner_r) {
  ColorSVRectangleGeometry retv;
  constexpr float ROUND_EPS = 1e-4;
  static const float sqrt_2_inv = std::sqrt(0.5f);

  retv.x_min = circle_center.x - circle_inner_r * sqrt_2_inv;
  retv.x_max = circle_center.x + circle_inner_r * sqrt_2_inv;
  retv.y_min = circle_center.y - circle_inner_r * sqrt_2_inv;
  retv.y_max = circle_center.y + circle_inner_r * sqrt_2_inv;

  retv.area_x_start = std::ceil(
      std::clamp(retv.x_min, 0.0f, (float)LCD.sLCD_DIS.LCD_Dis_Column) -
      ROUND_EPS);
  retv.area_y_start =
      std::ceil(std::clamp(retv.y_min, 0.0f, (float)LCD.sLCD_DIS.LCD_Dis_Page) -
                ROUND_EPS);
  LCD_POINT sv_x_end =
      std::clamp(retv.x_max + 1.0f, 0.0f, (float)LCD.sLCD_DIS.LCD_Dis_Column);
  LCD_POINT sv_y_end =
      std::clamp(retv.y_max + 1.0f, 0.0f, (float)LCD.sLCD_DIS.LCD_Dis_Page);
  retv.area_width = sv_x_end - retv.area_x_start;
  retv.area_height = sv_y_end - retv.area_y_start;

  return retv;
}
ColorSVCursorGeometry
calc_color_sv_cursor_geometry(const LCD_ST7735S &LCD,
                              const ColorSVRectangleGeometry &sv_geometry,
                              DISPLAY_UNIT cursor_radius, int s, int v) {
  ColorSVCursorGeometry retv;
  constexpr float ROUND_EPS = 1e-4;

  DISPLAY_UNIT sv_cursor_x =
      (sv_geometry.x_max * (float)s + sv_geometry.x_min * (255.0f - (float)s)) /
      255.0f;
  DISPLAY_UNIT sv_cursor_y =
      (sv_geometry.y_min * (float)v + sv_geometry.y_max * (255.0f - (float)v)) /
      255.0f;
  retv.x = std::clamp(sv_cursor_x, 0.0f, (float)LCD.sLCD_DIS.LCD_Dis_Column);
  retv.y = std::clamp(sv_cursor_y, 0.0f, (float)LCD.sLCD_DIS.LCD_Dis_Page);
  retv.area_x_start = std::clamp(sv_cursor_x - cursor_radius - 1.0f, 0.0f,
                                 (float)LCD.sLCD_DIS.LCD_Dis_Column);
  retv.area_y_start = std::clamp(sv_cursor_y - cursor_radius - 1.0f, 0.0f,
                                 (float)LCD.sLCD_DIS.LCD_Dis_Page);

  LCD_POINT sv_cursor_x_end =
      std::clamp(std::ceil(sv_cursor_x + cursor_radius + 1.0f - ROUND_EPS),
                 0.0f, (float)LCD.sLCD_DIS.LCD_Dis_Column);
  LCD_POINT sv_cursor_y_end =
      std::clamp(std::ceil(sv_cursor_y + cursor_radius + 1.0f - ROUND_EPS),
                 0.0f, (float)LCD.sLCD_DIS.LCD_Dis_Page);
  retv.area_width = sv_cursor_x_end - retv.area_x_start;
  retv.area_height = sv_cursor_y_end - retv.area_y_start;

  return retv;
}

std::optional<ColorSelectorGeometry>
ColorSelectorDrawer::calc_color_selector_geometry(
    const LCD_ST7735S &LCD, int h, int s, int v, DISPLAY_UNIT x_start,
    DISPLAY_UNIT y_start, DISPLAY_UNIT circle_outer_r,
    DISPLAY_UNIT circle_inner_r, DISPLAY_UNIT cursor_height,
    DISPLAY_UNIT cursor_width, LCD_LENGTH sv_cursor_radius) const {
  ColorSelectorGeometry retv;

  if (circle_inner_r >= circle_outer_r || circle_inner_r < 0 ||
      cursor_height <= 0 || cursor_width <= 0) {
    return std::nullopt;
  }

  retv.circle = calc_color_circle_geometry(LCD, x_start, y_start,
                                           circle_outer_r, circle_inner_r);
  retv.cursor = calc_color_cursor_geometry(
      LCD, h, retv.circle.center, circle_outer_r, cursor_height, cursor_width);
  retv.sv = calc_color_sv_geometry(LCD, retv.circle.center, circle_inner_r);
  retv.sv_cursor =
      calc_color_sv_cursor_geometry(LCD, retv.sv, sv_cursor_radius, s, v);

  return retv;
}

void ColorSelectorDrawer::draw_color_cursor(LCD_ST7735SBuffered &LCD,
                                            ColorCursorGeometry cursor,
                                            LCD_COLOR fg_color) const {
  LCD_LENGTH x_siz =
      std::min(cursor.area_width,
               (LCD_LENGTH)(LCD.sLCD_DIS.LCD_Dis_Column - cursor.area_x_start));
  LCD_LENGTH y_siz =
      std::min(cursor.area_height,
               (LCD_LENGTH)(LCD.sLCD_DIS.LCD_Dis_Page - cursor.area_y_start));

  // calculate the color of each point
  for (LCD_POINT i = 0; i < y_siz; ++i) {
    LCD_POINT y = cursor.area_y_start + i;
    for (uint j = 0; j < x_siz; ++j) {
      LCD_POINT x = cursor.area_x_start + j;
      // check point if it is inside of cursor
      Float2D p = Float2D{(float)x, (float)y};
      if (is_in_triangle(p, cursor.vertices)) {
        LCD.LCD_SetPointlColor(x, y, fg_color);
      }
    }
  }
}

void ColorSelectorDrawer::draw_color_svrect(LCD_ST7735SBuffered &LCD,
                                            ColorSVRectangleGeometry rect,
                                            uint h) const {
  h %= 360;

  for (LCD_POINT i = 0; i < rect.area_height; ++i) {
    LCD_POINT y = rect.area_y_start + i;
    float y_ratio = std::clamp(
        (rect.y_max - (float)y) / (rect.y_max - rect.y_min), 0.0f, 1.0f);
    for (LCD_POINT j = 0; j < rect.area_width; ++j) {
      LCD_POINT x = rect.area_x_start + j;
      float x_ratio = std::clamp(
          ((float)x - rect.x_min) / (rect.x_max - rect.x_min), 0.0f, 1.0f);
      uint s = 255.0f * x_ratio;
      uint v = 255.0f * y_ratio;
      LCD.LCD_SetPointlColor(x, y, color::HSV(h, s, v).to_rgb_approx().to_565());
    }
  }
}

void ColorSelectorDrawer::draw_color_svrect_cursor(
    LCD_ST7735SBuffered &LCD, ColorSVCursorGeometry cursor) const {
  const uint RADIUS = 6;
  const uint MID_SPACE = 3;
  LCD_POINT x_left_0 =
      std::clamp((int)(cursor.x - RADIUS), 0, (int)LCD.sLCD_DIS.LCD_Dis_Column);
  LCD_POINT x_left_1 = std::clamp((int)(cursor.x - MID_SPACE), 0,
                                  (int)LCD.sLCD_DIS.LCD_Dis_Column);
  LCD_POINT x_right_0 =
      std::clamp((int)(cursor.x + RADIUS), 0, (int)LCD.sLCD_DIS.LCD_Dis_Column);
  LCD_POINT x_right_1 = std::clamp((int)(cursor.x + MID_SPACE), 0,
                                   (int)LCD.sLCD_DIS.LCD_Dis_Column);
  LCD_POINT y_up_0 =
      std::clamp((int)(cursor.y - RADIUS), 0, (int)LCD.sLCD_DIS.LCD_Dis_Page);
  LCD_POINT y_up_1 = std::clamp((int)(cursor.y - MID_SPACE), 0,
                                (int)LCD.sLCD_DIS.LCD_Dis_Page);
  LCD_POINT y_down_0 =
      std::clamp((int)(cursor.y + RADIUS), 0, (int)LCD.sLCD_DIS.LCD_Dis_Page);
  LCD_POINT y_down_1 = std::clamp((int)(cursor.y + MID_SPACE), 0,
                                  (int)LCD.sLCD_DIS.LCD_Dis_Page);

  LCD.LCD_DrawLine(x_left_0, cursor.y, x_left_1, cursor.y, BLACK, LINE_SOLID,
                   DOT_PIXEL_2X2);
  LCD.LCD_DrawLine(x_left_0, cursor.y, x_left_1, cursor.y, WHITE, LINE_SOLID,
                   DOT_PIXEL_1X1);
  LCD.LCD_DrawLine(x_right_1, cursor.y, x_right_0, cursor.y, BLACK, LINE_SOLID,
                   DOT_PIXEL_2X2);
  LCD.LCD_DrawLine(x_right_1, cursor.y, x_right_0, cursor.y, WHITE, LINE_SOLID,
                   DOT_PIXEL_1X1);
  LCD.LCD_DrawLine(cursor.x, y_up_0, cursor.x, y_up_1, BLACK, LINE_SOLID,
                   DOT_PIXEL_2X2);
  LCD.LCD_DrawLine(cursor.x, y_up_0, cursor.x, y_up_1, WHITE, LINE_SOLID,
                   DOT_PIXEL_1X1);
  LCD.LCD_DrawLine(cursor.x, y_down_1, cursor.x, y_down_0, BLACK, LINE_SOLID,
                   DOT_PIXEL_2X2);
  LCD.LCD_DrawLine(cursor.x, y_down_1, cursor.x, y_down_0, WHITE, LINE_SOLID,
                   DOT_PIXEL_1X1);

  LCD.LCD_DrawCircle(cursor.x, cursor.y, RADIUS, BLACK, DRAW_EMPTY,
                     DOT_PIXEL_2X2);
  LCD.LCD_DrawCircle(cursor.x, cursor.y, RADIUS, WHITE, DRAW_EMPTY,
                     DOT_PIXEL_1X1);
}

void ColorSelectorDrawer::draw_color_selector(int h, int s, int v) {
  // calc cursor geometry
  std::optional<ColorSelectorGeometry> opt_geo =
      this->calc_color_selector_geometry(
          this->lcd, h, s, v, this->circle_params.area_x_start,
          this->circle_params.area_y_start, this->circle_params.outer_r,
          this->circle_params.inner_r, this->cursor_params.height,
          this->cursor_params.width, this->sv_cursor_params.radius);
  if (!opt_geo.has_value()) {
    printf("invalid geometry!!\n");
    return;
  }

  ColorSelectorGeometry geo = opt_geo.value();
  bool is_cleared = this->bg_color != this->prev_bg_color;
  if (is_cleared) {
    this->draw_color_circle(this->lcd, geo.circle);
  } else {
    bool is_same_circle =
        geo.circle == (opt_geo.has_value()
                           ? std::optional(opt_geo.value().circle)
                           : std::nullopt);

    // erase old cursor
    if (this->prev_geo.has_value()) {
      this->draw_color_cursor(lcd, this->prev_geo.value().cursor,
                              this->bg_color);
      this->lcd.LCD_buffer_flush();
    }

    if (!is_same_circle) {
      this->draw_color_circle(this->lcd, geo.circle);
      this->lcd.LCD_buffer_flush();
    }
  }

  this->draw_color_cursor(this->lcd, geo.cursor, this->cursor_params.color);

  // FIXME: test code
  // ----------------------------------------
  this->draw_color_svrect(this->lcd, geo.sv, h);
  this->draw_color_svrect_cursor(this->lcd, geo.sv_cursor);
  // ----------------------------------------

  this->prev_geo = geo;
  this->prev_bg_color = this->bg_color;

  lcd.LCD_buffer_flush();
}
} // namespace display
