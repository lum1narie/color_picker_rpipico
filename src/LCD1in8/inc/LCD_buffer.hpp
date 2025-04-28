#ifndef __LCD_BUFFER_HPP
#define __LCD_BUFFER_HPP

#include "LCD.h"
#include "hardware/spi.h"
#include "pico/stdlib.h"

/// @brief Defines the maximum dimension of the square buffer
///        (using LCD Y max pixel). 
#define BUFFER_MAXPIXEL LCD_Y_MAXPIXEL

/// @brief Represents an infinite or invalid point, used for initializing buffer bounds. 
#define LCD_POINT_INF 0xFFFF

/**
 * @brief Structure representing the frame buffer.
 *        Holds the pixel data and the bounding box of the modified area.
 */
struct LCDBuffer {
  /// @brief 2D array storing the color of each pixel in the buffer. 
  LCD_COLOR buffer[BUFFER_MAXPIXEL][BUFFER_MAXPIXEL];
  /// @brief Minimum X coordinate of the modified area (inclusive). 
  LCD_POINT x_start;
  /// @brief Minimum Y coordinate of the modified area (inclusive). 
  LCD_POINT y_start;
  /// @brief Maximum X coordinate of the modified area (exclusive). 
  LCD_POINT x_end;
  /// @brief Maximum Y coordinate of the modified area (exclusive). 
  LCD_POINT y_end;

  /**
   * @brief Construct a new LCDBuffer object.
   *        Initializes the dirty area bounds to encompass no area.
   */
  LCDBuffer();
};

/**
 * @brief LCD driver class with an internal frame buffer.
 *        Inherits from LCD_ST7735S and overrides drawing methods
 *        to write to the buffer first.
 *        The buffer needs to be explicitly flushed to the display
 *        using LCD_buffer_flush().
 */
class LCD_ST7735SBuffered : public LCD_ST7735S {
protected:
  /// @brief Pointer to the internal frame buffer structure. 
  LCDBuffer *buffer = NULL;
  /**
   * @brief Initialize the frame buffer (allocate memory if needed).
   */
  void LCD_buffer_init();

public:
  /**
   * @brief Construct a new LCD_ST7735SBuffered object.
   * @param[out] spi_port: Pointer to the SPI instance (e.g., spi0, spi1).
   * @param[in] pin_cs: GPIO pin number for CS.
   * @param[in] pin_dc: GPIO pin number for DC.
   * @param[in] pin_rst: GPIO pin number for RST.
   * @param[in] pin_bl: GPIO pin number for BL.
   */
  LCD_ST7735SBuffered(spi_inst_t *spi_port, uint pin_cs, uint pin_dc,
                       uint pin_rst, uint pin_bl);

  /**
   * @brief Set the color of a single pixel in the buffer.
   *        Updates the buffer and the dirty area bounds.
   * @param[in] Xpoint: X coordinate.
   * @param[in] Ypoint: Y coordinate.
   * @param[in] Color: Color value.
   */
  void LCD_SetPointlColor(LCD_POINT Xpoint, LCD_POINT Ypoint,
                          LCD_COLOR Color) override;
  /**
   * @brief Fill a rectangular area in the buffer with a specified color.
   *        Updates the buffer and the dirty area bounds.
   * @param[in] Xstart: Starting X coordinate.
   * @param[in] Ystart: Starting Y coordinate.
   * @param[in] Xend: Ending X coordinate.
   * @param[in] Yend: Ending Y coordinate.
   * @param[in] Color: Color value to fill with.
   */
  void LCD_SetArealColor(LCD_POINT Xstart, LCD_POINT Ystart, LCD_POINT Xend,
                         LCD_POINT Yend, LCD_COLOR Color) override;
  /**
   * @brief Clear the entire display and the buffer with a specified color.
   *        Immediately flushes the clear operation to the hardware display.
   *        Resets the buffer's dirty area.
   * @param[in] Color: Color value to clear with.
   */
  void LCD_Clear(LCD_COLOR Color) override;
  /**
   * @brief Display a test pattern or example content (writes to buffer).
   *        Also flushes the buffer content to the display afterwards.
   */
  void LCD_Show(void) override;

  /**
   * @brief Flush the modified portion of the buffer to the LCD hardware.
   *        Only the area defined by (x_start, y_start) to (x_end, y_end) is transferred.
   *        Resets the dirty area bounds after flushing.
   */
  void LCD_buffer_flush();
};

#endif
