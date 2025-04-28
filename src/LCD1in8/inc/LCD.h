#ifndef __LCD_H
#define __LCD_H

#include "fonts.h"
#include "hardware/spi.h"
#include "pico/stdlib.h"

/// @brief The variable type used for color representation
///        (typically uint16_t for RGB565).
#define LCD_COLOR uint16_t
/// @brief The variable type used for coordinates (X, Y).
#define LCD_POINT uint16_t
/// @brief The variable type used for lengths or dimensions (Width, Height).
#define LCD_LENGTH uint16_t

/********************************************************************************
  function:
                Define the full screen height length of the display
********************************************************************************/

// #define LCD_1IN44
#define LCD_1IN8

#if defined(LCD_1IN44)
/// @brief LCD width in pixels.
#define LCD_WIDTH 128
/// @brief LCD height in pixels.
#define LCD_HEIGHT 128
#elif defined(LCD_1IN8)
/// @brief LCD width in pixels.
#define LCD_WIDTH 160
/// @brief LCD height in pixels.
#define LCD_HEIGHT 128
#endif

/// @brief Offset for X coordinate in GRAM (ST7735 specific).
#define LCD_X 2
/// @brief Offset for Y coordinate in GRAM (ST7735 specific).
#define LCD_Y 1
/// @brief Maximum width of the LCD controller's GRAM
///        (may be larger than display width).
#define LCD_X_MAXPIXEL 132
/// @brief Maximum height of the LCD controller's GRAM
///        (may be larger than display height).
#define LCD_Y_MAXPIXEL 162

/********************************************************************************
  function:
                        scanning method (Memory Access Control)
********************************************************************************/
/// @brief LCD scan direction
typedef enum {
  /// Left to Right, Up to Down
  L2R_U2D = 0,
  /// Left to Right, Down to Up
  L2R_D2U,
  /// Right to Left, Up to Down
  R2L_U2D,
  /// Right to Left, Down to Up
  R2L_D2U,
  /// Up to Down, Left to Right
  U2D_L2R,
  /// Up to Down, Right to Left
  U2D_R2L,
  /// Down to Up, Left to Right
  D2U_L2R,
  /// Down to Up, Right to Left
  D2U_R2L,
} LCD_SCAN_DIR;
/// @brief Default scan direction.
#define SCAN_DIR_DFT L2R_U2D

/********************************************************************************
  function:
        Defines the display area parameters based on scan direction
********************************************************************************/
/// @brief Display area parameters
typedef struct {
  /// Display column width (horizontal size).
  LCD_LENGTH LCD_Dis_Column;
  /// Display page height (vertical size).
  LCD_LENGTH LCD_Dis_Page;
  /// Current scan direction.
  LCD_SCAN_DIR LCD_Scan_Dir;
  /// X coordinate adjustment for GRAM offset.
  LCD_POINT LCD_X_Adjust;
  /// Y coordinate adjustment for GRAM offset.
  LCD_POINT LCD_Y_Adjust;
} LCD_DIS;
/// @brief Global instance holding the current display parameters.
extern LCD_DIS sLCD_DIS;

/********************************************************************************
  function:
                        dot pixel size
********************************************************************************/
/// @brief Dot pixel size
typedef enum {
  /// 1x1 pixel dot
  DOT_PIXEL_1X1 = 1,
  /// 2x2 pixel dot
  DOT_PIXEL_2X2,
  /// 3x3 pixel dot
  DOT_PIXEL_3X3,
  /// 4x4 pixel dot
  DOT_PIXEL_4X4,
  /// 5x5 pixel dot
  DOT_PIXEL_5X5,
  /// 6x6 pixel dot
  DOT_PIXEL_6X6,
  /// 7x7 pixel dot
  DOT_PIXEL_7X7,
  /// 8x8 pixel dot
  DOT_PIXEL_8X8,
} DOT_PIXEL;
/// @brief Default dot pixel size.
#define DOT_PIXEL_DFT DOT_PIXEL_1X1

/********************************************************************************
  function:
                        dot Fill style (for larger dot pixels)
********************************************************************************/
/// @brief Dot fill style
typedef enum {
  /// Fill the square block
  DOT_FILL_AROUND = 1,
  /// Fill the top right part (not commonly used)
  DOT_FILL_RIGHTUP,
} DOT_STYLE;
/// @brief Default dot fill style.
#define DOT_STYLE_DFT DOT_FILL_AROUND
/********************************************************************************
  function:
                        solid line and dotted line style
********************************************************************************/
/// @brief Line style
typedef enum {
  /// Solid line
  LINE_SOLID = 0,
  /// Dotted line
  LINE_DOTTED,
} LINE_STYLE;

