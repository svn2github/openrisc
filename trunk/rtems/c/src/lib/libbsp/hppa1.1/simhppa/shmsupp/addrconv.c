/*  Shm_Convert_address
 *
 *  No address range conversion is required.
 *
 *  Input parameters:
 *    address - address to convert
 *
 *  Output parameters:
 *    returns - converted address
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id: addrconv.c,v 1.2 2001-09-27 11:59:45 chris Exp $
 */

#include <bsp.h>
shm_driverinclude <shm_driver.h>

void *Shm_Convert_address(
  void *address
)
{
  return ( address );
}
