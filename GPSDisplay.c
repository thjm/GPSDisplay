
/*
 * File   : GPSDisplay.c
 *
 * Purpose: Contains main() of project GPSDisplay 
 *
 * $Id: GPSDisplay.c,v 1.1 2009/07/10 14:21:20 avr Exp $
 *
 */
 

#include <stdint.h>
#include <stdlib.h>

#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include <uart.h>
#include <lcd.h>

#include "GPS.h"

// this should be set in the Makefile
#ifndef F_CPU
# warning F_CPU not set, setting it to 1 MHz
# define F_CPU 1000000UL  // 1 MHz
#endif // F_CPU

// baud rate for the UART to the GPS module (4800 Bd)
#define UART_BAUD_RATE 4800

// --------------------------------------------------------------------------

int main(void)
 {
  uart_init( UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU) );

  /* initialize display, cursor off */
  lcd_init(LCD_DISP_ON);
  
  // now receiving with interrupts is possible, TX will no longer conflict
  sei();

  unsigned int ch;
  
  // loop forever ...
  while (1) {
    
    ch = uart_getc();
    
    if ( ch & UART_NO_DATA ) {
  
      // no data received -> continue
    }
    else {
      // check error flags first
      if ( ch & UART_FRAME_ERROR )
        uart_puts_P( "UART Frame Error: " );
      if ( ch & UART_OVERRUN_ERROR )
        uart_puts_P( "UART Overrun Error: " );
      if ( ch & UART_BUFFER_OVERFLOW )
        uart_puts_P( "Buffer overflow Error: ");
    
      if ( ch & 0xff ) { // a valid character
      
        if ( GpsMsgHandler( ch & 0xff ) == kTRUE ) {
          GpsMsgPrepare();
	
	  if ( GpsDataIsComplete( &gGpsData ) ) {
	  
	    LcdDisplayShow();
	  
	    GpsDataClear( &gGpsData );
	  }
	  // indicate somehow that we have no or old data
	  else {
	  }
	}
      
      }
    }
  
  } // while (1) ...
  
  return 0;
}

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
