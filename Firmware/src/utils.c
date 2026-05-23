#include "utils.h"
#include <avr/io.h>
#include <util/delay.h>


// Debounce handling
static volatile uint8_t prev_pd4 = 1;
static volatile uint8_t prev_pd5 = 1;


int write_eeprom(uint8_t addr, uint8_t val){
    // Set 2^8 Bit to 0, we only use use the lower addresses (Bit 8 not addressable anyway on 328P)
    EEARH |= 0x0;
    // Set to Erase + Write flags
    EECR &= ~((1<<EEPM1) | (1<<EEPM0));  
    // Wait for write to free
    while(EECR & (1<<EEPE));

    //Write min_val to Byte 0 of the EEPROM
    EEARL = addr;
    EEDR = val;
    // Enable master Write which performs a write when EEPE is written in the next 4 cycles
    EECR |= (1<<(EEMPE));
    // Perform Write
    EECR |= (1<<EEPE);
    // Wait blocking for write to free
    while(EECR & (1<<EEPE));
    return 0;
}

uint8_t read_eeprom(uint8_t addr){
    //wait for write to free
    while(EECR & (1<<EEPE));
    // We don't write to higher addresses, so constantly zeroed
    EEARH |= 0x0;
    // Read max_val from byte 0
    EEARL = addr;
    // Perform Read
    EECR |= (1<<EERE);
    return EEDR;
}

uint8_t fetch_button_pd4(){
    uint8_t state = 0;
    if (!(PIND & (1 << PD4)) && prev_pd4) {
        _delay_ms(20);
        if (!(PIND & (1 << PD4))) {
            state = 1;
        }
    }
    prev_pd4 = (PIND & (1 << PD4)) ? 1 : 0;
    return state;
}

int hold_button_pd4(){
    prev_pd4 = 0;
    return 0;
}

int release_button_pd4(){
    prev_pd4 = 1;
    return 0;
}

uint8_t fetch_button_pd5(){
    uint8_t state = 0;
    if (!(PIND & (1 << PD5)) && prev_pd5) {
        _delay_ms(20);
        if (!(PIND & (1 << PD5))) {
            state = 1;
        }
    }
    prev_pd5 = (PIND & (1 << PD5)) ? 1 : 0;
    return state;
}

int hold_button_pd5(){
    prev_pd5 = 0;
    return 0;
}

int release_button_pd5(){
    prev_pd5 = 1;
    return 0;
}


uint16_t normalize_cap_reading(uint16_t cap_reading){
    uint16_t is_max = (cap_reading > CAP_MAX); uint16_t is_min = (cap_reading < CAP_MIN);
    cap_reading = is_max * CAP_MAX + is_min * CAP_MIN + (1 - is_max - is_min) * cap_reading; 
    // Requires conversion to 32 bit integer as max cap reading can overflow when multiplied by 100 (slow) 
    uint16_t normalized = (uint32_t)(cap_reading - CAP_MIN) * 100 / (CAP_MAX - CAP_MIN);
    return normalized;
}

int write_preset(uint8_t* min_moist, uint8_t* max_moist){
    write_eeprom(0x00, *min_moist);
    write_eeprom(0x01, *max_moist);
    return 0;
}
 
int load_preset(uint8_t* min_moist_addr, uint8_t* max_moist_addr){
    // Store temp for check
    uint8_t temp_val = read_eeprom(0x00);
    if(temp_val > 100)
        return -1;

    *min_moist_addr = temp_val;
   _delay_us(10);
    *max_moist_addr = read_eeprom(0x01);
   _delay_us(10);
   return 0; 
}