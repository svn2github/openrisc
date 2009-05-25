/*
 *  $Id: geteuid.c,v 1.2 2001-09-27 11:59:17 chris Exp $
 */

#include <limits.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>

#include <rtems/system.h>
#include <rtems/score/object.h>
#include <rtems/posix/seterr.h>

uid_t _POSIX_types_Euid = 0;

/*PAGE
 *
 *  4.2.1 Get Real User, Effective User, Ral Group, and Effective Group IDs, 
 *        P1003.1b-1993, p. 84
 */

uid_t geteuid( void )
{
  return _POSIX_types_Euid;
}
