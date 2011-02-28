/*
 * File   : GPS.h
 *
 * Purpose: GPS messaging definitions/declarations for the atmega8.
 *
 * $Id: GPS.h,v 1.6 2011/02/28 12:11:33 mathes Exp $
 */

#ifndef _GPS_h_
#define _GPS_h_

/** @file GPS.h
  * Declarations for file GPS.c
  * @author
  */

#define kTRUE   (1==1)
#define kFALSE  (1==0)

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** Enum to indicate the different GPS (NMEA) message types. */
typedef enum {

  kNONE = 0,
  
  kGPRMC = 1,
  kGPGGA = 2,
  
  kGPVTG = 3,
  
  kGPGSA = 9    // detected, but not decoded
  
} EGPSSentenceType;

/** Declaration of status bits. */
enum {
  kComplete = 0x01,
  kValid    = 0x80
};

/** A structure filled with position and date/time data. */
typedef struct {

  unsigned char  fStatus;              // Status bits to indicate ...
  
#if (defined GPS_NAVILOCK)
  char  fTime[11];		       // UTC time in HHMMSS.sss format
#else
  char  fTime[7];		       // UTC time in HHMMSS format
#endif // GPS_NAVILOCK
#ifndef APRS
  char  fDate[7];                      // Date in DDMMYY format
#endif /* APRS */
  char  fLatitude[10];  	       // Latitude in DDMM.MMMM format
  char  fNorthSouth[1];                // 'N' or 'S'
  char  fLongitude[11]; 	       // Longitude in DDDMM.MMMM format
  char  fEastWest[1];                  // 'E' or 'W'
  char  fAltitude[8];		       // Altitude (meters) in MMM.MMM format
#ifndef APRS
  char  fSpeed[7];		       // Speed (knots) in kkk.kk format
#else
  char  fSpeed[7];		       // Speed (km/h) in kkk.kk format
#endif /* APRS */
  char  fCourse[7];		       // Track angle (degrees) in ddd.dd format
#ifndef APRS
  char  fHDOP[5];                      // (H)DOP, precision value
#endif /* APRS */
  char  fSatellites[3]; 	       // Number of Satellites tracked

} GpsData_t;

/** Clear the status field of the GpsData_t struct. */
#define GpsDataClear(_gps_data) { (_gps_data)->fStatus = 0; }

/** Set the 'complete' bit of the GpsData_t struct. */
#define GpsDataSetComplete(_gps_data) { (_gps_data)->fStatus |= kComplete; }
#define GpsDataSetIncomplete(_gps_data) { (_gps_data)->fStatus &= ~kComplete; }

/** Check if GpsData_t struct contains complete data. */
#define GpsDataIsComplete(_gps_data) (((_gps_data)->fStatus & kComplete) ? 1 : 0)

/** Set the 'valid' bit of the GpsData_t struct. */
#define GpsDataSetValid(_gps_data) { (_gps_data)->fStatus |= kValid; }
#define GpsDataSetInvalid(_gps_data) { (_gps_data)->fStatus &= ~kValid; }

/** Check if GpsData_t struct contains valid data. */
#define GpsDataIsValid(_gps_data) (((_gps_data)->fStatus & kValid) ? 1 : 0)

/** To exchange the stable GPS data with other software modules. */
extern GpsData_t gGpsData;

/** Initialize some data structures of this software module. */
extern void GpsMsgInit(void);

/** Convert the acquired GPS data to usable data (for APRS etc.). */
extern void GpsMsgPrepare(void);

/** Handle incoming characters from GPS and parse them. */
extern unsigned char GpsMsgHandler(unsigned char newchar);

/** Altitude (feet) in FFFFFF format */
extern char gAltitudeFeet[];

/** Convert the GPS altitude (usually in meters) into feet. */
extern void GpsCalculateFeet(void);

#ifndef APRS
/** The Maidenhead locator calculated form latitude and longitude. */
extern char gLocator[];

/** Calculate the Maidenhead grid locator from gGpsData.
  *
  * The resulting, 6 characters long string is in gLocator.
  */
extern void GpsCalculateLocator(void);
#endif /* APRS */

#ifndef APRS
#if !(defined __AVR__)
extern void GpsMsgShow(void);
#endif /* __AVR__ */
#endif /* APRS */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _GPS_h_ */
