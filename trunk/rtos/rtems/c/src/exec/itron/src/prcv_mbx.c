/*
 *  ITRON 3.0 Mailbox Manager
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id: prcv_mbx.c,v 1.2 2001-09-27 11:59:13 chris Exp $
 */

#include <itron.h>

#include <rtems/itron/mbox.h>
#include <rtems/itron/task.h>

/*
 *  prcv_msg - Poll and Receive Message from Mailbox
 */

ER prcv_msg(
  T_MSG **ppk_msg,
  ID      mbxid
)
{
  return trcv_msg( ppk_msg, mbxid, TMO_POL );
}
