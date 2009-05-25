/*  bsp_cleanup()
 *
 *  This routine normally is part of start.s and returns
 *  control to a monitor but on the HP PA-RISC simulator
 *  we do that directly from main.c.
 *
 *  INPUT:  NONE
 *
 *  OUTPUT: NONE
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id: bspclean.c,v 1.2 2001-09-27 11:59:45 chris Exp $
 */

#include <bsp.h>

/*
 * The app has "exited" (called rtems_shutdown_executive)
 */

void bsp_cleanup( void )
{
    /*
     * Invoke any fatal error extension and "halt"
     * By definition, rtems_fatal_error_occurred does not return.
     */

    rtems_fatal_error_occurred(0);
}