/********************************************************************************
  function:
                        DRAW Internal fill options (for shapes)
********************************************************************************/
/// @brief Fill style
typedef enum {
  /// Draw outline only
  DRAW_EMPTY = 0,
  /// Fill the shape
  DRAW_FULL,
} DRAW_FILL;

/********************************************************************************
  function:
                        Defines commonly used colors for the display (RGB565
format)
********************************************************************************/
/// @brief Default background color for the display.
#define LCD_BACKGROUND WHITE
/// @brief Default background color for fonts.
#define FONT_BACKGROUND WHITE
/// @brief Default foreground color for fonts.
#define FONT_FOREGROUND GRED

/// @brief White color
#define WHITE 0xFFFF
/// @brief Black color
#define BLACK 0x0000
/// @brief Blue color
#define BLUE 0x001F
/// @brief Blue-Red (Magenta) color
#define BRED 0XF81F
/// @brief Green-Red (Yellow) color
#define GRED 0XFFE0
/// @brief Green-Blue (Cyan) color
#define GBLUE 0X07FF
/// @brief Red color
#define RED 0xF800
/// @brief Magenta color
#define MAGENTA 0xF81F
/// @brief Green color
#define GREEN 0x07E0
/// @brief Cyan color
#define CYAN 0x7FFF
/// @brief Yellow color
#define YELLOW 0xFFE0
/// @brief Brown color
#define BROWN 0XBC40
/// @brief Brownish Red color
#define BRRED 0XFC07
/// @brief Gray color
#define GRAY 0X8430

/********************************************************************************
  function:
                        LCD Driver class for ST7735S controller
********************************************************************************/
class LCD_ST7735S {
protected:
  /**
   * @brief Perform hardware reset for the LCD.
   */
  void LCD_Reset(void);
  /**
   * @brief Control the Chip Select (CS) pin.
   * @param[in] Val: Pin state (0=Low/Active, 1=High/Inactive).
   */
  void Write_CS(bool Val);
  /**
   * @brief Control the Data/Command (DC) pin.
   * @param[in] Val: Pin state (0=Command, 1=Data).
   */
  void Write_DC(bool Val);
  /**
   * @brief Control the Reset (RST) pin.
   * @param[in] Val: Pin state (0=Low/Active, 1=High/Inactive).
   */
  void Write_RST(bool Val);
  /**
   * @brief Control the Backlight (BL) pin.
   * @param[in] Val: Pin state (0=Off, 1=On).
   */
  void Write_BL(bool Val);
  /**
   * @brief Send an 8-bit command register value to the LCD.
   * @param[in] Reg: The command register address.
   */
  void LCD_WriteReg(uint8_t Reg);
  /**
   * @brief Send 8-bit data to the LCD.
   * @param[in] Data: The data byte to send.
   */
  void LCD_WriteData_8Bit(uint8_t Data);
  /**
   * @brief Send an array of 8-bit data to the LCD.
   * @param[in] Data: Pointer to the data array.
   * @param[in] DataLen: Number of bytes in the array.
   */
  void LCD_WriteData_8BitArray(uint8_t *Data, size_t DataLen);
  /**
   * @brief Send 16-bit data (e.g., color) to the LCD.
   * @param[in] Data: The 16-bit data value.
   */
  void LCD_WriteData_16Bit(uint16_t Data);
  /**
   * @brief Send the same 16-bit data multiple times to the LCD.
   * @param[in] Data: The 16-bit data value to repeat.
   * @param[in] DataLen: The number of times to send the data.
   */
  void LCD_WriteData_NLen16Bit(uint16_t Data, uint32_t DataLen);
  /**
   * @brief Send an array of 16-bit data to the LCD.
   * @param[in] Data: Pointer to the 16-bit data array.
   * @param[in] DataLen: Number of 16-bit values in the array.
   */
  void LCD_WriteData_16BitArray(uint16_t *Data, size_t DataLen);
  /**
   * @brief Send a 2D array of 16-bit data to the LCD (row by row).
   * @param[in] Data: Pointer to the 2D array (array of pointers to rows).
   * @param[in] DataRowN: Number of rows.
   * @param[in] DataColN: Number of columns (elements per row).
   */
  void LCD_WriteData_16Bit2DArray(uint16_t **Data, size_t DataRowN,
                                  size_t DataColN);

  /**
   * @brief Send initialization commands to the LCD controller.
   */
  void LCD_InitReg(void);
  /**
   * @brief Set the LCD's scan direction (Memory Access Control).
   * @param[in] Scan_dir: The desired scan direction enum value.
   */
  void LCD_SetGramScanWay(LCD_SCAN_DIR Scan_dir);

