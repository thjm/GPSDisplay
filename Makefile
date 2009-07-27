###############################################################################
# Makefile for the project GPSDisplay
#
# $Id: Makefile,v 1.4 2009/07/27 06:57:49 avr Exp $
#
###############################################################################

## General Flags
PROJECT = GPSDisplay
MCU = atmega8
TARGET = GPSDisplay.elf
FORMAT = ihex
CC = avr-gcc

SRCS	= 

UARTLIBDIR = $(HOME)/src/AVR/PFleury/uartlibrary
LCDLIBDIR =$(HOME)/src/AVR/PFleury/lcdlibrary

INCLUDES = -I. -I$(UARTLIBDIR)

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU) -std=c99

## Compile time definitions, program options
DEFINES = 
#DEFINES += -DAPRS
# 
#DEFINES += -DUSE_N4TXI_UART
# these definitions are for P.Fleurys 'uartlibrary'
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

## Sources for make depend
SRCS += GPSDisplay.c GPS.c uart.c get8key4.c LCDDisplay.c lcd.c

## Objects that must be built in order to link
OBJECTS = GPSDisplay.o GPS.o uart.o get8key4.o LCDDisplay.o lcd.o

## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Build
all: $(TARGET) GPSDisplay.hex GPSDisplay.eep test size

## Compile (via suffix rule)
.c.o:
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

## some pattern rules
%.hex : %.elf
	avr-objcopy -O $(FORMAT) $(HEX_FLASH_FLAGS)  $< $@

%.eep : %.elf
	avr-objcopy $(HEX_EEPROM_FLAGS) -O $(FORMAT) $< $@

%.lst : %.elf
	avr-objdump -h -S $< > $@

uart.o: $(UARTLIBDIR)/uart.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

lcd.o: $(LCDLIBDIR)/lcd.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

Serial.o: Serial.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

## Link
$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) $^ $(LINKONLYOBJECTS) $(LIBDIRS) $(LIBS) \
	-o $(TARGET) -Wl,-Map,$(PROJECT).map

## test targets

test:: testLCD.hex testLCD.lst

SRCS += testLCD.c

TESTLCD_OBJS = testLCD.o lcd.o

testLCD.hex: testLCD.elf
	avr-objcopy -O $(FORMAT) $(HEX_FLASH_FLAGS) $< $@
	@avr-size --target=$(FORMAT) testLCD.elf

testLCD.elf: $(TESTLCD_OBJS)
	$(CC) $(LDFLAGS) $(TESTLCD_OBJS) -o $@

size:: testLCD.elf
	@echo
	@avr-size --target=$(FORMAT) testLCD.elf

clean::
	-rm -f *.o testLCD.elf testLCD.hex testLCD.lst

## general targets

size:: ${TARGET}
	@echo
#	@avr-size -C --mcu=${MCU} ${TARGET}
	@avr-size --target=$(FORMAT) ${TARGET}
#	@avr-size -A $(TARGET)

## Clean target
.PHONY: clean
clean::
	-rm -rf $(OBJECTS) GPSDisplay.elf .deps/* GPSDisplay.hex GPSDisplay.eep GPSDisplay.map

## Other dependencies
-include $(shell mkdir .deps 2>/dev/null) $(wildcard .deps/*)

