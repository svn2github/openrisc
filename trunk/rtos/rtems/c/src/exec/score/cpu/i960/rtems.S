/*  rtems.s
 *
 *  This file contains the single entry point code for
 *  the i960 implementation of RTEMS.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id: rtems.S,v 1.2 2001-09-27 11:59:27 chris Exp $
 */

        .text

        .align  4
        .globl  RTEMS

RTEMS:
        ld        __Entry_points[g7*4],r4
        bx        (r4)

