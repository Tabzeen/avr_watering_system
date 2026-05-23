#include <avr/io.h>
#include <string.h>
#include "uart_wrap.h"


// Converts a single byte value to an ASCII hex representation
uint16_t byte_to_hex_ascii(uint8_t data) {
    uint8_t lo = (data & 0x0F) + 0x30 + ((data & 0x0F) > 9) * (0x41 - 0x3A);
    uint8_t hi = ((data >> 4) & 0x0F) + 0x30 + (((data >> 4) & 0x0F) > 9) * (0x41 - 0x3A);
    return ((uint16_t)hi << 8) | lo;
}

// Very unsafe sprintf variant
// Currently used as an internal legacy function for TWI Calls
// Do not use for anything else
int uart_sprintf(char* target_arr, size_t size, const char* str, uint8_t* hexdata){
    size_t out_idx = 0;
    size_t hex_idx = 0;
    while(*str){
        if(*str == '%'){
            uint16_t hex = byte_to_hex_ascii(*(hexdata + hex_idx));
            *(target_arr + out_idx++) = (char)(hex >> 8);
            // Additional increase for writing two characters
            *(target_arr + out_idx++) = (char)(hex & 0xFF);
            hex_idx++;
        }
        else 
            *(target_arr + out_idx++) = *(str);
        str++;
    }
    *(target_arr + out_idx) = '\0';
    return 0;
}

int uart_init(uint16_t baud){
    // 103 is standard for 16MHz
    UBRR0L = (uint8_t)(baud & 0xFF);
    UBRR0H = (uint8_t)(baud >> 8);

    //Enable IO Pins to act as RX/TX
    UCSR0B |= (1 << RXEN0) | (1 << TXEN0);

    return 0;
}

int uart_putc(char data){
    while(!(UCSR0A & (1 << UDRE0)));
    UDR0 = data;
    return 0;
}

int uart_puts(char* data){
    // Expects data to be Zero terminated
    while(*(data) != '\0'){
        uart_putc(*data);
        data++;
    }
    return 0;
}

void flush_write(char* msg, char* dest, size_t dest_size){
        memset(dest, 0, dest_size);
        memcpy(dest, msg, strlen(msg));
}
