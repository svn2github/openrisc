/*
 *  Some C Libraries reference this routine since they think getpid is
 *  a real system call.
 *
 *  $Id: __getpid.c,v 1.2 2001-09-27 12:01:15 chris Exp $
 */

#include <unistd.h>

pid_t __getpid(void)
{
  return getpid();
}

