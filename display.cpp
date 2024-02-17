#include "display.hpp"

const float pi = std::acos(-1.0);

namespace display {
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

ColorSelectorDrawer::ColorSelectorDrawer() {
  this->prev_geo.is_valid = false;
}

void ColorSelectorDrawer::set_circle_params(ColorCircleParams params) {
  this->circle_params = params;
}

void ColorSelectorDrawer::set_cursor_params(ColorCursorParams params) {
  this->cursor_params = params;
}

void ColorSelectorDrawer::set_bg_color(LCD_COLOR bg_color) {
  this->bg_color = bg_color;
}

void ColorSelectorDrawer::set_lcd(LCD_ST7735SBuffered *LCD) {
  this->LCD = LCD;
}

void ColorSelectorDrawer::draw_color_circle(
    LCD_ST7735SBuffered *LCD, ColorCircleGeometry circle) const {
  // set inner_r as half as outer_r if invalid value is given
  if (circle.inner_r >= circle.outer_r) {
    circle.inner_r = circle.outer_r * 0.75;
  }

  // cut off area size when circle go out from display
  LCD_LENGTH x_siz =
      std::min(circle.area_width, (LCD_LENGTH)(LCD->sLCD_DIS.LCD_Dis_Column -
                                               circle.area_x_start));
  LCD_LENGTH y_siz =
      std::min(circle.area_height,
               (LCD_LENGTH)(LCD->sLCD_DIS.LCD_Dis_Page - circle.area_y_start));

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
      float th = atan2(y_from_center, x_from_center);

      if (r < circle.inner_r || r > circle.outer_r) {
        continue;
      }
      int th_deg = 180 * th / pi;
      uint16_t h = 90 - th_deg + (th_deg > 90 ? 360 : 0);

      LCD_COLOR c = color::HSV(h, 0xFF, 0xFF).to_rgb().to_565();
      LCD->LCD_SetPointlColor(x, y, c);
    }
  }
}

ColorSelectorGeometry ColorSelectorDrawer::calc_color_selector_geometry(
    LCD_ST7735S *LCD, int h, DISPLAY_UNIT x_start, DISPLAY_UNIT y_start,
    DISPLAY_UNIT circle_outer_r, DISPLAY_UNIT circle_inner_r,
    DISPLAY_UNIT cursor_height, DISPLAY_UNIT cursor_width) const {
  ColorSelectorGeometry retv;
  static float ROUND_EPS = 1e-4;

  retv.is_valid = false;

  if (circle_inner_r >= circle_outer_r || circle_inner_r < 0 ||
      cursor_height <= 0 || cursor_width <= 0) {
    return retv;
  }

  h %= 360;

  float cos_th = std::cos(h * pi / 180.0);
  float sin_th = std::sin(h * pi / 180.0);

  retv.circle.center = {x_start + circle_outer_r, y_start + circle_outer_r};

  float v0_x_init_from_center = 0;
  float v0_y_init_from_center = -circle_outer_r;
  float v1_x_init_from_center = -cursor_width;
  float v1_y_init_from_center = -circle_outer_r - cursor_height;
  float v2_x_init_from_center = cursor_width;
  float v2_y_init_from_center = -circle_outer_r - cursor_height;

  retv.cursor.vertices[0].x = retv.circle.center.x +
                              v0_x_init_from_center * cos_th -
                              v0_y_init_from_center * sin_th;
  retv.cursor.vertices[0].y = retv.circle.center.y +
                              v0_x_init_from_center * sin_th +
                              v0_y_init_from_center * cos_th;
  retv.cursor.vertices[1].x = retv.circle.center.x +
                              v1_x_init_from_center * cos_th -
                              v1_y_init_from_center * sin_th;
  retv.cursor.vertices[1].y = retv.circle.center.y +
                              v1_x_init_from_center * sin_th +
                              v1_y_init_from_center * cos_th;
  retv.cursor.vertices[2].x = retv.circle.center.x +
                              v2_x_init_from_center * cos_th -
                              v2_y_init_from_center * sin_th;
  retv.cursor.vertices[2].y = retv.circle.center.y +
                              v2_x_init_from_center * sin_th +
                              v2_y_init_from_center * cos_th;

  retv.cursor.area_x_start = std::floor(
      std::clamp(std::min({retv.cursor.vertices[0].x, retv.cursor.vertices[1].x,
                           retv.cursor.vertices[2].x}),
                 0.0f, (float)LCD->sLCD_DIS.LCD_Dis_Column) +
      ROUND_EPS);
  retv.cursor.area_y_start = std::floor(
      std::clamp(std::min({retv.cursor.vertices[0].y, retv.cursor.vertices[1].y,
                           retv.cursor.vertices[2].y}),
                 0.0f, (float)LCD->sLCD_DIS.LCD_Dis_Page) +
      ROUND_EPS);
  LCD_POINT cursor_x_end = std::ceil(
      std::clamp(std::max({retv.cursor.vertices[0].x, retv.cursor.vertices[1].x,
                           retv.cursor.vertices[2].x}) +
                     1.0f,
                 0.0f, (float)LCD->sLCD_DIS.LCD_Dis_Column) -
      ROUND_EPS);
  LCD_POINT cursor_y_end = std::ceil(
      std::clamp(std::max({retv.cursor.vertices[0].y, retv.cursor.vertices[1].y,
                           retv.cursor.vertices[2].y}) +
                     1.0f,
                 0.0f, (float)LCD->sLCD_DIS.LCD_Dis_Page) -
      ROUND_EPS);

  retv.cursor.area_width = cursor_x_end - retv.cursor.area_x_start;
  retv.cursor.area_height = cursor_y_end - retv.cursor.area_y_start;

  retv.circle.inner_r = circle_inner_r;
  retv.circle.outer_r = circle_outer_r;
  retv.circle.area_x_start =
      std::clamp(x_start, 0.0f, (float)LCD->sLCD_DIS.LCD_Dis_Column);
  retv.circle.area_y_start =
      std::clamp(y_start, 0.0f, (float)LCD->sLCD_DIS.LCD_Dis_Column);

  LCD_POINT circle_x_end =
      std::ceil(std::clamp(x_start + circle_outer_r * 2.0f + 1.0f, 0.0f,
                           (float)LCD->sLCD_DIS.LCD_Dis_Column) -
                ROUND_EPS);
  LCD_POINT circle_y_end =
      std::ceil(std::clamp(y_start + circle_outer_r * 2.0f + 1.0f, 0.0f,
                           (float)LCD->sLCD_DIS.LCD_Dis_Page) -
                ROUND_EPS);

  retv.circle.area_width = circle_x_end - retv.circle.area_x_start;
  retv.circle.area_height = circle_y_end - retv.circle.area_y_start;

  retv.is_valid = true;
  return retv;
}

