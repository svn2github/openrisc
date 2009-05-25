/* 
 *  execve() - POSIX 1003.1b 3.1.2
 *
 *  $Id: execve.c,v 1.2 2001-09-27 11:59:17 chris Exp $
 */

#include <errno.h>

int execve(
  const char *path,
  char *const argv[],
  char *const envp[],
  ...
)
{
  errno = ENOSYS;
  return -1;
}
