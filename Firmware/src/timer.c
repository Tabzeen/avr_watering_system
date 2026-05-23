#include "timer.h"
#include <avr/interrupt.h>
#include <avr/io.h>



static volatile uint8_t t0_ovf = 0;
static volatile uint8_t t0_trigger = 0;

// TIMER //

static uint8_t mtr_cnt = 0;
static uint8_t mtr_prev = 0;
// Check for changes 
static uint16_t sensor_prev;
static uint8_t diff_sum = 0;


// Overflow interrupt of timer 0
/*
16MHz / 1024 = 15625Hz
1/15625Hz = 0.0064ms per cycle
8 Bit Overflow every 0.064 * 255 = 16.32ms
No of overflows for one second 1000/16.32 = ~61 Overflows per second
*/
ISR(TIMER0_OVF_vect){
    t0_ovf++;
    if(t0_ovf >= 61){
        t0_trigger = 1;
        t0_ovf = 0;
    }
}

int init_timer0(){
    // Disable all OC, set to normal mode
    TCCR0A = 0x0;
    // Set Prescaler to 1024 (Longest possible period ~15625Hz)
    TCCR0B = ((1<<CS02) | (1<<CS00));
    // Enable Overflow interrupt
    TIMSK0 = (1<<TOIE0);
    return 0;
}

int stop_timer0(){
    // Disable timer
    TCCR0B &= ~((1<<CS02) | (1<<CS01) | (1<<CS00));
    return 0;
}

// Expects that the timer has been initialized
int reset_timer0(){
    TCCR0B &= ~((1<<CS02) | (1<<CS01) | (1<<CS00));
    // Set counter to 0
    TCNT0 = 0x0;
    TCCR0B = ((1<<CS02) | (1<<CS00));
    return 0;
}

uint8_t get_trigger0(){
    return t0_trigger;
}

int reset_trigger0(){
    t0_trigger = 0;
    return 0;
}


int mtr_watchdog_update(uint16_t curr_reading){
    // Grab the current motor state
    uint8_t mtr_cur = (PORTD >> PD6) & 0x1;
    // Reset the counter if motor was turned off
    if(mtr_cur > mtr_prev){
        mtr_cnt = 0;
        diff_sum = 0;
    }
    else if(mtr_cur == mtr_prev && mtr_cur){
        mtr_cnt++;
        diff_sum += (curr_reading > sensor_prev) ? (curr_reading - sensor_prev) : 0;
        // If no sufficient change in set in the past 10 seconds, throw an error state else reset timer 
        if(mtr_cnt >= 10){
            if(diff_sum < 10) 
                return -1; 
            mtr_cnt = 0;
            diff_sum = 0;
        }
    }
    sensor_prev = curr_reading;
    mtr_prev = mtr_cur; 
    return 0;
}