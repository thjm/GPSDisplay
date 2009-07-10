
//
// File   : ui.c
//
// Purpose: File to be included with Unix implementation of kbhit() and getch()
//
// $Id: ui.c,v 1.1 2009/07/10 10:59:00 avr Exp $
//


#include <stdio.h>
#include <stdlib.h>

/** @file ui.c
  * File to be linked or included to provide us with an Unix implementation of 
  * kbhit() and getch().
  * @author H.-J. Mathes, FzK
  */

/* ------------------------------------------------------------------------- */
/* ---     special functions for linux as kbhit() or getch() on PC       --- */
/* ------------------------------------------------------------------------- */

#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <termios.h>

/** Get a character from the console.
  *
  * It is not necessary to preee <Enter> to send the character to the 
  * application
  */
int getch(void)
 {
  struct termios org_mode, cur_mode;
  int tty_desc;
  char ch;

  tty_desc = open ( "/dev/tty", O_RDWR );

  ioctl ( tty_desc, TCGETS, &org_mode );
  ioctl ( tty_desc, TCGETS, &cur_mode );

  cur_mode.c_iflag &= ~ICRNL;
  cur_mode.c_lflag &= ~(ICANON | ECHO);
  cur_mode.c_cc[VTIME] = 0;
  cur_mode.c_cc[VMIN] = 1;
  ioctl ( tty_desc, TCSETS, &cur_mode );

  read ( tty_desc, &ch, 1 );

  ioctl ( tty_desc, TCSETS, &org_mode );
  close ( tty_desc );

  return ch;
}

/* ------------------------------------------------------------------------- */

/** Return 1 if any key on the keyboard was pressed.
  *
  * The value of the key can then be inquired using the getch() function.
  */
int kbhit(void)
 {
  struct termios org_mode, cur_mode;
  int tty_desc;
  int nchars, result = 0;

  tty_desc = open ( "/dev/tty", O_RDWR );

  ioctl ( tty_desc, TCGETS, &org_mode );
  ioctl ( tty_desc, TCGETS, &cur_mode );

  cur_mode.c_iflag &= ~ICRNL;
  cur_mode.c_lflag &= ~(ICANON | ECHO);
  cur_mode.c_cc[VTIME] = 0;
  cur_mode.c_cc[VMIN] = 1;
  ioctl ( tty_desc, TCSETS, &cur_mode );

  ioctl ( tty_desc, FIONREAD, &nchars );
  if ( nchars > 0 ) result = 1;

  ioctl ( tty_desc, TCSETS, &org_mode );
  close ( tty_desc );

  return result;
}

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------
