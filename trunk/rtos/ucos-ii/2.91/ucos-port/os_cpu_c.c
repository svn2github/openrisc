/*
  ----------------------------------------------------------------------
  CHiPES Embedded RTR	Systems Copyright (c) Tim Oliver 2002-2004
  ----------------------------------------------------------------------
  File   :	os_cpu_c.c
  Author : Tim Oliver
  Email  : timtimoliver@yahoo.co.uk
  ---------------------------[Description]------------------------------

  uC/OS-II
  The Real-Time Kernel

  (c) Copyright 1992-2002, Jean J. Labrosse, Weston, FL
  All Rights Reserved

  OpenCores OpenRISC

  Inline NOP ORP simulator functions :
  void exit (int i) - simulator exit point
  void report(unsigned long value) - simulator report messaging

  Internal Routines :
  void mtspr(unsigned long spr, unsigned long vakye) -  Move To SPR
  unsigned long mfspr(unsigned long spr) -  Move From SPR
  OS_CPU_SR OSDisableInterrupts (void)
  void OSEnableInterrupts(OS_CPU_SR cpu_sr)
  void OSInitTick(void)
  void putc (char c) - standard output patch to UART
  int getc () - standard input patch to UART
  int testc () - check for character received by UART
  int ctrlc () - check for control-C received by UART
  For OS Routines see descriptions below

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

#define  OS_CPU_GLOBALS
#include "includes.h"

/* return value by making a syscall */
void exit (int i)
{
  asm("l.add r3,r0,%0": : "r" (i));
  asm("l.nop %0": :"K" (NOP_EXIT));
  while (1);
}

/* print long */
void report(unsigned long value)
{
  asm("l.addi\tr3,%0,0": :"r" (value));
  asm("l.nop %0": :"K" (NOP_REPORT));
}

/* For writing into SPR. */
void mtspr(unsigned long spr, unsigned long value)
{
  asm("l.mtspr\t\t%0,%1,0": : "r" (spr), "r" (value));
}

/* For reading SPR. */
unsigned long mfspr(unsigned long spr)
{
  unsigned long value;
  asm("l.mfspr\t\t%0,%1,0" : "=r" (value) : "r" (spr));
  return value;
}

#if      OS_CRITICAL_METHOD == 3
OS_CPU_SR OSDisableInterrupts (void)
{
  OS_CPU_SR cpu_sr;

  cpu_sr = mfspr(SPR_SR);
  mtspr(SPR_SR, cpu_sr & ~(SPR_SR_IEE | SPR_SR_TEE));
  return cpu_sr;
}

void OSEnableInterrupts(OS_CPU_SR cpu_sr)
{
  mtspr(SPR_SR, cpu_sr); /* | (SPR_SR_IEE | SPR_SR_TEE)*/
}

#endif

void OSInitTick(void)
{
  /* Enable tick interrupts in supervisor register */
  mtspr(SPR_SR, SPR_SR_TEE | mfspr(SPR_SR));
  /* Set tick timer to restart on match, and set interrupt enable, and set period depending on clock
     frequency and desired tick rate */
  mtspr(SPR_TTMR, SPR_TTMR_IE | SPR_TTMR_RT | ((IN_CLK/TICKS_PER_SEC) & SPR_TTMR_PERIOD));
}

/* stdio patches */
void putc (char c) {
  OS_CPU_SR cpu_sr;

  OS_ENTER_CRITICAL()
    uart_putc (c);
  OS_EXIT_CRITICAL()
}

int getc () {

  return uart_getc ();
}


int testc () {

  return uart_testc ();
}

int ctrlc ()
{
  if (testc ()) {
    switch (getc ()) {
    case 0x03:    /* ^C - Control C */
      return 1;
    default:
      break;
    }
  }
  return 0;
}

/*
*********************************************************************************************************
*                                       OS INITIALIZATION HOOK
*                                            (BEGINNING)
*
* Description: This function is called by OSInit() at the beginning of OSInit().
*
* Arguments  : none
*
* Note(s)    : 1) Interrupts should be disabled during this call.
*********************************************************************************************************
*/
#if OS_CPU_HOOKS_EN > 0 && OS_VERSION > 203
void  OSInitHookBegin (void)
{

  uart_init();

#ifdef __TFP_PRINTF__
  init_printf(NULL,putc);
#endif

  return;

}
#endif

