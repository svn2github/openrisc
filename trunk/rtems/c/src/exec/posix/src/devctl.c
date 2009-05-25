/*
 *  $Id: devctl.c,v 1.2 2001-09-27 11:59:17 chris Exp $
 */

#include <devctl.h>

#include <rtems/system.h>

/*PAGE
 *
 *  21.2.1 Control a Device, P1003.4b/D8, p. 65
 */

int devctl(
  int     filedes,
  void   *dev_data_ptr,
  size_t  nbyte,
  int    *dev_info_ptr
)
{
  return POSIX_NOT_IMPLEMENTED();
}
