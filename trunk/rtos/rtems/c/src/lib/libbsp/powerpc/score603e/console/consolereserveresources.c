/*
 *  This file contains the routine console_reserve_resources
 *  for the Score603e console driver.
 *
 *  COPYRIGHT (c) 1989-1997.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id: consolereserveresources.c,v 1.2 2001-09-27 12:01:03 chris Exp $
 */

#include <bsp.h>
#include <rtems/libio.h>
#include <stdlib.h>
#include <assert.h>

#include "consolebsp.h"

int console_reserve_resources_removed;

