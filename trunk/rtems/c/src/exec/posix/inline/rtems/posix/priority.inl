/*  
 *  $Id: priority.inl,v 1.2 2001-09-27 11:59:14 chris Exp $
 */

#ifndef __RTEMS_POSIX_PRIORITY_inl
#define __RTEMS_POSIX_PRIORITY_inl

/*
 *  1003.1b-1993,2.2.2.80 definition of priority, p. 19
 *
 *  "Numericallly higher values represent higher priorities."
 *
 *  Thus, RTEMS Core has priorities run in the opposite sense of the POSIX API.
 */

RTEMS_INLINE_ROUTINE boolean _POSIX_Priority_Is_valid(
  int priority
)
{
  return (boolean) (priority >= 1 && priority <= 254);
}

RTEMS_INLINE_ROUTINE Priority_Control _POSIX_Priority_To_core(
  int priority
)
{
  return (Priority_Control) (255 - priority);
}

RTEMS_INLINE_ROUTINE int _POSIX_Priority_From_core(
  Priority_Control priority
)
{
  return 255 - priority;
}

#endif
