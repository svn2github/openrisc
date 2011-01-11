/*
  ----------------------------------------------------------------------
  CHiPES Embedded RTR	Systems Copyright (c) Tim Oliver 2002-2004
  ----------------------------------------------------------------------
  File      : main.c
  Author(s) : Tim Oliver, timtimoliver@yahoo.co.uk
              Julius Baxter, julius@opencores.org

  ---------------------------[Description]------------------------------
  Very simple MicroC/OS-II test application for the ORP
  Uses the UART as standard io
  Initialises RTOS
  Calls task init hook that should be provided by another file.
  Starts multitasking
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

#include "includes.h"

/* Prototype for function we'll call to install real tasks  */
void  TaskStart(void *data);
extern void  TaskStartCreateTasks(void); /* Hook to function provided elsewhere */
/* main() prototype */
void main (void);

#define TASK_STK_SIZE 256

OS_STK TaskStartStk[TASK_STK_SIZE];

/* Main loop */
void main(void)
{
  char c;
  /* OS Init function */
  OSInit();

  /* Clear console */
  console_init(80,24);

  console_puts(28,7,"************************");
  console_puts(28,8,"* uC/OS-II on OpenRISC *");
  console_puts(28,9,"************************");

  console_puts(1, 14,"OS Initialising...");

  /* Init first task, which will spawn all others */
  OSTaskCreate(TaskStart, 
	       (void *)0, 
	       &TaskStartStk[TASK_STK_SIZE - 1], 
	       0);
  console_puts(1,12,"Press a key to start OS");
  c = getc();
  console_puts(1,14,"OS Starting...");

  /* Init OR1K tick timer */
  OSInitTick();

  /* Kick off multi-tasking */
  OSStart();
}



/* This task is set to run from main() */
void  TaskStart (void *pdata)
{
    char c;
    OS_CPU_SR cpu_sr;

    pdata = pdata;            /* Prevent compiler warning                 */

    /* Do other user task init things here ... */

    OSStatInit();            /* Initialize uC/OS-II's statistics         */ 

    TaskStartCreateTasks();  /* Create all other tasks                   */
    
    for (;;) {
      /* Maybe do something like update a display here */
      
      /* Check if UART has received anything, potentialy abort due to it. */
      c = testc();
      if (c)
	{ 
	  if (c=='q')
	    {
	      /* Clear up console */
	      console_finish();

	      /* Disable interrupts */
	      OS_ENTER_CRITICAL()

	      /* Exit if simulator, reboot if board */
	      asm("l.ori r3, r0, 0; \
                   l.nop 0x1;	      \
                   l.movhi r3, 0xf000; \
                   l.ori r3,r3,0x100; \
                   l.jr r3;	      \
                   l.nop 0x1");
	    }
	}
          
      OSCtxSwCtr = 0;        /* Clear context switch counter             */
      OSTimeDly(OS_TICKS_PER_SEC);  /* Wait one second                   */
    }
    
}
