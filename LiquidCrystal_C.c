#include "LiquidCrystal_C.h"
#include "stm32f4xx_hal.h" // For HAL_Delay, etc. Replace with stm32f1xx_hal.h or whatever hardware you're using

/*******************************************************************************
 * STATIC HELPER FUNCTIONS
 ******************************************************************************/

// Low-level: write a command (mode=0) or data (mode=1).
static void lcd_send(LiquidCrystal_C *lcd, uint8_t value, bool mode);
static void lcd_write4bits(LiquidCrystal_C *lcd, uint8_t value);
static void lcd_write8bits(LiquidCrystal_C *lcd, uint8_t value);
static void lcd_pulseEnable(LiquidCrystal_C *lcd);

// Send a command to the LCD (mode=false for command mode)
static void lcd_command(LiquidCrystal_C *lcd, uint8_t value) {
    lcd_send(lcd, value, false);
}

// Replicate digitalWrite() style bit setting to set or clear a bit on the MCP23008
static void lcd_digitalWrite(LiquidCrystal_C *lcd, uint8_t pin, bool level)
{
    if (pin == 0xFF) return; // if invalid or unused, skip it

    // 1) Read current GPIO state
    uint8_t current = MCP23008_ReadGPIO(lcd->mcp);
    // 2) Modify the bit
    if (level) {
        current |= (1 << pin);
    } else {
        current &= ~(1 << pin);
    }
    // 3) Write the new GPIO state
    MCP23008_WriteGPIO(lcd->mcp, current);
}

/*******************************************************************************
 * PUBLIC FUNCTIONS
 ******************************************************************************/
// Initialize the LCD structure with pin mappings (from LCD to MCP23008) and mode (4-bit or 8-bit)
void LCD_Init(LiquidCrystal_C *lcd,
              MCP23008_HandleTypeDef *mcp,
              uint8_t fourbitmode,
              uint8_t rs_pin, uint8_t rw_pin, uint8_t enable_pin,
              uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
              uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7)
{
    lcd->mcp = mcp;

    lcd->rs_pin     = rs_pin;
    lcd->rw_pin     = rw_pin;   // 255 if unused
    lcd->enable_pin = enable_pin;

    lcd->data_pins[0] = d0;
    lcd->data_pins[1] = d1;
    lcd->data_pins[2] = d2;
    lcd->data_pins[3] = d3;
    lcd->data_pins[4] = d4;
    lcd->data_pins[5] = d5;
    lcd->data_pins[6] = d6;
    lcd->data_pins[7] = d7;

    if (fourbitmode) {
        lcd->displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;
    } else {
        lcd->displayfunction = LCD_8BITMODE | LCD_1LINE | LCD_5x8DOTS;
    }

    lcd->numlines  = 1;
    lcd->currline  = 0;
}

// Finalize LCD initialization and configure display parameters
bool LCD_Begin(LiquidCrystal_C *lcd, uint8_t cols, uint8_t lines, uint8_t dotsize)
{
    // Check if we have two lines
    if (lines > 1) {
        lcd->displayfunction |= LCD_2LINE;
    }
    lcd->numlines = lines;

    // 5x10 font if we only have 1 line
    if ((dotsize != 0) && (lines == 1)) {
        lcd->displayfunction |= LCD_5x10DOTS;
    }

    // Wait for LCD power up
    HAL_Delay(50);

    // Set RS, EN, RW low
    lcd_digitalWrite(lcd, lcd->rs_pin, false);
    lcd_digitalWrite(lcd, lcd->enable_pin, false);
    if (lcd->rw_pin != 0xFF) {
        lcd_digitalWrite(lcd, lcd->rw_pin, false);
    }

    // HD44780 initialization sequence for 4-bit or 8-bit mode
    if (!(lcd->displayfunction & LCD_8BITMODE)) {
        // 4-bit mode
        lcd_write4bits(lcd, 0x03);
        HAL_Delay(5);
        lcd_write4bits(lcd, 0x03);
        HAL_Delay(5);
        lcd_write4bits(lcd, 0x03);
        HAL_Delay(1);
        lcd_write4bits(lcd, 0x02);
    } else {
        // 8-bit mode
        lcd_command(lcd, LCD_FUNCTIONSET | lcd->displayfunction);
        HAL_Delay(5);
        lcd_command(lcd, LCD_FUNCTIONSET | lcd->displayfunction);
        HAL_Delay(1);
        lcd_command(lcd, LCD_FUNCTIONSET | lcd->displayfunction);
    }

    // set lines, font size, etc.
    lcd_command(lcd, LCD_FUNCTIONSET | lcd->displayfunction);

    // turn on display, no cursor, no blink
    lcd->displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
    LCD_Display(lcd);

    // clear display
    LCD_Clear(lcd);

    // set mode: left to right, no shift
    lcd->displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
    lcd_command(lcd, LCD_ENTRYMODESET | lcd->displaymode);

    return true;
}

void LCD_Clear(LiquidCrystal_C *lcd)
{
    lcd_command(lcd, LCD_CLEARDISPLAY);
    HAL_Delay(2);
}

void LCD_Home(LiquidCrystal_C *lcd)
{
    lcd_command(lcd, LCD_RETURNHOME);
    HAL_Delay(2);
}

