#include "hardware/spi.h"
#include "pico/stdlib.h"
#include <cstdio>

#include "LCD_buffer.hpp"
#include "color.hpp"
#include "display.hpp"

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

int main() {
  stdio_init_all();

  setup_spi();

  LCD_Init();

  LCD_COLOR bg_color = BLACK;

  LCD.LCD_Clear(bg_color);

  sleep_ms(2000);

  display::ColorSelectorDrawer cs_drawer;
  cs_drawer.set_bg_color(bg_color);
  cs_drawer.set_lcd(&LCD);
  cs_drawer.set_circle_params({40, 30, 40, 30});
  cs_drawer.set_cursor_params({10, 4, WHITE});

  int h = 0;

  while (true) {
    cs_drawer.draw_color_selector(h);
    h += 2;
    h %= 360;
    sleep_ms(16);
  }

  return 0;
}
