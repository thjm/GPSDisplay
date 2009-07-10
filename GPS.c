
/*
 * File   : GPS.c
 *
 * Purpose: Implementation of the GPS stuff
 *
 * $Id: GPS.c,v 1.1 2009/07/10 10:59:00 avr Exp $
 *
 */

#include <stdio.h>
#include <stdlib.h>  // atoi()
#include <string.h>

#include "GPS.h"


GpsData_t gGpsData;                             // externally visible variables

static GpsData_t gTempGpsData;			// Temporary variables used for decoding.

char gAltitudeFeet[7];				// Altitude (feet) in FFFFFF format

#ifndef APRS
char gLocator[7];				// Maidenhead locator
#endif /* APRS */

static EGPSSentenceType	gSentenceType;		// GPRMC, GPGGA, or unrecognized

/* ------------------------------------------------------------------------- */

void GpsMsgInit(void)
/*
 * ABSTRACT:	Initialize some of the fields in case we transmit before the GPS
 *				has lock and sends us valid data.
 *
 * INPUT:	None
 * OUTPUT:	None
 * RETURN:	None
 */
 {
  memset( &gTempGpsData, sizeof(gTempGpsData), 0);
  
  gTempGpsData.fSpeed[0]      = gTempGpsData.fSpeed[1] 
                              = gTempGpsData.fSpeed[2]    = '0';
  gTempGpsData.fCourse[0]     = gTempGpsData.fCourse[1] 
                              = gTempGpsData.fCourse[2]   = '0';
  gTempGpsData.fAltitude[0]   = gTempGpsData.fAltitude[1] = '0';
  gTempGpsData.fAltitude[2]   = '.';
  
  gTempGpsData.fNorthSouth[0] = 'N';
  gTempGpsData.fEastWest[0]   = 'E';

} // End GpsMsgInit

/* ------------------------------------------------------------------------- */

void GpsMsgPrepare(void)
/*
 * ABSTRACT:	Call this function right before sending a position report for two
 *				reasons. This copies all the temp strings into transmit strings so
 *				they are not modified by the GPS receive handler.  Altitude is also
 *				converted into feet from meters.
 *
 * INPUT:	None
 * OUTPUT:	None
 * RETURN:	None
 */
 {
  GpsDataClear( &gGpsData );
  
  strcpy( gGpsData.fTime, gTempGpsData.fTime ); 	    // latest Time
  strcpy( gGpsData.fDate, gTempGpsData.fDate ); 	    // latest Date
  strcpy( gGpsData.fLatitude, gTempGpsData.fLatitude );     // latest Latitude
  gGpsData.fNorthSouth[0] = gTempGpsData.fNorthSouth[0];
  strcpy( gGpsData.fLongitude, gTempGpsData.fLongitude );   // latest Longitude
  gGpsData.fEastWest[0] = gTempGpsData.fEastWest[0];
  strcpy( gGpsData.fAltitude, gTempGpsData.fAltitude );     // latest Altitude
  strcpy( gGpsData.fSpeed, gTempGpsData.fSpeed );	    // latest Speed
  strcpy( gGpsData.fCourse, gTempGpsData.fCourse );	    // latest Course
  strcpy( gGpsData.fSatellites, gTempGpsData.fSatellites ); // latest Satellites

  // convert altitude string into feet
  GpsCalculateFeet();
  
  // finally manipulate the status bits ...
  if ( GpsDataIsValid( &gTempGpsData ) ) GpsDataSetValid( &gGpsData );

  GpsDataSetComplete( &gGpsData );
  GpsDataClear( &gTempGpsData );
  
} // End GpsMsgPrepare(void)

/* ------------------------------------------------------------------------- */

