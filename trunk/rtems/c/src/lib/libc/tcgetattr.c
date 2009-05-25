/*
 *  tcgetattr() - POSIX 1003.1b 7.2.1 - Get and Set State
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id: tcgetattr.c,v 1.2 2001-09-27 12:01:15 chris Exp $
 */

#include <rtems.h>
#if defined(RTEMS_NEWLIB)

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <termios.h>
/* #include <sys/ioctl.h> */

int ioctl();

#include <rtems/libio.h>

int tcgetattr(
  int fd,
  struct termios *tp
)
{
  return ioctl( fd, RTEMS_IO_GET_ATTRIBUTES, tp );
}
#endif
