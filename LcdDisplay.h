/*
 * File   : LcdDisplay.h
 *
 * Purpose: Display the GPS data on a 2*16 LCD display.
 *
 * $Id: LcdDisplay.h,v 1.1 2009/07/10 10:59:00 avr Exp $
 */

#ifndef _LcdDisplay_h_
#define _LcdDisplay_h_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** Possible display modes for the LCD display. */
typedef enum {

  kTimeLocator = 0,
  kDateTime = 1, // the default
  kLatLon,
  kLocatorAltitude,
  kSpeedRoute,
  kDOP,

} EDisplayMode;

extern void LcdDisplaySetMode(EDisplayMode);
extern void LcdDisplayShow(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LcdDisplay_h_ */
