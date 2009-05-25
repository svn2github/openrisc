/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id: bspclean.c,v 1.2 2001-09-27 12:01:04 chris Exp $
 */

void bsp_cleanup( void )
{
  asm volatile( "li  10,99" );  /* 0x63 */
  asm volatile( "sc" );
}
