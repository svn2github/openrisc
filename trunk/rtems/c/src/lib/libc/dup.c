/*
 *  dup() - POSIX 1003.1b 6.2.1 Duplicate an Open File Descriptor
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id: dup.c,v 1.2 2001-09-27 12:01:15 chris Exp $
 */

#include <unistd.h>
#include <fcntl.h>

int dup(
  int fildes
)
{
  return fcntl( fildes, F_DUPFD, 0 );
}
