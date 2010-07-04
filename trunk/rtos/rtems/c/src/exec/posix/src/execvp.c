/* 
 *  execvp() - POSIX 1003.1b 3.1.2
 *
 *  $Id: execvp.c,v 1.2 2001-09-27 11:59:17 chris Exp $
 */

#include <errno.h>

int execvp(
  const char *path,
  char const *argv[]
)
{
  errno = ENOSYS;
  return -1;
}