/*
*********************************************************************************************************
*                                       OS INITIALIZATION HOOK
*                                               (END)
*
* Description: This function is called by OSInit() at the end of OSInit().
*
* Arguments  : none
*
* Note(s)    : 1) Interrupts should be disabled during this call.
*********************************************************************************************************
*/
#if OS_CPU_HOOKS_EN > 0 && OS_VERSION > 203
void  OSInitHookEnd (void)
{
}
#endif

/*$PAGE*/
/*
*********************************************************************************************************
*                                          TASK CREATION HOOK
*
* Description: This function is called when a task is created.
*
* Arguments  : ptcb   is a pointer to the task control block of the task being created.
*
* Note(s)    : 1) Interrupts are disabled during this call.
*********************************************************************************************************
*/
#if OS_CPU_HOOKS_EN > 0
void  OSTaskCreateHook (OS_TCB *ptcb)
{
  ptcb = ptcb;                       /* Prevent compiler warning                                     */
}
#endif


/*
*********************************************************************************************************
*                                           TASK DELETION HOOK
*
* Description: This function is called when a task is deleted.
*
* Arguments  : ptcb   is a pointer to the task control block of the task being deleted.
*
* Note(s)    : 1) Interrupts are disabled during this call.
*********************************************************************************************************
*/
#if OS_CPU_HOOKS_EN > 0
void  OSTaskDelHook (OS_TCB *ptcb)
{
  ptcb = ptcb;                       /* Prevent compiler warning                                     */
}
#endif

/*
*********************************************************************************************************
*                                             IDLE TASK HOOK
*
* Description: This function is called by the idle task.  This hook has been added to allow you to do
*              such things as STOP the CPU to conserve power.
*
* Arguments  : none
*
* Note(s)    : 1) Interrupts are enabled during this call.
*********************************************************************************************************
*/
#if OS_CPU_HOOKS_EN > 0 && OS_VERSION >= 251
void  OSTaskIdleHook (void)
{
  /* printf("\nEntering idle task"); */
}
#endif

/*
*********************************************************************************************************
*                                           STATISTIC TASK HOOK
*
* Description: This function is called every second by uC/OS-II's statistics task.  This allows your
*              application to add functionality to the statistics task.
*
* Arguments  : none
*********************************************************************************************************
*/

#if OS_CPU_HOOKS_EN > 0
void  OSTaskStatHook (void)
{
}
#endif

/*$PAGE*/
/*
*********************************************************************************************************
*                                        INITIALIZE A TASK'S STACK
*
* Description: This function is called by either OSTaskCreate() or OSTaskCreateExt() to initialize the
*              stack frame of the task being created.  This function is highly processor specific.
*
* Arguments  : task          is a pointer to the task code
*
*              pdata         is a pointer to a user supplied data area that will be passed to the task
*                            when the task first executes.
*
*              ptos          is a pointer to the top of stack.  It is assumed that 'ptos' points to
*                            a 'free' entry on the task stack.  If OS_STK_GROWTH is set to 1 then
*                            'ptos' will contain the HIGHEST valid address of the stack.  Similarly, if
*                            OS_STK_GROWTH is set to 0, the 'ptos' will contains the LOWEST valid address
*                            of the stack.
*
*              opt           specifies options that can be used to alter the behavior of OSTaskStkInit().
*                            (see uCOS_II.H for OS_TASK_OPT_???).
*
* Returns    : Always returns the location of the new top-of-stack' once the processor registers have
*              been placed on the stack in the proper order.
*
*********************************************************************************************************
*/

