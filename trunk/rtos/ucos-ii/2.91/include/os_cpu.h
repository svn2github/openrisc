/*
----------------------------------------------------------------------
    CHiPES Embedded RTR	Systems Copyright (c) Tim Oliver 2002-2004
----------------------------------------------------------------------
 File   : os_cpu.h
 Author : Tim Oliver
 Email  : timtimoliver@yahoo.co.uk
---------------------------[Description]------------------------------

		Part of the OpenRISC Reference Platform Port of MicroC/OS-II

-------------------------[CVS Information]----------------------------
  $Id: os_cpu.h,v 1.1 2004/07/22 06:36:58 p014082819 Exp $
  $Revision: 1.1 $
----------------------------------------------------------------------
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

#ifndef OS_CPU_H
#define OS_CPU_H

#ifdef  OS_CPU_GLOBALS
#define OS_CPU_EXT
#else
#define OS_CPU_EXT  extern
#endif

/* Register access macros */
#define REG8(add) *((volatile unsigned char *)(add))
#define REG16(add) *((volatile unsigned short *)(add))
#define REG32(add) *((volatile unsigned long *)(add))

/*$PAGE*/
/*
*********************************************************************************************************
*                                              DATA TYPES
*                                         (Compiler Specific)
*********************************************************************************************************
*/

typedef unsigned char  BOOLEAN;
typedef unsigned char  INT8U;                    /* Unsigned  8 bit quantity                           */
typedef signed   char  INT8S;                    /* Signed    8 bit quantity                           */
typedef unsigned int   INT16U;                   /* Unsigned 16 bit quantity                           */
typedef signed   int   INT16S;                   /* Signed   16 bit quantity                           */
typedef unsigned long  INT32U;                   /* Unsigned 32 bit quantity                           */
typedef signed   long  INT32S;                   /* Signed   32 bit quantity                           */

typedef unsigned long   OS_STK;                   /* Each stack entry is 32-bits wide                  */
typedef unsigned long volatile   OS_CPU_SR;       /* The CPU Status Word is 32-bits wide               */
						  /* This variable MUST be volatile for proper */
						  /* operation. Refer to os_cpu_a.s for more */
						  /* details. */

/*
*********************************************************************************************************
*                              OpenCores OpenRISC
*
* Method #1:  Disable/Enable interrupts using simple instructions.  After critical section, interrupts
*             will be enabled even if they were disabled before entering the critical section.
*
* Method #2:  Disable/Enable interrupts by preserving the state of interrupts.  In other words, if
*             interrupts were disabled before entering the critical section, they will be disabled when
*             leaving the critical section.
*
* Method #3:  Disable/Enable interrupts by preserving the state of interrupts.  Generally speaking you
*             would store the state of the interrupt disable flag in the local variable 'cpu_sr' and then
*             disable interrupts.  'cpu_sr' is allocated in all of uC/OS-II's functions that need to
*             disable interrupts.  You would restore the interrupt disable state by copying back 'cpu_sr'
*             into the CPU's status register.
*********************************************************************************************************
*/

#define  OS_CRITICAL_METHOD    3

#if      OS_CRITICAL_METHOD == 3
#define  OS_ENTER_CRITICAL() cpu_sr = OSDisableInterrupts();
#define  OS_EXIT_CRITICAL()  OSEnableInterrupts(cpu_sr);
#endif

/*
*********************************************************************************************************
*                           OpenCores OpenRISC
*********************************************************************************************************
*/

#define  OS_STK_GROWTH        1  /* Stack grows from HIGH to LOW memory on OpenRISC  */

#define  OS_TASK_SW() __asm__ ("l.sys 0");\
                      __asm__ ("l.nop");
/*
*********************************************************************************************************
*                                            GLOBAL VARIABLES
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                          FUNCTION PROTOTYPES
*********************************************************************************************************
*/
#if      OS_CRITICAL_METHOD == 3
OS_CPU_SR OSDisableInterrupts(void);
void OSEnableInterrupts(OS_CPU_SR cpu_sr);
#endif
/*
#ifdef	DEBUG
#define debug(fmt,args...) printf (fmt ,##args)
#else
#define debug(fmt,args...) __printf (fmt ,##args)
#endif
*/
extern void mtspr(unsigned long spr, unsigned long value);      /* For writing into SPR. */
extern unsigned long mfspr(unsigned long spr);                  /* For reading SPR. */
extern void report(unsigned long value);                        /* Prints out a value */
extern void exit (int i) __attribute__ ((__noreturn__));        /* return value by making a syscall */
//extern void __printf (const char *fmt, ...);                    /* simulator stdout */
extern void OSInitTick (void);                                  /* init ticker */

void main (void); /* Function to be called at entry point - not defined here.  */

/* stdio */
extern int getc (void);
extern int testc (void);
extern int ctrlc (void);
extern void putc (const char c);

#endif

