/*
 * mc68ec.c -- Low level support for the Motorola mc68ec0x0 board.
 *             Written by rob@cygnus.com (Rob Savoye)
 *
 *  $Id: mc68ec.c,v 1.2 2001-09-27 12:00:09 chris Exp $
 */

#include "leds.h"

/*
 * delay -- delay execution. This is an ugly hack. It should
 *          use the timer, but I'm waiting for docs. (sigh)
 */
void delay(num)
int num;
{
  while (num--)
    {
      asm ("nop");
    }
}
