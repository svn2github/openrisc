/*
 *  $Id: utime.h,v 1.2 2001-09-27 11:59:35 chris Exp $
 */

#ifndef __UTIME_h__
#define __UTIME_h__

/*
 *  POSIX 1003.1b 5.6.6 Set File Access and Modification Times
 */

struct utimbuf {
  time_t  actime;   /* Access time */
  time_t  modtime;  /* Modification time */
};

/* Functions */

int utime(
  const char           *path,
  const struct utimbuf *times
);

#endif
/* end of include file */
