
//
// File   : gpssim.cc
//
// Purpose: Program which provides GPS tracks with real time info
//          for testing GPS (NMEA) decoding programs and devices
//
// $Id: gpssim.cc,v 1.1 2009/08/10 15:05:53 avr Exp $
//


#include <iostream>
#include <list>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>   // getopt() stuff

#include <SerialPort.h>

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
       << "[-i <input_file>]" << endl << endl;
  cerr << "Example: " << pname << " -p /dev/ttyS0 -i gpstrack.dat" << endl;
}

// --------------------------------------------------------------------------

static unsigned char GetNMEAChecksum(const std::string data)
 {
  if ( !data.length() ) return 0;
  
  unsigned char checksum = 0x00;
  const char *ptr = data.c_str();
  
  while ( *ptr != '*' ) {
    checksum ^= *ptr++;
  }
  
  return checksum;
}

// --------------------------------------------------------------------------

//
// GPS track in NMEA format:
// - time (TTTTTT) and data (DDDDDD) will be replaced by software
// - the checksum will be calculated by software
//
static const char * gDefaultTrack[] = {

  "$GPRMC,TTTTTT,A,4905.7073,N,00826.0084,E,000.0,102.0,DDDDDD,000.1,E*",

  "$GPRMC,TTTTTT,A,4905.5918,N,00825.9907,E,000.0,102.0,DDDDDD,000.1,E*",
  "$GPRMC,TTTTTT,A,4905.6227,N,00825.7945,E,000.0,102.0,DDDDDD,000.1,E*",
  "$GPRMC,TTTTTT,A,4905.4167,N,00825.6683,E,000.0,102.0,DDDDDD,000.1,E*",
  "$GPRMC,TTTTTT,A,4905.4582,N,00826.8230,E,000.0,102.0,DDDDDD,000.1,E*",
  "$GPRMC,TTTTTT,A,4904.6253,N,00827.6905,E,000.0,102.0,DDDDDD,000.1,E*",
  "$GPRMC,TTTTTT,A,4906.8097,N,00829.4970,E,000.0,102.0,DDDDDD,000.1,E*",
  "$GPRMC,TTTTTT,A,4906.3332,N,00830.1773,E,000.0,102.0,DDDDDD,000.1,E*",
  "$GPRMC,TTTTTT,A,4906.8645,N,00828.5210,E,000.0,102.0,DDDDDD,000.1,E*",
  "$GPRMC,TTTTTT,A,4906.8058,N,00826.9370,E,000.0,102.0,DDDDDD,000.1,E*",
  "$GPRMC,TTTTTT,A,4906.9000,N,00826.3248,E,000.0,102.0,DDDDDD,000.1,E*",
  "$GPRMC,TTTTTT,A,4906.3295,N,00826.0783,E,000.0,102.0,DDDDDD,000.1,E*",
  "$GPRMC,TTTTTT,A,4905.8170,N,00825.8748,E,000.0,102.0,DDDDDD,000.1,E*",
  "$GPRMC,TTTTTT,A,4905.7723,N,00826.0663,E,000.0,102.0,DDDDDD,000.1,E*",

  "$GPRMC,TTTTTT,A,4905.7073,N,00826.0084,E,000.0,102.0,DDDDDD,000.1,E*",
  NULL
};

//
// run with:
//  ./gpssim -p /dev/ttyUSB0
//

int main(int argc,char** argv)
 {
  if ( argc < 3 ) {
    Usage( argv[0] );
    exit( EXIT_FAILURE );
  }
  
  // --- read application parameters from the cmd line
  
  string infile_name;
  string ser_device;
  
  int getopt_status;
  
  do {
    
    getopt_status = getopt( argc, argv, "p:o:?" );
    
    if ( getopt_status == EOF ) break;
    
    // Note: getopt re-orders the argument list, i.e. first all
    //       arguments and their parameters with '-option' syntax,
    //       then the rest

    switch ( getopt_status ) {

      case 'i': infile_name = optarg;
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

  // open input file if existing ...
  //
  
  FILE * infile = NULL;
  
  if ( !infile_name.empty() ) {
  
    infile = fopen( infile_name.c_str(), "w" );
    
    if ( !infile )
      cerr << argv[0] << ": could not open NMEA data input file!" << endl;
  }

  // array of GPS data (NMEA) strings
  //
  
  list<string> gps_data;
  
  if ( infile ) {  // from file
  }
  else {           // from default array
    unsigned int i = 0;
    
    while ( gDefaultTrack[i] != NULL ) {
      gps_data.push_back( gDefaultTrack[i++] );
    }
  }  

  // main loop ...
  //
  
  bool leave = false;
  bool send = true;

  list<string>::iterator gps_iter = gps_data.begin();
  time_t t0 = time(NULL);
  
  cout << "You might leave the main loop with 'q' or 'Q' ..." << endl;
  
  while ( !leave ) {
    
    unsigned char ch;
    char date_str[10], time_str[10];
    struct tm *tm;
    
    // check if time has elapsed and set 'send' flag eventually
    
    if ( (time(NULL) - t0) > 10 ) {
      send = true;
    }
    
    // 'send' flag is set:
    // - replace time and date fields
    // - calculate and append checeksum
    
    if ( send ) {
      
      t0 = time(NULL);
      tm = gmtime( &t0 );
      
      string send_str = *gps_iter;

      sprintf( time_str, "%02d%02d%02d", 
                         tm->tm_hour, tm->tm_min, tm->tm_sec );
      send_str.replace( send_str.find( "TTTTTT" ), 6, time_str );
      
      sprintf( date_str, "%02d%02d%02d", 
                         tm->tm_mday, tm->tm_mon, tm->tm_year - 100 );
      send_str.replace( send_str.find( "DDDDDD" ), 6, date_str );
      
      cout << send_str << (int)GetNMEAChecksum(send_str) << endl;
      
      gps_iter++;
      if ( gps_iter == gps_data.end() ) gps_iter = gps_data.begin();
      
      send = false;
    }
    
    if ( kbhit() ) {
    
      ch = getch();
      
      switch ( ch ) {

        case 'n':
        case 'N': cout << endl;
	          send = true;
	          break;

        case 'r':
        case 'R': cout << endl;
	          gps_iter = gps_data.begin();
	          send = true;
	          break;

        case 'q':
	case 'Q': cout << endl;
	          leave = 1;
	          break;
      }
      
    } // if (kbhit()) ...
    
    // sleep a while
    
    usleep( 10 * 1000 );
    
  } // while (!leave) ...
  
  cout << endl << argv[0] << ": main loop terminating..." << endl;
  
  // Close the serial port properly
  //
  serial_port.Close();
  
  if ( infile )
    fclose( infile );

  exit(EXIT_SUCCESS);
}

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
