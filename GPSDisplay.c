
/*
 * File   : GPSDisplay.c
 *
 * Purpose: Contains main() of project GPSDisplay 
 *
 * $Id: GPSDisplay.c,v 1.3 2009/07/27 06:57:49 avr Exp $
 *
 */
 

#include <stdint.h>
#include <stdlib.h>

#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

/** @file GPSDisplay.c
  * Contains the main() of the 'GPSDisplay' project.
  * @author H.-J.Mathes, DC2IP
  */

#include "global.h"

#ifndef USE_N4TXI_UART
# include <uart.h>
#endif // USE_N4TXI_UART
#include <lcd.h>

#ifdef USE_N4TXI_UART
# include "Serial.h"
#endif // USE_N4TXI_UART

#include "get8key4.h"
#include "LCDDisplay.h"
#include "GPS.h"


/** variable to indicate the GPS data quality. */
enum {

  kValidData,   // sufficient HDOP and 'V' flag set
  kBadData,     // bad precision
  kOldData,     // reading old data from module
  kNoSignal

} EGPSDataQuality;

volatile char gGPSDataQuality = kNoSignal;

/* ------------------------------------------------------------------------- */

/** Delay routine for delays in second regime. */
void delay_sec(unsigned int sec)
 {
  while ( sec ) {
    for ( int ms=0; ms<1000; ms++ )
      _delay_ms( 1 );
    
    sec--;
  }
}

/* ------------------------------------------------------------------------- */

/** Message handler function called by Serial_Processes() from Serial.c
  */
void MsgHandler(unsigned char newchar)
 {
//#ifdef USE_N4TXI_UART
//  SendByte( newchar );
//#else
//  uart_putc( newchar );
//#endif // USE_N4TXI_UART

  // just for testing ...
  LedGPSOn();
  
  // end of NMEA sentence reached ?

  if ( GpsMsgHandler( newchar ) == kTRUE ) {
  
    GpsMsgPrepare();

    if ( GpsDataIsComplete( &gGpsData ) ) {
    
      LcdDisplayShow();
    
      GpsDataClear( &gGpsData );
    }
//    // indicate somehow that we have no or old data
//    else {
//      //LedGPSOn();
//    }
//    
//    if ( GpsDataIsValid( &gGpsData ) ) {
//      gGPSDataQuality = kValidData;
//      LedGPSOn();
//    }
//    else {
//      LedGPSOn();
//      //LedGPSOff();
//    }
    
  } // if (  GpsMsgHandler() == kTRUE )
}

// --------------------------------------------------------------------------

static const PROGMEM char gCopyRight1[] = "GPSDisplay V1.0 ";
static const PROGMEM char gCopyRight2[] = " (C) DC2IP 2009";

/** main() function of GPSDisplay.c
  *
  *
  * This is the current LCD connection scheme:
  *
  * DATA0..3 = PC0..3
  * RS       = PD3
  * RW       = PD4
  * E        = PD5
  */
int main(void)
 {
  /* Initialize output ports for the LEDs */
  LedGPSInit();
  
  /* Initialize port for the button ... */
  BUTTON_DDR &= ~BUTTON1;
  /* ...and enable the pullup resistor. */
  BUTTON_PORT |= BUTTON1;
  
  /* Initialize the timer 1 to trigger every 10 ms */
  
  TCNT1 = CNT1_PRESET;
  TCCR1A = 0;                      // normal counter operating mode
  TCCR1B = (1<<CS12)|(1<<CS10);    // CK/1024 -> 14.4 kHz
  
  /* enable timer overflow interrupt */
  TIMSK |= (1<<TOIE1);
  
  /* Initialize serial communication functions */
#ifdef USE_N4TXI_UART
  SerInit();
#else
  uart_init( UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU) );
#endif // USE_N4TXI_UART

  /* initialize display, cursor off */
  lcd_init(LCD_DISP_ON);
  
  /* issue initial copyright message */
  lcd_clrscr();
  lcd_gotoxy( 0, 0 );
  lcd_puts_p( gCopyRight1 );
  lcd_gotoxy( 0, 1 );
  lcd_puts_p( gCopyRight2 );
  
  /* wait a bit and display the copyright message. */
  delay_sec( 5 );
  
  /* Now receiving with interrupts is possible, TX will no longer conflict */
  sei();

#ifndef USE_N4TXI_UART
  uart_puts_p( gCopyRight1 );
  uart_puts_p( gCopyRight2 );
  uart_puts_P( "\r\n" );
  
  unsigned int ch;
#endif // USE_N4TXI_UART
  
  unsigned int lcd_mode = kDateTime;
  
  LcdDisplaySetMode( lcd_mode );
  
  /* loop forever ... */
  while (1) {
    
#ifdef USE_N4TXI_UART
    Serial_Processes();
#else
    ch = uart_getc();
    
    if ( ch & UART_NO_DATA ) {
  
      // no data received -> continue
    }
    else {
      // check error flags first
      if ( ch & UART_FRAME_ERROR )
        //uart_puts_p( PSTR("UART Frame Error!\r\n") );
        uart_puts_P( "UART Frame Error!\r\n" );
      if ( ch & UART_OVERRUN_ERROR )
        uart_puts_P( "UART Overrun Error!\r\n" );
      if ( ch & UART_BUFFER_OVERFLOW )
        uart_puts_P( "Buffer overflow Error!\r\n");
    
      if ( ch & 0xff ) { // a valid character
      
        MsgHandler( (unsigned char)(ch & 0xff) );
      
      }
    }
#endif // USE_N4TXI_UART

    /* check if button has been pressed */
    
    if ( GetKeyPress( BUTTON1 ) ) {
      
      //while ( gKeyState & BUTTON1 );  // wait until button was released
    
      lcd_mode++; 
      
      if ( lcd_mode > kMaxDisplayMode ) lcd_mode = kTimeLocator;
      
      LcdDisplaySetMode( lcd_mode );
    }

  } // while (1) ...
  
  return 0;
}

// --------------------------------------------------------------------------

// ISR for timer/counter 1: called every 10 ms
// - load counter with initial constant
// - call button check routine

ISR(TIMER1_OVF_vect)
 {
  TCNT1 = CNT1_PRESET;
  
  // call button check routine
  CheckKeys();
}

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
