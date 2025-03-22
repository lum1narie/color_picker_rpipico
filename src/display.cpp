#include "display.hpp"
#include "LCD1in8/inc/LCD.h"
#include "LCD_buffer.hpp"
#include "color.hpp"
#include <cstdarg>

namespace display {
const float pi = std::acos(-1.0);

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
               (LCD_LENGTH)(lcd.sLCD_DIS.LCD_Dis_Column - circle.area_x_start));
  LCD_LENGTH y_siz =
      std::min(circle.area_height,
               (LCD_LENGTH)(lcd.sLCD_DIS.LCD_Dis_Page - circle.area_y_start));

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
      LCD_COLOR c = color::HSV(h, 0xFF, 0xFF).to_rgb().to_565();
      lcd.LCD_SetPointlColor(x, y, c);
    }
  }
}

std::optional<ColorSelectorGeometry>
ColorSelectorDrawer::calc_color_selector_geometry(
    LCD_ST7735S *LCD, int h, int s, int v, DISPLAY_UNIT x_start,
    DISPLAY_UNIT y_start, DISPLAY_UNIT circle_outer_r,
    DISPLAY_UNIT circle_inner_r, DISPLAY_UNIT cursor_height,
    DISPLAY_UNIT cursor_width) const {
  ColorSelectorGeometry retv;
  static float ROUND_EPS = 1e-4;

  if (circle_inner_r >= circle_outer_r || circle_inner_r < 0 ||
      cursor_height <= 0 || cursor_width <= 0) {
    return std::nullopt;
  }

  float th = h_to_angle(h);
  float cos_th = std::cos(th);
  float sin_th = std::sin(th);

  retv.circle.center = {x_start + circle_outer_r, y_start + circle_outer_r};

  float v0_x_init_from_center = circle_outer_r;
  float v0_y_init_from_center = 0;
  float v1_x_init_from_center = circle_outer_r + cursor_height;
  float v1_y_init_from_center = -cursor_width;
  float v2_x_init_from_center = circle_outer_r + cursor_height;
  float v2_y_init_from_center = cursor_width;

  retv.cursor.vertices[0].x = retv.circle.center.x +
                              v0_x_init_from_center * cos_th -
                              v0_y_init_from_center * sin_th;
  retv.cursor.vertices[0].y =
      retv.circle.center.y -
      (v0_x_init_from_center * sin_th + v0_y_init_from_center * cos_th);
  retv.cursor.vertices[1].x = retv.circle.center.x +
                              v1_x_init_from_center * cos_th -
                              v1_y_init_from_center * sin_th;
  retv.cursor.vertices[1].y =
      retv.circle.center.y -
      (v1_x_init_from_center * sin_th + v1_y_init_from_center * cos_th);
  retv.cursor.vertices[2].x = retv.circle.center.x +
                              v2_x_init_from_center * cos_th -
                              v2_y_init_from_center * sin_th;
  retv.cursor.vertices[2].y =
      retv.circle.center.y -
      (v2_x_init_from_center * sin_th + v2_y_init_from_center * cos_th);

  retv.cursor.area_x_start = std::floor(
      std::clamp(std::min({retv.cursor.vertices[0].x, retv.cursor.vertices[1].x,
                           retv.cursor.vertices[2].x}),
                 0.0f, (float)lcd.sLCD_DIS.LCD_Dis_Column) +
      ROUND_EPS);
  retv.cursor.area_y_start = std::floor(
      std::clamp(std::min({retv.cursor.vertices[0].y, retv.cursor.vertices[1].y,
                           retv.cursor.vertices[2].y}),
                 0.0f, (float)lcd.sLCD_DIS.LCD_Dis_Page) +
      ROUND_EPS);
  LCD_POINT cursor_x_end = std::ceil(
      std::clamp(std::max({retv.cursor.vertices[0].x, retv.cursor.vertices[1].x,
                           retv.cursor.vertices[2].x}) +
                     1.0f,
                 0.0f, (float)lcd.sLCD_DIS.LCD_Dis_Column) -
      ROUND_EPS);
  LCD_POINT cursor_y_end = std::ceil(
      std::clamp(std::max({retv.cursor.vertices[0].y, retv.cursor.vertices[1].y,
                           retv.cursor.vertices[2].y}) +
                     1.0f,
                 0.0f, (float)lcd.sLCD_DIS.LCD_Dis_Page) -
      ROUND_EPS);

  retv.cursor.area_width = cursor_x_end - retv.cursor.area_x_start;
  retv.cursor.area_height = cursor_y_end - retv.cursor.area_y_start;

  retv.circle.inner_r = circle_inner_r;
  retv.circle.outer_r = circle_outer_r;
  retv.circle.area_x_start =
      std::clamp(x_start, 0.0f, (float)lcd.sLCD_DIS.LCD_Dis_Column);
  retv.circle.area_y_start =
      std::clamp(y_start, 0.0f, (float)lcd.sLCD_DIS.LCD_Dis_Column);

  LCD_POINT circle_x_end =
      std::ceil(std::clamp(x_start + circle_outer_r * 2.0f + 1.0f, 0.0f,
                           (float)lcd.sLCD_DIS.LCD_Dis_Column) -
                ROUND_EPS);
  LCD_POINT circle_y_end =
      std::ceil(std::clamp(y_start + circle_outer_r * 2.0f + 1.0f, 0.0f,
                           (float)lcd.sLCD_DIS.LCD_Dis_Page) -
                ROUND_EPS);

  retv.circle.area_width = circle_x_end - retv.circle.area_x_start;
  retv.circle.area_height = circle_y_end - retv.circle.area_y_start;

  float sqrt_2 = std::sqrt(2.0f);
  retv.sv.x_min = retv.circle.center.x - circle_inner_r / sqrt_2;
  retv.sv.x_max = retv.circle.center.x + circle_inner_r / sqrt_2;
  retv.sv.y_min = retv.circle.center.y - circle_inner_r / sqrt_2;
  retv.sv.y_max = retv.circle.center.y + circle_inner_r / sqrt_2;

  retv.sv.area_x_start = std::ceil(
      std::clamp(retv.sv.x_min, 0.0f, (float)lcd.sLCD_DIS.LCD_Dis_Column) -
      ROUND_EPS);
  retv.sv.area_y_start = std::ceil(
      std::clamp(retv.sv.y_min, 0.0f, (float)lcd.sLCD_DIS.LCD_Dis_Page) -
      ROUND_EPS);
  LCD_POINT sv_x_end = std::clamp(retv.sv.x_max + 1.0f, 0.0f,
                                  (float)lcd.sLCD_DIS.LCD_Dis_Column);
  LCD_POINT sv_y_end =
      std::clamp(retv.sv.y_max + 1.0f, 0.0f, (float)lcd.sLCD_DIS.LCD_Dis_Page);
  retv.sv.area_width = sv_x_end - retv.sv.area_x_start;
  retv.sv.area_height = sv_y_end - retv.sv.area_y_start;

  retv.sv_cursor.x = std::clamp(
      (retv.sv.x_max * (float)s + retv.sv.x_min * (255.0f - (float)s)) / 255.0f,
      0.0f, (float)lcd.sLCD_DIS.LCD_Dis_Column);
  retv.sv_cursor.y = std::clamp(
      (retv.sv.y_min * (float)v + retv.sv.y_max * (255.0f - (float)v)) / 255.0f,
      0.0f, (float)lcd.sLCD_DIS.LCD_Dis_Page);

  return retv;
}

