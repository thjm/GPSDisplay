
/*
 * File   : GPSDisplay.c
 *
 * Purpose: Contains main() of project GPSDisplay
 *
 * $Id: GPSDisplay.c,v 1.9 2011/03/03 13:14:11 mathes Exp $
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
 #include <uart.h>
#endif /* USE_N4TXI_UART */

#include <lcd.h>

#ifdef USE_N4TXI_UART
 #include "Serial.h"
#endif /* USE_N4TXI_UART */

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

static volatile int8_t gGPSDataQuality = kNoSignal;

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

#ifndef USE_N4TXI_UART

/** Function SerialProcesses() called for P.Fleury UART-library.
  */
void SerialProcesses(void) {

  unsigned int ch;

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
}

#endif /* USE_N4TXI_UART */

/* ------------------------------------------------------------------------- */

/** Message handler function called by SerialProcesses() from Serial.c
  */
void MsgHandler(unsigned char newchar)
 {
//// output GPS data also to serial interface
//#ifdef USE_N4TXI_UART
//  SerialPutByte( newchar );
//#else
//  uart_putc( newchar );
//#endif // USE_N4TXI_UART

  // just for testing ...
  LedGPSOn();

  // end of NMEA sentence reached ?

  if ( GpsMsgHandler( newchar ) == kTRUE ) {

    GpsMsgPrepare();

    if ( GpsDataIsComplete( &gGpsData ) && !GpsDataIsValid( &gGpsData ) )
      gGPSDataQuality = kOldData;
    else if ( GpsDataIsValid( &gGpsData ) )
      gGPSDataQuality = kValidData;

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

static const PROGMEM char gCopyRight1[] = "GPS-Display V1.3";
static const PROGMEM char gCopyRight2[] = " (C) DC2IP 2011";

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
  static uint8_t first = 1;

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
  SerialInit();
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

#ifdef USE_N4TXI_UART
  SerialPutString( "\r\n" );
  SerialPutString_p( gCopyRight1 );
  SerialPutString_p( gCopyRight2 );
  SerialPutString( "\r\n" );
#else
  uart_puts_P( "\r\n" );
  uart_puts_p( gCopyRight1 );
  uart_puts_p( gCopyRight2 );
  uart_puts_P( "\r\n" );
#endif // USE_N4TXI_UART

  uint8_t lcd_mode = kDateTime;

  LcdDisplaySetMode( lcd_mode );

  /* loop forever ... */
  while (1) {

    SerialProcesses();

#if 1
    /* display no signal message, if gGPSDataQuality == kNoSignal */

    if ( gGPSDataQuality == kNoSignal && first == 1 ) {

      lcd_clrscr();

      lcd_gotoxy( 0, 0 );
      lcd_puts_P("No GPS signal !");

      first = 0;

      continue;
    }
#endif

    /* check if button has been pressed */

    if ( (gGPSDataQuality == kValidData) && GetKeyPress( BUTTON1 ) ) {

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
