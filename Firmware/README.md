# Firmware for the AVR-Watering-System
This directory contains the entire firmware for the watering system, including minimal footprint drivers for UART and interaction with the I2C IC.
The code is structured into the following segments:
## I/O Drivers
Describe the necessary register configuration and data transmission registers to transmit data between each component. 
- twi-wrap: Contains the I2C driver components
- uart_wrap: Contains the UART driver for debug communication
- timer: Interfaces with the timer IC of the AVR
- utils: Various components such as R/W from/to EEPROM
## Component Drivers
Utilize the minimal interface driver set to wrap R/W functionality for easy access.
- lcd_wrap: Provides higher level access to the AiP31068L panel
- seesaw_wrap: Minimal library for interaction with the Seesaw Soil Sensor
## Orchestration
- main: FSM to manage different features of the system
