/*
 * File   : LCDDisplay.h
 *
 * Purpose: Display the GPS data on a 2*16 LCD display.
 *
 * $Id: LCDDisplay.h,v 1.1 2009/07/24 15:52:40 avr Exp $
 */

#ifndef _LCDDisplay_h_
#define _LCDDisplay_h_

/** @file LCDDisplay.h
  * Declarations & definitions from/for LCDDisplay.c
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

#endif /* _LCDDisplay_h_ */
