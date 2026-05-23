
#include "lcd_wrap.h"
#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include "twi_wrap.h"


// Error handled by main
extern void ERROR();

void lcd_display(uint8_t state){
    if(twi_start()) ERROR();
    if(twi_request(LC_SLV, WRITE)) ERROR();
    if(twi_write(0x00)) ERROR();
    // Set LCD Registers: Display ON: Display ON, Cursor OFF, Cursor blink OFF
    if(twi_write(0b00001000 | state)) ERROR();
    twi_stop();
    _delay_us(60);
}


void lcd_init(){
    // Clear display
    if(twi_start()) ERROR();
    if(twi_request(LC_SLV, WRITE)) ERROR();
    if(twi_write(0x00)) ERROR();
    if(twi_write(0x01)) ERROR();
    twi_stop();
    _delay_ms(3);

    // Function Set: 8-bit, 2 lines, 5x8 font
    if(twi_start()) ERROR();
    if(twi_request(LC_SLV, WRITE)) ERROR();
    if(twi_write(0x00)) ERROR();
    if(twi_write(0b00101100)) ERROR();
    twi_stop();
    _delay_us(60);

    // Display ON: Display ON, Cursor OFF, Cursor blink OFF
    if(twi_start()) ERROR();
    if(twi_request(LC_SLV, WRITE)) ERROR();
    if(twi_write(0x00)) ERROR();
    if(twi_write(0b00001100)) ERROR();
    twi_stop();
    _delay_us(60);

    // Entry Mode Set: Cursor Increment, No Display Shift
    if(twi_start()) ERROR();
    if(twi_request(LC_SLV, WRITE)) ERROR();
    if(twi_write(0x00)) ERROR();
    if(twi_write(0b00000110)) ERROR();
    twi_stop();
    _delay_us(60);
}

int lcd_putc(char letter, uint8_t addr){
    // 1st Line DDRAM Address 0x00 to 0x27
    // 2nd Line DDRAM; Address 0x40 to 0x67

    // Abort if invalid memory is written
    if((addr > 0x27 && addr < 0x40) || addr > 0x67){
        return -1;
    }

    // Set DDRAM address
    if(twi_start()) ERROR();
    if(twi_request(LC_SLV, WRITE)) ERROR();
    if(twi_write(0x00)) ERROR();
    if(twi_write(0x80 | addr)) ERROR();
    twi_stop();
    _delay_us(50);

    // Write Character to address
    if(twi_start()) ERROR();
    if(twi_request(LC_SLV, WRITE)) ERROR();
    // RS = 1, as DRAM is accessed 
    if(twi_write(0x40)) ERROR(); 
    if(twi_write(letter)) ERROR();
    twi_stop();
    _delay_us(50);

    return 0;
}

void lcd_clr(){
    // Clear display
    if(twi_start()) ERROR();
    if(twi_request(LC_SLV, WRITE)) ERROR();
    if(twi_write(0x00)) ERROR();
    if(twi_write(0x01)) ERROR();
    twi_stop();
    _delay_ms(3);
}

void lcd_puts(char* str, uint8_t addr){
    while(*str){
        if(lcd_putc(*str++, addr++)) break;
    }
}


// LEGACY: Shifting is not needed anymore with full TWI Speeds
void lcd_shift(uint8_t direction){
    if(twi_start()) ERROR();
    if(twi_request(LC_SLV, WRITE)) ERROR();
    if(twi_write(0x00)) ERROR();
    // Shift both LCD rows by one character to the left or right
    if(twi_write(0x18 | direction)) ERROR();
    twi_stop();
    _delay_us(80);
}