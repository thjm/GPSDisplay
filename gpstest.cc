
//
// File   : gpstest.cc
//
// Purpose: Test program for GPS receiver string evaluation
//
// $Id: gpstest.cc,v 1.1 2009/07/10 10:59:00 avr Exp $
//


#include <iostream>
#include <sstream>
#include <cstdio>
#include <cstdlib>

#include <SerialPort.h>

#include "GPS.h"
#include "LcdDisplay.h"

// --- C prototypes for linked ui.c

extern "C" {

  extern int getch(void);
  extern int kbhit(void);

} // extern "C+"

using namespace std;

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------

//
// run with:
//  ./gpstest /dev/ttyUSB0
//


int main(int argc,char** argv)
 {
  if ( argc < 2 ) {
    cerr << "Usage: " << argv[0] << " <serial-port>" << endl;
    exit( EXIT_FAILURE );
  }
  
  // Open the serial port. 
  //
  SerialPort serial_port( argv[1] );
  
  // Set communication parameters & open the port, catch exceptions
  //
  try {
    serial_port.Open( SerialPort::BAUD_4800,
		      SerialPort::CHAR_SIZE_8,
		      SerialPort::PARITY_NONE,
		      SerialPort::STOP_BITS_1,
		      SerialPort::FLOW_CONTROL_NONE );
  }
  catch ( SerialPort::AlreadyOpen& ex ) {
    cerr << "Error: Port is already open: " << ex.what() << endl;
    exit(EXIT_FAILURE);
  }
  catch ( SerialPort::OpenFailed& ex ) {
    cerr << "Error: Could not open port: " << ex.what() << endl;
    exit(EXIT_FAILURE);
  }
  catch ( ... ) {
    cerr << "Caught unknown exception in SerialPort::Open()"
            << " in file " << __FILE__
            << ", line " << __LINE__
            << " !" << endl;
    exit(EXIT_FAILURE);
  }

  // test code initialisation
  //
  GpsMsgInit();
  
  LcdDisplaySetMode( kDateTime );
  
  // main loop ...
  //
  
  bool leave = false;
  
  cout << "You might leave the main loop with 'q' or 'Q' ..." << endl;
  
  ostringstream gps_msg;
  int display_mode = 0;
  
  while ( !leave ) {
    
    unsigned char ch, data;
    
    if ( serial_port.IsDataAvailable() ) {
    
      data = serial_port.ReadByte( 0 );
      
      gps_msg << data;

      if ( GpsMsgHandler( data ) == kTRUE ) {
      
        cout << "GPS: " << gps_msg.str();
	gps_msg.str("");
	
        GpsMsgPrepare();
	
	if ( GpsDataIsComplete( &gGpsData ) ) {
	  
	  GpsMsgShow();
	  
	  LcdDisplayShow();
	  
	  GpsDataClear( &gGpsData );
	}
	
	// switch display mode from time to time (AVR: done via push button)
	//
	
	switch ( display_mode ) {
	  case 0: LcdDisplaySetMode( kTimeLocator );
	          break;
          
	  case 2: LcdDisplaySetMode( kLatLon );
	          break;
          
	  case 3: LcdDisplaySetMode( kLocatorAltitude );
	          break;
          
	  case 4: LcdDisplaySetMode( kSpeedRoute );
	          break;
          
	  case 5: LcdDisplaySetMode( kDOP );
	          break;
          
	  default: LcdDisplaySetMode( kDateTime );
	}
      }
    }
    
    if ( kbhit() ) {
    
      ch = getch();
      
      switch ( ch ) {

        case 'q':
	case 'Q': leave = 1;
	          break;

	default:  display_mode++; 
	          display_mode %= 6;
      }
      
    } // if (kbhit()) ...
    
  } // while (!leave) ...
  
  cout << endl << argv[0] << ": main loop terminating..." << endl;
  
  // Close the serial port properly
  //
  serial_port.Close();
  
  exit(EXIT_SUCCESS);
}

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
