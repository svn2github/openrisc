/*
----------------------------------------------------------------------
    CHiPES Embedded RTR	Systems Copyright (c) Tim Oliver 2002-2004
----------------------------------------------------------------------
 File   : os_cpu_a.S
 Author(s) : 	Tim Oliver, timtimoliver@yahoo.co.uk
		Julius Baxter, julius@opencores.org
---------------------------[Description]------------------------------
	Start up file for OpenRISC Reference Platform
	Assembly code required for ORP port of MicroC/OS-II 
	
	Macros :
	 exception_vector name org
		load32i reg const
		store_context
		restore_context
	
	Internal Routines :
		_reset		Boot code installed at interrupt vector 0x100
		_start      	Start up sequence

		_OSTickISR  	Operating system timer tick interrupt service
	                        routine
		_OSStartHighRdy	Starts the highest priority task that is
	                        available to run
		_OSCtxSwBP
		_OSCtxSw	Task switch
		_OSIntCtxSw 	Task switch after interrupt
		_UserISR    	Interrupt service routine template - requires
	                        code
		_align		Attempts to recover from memory alignment
	                        errors
		nest_not_one
	
	External Routines	:
		_OSIntExit      
		_OSTimeTick
		_OSTaskSwHook
 
	External variables	:
		_OSIntNesting   
		_OSRunning
		_OSPrioHighRdy
		_OSPrioCur
		_OSTCBCur
		_OSTCBHighRdy
	
	Interrupt Vectors Installed:
		0x100 	_reset
		0x500	_OSTickISR
		0x600 	_align

		0x800 	_UserISR

		0xc00 	_OSCtxSw
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

#define RED_ZONE_SIZE 128
#define STK_FRAME_SIZE (128+RED_ZONE_SIZE)
#define SPR_TTMR_LOAD  (SPR_TTMR_IE | SPR_TTMR_RT | \
	((IN_CLK/TICKS_PER_SEC) & SPR_TTMR_PERIOD))

/* function prototypes */

        .global  OSTickISR      /* internal functions */
        .global  OSStartHighRdy
        .global  OSCtxSwBP
        .global  OSCtxSw
        .global  OSIntCtxSw
        .global  UserISR
        .global  _align
        .global  nest_not_one

        .extern  OSIntExit      /* external functions */
        .extern  OSTimeTick
        .extern  OSTaskSwHook

        .extern  OSIntNesting   /* values */
        .extern  OSRunning
        .extern  OSPrioHighRdy
        .extern  OSPrioCur
        .extern  OSTCBCur
        .extern  OSTCBHighRdy

/* Macro Definitions */
/* Utility macro: Load a 32-bit constant into a register */
        .macro  load32i reg const
        l.movhi \reg,hi(\const)
        l.ori   \reg,\reg,lo(\const)
        .endm

/* Utility macro: Start code for an exception handler */
        .macro exception_vector name
        l.addi  r1,r1,-STK_FRAME_SIZE
        l.sw    0xc(r1),r3
        l.movhi r3,hi(\name)
        l.ori   r3,r3,lo(\name)
        l.jr    r3
        l.nop

        .endm

	.macro unhandled_exception
	l.ori r3, r0, 1
	l.j 0
	l.nop NOP_EXIT

	.endm
	
/* Utility macro: store the cpu context on the stack */
        .macro store_context
        l.sw    0x8(r1),r2
        l.sw    0x10(r1),r4
        l.sw    0x14(r1),r5
        l.sw    0x18(r1),r6
        l.sw    0x1c(r1),r7
        l.sw    0x20(r1),r8
        l.sw    0x24(r1),r9
        l.sw    0x28(r1),r10
        l.sw    0x2c(r1),r11
        l.sw    0x30(r1),r12
        l.sw    0x34(r1),r13
        l.sw    0x38(r1),r14
        l.sw    0x3c(r1),r15
        l.sw    0x40(r1),r16
        l.sw    0x44(r1),r17
        l.sw    0x48(r1),r18
        l.sw    0x4c(r1),r19
        l.sw    0x50(r1),r20
        l.sw    0x54(r1),r21
        l.sw    0x58(r1),r22
        l.sw    0x5c(r1),r23
        l.sw    0x60(r1),r24
        l.sw    0x64(r1),r25
        l.sw    0x68(r1),r26
        l.sw    0x6c(r1),r27
        l.sw    0x70(r1),r28
        l.sw    0x74(r1),r29
        l.sw    0x78(r1),r30
        l.sw    0x7c(r1),r31
        .endm

