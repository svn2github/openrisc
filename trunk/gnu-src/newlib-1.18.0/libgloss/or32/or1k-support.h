/* OR1K support defines
  
   Copyright (C) 2011, ORSoC AB
   Copyright (C) 2008, 2010 Embecosm Limited
  
   Contributor Julius Baxter  <julius.baxter@orsoc.se>
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

#ifndef OR1K_SUPPORT__H
#define OR1K_SUPPORT__H

/* External symbols from each board's object file */
extern unsigned long _board_clk_freq;

extern unsigned long _board_uart_base;
extern unsigned long _board_uart_baud;
extern unsigned long _board_uart_irq;

/* Check if board has UART - test base address */
#define BOARD_HAS_UART (_board_uart_base)

/* l.nop constants */
#define NOP_EXIT        0x0001      /*!< End of simulation */
#define NOP_PUTC        0x0004      /*!< Put a character */

/*! 8-bit register access macro */
#define REG8(add) *((volatile unsigned char *) (add))

#endif	/* OR1K_SUPPORT__H */
