
/*
 * File   : testLCD.c
 *
 * Purpose: Test the LCD attached to the board (WhereAVR)
 *
 * $Id: testLCD.c,v 1.1 2009/07/16 13:30:52 avr Exp $
 *
 */


#include <stdint.h>
#include <stdlib.h>

#include <avr/io.h>
#include <avr/pgmspace.h>


/** @file testLCD.c
  * Test the LCD attached to the board (WhereAVR).
  * @author H.-J. Mathes (DC2IP)
  */

#include <lcd.h>

#ifndef F_CPU
# warning F_CPU not defined, assuming 1 MHz
# define F_CPU 1000000
#endif // F_CPU

/* ------------------------------------------------------------------------- */

void delay_ms(unsigned short ms)
/* delay for a minimum of <ms> */
/* with a 1Mhz clock, the resolution is 1 ms */
 {
  // Note: this function is faulty, see avrm8ledtest-0.2.tar.gz for
  //	   updated code.
  unsigned short outer1, outer2;
  outer1 = 50 * (F_CPU/1000000UL); 

  while ( outer1 ) {
    outer2 = 1000;
    while ( outer2 ) {
      while ( ms ) ms--;
      outer2--;
    }
    outer1--;
  }
}

/* ------------------------------------------------------------------------- */

/** For programming the 'copyright' character into the display. */
static const PROGMEM unsigned char copyRightChar[] = {
  0x07, 0x08, 0x13, 0x14, 0x14, 0x13, 0x08, 0x07,
  0x00, 0x10, 0x08, 0x08, 0x08, 0x08, 0x10, 0x00
};

/** main() function of testLCD.c
  *
  * The code here is basically taken from lcdtest.c which goes back to
  * the test example of P.Fleury lcdlibrary. The main difference is that no
  * button is used, i.e. the displayed stuff changes in a slow loop.
  *
  * This is the current connection scheme:
  * DATA0..3 = PC0..3
  * RS       = PD3
  * RW       = PD4
  * E        = PD5
  */
int main(void)
 {
  int test = 0;
  char buffer[7];
  int  num = 1904;
  
  /* initialize display, cursor off */
  lcd_init(LCD_DISP_ON);

  /* main loop (endless) */

  while (1) {
  
    switch ( test ) {
    
      case 0:   /* clear display and home cursor */
        lcd_clrscr();
	break;
	
      case 1:   /* print characters in two lines */
        lcd_gotoxy( 0, 0 );
	lcd_puts("0123456789ABCDEF");
	lcd_gotoxy( 0, 1 );
	lcd_puts("0123456789ABCDEF");
	break;

      case 2:   /* write text to display */
        lcd_clrscr();
	/* put string to display (line 1) with linefeed */
	lcd_puts("LCD Test Line 1\n");
	/* cursor is now on second line, write second line */
	lcd_puts("Line 2");
	/* move cursor to position 8 on line 2 */
	lcd_gotoxy(7,1); 
	lcd_putc(':');
	break;

      case 3:  /* use lcd_command() to turn on cursor */
        /* turn on cursor */
	lcd_command(LCD_DISP_ON_CURSOR);
	/* put string */
	lcd_puts( "CurOn");
        break;

      case 4:  /* display shift */
        lcd_clrscr();
        /* put string from program memory to display */
        lcd_puts_P( "Line 1 longer than 14 characters\n" );
        lcd_puts_P( "Line 2 longer than 14 characters" );
	delay_ms(500);
	/* move BOTH lines one position to the left */
	lcd_command(LCD_MOVE_DISP_LEFT);
	delay_ms(500);
	/* turn off cursor */
	lcd_command(LCD_DISP_ON);
	break;

     case 5:  /* Display integer values */
       lcd_clrscr();
       /* convert integer into string */
       itoa( num , buffer, 10);
       /* put converted string to display */
       lcd_puts(buffer);
       break;

     case 6:  /* Display userdefined characters */
       lcd_clrscr();
       lcd_puts("Copyright: ");
       /* load two userdefined characters from program memory
        * into LCD controller CG RAM location 0 and 1
        */
       lcd_command(_BV(LCD_CGRAM));  /* set CG RAM start address 0 */
       for ( int i=0; i<16; i++) {
         lcd_data(pgm_read_byte_near(&copyRightChar[i]));
       }

       /* move cursor to position 0 on line 2 */
       /* Note: this switches back to DD RAM adresses */
       lcd_gotoxy(0,1);

       /* display user defined (c), built using two user defined chars */
       lcd_putc(0);
       lcd_putc(1);
       break;

    } // switch(test) ...
    
    test++; if ( test > 6 ) test = 0;
    
    delay_ms(1000);
  }
  
  return 0;
}

/* ------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------- */
