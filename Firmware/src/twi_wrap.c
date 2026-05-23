
#include <avr/io.h>
#include <string.h>
#include <util/twi.h>
#include "uart_wrap.h" // use uart for error messages or verbose output
#include "twi_wrap.h"



int twi_init(){
    TWSR = 0x00;  // Prescaler = 1
    TWBR = 72;    // Assuming 16 MHz clock

    #if !VERBOSE
        uart_puts("TWI LOGGING DISABLED!\r\n");
    #endif

    // Set Inputs
    DDRC &= ~(_BV(PC4) | _BV(PC5)); 
    // Enable Pull ups
    PORTC |= _BV(PC4) | _BV(PC5);
    return 0;
}

int twi_start(void){
    // Print Log to console
    #if VERBOSE
        uart_puts("TWI START\r\n");
    #endif
    // Setup TWI and send start condition
    // Set flags = Clear Interupt flag, Trigger Start Condition, Enable TWI module 
    TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN);
    // Wait for Start condition to finish, i.e. wait for next interrupt  
    while(!(TWCR & _BV(TWINT)));
    if (!((TWSR & 0xF8) == TW_START || (TWSR & 0xF8) == TW_REP_START)){
        flush_write("Init failed\r\n", err, sizeof(err));
        return -1;
    }
    return 0; 
}

int twi_stop(void){
    #if VERBOSE 
        uart_puts("TWI STOP\r\n");
    #endif
    TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWSTO);
    return 0;
}

int twi_request(uint8_t slv_addr, uint8_t rw_bit){
    // Write Log to console
    #if VERBOSE
        uint8_t hexdata[] = {slv_addr, rw_bit};
        memset(msg, 0, sizeof(msg));
        uart_sprintf(msg, 10, "TWI REQ SLV: 0x% RW: 0x%\r\n", hexdata);
        uart_puts(msg);  
    #endif

    // Place slave addr in the data register
    // Shifted by one, LSB contains R/W select
    TWDR = (slv_addr << 1) | rw_bit;
    // Clear interrupt, enable TWI
    TWCR = _BV(TWINT) | _BV(TWEN);
    // Wait for next interrupt
    while(!(TWCR & _BV(TWINT)));
    if (!rw_bit && ((TWSR & 0xF8) != TW_MT_SLA_ACK)){
        flush_write("SLV did NACK Write Req\r\n", err, sizeof(err));
        return -1;
    }
    else if(rw_bit && ((TWSR & 0xF8) != TW_MR_SLA_ACK)){
        flush_write("SLV did NACK Read Req\r\n", err, sizeof(err));
        return -1;
    }
    return 0;
}

int twi_write(uint8_t data){
    // Write Log to console
    #if VERBOSE
        uint8_t hexdata[] = {data};
        memset(msg, 0, sizeof(msg));
        uart_sprintf(msg, 10, "TWI WRITE: 0x%\r\n", hexdata);
        uart_puts(msg);  
    #endif
    // Place the data in the data register
    TWDR = data;
    // Clear interrupt, Enable TWI module
    TWCR = _BV(TWINT) | _BV(TWEN);
    // Wait for next interrupt
    while(!(TWCR & _BV(TWINT)));
    // If the Slave did not send an ACK, throw error
    if ((TWSR & 0xF8) != TW_MT_DATA_ACK){
        flush_write("SLV did NACK Data\r\n", err, sizeof(err));
        return -1;
    }
    return 0; 
}

uint8_t twi_read(uint8_t final){
    uint8_t data = 0x00;
    TWDR = 0x00;
    // For the last recieved Byte, do not send an ACK flag
    TWCR = _BV(TWINT) | _BV(TWEN) | (_BV(TWEA) * !final);
    // Wait until data is recieved and interupt flag flips
    while(!(TWCR & _BV(TWINT)));
    data = TWDR;
    // Write Log to console
    #if VERBOSE
        uint8_t hexdata[] = {data, final};
        memset(msg, 0, sizeof(msg));
        uart_sprintf(msg, 10, "TWI READ: DATA:0x% Final:0x%\r\n", hexdata);
        uart_puts(msg);
    #endif
    return data;
}

