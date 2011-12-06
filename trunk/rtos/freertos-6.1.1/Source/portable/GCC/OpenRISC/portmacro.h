/*
    FreeRTOS V6.1.1 - Copyright (C) 2011 Real Time Engineers Ltd.

    ***************************************************************************
    *                                                                         *
    * If you are:                                                             *
    *                                                                         *
    *    + New to FreeRTOS,                                                   *
    *    + Wanting to learn FreeRTOS or multitasking in general quickly       *
    *    + Looking for basic training,                                        *
    *    + Wanting to improve your FreeRTOS skills and productivity           *
    *                                                                         *
    * then take a look at the FreeRTOS books - available as PDF or paperback  *
    *                                                                         *
    *        "Using the FreeRTOS Real Time Kernel - a Practical Guide"        *
    *                  http://www.FreeRTOS.org/Documentation                  *
    *                                                                         *
    * A pdf reference manual is also available.  Both are usually delivered   *
    * to your inbox within 20 minutes to two hours when purchased between 8am *
    * and 8pm GMT (although please allow up to 24 hours in case of            *
    * exceptional circumstances).  Thank you for your support!                *
    *                                                                         *
    ***************************************************************************

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation AND MODIFIED BY the FreeRTOS exception.
    ***NOTE*** The exception to the GPL is included to allow you to distribute
    a combined work that includes FreeRTOS without being obliged to provide the
    source code for proprietary components outside of the FreeRTOS kernel.
    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
    more details. You should have received a copy of the GNU General Public 
    License and the FreeRTOS license exception along with FreeRTOS; if not it 
    can be viewed here: http://www.freertos.org/a00114.html and also obtained 
    by writing to Richard Barry, contact details for whom are available on the
    FreeRTOS WEB site.

    1 tab == 4 spaces!

    http://www.FreeRTOS.org - Documentation, latest information, license and
    contact details.

    http://www.SafeRTOS.com - A version that is certified for use in safety
    critical systems.

    http://www.OpenRTOS.com - Commercial support, development, porting,
    licensing and training services.
*/
#ifndef PORTMACRO_H
#define PORTMACRO_H

#ifdef __cplusplus
extern "C" {
#endif

//-----------------------------------------------------------
// Port specific definitions
//-----------------------------------------------------------
// Type definitions.
#define portCHAR        char
#define portFLOAT       float
#define portDOUBLE      double
#define portLONG        long
#define portSHORT       short
#define portSTACK_TYPE  unsigned portLONG
#define portBASE_TYPE   long
#define portTickType    unsigned portLONG
#define portMAX_DELAY   (portTickType)0xffffffff

#if( configUSE_16_BIT_TICKS == 1 )
	#effor "configUSE_16_BIT_TICKS must be 0"
#endif

/*-----------------------------------------------------------*/	
#define portSTACK_GROWTH         		-1
#define portTICK_RATE_MS         		( (portTickType) 1000 / configTICK_RATE_HZ )
#define portBYTE_ALIGNMENT       		4
#define	portCRITICAL_NESTING_IN_TCB		1
#define portINSTRUCTION_SIZE			( ( portSTACK_TYPE ) 4 )
#define portNO_CRITICAL_SECTION_NESTING	( ( portSTACK_TYPE ) 0 )

#define portYIELD_FROM_ISR()			portYIELD()
#define portYIELD()		{	\
	__asm__ __volatile__ ( "l.nop       " ); \
	__asm__ __volatile__ ( "l.sys 0x0FCC" ); \
	__asm__ __volatile__ ( "l.nop       " ); \
} 
#define portNOP()		__asm__ __volatile__ ( "l.nop" )


void vPortDisableInterrupts( void );
void vPortEnableInterrupts( void );
#define portDISABLE_INTERRUPTS()	vPortDisableInterrupts()
#define portENABLE_INTERRUPTS()		vPortEnableInterrupts()

/*-----------------------------------------------------------*/	
// Critical section handling.
extern void vTaskEnterCritical( void );
extern void vTaskExitCritical( void );
#define portENTER_CRITICAL()		vTaskEnterCritical()
#define portEXIT_CRITICAL()			vTaskExitCritical()

/* Task function macros as described on the FreeRTOS.org WEB site. */
#define portTASK_FUNCTION_PROTO( vFunction, pvParameters ) void vFunction( void *pvParameters )
#define portTASK_FUNCTION( vFunction, pvParameters ) void vFunction( void *pvParameters )

#ifdef __cplusplus
}
#endif

