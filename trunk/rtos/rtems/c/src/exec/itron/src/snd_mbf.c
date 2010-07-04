/*
 *  ITRON Message Buffer Manager
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id: snd_mbf.c,v 1.2 2001-09-27 11:59:13 chris Exp $
 */

#include <itron.h>

#include <rtems/itron/msgbuffer.h>
#include <rtems/itron/task.h>

/*
 *  snd_mbf - Send Message to MessageBuffer
 */

ER snd_mbf(
  ID  mbfid,
  VP  msg,
  INT msgsz
)
{
  return tsnd_mbf( mbfid, msg, msgsz, TMO_FEVR ); 
}
