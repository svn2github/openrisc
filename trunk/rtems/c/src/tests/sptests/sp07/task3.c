/*  Task_3
 *
 *  This routine serves as a low priority test task that should never
 *  execute.
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
  rtems_status_code status;

  while( FOREVER ) {
    status = rtems_task_wake_after( RTEMS_YIELD_PROCESSOR );
    directive_failed( status, "rtems_task_wake_after" );
  }
}
