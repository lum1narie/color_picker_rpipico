#ifndef __LCD_H
#define __LCD_H

#include "fonts.h"
#include "hardware/spi.h"
#include "pico/stdlib.h"

#define LCD_COLOR uint16_t  // The variable type of the color
#define LCD_POINT uint16_t  // The type of coordinate
#define LCD_LENGTH uint16_t // The type of coordinate

/********************************************************************************
  function:
                Define the full screen height length of the display
********************************************************************************/
// #define LCD_1IN44
#define LCD_1IN8
#if defined(LCD_1IN44)
#define LCD_WIDTH 128  // LCD width
#define LCD_HEIGHT 128 // LCD height
#elif defined(LCD_1IN8)
#define LCD_WIDTH 160  // LCD width
#define LCD_HEIGHT 128 // LCD height
#endif

#define LCD_X 2
#define LCD_Y 1
#define LCD_X_MAXPIXEL 132 // LCD width maximum memory
#define LCD_Y_MAXPIXEL 162 // LCD height maximum memory

/********************************************************************************
  function:
                        scanning method
********************************************************************************/
typedef enum {
  L2R_U2D = 0, // The display interface is displayed , left to right, up to down
  L2R_D2U,
  R2L_U2D,
  R2L_D2U,

  U2D_L2R,
  U2D_R2L,
  D2U_L2R,
  D2U_R2L,
} LCD_SCAN_DIR;
#define SCAN_DIR_DFT L2R_U2D // Default scan direction = L2R_U2D

/********************************************************************************
  function:
        Defines the total number of rows in the display area
********************************************************************************/

typedef struct {
  LCD_LENGTH LCD_Dis_Column; // COLUMN
  LCD_LENGTH LCD_Dis_Page;   // PAGE
  LCD_SCAN_DIR LCD_Scan_Dir;
  LCD_POINT LCD_X_Adjust; // LCD x actual display position calibration
  LCD_POINT LCD_Y_Adjust; // LCD y actual display position calibration
} LCD_DIS;
extern LCD_DIS sLCD_DIS;

/********************************************************************************
  function:
                        dot pixel
********************************************************************************/
typedef enum {
  DOT_PIXEL_1X1 = 1, // dot pixel 1 x 1
  DOT_PIXEL_2X2,     // dot pixel 2 X 2
  DOT_PIXEL_3X3,     // dot pixel 3 X 3
  DOT_PIXEL_4X4,     // dot pixel 4 X 4
  DOT_PIXEL_5X5,     // dot pixel 5 X 5
  DOT_PIXEL_6X6,     // dot pixel 6 X 6
  DOT_PIXEL_7X7,     // dot pixel 7 X 7
  DOT_PIXEL_8X8,     // dot pixel 8 X 8
} DOT_PIXEL;
#define DOT_PIXEL_DFT DOT_PIXEL_1X1 // Default dot pilex

/********************************************************************************
  function:
                        dot Fill style
********************************************************************************/
typedef enum {
  DOT_FILL_AROUND = 1, // dot pixel 1 x 1
  DOT_FILL_RIGHTUP,    // dot pixel 2 X 2
} DOT_STYLE;
#define DOT_STYLE_DFT DOT_FILL_AROUND // Default dot pilex
/********************************************************************************
  function:
                        solid line and dotted line
********************************************************************************/
typedef enum {
  LINE_SOLID = 0,
  LINE_DOTTED,
} LINE_STYLE;

/********************************************************************************
  function:
                        DRAW Internal fill
********************************************************************************/
typedef enum {
  DRAW_EMPTY = 0,
  DRAW_FULL,
} DRAW_FILL;

/********************************************************************************
  function:
                        Defines commonly used colors for the display
********************************************************************************/
#define LCD_BACKGROUND WHITE  // Default background color
#define FONT_BACKGROUND WHITE // Default font background color
#define FONT_FOREGROUND GRED  // Default font foreground color

#define WHITE 0xFFFF
#define BLACK 0x0000
#define BLUE 0x001F
#define BRED 0XF81F
#define GRED 0XFFE0
#define GBLUE 0X07FF
#define RED 0xF800
#define MAGENTA 0xF81F
#define GREEN 0x07E0
#define CYAN 0x7FFF
#define YELLOW 0xFFE0
#define BROWN 0XBC40
#define BRRED 0XFC07
#define GRAY 0X8430

