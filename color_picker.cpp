#include "hardware/spi.h"
#include "pico/stdlib.h"
#include <algorithm>
#include <cmath>
#include <cstdio>

#include "LCD.h"
#include "LCD_buffer.hpp"
#include "color.hpp"

// SPI Defines
// We are going to use SPI 0, and allocate it to the following GPIO pins
// Pins can be changed, see the GPIO function select table in the datasheet for
// information on GPIO assignments
#define SPI_PORT spi0
#define PIN_MISO 16
#define PIN_CS 17
#define PIN_SCK 18
#define PIN_MOSI 19

#define PIN_DC 2
#define PIN_RST 3
#define PIN_BL 4

#define SCAN_DIR D2U_L2R // rot CCW 90 degree
#define BG_COLOR WHITE

inline void setup_spi() {
  // SPI initialisation. This example will use SPI at 1MHz.
  spi_init(SPI_PORT, 25 * 1000 * 1000);
  gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
  gpio_set_function(PIN_CS, GPIO_FUNC_SIO);
  gpio_set_function(PIN_SCK, GPIO_FUNC_SPI);
  gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);

  gpio_set_dir(PIN_CS, GPIO_OUT);
}

// LCD_ST7735S LCD(SPI_PORT, PIN_CS, PIN_DC, PIN_RST, PIN_BL);
LCD_ST7735S_buffered LCD(SPI_PORT, PIN_CS, PIN_DC, PIN_RST, PIN_BL);
void LCD_Init() {
  gpio_init(PIN_DC);
  gpio_set_dir(PIN_DC, GPIO_OUT);
  gpio_init(PIN_RST);
  gpio_set_dir(PIN_RST, GPIO_OUT);
  gpio_init(PIN_BL);
  gpio_set_dir(PIN_BL, GPIO_OUT);

  LCD.LCD_Init(SCAN_DIR);
}

const float pi = acos(-1.0);

void draw_color_circle(LCD_ST7735S *LCD, LCD_POINT x_start, LCD_POINT y_start,
                       LCD_LENGTH outer_r, LCD_LENGTH inner_r,
                       LCD_COLOR blank) {
  if (inner_r >= outer_r) {
    inner_r = outer_r >> 1;
  }

  LCD_LENGTH x_siz =
      std::min(outer_r << 1, LCD->sLCD_DIS.LCD_Dis_Column - x_start);
  LCD_LENGTH y_siz =
      std::min(outer_r << 1, LCD->sLCD_DIS.LCD_Dis_Page - y_start);

  LCD_COLOR img[x_siz * y_siz];

  for (uint i = 0; i < y_siz; ++i) {
    LCD_POINT y = y_start + i;

    for (uint j = 0; j < x_siz; ++j) {
      LCD_POINT x = x_start + j;
      size_t idx = i * x_siz + j;
      LCD_COLOR c;

      float x_dif = (float)j - (float)outer_r;
      float y_dif = (float)outer_r - (float)i;
      if (x_dif == 0 && y_dif == 0) {
        img[idx] = blank;
        continue;
      }

      float r = sqrt(x_dif * x_dif + y_dif * y_dif);
      float th = atan2(y_dif, x_dif);

      if (r < inner_r || r > outer_r) {
        img[idx] = blank;
        continue;
      }
      int th_deg = 180 * th / pi;
      uint16_t h = 90 - th_deg + (th_deg > 90 ? 360 : 0);

      c = color::HSV(h, 0xFF, 0xFF).to_rgb().to_565();
      img[idx] = c;
    }
  }

  LCD->LCD_DrawImage(x_start, y_start, x_start + x_siz, y_start + y_siz, img);

  gpio_put(PIN_CS, 1);
}

int main() {
  stdio_init_all();

  setup_spi();

  LCD_Init();

  LCD.LCD_Clear(BG_COLOR);

  uint t = 0;
  while (true) {
    draw_color_circle(&LCD, 20, 10, 50, 45, 0x8410);

    sleep_ms(1500);
    // t = (t >= 17 ? 0 : t + 1);
  }

  return 0;
}