OS_STK  *OSTaskStkInit (void (*task)(void *pd), void *pdata, OS_STK *ptos, 
			INT16U opt)
{
  OS_STK *stk;
  OS_STK *fp;
  INT32U  sr;

#ifdef DEBUG
  printf("\nCreating Stack at %x for task %x", ptos, task);
#endif

  sr     = mfspr(SPR_SR);
  opt    = opt;                           /* 'opt' is not used, prevent warning */
  stk    = ptos;                          /* Load stack pointer */
  stk    -=32;                            /* Skip over red zone, 32 words */
  stk--;

  *stk-- = (OS_STK)(pdata);               /* fp+0 -> parameter 0    */
  fp     = stk;
  *stk-- = (OS_STK)fp;                    /* sp+4 -> previous fp    */
  *stk-- = (INT32U)0;                     /* sp+0 -> return address */

  *stk-- = (INT32U)31;                     /* r31 = 0  */
  *stk-- = (INT32U)30;                     /* r30 = 0  */
  *stk-- = (INT32U)29;                     /* r29 = 0  */
  *stk-- = (INT32U)28;                     /* r28 = 0  */
  *stk-- = (INT32U)27;                     /* r27 = 0  */
  *stk-- = (INT32U)26;                     /* r26 = 0  */
  *stk-- = (INT32U)25;                     /* r25 = 0  */
  *stk-- = (INT32U)24;                     /* r24 = 0  */
  *stk-- = (INT32U)23;                     /* r23 = 0  */
  *stk-- = (INT32U)22;                     /* r22 = 0  */
  *stk-- = (INT32U)21;                     /* r21 = 0  */
  *stk-- = (INT32U)20;                     /* r20 = 0  */
  *stk-- = (INT32U)19;                     /* r19 = 0  */
  *stk-- = (INT32U)18;                     /* r18 = 0  */
  *stk-- = (INT32U)17;                     /* r17 = 0  */
  *stk-- = (INT32U)16;                     /* r16 = 0  */
  *stk-- = (INT32U)15;                     /* r15 = 0  */
  *stk-- = (INT32U)14;                     /* r14 = 0  */
  *stk-- = (INT32U)13;                     /* r13 = 0  */
  *stk-- = (INT32U)12;                     /* r12 = 0  */
  *stk-- = (INT32U)11;                     /* r11 = 0 return value      */
  *stk-- = (INT32U)10;                     /* r10 = 0                   */
  *stk-- = (INT32U)9;                     /* r09 = 0 link register     */
  *stk-- = (INT32U)8;                     /* r08 = 0 function paramters*/
  *stk-- = (INT32U)7;                     /* r07 = 0                   */
  *stk-- = (INT32U)6;                     /* r06 = 0                   */
  *stk-- = (INT32U)5;                     /* r05 = 0                   */
  *stk-- = (INT32U)4;                     /* r04 = 0                   */
  *stk-- = (OS_STK)(pdata);               /* r03 = arg0                */
  *stk-- = (OS_STK)fp;                    /* r02 = frame pointer       */
                                          /* supervision register      */
  *stk-- = (INT32U)(SPR_SR_IEE | SPR_SR_TEE | SPR_SR_SM |
	     (sr & (SPR_SR_ICE | SPR_SR_DCE)));
  *stk   = (OS_STK)(task);                /* program counter           */

  return ((OS_STK *)stk);                 /* sp gets saved in TCB      */
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                           TASK SWITCH HOOK
*
* Description: This function is called when a task switch is performed.  This allows you to perform other
*              operations during a context switch.
*
* Arguments  : none
*
* Note(s)    : 1) Interrupts are disabled during this call.
*              2) It is assumed that the global pointer 'OSTCBHighRdy' points to the TCB of the task that
*                 will be 'switched in' (i.e. the highest priority task) and, 'OSTCBCur' points to the
*                 task being switched out (i.e. the preempted task).
*********************************************************************************************************
*/
#if OS_CPU_HOOKS_EN > 0
void  OSTaskSwHook (void)
{
  /* printf("\nTask Switching\n"); */
}
#endif

/*
*********************************************************************************************************
*                                           OSTCBInit() HOOK
*
* Description: This function is called by OS_TCBInit() after setting up most of the TCB.
*
* Arguments  : ptcb    is a pointer to the TCB of the task being created.
*
* Note(s)    : 1) Interrupts may or may not be ENABLED during this call.
*********************************************************************************************************
*/
#if OS_CPU_HOOKS_EN > 0 && OS_VERSION > 203
void  OSTCBInitHook (OS_TCB *ptcb)
{
  ptcb = ptcb;                                           /* Prevent Compiler warning                 */
}
#endif


/*
*********************************************************************************************************
*                                               TICK HOOK
*
* Description: This function is called every tick.
*
* Arguments  : none
*
* Note(s)    : 1) Interrupts may or may not be ENABLED during this call.
*********************************************************************************************************
*/
#if OS_CPU_HOOKS_EN > 0
void  OSTimeTickHook (void)
{
}
#endif


void          OSTaskReturnHook        (OS_TCB          *ptcb)
{
  /* do nothing for now */
  return;
}
