
//
// File   : gpstest.cc
//
// Purpose: Test program for GPS receiver string evaluation
//
// $Id: gpstest.cc,v 1.6 2009/08/10 15:05:53 avr Exp $
//


#include <iostream>
#include <sstream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>   // getopt() stuff

#include <SerialPort.h>

#include "GPS.h"
#include "LCDDisplay.h"

// --- C prototypes for linked ui.c

extern "C" {

  extern int getch(void);
  extern int kbhit(void);

} // extern "C+"

using namespace std;

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------

static void Usage(const char *pname)
 {
  cerr << "Usage: " << pname << " -p <serial-port> "
       << "[-i] [-o <outfile>]" << endl << endl;
  cerr << "Example: " << pname << " -i -p /dev/ttyS0 -o nmea.dat" << endl;
}

// --------------------------------------------------------------------------

static const char * gInitSequence[] = {

  "$PGRMO,GPGSV,0",
  "$PGRMIE",
  "$PGRALM"
};

//
// run with:
//  ./gpstest -p /dev/ttyUSB0
//

int main(int argc,char** argv)
 {
  if ( argc < 3 ) {
    Usage( argv[0] );
    exit( EXIT_FAILURE );
  }
  
  // --- read application parameters from the cmd line
  
  string outfile_name;
  string ser_device;
  bool do_init = false;
  
  int getopt_status;
  
  do {
    
    getopt_status = getopt( argc, argv, "ip:o:?" );
    
    if ( getopt_status == EOF ) break;
    
    // Note: getopt re-orders the argument list, i.e. first all
    //       arguments and their parameters with '-option' syntax,
    //       then the rest

    switch ( getopt_status ) {
      
      case 'i': do_init = true;
        	break;

      case 'o': outfile_name = optarg;
        	break;

      case 'p': ser_device = optarg;
        	break;

      case '?': Usage( argv[0] );
        	exit( EXIT_FAILURE );
        	break;

      default: printf ( "Encountered unknown option: %d,%c\n",
	       getopt_status, getopt_status );
    }
    
  } while ( getopt_status != EOF );

  // Open the serial port. 
  //
  SerialPort serial_port( ser_device );
  
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

  // initialisation of the GPS module (optional)
  //
  
  if ( do_init ) {
    
    ostringstream msg;
    unsigned char checksum;
    
    for ( unsigned int i=0; i<sizeof(gInitSequence)/sizeof(char *); i++ ) {
      
      msg.str( gInitSequence[i] );
      
      checksum = 0;
      for ( unsigned int s=0; s<strlen(gInitSequence[i]); s++ )
        checksum ^= gInitSequence[i][s];
      
      msg << '*';
      msg << hex << checksum << dec;
      msg << '\r';
      msg << '\n';
            
      try {
        serial_port.Write( msg.str() );
      }
      catch ( SerialPort::NotOpen& ex ) {
        cerr << "Error: Port is not open: " << ex.what() << endl;
        exit( EXIT_FAILURE );;
      }
      catch ( std::exception& ex ) {
        cerr << "Error: RunTime error: " << ex.what() << endl;
        exit( EXIT_FAILURE );;
      }
      catch (...) {
        cerr << "Caught unknown exception in SerialPort::Write()"
             << " in file " << __FILE__
             << ", line " << __LINE__
             << " !" << endl;
        exit(EXIT_FAILURE);
      }
    }
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
  
  FILE * outfile = NULL;
  
  if ( !outfile_name.empty() ) {
  
    outfile = fopen( outfile_name.c_str(), "w" );
    
    if ( !outfile )
      cerr << argv[0] << ": could not open NMEA data output file!" << endl;
  }
  
  while ( !leave ) {
    
    unsigned char ch, data;
    
    if ( serial_port.IsDataAvailable() ) {
    
      data = serial_port.ReadByte( 0 );
      
      gps_msg << data;

      if ( GpsMsgHandler( data ) == kTRUE ) {
      
        if ( outfile ) {
	  fprintf( outfile, "%s", gps_msg.str().c_str() );
	  fflush( outfile );
	}

        cout << gps_msg.str();
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
      
    } // if ( serial_port.IsDataAvailable() )
    else {
      usleep( 10 * 1000 ); // 10 msec, chars every 2 msec @ 4800 Bd
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
  
  if ( outfile )
    fclose( outfile );

  exit(EXIT_SUCCESS);
}

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
