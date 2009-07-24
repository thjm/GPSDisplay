
/*
 * File   : LcdDisplay.c
 *
 * Purpose: Implementation of the generic LCD display stuff
 *
 * $Id: LcdDisplay.c,v 1.3 2009/07/24 15:48:25 avr Exp $
 *
 */


//#if !(defined __AVR__)
# include <stdio.h>
//#endif /* __AVR__ */

#include <stdlib.h>
#include <string.h>

/** @file LcdDisplay.c
  * Routines for displaying GPS data on LCD display.
  * @author H.-J.Mathes, DC2IP
  */
 
#if (defined __AVR__)
# include <avr/pgmspace.h>
# include "lcd.h"
#endif /* __AVR__ */

#include "GPS.h"
#include "LcdDisplay.h"

static EDisplayMode gDisplayMode = kDateTime;

static char gLCDLine1[16];
static char gLCDLine2[16];

// --- local prototypes

static void LcdDisplayUpdate(void);

/* ------------------------------------------------------------------------- */

void LcdDisplayShow(void)
 {
  // update local memory
  
  LcdDisplayUpdate();
  
  // clear display

#if (defined __AVR__)
  lcd_clrscr();        // clear display and home cursor
#endif /* __AVR */

  // and output the data
    
#if (defined __AVR__)
  lcd_gotoxy( 0, 0 );
  for (unsigned int i=0; i<sizeof(gLCDLine1); i++ )
    lcd_putc( gLCDLine1[i] );
  
  lcd_gotoxy( 0, 1 );
  for (unsigned int i=0; i<sizeof(gLCDLine2); i++ )
    lcd_putc( gLCDLine2[i] );
#else
  printf("LCD0: '");
  for (unsigned int i=0; i<sizeof(gLCDLine1); i++ )
    printf("%c", gLCDLine1[i] );
  printf("'\n");
  
  printf("LCD1: '");
  for (unsigned int i=0; i<sizeof(gLCDLine2); i++ )
    printf("%c", gLCDLine2[i] );
  printf("'\n");
#endif /* __AVR__ */
}

/* ------------------------------------------------------------------------- */

void LcdDisplaySetMode(EDisplayMode mode)
 {
  gDisplayMode = mode;
}

/* ------------------------------------------------------------------------- */

/** predefined text strings for the 2*16 char LCD display.
  *
  * Note: On the AVR it should be inside the program memory (flash)!
  */
#if (defined __AVR__)
static const char *gLCDText[][2] /* PROGMEM */ = {
#else
static const char *gLCDText[][2] = {
#endif /* __AVR__ */
//  0123456789012345    0123456789012345
 { "   xx:xx:xxUT   ", "     JN49FD     " }, // kTimeLocator
 { "DATE:   .  .    ", "TIME:   :  :  UT" }, // kDateTime
 { "LAT:    °  '    ", "LON:    °  '    " }, // kLatLon
 { "LOCATOR:        ", "HEIGHT:        m" }, // kLocatorAltitude
 { "SPEED:      km/h", "ROUTE:         °" }, // kSpeedRoute
 { "HDOP:           ", "SATS:           " }, // kDOP
};

static void LcdDisplayUpdate(void)
 {
  char temp[10];
  char *src;
  int seconds;

  memset( gLCDLine1, ' ', sizeof(gLCDLine1));
  memset( gLCDLine2, ' ', sizeof(gLCDLine2));
  
  switch ( gDisplayMode ) {

    case kTimeLocator:
      memcpy( gLCDLine1, gLCDText[0][0], sizeof(gLCDLine1) );
      src = gGpsData.fTime;
      gLCDLine1[3] = *src++;
      gLCDLine1[4] = *src++;
      gLCDLine1[6] = *src++;
      gLCDLine1[7] = *src++;
      gLCDLine1[9] = *src++;
      gLCDLine1[10] = *src++;

      GpsCalculateLocator();
      memcpy( gLCDLine2, gLCDText[0][1], sizeof(gLCDLine2) );
      strncpy( &gLCDLine2[5], gLocator, 6 );
      break;

    case kLatLon:
      memcpy( gLCDLine1, gLCDText[2][0], sizeof(gLCDLine1) );
      
      src = gGpsData.fLatitude;
      gLCDLine1[6] = *src++;
      gLCDLine1[7] = *src++;
      gLCDLine1[9] = *src++;
      gLCDLine1[10] = *src++;
      
      strncpy( temp, &gGpsData.fLatitude[5], 4 ); temp[4] = 0;
      seconds = atoi( temp ) * 6;
      seconds /= 1000;
      sprintf(temp,"%02d",seconds);
      strncpy( &gLCDLine1[12], temp, 2);
      
      gLCDLine1[14] = '"';
      gLCDLine1[15] = gGpsData.fNorthSouth[0];

      memcpy( gLCDLine2, gLCDText[2][1], sizeof(gLCDLine2) );

      src = gGpsData.fLongitude;
      gLCDLine2[5] = *src++;
      gLCDLine2[6] = *src++;
      gLCDLine2[7] = *src++;
      gLCDLine2[9] = *src++;
      gLCDLine2[10] = *src++;

      strncpy( temp, &gGpsData.fLongitude[6], 4 ); temp[4] = 0;
      seconds = atoi( temp ) * 6;
      seconds /= 1000;
      sprintf(temp,"%02d",seconds);
      strncpy( &gLCDLine2[12], temp, 2);

      gLCDLine2[14] = '"';
      gLCDLine2[15] = gGpsData.fEastWest[0];
      break;
    
    case kLocatorAltitude:
      GpsCalculateLocator();
      memcpy( gLCDLine1, gLCDText[3][0], sizeof(gLCDLine1) );
      strncpy( &gLCDLine1[9], gLocator, 6 );

      memcpy( gLCDLine2, gLCDText[3][1], sizeof(gLCDLine2) );
      strncpy( &gLCDLine2[14-strlen(gGpsData.fAltitude)], 
               gGpsData.fAltitude, strlen(gGpsData.fAltitude) );
      break;

#if 0
    case kSpeedRoute:
      memcpy( gLCDLine1, gLCDText[4][0], sizeof(gLCDLine1) );
      memcpy( gLCDLine2, gLCDText[4][1], sizeof(gLCDLine2) );
      break;
#endif

    case kDOP:
      memcpy( gLCDLine1, gLCDText[5][0], sizeof(gLCDLine1) );
      
      memcpy( gLCDLine2, gLCDText[5][1], sizeof(gLCDLine2) );
      src = gGpsData.fSatellites;
      gLCDLine2[14] = *src++;
      gLCDLine2[15] = *src++;
      break;

    case kDateTime:
    default:
      memcpy( gLCDLine1, gLCDText[1][0], sizeof(gLCDLine1) );
      src = gGpsData.fDate;
      gLCDLine1[6] = *src++;
      gLCDLine1[7] = *src++;
      gLCDLine1[9] = *src++;
      gLCDLine1[10] = *src++;
      gLCDLine1[12] = *src++;
      gLCDLine1[13] = *src++;
      
      memcpy( gLCDLine2, gLCDText[1][1], sizeof(gLCDLine2) );
      src = gGpsData.fTime;
      gLCDLine2[6] = *src++;
      gLCDLine2[7] = *src++;
      gLCDLine2[9] = *src++;
      gLCDLine2[10] = *src++;
      gLCDLine2[12] = *src++;
      gLCDLine2[13] = *src++;
  }
}

/* ------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------- */
