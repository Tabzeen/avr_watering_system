//#include <iom328p.h>


#ifndef UART_WRAP_H
#define UART_WRAP_H

typedef unsigned int size_t;

// Simple sprintf implementation for 8 bit hex data support using % as a wildcard
int uart_sprintf(char* target_arr, size_t size, const char* str, uint8_t* hexdata);
// Initialize UART with given baud rate
int uart_init(uint16_t baud);
// Send a single character over UART
int uart_putc(char data);
// Send a zero-terminated string over UART
int uart_puts(char* data);
// Safely copy a message to a destination buffer, ensuring no overflow
void flush_write(char* msg, char* dest, size_t dest_size);

#endif