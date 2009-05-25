/*  Task_create_extension
 *
 *  This routine is the tcreate user extension.
 *
 *  Input parameters:
 *    unused       - pointer to currently running TCB
 *    created_task - pointer to new TCB being created
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
 *  $Id: tcreate.c,v 1.2 2001-09-27 12:02:31 chris Exp $
 */

#include "system.h"

rtems_boolean Task_create_extension(
  rtems_tcb *unused,
  rtems_tcb *created_task
)
{
  if ( task_number( created_task->Object.id ) > 0 ) {
    puts_nocr( "TASK_CREATE - " );
    put_name( Task_name[ task_number( created_task->Object.id ) ], FALSE );
    puts( " - created." );
  }
  return TRUE;
}
