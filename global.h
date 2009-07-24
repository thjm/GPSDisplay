
/*
 * File   : global.h
 *
 * Purpose: Contains all global definitions of the 'GPSDisplay' project
 *
 * $Id: global.h,v 1.1 2009/07/24 15:52:40 avr Exp $
 */


#ifndef _global_h_
#define _global_h_

#include <stdint.h>

#include <avr/io.h>

/** @file global.h
  * Global header file for 'GPSDisplay' project.
  * @author H.-J.Mathes, DC2IP
  */

/* this should be set in the Makefile */
#ifndef F_CPU
# warning F_CPU not set, setting it to 1 MHz
# define F_CPU 1000000UL  // 1 MHz
#endif // F_CPU

/** Baud rate for the UART to the GPS module (4800 Bd) */
#define UART_BAUD_RATE 4800

/** Constant for interrupts every 10ms, pre-scaler = 1024 */
#define CNT1_PRESET       (0xffff - 144)

// --- Definitions for the LEDs etc.

/** Green LED = DCD at port B, pin 0. */
#define LED_DCD           (1<<PB0)  // @ PB0
#define LED_GREEN         LED_DCD
# define LedDCDInit()     { DDRB |= LED_DCD; }   // as output
# define LedDCDOn()       { PORTB |= LED_DCD; }
# define LedDCDOff()      { PORTB &= ~LED_DCD; }

/** Red LED = TX at port B, pin 1. */
#define LED_TX            (1<<PB1)  // @ PB1
#define LED_RED           LED_TX
# define LedTXInit()      { DDRB |= LED_TX; }     // as output
# define LedTXOn()        { PORTB |= LED_TX; }
# define LedTXOff()       { PORTB &= ~LED_TX; }

/** Yellow LED = GPS quality, (on: good, blinking: not valid) at port D, pin 2. */
#define LED_GPS           (1<<PD2)  // @ PD2
#define LED_YELLOW        LED_GPS
# define LedGPSInit()     { DDRD |= LED_GPS; }   // as output
# define LedGPSOn()       { PORTD |= LED_GPS; }
# define LedGPSOff()      { PORTD &= ~GPS; }

/** Push button connected to some port. */

#define BUTTON_PORT       PORTD
#define BUTTON_PIN        PIND
#define BUTTON_DDR        DDRD

#define BUTTON1           (1<<PD6)

#endif /* _global_h_ */
