/*
 *  $Id: sysconf.c,v 1.2 2001-09-27 11:59:17 chris Exp $
 */

#include <time.h>
#include <unistd.h>

#include <rtems/system.h>

/*PAGE
 *
 *  4.8.1 Get Configurable System Variables, P1003.1b-1993, p. 95
 */

long sysconf(
  int name
)
{
  return POSIX_NOT_IMPLEMENTED();
}
