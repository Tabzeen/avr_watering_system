/* Plant Watering System Board Rev 3.1 Software
*  Written for ATmega328P
*  Author: Talgat Abzanov (tabzanov@techfak.de)
*  Revision Date: 2025-09-11
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "uart_wrap.h"
#include "twi_wrap.h"
#include "lcd_wrap.h"
#include "seesaw_wrap.h"
#include "timer.h"
#include "utils.h"

// MACROS //

// Set Flag for Verbose TWI Outputs
// NOTE Only use for Debugging, as UART Outputs lead to TWI clock stretching
#define VERBOSE 0

// Basic defines for readability
#define READ 1
#define WRITE 0
typedef unsigned int size_t;

// Min and Max Cap values
#define CAP_MIN 300
#define CAP_MAX 1000


// LCD Static text mode_0
char lcd_text_mode_0[16] = "Moisture:      %";
// LCD Static text mode_0_ln2
char lcd_text_mode_0_ln2[16] = "Target:    -   %";
// LCD Static text mode_1_2
char lcd_text_mode_1[16] = "Set ___ Moisture";
// LCD Static text mode_3_4
char lcd_text_mode_3[16] = "Load Preset?    ";  
// Motor stop text buffer
char lcd_text_motor_stop[] = "MOTOR STANDBY   ";
// Preset Saved text buffer
char lcd_text_preset_saved[] = "PRESET SAVED    ";
// Preset Loaded text buffer
char lcd_text_preset_loaded[] = "PRESET LOADED   ";
// No Preset saved buffer
char lcd_text_preset_error[] = "NO PRESET SAVED ";

// BUFFERS //

// Error buffer
char err[64] = {0};
// General Message buffer
char msg[64] = {0};
// LCD Digit buffer
char digit_buff[16] = {0};

// VARIABLES //

// Seesaw capacitance reading buffer
static uint16_t cap_reading = 0;
// Normalized Cap reading in percent
static uint16_t norm_cap_reading = 0;
// Minimum stored sensor reading in percent 
static uint8_t min_moist = 0;
// Maximum stored sensor in percent
static uint8_t max_moist = 100;
// Device mode
static uint8_t mode = 0;

// FLAGS //

// Check if user changed variable and update to the screen is necessary in modes 1 and 2
static uint8_t update_screen = 0;
// Standby flag. Initially enabled to avoid unintended motor startup
static uint8_t mtr_sb = 1;


void ERROR(){
    uart_puts("ERROR: ");
    uart_puts(err);
    while(1){
        // Make LED blink every second to signal an error
        _delay_ms(1000);
        PORTD ^= (1 << PD3); 
    }
}



// Switches the mode and initializes its respective settings
void mode_set(int target_mode){
    switch(target_mode){
        case 0:
            //Start Timer back up
            reset_timer0();
            // Write mode 1 display
            lcd_display(LCD_OFF);

            lcd_clr();
            lcd_puts(lcd_text_mode_0, LC_LN1);
            lcd_puts(lcd_text_motor_stop, LC_LN2);
            
            lcd_display(LCD_ON);
            
            // Turn on Motor Standby when returning to main screen
            mtr_sb = 1;
            
            // Disable button hold if pressed in previous menu
            release_button_pd4();
            mode = 0;

            break;
        case 1:
            // Disable Timer while in menus(
            stop_timer0();
            // Disable Pump while in menus
            PORTD &= ~((1 << PD6) | (1 << PD3));
            // Wait out previous LCD writes
            _delay_us(100);
            lcd_display(LCD_OFF);
            // Write Set mode text
            lcd_clr();
            lcd_puts(lcd_text_mode_1, LC_LN1);
            // Write Settings values
            // Write suffix for static text
            lcd_puts("MAX", LC_LN1 + 4);
            // Write max setting to second screen of line 2
            memset(digit_buff, 0, sizeof(digit_buff));
            sprintf(digit_buff, "%3u%%", max_moist);
            lcd_puts(digit_buff, LC_LN2);
            lcd_display(LCD_ON);
            mode = 1;
            break;
        case 2:
            // Turn display off for smooth write
            lcd_display(LCD_OFF);
            // Write Settings values
            // Suffix for static text
            lcd_puts("MIN", LC_LN1 + 4);
            // Write Min setting to second screen of line 2
            memset(digit_buff, 0, sizeof(digit_buff));
            sprintf(digit_buff, "%3u", min_moist);
            lcd_puts(digit_buff, LC_LN2);
            lcd_display(LCD_ON);
            mode = 2;
            break;
        case 3:
            // Turn off display for writes
            lcd_display(LCD_OFF);
            // Write Present Load text
            lcd_clr();
            lcd_puts(lcd_text_mode_3, LC_LN1);
            lcd_display(LCD_ON);
            mode = 3;
            break;
        case 4:
            // Turn off display for writes
            lcd_display(LCD_OFF);
            // Write Preset Save text
            lcd_puts("Save", LC_LN1); 
            // Clear "Preset loaded" text if it exists
            lcd_puts("               ", LC_LN2);
            lcd_display(LCD_ON);
            // Disable button hold if pressed in previous menu
            release_button_pd4();
            mode = 4;
            break;
        case 5: 
            lcd_display(LCD_OFF);
            lcd_clr();
            lcd_puts("CHECK SENSOR!", LC_LN1);
            lcd_puts("RETURN WITH MODE", LC_LN2);
            lcd_display(LCD_ON);
            mode = 5;
            break;
    }
}

int main(void){

    uart_init(103);
    twi_init();

    // Set button pins PD2 and PD3 as input
    DDRD &= ~((1 << PD4) | (1 << PD5));
    // Set transistor pin and LED pins as output
    DDRD |= (1 << PD6) | (1 << PD2) | (1 << PD3);
    // Enable internal pull-ups for buttons
    PORTD |= (1 << PD4) | (1 << PD5);
    // Turn on Power/Status LED
    PORTD |= (1 << PD2);
    
    uart_init(103);
    twi_init();
    _delay_ms(200);
    lcd_init();

    if(load_preset(&min_moist, &max_moist)) uart_puts("No valid Preset in EEPROM\r\n");

    // Enable Interrupts
    sei();
    init_timer0();

    if(!(*err)) flush_write("Status OK\r\n", err, sizeof(err));

    // Load static text into LCD RAM
    lcd_display(LCD_OFF);
    lcd_puts(lcd_text_mode_0, LC_LN1);
    // Load Standby text as standby is enabled on startup
    lcd_puts(lcd_text_motor_stop, LC_LN2);
    lcd_display(LCD_ON);
    lcd_puts(digit_buff, 10);
   

    // Main Execution Loop, jump statement for indent reduction.
    loop:
    switch(mode){
        case 0:
            cap_reading = fetch_seesaw_cap();
            norm_cap_reading = normalize_cap_reading(cap_reading);

            memset(msg, 0, sizeof(msg));
            sprintf(msg, "Cap Reading %u \r\n", norm_cap_reading);

            uart_puts(msg);
            // Timed events trigger, updates every second
            if(get_trigger0()){
                // Place text where the "Moisture: " text ends
                // The Magic number "12" describes this offset
                sprintf(digit_buff, "%3u", norm_cap_reading);
                lcd_puts(digit_buff, 12);

                // Check if no change occurs stop motor and switch to error mode
                if(mtr_watchdog_update(norm_cap_reading)){
                    mode_set(5);
                    PORTD &= ~((1 << PD6) | (1 << PD3));  
                    break;
                }
                reset_trigger0();
            }
            // Control motor based on min and max settings
            if (norm_cap_reading <= min_moist && !mtr_sb) {
                // Turn motor and Motor LED on
                PORTD |= (1 << PD6) | (1 << PD3);
                uart_puts("Motor ON\r\n");
            } else if (norm_cap_reading >= max_moist && !mtr_sb) {
                // Turn motor and Motor LED off
                PORTD &= ~((1 << PD6) | (1 << PD3));  
                uart_puts("Motor OFF\r\n");
            }
            // Toggle motor standby using "Set" key with debounce protection 
            if (fetch_button_pd4()) {
                if(!mtr_sb){
                    lcd_puts(lcd_text_motor_stop, LC_LN2);
                    PORTD &= ~((1 << PD6) | (1 << PD3));  
                    mtr_sb = 1;
                }
                else{
                    // Clear standby text
                    lcd_puts(lcd_text_mode_0_ln2, LC_LN2);
                    sprintf(digit_buff, "%3u", min_moist);
                    lcd_puts(digit_buff, LC_LN2 + 8);
                    sprintf(digit_buff, "%3u", max_moist);
                    lcd_puts(digit_buff, LC_LN2 + 12);
                    PORTD &= ~((1 << PD6) | (1 << PD3));  
                    mtr_sb = 0;
                }
            }   
            // Check for "Mode" button press with debounce
            if(fetch_button_pd5()) mode_set(1);
            break;
        case 1:
            // Check for change button press with debounce
            if(fetch_button_pd5()) mode_set(2);
            // Loop over Max setting
            if (fetch_button_pd4()) {
                max_moist = max_moist >= 100 ? min_moist : max_moist + 10;
                update_screen = 1;
            }
            if(update_screen){
                memset(digit_buff, 0, sizeof(digit_buff));
                sprintf(digit_buff, "%3u%%", max_moist);
                lcd_puts(digit_buff, LC_LN2);
                update_screen = 0;
            }
            break;
        case 2:
            // Loop over Min setting
            if (fetch_button_pd4()) {
                min_moist = min_moist >= max_moist ? 0 : min_moist + 10;
                update_screen = 1;
            }
            if(update_screen){
                memset(digit_buff, 0, sizeof(digit_buff));
                sprintf(digit_buff, "%3u%%", min_moist);
                lcd_puts(digit_buff, LC_LN2);
                update_screen = 0;
            }
            // Check for change button press with debounce
            if(fetch_button_pd5()) mode_set(3);
            break;
        case 3:
            if(fetch_button_pd5()) mode_set(4);
            if (fetch_button_pd4()) {
                lcd_puts(lcd_text_preset_loaded, LC_LN2);
                // If no data available display message
                if(load_preset(&min_moist, &max_moist)){
                    lcd_puts(lcd_text_preset_error, LC_LN2);
                }
                // Prohibit multiple presses without menu cycling (Mainly for consistency)
                hold_button_pd4();
            }
            
            break;
        case 4:
            if(fetch_button_pd5()) mode_set(0);
            if (fetch_button_pd4()) {
                lcd_puts(lcd_text_preset_saved, LC_LN2);
                write_preset(&min_moist, &max_moist);
                // Prohibit multiple presses without menu cycling to avoid unnecessary EEPROM writes
                hold_button_pd4();
            }
            break;
        case 5:
            if(fetch_button_pd5()) mode_set(0);
            break;

    }
    goto loop;
}
    
    