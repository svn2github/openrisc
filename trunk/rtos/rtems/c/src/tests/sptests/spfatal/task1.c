/*  Task_1
 *
 *  This routine serves as a test task.  It verifies the task manager.
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
 *  $Id: task1.c,v 1.2 2001-09-27 12:02:38 chris Exp $
 */

#include "system.h"

rtems_task Task_1(
  rtems_task_argument argument
)
{
  puts( "\n\n*** TEST FATAL ***" );
  puts( "TA1 - exitting task" );
}
