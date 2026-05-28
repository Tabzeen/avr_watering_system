<div align="center">
  <img width="30%" height="30%" alt="image psd" src="https://github.com/user-attachments/assets/318329a3-be38-4bcc-9467-6037c5b82de5" />
</div>

## Files for the PCB of the AVR based Watering System


This directory contains all files necessary for manufacturing the systems mainboard.

### Board Preview
<img width="80%" height="80%" alt="image" src="https://github.com/user-attachments/assets/4a8d03c0-8f0e-4e6e-925e-ce0c482595a1" />

### Board Details

**Design Architecture**
- 2 Layer FR-4
- Dim: 66mm x 78mm 
- Top Layer: Decoupled Motor and Controller Section
- Bottom layer primarily GND

**Labeled I/O Pin Headers**
- Two I2C connectors, connecting the sensor and LCD respectively
- ISP for flashing the ATMega Microcontroller
- UART for Debugging 
- Two Onboard LEDs: One indicates power status while the other serves as a programmable debug indicator.
- Connectors for external 2 LEDs
- Connectors for two external Buttons (SET and MODE)

**Power Delivery**
- DC Barrel Jack
- 5V 1.7A Max input capacity

### Additional Hardware
The Board requires the following hardware to function
- 2x Microswitch buttons for "MODE" and "SET"
- 1x Colored LED (Green)
- 1x Colored LED (Red)
- I2C Seesaw Capacitive Soil Sensor
- I2C LCD Panel (AiP31068L)
- 5v Pneumatic Pump

#### Directory Structure
- [Altium](./Altium)
  - Altium Designer project source files  
- [Manufacturing](./Manufacturing)
  - Gerber & Drill files as a zip archive
  - Bill of Materials
