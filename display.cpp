#include "display.hpp"

const float pi = std::acos(-1.0);

namespace display {
void draw_color_circle(LCD_ST7735S_buffered *LCD, LCD_POINT x_start,
                       LCD_POINT y_start, LCD_LENGTH outer_r,
                       LCD_LENGTH inner_r) {
  // set inner_r as half as outer_r if invalid value is given
  if (inner_r >= outer_r) {
    inner_r = outer_r >> 1;
  }

  // cut off area size when circle go out from display
  LCD_LENGTH x_siz =
      std::min((outer_r << 1) + 2, LCD->sLCD_DIS.LCD_Dis_Column - x_start);
  LCD_LENGTH y_siz =
      std::min((outer_r << 1) + 2, LCD->sLCD_DIS.LCD_Dis_Page - y_start);

  // calculate the color of each point
  for (uint i = 0; i < y_siz; ++i) {
    for (uint j = 0; j < x_siz; ++j) {
      LCD_POINT x = x_start + j;
      LCD_POINT y = y_start + i;
      // assume the center is origin
      float x_from_center = (float)j - (float)outer_r;
      float y_from_center = (float)outer_r - (float)i;
      if (x_from_center == 0 && y_from_center == 0) {
        continue;
      }

      // deal point in polar coordinates
      float r =
          sqrt(x_from_center * x_from_center + y_from_center * y_from_center);
      float th = atan2(y_from_center, x_from_center);

      if (r < inner_r || r > outer_r) {
        continue;
      }
      int th_deg = 180 * th / pi;
      uint16_t h = 90 - th_deg + (th_deg > 90 ? 360 : 0);

      LCD_COLOR c = color::HSV(h, 0xFF, 0xFF).to_rgb().to_565();
      LCD->LCD_SetPointlColor(x, y, c);
    }
  }
}

void print_color_selector_geometry(Color_selector_geometry *geo) {
  printf("cursor:\n"
         "\tarea: (%d, %d) -> w: %d, h: %d\n"
         "\tverticies:\n"
         "\t\t(%f, %f)\n"
         "\t\t(%f, %f)\n"
         "\t\t(%f, %f)\n"
         "circle:\n"
         "\tarea: (%d, %d) -> r: %d, inner: %d\n",
         geo->cursor_x_start, geo->cursor_y_start, geo->cursor_width,
         geo->cursor_height, geo->vertices[0].x, geo->vertices[0].y,
         geo->vertices[1].x, geo->vertices[1].y, geo->vertices[2].x,
         geo->vertices[2].y, geo->circle_x_start, geo->circle_y_start,
         geo->circle_outer_r, geo->circle_inner_r);
}

Color_selector_geometry
calc_color_selector_geometry(LCD_ST7735S *LCD, int h, LCD_POINT x_start,
                             LCD_POINT y_start, LCD_LENGTH circle_outer_r,
                             LCD_LENGTH circle_inner_r, LCD_LENGTH height,
                             LCD_LENGTH width) {
  Color_selector_geometry retv;

  h %= 360;

  retv.circle_inner_r = circle_inner_r;
  retv.circle_outer_r = circle_outer_r;
  retv.circle_x_start = x_start;
  retv.circle_y_start = y_start;

  // LCD_LENGTH a = circle_outer_r + height;
  // LCD_LENGTH overall_r = ceil(sqrt(a * a + width * width));

  float x_center = x_start + circle_outer_r;
  float y_center = y_start + circle_outer_r;

  float cos_th = std::cos(h * pi / 180.0);
  float sin_th = std::sin(h * pi / 180.0);

  float v0_x_init_from_center = 0;
  float v0_y_init_from_center = -circle_outer_r;
  float v1_x_init_from_center = -width;
  float v1_y_init_from_center = -circle_outer_r - height;
  float v2_x_init_from_center = width;
  float v2_y_init_from_center = -circle_outer_r - height;

  retv.vertices[0].x = x_center + v0_x_init_from_center * cos_th -
                       v0_y_init_from_center * sin_th;
  retv.vertices[0].y = y_center + v0_x_init_from_center * sin_th +
                       v0_y_init_from_center * cos_th;
  retv.vertices[1].x = x_center + v1_x_init_from_center * cos_th -
                       v1_y_init_from_center * sin_th;
  retv.vertices[1].y = y_center + v1_x_init_from_center * sin_th +
                       v1_y_init_from_center * cos_th;
  retv.vertices[2].x = x_center + v2_x_init_from_center * cos_th -
                       v2_y_init_from_center * sin_th;
  retv.vertices[2].y = y_center + v2_x_init_from_center * sin_th +
                       v2_y_init_from_center * cos_th;

  retv.cursor_x_start = std::floor(std::max(
      std::min({retv.vertices[0].x, retv.vertices[1].x, retv.vertices[2].x}),
      0.0f));
  retv.cursor_y_start = std::floor(std::max(
      std::min({retv.vertices[0].y, retv.vertices[1].y, retv.vertices[2].y}),
      0.0f));
  LCD_POINT cursor_x_end = std::ceil(std::min(
      std::max({retv.vertices[0].x, retv.vertices[1].x, retv.vertices[2].x}) +
          1,
      (float)LCD->sLCD_DIS.LCD_Dis_Column));
  LCD_POINT cursor_y_end = std::ceil(std::min(
      std::max({retv.vertices[0].y, retv.vertices[1].y, retv.vertices[2].y}) +
          1,
      (float)LCD->sLCD_DIS.LCD_Dis_Page));

  retv.cursor_width = cursor_x_end - retv.cursor_x_start;
  retv.cursor_height = cursor_y_end - retv.cursor_y_start;

  return retv;
}

/**
 * @brief draw cursor for color circle
 * TODO: Fill Documentation
 */
void draw_color_cursor(LCD_ST7735S_buffered *LCD, LCD_POINT x_start,
                       LCD_POINT y_start, LCD_LENGTH height, LCD_LENGTH width,
                       Float_2D *vertices, LCD_COLOR fg_color) {
  LCD_LENGTH x_siz =
      std::min(width, (LCD_LENGTH)(LCD->sLCD_DIS.LCD_Dis_Column - x_start));
  LCD_LENGTH y_siz =
      std::min(height, (LCD_LENGTH)(LCD->sLCD_DIS.LCD_Dis_Page - y_start));

  // calculate the color of each point
  for (LCD_POINT i = 0; i < y_siz; ++i) {
    LCD_POINT y = y_start + i;
    for (uint j = 0; j < x_siz; ++j) {
      LCD_POINT x = x_start + j;
      // check point if it is inside of cursor
      Float_2D p = Float_2D{(float)x, (float)y};
      if (is_in_triangle(p, vertices)) {
        LCD->LCD_SetPointlColor(x, y, fg_color);
      }
    }
  }
}

Color_selector_geometry prev_geo = {
    0, 0, 0, 0, {{0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}}, 0, 0, 0, 0};

/**
 * @brief draw color selector
 * TODO: Fill Documentation
 */
void draw_color_selector(LCD_ST7735S_buffered *LCD, int h, LCD_POINT x_start,
                         LCD_POINT y_start, LCD_LENGTH outer_r,
                         LCD_LENGTH inner_r, LCD_LENGTH cursor_height,
                         LCD_LENGTH cursor_width, LCD_COLOR cursor_color,
                         LCD_COLOR bg_color) {
  // calc cursor geometry
  Color_selector_geometry geo = calc_color_selector_geometry(
      LCD, h, x_start, y_start, outer_r, inner_r, cursor_height, cursor_width);

  bool is_same_circle = (geo.circle_x_start == prev_geo.circle_x_start) &&
                        (geo.circle_y_start == prev_geo.circle_y_start) &&
                        (geo.circle_outer_r == prev_geo.circle_outer_r) &&
                        (geo.circle_inner_r == prev_geo.circle_inner_r);

  // erase old cursor
  draw_color_cursor(LCD, prev_geo.cursor_x_start, prev_geo.cursor_y_start,
                    prev_geo.cursor_height + 1, prev_geo.cursor_width + 1,
                    prev_geo.vertices, bg_color);

  if (is_same_circle) {
    LCD->LCD_buffer_flush();
  } else {
    draw_color_circle(LCD, geo.circle_x_start, geo.circle_y_start,
                      geo.circle_outer_r, geo.circle_inner_r);
  }

  draw_color_cursor(LCD, geo.cursor_x_start, geo.cursor_y_start,
                    geo.cursor_height, geo.cursor_width, geo.vertices,
                    cursor_color);

  prev_geo = geo;

  LCD->LCD_buffer_flush();
}
} // namespace display
