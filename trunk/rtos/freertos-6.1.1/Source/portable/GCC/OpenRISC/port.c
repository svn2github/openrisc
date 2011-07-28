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

/*-----------------------------------------------------------
 * Implementation of functions defined in portable.h for the OpenRISC port.
 *----------------------------------------------------------*/


/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"

/* Processor constants. */
#include "port_spr_defs.h"

/* Tick Timer Interrupt handler */
void vTickHandler( void );	

/* Setup the timer to generate the tick interrupts. */
static void prvSetupTimerInterrupt( void );

/* For writing into SPR. */
static inline void mtspr(unsigned long spr, unsigned long value) {	
	asm("l.mtspr\t\t%0,%1,0": : "r" (spr), "r" (value));
}

/* For reading SPR. */
static inline unsigned long mfspr(unsigned long spr) {	
	unsigned long value;
	asm("l.mfspr\t\t%0,%1,0" : "=r" (value) : "r" (spr));
	return value;
}


/* 
 * naked attribute is ignored or32-elf-gcc 4.5.1-or32-1.0rc1 
 * use assemble routines in portasm.S
 */
#if 0
void vPortDisableInterrupts( void ) __attribute__ ((__naked__)) 
{
	asm volatile (								\
	"	@ get current SR				\n\t"	\
	"	l.mfspr	r3, r0, SPR_SR			\n\t"	\
	"	l.addi	r4, r0, SPR_SR_TEE		\n\t"	\
	"	l.xori	r4, r4, 0xffffffff		\n\t"	\
	"	l.and	r3, r3, r4				\n\t"	\
	"	l.addi	r4, r0, SPR_SR_IEE		\n\t"	\
	"	l.xori	r4, r4, 0xffffffff		\n\t"	\
	"	l.and	r3, r3, r4				\n\t"	\
	"	@ update SR						\n\t"	\
	"	l.mtspr	r0, r3, SPR_SR			\n\t"	\
	);
}

void vPortEnableInterrupts( void ) __attribute__ ((__naked__))
{
	asm volatile (								\
	"	@ get current SR				\n\t"	\
	"	l.mfspr	r3, r0, SPR_SR			\n\t"	\
	"	@ enable Tick Timer Interrupt	\n\t"	\
	"	l.ori	r3, r3, SPR_SR_TEE		\n\t"	\
	"	@ enable External Interrupt		\n\t"	\
	"	l.ori	r3, r3, SPR_SR_IEE		\n\t"	\
	"	@ update SR						\n\t"	\
	"	l.mtspr	r0, r3, SPR_SR			\n\t"	\
	);
}
#endif


/* 
 * Initialise the stack of a task to look exactly as if a call to 
 * portSAVE_CONTEXT had been called.
 *
 * See header file for description. 
 */