unsigned char GpsMsgHandler(unsigned char newchar)
/*
 * ABSTRACT:	Processes the characters coming in from USART.  In this case,
 *				this is the port connected to the gps receiver.
 *
 * INPUT:	newchar		Next character from the serial port.
 * OUTPUT:	None
 * RETURN:	kTRUE if message complete, kFALSE otherwise
 */
 {
  static unsigned char    commas;	       	// Number of commas for far in sentence
  static unsigned char    index;	       	// Individual array index

  if (newchar == 0) {			       	// A NULL character resets GPS decoding
    commas = 25;			       	// Set to an outrageous value
    gSentenceType = kNONE;		       	// Clear local parse variable
    GpsDataClear( &gTempGpsData );
    return kFALSE;
  }

  if (newchar == '$') { 		       	// Start of Sentence character, reset
    commas = 0; 			       	// No commas detected in sentence for far
    gSentenceType = kNONE;		       	// Clear local parse variable
    return kFALSE;
  }

  if (newchar == ',') { 		       	// If there is a comma
    commas += 1;			       	// Increment the comma count
    index = 0;  			       	// And reset the field index
    return kFALSE;
  }

  if ( (newchar == '\n') &&                      // If there is a newline char
      (gSentenceType == kGPRMC || gSentenceType == kGPGGA) ) {
    GpsDataSetComplete( &gTempGpsData );
    return kTRUE;
  }
  
  if (commas == 0) {
  	  
    switch (newchar) {
    
      case ('C'):		      		// Only the GPRMC sentence contains a "C"
  	  gSentenceType = kGPRMC; 		// Set local parse variable
  	  break;
      
      case ('S'):		      		// Take note if sentence contains an "S"
  	  gSentenceType = kGPGSA; 		// ...because we don't want to parse it
  	  break;
      
      case ('A'):		      		// The GPGGA sentence ID contains "A"
  	  if (gSentenceType != kGPGSA)    	// As does GPGSA, which we will ignore
  	    gSentenceType = kGPGGA; 		// Set local parse variable
  	  break;
    }

    return kFALSE;
  }

  //
  // example of $GPGGA sentence:
  //
  // "$GPGGA,111148,4905.7046,N,00826.0110,E,1,04,5.1,130.6,M,47.9,M,,*41"
  //
  // = Global Positioning System Fixed Data
  //
  
  if (gSentenceType == kGPGGA) { 		// GPGGA sentence decode initiated

    switch (commas) {
    
      case 1: 					// Time field
  	  gTempGpsData.fTime[index++] = newchar;
  	  return kFALSE;

      case 2: 					// Latitude field
  	  gTempGpsData.fLatitude[index++] = newchar;
  	  return kFALSE;

      case 3:					// N/S indicator
          gTempGpsData.fNorthSouth[index++] = newchar;
  	  return kFALSE;

      case 4: 					// Longitude field
  	  gTempGpsData.fLongitude[index++] = newchar;
  	  return kFALSE;

      case 5:					// E/W indicator
          gTempGpsData.fEastWest[index++] = newchar;
  	  return kFALSE;

#if 1
      case 6: 					// GPS quality indication
  	  if (newchar == '1' || newchar == '2')
	    { GpsDataSetValid( &gTempGpsData ); }
	  else
	    { GpsDataSetInvalid( &gTempGpsData ); }
	  return kFALSE;
#endif

      case 7: 					// Satellite field
  	  gTempGpsData.fSatellites[index++] = newchar;
  	  return kFALSE;

#ifndef APRS
      case 8: 					// HDOP field
  	  return kFALSE;
#endif /* APRS */

      case 9: 					// MSL Altitude field [meters]
  	  gTempGpsData.fAltitude[index++] = newchar;
  	  return kFALSE;

#if 0
      case 9: 					// Unit for MSL Altitude field
  	  return kFALSE;
#endif
    }

    return kFALSE;

  } // end if (gSentenceType == kGPGGA)

  //
  // example of $GPRMC sentence:
  //
  // "$GPRMC,111148,A,4905.7046,N,00826.0110,E,000.0,069.4,070709,000.1,E*78"
  //
  // = Recommended Minimum Specific GNSS Data
  //
  
  if (gSentenceType == kGPRMC) { 		// GPGGA sentence decode initiated

    switch (commas) {

      case 2:
          if (newchar == 'A')  			// 'A' = valid, 'V' = invalid
	    { GpsDataSetValid( &gTempGpsData ); }
	  else
	    { GpsDataSetInvalid( &gTempGpsData ); }
          return kFALSE;

#if 0
      case 3: 					// Latitude field
  	  gTempGpsData.fLatitude[index++] = newchar;
  	  return kFALSE;
#endif

#if 0
      case 4:					// N/S indicator
          gTempGpsData.fNorthSouth[index++] = newchar;
  	  return kFALSE;
#endif

#if 0
      case 5: 					// Longitude field
  	  gTempGpsData.fLongitude[index++] = newchar;
  	  return kFALSE;
#endif

#if 0
      case 6:					// E/W indicator
          gTempGpsData.fEastWest[index++] = newchar;
  	  return kFALSE;
#endif

      case 7: 					// Speed field [knots]
  	  gTempGpsData.fSpeed[index++] = newchar;
  	  return kFALSE;

      case 8: 					// Course field [degrees]
  	  gTempGpsData.fCourse[index++] = newchar;
  	  return kFALSE;

#ifndef APRS
      case 9: 					// Date field
  	  gTempGpsData.fDate[index++] = newchar;
  	  return kFALSE;
#endif /* APRS */
    }

    return kFALSE;

  } // end if (sentence_type == GPRMC)

  return kFALSE;
  
} // End MsgHandler(unsigned char newchar)

/* ------------------------------------------------------------------------- */

