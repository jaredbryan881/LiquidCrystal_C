#ifndef LIQUIDCRYSTAL_C_H
#define LIQUIDCRYSTAL_C_H

#include <stdint.h>
#include <stdbool.h>
#include "MCP23008.h"  // From https://github.com/m1geo/MCP23008_STM32

/******************************************************************************
 * HD44780 Instruction Set
 ******************************************************************************/
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME   0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT  0x10
#define LCD_FUNCTIONSET  0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT            0x00
#define LCD_ENTRYLEFT             0x02
#define LCD_ENTRYSHIFTINCREMENT   0x01
#define LCD_ENTRYSHIFTDECREMENT   0x00

// flags for display on/off control
#define LCD_DISPLAYON   0x04
#define LCD_DISPLAYOFF  0x00
#define LCD_CURSORON    0x02
#define LCD_CURSOROFF   0x00
#define LCD_BLINKON     0x01
#define LCD_BLINKOFF    0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE  0x00
#define LCD_MOVERIGHT   0x04
#define LCD_MOVELEFT    0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE    0x08
#define LCD_1LINE    0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS  0x00

/******************************************************************************
 * LiquidCrystal_C structure
 ******************************************************************************/
typedef struct {
    MCP23008_HandleTypeDef *mcp; // Point to the MCP23008 handle

    // Pin mappings on the MCP23008
    uint8_t rs_pin;
    uint8_t rw_pin; // set to 255 (or 0xFF) if unused (tied to ground)
    uint8_t enable_pin;
    uint8_t data_pins[8];

    // Display state
    uint8_t displayfunction;
    uint8_t displaycontrol;
    uint8_t displaymode;

    uint8_t numlines;
    uint8_t currline;
} LiquidCrystal_C;

/*******************************************************************************
 * Public API
 ******************************************************************************/
// Initialization functions, just splitting the struct/constructor initialization and the hardware initialization.
// Initialize data structures
void LCD_Init(LiquidCrystal_C *lcd,
              MCP23008_HandleTypeDef *mcp,
              uint8_t fourbitmode,
              uint8_t rs_pin, uint8_t rw_pin, uint8_t enable_pin,
              uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
              uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7);
// Send the actual commands
bool LCD_Begin(LiquidCrystal_C *lcd, uint8_t cols, uint8_t lines, uint8_t dotsize);

// Basic display commands
// Clear LCD and return cursor to (0,0)
void LCD_Clear(LiquidCrystal_C *lcd);
// Return cursor to (0,0)
void LCD_Home(LiquidCrystal_C *lcd);
// Set cursor to (col, row)
void LCD_SetCursor(LiquidCrystal_C *lcd, uint8_t col, uint8_t row);
// Turn off LCD display (note that text remains in memory)
void LCD_NoDisplay(LiquidCrystal_C *lcd);
// Turn on display
void LCD_Display(LiquidCrystal_C *lcd);
// Hide cursor
void LCD_NoCursor(LiquidCrystal_C *lcd);
// Show cursor
void LCD_Cursor(LiquidCrystal_C *lcd);
// Disable cursor blinking
void LCD_NoBlink(LiquidCrystal_C *lcd);
// Enable cursor blinking
void LCD_Blink(LiquidCrystal_C *lcd);

// Display shifting
// Scroll display one char to the left
void LCD_ScrollDisplayLeft(LiquidCrystal_C *lcd);
// Scroll display one char to the right
void LCD_ScrollDisplayRight(LiquidCrystal_C *lcd);

// Text direction control
// Set text direction so new chars go left-to-right
void LCD_LeftToRight(LiquidCrystal_C *lcd);
// Set text direction so new chars go right-to-left
void LCD_RightToLeft(LiquidCrystal_C *lcd);
// Enable autoscroll
void LCD_Autoscroll(LiquidCrystal_C *lcd);
// Disable autoscroll
void LCD_NoAutoscroll(LiquidCrystal_C *lcd);

// Create custom char in locations 0..7
void LCD_CreateChar(LiquidCrystal_C *lcd, uint8_t location, const uint8_t charmap[8]);

// Write a single character (mimicking Adafruit's write(uint8_t)).
void LCD_WriteChar(LiquidCrystal_C *lcd, uint8_t value);
// Write a string to the LCD.
void LCD_WriteString(LiquidCrystal_C *lcd, const char *str);

// Toggle the backlight of the LCD (if it's supported)
void LCD_SetBacklight(LiquidCrystal_C *lcd, bool on);

#endif
