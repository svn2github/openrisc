/*
 *  3.4.3 Delay Process Execution, P1003.1b-1993, p. 81
 *
 *  $Id: sleep.c,v 1.2 2001-09-27 11:59:17 chris Exp $
 */

#include <time.h>
#include <unistd.h>

#include <rtems/system.h>


unsigned int sleep(
  unsigned int seconds
)
{
  /* XXX can we get away with this implementation? */
  struct timespec tp;
  struct timespec tm;

  tp.tv_sec = seconds;
  tp.tv_nsec = 0;

  nanosleep( &tp, &tm );

  return tm.tv_sec;       /* seconds remaining */
}
