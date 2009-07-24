/*
 * File   : LcdDisplay.h
 *
 * Purpose: Display the GPS data on a 2*16 LCD display.
 *
 * $Id: LcdDisplay.h,v 1.3 2009/07/24 15:48:25 avr Exp $
 */

#ifndef _LcdDisplay_h_
#define _LcdDisplay_h_

/** @file LcdDisplay.h
  * Declarations & definitions from/for LcdDisplay.c
  * @author H.-J.Mathes, DC2IP
  */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** Possible display modes for the LCD display. */
typedef enum {

  kTimeLocator = 0,
  kDateTime = 1,    // the default display option
  kLatLon,
  kLocatorAltitude,
  kSpeedRoute,
  kDOP,
  
  kMaxDisplayMode = kDOP

} EDisplayMode;

extern void LcdDisplaySetMode(EDisplayMode);
extern void LcdDisplayShow(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LcdDisplay_h_ */
