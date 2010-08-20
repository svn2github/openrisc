/* crt0.S. C Runtime startup file.

   Copyright (C) 2004, Jacob Bower
   Copyright (C) 2010, Embecosm Limited <info@embecosm.com>

   Contributor Jeremy Bennett <jeremy.bennett@embecosm.com>

   This file is part of Newlib.

   The original work by Jacob Bower is provided as-is without any kind of
   warranty. Use it at your own risk!

   All subsequent work is bound by version 3 of the GPL as follows.

   This program is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the Free
   Software Foundation; either version 3 of the License, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
   FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
   more details.

   You should have received a copy of the GNU General Public License along
   with this program.  If not, see <http://www.gnu.org/licenses/>.            */
/* -------------------------------------------------------------------------- */
/* This program is commented throughout in a fashion suitable for processing
   with Doxygen.                                                              */
/* -------------------------------------------------------------------------- */

#include "spr-defs.h"
#include "or1ksim-board.h"

	
/* -------------------------------------------------------------------------- */
/*!Macro for expressions which do not have their own handler.
  
   Construct the arguments for a call to printf, then exit. Being varargs, the
   arguments to printf must be on the stack, which we update using the
   standard prologue.
  
  @param[in] str  Label for a string describing the macro, suitable for
                  printf.                                                     */
/* -------------------------------------------------------------------------- */
#define UNHANDLED_EXCEPTION(str)					 \
	l.addi	r1,r1,-20		/* Standard prologue */		;\
	l.sw	16(r1),r2						;\
	l.addi	r2,r1,20						;\
	l.sw	12(r1),r9						;\
									;\
	l.movhi	r3,hi(.Lfmt)		/* printf format string */	;\
	l.ori	r3,r3,lo(.Lfmt)						;\
	l.sw	0(r1),r3						;\
	l.movhi	r4,hi(str)		/* Name of exception */		;\
	l.ori	r4,r4,lo(str)						;\
	l.sw	4(r1),r4						;\
	l.mfspr r5,r0,SPR_EPCR_BASE	/* Source of the interrupt */	;\
	l.jal	printf							;\
	l.sw	8(r1),r5						;\
									;\
	l.ori	r3,r0,0xffff		/* Failure RC */		;\
	l.jal	_exit							;\
	l.nop								;\
									;\
	l.rfe				/* Never executed we hope */


/* -------------------------------------------------------------------------- */
/*!Text strings for the different exceptions                                  */
/* -------------------------------------------------------------------------- */
	.section .rodata
.Lfmt:	.string	"Unhandled %s exception at address %08p\n"
.L200:	.string	"bus error"
.L300:	.string	"data page fault"
.L400:	.string	"instruction page fault"
.L500:	.string	"timer"
.L600:	.string	"alignment"
.L700:	.string	"illegal instruction"
.L800:	.string	"external interrupt"
.L900:	.string	"data TLB"
.La00:	.string	"instruction TLB"
.Lb00:	.string	"range"
.Lc00:	.string	"syscall"
.Ld00:	.string	"floating point"
.Le00:	.string	"trap"
.Lf00:	.string	"undefined 0xf00"
.L1000:	.string	"undefined 0x1000"
.L1100:	.string	"undefined 0x1100"
.L1200:	.string	"undefined 0x1200"
.L1300:	.string	"undefined 0x1300"
.L1400:	.string	"undefined 0x1400"
.L1500:	.string	"undefined 0x1500"
.L1600:	.string	"undefined 0x1600"
.L1700:	.string	"undefined 0x1700"
.L1800:	.string	"undefined 0x1800"
.L1900:	.string	"undefined 0x1900"
.L1a00:	.string	"undefined 0x1a00"
.L1b00:	.string	"undefined 0x1b00"
.L1c00:	.string	"undefined 0x1c00"
.L1d00:	.string	"undefined 0x1d00"
.L1e00:	.string	"undefined 0x1e00"
.L1f00:	.string	"undefined 0x1f00"

	
/* -------------------------------------------------------------------------- */
/*!Exception vectors                                                          */
/* -------------------------------------------------------------------------- */
	.section .vectors,"ax"

	/* 0x100: RESET exception */
        .org	0x100 	