/* Utility macro: restore the cpu context from the stack */
        .macro restore_context
        l.lwz   r2,0x8(r1)
        l.lwz   r3,0xc(r1)
        l.lwz   r4,0x10(r1)
        l.lwz   r5,0x14(r1)
        l.lwz   r6,0x18(r1)
        l.lwz   r7,0x1c(r1)
        l.lwz   r8,0x20(r1)
        l.lwz   r9,0x24(r1)
        l.lwz   r10,0x28(r1)
        l.lwz   r11,0x2c(r1)
        l.lwz   r12,0x30(r1)
        l.lwz   r13,0x34(r1)
        l.lwz   r14,0x38(r1)
        l.lwz   r15,0x3c(r1)
        l.lwz   r16,0x40(r1)
        l.lwz   r17,0x44(r1)
        l.lwz   r18,0x48(r1)
        l.lwz   r19,0x4c(r1)
        l.lwz   r20,0x50(r1)
        l.lwz   r21,0x54(r1)
        l.lwz   r22,0x58(r1)
        l.lwz   r23,0x5c(r1)
        l.lwz   r24,0x60(r1)
        l.lwz   r25,0x64(r1)
        l.lwz   r26,0x68(r1)
        l.lwz   r27,0x6c(r1)
        l.lwz   r28,0x70(r1)
        l.lwz   r29,0x74(r1)
        l.lwz   r30,0x78(r1)
        l.lwz   r31,0x7c(r1)
        .endm

/* actual code */
        
        .section .stack, "aw", @nobits

.space  STACK_SIZE
_stack:

/* Exception vectors */	

        .section .vectors, "ax"

	.org 0x100
_reset:
	l.movhi r0,0
        l.addi  r3,r0,SPR_SR_SM
        l.mtspr r0,r3,SPR_SR
        load32i r3,_start
        l.jr    r3
        l.nop

	.org 0x200
	unhandled_exception

	.org 0x300
	unhandled_exception

	.org 0x400
	unhandled_exception
	
	.org 0x500
        exception_vector OSTickISR
	
	.org 0x600
        exception_vector _align

	.org 0x700
	unhandled_exception
	
	.org 0x800
        exception_vector UserISR

	.org 0x900
	unhandled_exception

	.org 0xa00
	unhandled_exception

	.org 0xb00
	unhandled_exception

	.org 0xc00
        exception_vector OSCtxSw


	/* Start of text section */
	
        .section .text

	/* _start function - called immediately after reset */
_start:
	
	/* Instruction cache enable */
	/* Check if IC present and skip enabling otherwise */
	l.mfspr r24,r0,SPR_UPR
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
	
.L7:
	l.mtspr r0,r6,SPR_ICBIR
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

.L8:
	/* Data cache enable */
        /* Check if DC present and skip enabling otherwise */
        l.mfspr r24,r0,SPR_UPR
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
.L9:
        l.mtspr r0,r6,SPR_DCBIR
        l.sfne  r6,r5
        l.bf    .L9
	l.add   r6,r6,r14
        /* Enable DC */
        l.mfspr r6,r0,SPR_SR
        l.ori   r6,r6,SPR_SR_DCE
        l.mtspr r0,r6,SPR_SR

.L10:

	/* Clear BSS */
        load32i	r28, ___bss_start
        load32i r30, __end
1:
        l.sw    (0)(r28), r0
        l.sfltu r28, r30
        l.bf    1b
        l.addi  r28, r28, 4

	/* Initialise stack pointer */
        load32i r1,_stack-4

        l.addi  r2,r0,-3
        l.and   r1,r1,r2
	l.ori	r2,r1,0
	
        load32i r3,main

        l.jr    r3
        l.addi  r3,r0,0


	
_align:
	l.ori	r3,r0,0x600
	l.nop	NOP_REPORT
	l.mfspr	r3,r0,SPR_EPCR_BASE
	l.nop	NOP_REPORT
	l.lwz	r3,0(r3)
	l.nop	NOP_REPORT	
	l.mfspr	r3,r0,SPR_EEAR_BASE
	l.nop	NOP_REPORT
	/* Loop in place, cause simulator to exit */
	l.ori	r3,r0,1
	l.j	0
	l.nop 	NOP_EXIT

