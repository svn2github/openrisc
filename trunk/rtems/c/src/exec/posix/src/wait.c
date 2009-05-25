/* 
 *  waitpid() - POSIX 1003.1b 3.2.1
 *
 *  $Id: wait.c,v 1.2 2001-09-27 11:59:17 chris Exp $
 */

#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

int wait(
  int   *stat_loc
)
{
  errno = ENOSYS;
  return -1;
}