void LCD_SetCursor(LiquidCrystal_C *lcd, uint8_t col, uint8_t row)
{
    static const uint8_t row_offsets[4] = {0x00, 0x40, 0x14, 0x54};
    if (row >= lcd->numlines) {
        row = lcd->numlines - 1;
    }
    lcd_command(lcd, LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

void LCD_NoDisplay(LiquidCrystal_C *lcd)
{
    lcd->displaycontrol &= ~LCD_DISPLAYON;
    lcd_command(lcd, LCD_DISPLAYCONTROL | lcd->displaycontrol);
}

void LCD_Display(LiquidCrystal_C *lcd)
{
    lcd->displaycontrol |= LCD_DISPLAYON;
    lcd_command(lcd, LCD_DISPLAYCONTROL | lcd->displaycontrol);
}

void LCD_NoCursor(LiquidCrystal_C *lcd)
{
    lcd->displaycontrol &= ~LCD_CURSORON;
    lcd_command(lcd, LCD_DISPLAYCONTROL | lcd->displaycontrol);
}

void LCD_Cursor(LiquidCrystal_C *lcd)
{
    lcd->displaycontrol |= LCD_CURSORON;
    lcd_command(lcd, LCD_DISPLAYCONTROL | lcd->displaycontrol);
}

void LCD_NoBlink(LiquidCrystal_C *lcd)
{
    lcd->displaycontrol &= ~LCD_BLINKON;
    lcd_command(lcd, LCD_DISPLAYCONTROL | lcd->displaycontrol);
}

void LCD_Blink(LiquidCrystal_C *lcd)
{
    lcd->displaycontrol |= LCD_BLINKON;
    lcd_command(lcd, LCD_DISPLAYCONTROL | lcd->displaycontrol);
}

void LCD_ScrollDisplayLeft(LiquidCrystal_C *lcd)
{
    lcd_command(lcd, LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}

void LCD_ScrollDisplayRight(LiquidCrystal_C *lcd)
{
    lcd_command(lcd, LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

void LCD_LeftToRight(LiquidCrystal_C *lcd)
{
    lcd->displaymode |= LCD_ENTRYLEFT;
    lcd_command(lcd, LCD_ENTRYMODESET | lcd->displaymode);
}

void LCD_RightToLeft(LiquidCrystal_C *lcd)
{
    lcd->displaymode &= ~LCD_ENTRYLEFT;
    lcd_command(lcd, LCD_ENTRYMODESET | lcd->displaymode);
}

void LCD_Autoscroll(LiquidCrystal_C *lcd)
{
    lcd->displaymode |= LCD_ENTRYSHIFTINCREMENT;
    lcd_command(lcd, LCD_ENTRYMODESET | lcd->displaymode);
}

void LCD_NoAutoscroll(LiquidCrystal_C *lcd)
{
    lcd->displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
    lcd_command(lcd, LCD_ENTRYMODESET | lcd->displaymode);
}

void LCD_CreateChar(LiquidCrystal_C *lcd, uint8_t location, const uint8_t charmap[8])
{
    location &= 0x7;
    lcd_command(lcd, LCD_SETCGRAMADDR | (location << 3));
    for (int i = 0; i < 8; i++) {
        LCD_WriteChar(lcd, charmap[i]);
    }
}

void LCD_WriteChar(LiquidCrystal_C *lcd, uint8_t value)
{
    lcd_send(lcd, value, true); // mode=true => data
}

void LCD_WriteString(LiquidCrystal_C *lcd, const char *str)
{
    while (*str) {
        LCD_WriteChar(lcd, (uint8_t)*str++);
    }
}

void LCD_SetBacklight(LiquidCrystal_C *lcd, bool on)
{
    if (on) {
        // set bit 7
        uint8_t current = MCP23008_ReadGPIO(lcd->mcp);
        current |= (1 << 7);
        MCP23008_WriteGPIO(lcd->mcp, current);
    } else {
        // clear bit 7
        uint8_t current = MCP23008_ReadGPIO(lcd->mcp);
        current &= ~(1 << 7);
        MCP23008_WriteGPIO(lcd->mcp, current);
    }
}

/*******************************************************************************
 * STATIC HELPER IMPLEMENTATIONS
 ******************************************************************************/
// Send a byte either as a command (mode=false) or data (mode=true)
static void lcd_send(LiquidCrystal_C *lcd, uint8_t value, bool mode)
{
    lcd_digitalWrite(lcd, lcd->rs_pin, mode);
    if (lcd->rw_pin != 0xFF) {
        lcd_digitalWrite(lcd, lcd->rw_pin, false);
    }

    // check if 4-bit or 8-bit
    if (lcd->displayfunction & LCD_8BITMODE) {
        lcd_write8bits(lcd, value);
    } else {
        lcd_write4bits(lcd, (value >> 4) & 0x0F);
        lcd_write4bits(lcd, value & 0x0F);
    }
}

// Writeh te lower 4-bits to D0..D3
static void lcd_write4bits(LiquidCrystal_C *lcd, uint8_t value)
{
    for (int i = 0; i < 4; i++) {
        bool bitState = (value >> i) & 0x01;
        lcd_digitalWrite(lcd, lcd->data_pins[i], bitState);
    }
    lcd_pulseEnable(lcd);
}

// Write the full 8-bits to D0..D7
static void lcd_write8bits(LiquidCrystal_C *lcd, uint8_t value)
{
    for (int i = 0; i < 8; i++) {
        bool bitState = (value >> i) & 0x01;
        lcd_digitalWrite(lcd, lcd->data_pins[i], bitState);
    }
    lcd_pulseEnable(lcd);
}

// Pulse enable pin to latch the data into the LCD
static void lcd_pulseEnable(LiquidCrystal_C *lcd)
{
    lcd_digitalWrite(lcd, lcd->enable_pin, false);
    HAL_Delay(1);
    lcd_digitalWrite(lcd, lcd->enable_pin, true);
    HAL_Delay(1);
    lcd_digitalWrite(lcd, lcd->enable_pin, false);
    HAL_Delay(1);
}
