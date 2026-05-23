#ifndef TWI_WRAP_H
#define TWI_WRAP_H

/*
 * TWI Wrapper library for the ATmega328P
 * Provides basic TWI functionality with optional UART logging
*/
#include <stdint.h>



// Set if TWI needs to be debugged
// ATTENTION! Leads to very high clock stretching, due to UART outputs blocking the CPU. Only use for debugging
#ifndef VERBOSE
    #define VERBOSE 0
#endif

#ifndef READ
    #define READ 1
    #define WRITE 0
#endif

// msg and err buffer should be defined in main file and are global for board
extern char msg[64];
extern char err[64];



// Initializes the TWI module
int twi_init();
// Sends a START condition on the TWI bus to all slaves
int twi_start();
// Sends a STOP condition on the TWI bus
int twi_stop();
// Sends a request to a slave with its address and R/W bit
int twi_request(uint8_t slv_addr, uint8_t rw_bit);
// Writes a single byte to the bus
int twi_write(uint8_t data);
// Reads a single byte from the bus, if final = 1 sends NACK after read, else sends ACK
uint8_t twi_read(uint8_t final);

#endif
