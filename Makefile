###############################################################################
# Makefile for the project GPSDisplay
#
# $Id: Makefile,v 1.1 2009/07/10 14:21:20 avr Exp $
#
###############################################################################

## General Flags
PROJECT = GPSDisplay
MCU = atmega8
TARGET = GPSDisplay.elf
FORMAT = ihex
CC = avr-gcc

UARTLIBDIR = $(HOME)/src/AVR/PFleury/uartlibrary
LCDLIBDIR =$(HOME)/src/AVR/PFleury/lcdlibrary

INCLUDES = -I. -I$(UARTLIBDIR)

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU) -std=c99

## Compile time definitions, program options
DEFINES = 
#DEFINES += -DAPRS
DEFINES += -DUART_RX_BUFFER_SIZE=128
#DEFINES += -DUART_TX_BUFFER_SIZE=32
DEFINES = -DLCD_MODE=LCD_2X16

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += $(DEFINES)
CFLAGS += -Wall -gdwarf-2  -DF_CPU=14745600UL -Os -fsigned-char
CFLAGS += -MD -MP -MT $(*F).o -MF .deps/$(@F).d 

## Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2

## Linker flags
LDFLAGS = $(COMMON)
LDFLAGS += 


## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0


## Objects that must be built in order to link
OBJECTS = GPSDisplay.o GPS.o uart.o LcdDisplay.o lcd.o

## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Build
all: $(TARGET) GPSDisplay.hex GPSDisplay.eep size

## Compile
GPSDisplay.o: GPSDisplay.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

GPS.o: GPS.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

LcdDisplay.o: LcdDisplay.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

uart.o: $(UARTLIBDIR)/uart.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

lcd.o: $(LCDLIBDIR)/lcd.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

## Link
$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) $(LINKONLYOBJECTS) $(LIBDIRS) $(LIBS) -o $(TARGET)

%.hex: $(TARGET)
	avr-objcopy -O $(FORMAT) $(HEX_FLASH_FLAGS)  $< $@

%.eep: $(TARGET)
	avr-objcopy $(HEX_EEPROM_FLAGS) -O $(FORMAT) $< $@

%.lst: $(TARGET)
	avr-objdump -h -S $< > $@

## general targets

size: ${TARGET}
	@echo
#	@avr-size -C --mcu=${MCU} ${TARGET}
	@avr-size --target=$(FORMAT) ${TARGET}
	@avr-size -A $(TARGET)

## Clean target
.PHONY: clean
clean:
	-rm -rf $(OBJECTS) GPSDisplay.elf .deps/* GPSDisplay.hex GPSDisplay.eep

## Other dependencies
-include $(shell mkdir .deps 2>/dev/null) $(wildcard .deps/*)

