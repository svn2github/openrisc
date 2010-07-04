/*
 *  $Id: sersupp.h,v 1.2 2001-09-27 12:01:42 chris Exp $
 */

#ifndef __LIBCHIP_SERIAL_SUPPORT_h
#define __LIBCHIP_SERIAL_SUPPORT_h

int termios_baud_to_index(
  int termios_baud
);

int termios_baud_to_number(
  int termios_baud
);

boolean libchip_serial_default_probe(
  int minor
);


#endif
/* end of include file */
