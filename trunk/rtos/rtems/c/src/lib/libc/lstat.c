/*
 *  lstat() - BSD 4.3 and SVR4 - Get File Status
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id: lstat.c,v 1.2 2001-09-27 12:01:15 chris Exp $
 */

#define _STAT_NAME         lstat
#define _STAT_R_NAME       _lstat_r
#define _STAT_FOLLOW_LINKS FALSE

#include "stat.c"
