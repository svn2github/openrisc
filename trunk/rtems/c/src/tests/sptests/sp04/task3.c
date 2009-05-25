/*  Task_3
 *
 *  This routine serves as a test task.  It is just a CPU bound task
 *  requiring timesliced operation.
 *
 *  Input parameters:
 *    argument - task argument
 *
 *  Output parameters:  NONE
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id: task3.c,v 1.2 2001-09-27 12:02:31 chris Exp $
 */

#include "system.h"

rtems_task Task_3(
  rtems_task_argument argument
)
{
  while( FOREVER );
}
