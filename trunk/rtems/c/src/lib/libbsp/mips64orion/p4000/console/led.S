/*
 *  $Id: led.S,v 1.2 2001-09-27 12:00:24 chris Exp $
 */

#include <iregdef.h>
#include <idtmon.h>
#include <idtcpu.h>


FRAME(mips_leddisplay,sp,0,ra)
	j ra
	nop
ENDFRAME(mips_leddisplay)

FRAME(sbddisplay,sp,0,ra)
	j	ra
ENDFRAME(sbddisplay)


FRAME(sbdblank,sp,0,ra)
	j	ra
ENDFRAME(sbdblank)