void GpsCalculateFeet(void)
 {
  static unsigned long    lAltitude;   	// Used to convert meters to feet
  static unsigned long    lTemp;       	// Just a long temp variable
  static unsigned char    index;	// For indexing local arrays
  static unsigned char    count;	// Keeps track of loops in F-to-A conv.

  index = 0;						// Reset index for this search
  gGpsData.fAltitude[6] = '.';  			// Force last character to a '.'

  while (gGpsData.fAltitude[++index] != '.');	  	// Find the decimal in Alt string

  lAltitude = 0;					// Begin with a blank slate
  while (index) {		  			// This is Float-to-A, working from the decimal leftward

    lAltitude += (gGpsData.fAltitude[--index] - 48) * lTemp; // Right to left
    lTemp *= 10;					// Each digit is worth 10x previous
  }
  // The lAltitude variable now contains the altitude in meters.

  printf("lAltitude= %ld m\n",lAltitude);
  
  // The following is an approximation of 3.28 to convert Meters to Feet
  lAltitude *= 3;					// Start by multiplying by 3
  lAltitude += lAltitude>>4;		  		// add to self/16  (3.1875)
  lAltitude += lAltitude>>6;		  		// add to self/64  (3.2373)
  lAltitude += lAltitude>>7;		  		// add to self/128 (3.2626)
  lAltitude += lAltitude>>8;		  		// add to self/256 (3.2758)
  lAltitude += lAltitude>>10;	  	  		// add to self/1024 (3.279)
  // The lAltitude variable now contains the altitude in feet.

  // This converts a long to ASCII with six characters & leading zeros.
  //
  // #include <stdlib.h>
  //
  // char * ltoa(long l, char * buffer, int radix);
  //
  count = 0;									  // Convert to character each cycle
  index = 0;									  // Start on left and work right
  for (lTemp = 100000 ; lTemp > 1; lTemp /= 10) {
  
    while (lAltitude >= lTemp) {	    	// Convert each order of 10 to a #

      lAltitude -= lTemp; 	      		// By looping, sub'ing each time
      count++;  				// Keep track of iteration loops
    }
    gAltitudeFeet[index++] = count + 48; 	// Save that count as a character
    count = 0;  				// Reset count and start over
  }

  gAltitudeFeet[index++] = lAltitude + 48;  	// Last digit resides in LongAlt...
  gAltitudeFeet[index] = 0;  			// Terminate string

  printf("gAltitudeFeet= %s ft\n", gAltitudeFeet);
}

/* ------------------------------------------------------------------------- */

#ifndef APRS

// results in 6 char locator string in variable gLocator
void GpsCalculateLocator(void)
 {
  for (int i=0; i<sizeof(gLocator); i++ ) gLocator[i] = ' ';
  char temp[5];
  int degrees;
  
  // --- 1st character
  
  strncpy( temp, gGpsData.fLongitude, 3 ); temp[3] = 0;
  
  if ( gGpsData.fEastWest[0] == 'W' )
    degrees = 180 - atoi( temp );
  else
    degrees = 180 + atoi( temp );

  gLocator[0] = 'A' + degrees / 20;

  // --- 3rd character
  
  gLocator[2] = '0' + ((degrees % 20) >> 1);
  
  // --- 5th character
  
  strncpy( temp, &gGpsData.fLongitude[4], 2 ); temp[2] = 0;
  
  gLocator[4] = 'A' + atoi( temp ) / 24;
  
  // --- 2nd character
  
  strncpy( temp, gGpsData.fLatitude, 2 ); temp[2] = 0;
  
  if ( gGpsData.fNorthSouth[0] == 'S' )
    degrees = 90 - atoi( temp );
  else
    degrees = 90 + atoi( temp );

  gLocator[1] = 'A' + degrees / 10;
  
  // --- 4th character

  gLocator[3] = '0' + degrees % 10;
  
  // --- 6th character

  strncpy( temp, &gGpsData.fLatitude[2], 2 ); temp[2] = 0;
  
  gLocator[5] = 'A' + (atoi( temp ) << 1) / 24;

  // finally add the trailing \000
  
  gLocator[6] = 0;
}

#endif /* APRS */

/* ------------------------------------------------------------------------- */

#ifndef APRS

void GpsMsgShow(void)
 {
  printf( "---------------------------------\n" );
  printf( "stat:  0x%02x\n", gGpsData.fStatus );
  printf( "valid: %s\n", ( (GpsDataIsValid( &gGpsData )) ? "yes" : "no" ) );
  printf( "time:  %s\n", gGpsData.fTime );
  printf( "date:  %s\n", gGpsData.fDate );
  printf( "lat:   %s\n", gGpsData.fLatitude );
  printf( "long:  %s\n", gGpsData.fLongitude );
  printf( "alt:   %s\n", gGpsData.fAltitude );
  printf( "vel:   %s\n", gGpsData.fSpeed );
  printf( "dir:   %s\n", gGpsData.fCourse );
  printf( "sats:  %s\n", gGpsData.fSatellites );
  printf( "---------------------------------\n" );
}

#endif /* APRS */

/* ------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------- */

