#ifndef LCD_WRAP_H
#define LCD_WRAP_H

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include "twi_wrap.h"


// LCD Shift direction
#define LCD_RIGHT 0
#define LCD_LEFT 0x4
// LCD Display state
#define LCD_ON 0x4
#define LCD_OFF 0
// LCD Controller TWI address
#define LC_SLV 0x3E
// LCD Line 1 DDRAM offset
#define LC_LN1 0x00
// LCD Line 2 DDRAM offset
#define LC_LN2 0x40
// LCD Display Controller TWI address
#define DP_SLV 0x6B

// Turns the LCD backlight on or off
void lcd_display(uint8_t state);
// Initialize the LCD
void lcd_init(void);
// Write a single character to a specific DDRAM address
int lcd_putc(char letter, uint8_t addr);
// Send a clear command to the LCD controller
void lcd_clr(void);
// Write a string starting at a specific DDRAM address
void lcd_puts(char* str, uint8_t addr);
// Shift the entire display left or right by one character
void lcd_shift(uint8_t direction);

#endif // LCD_WRAP_H
