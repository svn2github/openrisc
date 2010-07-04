/*  _Shm_isr()
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id: mpisr.c,v 1.2 2001-09-27 12:01:12 chris Exp $
 */

#include <rtems.h>
#include "shm_driver.h"

rtems_isr Shm_isr(
  rtems_vector_number vector
)
{
  Shm_Interrupt_count += 1;
  rtems_multiprocessing_announce();
}