  /// @brief Pointer to the SPI instance used for communication.
  spi_inst_t *spi_port;
  /// @brief GPIO pin number for Chip Select (CS).
  uint pin_cs;
  /// @brief GPIO pin number for Data/Command (DC).
  uint pin_dc;
  /// @brief GPIO pin number for Reset (RST).
  uint pin_rst;
  /// @brief GPIO pin number for Backlight (BL).
  uint pin_bl;

public:
  /**
   * @brief Construct a new LCD_ST7735S object.
   * @param[out] spi_port: Pointer to the SPI instance (e.g., spi0, spi1).
   * @param[in] pin_cs: GPIO pin number for CS.
   * @param[in] pin_dc: GPIO pin number for DC.
   * @param[in] pin_rst: GPIO pin number for RST.
   * @param[in] pin_bl: GPIO pin number for BL.
   */
  LCD_ST7735S(spi_inst_t *spi_port, uint pin_cs, uint pin_dc, uint pin_rst,
              uint pin_bl);
  /**
   * @brief Initialize the LCD controller.
   *        Performs reset, sends initialization commands, sets scan direction,
   * and clears the screen.
   * @param[in] Lcd_ScanDir: The desired initial scan direction.
   */
  void LCD_Init(LCD_SCAN_DIR Lcd_ScanDir);

  // LCD set cursor + windows + color
  // ----------------------------------------
  
  /**
   * @brief Set the active drawing window (GRAM area).
   * @param[in] Xstart: Starting X coordinate.
   * @param[in] Ystart: Starting Y coordinate.
   * @param[in] Xend: Ending X coordinate.
   * @param[in] Yend: Ending Y coordinate.
   */
  void LCD_SetWindows(LCD_POINT Xstart, LCD_POINT Ystart, LCD_POINT Xend,
                      LCD_POINT Yend);
  /**
   * @brief Set the cursor position for subsequent data writes
   *        (sets a 1x1 window).
   * @param[in] Xpoint: X coordinate.
   * @param[in] Ypoint: Y coordinate.
   */
  void LCD_SetCursor(LCD_POINT Xpoint, LCD_POINT Ypoint);
  /**
   * @brief Set the color of a single pixel
   * @param[in] Color: Color value.
   * @param[in] Xpoint: X coordinate.
   * @param[in] Ypoint: Y coordinate.
   */
  void LCD_SetColor(LCD_COLOR Color, LCD_POINT Xpoint, LCD_POINT Ypoint);
  /**
   * @brief Set the color of a single pixel at the specified coordinates.
   *        This is the preferred method for drawing single pixels.
   * @param[in] Xpoint: X coordinate.
   * @param[in] Ypoint: Y coordinate.
   * @param[in] Color: Color value.
   */
  virtual void LCD_SetPointlColor(LCD_POINT Xpoint, LCD_POINT Ypoint,
                                  LCD_COLOR Color);
  /**
   * @brief Fill a rectangular area with a specified color.
   * @param[in] Xstart: Starting X coordinate.
   * @param[in] Ystart: Starting Y coordinate.
   * @param[in] Xend: Ending X coordinate.
   * @param[in] Yend: Ending Y coordinate.
   * @param[in] Color: Color value to fill with.
   */
  virtual void LCD_SetArealColor(LCD_POINT Xstart, LCD_POINT Ystart,
                                 LCD_POINT Xend, LCD_POINT Yend,
                                 LCD_COLOR Color);
  /**
   * @brief Clear the entire display with a specified color.
   * @param[in] Color: Color value to clear with.
   */
  virtual void LCD_Clear(LCD_COLOR Color);

  // Drawing
  // ----------------------------------------

