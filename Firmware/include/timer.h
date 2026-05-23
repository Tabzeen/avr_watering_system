#ifndef TIMER_H
#define TIMER_H

#include <avr/io.h>

// initializes timer0 for 1 second overflow interrupts
int init_timer0(void);
// stops timer0
int stop_timer0(void);
// resets timer0 counter and restarts counting
int reset_timer0(void);
// returns 1 if timer0 has triggered the overflow event
uint8_t get_trigger0();
// resets the trigger flag
int reset_trigger0();
// updates the motor watchdog, returns 1 if motor should be turned off
int mtr_watchdog_update(uint16_t);

#endif // TIMER_H
