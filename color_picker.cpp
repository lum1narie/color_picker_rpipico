#include "hardware/spi.h"
#include "pico/stdlib.h"
#include <cstdio>

#include "LCD.h"
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

LCD_ST7735S LCD(SPI_PORT, PIN_CS, PIN_DC, PIN_RST, PIN_BL);
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

  LCD.LCD_Clear(BG_COLOR);

  uint t = 0;
  while (true) {
    uint16_t h = t * 20;
    for (int i = 0; i < (sLCD_DIS.LCD_Dis_Page >> 3); ++i) {
      uint8_t v = i << 4;
      for (int j = 0; j < (sLCD_DIS.LCD_Dis_Column >> 3); ++j) {
        uint8_t s = j << 4;
        LCD.LCD_SetArealColor(j << 3, i << 3, (j + 1) << 3, (i + 1) << 3,
                              color::HSV(h, s, v).to_rgb().to_565());
      }
    }

    sleep_ms(1500);
    t = (t >= 17 ? 0 : t + 1);
  }

  return 0;
}
