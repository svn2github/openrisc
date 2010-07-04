/*
 *  $Id: ioctl.h,v 1.2 2001-09-27 11:59:35 chris Exp $
 */

#ifndef __SYS_IOCTL_h__
#define __SYS_IOCTL_h__

/* Functions */

int ioctl(
  int  fd,
  int  request,
  ... 
);

#endif
/* end of include file */
