/*
 *  Memfile Operations Tables for the IMFS
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id: imfs_handlers_memfile.c,v 1.2 2001-09-27 12:01:15 chris Exp $
 */

#include <errno.h>

#include "imfs.h"

/*
 *  Set of operations handlers for operations on memfile entities.
 */

rtems_filesystem_file_handlers_r IMFS_memfile_handlers = {
  memfile_open,
  memfile_close,
  memfile_read,
  memfile_write,
  memfile_ioctl,
  memfile_lseek,
  IMFS_stat,
  IMFS_fchmod,
  memfile_ftruncate,
  NULL,                /* fpathconf */
  IMFS_fdatasync,      /* fsync */
  IMFS_fdatasync,
  IMFS_fcntl,
  memfile_rmnod
};
