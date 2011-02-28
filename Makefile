###############################################################################
# Makefile for the project GPSDisplay
#
# $Id: Makefile,v 1.7 2011/02/28 12:11:33 mathes Exp $
#
###############################################################################

## Flags to steer make
Use_N4TXI_UART = 1
Use_Navilock = 1

## General Flags
PROJECT = GPSDisplay
MCU = atmega8
TARGET = GPSDisplay
FORMAT = ihex
CC = avr-gcc
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
SIZE = avr-size
NM = avr-nm
AVRDUDE = avrdude
REMOVE = rm -f
MV = mv -f

SRCS	= 

UARTLIBDIR = $(HOME)/src/AVR/PFleury/uartlibrary
LCDLIBDIR =$(HOME)/src/AVR/PFleury/lcdlibrary

INCLUDES = -I.
ifeq ($(Use_N4TXI_UART),0)
INCLUDES += -I$(UARTLIBDIR)
endif

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU) -std=c99

## Compile time definitions, program options
DEFINES = 
#DEFINES += -DAPRS
# 
ifeq ($(Use_N4TXI_UART),1)
DEFINES += -DUSE_N4TXI_UART
endif
ifeq ($(Use_Navilock),1)
DEFINES += -DGPS_NAVILOCK
else
DEFINES += -DGPS_GARMIN
endif
# these definitions are for P.Fleurys 'uartlibrary'
ifeq ($(Use_N4TXI_UART),0)
DEFINES += -DUART_RX_BUFFER_SIZE=128
#DEFINES += -DUART_TX_BUFFER_SIZE=32
endif
DEFINES += -DLCD_MODE=LCD_2X16

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += $(DEFINES)
CFLAGS += -Wall -gdwarf-2  -DF_CPU=14745600UL -Os -fsigned-char
CFLAGS += -MD -MP -MT $(*F).o -MF .deps/$(@F).d 
#CFLAGS += --combine
#CFLAGS += -fwhole-program
#CFLAGS += -ffunction-sections


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


## Programming support using avrdude. Settings and variables.

AVRDUDE_PROGRAMMER = usbasp
AVRDUDE_PORT = usb

AVRDUDE_WRITE_FLASH = -U flash:w:$(TARGET).hex
AVRDUDE_WRITE_EEPROM = -U eeprom:w:$(TARGET).eep


# Uncomment the following if you want avrdude's erase cycle counter.
# Note that this counter needs to be initialized first using -Yn,
# see avrdude manual.
AVRDUDE_ERASE_COUNTER = -y

# Uncomment the following if you do /not/ wish a verification to be
# performed after programming the device.
#AVRDUDE_NO_VERIFY = -V

# Increase verbosity level.  Please use this when submitting bug
# reports about avrdude. See <http://savannah.nongnu.org/projects/avrdude>
# to submit bug reports.
#AVRDUDE_VERBOSE = -v -v

AVRDUDE_BASIC = -p $(MCU) -P $(AVRDUDE_PORT) -c $(AVRDUDE_PROGRAMMER)
AVRDUDE_FLAGS = $(AVRDUDE_BASIC) $(AVRDUDE_NO_VERIFY) $(AVRDUDE_VERBOSE) $(AVRDUDE_ERASE_COUNTER)


## Sources for make depend
SRCS += GPSDisplay.c GPS.c get8key4.c LCDDisplay.c lcd.c
ifeq ($(Use_N4TXI_UART),1)
SRCS += Serial.c
else
SRCS += uart.c
endif

## Objects that must be built in order to link
OBJECTS = GPSDisplay.o GPS.o get8key4.o LCDDisplay.o lcd.o
ifeq ($(Use_N4TXI_UART),1)
OBJECTS += Serial.o
else
OBJECTS += uart.o
endif

## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Build (without GPSDisplay.eep)
all: $(TARGET).elf GPSDisplay.hex test size

## Compile (via suffix rule)
.c.o:
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

## some pattern rules
%.hex : %.elf
	$(OBJCOPY) -O $(FORMAT) $(HEX_FLASH_FLAGS)  $< $@

%.eep : %.elf
	$(OBJCOPY) $(HEX_EEPROM_FLAGS) -O $(FORMAT) $< $@

%.lst : %.elf
	$(OBJDUMP) -h -S $< > $@

uart.o: $(UARTLIBDIR)/uart.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

lcd.o: $(LCDLIBDIR)/lcd.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

Serial.o: Serial.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

## Link
$(TARGET).elf: $(OBJECTS)
	$(CC) $(LDFLAGS) $^ $(LINKONLYOBJECTS) $(LIBDIRS) $(LIBS) \
	-o $(TARGET).elf -Wl,-Map,$(PROJECT).map

## Program the device
program: $(TARGET).hex $(TARGET).eep
	$(AVRDUDE) $(AVRDUDE_FLAGS) $(AVRDUDE_WRITE_FLASH) $(AVRDUDE_WRITE_EEPROM)

## test targets

test:: testLCD.hex testLCD.lst

SRCS += testLCD.c

TESTLCD_OBJS = testLCD.o lcd.o

testLCD.hex: testLCD.elf
	$(OBJCOPY) -O $(FORMAT) $(HEX_FLASH_FLAGS) $< $@
	@$(SIZE) --target=$(FORMAT) testLCD.elf

testLCD.elf: $(TESTLCD_OBJS)
	$(CC) $(LDFLAGS) $(TESTLCD_OBJS) -o $@

size:: testLCD.elf
	@echo
	@$(SIZE) --target=$(FORMAT) testLCD.elf

clean::
	-$(REMOVE) -f *.o testLCD.elf testLCD.hex testLCD.lst

## general targets

size:: ${TARGET}.elf
	@echo
#	@$(SIZE) -C --mcu=${MCU} ${TARGET}.elf
	@$(SIZE) --target=$(FORMAT) ${TARGET}.elf
#	@$(SIZE) -A $(TARGET).elf

## Clean target
.PHONY: clean
clean::
	-$(REMOVE) -rf $(OBJECTS) $(TARGET).elf .deps/* $(TARGET).hex $(TARGET).eep $(TARGET).map $(TARGET).lst


## Other dependencies
-include $(shell mkdir .deps 2>/dev/null) $(wildcard .deps/*)

