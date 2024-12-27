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

## Usage

## Example

## Limitations

## Licence

This library is provided under the MIT licence.

## Acknowledgements

This project makes heavy use of
- Adafruit_LiquidCrystal (https://github.com/adafruit/Adafruit_LiquidCrystal/tree/master)
- MCP23008_STM32 (https://github.com/m1geo/MCP23008_STM32/tree/main)