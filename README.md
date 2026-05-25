## AVR-Based Automatic Plant Watering System
![Project Image](preview.jpeg)

An embedded system designed to automate the process of watering houseplants by monitoring capacitive sensor values. The device detects soil moisture and activates a motorized pump to supply water when the moisture falls outside a user-defined range.

#### Core Features
* **Automated Irrigation:** Monitors soil moisture and runs a 5V pneumatic pump until the target moisture range is satisfied.
* **Custom Thresholds:** Allows users to define specific minimum and maximum moisture bounds via two physical interface buttons.
* **Persistent Storage:** Saves user presets to the ATmega328P's EEPROM, which are then automatically loaded upon device startup.
* **Safety Shutoff:** Includes an emergency shutoff feature that powers down the device if sensor values show little to no change while the pump is running, protecting against failure.
* **Live Status Interface:** Displays current moisture readings, target ranges, and motor status on a 16x2 character I2C LCD.

#### Hardware Architecture

* **Microcontroller:** ATmega328P 8-Bit RISC CPU operating at 16MHz with 32KB of flash storage and 1KB of EEPROM.
* **Mainboard:** Custom 2-layer FR-4 PCB (66mm x 78mm) integrating a motor controller section, I/O pin headers, ISP, and UART connectors.
* **Sensors & Actuators:** Utilizes an Adafruit I2C capacitive moisture sensor (equipped with an onboard Seesaw controller) and a 5V pneumatic water pump.
* **Enclosure:** Housed in a custom-designed, 3D-printed two-part PLA case (138mm x 85mm x 80mm) assembled with threaded M3 screw inserts.

#### Software Implementation
* **Bare-Metal C:** Written in bare-metal C utilizing standard AVR macros (avr/io.h), resulting in a binary approximately 6KB in size.
* **Custom Drivers:** Employs self-written libraries for TWI and UART communication to interact with hardware units.
* **Component Libraries:** Includes dedicated driver files for device interaction, specifically for the LCD and the Adafruit capacitance sensor.
* **Debugging:** Features verbose status output routed to the UART interface for debugging.

#### User Interface Modes
* **Monitoring Mode:** The default state displaying the current moisture percentage and target range, with an option to disable the motor.
* **Set Mode:** A configuration screen used to manually adjust the minimum and maximum moisture thresholds.
* **Store Mode:** A menu option allowing the user to save current settings into a preset slot or load them from EEPROM.

#### Repository Contents
The project repository contains the following files:
* Altium project files, including final Gerber files, Drill files, and the component Bill of Materials (BOM).
* Complete software source code and custom header libraries.
* FreeCAD project files and STL models for the enclosure.
