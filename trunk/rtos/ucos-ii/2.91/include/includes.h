/*
----------------------------------------------------------------------
    CHiPES Embedded RTR	Systems Copyright (c) Tim Oliver 2002-2004
----------------------------------------------------------------------
 File   : includes.h
 Author : Tim Oliver
 Email  : timtimoliver@yahoo.co.uk
---------------------------[Description]------------------------------

		Top level include file for MicroC/OS-II
		on the OpenRISC Reference Platfrom
*/

/*
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include "spr-defs.h"
#include "board.h"
#include "uart.h"

#include "console.h"

#include "cprintf_r.h"

#include <stdarg.h>
#include <stddef.h>
#include <limits.h>

#ifdef FLOATING_POINT
#include <locale.h>
#include <math.h>
#include "floatio.h"
#endif /* FLOATING_POINT */

#include "ctype.h"
#include "string.h"

#include "os_cpu.h"
#include "os_cfg.h"
#include "ucos_ii.h"
