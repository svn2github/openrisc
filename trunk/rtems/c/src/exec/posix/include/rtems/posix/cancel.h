/* rtems/posix/cancel.h
 *
 *  $Id: cancel.h,v 1.2 2001-09-27 11:59:14 chris Exp $
 */

#ifndef __RTEMS_POSIX_CANCEL_h
#define __RTEMS_POSIX_CANCEL_h

typedef struct {
  Chain_Node  Node;
  void      (*routine)( void * );
  void       *arg;
}  POSIX_Cancel_Handler_control;

/*
 *  _POSIX_Thread_cancel_run
 *
 *  DESCRIPTION:
 *
 *  This support routine runs through the chain of cancel handlers that
 *  have been registered and executes them.
 */

void _POSIX_Thread_cancel_run(
  Thread_Control *the_thread
);

#endif
/* end of include file */