/*
------------------------------
            OSCtxSw
------------------------------
 Description :
 This routine switches between two different tasks.
 The task state of one is saved on its kernel stack.
 Then the state of the other is restored from its kernel stack.

 There maybe memory management hardware issues

 Finally, we can return to the second task, via the 'return'.

 Includes OSIntCtxSw
------------------------------
 Uses:
------------------------------
*/

OSCtxSw:
	/* l.sys exception for now so we are in supervisor mode */
	/* exception  - recover pc from epcr */

        l.mfspr r3,r0,SPR_EPCR_BASE	/* save program counter that was put in
						exception register */
        l.sw    0(r1),r3
        l.mfspr r3,r0,SPR_ESR_BASE	/* save status register that was put in
						exception register */
        l.sw    4(r1),r3

        store_context
                                        /* Store current stack pointer   */
        load32i r3,OSTCBCur            /* r3= &OSTCBCur */

        l.lwz   r3,0(r3)                /* r3 = &CurrentTask.OSTCBStkPtr  */
        l.sw    0(r3),r1                /* CurrentTask.OSTCBStkPtr = SP */

OSIntCtxSw:
        l.jal   OSTaskSwHook           /* call OSTaskSwHook */
        l.nop

        load32i r2,OSTCBHighRdy        /* r2= &OSTCBHighRdy */
        l.lwz   r2,0(r2)
        load32i r3,OSTCBCur            /* r3= &OSTCBCur */
        l.sw    0(r3),r2                /* OSTCBCur = OSTCBHighRdy */

        load32i r3,OSPrioHighRdy       /* r3= &OSPrioHighRdy */
        l.lbz   r3, 0(r3)
        load32i r4,OSPrioCur           /* r4= &OSPrioCur */
        l.sb    0(r4), r3               /* OSPrioCur = OSPrioHighRdy */

        l.lwz   r1, 0(r2)               /* sp = OSTCBHighRdy */


        l.lwz   r2,0(r1)                /* load context for task to be resumed */
        l.mtspr r0,r2,SPR_EPCR_BASE
        l.lwz   r2,4(r1)
        l.mtspr r0,r2,SPR_ESR_BASE

        restore_context
	
OSCtxSwBP:
        l.addi  r1,r1,STK_FRAME_SIZE
        l.rfe
        l.nop

/*
------------------------------
        OSStartHighRdy
------------------------------
 Description :
   Starts the highest priority task that is available to run

 OSStartHighRdy() MUST:
  a) Call OSTaskSwHook() then,
  b) Set OSRunning to TRUE,
  c) Switch to the HPT

------------------------------
 Uses :
------------------------------
*/
OSStartHighRdy:

        l.jal   OSTaskSwHook            /* call OSTaskSwHook */
        l.nop

        load32i r3,OSRunning            /* r3= &OSRunning */

        l.ori   r4,r0, 0x01             /* set OSRunning == TRUE */
        l.sb    0(r3), r4
                                        /* load stack pointer from next task's
	                                   TCB area  */
        load32i r3,OSTCBHighRdy         /* r3 = &OSTCBHighRdy */
        l.lwz   r3,0(r3)                /* r3 = &OS_TCB */
        l.lwz   r1, 0(r3)               /* stack is the first element */

        l.lwz   r2,0(r1)
        l.mtspr r0,r2,SPR_EPCR_BASE
        l.lwz   r2,4(r1)
        l.mtspr r0,r2,SPR_ESR_BASE

        restore_context

        l.addi  r1,r1,STK_FRAME_SIZE
        l.rfe
        l.nop

/*
------------------------------
        OSTickISR
------------------------------
 Description :

------------------------------
 Uses :
------------------------------
*/
OSTickISR:
        l.mfspr r3,r0,SPR_EPCR_BASE	/* save program counter that was put
	                                   in exception register */
        l.sw    0(r1),r3
        l.mfspr r3,r0,SPR_ESR_BASE	/* save status register that was put in
	                                   exception register */
        l.sw    4(r1),r3

        store_context
