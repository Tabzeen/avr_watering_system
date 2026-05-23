
#include "seesaw_wrap.h"
#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>
#include "twi_wrap.h"


// SEESAW SENSOR //

// Seesaw capacitive sensor TWI address
static const uint8_t SS_SLV = 0x36;
// Seesaw TOUCH_BASE Module ID
static const uint8_t SS_HI = 0x0F;
// TOUCH_BASE Module Capacitance
static const uint8_t SS_LOW = 0x10;
// Seesaw STATUS_BASE Module ID
static const uint8_t SB_HI = 0x00;
// STATUS_BASE Module Temperature;
static const uint8_t SB_LO = 0x04; 


// Error handled by main
extern void ERROR();

uint8_t seesaw_status(){
    uint8_t data = 0x00;
    // Setup TWI and send start condition
    if(twi_start()) ERROR();
    // Set Write request to select module from which to read from
    if(twi_request(SS_SLV, WRITE)) ERROR();
    // Send the first byte of the module to read from
    if(twi_write(SB_HI)) ERROR();
    // Send the second byte of the module to read from
    if(twi_write(0x01)) ERROR();
    _delay_ms(100);
    // Send a repeated start to switch to read
    if(twi_start()) ERROR();
    if(twi_request(SS_SLV, READ)) ERROR();
    // For last expected Byte send a NACK, by not setting the ACK flag
    data = twi_read(1);
    // Send stop condition
    twi_stop();
    return data;
}

uint16_t fetch_seesaw_cap(){
    uint8_t hi, lo;
    // Setup TWI and send start condition
    if(twi_start()) ERROR();
    // Set Write request to select module from which to read from
    if(twi_request(SS_SLV, WRITE)) ERROR();
    // Send the first byte of the module to read from
    if(twi_write(SS_HI)) ERROR();
    // Send the second byte of the module to read from
    if(twi_write(SS_LOW)) ERROR();
    _delay_ms(100);
    // Send a repeated start to switch to read
    if(twi_start()) ERROR();

    if(twi_request(SS_SLV, READ)) ERROR();
    // The data we want to receive is 2 bytes long, so wait for first byte to transfer
    hi = twi_read(0);
    // For last expected Byte send a NACK, by not setting the ACK flag
    lo = twi_read(1);
    // Send stop condition
    twi_stop();
    return ((uint16_t)hi << 8) | lo;
}


uint32_t fetch_seesaw_temp(){
    // Setup TWI and send start condition
    if(twi_start()) ERROR();
    // Set Write request to select module from which to read from
    if(twi_request(SS_SLV, WRITE)) ERROR();
    // Send the first byte of the module to read from
    if(twi_write(SB_HI)) ERROR();
    // Send the second byte of the module to read from
    if(twi_write(SB_LO)) ERROR();
    // Send a repeated start to switch to read
    if(twi_start()) ERROR();
    if(twi_request(SS_SLV, READ)) ERROR();

    uint32_t val = 0;
    val |= (uint32_t)twi_read(0) << 24;
    val |= (uint32_t)twi_read(0) << 16;
    val |= (uint32_t)twi_read(0) << 8;
    val |= (uint32_t)twi_read(1);

    // Send stop condition
    twi_stop();
    return val;
}

uint16_t seesaw_fw_version() {
    uint16_t data = 0;
    if(twi_start()) ERROR();
    if(twi_request(SS_SLV, WRITE)) ERROR();
    if(twi_write(SB_HI)) ERROR();
    if(twi_write(0x02)) ERROR();  // FW version register
    _delay_ms(50);
    if(twi_start()) ERROR();
    data = (twi_read(0) << 8);
    data = twi_read(1);
    twi_stop();
    return data;
}

