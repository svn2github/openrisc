/* 
 *  execv() - POSIX 1003.1b 3.1.2
 *
 *  $Id: execv.c,v 1.2 2001-09-27 11:59:17 chris Exp $
 */

#include <errno.h>

int execv(
  const char *file,
  char *const argv[],
  ...
)
{
  errno = ENOSYS;
  return -1;
}