void ColorSelectorDrawer::draw_color_cursor(LCD_ST7735SBuffered *LCD,
                                              ColorCursorGeometry cursor,
                                              LCD_COLOR fg_color) const {
  LCD_LENGTH x_siz =
      std::min(cursor.area_width, (LCD_LENGTH)(LCD->sLCD_DIS.LCD_Dis_Column -
                                               cursor.area_x_start));
  LCD_LENGTH y_siz =
      std::min(cursor.area_height,
               (LCD_LENGTH)(LCD->sLCD_DIS.LCD_Dis_Page - cursor.area_y_start));

  // calculate the color of each point
  for (LCD_POINT i = 0; i < y_siz; ++i) {
    LCD_POINT y = cursor.area_y_start + i;
    for (uint j = 0; j < x_siz; ++j) {
      LCD_POINT x = cursor.area_x_start + j;
      // check point if it is inside of cursor
      Float2D p = Float2D{(float)x, (float)y};
      if (is_in_triangle(p, cursor.vertices)) {
        LCD->LCD_SetPointlColor(x, y, fg_color);
      }
    }
  }
}

void ColorSelectorDrawer::draw_color_selector(int h) {
  // calc cursor geometry
  ColorSelectorGeometry geo = this->calc_color_selector_geometry(
      this->LCD, h, this->circle_params.area_x_start,
      this->circle_params.area_y_start, this->circle_params.outer_r,
      this->circle_params.inner_r, this->cursor_params.height,
      this->cursor_params.width);
  if (!geo.is_valid) {
    printf("invalid geometry!!\n");
    print_color_selector_geometry(&geo);
    return;
  }

  bool is_same_circle =
      float_eq(geo.circle.center.x, this->prev_geo.circle.center.x) ||
      float_eq(geo.circle.center.y, this->prev_geo.circle.center.y) ||
      float_eq(geo.circle.outer_r, this->prev_geo.circle.outer_r) ||
      float_eq(geo.circle.inner_r, this->prev_geo.circle.inner_r);

  // erase old cursor
  this->draw_color_cursor(LCD, this->prev_geo.cursor, this->bg_color);

  if (is_same_circle) {
    this->LCD->LCD_buffer_flush();
  } else {
    this->draw_color_circle(this->LCD, geo.circle);
  }

  this->draw_color_cursor(this->LCD, geo.cursor, this->cursor_params.color);

  this->prev_geo = geo;

  LCD->LCD_buffer_flush();
}
} // namespace display