// Macro to save all registers, stack pointer into the TCB.
#define portSAVE_CONTEXT()						\
	asm volatile (								\
	"	.global	pxCurrentTCB			\n\t"	\
	"	# make rooms in stack			\n\t"	\
	"	l.addi	r1, r1, -128			\n\t"	\
	"	# early save r3-r5, these are clobber register\n\t" \
	"	l.sw	0x08(r1), r3			\n\t"	\
	"	l.sw	0x0C(r1), r4			\n\t"	\
	"	l.sw	0x10(r1), r5			\n\t"	\
	"	# save SPR_ESR_BASE(0), SPR_EPCR_BASE(0)\n\t"	\
	"	l.mfspr	r3, r0, (0<<11) + 64	\n\t"	\
	"	l.mfspr	r4, r0, (0<<11) + 32	\n\t"	\
	"	l.sw	0x78(r1), r3			\n\t"	\
	"	l.sw	0x7C(r1), r4			\n\t"	\
	"	l.sw	0x00(r1), r9			\n\t"	\
	"	# disable interrupts			\n\t"	\
	"	l.jal vPortDisableInterrupts	\n\t"	\
	"	# Save Context					\n\t"	\
	"	l.sw	0x04(r1), r2			\n\t"	\
	"	l.sw	0x14(r1), r6			\n\t"	\
	"	l.sw	0x18(r1), r7			\n\t"	\
	"	l.sw	0x1C(r1), r8			\n\t"	\
	"	l.sw	0x20(r1), r10			\n\t"	\
	"	l.sw	0x24(r1), r11			\n\t"	\
	"	l.sw	0x28(r1), r12			\n\t"	\
	"	l.sw	0x2C(r1), r13			\n\t"	\
	"	l.sw	0x30(r1), r14			\n\t"	\
	"	l.sw	0x34(r1), r15			\n\t"	\
	"	l.sw	0x38(r1), r16			\n\t"	\
	"	l.sw	0x3C(r1), r17			\n\t"	\
	"	l.sw	0x40(r1), r18			\n\t"	\
	"	l.sw	0x44(r1), r19			\n\t"	\
	"	l.sw	0x48(r1), r20			\n\t"	\
	"	l.sw	0x4C(r1), r21			\n\t"	\
	"	l.sw	0x50(r1), r22			\n\t"	\
	"	l.sw	0x54(r1), r23			\n\t"	\
	"	l.sw	0x58(r1), r24			\n\t"	\
	"	l.sw	0x5C(r1), r25			\n\t"	\
	"	l.sw	0x60(r1), r26			\n\t"	\
	"	l.sw	0x64(r1), r27			\n\t"	\
	"	l.sw	0x68(r1), r28			\n\t"	\
	"	l.sw	0x6C(r1), r29			\n\t"	\
	"	l.sw	0x70(r1), r30			\n\t"	\
	"	l.sw	0x74(r1), r31			\n\t"	\
	"	# Save the top of stack in TCB	\n\t"	\
	"	l.movhi	r3, hi(pxCurrentTCB)	\n\t"	\
	"	l.ori	r3, r3, lo(pxCurrentTCB)\n\t"	\
	"	l.lwz	r3, 0x0(r3)				\n\t"	\
	"	l.sw	0x0(r3), r1				\n\t"	\
	"	# restore clobber register		\n\t"	\
	"	l.lwz	r3, 0x08(r1)			\n\t"	\
	"	l.lwz	r4, 0x0C(r1)			\n\t"	\
	"	l.lwz	r5, 0x10(r1)			\n\t"	\
	);

#define portRESTORE_CONTEXT()					\
	asm volatile (								\
	"	.global	pxCurrentTCB			\n\t"	\
	"	# restore stack pointer			\n\t"	\
	"	l.movhi	r3, hi(pxCurrentTCB)	\n\t"	\
	"	l.ori	r3, r3, lo(pxCurrentTCB)\n\t"	\
	"	l.lwz	r3, 0x0(r3)				\n\t"	\
	"	l.lwz	r1, 0x0(r3)				\n\t"	\
	"	# restore context				\n\t"	\
	"	l.lwz	r9, 0x00(r1)			\n\t"	\
	"	l.lwz	r2, 0x04(r1)			\n\t"	\
	"	l.lwz	r6, 0x14(r1)			\n\t"	\
	"	l.lwz	r7, 0x18(r1)			\n\t"	\
	"	l.lwz	r8, 0x1C(r1)			\n\t"	\
	"	l.lwz	r10, 0x20(r1)			\n\t"	\
	"	l.lwz	r11, 0x24(r1)			\n\t"	\
	"	l.lwz	r12, 0x28(r1)			\n\t"	\
	"	l.lwz	r13, 0x2C(r1)			\n\t"	\
	"	l.lwz	r14, 0x30(r1)			\n\t"	\
	"	l.lwz	r15, 0x34(r1)			\n\t"	\
	"	l.lwz	r16, 0x38(r1)			\n\t"	\
	"	l.lwz	r17, 0x3C(r1)			\n\t"	\
	"	l.lwz	r18, 0x40(r1)			\n\t"	\
	"	l.lwz	r19, 0x44(r1)			\n\t"	\
	"	l.lwz	r20, 0x48(r1)			\n\t"	\
	"	l.lwz	r21, 0x4C(r1)			\n\t"	\
	"	l.lwz	r22, 0x50(r1)			\n\t"	\
	"	l.lwz	r23, 0x54(r1)			\n\t"	\
	"	l.lwz	r24, 0x58(r1)			\n\t"	\
	"	l.lwz	r25, 0x5C(r1)			\n\t"	\
	"	l.lwz	r26, 0x60(r1)			\n\t"	\
	"	l.lwz	r27, 0x64(r1)			\n\t"	\
	"	l.lwz	r28, 0x68(r1)			\n\t"	\
	"	l.lwz	r29, 0x6C(r1)			\n\t"	\
	"	l.lwz	r30, 0x70(r1)			\n\t"	\
	"	l.lwz	r31, 0x74(r1)			\n\t"	\
	"	# restore SPR_ESR_BASE(0), SPR_EPCR_BASE(0)\n\t"	\
	"	l.lwz	r3, 0x78(r1)			\n\t"	\
	"	l.lwz	r4, 0x7C(r1)			\n\t"	\
	"	l.mtspr	r0, r3, (0<<11) + 64	\n\t"	\
	"	l.mtspr	r0, r4, (0<<11) + 32	\n\t"	\
	"	# restore clobber register		\n\t"	\
	"	l.lwz	r3, 0x08(r1)			\n\t"	\
	"	l.lwz	r4, 0x0C(r1)			\n\t"	\
	"	l.lwz	r5, 0x10(r1)			\n\t"	\
	"	l.addi	r1, r1, 128				\n\t"	\
	"	l.rfe							\n\t"	\
	"	l.nop							\n\t"	\
	);

#ifdef __cplusplus
}
#endif

#endif /* PORTMACRO_H */
