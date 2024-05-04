#ifndef __LCD_BUFFER_HPP
#define __LCD_BUFFER_HPP

#include "LCD.h"
#include "hardware/spi.h"
#include "pico/stdlib.h"

#define BUFFER_MAXPIXEL LCD_Y_MAXPIXEL

#define LCD_POINT_INF 0xFFFF
struct LCDBuffer {
  LCD_COLOR buffer[BUFFER_MAXPIXEL][BUFFER_MAXPIXEL];
  LCD_POINT x_start;
  LCD_POINT y_start;
  LCD_POINT x_end;
  LCD_POINT y_end;

  LCDBuffer();
};

class LCD_ST7735SBuffered : public LCD_ST7735S {
protected:
  void LCD_buffer_init();

public:
  LCD_ST7735SBuffered(spi_inst_t *spi_port, uint pin_cs, uint pin_dc,
                       uint pin_rst, uint pin_bl);

  void LCD_SetPointlColor(LCD_POINT Xpoint, LCD_POINT Ypoint,
                          LCD_COLOR Color) override;
  void LCD_SetArealColor(LCD_POINT Xstart, LCD_POINT Ystart, LCD_POINT Xend,
                         LCD_POINT Yend, LCD_COLOR Color) override;
  void LCD_Clear(LCD_COLOR Color) override;
  void LCD_Show(void) override;

  void LCD_buffer_flush();
};

#endif
