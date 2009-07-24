
/*
 * File   : get8key4.h
 *
 * Purpose: Contains definitions of the get8key4.c file
 *
 * $Id: get8key4.h,v 1.1 2009/07/24 15:52:40 avr Exp $
 */


#ifndef _get8key4_h_
#define _get8key4_h_

#include <stdint.h>

/* --- declaration of global vars: file get8key4.c --- */

extern volatile uint8_t gKeyState;
extern volatile uint8_t gKeyPress;

extern void CheckKeys(void);
extern uint8_t GetKeyPress(uint8_t key_mask);
extern uint8_t GetKeyShort(uint8_t key_mask);

#endif /* _get8key4_h_ */
