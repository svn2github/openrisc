/*
 *  $Id: getpid.c,v 1.2 2001-09-27 11:59:17 chris Exp $
 */

#include <limits.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>

#include <rtems/system.h>
#include <rtems/score/object.h>
#include <rtems/posix/seterr.h>

/*PAGE
 *
 *  4.1.1 Get Process and Parent Process IDs, P1003.1b-1993, p. 83
 */

pid_t getpid( void )
{
  return _Objects_Local_node;
}

/*
 *  _getpid_r
 *
 *  This is the Newlib dependent reentrant version of getpid().
 */

#if defined(RTEMS_NEWLIB)

#include <reent.h>

pid_t _getpid_r(
  struct _reent *ptr
)
{
  return getpid();
}
#endif

