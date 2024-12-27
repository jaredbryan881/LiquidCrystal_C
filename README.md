# LiquidCrystal_C Library

This is a small C library for controlling an HD44780-compatible LCD over an I2C interface, using the MCP23008 I/O expander. The library is designed for STM32 microcontrollers and makes use of the STM32 HAL for I2C communication.

## Features

- Supports 4-bit and 8-bit modes of the HD44780
- Compatible with 16x02, 20x04, and other HD44780-based LCDs
- Includes the essential commands for controling the display, backlight, cursor, and text.
- Allows for custom character definitions.

## Requirements

- **Hardware**
  - STM32 microcontroller
  - MCP23008 I/O expander connected to the LCD
  - HD44780-compatible LCD
  - Appropriate pull-up resistors on I2C lines

- **Software**
  - STM32 HAL library
  - C compiler
  - STM32 HAL driver for the MCP23008 I/O expander from https://github.com/m1geo/MCP23008_STM32/tree/main

## Installation

1. Clone or download the repository
2. Include the following files in your project:
  - `LiquidCrystal_C.h`
  - `LiquidCrystal_C.c`
  - `MCP23008.h`
  - `MCP23008.c`
3. Add the `#include` directives where they're needed:
  ```c
  #include "LiquidCrystal_C.h"
  #include "MCP23007.h"
  ```

## Wiring diagram

| MCP23008 Pin | Function             | LCD Pin |
| ------------ | -------------------- | ------- |
| GP1          | RS (Register select) | 4       |
| GP2          | EN (Enable)          | 6       |
| GP3          | D4                   | 11      |
| GP4          | D5                   | 12      |
| GP5          | D6                   | 13      |
| GP6          | D7                   | 14      |
| GP7          | Backlight (optional) | 15      |

These pin assignments will vary depending on your LCD and I2C backpack, so you need to be absolutely sure you set them correctly.
Make sure that the other LCD pins are also properly connected
- VSS (Pin 1): GND
- VDD (Pin 2): 5V (or 3.3V, depending on your LCD)
- Vo (Pin 3): Contrast adjustment (set with potentiometer)
- RW (Pin 5): GND (unused)

## Usage

**Initialization**
To initialize the LCD, first configure the MCP23008 and I2C. Then, initialize the LCD.
```c
MCP23008_Init(&hi2c1, &hmcp, 0x20); // Initialize MCP23008 located at I2C address 0x20
MCP23008_SetDirection(&hmcp, 0x00); // Set all MCP23008 pins as outputs
LCD_Init(&lcd, &hmcp,
		  1, // four-bit mode
		  1, 255, 2, // RS=GP1, RW=invalid, EN=GP2
		  3, 4, 5, 6, // D0..D3 (these map to LCD D3..D6 in 4-bit mode)
		  0, 0, 0, 0); // D4..D7 not used in 4-bit mode
LCD_Begin(&lcd, 16, 2, LCD_5x8DOTS); // 16x02 LCD
```

**Basic functions**
```c
LCD_Clear(&lcd);						// Clear the display
LCD_SetCursor(&lcd, 0, 0);				// Set cursor to column 0, row 0
LCD_WriteString(&lcd, "Hello, world!"); // Print a string
LCD_Cursor(&lcd);						// Enable cursor
LCD_Blink(&lcd);						// Enable blinking cursor
LCD_NoCursor(&lcd);						// Disable cursor
LCD_NoBlink(&lcd);						// Disable blinking cursor
LCD_ScrollDisplayLeft(&lcd);			// Scroll text left
LCD_ScrollDisplayRight(&lcd);			// Scroll text right
```

**Custom characters**
```c
uint8_t smiley[8] = {0x00, 0x0A, 0x0A, 0x00, 0x11, 0x0E, 0x00, 0x00};
LCD_CreateChar(&lcd, 0, smiley); // Create character at CGRAM slot 0
LCD_SetCursor(&lcd, 0, 0); 		 // Set cursor to column 0, row 0
LCD_WriteChar(&lcd, 0);			 // Display custom character stored in CGRAM slot 0
```

**Backlight control**
```c
LCD_SetBacklight(&lcd, true);  // Turn backlight on
LCD_SetBacklight(&lcd, false); // Turn backlight off
```

## Example

Refer to ```main.c``` and the above usage instructions for an example.
This implementation uses the Adafruit standard 16x02 LCD (https://www.adafruit.com/product/181), the Adafruit I2C/SPI character LCD backpack (https://www.adafruit.com/product/292), and the STM32F411 "BlackPill" dev board (https://www.adafruit.com/product/4877).

## Limitations

- Limited portability. Limited to HD44780-compatible LCDs and the MCP23008 expander. This won't work with the common PCF8574 I/O expander, which is commonly used in I2C LCD modules. Won't work with SPI. Limited to character LCDs and does not support graphical LCDs.
- No error handling.
- No dynamic memory management. Pin mappings and settings are statically defined at initialization. You can't switch from a 16x02 to a 20x04 LCD without restarting.
- Limited backlight control. Does not support dimming by PWM.
- Limited custom character storage. Only 8 custom chars can be stored in the LCD's CGRAM at once.
- No buffering for scrolling text.

## Next steps

If you are interested in contributing, good next steps are
- Implement error handling
- Add multi-LCD support
- Extend the library to build LCD-based user interfaces

## Licence

This library is provided under the MIT licence.

## Acknowledgements

This project makes use of
- Adafruit_LiquidCrystal (https://github.com/adafruit/Adafruit_LiquidCrystal/tree/master)
- MCP23008_STM32 (https://github.com/m1geo/MCP23008_STM32/tree/main)