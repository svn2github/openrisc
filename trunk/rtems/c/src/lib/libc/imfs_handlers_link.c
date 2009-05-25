/*
 *  Link Operations Table for the IMFS
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id: imfs_handlers_link.c,v 1.2 2001-09-27 12:01:15 chris Exp $
 */

#include <errno.h>

#include "imfs.h"

/*
 *  Handler table for IMFS device nodes
 */

rtems_filesystem_file_handlers_r IMFS_link_handlers = {
  NULL,   /* open */
  NULL,   /* close */
  NULL,   /* read */
  NULL,   /* write */
  NULL,   /* ioctl */
  NULL,   /* lseek */
  NULL,   /* stat */
  NULL,   /* fchmod */
  NULL,   /* ftruncate */
  NULL,   /* fpathconf */
  NULL,   /* fsync */
  NULL,   /* fdatasync */
  NULL,   /* fcntl */
  IMFS_rmnod
};
