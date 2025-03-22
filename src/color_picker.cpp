#include "LCD.h"
#include "hardware/spi.h"
#include "pico/stdlib.h"
#include <cstdio>

#include "LCD_buffer.hpp"
#include "color.hpp"
#include "display.hpp"

// SPI Defines
// information on GPIO assignments
spi_inst_t *const SPI_PORT = spi0;
const uint PIN_MISO = 16;
const uint PIN_CS = 17;
const uint PIN_SCK = 18;
const uint PIN_MOSI = 19;

const uint PIN_DC = 2;
const uint PIN_RST = 3;
const uint PIN_BL = 4;

const LCD_SCAN_DIR SCAN_DIR = D2U_L2R;
const LCD_COLOR BG_COLOR = BLACK;

struct AppBoard {
  spi_inst_t *spi;

  uint pin_miso;
  uint pin_mosi;
  uint pin_sck;
  uint pin_cs;

  uint pin_dc;
  uint pin_reset;
  uint pin_bl;
};

struct LcdConfig {
  LCD_SCAN_DIR scan_dir;
  LCD_COLOR bg_color;
};

class App {
protected:
  AppBoard board;
  LcdConfig lcd_conf;
  LCD_ST7735SBuffered lcd;

  /**
   * @brief setup SPI connection
   * @param[in] baudrate: baud rate for SPI (Hz)
   */
  void setup_spi(const AppBoard &board, uint baud_rate) {
    spi_init(board.spi, baud_rate);
    gpio_set_function(board.pin_miso, GPIO_FUNC_SPI);
    gpio_set_function(board.pin_cs, GPIO_FUNC_SIO);
    gpio_set_function(board.pin_sck, GPIO_FUNC_SPI);
    gpio_set_function(board.pin_mosi, GPIO_FUNC_SPI);

    gpio_set_dir(board.pin_cs, GPIO_OUT);
  }

  /**
   * @brief setup GPIO and config for LCD
   * @param[in] scan_dir: direction of LCD
   */
  void LCD_init(const AppBoard &board, LCD_ST7735SBuffered &lcd,
                LCD_SCAN_DIR scan_dir) {
    gpio_init(board.pin_dc);
    gpio_set_dir(board.pin_dc, GPIO_OUT);
    gpio_init(board.pin_reset);
    gpio_set_dir(board.pin_reset, GPIO_OUT);
    gpio_init(board.pin_bl);
    gpio_set_dir(board.pin_bl, GPIO_OUT);

    lcd.LCD_Init(scan_dir);
  }

public:
  App(AppBoard board, LcdConfig lcd_conf, uint baud_rate)
      : board(board), lcd_conf(lcd_conf),
        lcd(LCD_ST7735SBuffered(board.spi, board.pin_cs, board.pin_dc,
                                board.pin_reset, board.pin_bl)) {
    this->setup_spi(board, baud_rate);
    this->LCD_init(board, lcd, lcd_conf.scan_dir);
  }

  void run() {
    display::ColorSelectorDrawer cs_drawer(this->lcd);
    cs_drawer.set_bg_color(this->lcd_conf.bg_color);
    cs_drawer.set_circle_params({40, 30, 40, 30});
    cs_drawer.set_cursor_params({10, 4, WHITE});

    int h = 0;
    while (true) {
      cs_drawer.draw_color_selector(h, 100, 50);
      h += 2;
      h %= 360;
      sleep_ms(16);
    }
  }
};

int main() {
  stdio_init_all();

  const AppBoard board = {
      SPI_PORT, PIN_MISO, PIN_MOSI, PIN_SCK, PIN_CS, PIN_DC, PIN_RST, PIN_BL,
  };
  const LcdConfig lcd_conf = {SCAN_DIR, BLACK};
  const uint baud_rate = 25 * 1000 * 1000;

  App app(board, lcd_conf, baud_rate);
  sleep_ms(2000);
  app.run();

  return 0;
}
