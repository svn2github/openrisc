/* Newlib board descriptions for Or1ksim
  
   Copyright (C)
   Copyright (C) 2008, 2010 Embecosm Limited
  
   Contributor
   Contributor Jeremy Bennett <jeremy.bennett@embecosm.com>
  
   This file is part of Newlib.
  
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

/* This machine configuration matches the Or1ksim configuration file in this
   directory. */

#ifndef OR1KSIM_BOARD__H
#define OR1KSIM_BOARD__H

/* Cache information (no longer used, cache configured through UPR and CCFGRs)*/
#define IC_ENABLE 	    1
#define IC_SIZE          8192

#define DC_ENABLE 	    0
#define DC_SIZE          8192

#define SDRAM_BASE_ADDR 0x00000000
#define SDRAM_TMS_VAL   0x07248230

/* UART information */
#define IN_CLK  	  25000000	/*!< 25 MHz */
#define UART_BAUD_RATE 	    115200	/*!< 115.2 kHz */

#define UART_BASE  	0x90000000
#define UART_IRQ                19

/* l.nop constants */
#define NOP_EXIT        0x0001      /*!< End of simulation */
#define NOP_PUTC        0x0004      /*!< Put a character */

/*! Define UART_AUTO_ECHO to have the UART echo reads. */
#undef UART_AUTO_ECHO

/*! 8-bit register access macro */
#define REG8(add) *((volatile unsigned char *) (add))

#endif	/* OR1KSIM_BOARD__H */
