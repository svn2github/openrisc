/*
 *  IMFS_fdatasync
 *
 *  The following routine does a sync on an IMFS node.  The In Memory
 *  File System is always in sync, therefore this routine always returns
 *  pass.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id: imfs_fdatasync.c,v 1.2 2001-09-27 12:01:15 chris Exp $
 */

#include "imfs.h"

int IMFS_fdatasync(
  rtems_libio_t *iop
)
{
  return 0;
}
