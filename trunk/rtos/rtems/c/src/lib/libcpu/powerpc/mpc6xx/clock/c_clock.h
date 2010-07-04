/*
 *  Clock Tick Device Driver
 *
 *  This routine utilizes the Decrementer Register common to the PPC family.
 *
 *  The tick frequency is directly programmed to the configured number of
 *  microseconds per tick.
 *
 *  COPYRIGHT (c) 1989-1997.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may in
 *  the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  Modified to support the MPC750.
 *  Modifications Copyright (c) 1999 Eric Valette valette@crf.canon.fr
 *
 *  $Id: c_clock.h,v 1.2 2001-09-27 12:01:24 chris Exp $
 */

#ifndef _LIB_LIBCPU_C_CLOCK_H
#define _LIB_LIBCPU_C_CLOCK_H

#include <rtems.h>
#include <bsp.h>

/*
 * Theses functions and variables represent the API exported by the CPU to the BSP
 */
extern void clockOff	(void* unused);
extern void clockOn	(void* unused);
extern void clockIsr	(void);
extern int  clockIsOn	(void* unused);

#endif