void ColorSelectorDrawer::draw_color_cursor(LCD_ST7735SBuffered &LCD,
                                            ColorCursorGeometry cursor,
                                            LCD_COLOR fg_color) const {
  LCD_LENGTH x_siz =
      std::min(cursor.area_width,
               (LCD_LENGTH)(lcd.sLCD_DIS.LCD_Dis_Column - cursor.area_x_start));
  LCD_LENGTH y_siz =
      std::min(cursor.area_height,
               (LCD_LENGTH)(lcd.sLCD_DIS.LCD_Dis_Page - cursor.area_y_start));

  // calculate the color of each point
  for (LCD_POINT i = 0; i < y_siz; ++i) {
    LCD_POINT y = cursor.area_y_start + i;
    for (uint j = 0; j < x_siz; ++j) {
      LCD_POINT x = cursor.area_x_start + j;
      // check point if it is inside of cursor
      Float2D p = Float2D{(float)x, (float)y};
      if (is_in_triangle(p, cursor.vertices)) {
        lcd.LCD_SetPointlColor(x, y, fg_color);
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
      LCD.LCD_SetPointlColor(x, y, color::HSV(h, s, v).to_rgb().to_565());
    }
  }
}

void ColorSelectorDrawer::draw_color_svrect_cursor(
    LCD_ST7735SBuffered &LCD, ColorSVRectangleCursorGeometry cursor) const {
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
          &this->lcd, h, s, v, this->circle_params.area_x_start,
          this->circle_params.area_y_start, this->circle_params.outer_r,
          this->circle_params.inner_r, this->cursor_params.height,
          this->cursor_params.width);
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
