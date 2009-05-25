/*
 *  exit
 *
 *  This routine returns control to "the pre-RTEMS environment".
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id: exit.c,v 1.2 2001-09-27 12:01:15 chris Exp $
 */

#include <bsp.h>
#include <clockdrv.h>

void _exit( void )
{
   /* Clock or Timer cleanup is run by at_exit() */

   Io_cleanup();

   bsp_cleanup();
}
