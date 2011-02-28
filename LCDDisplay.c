
/*
 * File   : LCDDisplay.c
 *
 * Purpose: Implementation of the generic LCD display stuff
 *
 * $Id: LCDDisplay.c,v 1.9 2011/02/28 21:06:39 mathes Exp $
 *
 */


//#if !(defined __AVR__)
# include <stdio.h>
//#endif /* __AVR__ */

#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/** @file LCDDisplay.c
  * Routines for displaying GPS data on LCD display.
  * @author H.-J.Mathes, DC2IP
  */
 
#if (defined __AVR__)
# include <avr/pgmspace.h>
# include "lcd.h"
#else
# define PROGMEM
# define PGM_P      const char *
# define memcpy_P(_dest,_src,_size)  memcpy(_dest,_src,_size)
#endif /* __AVR__ */

#include "GPS.h"
#include "LCDDisplay.h"

static EDisplayMode gDisplayMode = kDateTime;

static char gLCDLine_0[16];
static char gLCDLine_1[16];

// --- local prototypes

static void LcdDisplayUpdate(void);

/* ------------------------------------------------------------------------- */

void LcdDisplayShow(void)
 {
  // update local memory
  
  LcdDisplayUpdate();
  
//
// http://www.mikrocontroller.net/topic/46867#new
// -> write LCD completely, don't clear it before !!!
//
//  // clear display
//
//#if (defined __AVR__)
//  lcd_clrscr();        // clear display and home cursor
//#endif /* __AVR */

  // and output the data
    
#if (defined __AVR__)
  lcd_gotoxy( 0, 0 );
  for (unsigned int i=0; i<sizeof(gLCDLine_0); i++ )
    lcd_putc( gLCDLine_0[i] );
  
  lcd_gotoxy( 0, 1 );
  for (unsigned int i=0; i<sizeof(gLCDLine_1); i++ )
    lcd_putc( gLCDLine_1[i] );
#else
  printf("      '0123456789012345'\n");
  printf("LCD0: '");
  for (unsigned int i=0; i<sizeof(gLCDLine_0); i++ )
    printf("%c", gLCDLine_0[i] );
  printf("'\n");
  
  printf("LCD1: '");
  for (unsigned int i=0; i<sizeof(gLCDLine_1); i++ )
    printf("%c", gLCDLine_1[i] );
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
  *       This must be done however in two steps, see also:
  *
  *       http://www.mikrocontroller.net/articles/AVR-GCC-Tutorial#Array_aus_Strings_im_Flash-Speicher
  *       http://www.nongnu.org/avr-libc/user-manual/FAQ.html#faq_rom_array
  */
//                                          0123456789012345
static const char gLCDText_0_0[] PROGMEM = "   xx:xx:xxUT   "; // kTimeLocator
static const char gLCDText_0_1[] PROGMEM = "     JN49FD     ";

static const char gLCDText_1_0[] PROGMEM = "DATE:   .  .    "; // kDateTime
static const char gLCDText_1_1[] PROGMEM = "TIME:   :  :  UT";

#if (defined __AVR__)
static const char gLCDText_2_0[] PROGMEM = "LAT:    \337  '    "; // kLatLon
static const char gLCDText_2_1[] PROGMEM = "LON:    \337  '    ";
#else
static const char gLCDText_2_0[] PROGMEM = "LAT:    °  '    "; // kLatLon
static const char gLCDText_2_1[] PROGMEM = "LON:    °  '    ";
#endif /* __AVR__ */

static const char gLCDText_3_0[] PROGMEM = "LOCATOR:        "; // kLocatorAltitude
static const char gLCDText_3_1[] PROGMEM = "HEIGHT:        m";

static const char gLCDText_4_0[] PROGMEM = "SPEED:    0 km/h"; // kSpeedRoute
#if (defined __AVR__)
static const char gLCDText_4_1[] PROGMEM = "ROUTE:       0 \337";
#else
static const char gLCDText_4_1[] PROGMEM = "ROUTE:       0 °";
#endif /* __AVR__ */

static const char gLCDText_5_0[] PROGMEM = "HDOP:           "; // kDOP
static const char gLCDText_5_1[] PROGMEM = "SATS:           ";

#if (defined __AVR__)
static const char gLCDText_6_0[] PROGMEM = "LAT:   \337  .     "; // kLatLonGeo
static const char gLCDText_6_1[] PROGMEM = "LON:   \337  .     ";
#else
static const char gLCDText_6_0[] PROGMEM = "LAT:   °  .      "; // kLatLonGeo
static const char gLCDText_6_1[] PROGMEM = "LON:   °  .      ";
#endif /* __AVR__ */


static PGM_P gLCDText_0[] PROGMEM = {
  gLCDText_0_0,
  gLCDText_1_0,
  gLCDText_2_0,
  gLCDText_3_0,
  gLCDText_4_0,
  gLCDText_5_0,
  gLCDText_6_0,
};

static PGM_P gLCDText_1[] PROGMEM = {
  gLCDText_0_1,
  gLCDText_1_1,
  gLCDText_2_1,
  gLCDText_3_1,
  gLCDText_4_1,
  gLCDText_5_1,
  gLCDText_6_1,
};

static void LcdDisplayUpdate(void)
 {
  char temp[10];
  char *src;
  const PGM_P pLCD_0;
  const PGM_P pLCD_1;

  // clear strings to be displayed next
  
  memset( gLCDLine_0, ' ', sizeof(gLCDLine_0));
  memset( gLCDLine_1, ' ', sizeof(gLCDLine_1));

  // read display masks from memory
  
  switch ( gDisplayMode ) {

    case kTimeLocator:
      
      memcpy_P( &pLCD_0, &gLCDText_0[0], sizeof(PGM_P) );
      memcpy_P( &pLCD_1, &gLCDText_1[0], sizeof(PGM_P) );
      break;

    case kLatLon:
      memcpy_P( &pLCD_0, &gLCDText_0[2], sizeof(PGM_P) );
      memcpy_P( &pLCD_1, &gLCDText_1[2], sizeof(PGM_P) );
      break;

    case kLatLonGeo:
      memcpy_P( &pLCD_0, &gLCDText_0[6], sizeof(PGM_P) );
      memcpy_P( &pLCD_1, &gLCDText_1[6], sizeof(PGM_P) );
      break;

    case kLocatorAltitude:
      memcpy_P( &pLCD_0, &gLCDText_0[3], sizeof(PGM_P) );
      memcpy_P( &pLCD_1, &gLCDText_1[3], sizeof(PGM_P) );
      break;

    case kSpeedRoute:
      memcpy_P( &pLCD_0, &gLCDText_0[4], sizeof(PGM_P) );
      memcpy_P( &pLCD_1, &gLCDText_1[4], sizeof(PGM_P) );
      break;

    case kDOP:
      memcpy_P( &pLCD_0, &gLCDText_0[5], sizeof(PGM_P) );
      memcpy_P( &pLCD_1, &gLCDText_1[5], sizeof(PGM_P) );
      break;

    case kDateTime:
    default:
      memcpy_P( &pLCD_0, &gLCDText_0[1], sizeof(PGM_P) );
      memcpy_P( &pLCD_1, &gLCDText_1[1], sizeof(PGM_P) );
  }
      
  memcpy_P( gLCDLine_0, pLCD_0, sizeof(gLCDLine_0) );
  memcpy_P( gLCDLine_1, pLCD_1, sizeof(gLCDLine_1) );
  
  // fill display strings with data

  uint16_t seconds;
    
  switch ( gDisplayMode ) {

    case kTimeLocator:
      
      src = gGpsData.fTime;
      gLCDLine_0[3] = *src++;
      gLCDLine_0[4] = *src++;
      gLCDLine_0[6] = *src++;
      gLCDLine_0[7] = *src++;
      gLCDLine_0[9] = *src++;
      gLCDLine_0[10] = *src++;

      GpsCalculateLocator();
      strncpy( &gLCDLine_1[5], gLocator, 6 );
      break;

    case kLatLon:
      
      src = gGpsData.fLatitude;
      gLCDLine_0[6] = *src++;
      gLCDLine_0[7] = *src++;
      gLCDLine_0[9] = *src++;
      gLCDLine_0[10] = *src++;
      
      strncpy( temp, &gGpsData.fLatitude[5], 4 ); temp[4] = 0;
      seconds = atoi( temp ) * 6;
      seconds /= 1000;
#if (defined __AVR__)
      if ( seconds < 10 ) {
        temp[0] = '0';
        itoa( seconds, &temp[1], 10 );
      }
      else
        itoa( seconds, temp, 10 );
#else
      sprintf(temp,"%02d",seconds);
#endif // __AVR__
      strncpy( &gLCDLine_0[12], temp, 2);
      
      gLCDLine_0[14] = '"';
      gLCDLine_0[15] = gGpsData.fNorthSouth[0];

      src = gGpsData.fLongitude;
      gLCDLine_1[5] = *src++;
      gLCDLine_1[6] = *src++;
      gLCDLine_1[7] = *src++;
      gLCDLine_1[9] = *src++;
      gLCDLine_1[10] = *src++;

      strncpy( temp, &gGpsData.fLongitude[6], 4 ); temp[4] = 0;
      seconds = atoi( temp ) * 6;
      seconds /= 1000;
#if (defined __AVR__)
      if ( seconds < 10 ) {
        temp[0] = '0';
        itoa( seconds, &temp[1], 10 );
      }
      else
        itoa( seconds, temp, 10 );
#else
      sprintf(temp,"%02d",seconds);
#endif // __AVR__
      strncpy( &gLCDLine_1[12], temp, 2);

      gLCDLine_1[14] = '"';
      gLCDLine_1[15] = gGpsData.fEastWest[0];
      break;
    
    case kLatLonGeo:
      
      src = gGpsData.fLatitude;
      gLCDLine_0[5] = *src++;
      gLCDLine_0[6] = *src++;
      for ( int i=8; i<=14; i++ )
        gLCDLine_0[i] = *src++;
      gLCDLine_0[15] = gGpsData.fNorthSouth[0];

      src = gGpsData.fLongitude;
      gLCDLine_1[4] = *src++;
      gLCDLine_1[5] = *src++;
      gLCDLine_1[6] = *src++;
      for ( int i=8; i<=14; i++ )
        gLCDLine_1[i] = *src++;
      gLCDLine_1[15] = gGpsData.fEastWest[0];
      break;
    
    case kLocatorAltitude:
      GpsCalculateLocator();
      strncpy( &gLCDLine_0[9], gLocator, 6 );

      strncpy( &gLCDLine_1[14-strlen(gGpsData.fAltitude)], 
               gGpsData.fAltitude, strlen(gGpsData.fAltitude) );
      break;

    case kSpeedRoute:
      if ( strlen(gGpsData.fSpeed) == 1 )
        strncpy( &gLCDLine_0[10], gGpsData.fSpeed, 1 );
      else if ( strlen(gGpsData.fSpeed) == 2 )
        strncpy( &gLCDLine_0[9], gGpsData.fSpeed, 2 );
      else
        strncpy( &gLCDLine_0[8], gGpsData.fSpeed, 3 );
      if ( strlen(gGpsData.fCourse) == 1 )
        strncpy( &gLCDLine_1[13], gGpsData.fCourse, 1  );
      else if ( strlen(gGpsData.fCourse) == 2 )
        strncpy( &gLCDLine_1[12], gGpsData.fCourse, 2  );
      else
        strncpy( &gLCDLine_1[11], gGpsData.fCourse, 3  );
      break;

    case kDOP:
      src = gGpsData.fHDOP;
      if ( gGpsData.fHDOP[1] == '.' )
        memcpy( &gLCDLine_0[13], src, 3 );
      else
        memcpy( &gLCDLine_0[12], src, 4 );
      
      src = gGpsData.fSatellites;
      gLCDLine_1[14] = *src++;
      gLCDLine_1[15] = *src++;
      break;

    case kDateTime:
    default:
      src = gGpsData.fDate;
      gLCDLine_0[6] = *src++;
      gLCDLine_0[7] = *src++;
      gLCDLine_0[9] = *src++;
      gLCDLine_0[10] = *src++;
      gLCDLine_0[12] = *src++;
      gLCDLine_0[13] = *src++;
      
      src = gGpsData.fTime;
      gLCDLine_1[6] = *src++;
      gLCDLine_1[7] = *src++;
      gLCDLine_1[9] = *src++;
      gLCDLine_1[10] = *src++;
      gLCDLine_1[12] = *src++;
      gLCDLine_1[13] = *src++;
  }
}

/* ------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------- */