_reset:
	/* Jump to program initialisation code */
	l.j	_start
	l.nop

	/* 0x200: BUS exception is special, because during startup we use it
	   to detect where the stack should go. So we need some special code
	   before we return, which wel will later overwrite with l.nop. We
	   need to deal with the case when _start is called multiple times, so
	   this code must be copied into the bus error vector each time. It is
	   position independent to allow this copying.

	   We use registers we know will not interfere in this case. */
	.org	0x200
_buserr:
	l.nop				/* Will be overwritten */
	l.nop
	l.nop
	l.nop

_buserr_std:
	UNHANDLED_EXCEPTION (.L200)

_buserr_special:	
	l.mfspr	r24,r0,SPR_EPCR_BASE
	l.addi	r24,r24,4		/* Return one instruction on */
	l.mtspr	r0,r24,SPR_EPCR_BASE
	l.rfe

	/* 0x300: Data Page Fault exception */
	.org	0x300
	UNHANDLED_EXCEPTION (.L300)
	
	/* 0x400: Insn Page Fault exception */
	.org	0x400
	UNHANDLED_EXCEPTION (.L400)

	/* 0x500: Timer exception */
	.org	0x500
	UNHANDLED_EXCEPTION (.L500)

	/* 0x600: Aligment exception */
	.org	0x600
	UNHANDLED_EXCEPTION (.L600)
	
	/* 0x700: Illegal insn exception */
	.org	0x700
	UNHANDLED_EXCEPTION (.L700)

	/* 0x800: External interrupt exception */
	.org	0x800
	UNHANDLED_EXCEPTION (.L800)

	/* 0x900: DTLB miss exception */
	.org	0x900
	UNHANDLED_EXCEPTION (.L900)
	
	/* 0xa00: ITLB miss exception */
	.org	0xa00
	UNHANDLED_EXCEPTION (.La00)
	
	/* 0xb00: Range exception */
	.org	0xb00
	UNHANDLED_EXCEPTION (.Lb00)
	
	/* 0xc00: Syscall exception */
	.org	0xc00
	UNHANDLED_EXCEPTION (.Lc00)
	
	/* 0xd00: floating point exception */
	.org	0xd00
	UNHANDLED_EXCEPTION (.Ld00)
	
	/* 0xe00: Trap exception */
	.org	0xe00
	UNHANDLED_EXCEPTION (.Le00)
	
	/* 0xf00: Reserved exceptions */
	.org	0xf00
	UNHANDLED_EXCEPTION (.Lf00)
	
	.org	0x1000
	UNHANDLED_EXCEPTION (.L1000)
	
	.org	0x1100
	UNHANDLED_EXCEPTION (.L1100)
	
	.org	0x1200
	UNHANDLED_EXCEPTION (.L1200)
	
	.org	0x1300
	UNHANDLED_EXCEPTION (.L1300)
	
	.org	0x1400
	UNHANDLED_EXCEPTION (.L1400)
	
	.org	0x1500
	UNHANDLED_EXCEPTION (.L1500)
	
	.org	0x1600
	UNHANDLED_EXCEPTION (.L1600)
	
	.org	0x1700
	UNHANDLED_EXCEPTION (.L1700)
	
	.org	0x1800
	UNHANDLED_EXCEPTION (.L1800)
	
	.org	0x1900
	UNHANDLED_EXCEPTION (.L1900)
	
	.org	0x1a00
	UNHANDLED_EXCEPTION (.L1a00)
	
	.org	0x1b00
	UNHANDLED_EXCEPTION (.L1b00)
	
	.org	0x1c00
	UNHANDLED_EXCEPTION (.L1c00)
	
	.org	0x1d00
	UNHANDLED_EXCEPTION (.L1d00)
	
	.org	0x1e00
	UNHANDLED_EXCEPTION (.L1e00)
	
	.org	0x1f00
	UNHANDLED_EXCEPTION (.L1f00)

	/* Pad to the end */
	.org	0x1ffc
	l.nop

