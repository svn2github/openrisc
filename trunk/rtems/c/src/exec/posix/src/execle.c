/* 
 *  execle() - POSIX 1003.1b 3.1.2
 *
 *  $Id: execle.c,v 1.2 2001-09-27 11:59:17 chris Exp $
 */

#include <errno.h>

int execle(
  const char *path,
  char const *arg,
  ...
)
{
  errno = ENOSYS;
  return -1;
}
