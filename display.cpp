#include "display.hpp"

const float pi = std::acos(-1.0);

namespace display {
void draw_color_circle(LCD_ST7735S_buffered *LCD, Float_2D center,
                       DISPLAY_UNIT outer_r, DISPLAY_UNIT inner_r,
                       LCD_POINT x_start, LCD_POINT y_start, LCD_LENGTH height,
                       LCD_LENGTH width) {
  // set inner_r as half as outer_r if invalid value is given
  if (inner_r >= outer_r) {
    inner_r = outer_r * 0.75;
  }

  // cut off area size when circle go out from display
  LCD_LENGTH x_siz =
      std::min(width, (LCD_LENGTH)(LCD->sLCD_DIS.LCD_Dis_Column - x_start));
  LCD_LENGTH y_siz =
      std::min(height, (LCD_LENGTH)(LCD->sLCD_DIS.LCD_Dis_Page - y_start));

  // calculate the color of each point
  for (uint i = 0; i < y_siz; ++i) {
    for (uint j = 0; j < x_siz; ++j) {
      LCD_POINT x = x_start + j;
      LCD_POINT y = y_start + i;
      // assume the center is origin
      float x_from_center = (float)x - center.x;
      float y_from_center = (float)y - center.y;

      if (float_eq(x_from_center, 0.0f) && float_eq(y_from_center, 0.0f)) {
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
         "\tvertices:\n"
         "\t\t(%f, %f)\n"
         "\t\t(%f, %f)\n"
         "\t\t(%f, %f)\n"
         "circle:\n"
         "\tarea: (%d, %d) -> w: %d, h: %d\n"
         "\tcenter: (%f, %f)\n"
         "\tr: %f, inner: %f\n",
         geo->cursor_area_x_start, geo->cursor_area_y_start,
         geo->cursor_area_width, geo->cursor_area_height,
         geo->cursor_vertices[0].x, geo->cursor_vertices[0].y,
         geo->cursor_vertices[1].x, geo->cursor_vertices[1].y,
         geo->cursor_vertices[2].x, geo->cursor_vertices[2].y,
         geo->circle_area_x_start, geo->circle_area_y_start,
         geo->circle_area_width, geo->circle_area_height, geo->circle_center.x,
         geo->circle_center.y, geo->circle_outer_r, geo->circle_inner_r);
}

Color_selector_geometry calc_color_selector_geometry(
    LCD_ST7735S *LCD, int h, DISPLAY_UNIT x_start, DISPLAY_UNIT y_start,
    DISPLAY_UNIT circle_outer_r, DISPLAY_UNIT circle_inner_r,
    DISPLAY_UNIT cursor_height, DISPLAY_UNIT cursor_width) {
  Color_selector_geometry retv;
  static float ROUND_EPS = 0.0001f;

  retv.is_valid = false;

  if (circle_inner_r >= circle_outer_r || circle_inner_r < 0 ||
      cursor_height <= 0 || cursor_width <= 0) {
    return retv;
  }

  h %= 360;

  float cos_th = std::cos(h * pi / 180.0);
  float sin_th = std::sin(h * pi / 180.0);

  retv.circle_center = {x_start + circle_outer_r, y_start + circle_outer_r};

  float v0_x_init_from_center = 0;
  float v0_y_init_from_center = -circle_outer_r;
  float v1_x_init_from_center = -cursor_width;
  float v1_y_init_from_center = -circle_outer_r - cursor_height;
  float v2_x_init_from_center = cursor_width;
  float v2_y_init_from_center = -circle_outer_r - cursor_height;

  retv.cursor_vertices[0].x = retv.circle_center.x +
                              v0_x_init_from_center * cos_th -
                              v0_y_init_from_center * sin_th;
  retv.cursor_vertices[0].y = retv.circle_center.y +
                              v0_x_init_from_center * sin_th +
                              v0_y_init_from_center * cos_th;
  retv.cursor_vertices[1].x = retv.circle_center.x +
                              v1_x_init_from_center * cos_th -
                              v1_y_init_from_center * sin_th;
  retv.cursor_vertices[1].y = retv.circle_center.y +
                              v1_x_init_from_center * sin_th +
                              v1_y_init_from_center * cos_th;
  retv.cursor_vertices[2].x = retv.circle_center.x +
                              v2_x_init_from_center * cos_th -
                              v2_y_init_from_center * sin_th;
  retv.cursor_vertices[2].y = retv.circle_center.y +
                              v2_x_init_from_center * sin_th +
                              v2_y_init_from_center * cos_th;

  retv.cursor_area_x_start = std::floor(
      std::clamp(std::min({retv.cursor_vertices[0].x, retv.cursor_vertices[1].x,
                           retv.cursor_vertices[2].x}),
                 0.0f, (float)LCD->sLCD_DIS.LCD_Dis_Column) +
      ROUND_EPS);
  retv.cursor_area_y_start = std::floor(
      std::clamp(std::min({retv.cursor_vertices[0].y, retv.cursor_vertices[1].y,
                           retv.cursor_vertices[2].y}),
                 0.0f, (float)LCD->sLCD_DIS.LCD_Dis_Page) +
      ROUND_EPS);
  LCD_POINT cursor_x_end = std::ceil(
      std::clamp(std::max({retv.cursor_vertices[0].x, retv.cursor_vertices[1].x,
                           retv.cursor_vertices[2].x}) +
                     1.0f,
                 0.0f, (float)LCD->sLCD_DIS.LCD_Dis_Column) -
      ROUND_EPS);
  LCD_POINT cursor_y_end = std::ceil(
      std::clamp(std::max({retv.cursor_vertices[0].y, retv.cursor_vertices[1].y,
                           retv.cursor_vertices[2].y}) +
                     1.0f,
                 0.0f, (float)LCD->sLCD_DIS.LCD_Dis_Page) -
      ROUND_EPS);

  retv.cursor_area_width = cursor_x_end - retv.cursor_area_x_start;
  retv.cursor_area_height = cursor_y_end - retv.cursor_area_y_start;

  retv.circle_inner_r = circle_inner_r;
  retv.circle_outer_r = circle_outer_r;
  retv.circle_area_x_start =
      std::clamp(x_start, 0.0f, (float)LCD->sLCD_DIS.LCD_Dis_Column);
  retv.circle_area_y_start =
      std::clamp(y_start, 0.0f, (float)LCD->sLCD_DIS.LCD_Dis_Column);

  LCD_POINT circle_x_end =
      std::ceil(std::clamp(x_start + circle_outer_r * 2.0f + 1.0f, 0.0f,
                           (float)LCD->sLCD_DIS.LCD_Dis_Column) -
                ROUND_EPS);
  LCD_POINT circle_y_end =
      std::ceil(std::clamp(y_start + circle_outer_r * 2.0f + 1.0f, 0.0f,
                           (float)LCD->sLCD_DIS.LCD_Dis_Page) -
                ROUND_EPS);

  retv.circle_area_width = circle_x_end - retv.circle_area_x_start;
  retv.circle_area_height = circle_y_end - retv.circle_area_y_start;

  retv.is_valid = true;
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

Color_selector_geometry prev_geo;

/**
 * @brief draw color selector
 * TODO: Fill Documentation
 */
void draw_color_selector(LCD_ST7735S_buffered *LCD, int h, DISPLAY_UNIT x_start,
                         DISPLAY_UNIT y_start, DISPLAY_UNIT outer_r,
                         DISPLAY_UNIT inner_r, DISPLAY_UNIT cursor_height,
                         DISPLAY_UNIT cursor_width, LCD_COLOR cursor_color,
                         LCD_COLOR bg_color) {
  // calc cursor geometry
  Color_selector_geometry geo = calc_color_selector_geometry(
      LCD, h, x_start, y_start, outer_r, inner_r, cursor_height, cursor_width);
  if (!geo.is_valid) {
    printf("invalid geometry!!\n");
    print_color_selector_geometry(&geo);
    return;
  }

  bool is_same_circle =
      float_eq(geo.circle_center.x, prev_geo.circle_center.x) ||
      float_eq(geo.circle_center.y, prev_geo.circle_center.y) ||
      float_eq(geo.circle_outer_r, prev_geo.circle_outer_r) ||
      float_eq(geo.circle_inner_r, prev_geo.circle_inner_r);

  // erase old cursor
  draw_color_cursor(LCD, prev_geo.cursor_area_x_start,
                    prev_geo.cursor_area_y_start, prev_geo.cursor_area_height,
                    prev_geo.cursor_area_width, prev_geo.cursor_vertices,
                    bg_color);

  if (is_same_circle) {
    LCD->LCD_buffer_flush();
  } else {
    draw_color_circle(LCD, geo.circle_center, geo.circle_outer_r,
                      geo.circle_inner_r, geo.circle_area_x_start,
                      geo.circle_area_y_start, geo.circle_area_height,
                      geo.circle_area_width);
  }

  draw_color_cursor(LCD, geo.cursor_area_x_start, geo.cursor_area_y_start,
                    geo.cursor_area_height, geo.cursor_area_width,
                    geo.cursor_vertices, cursor_color);

  prev_geo = geo;

  LCD->LCD_buffer_flush();
}
} // namespace display
