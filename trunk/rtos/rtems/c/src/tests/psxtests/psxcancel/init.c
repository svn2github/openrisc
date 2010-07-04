/* 
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id: init.c,v 1.2 2001-09-27 12:02:25 chris Exp $
 */

#define CONFIGURE_INIT
#include "system.h"
#include <sched.h>

void *POSIX_Init(
  void *argument
)
{

  puts( "\n\n*** POSIX CANCEL TEST ***" );

  puts( "*** END OF POSIX CANCEL TEST ***" );
  exit( 0 );

  return NULL; /* just so the compiler thinks we returned something */
}
