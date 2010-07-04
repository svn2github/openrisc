/* 
 *  fork() - POSIX 1003.1b 3.1.1
 *
 *  $Id: fork.c,v 1.2 2001-09-27 11:59:17 chris Exp $
 */

#include <sys/types.h>
#include <errno.h>

int fork( void )
{
  errno = ENOSYS;
  return -1;
}
