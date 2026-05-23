#ifndef SEESAW_WRAP_H
#define SEESAW_WRAP_H

#include "seesaw_wrap.h"
#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>
#include "twi_wrap.h"

// Fetches the status register of the seesaw
uint8_t  seesaw_status(void);
// Fetches the capacitance value from the sensr
uint16_t fetch_seesaw_cap(void);
// Fetches the temperature value from the sensor
uint32_t fetch_seesaw_temp(void);
// Fetches the firmware version of the seesaw
uint16_t seesaw_fw_version(void);

#endif
