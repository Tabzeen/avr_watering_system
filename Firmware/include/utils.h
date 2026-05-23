#ifndef EEPROM_UTILS_H
#define EEPROM_UTILS_H

#ifndef CAP_MAX
// Min and Max Cap values
#define CAP_MIN 300
#define CAP_MAX 1000
#endif

#include <avr/io.h>

// Fetch button state on pin PD4 with debounce
uint8_t fetch_button_pd4();
// Hold button state on pin PD4 (Set to pressed) (Implemented in software)
int hold_button_pd4();
// Release button state on pin PD4 (Set to not pressed) (Implemented in software)
int release_button_pd4();
// Fetch button state on pin PD5 with debounce
uint8_t fetch_button_pd5();
// Hold button state on pin PD5 (Set to pressed) (Implemented in software)
int hold_button_pd5();
// Release button state on pin PD5 (Set to not pressed) (Implemented in software)
int release_button_pd5();
// Normalize capacitance reading to percentage based on CAP_MIN and CAP_MAX defines
uint16_t normalize_cap_reading(uint16_t cap_reading);
// Write preset values to EEPROM addresses 0x00 and 0x01 for min and max moisture respectively
int write_preset(uint8_t* min_moist, uint8_t* max_moist);
// Load preset values from EEPROM addresses 0x00 and 0x01 for min and max moisture respectively
int load_preset(uint8_t* min_moist_addr, uint8_t* max_moist_addr);

#endif