/* -------------------------------------------------------------------------- */
/*!Main entry point
  
   We initialise the stack and frame pointer first, before we set up the
   caches, since otherwise we'll need to disable the instruction cache when
   patching the bus error vector code.

   The remaining tasks are then:
   - optionally set up instruction and/or data caches
   - clear BSS
   - call global and static constructors
   - set up destructors to be called from exit
   - initialize the UART (may be dummy, if no UART supported)
   - jump to the main function
   - call exit if the main function ever returns.
   - loop forever (should never get here)                                     */
/* -------------------------------------------------------------------------- */
	/* The stack grows down from the top of writable memory. */
	.section .data
	.global	stack
stack:	.space	4,0

	.section .text
	.global	_start
        .type	_start,@function

_start:
	/* Finding the end of stack means we need to handle the bus
	   error. Patch in some special handler code. */
	l.movhi	r30,hi(_buserr)		/* Where to copy to */
	l.ori	r30,r30,lo(_buserr)
	l.movhi	r28,hi(_buserr_std)	/* Where to stop copying */
	l.ori	r28,r28,lo(_buserr_std)
	l.movhi	r26,hi(_buserr_special)	/* Where to copy from */
	l.ori	r26,r26,lo(_buserr_special)

.L11:	l.sfeq	r28,r30
	l.bf	.L12
	l.nop

	l.lwz	r24,0(r26)		/* Get the instruction */
	l.sw	0(r30),r24		/* Patch the instruction */
	l.addi	r26,r26,4		/* Next source instruction */
	l.j	.L11
	l.addi	r30,r30,4		/* Delay slot: next dest location */
	
	/* Determine where the stack should end. Must be somewhere above the
	   end of loaded memory. We look in blocks of 64KB. */
.L12:	l.movhi r30,hi(end)
	l.ori   r30,r30,lo(end)
	l.srli	r30,r30,16		/* Round down to 64KB boundary */
	l.slli	r30,r30,16

	l.addi	r28,r0,1		/* Constant 64KB in register */
	l.slli	r28,r28,16

	l.add	r30,r30,r28
	l.addi	r30,r30,-4		/* SP one word inside next 64KB? */

	l.movhi	r26,0xaaaa		/* Test pattern to store in memory */
	l.ori	r26,r26,0xaaaa

	/* Is this a writeable location? */
.L3:	l.sw	0(r30),r26
	l.lwz	r24,0(r30)
	l.sfeq	r24,r26
	l.bnf	.L4
	l.nop
	
	l.j	.L3
	l.add	r30,r30,r28		/* Try 64KB higher */

.L4:	l.sub	r30,r30,r28		/* Previous value was wanted */
	l.movhi	r26,hi(stack)
	l.ori	r26,r26,lo(stack)
	l.sw	0(r26),r30

	/* Initialise stack and frame pointer (set to same value) */
	l.add	r1,r30,r0
	l.add	r2,r30,r0

	/* Clear out the bus error vector special code. */
	l.movhi	r30,hi(_buserr)
	l.ori	r30,r30,lo(_buserr)
	l.movhi	r28,hi(_buserr_std)
	l.ori	r28,r28,lo(_buserr_std)
	l.movhi	r26,0x1500		/* l.nop 0 */
	l.ori	r26,r26,0x0000

.L5:	l.sfeq	r28,r30
	l.bf	.L6
	l.nop

	l.sw	0(r30),r26		/* Patch the instruction */
	l.j	.L5
	l.addi	r30,r30,4		/* Delay slot: next instruction */

	/* Instruction cache enable */
	/* Check if IC present and skip enabling otherwise */
