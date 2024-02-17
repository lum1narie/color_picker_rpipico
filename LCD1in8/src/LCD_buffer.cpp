#include "LCD_buffer.hpp"
#include <algorithm>
#include <cstdio>

static LCDBuffer *buffer;

LCDBuffer::LCDBuffer() {
  x_start = LCD_POINT_INF;
  x_end = 0;
  y_start = LCD_POINT_INF;
  y_end = 0;
}

void LCD_ST7735SBuffered::LCD_buffer_init() {
  if (buffer == NULL) {
    buffer = new LCDBuffer();
  }
}

LCD_ST7735SBuffered::LCD_ST7735SBuffered(spi_inst_t *spi_port, uint pin_cs,
                                           uint pin_dc, uint pin_rst,
                                           uint pin_bl)
    : LCD_ST7735S(spi_port, pin_cs, pin_dc, pin_rst, pin_bl) {
  LCD_buffer_init();
}

void LCD_ST7735SBuffered::LCD_SetPointlColor(LCD_POINT Xpoint,
                                              LCD_POINT Ypoint,
                                              LCD_COLOR Color) {

  if ((Xpoint > sLCD_DIS.LCD_Dis_Column) || (Ypoint > sLCD_DIS.LCD_Dis_Page)) {
    return;
  }

  buffer->buffer[Ypoint][Xpoint] = Color;
  if (Xpoint < buffer->x_start) {
    buffer->x_start = Xpoint;
  }
  if (Ypoint < buffer->y_start) {
    buffer->y_start = Ypoint;
  }
  if (Xpoint >= buffer->x_end) {
    buffer->x_end = Xpoint + 1;
  }
  if (Ypoint >= buffer->y_end) {
    buffer->y_end = Ypoint + 1;
  }
}

void LCD_ST7735SBuffered::LCD_SetArealColor(LCD_POINT Xstart, LCD_POINT Ystart,
                                             LCD_POINT Xend, LCD_POINT Yend,
                                             LCD_COLOR Color) {

  if ((Xend <= Xstart) || (Yend <= Ystart)) {
    return;
  }

  for (LCD_POINT y = Ystart; y < Yend; ++y) {
    for (LCD_POINT x = Xstart; x < Xend; ++x) {
      buffer->buffer[y][x] = Color;
    }
  }

  if (Xstart < buffer->x_start) {
    buffer->x_start = Xstart;
  }
  if (Ystart < buffer->y_start) {
    buffer->y_start = Ystart;
  }
  if (Xend > buffer->x_end) {
    buffer->x_end = Xend;
  }
  if (Yend > buffer->y_end) {
    buffer->y_end = Yend;
  }
}

void LCD_ST7735SBuffered::LCD_Clear(LCD_COLOR Color) {
  LCD_ST7735S::LCD_Clear(Color);
  LCD_buffer_flush();

  buffer->x_start = LCD_POINT_INF;
  buffer->x_end = 0;
  buffer->y_start = LCD_POINT_INF;
  buffer->y_end = 0;
}

void LCD_ST7735SBuffered::LCD_Show(void) {
  LCD_ST7735S::LCD_Show();
  LCD_buffer_flush();
}

void LCD_ST7735SBuffered::LCD_buffer_flush() {
  LCD_SetWindows(buffer->x_start, buffer->y_start, buffer->x_end,
                 buffer->y_end);

  Write_DC(1);
  Write_CS(0);
  for (LCD_POINT y = buffer->y_start; y < buffer->y_end; ++y) {
    for (LCD_POINT x = buffer->x_start; x < buffer->x_end; ++x) {
      uint8_t buf[2] = {(uint8_t)(buffer->buffer[y][x] >> 8),
                        (uint8_t)(buffer->buffer[y][x] & 0xff)};
      spi_write_blocking(spi_port, buf, 2);
    }
  }
  Write_CS(1);

  buffer->x_start = LCD_POINT_INF;
  buffer->x_end = 0;
  buffer->y_start = LCD_POINT_INF;
  buffer->y_end = 0;
}