portSTACK_TYPE *pxPortInitialiseStack( portSTACK_TYPE *pxTopOfStack, pdTASK_CODE pxCode, void *pvParameters )
{
	unsigned portLONG uTaskSR = mfspr(SPR_ESR_BASE);			
	uTaskSR &= ~SPR_SR_SM;					// User mode
	uTaskSR |= (SPR_SR_TEE | SPR_SR_IEE);	// Tick interrupt enable, All External interupt enable

	/* Setup the initial stack of the task.  The stack is set exactly as 
	expected by the portRESTORE_CONTEXT() macro. */
	*(--pxTopOfStack) = (portSTACK_TYPE)pxCode;			// SPR_EPCR_BASE(0)
	*(--pxTopOfStack) = (portSTACK_TYPE)uTaskSR;		// SPR_ESR_BASE(0) 

	*(--pxTopOfStack) = (portSTACK_TYPE)0x00000031;		// r31
	*(--pxTopOfStack) = (portSTACK_TYPE)0x00000030;		// r30
	*(--pxTopOfStack) = (portSTACK_TYPE)0x00000029;		// r29
	*(--pxTopOfStack) = (portSTACK_TYPE)0x00000028;		// r28
	*(--pxTopOfStack) = (portSTACK_TYPE)0x00000027;		// r27
	*(--pxTopOfStack) = (portSTACK_TYPE)0x00000026;		// r26
	*(--pxTopOfStack) = (portSTACK_TYPE)0x00000025;		// r25
	*(--pxTopOfStack) = (portSTACK_TYPE)0x00000024;		// r24
	*(--pxTopOfStack) = (portSTACK_TYPE)0x00000023;		// r23
	*(--pxTopOfStack) = (portSTACK_TYPE)0x00000022;		// r22
	*(--pxTopOfStack) = (portSTACK_TYPE)0x00000021;		// r21
	*(--pxTopOfStack) = (portSTACK_TYPE)0x00000020;		// r20
	*(--pxTopOfStack) = (portSTACK_TYPE)0x00000019;		// r19
	*(--pxTopOfStack) = (portSTACK_TYPE)0x00000018;		// r18
	*(--pxTopOfStack) = (portSTACK_TYPE)0x00000017;		// r17
	*(--pxTopOfStack) = (portSTACK_TYPE)0x00000016;		// r16
	*(--pxTopOfStack) = (portSTACK_TYPE)0x00000015;		// r15
	*(--pxTopOfStack) = (portSTACK_TYPE)0x00000014;		// r14
	*(--pxTopOfStack) = (portSTACK_TYPE)0x00000013;		// r13
	*(--pxTopOfStack) = (portSTACK_TYPE)0x00000012;		// r12
	*(--pxTopOfStack) = (portSTACK_TYPE)0x00000011;		// r11
	*(--pxTopOfStack) = (portSTACK_TYPE)0x00000010;		// r10
	*(--pxTopOfStack) = (portSTACK_TYPE)0x00000008;		// r8
	*(--pxTopOfStack) = (portSTACK_TYPE)0x00000007;		// r7
	*(--pxTopOfStack) = (portSTACK_TYPE)0x00000006;		// r6
	*(--pxTopOfStack) = (portSTACK_TYPE)0x00000005;		// r5
	*(--pxTopOfStack) = (portSTACK_TYPE)0x00000004;		// r4
	*(--pxTopOfStack) = (portSTACK_TYPE)pvParameters;	// task argument
	*(--pxTopOfStack) = (portSTACK_TYPE)0x00000002;		// r2
	*(--pxTopOfStack) = (portSTACK_TYPE)pxCode;			// PC

	return pxTopOfStack;
}

portBASE_TYPE xPortStartScheduler( void )
{
	/* Start the timer that generates the tick ISR.  Interrupts are disabled
	here already. */
	prvSetupTimerInterrupt();

	/* Start the first task. */
	portRESTORE_CONTEXT();	
	
	/* Should not get here! */
	return 0;
}

void vPortEndScheduler( void )
{
	mtspr(SPR_SR, mfspr(SPR_SR) & (~SPR_SR_TEE));	// Tick stop
}

/*
 * Setup the tick timer to generate the tick interrupts at the required frequency.
 */
static void prvSetupTimerInterrupt( void )
{
	const unsigned portLONG ulTickPeriod = configCPU_CLOCK_HZ / configTICK_RATE_HZ;

	// Disable tick timer exception recognition 
	mtspr(SPR_SR, mfspr(SPR_SR) & ~SPR_SR_TEE);

	// clears interrupt
   	mtspr(SPR_TTMR, mfspr(SPR_TTMR) & ~(SPR_TTMR_IP)); 

	// Set period of one cycle, restartable mode 
	mtspr(SPR_TTMR, SPR_TTMR_IE | SPR_TTMR_RT | (ulTickPeriod & SPR_TTMR_PERIOD));

	// Reset counter 
	mtspr(SPR_TTCR, 0);

    // set OR1200 to accept exceptions
    mtspr(SPR_SR, mfspr(SPR_SR) | SPR_SR_TEE);
}

#if 0
void vTickHandler( void )
{
	// clears interrupt
	mtspr(SPR_TTMR, mfspr(SPR_TTMR) & ~(SPR_TTMR_IP));    

	/* Increment the RTOS tick count, then look for the highest priority 
	   task that is ready to run. */
	vTaskIncrementTick();

	// The cooperative scheduler requires a normal simple Tick ISR to
	// simply increment the system tick.
#if configUSE_PREEMPTION == 0
	// nothing to do here
#else
	/* Save the context of the current task. */
	portSAVE_CONTEXT();			

	/* Find the highest priority task that is ready to run. */
	vTaskSwitchContext();

	portRESTORE_CONTEXT();
#endif
}
#endif