.L6:	l.mfspr r24,r0,SPR_UPR
	l.andi  r26,r24,SPR_UPR_ICP
	l.sfeq  r26,r0
	l.bf    .L8
	l.nop
	
	/* Disable IC */
	l.mfspr r6,r0,SPR_SR
	l.addi  r5,r0,-1
	l.xori  r5,r5,SPR_SR_ICE
	l.and   r5,r6,r5
	l.mtspr r0,r5,SPR_SR
	
	/* Establish cache block size
	If BS=0, 16;
	If BS=1, 32;
	r14 contain block size
	*/
	l.mfspr r24,r0,SPR_ICCFGR
	l.andi  r26,r24,SPR_ICCFGR_CBS
	l.srli  r28,r26,7
	l.ori   r30,r0,16
	l.sll   r14,r30,r28
	
	/* Establish number of cache sets
	r16 contains number of cache sets
	r28 contains log(# of cache sets)
	*/
	l.andi  r26,r24,SPR_ICCFGR_NCS
	l.srli  r28,r26,3
	l.ori   r30,r0,1
	l.sll   r16,r30,r28
	
	/* Invalidate IC */
	l.addi  r6,r0,0
	l.sll   r5,r14,r28
	
.L7:	l.mtspr r0,r6,SPR_ICBIR
	l.sfne  r6,r5
	l.bf    .L7
	l.add   r6,r6,r14
	
	/* Enable IC */
	l.mfspr r6,r0,SPR_SR
	l.ori   r6,r6,SPR_SR_ICE
	l.mtspr r0,r6,SPR_SR
	l.nop
	l.nop
	l.nop
	l.nop
	l.nop
	l.nop
	l.nop
	l.nop

	/* Data cache enable */
        /* Check if DC present and skip enabling otherwise */
.L8:    l.mfspr r24,r0,SPR_UPR
        l.andi  r26,r24,SPR_UPR_DCP
        l.sfeq  r26,r0
        l.bf    .L10
        l.nop
        /* Disable DC */
        l.mfspr r6,r0,SPR_SR
        l.addi  r5,r0,-1
        l.xori  r5,r5,SPR_SR_DCE
	l.and   r5,r6,r5
        l.mtspr r0,r5,SPR_SR
        /* Establish cache block size
           If BS=0, 16;
           If BS=1, 32;
           r14 contain block size
        */
        l.mfspr r24,r0,SPR_DCCFGR
        l.andi  r26,r24,SPR_DCCFGR_CBS
        l.srli  r28,r26,7
        l.ori   r30,r0,16
        l.sll   r14,r30,r28
        /* Establish number of cache sets
           r16 contains number of cache sets
           r28 contains log(# of cache sets)
        */
	l.andi  r26,r24,SPR_DCCFGR_NCS
	l.srli  r28,r26,3
        l.ori   r30,r0,1
        l.sll   r16,r30,r28
        /* Invalidate DC */
        l.addi  r6,r0,0
        l.sll   r5,r14,r28

.L9:    l.mtspr r0,r6,SPR_DCBIR
        l.sfne  r6,r5
        l.bf    .L9
	l.add   r6,r6,r14
        /* Enable DC */
        l.mfspr r6,r0,SPR_SR
        l.ori   r6,r6,SPR_SR_DCE
        l.mtspr r0,r6,SPR_SR

	/* Clear BSS */
.L10:	l.movhi r28,hi(__bss_start)
	l.ori   r28,r28,lo(__bss_start)
	l.movhi r30,hi(end)
	l.ori   r30,r30,lo(end)

.L1:    l.sw    (0)(r28),r0
        l.sfltu r28,r30
        l.bf    .L1
        l.addi  r28,r28,4		/* Delay slot */

	/* Call global and static constructors */
	l.jal	init
	l.nop
	
	/* Set up destructors to be called from exit if main never returns */
	l.movhi	r3,hi(fini)
	l.jal	atexit
	l.ori	r3,r3,lo(fini)		/* Delay slot */

	/* Initialise UART in a C function. If the UART isn't present, we'll */
	/* link against a dummy function. */
	l.jal    _uart_init
	l.nop

	/* Jump to main program entry point (argc = argv = envp = 0) */
	l.or	r3,r0,r0
	l.or	r4,r0,r0
	l.jal   main
	l.or	r5,r0,r0		/* Delay slot */

	/* If program exits, call exit routine */
	l.jal   _exit
	l.addi  r3,r11,0		/* Delay slot */

	/* Loop forever */
.L2:	l.j	.L2
	l.nop
	
	.size	_start, .-_start
