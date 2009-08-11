/*******************************************************************************
File:		Serial.h

 		Serial functions module definitions/declarations.

Version:	1.03

$Id: Serial.h,v 1.1 2009/08/11 10:14:20 avr Exp $

*******************************************************************************/

#ifndef _Serial_h_
#define _Serial_h_

// external function prototypes
extern void	SerialInit(void);
extern void	SerialPutByte(unsigned char chr);
extern void 	SerialPutString(const char *address);
extern void 	SerialPutString_p(const char *address);
extern void 	SerialProcesses(void);

#endif // _Serial_h_
