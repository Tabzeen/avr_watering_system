MCU=atmega328p
F_CPU=16000000UL
CC=avr-gcc
OBJCOPY=avr-objcopy
CFLAGS=-mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os -Wall -Iinclude
PORT=/dev/ttyACM0  # or /dev/ttyUSB0 depending on your Arduino
PROGRAMMER=arduino

SRCDIR = src
BUILDDIR = build

SRCS = $(wildcard $(SRCDIR)/*.c)
OBJS = $(patsubst $(SRCDIR)/%.c,$(BUILDDIR)/%.o,$(SRCS))
ASMS = $(patsubst $(SRCDIR)/%.c,$(BUILDDIR)/%.s,$(SRCS))

all: main.hex asm

# Create build directory if it doesn't exist
$(BUILDDIR):
	mkdir -p $(BUILDDIR)

main.elf: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

$(BUILDDIR)/%.o: $(SRCDIR)/%.c | $(BUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILDDIR)/%.s: $(SRCDIR)/%.c | $(BUILDDIR)
	$(CC) $(CFLAGS) -S -fverbose-asm $< -o $@

main.hex: main.elf
	$(OBJCOPY) -O ihex -R .eeprom main.elf main.hex

flash: main.hex
	avrdude -c $(PROGRAMMER) -p $(MCU) -P $(PORT) -b 19200 -U flash:w:main.hex

asm: $(ASMS)

clean:
	rm -rf $(BUILDDIR) *.elf *.hex
