
/*
 * File   : get8key4.c
 *
 * Purpose: debounce n (n<=8) buttons, sample 4 times
 *          http://www.mikrocontroller.net/topic/6492#new
 *          Author: Peter Dannegger
 *
 * $Id: get8key4.c,v 1.1 2009/07/24 15:52:40 avr Exp $
 */

/************************************************************************/
/*                                                                      */
/*                      Debouncing 8 Keys				*/
/*			Sampling 4 Times				*/
/*                                                                      */
/*              Author: Peter Dannegger                                 */
/*                      danni@specs.de                                  */
/*                                                                      */
/************************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>

#define KEY_INPUT	PIND

#include "global.h"

volatile uint8_t gKeyState = 0;
volatile uint8_t gKeyPress = 0;

// --------------------------------------------------------------------------

void CheckKeys(void)
{
  static uint8_t ct0, ct1;
  uint8_t i;

  i = gKeyState ^ ~KEY_INPUT;	// key changed ?

  ct0 = ~( ct0 & i );		// reset or count ct0
  ct1 = ct0 ^ ( ct1 & i );      // reset or count ct1
  i &= ct0 & ct1;		// count until roll over
  gKeyState ^= i;		// then toggle debounced state
  gKeyPress |= gKeyState & i;	// 0->1: key pressing detect
}

// --------------------------------------------------------------------------

uint8_t GetKeyPress(uint8_t key_mask)
 {
  cli();          // read and clear atomic !

  key_mask &= gKeyPress;                        // read key(s)
  gKeyPress ^= key_mask;                        // clear key(s)

  sei();

  return key_mask;
}

// --------------------------------------------------------------------------

uint8_t GetKeyShort(uint8_t key_mask)
 {
  cli();      // read key state and key press atomic !

  return GetKeyPress( ~gKeyState & key_mask );
}

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
