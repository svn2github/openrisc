# file rtems.s
#	
#  This file contains the single entry point code for
#  the or1k port of RTEMS.
#
#  Author: Chris Ziomkowski <chris@asics.ws>
#
#  This file is provided "AS IS" without any warranty expressed
#  or implied.
#
#  This file is distributed as part of the RTEMS package. It
#  is governed by the RTEMS copyright and license agreement as
#  stated below:
#
#  COPYRIGHT (c) 1989-1997.
#  On-Line Applications Research Corporation (OAR).
#  Copyright assigned to U.S. Government, 1994.
#
#  The license and distribution terms for this file may in
#  the file LICENSE in this distribution or at
#  http://www.OARcorp.com/rtems/license.html.
#
#
#  This file is designed to work only with a COFF version of the gnu
#  assembler and includes necessary debugging information. Other
#  assemblers will probably fail.
#	
.file	"rtems.s"

.text
	.align	4
.proc	_RTEMS
#	.def	_RTEMS
#	.val	_RTEMS
#	.scl	2
#	.type	041
#	.endef
	.global	_RTEMS
_RTEMS:	
#	.def	.bf
#	.val	.
#	.scl	101
#	.endef

#
# We assume the index to the function to call is stored
# in r12. Simply offset from this and transfer control to
# to the appropriate location. By calculating the offset
# first, loading the high word and then adding the low
# word, we can conserve a register.
#
	l.slli		r12,r12,2
	l.movhi		r12,hi(__Entry_points)
	l.addi		r12,r12,lo(__Entry_points)
	l.lwz		r12,0(r12)
	l.jr		r12
	l.nop
	
#        .def    .ef
#        .val    .
#        .scl    101
#	.endef
	
.endproc _RTEMS
#	.def	_RTEMS
#	.val	.
#	.scl	-1
#	.endef

