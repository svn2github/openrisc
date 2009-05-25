/* 
 *  wait() - POSIX 1003.1b 3.2.1
 *
 *  $Id: waitpid.c,v 1.2 2001-09-27 11:59:17 chris Exp $
 */

#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

int waitpid(
  pid_t pid,
  int *stat_loc,
  int options
)
{
  errno = ENOSYS;
  return -1;
}