/********************************************************************************
  function:
                        Macro definition variable name
********************************************************************************/
class LCD_ST7735S {
  void LCD_Reset(void);
  void Write_CS(bool Val);
  void Write_DC(bool Val);
  void Write_RST(bool Val);
  void Write_BL(bool Val);
  void LCD_WriteReg(uint8_t Reg);
  void LCD_WriteData_8Bit(uint8_t Data);
  void LCD_WriteData_8BitArray(uint8_t *Data, size_t DataLen);
  void LCD_WriteData_16Bit(uint16_t Data);
  void LCD_WriteData_NLen16Bit(uint16_t Data, uint32_t DataLen);
  void LCD_WriteData_16BitArray(uint16_t *Data, size_t DataLen);
  void LCD_InitReg(void);
  void LCD_SetGramScanWay(LCD_SCAN_DIR Scan_dir);

  spi_inst_t *spi_port;
  uint pin_cs;
  uint pin_dc;
  uint pin_rst;
  uint pin_bl;

public:
  LCD_ST7735S(spi_inst_t *spi_port, uint pin_cs, uint pin_dc, uint pin_rst,
              uint pin_bl);
  void LCD_Init(LCD_SCAN_DIR Lcd_ScanDir);

  // LCD set cursor + windows + color
  void LCD_SetWindows(LCD_POINT Xstart, LCD_POINT Ystart, LCD_POINT Xend,
                      LCD_POINT Yend);
  void LCD_SetCursor(LCD_POINT Xpoint, LCD_POINT Ypoint);
  void LCD_SetColor(LCD_COLOR Color, LCD_POINT Xpoint, LCD_POINT Ypoint);
  void LCD_SetPointlColor(LCD_POINT Xpoint, LCD_POINT Ypoint, LCD_COLOR Color);
  void LCD_SetArealColor(LCD_POINT Xstart, LCD_POINT Ystart, LCD_POINT Xend,
                         LCD_POINT Yend, LCD_COLOR Color);
  void LCD_Clear(LCD_COLOR Color);

  // Drawing
  void LCD_DrawPoint(LCD_POINT Xpoint, LCD_POINT Ypoint, LCD_COLOR Color,
                     DOT_PIXEL Dot_Pixel, DOT_STYLE Dot_FillWay);
  void LCD_DrawLine(LCD_POINT Xstart, LCD_POINT Ystart, LCD_POINT Xend,
                    LCD_POINT Yend, LCD_COLOR Color, LINE_STYLE Line_Style,
                    DOT_PIXEL Dot_Pixel);
  void LCD_DrawRectangle(LCD_POINT Xstart, LCD_POINT Ystart, LCD_POINT Xend,
                         LCD_POINT Yend, LCD_COLOR Color, DRAW_FILL Filled,
                         DOT_PIXEL Dot_Pixel);
  void LCD_DrawCircle(LCD_POINT X_Center, LCD_POINT Y_Center, LCD_LENGTH Radius,
                      LCD_COLOR Color, DRAW_FILL Draw_Fill,
                      DOT_PIXEL Dot_Pixel);
  void LCD_DrawImage(LCD_POINT Xstart, LCD_POINT Ystart, LCD_POINT Xend,
                     LCD_POINT Yend, LCD_COLOR *img);

  // Display string
  void LCD_DisplayChar(LCD_POINT Xstart, LCD_POINT Ystart,
                       const char Acsii_Char, sFONT *Font,
                       LCD_COLOR Color_Background, LCD_COLOR Color_Foreground);
  void LCD_DisplayString(LCD_POINT Xstart, LCD_POINT Ystart,
                         const char *pString, sFONT *Font,
                         LCD_COLOR Color_Background,
                         LCD_COLOR Color_Foreground);
  void LCD_DisplayNum(LCD_POINT Xpoint, LCD_POINT Ypoint, int32_t Nummber,
                      sFONT *Font, LCD_COLOR Color_Background,
                      LCD_COLOR Color_Foreground);
  void LCD_Show(void);
};
extern LCD_ST7735S LCD;
#endif
