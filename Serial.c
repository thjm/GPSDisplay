/*******************************************************************************
File:		Serial.c

		Serial I/O subsystem function library.

Functions:	extern void	SerialInit(void)
		extern void	SerialPutByte(unsigned char chr)
		extern void 	SerialPutString(const char *address)
		extern void 	SerialPutString_p(const char *progmem_address)
		extern void 	SerialProcesses(void)
		ISR(USART_RXC_vect)
		ISR(USART_TXC_vect)

Revisions:	1.00	04/14/03	GND	Original - Gary N. Dion
		1.01	11/01/04	GND	Modified for ISR based transmit
		1.02	11/02/04	GND	Optimized the ASCII routine (later removed)
		1.03	05/26/05	GND	Converted to C++ comment style

Copyright:	(c)2005, Gary N. Dion (me@garydion.com). All rights reserved.
		This software is available only for non-commercial amateur radio
		or educational applications.  ALL other uses are prohibited.
		This software may be modified only if the resulting code be
		made available publicly and the original author(s) given credit.

$Id: Serial.c,v 1.1 2009/08/11 10:14:20 avr Exp $

*******************************************************************************/

// OS headers
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include "global.h"

// defined in GPSDisplay.c
extern void MsgHandler(unsigned char);

// App required include files
#include "Serial.h"

// Educated guess for good buffer sizes
#define UART_RX_BUFFER_SIZE	(96)
#define UART_TX_BUFFER_SIZE	(96)

// variables for USART RX part
static unsigned char inbuf[UART_RX_BUFFER_SIZE];	// USART input buffer array
static unsigned char inhead;				// USART input buffer head pointer
static unsigned char intail;				// USART input buffer tail pointer

// variables for USART TX part
static unsigned char outbuf[UART_TX_BUFFER_SIZE];	// USART output buffer array
static unsigned char outhead;				// USART output buffer head pointer
static unsigned char outtail;				// USART output buffer tail pointer

// Baud rate calculations (see http://www.mikrocontroller.net/articles/AVR-GCC-Tutorial#UART_initialisieren)
#define BAUD 4800L          					// Baud rate, the L is important, DON'T use UL !

#define UBRR_VAL ((F_CPU+BAUD*8)/(BAUD*16)-1)   // clever rounding
#define BAUD_REAL (F_CPU/(16*(UBRR_VAL+1)))     // real baud rate
#define BAUD_ERROR ((BAUD_REAL*1000)/BAUD-1000) // error in promille

//#if ((BAUD_ERROR>10) || (BAUD_ERROR<-10))
#if (BAUD_ERROR>10)
# error Systematic error of baud rate larger than 1% and thus too high!
#endif

/******************************************************************************/
void		SerialInit(void)
/*******************************************************************************
* ABSTRACT:	This function initializes the serial USART.
*
*		It sets up the baud rate, data and stop bits, and parity
*
* INPUT:	None
* OUTPUT:	None
* RETURN:	None
*/
{
  // Set baud rate of USART to 4800 baud at 14.7456 MHz
#if 0
  UBRRH = 0;
  UBRRL = 191;
#else
  UBRRH = UBRR_VAL >> 8;
  UBRRL = UBRR_VAL & 0xFF;
#endif

  // Set frame format to 8 data bits, no parity, and 1 stop bit
  UCSRC = (1<<URSEL)|(3<<UCSZ0);

  // Enable Receiver and Transmitter Interrupt, Receiver and Transmitter
  UCSRB = (1<<RXCIE)|(1<<TXCIE)|(1<<RXEN)|(1<<TXEN);

} // End SerialInit(void)


/******************************************************************************/
void		SerialPutByte(unsigned char chr)
/*******************************************************************************
* ABSTRACT:	This function pushes a new character into the output buffer
*		then pre-advances the pointer to the next empty location.
*
* INPUT:	chr			byte to send
* OUTPUT:	None
* RETURN:	None
*/
{
  if (++outhead == UART_TX_BUFFER_SIZE) outhead = 0;	  // Advance and wrap pointer
  outbuf[outhead] = chr;				  // Transfer the byte to output buffer

  if (UCSRA & (1<<UDRE))				  // If the transmit buffer is empty
  {
    if (++outtail == UART_TX_BUFFER_SIZE) outtail = 0;// Advance and wrap pointer
    UDR = outbuf[outtail];			    // Place the byte in the output buffer
  }

} // End SerialPutByte(unsigned char chr)


/******************************************************************************/
void SerialPutString(const char *address)
/*******************************************************************************
* ABSTRACT:	This function sends a null-terminated string to the serial port.
*
* INPUT:	*address	Pointer to string to send (ASCIIZ)
* OUTPUT:	None
* RETURN:	None
*/
{
  while (*address != 0) 				  // While not to the string end yet
  {
    SerialPutByte(*(address++));			    // Send the byte and increment index
  }

} // End SerialPutString(char *address)


/******************************************************************************/
void SerialPutString_p(const char *progmem_address)
/*******************************************************************************
* ABSTRACT:	This function sends a null-terminated string from PROGMEM
*		(= flash memory) to the serial port.
*
* INPUT:	*address	Pointer to string to send (ASCIIZ)
* OUTPUT:	None
* RETURN:	None
*/
{
  register char c;
  
  while ( (c = pgm_read_byte(progmem_address++)) )   // While not to the string end yet
    SerialPutByte(c);

} // End SerialPutString(char *address)


/******************************************************************************/
void 	SerialProcesses(void)
/*******************************************************************************
* ABSTRACT:	Called by main.c during idle time. 
*
*		Processes any waiting serial characters coming in or going 
*		out both serial ports.
*
* INPUT:	None
* OUTPUT:	None
* RETURN:	None
*/
{
  if (intail != inhead) 			  // If there are incoming bytes pending
  {
    if (++intail == UART_RX_BUFFER_SIZE) intail = 0;	    // Advance and wrap pointer
    MsgHandler(inbuf[intail]);  	    // And pass it to a handler
  }

} // End SerialProcesses(void)


/******************************************************************************/
ISR(USART_RXC_vect)
/*******************************************************************************
* ABSTRACT:	Called by the receive ISR (interrupt). 
*
*		Saves the next serial byte to the head of the RX buffer.
*
* INPUT:	None
* OUTPUT:	None
* RETURN:	None
*/
{
  if (++inhead == UART_RX_BUFFER_SIZE) inhead = 0;	  // Advance and wrap buffer pointer
  inbuf[inhead] = UDR;  				  // Transfer the byte to the input buffer

} // End ISR(USART_RXC_vect)


/******************************************************************************/
ISR(USART_TXC_vect)
/*******************************************************************************
* ABSTRACT:	Called by the transmit ISR (interrupt). 
*
*		Puts the next serial byte into the TX register.
*
* INPUT:	None
* OUTPUT:	None
* RETURN:	None
*/
{
  if (outtail != outhead)				// If there are outgoing bytes pending
  {
    if (++outtail == UART_TX_BUFFER_SIZE) outtail = 0;	// Advance and wrap pointer
    UDR = outbuf[outtail];			    	// Place the byte in the output buffer
  }

} // End ISR(USART_TXC_vect)
