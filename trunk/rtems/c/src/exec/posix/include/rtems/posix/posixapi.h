/*
 *  POSIX API Support
 *
 *  NOTE:
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id: posixapi.h,v 1.2 2001-09-27 11:59:14 chris Exp $
 */
 
#ifndef __POSIX_API_h
#define __POSIX_API_h
 
#include <rtems/config.h>

/*PAGE
 *
 *  _POSIX_API_Initialize
 *
 *  XXX
 */
 
void _POSIX_API_Initialize(
  rtems_configuration_table *configuration_table
);

#endif
/* end of include file */