/* either call OSIntEnter or Increment OSIntNesting */

/*
        l.jal   OSIntEnter
        l.nop
        load32i r2,OSIntNesting
        l.lbz   r3,0(r2)
*/
        load32i r2,OSIntNesting     /* r2 &OSIntNesting */
        l.lbz   r3,0(r2)             /* r3 OSIntNesting  */
        l.addi  r3,r3,1
        l.sb    0(r2),r3

   /* if (OSIntNesting == 1) OSTCBCur->OSTCBStkPtr = sp */
        l.sfeqi r3,1
        l.bnf   nest_not_one
        l.nop

        load32i r4,OSTCBCur            /* set pointer to Current TCB pointer */
                                        /* r4= &OSTCBCur */

                                        /* Store current stack pointer   */
        l.lwz   r5,0(r4)                /* r5 = &CurrentTask.OSTCBStkPtr  */
        l.sw    0(r5),r1                /* CurrentTask.OSTCBStkPtr = SP */

nest_not_one:
        /* clear interrupt */
        load32i r3,SPR_TTMR_LOAD
        l.mtspr r0,r3,SPR_TTMR

        /* optionally re enable interrupt */

        /*Call OSTimeTick()*/
        l.jal   OSTimeTick
        l.nop
        /*Call OSIntExit()*/
        l.jal   OSIntExit
        l.nop
                                        /* load stack pointer from next task's
	                                   TCB area  */
        load32i r3,OSTCBHighRdy         /* r3 = &OSTCBHighRdy */
        l.lwz   r3,0(r3)                /* r3 = &OS_TCB */
        l.lwz   r1, 0(r3)               /* stack is the first element */

        l.lwz   r2,0(r1)
        l.mtspr r0,r2,SPR_EPCR_BASE
        l.lwz   r2,4(r1)
        l.mtspr r0,r2,SPR_ESR_BASE

        restore_context

        l.addi  r1,r1,STK_FRAME_SIZE
        l.rfe
        l.nop


        /*
------------------------------
        UserISR
------------------------------
 Description :

------------------------------
 Uses :
------------------------------
*/
UserISR:
        l.mfspr r3,r0,SPR_EPCR_BASE	/* save program counter that was put in
						exception register */
        l.sw    0(r1),r3
        l.mfspr r3,r0,SPR_ESR_BASE	/* save status register that was put
						in exception register */
        l.sw    4(r1),r3

        store_context
/* either call OSIntEnter or Increment OSIntNesting */

/*
        l.jal   _OSIntEnter
        l.nop
        load32i r2,_OSIntNesting
        l.lbz   r3,0(r2)
*/
        load32i r2,OSIntNesting     /* r2 &OSIntNesting */

        l.lbz   r3,0(r2)             /* r3 OSIntNesting  */
        l.addi  r3,r3,1
        l.sb    0(r2),r3

   /* if (OSIntNesting == 1) OSTCBCur->OSTCBStkPtr = sp */
        l.sfeqi r3,1
        l.bnf   Unest_not_one
        l.nop

        load32i r4,OSTCBCur            /* set pointer to Current TCB pointer */
                                        /* r4= &OSTCBCur */

                                        /* Store current stack pointer   */
        l.lwz   r5,0(r4)                /* r5 = &CurrentTask.OSTCBStkPtr  */
        l.sw    0(r5),r1                /* CurrentTask.OSTCBStkPtr = SP */

Unest_not_one:
        /* clear interrupt */
        l.mtspr r0,r0,SPR_PICSR

        /* optionally re enable interrupt */

        /*Call interrupt service routine */

        /*Call OSIntExit()*/
        l.jal   OSIntExit
        l.nop

                                        /* load stack pointer from next task's TCB area  */
        load32i r3,OSTCBHighRdy        /* r3 = &OSTCBHighRdy */
        l.lwz   r3,0(r3)                /* r3 = &OS_TCB */
        l.lwz   r1, 0(r3)               /* stack is the first element */

        l.lwz   r2,0(r1)
        l.mtspr r0,r2,SPR_EPCR_BASE
        l.lwz   r2,4(r1)
        l.mtspr r0,r2,SPR_ESR_BASE

        restore_context

        l.addi  r1,r1,STK_FRAME_SIZE
        l.rfe
        l.nop
