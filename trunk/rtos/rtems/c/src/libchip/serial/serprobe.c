/*
 *  $Id: serprobe.c,v 1.2 2001-09-27 12:01:42 chris Exp $
 */

#include <rtems.h>
#include <libchip/serial.h>
#include "sersupp.h"

boolean libchip_serial_default_probe(int minor)
{
  /*
   * If the configuration dependent probe has located the device then
   * assume it is there
   */

  return TRUE;
}