  /**
   * @brief Draw a point (potentially larger than 1x1 pixel).
   * @param[in] Xpoint: X coordinate of the top-left corner.
   * @param[in] Ypoint: Y coordinate of the top-left corner.
   * @param[in] Color: Color value.
   * @param[in] Dot_Pixel: Size of the dot
   * @param[in] Dot_FillWay: Style of filling
   */
  void LCD_DrawPoint(LCD_POINT Xpoint, LCD_POINT Ypoint, LCD_COLOR Color,
                     DOT_PIXEL Dot_Pixel, DOT_STYLE Dot_FillWay);
  /**
   * @brief Draw a line between two points.
   * @param[in] Xstart: Starting X coordinate.
   * @param[in] Ystart: Starting Y coordinate.
   * @param[in] Xend: Ending X coordinate.
   * @param[in] Yend: Ending Y coordinate.
   * @param[in] Color: Color value.
   * @param[in] Line_Style: Style of the line
   * @param[in] Dot_Pixel: Thickness of the line
   */
  void LCD_DrawLine(LCD_POINT Xstart, LCD_POINT Ystart, LCD_POINT Xend,
                    LCD_POINT Yend, LCD_COLOR Color, LINE_STYLE Line_Style,
                    DOT_PIXEL Dot_Pixel);
  /**
   * @brief Draw a rectangle.
   * @param[in] Xstart: Starting X coordinate (top-left).
   * @param[in] Ystart: Starting Y coordinate (top-left).
   * @param[in] Xend: Ending X coordinate (bottom-right).
   * @param[in] Yend: Ending Y coordinate (bottom-right).
   * @param[in] Color: Color value.
   * @param[in] Filled: Fill style
   * @param[in] Dot_Pixel: Thickness of the outline
   */
  void LCD_DrawRectangle(LCD_POINT Xstart, LCD_POINT Ystart, LCD_POINT Xend,
                         LCD_POINT Yend, LCD_COLOR Color, DRAW_FILL Filled,
                         DOT_PIXEL Dot_Pixel);
  /**
   * @brief Draw a circle.
   * @param[in] X_Center: X coordinate of the center.
   * @param[in] Y_Center: Y coordinate of the center.
   * @param[in] Radius: Radius of the circle.
   * @param[in] Color: Color value.
   * @param[in] Draw_Fill: Fill style
   * @param[in] Dot_Pixel: Thickness of the outline
   */
  void LCD_DrawCircle(LCD_POINT X_Center, LCD_POINT Y_Center, LCD_LENGTH Radius,
                      LCD_COLOR Color, DRAW_FILL Draw_Fill,
                      DOT_PIXEL Dot_Pixel);
  /**
   * @brief Display an image from a 2D color array.
   * @param[in] Xstart: Starting X coordinate on the LCD.
   * @param[in] Ystart: Starting Y coordinate on the LCD.
   * @param[in] Xend: Ending X coordinate on the LCD.
   * @param[in] Yend: Ending Y coordinate on the LCD.
   * @param[in] img: Pointer to a 2D array (array of row pointers) containing
   *                 LCD_COLOR data. The dimensions of the image data should match
   *                 (Xend-Xstart) x (Yend-Ystart).
   */
  void LCD_DrawImage(LCD_POINT Xstart, LCD_POINT Ystart, LCD_POINT Xend,
                     LCD_POINT Yend, LCD_COLOR **img);

  // Display string
  /**
   * @brief Display a single ASCII character.
   * @param[in] Xstart: Starting X coordinate (top-left).
   * @param[in] Ystart: Starting Y coordinate (top-left).
   * @param[in] Acsii_Char: The character to display.
   * @param[in] Font: Pointer to the font definition structure.
   * @param[in] Color_Background: Background color for the character.
   * @param[in] Color_Foreground: Foreground color for the character.
   */
  void LCD_DisplayChar(LCD_POINT Xstart, LCD_POINT Ystart,
                       const char Acsii_Char, sFONT *Font,
                       LCD_COLOR Color_Background, LCD_COLOR Color_Foreground);
  /**
   * @brief Display a null-terminated string.
   * @param[in] Xstart: Starting X coordinate (top-left of the first character).
   * @param[in] Ystart: Starting Y coordinate (top-left of the first character).
   * @param[in] pString: Pointer to the null-terminated string.
   * @param[in] Font: Pointer to the font definition structure.
   * @param[in] Color_Background: Background color for the string.
   * @param[in] Color_Foreground: Foreground color for the string.
   */
  void LCD_DisplayString(LCD_POINT Xstart, LCD_POINT Ystart,
                         const char *pString, sFONT *Font,
                         LCD_COLOR Color_Background,
                         LCD_COLOR Color_Foreground);
  /**
   * @brief Display a signed 32-bit integer number.
   * @param[in] Xpoint: Starting X coordinate (top-left).
   * @param[in] Ypoint: Starting Y coordinate (top-left).
   * @param[in] Nummber: The number to display.
   * @param[in] Font: Pointer to the font definition structure.
   * @param[in] Color_Background: Background color for the number.
   * @param[in] Color_Foreground: Foreground color for the number.
   */
  void LCD_DisplayNum(LCD_POINT Xpoint, LCD_POINT Ypoint, int32_t Nummber,
                      sFONT *Font, LCD_COLOR Color_Background,
                      LCD_COLOR Color_Foreground);
  /**
   * @brief Display a test pattern or example content.
   *        Marked virtual for potential buffering.
   */
  virtual void LCD_Show(void);

  /// @brief Structure holding the current display parameters
  ///        (dimensions, scan direction, offsets).
  LCD_DIS sLCD_DIS;
};
#endif
