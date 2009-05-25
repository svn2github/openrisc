/* Remote debugging interface for OpenRISC 1000 JTAG debugging protocol.

   Copyright (C) 2001 Chris Ziomkowski, chris@asics.ws
   Copyright 2008 Embecosm Limited

   Areas noted by (CZ) were modified by Chris Ziomkowski <chris@asics.ws>
   Contributor Jeremy Bennett <jeremy.bennett@embecosm.com>

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the Free
   Software Foundation; either version 3 of the License, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
   FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
   more details.

   You should have received a copy of the GNU General Public License along
   with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/*!---------------------------------------------------------------------------
   Updated for GDB 6.8 by Jeremy Bennett. All code converted to ANSI C style
   and in general to GDB format. All global OpenRISC specific functions and
   variables should now have a prefix of or1k_ or OR1K_.

   JTAG connects to or1k target ops. See or1k-tdep.c. This header is also used
   by Or1ksim, the OpenRISC 1000 architectural simulator.
   --------------------------------------------------------------------------*/

#ifndef OR1K_JTAG__H
#define OR1K_JTAG__H

#include <sys/types.h>
#include <inttypes.h>

/*! JTAG instruction size. See JTAG documentation about these.  */
#define OR1K_JI_SIZE (4)

/*! JTAG instructions. See JTAG documentation about these.  */
enum or1k_jtag_instr {
  OR1K_JI_CHAIN_SELECT = 3,
  OR1K_JI_DEBUG        = 8
};

/*! Scan chain size in bits.  */
#define OR1K_SC_SIZE (4)

/*! All JTAG chains.  */
enum or1k_jtag_chains {
  OR1K_SC_UNDEF      = -1,	/* Scan chain not defined */
  OR1K_SC_RISC_DEBUG =  1,	/* 1 RISC Debug Interface chain (for SPRs) */
  OR1K_SC_REGISTER   =  4,	/* 4 JTAG Register Chain */
  OR1K_SC_WISHBONE   =  5,	/* 5 Wisbone Chain (for memory access) */
};

/*! Version of the debug interface. This affects the address on the control
    (SC_REGISTER) scan chain. */
enum or1k_dbg_if_version_enum {
  OR1K_DBG_IF_ORPSOC,			/* Version with ORPSoC */
  OR1K_DBG_IF_MOHOR			/* Igor Mohor's debug interface */
};

/*! JTAG registers (for use with ORK1_SC_REGISTER scan chain). There are two
   flavours of this, depending which version of the debug interface you are
   using. */
#define OR1K_JTAG_RISCOP  (OR1K_DBG_IF_ORPSOC == or1k_dbg_if_version ? 0x04 : \
                                                                       0x00 )

/* JTAG register fields */

#define OR1K_JTAG_RISCOP_STALL  0x00000001
#define OR1K_JTAG_RISCOP_RESET  0x00000002

/*! The OR1K JTAG proxy protocol commands. */
enum or1k_jtag_proxy_protocol_commands {
  OR1K_JTAG_COMMAND_READ        = 1,
  OR1K_JTAG_COMMAND_WRITE       = 2,
  OR1K_JTAG_COMMAND_READ_BLOCK  = 3,
  OR1K_JTAG_COMMAND_WRITE_BLOCK = 4,
  OR1K_JTAG_COMMAND_CHAIN       = 5,
};

/*! Maximum size of write block sent in one go */
#define  OR1K_MAX_JTAG_WRITE  1024

/* Each transmit structure must begin with an integer which specifies the type
 * of command. Information after this is variable. Make sure to have all
 * information aligned properly. If we stick with 32 bit integers, it should
 * be portable onto every platform. These structures will be transmitted
 * across the network in network byte order.
*/

struct jtr_read_message {
  uint32_t  command;
  uint32_t  length;
  uint32_t  address;
};

struct jtr_write_message {
  uint32_t  command;
  uint32_t  length;
  uint32_t  address;
  uint32_t  data_h;
  uint32_t  data_l;
};

struct jtr_read_block_message {
  uint32_t  command;
  uint32_t  length;
  uint32_t  address;
  int32_t   num_regs;
};

struct jtr_write_block_message {
  uint32_t  command;
  uint32_t  length;
  uint32_t  address;
  int32_t   num_regs;
  uint32_t  data[1];
};

struct jtr_chain_message {
  uint32_t  command;
  uint32_t  length;
  uint32_t  chain;
};

/* The responses are messages specific, however convention states the first
 * word should be an error code. Again, sticking with 32 bit integers should
 * provide maximum portability. */

struct jtr_failure_response {
  int32_t  status;
};

struct jtr_read_response {
  int32_t   status;
  uint32_t  data_h;
  uint32_t  data_l;
};
  
struct jtr_write_response {
  int32_t  status;
};

struct jtr_read_block_response {
  int32_t   status;
  int32_t   num_regs;
};

struct jtr_write_block_response {
  int32_t  status;
};

struct jtr_chain_response {
  int32_t  status;
};


/*! Error codes for the OpenRISC 1000 JTAG debugging protocol */
enum or1k_jtag_errors  /* modified <chris@asics.ws> CZ 24/05/01 */
{
  /* Codes > 0 are for system errors */

  OR1K_JTAG_ERR_NONE = 0,
  OR1K_JTAG_ERR_CRC = -1,
  OR1K_JTAG_ERR_MEM = -2,
  OR1K_JTAG_ERR_INVALID_COMMAND = -3,
  OR1K_JTAG_ERR_SERVER_TERMINATED = -4,
  OR1K_JTAG_ERR_NO_CONNECTION = -5,
  OR1K_JTAG_ERR_PROTOCOL_ERROR = -6,
  OR1K_JTAG_ERR_COMMAND_NOT_IMPLEMENTED = -7,
  OR1K_JTAG_ERR_INVALID_CHAIN = -8,
  OR1K_JTAG_ERR_INVALID_ADDRESS = -9,
  OR1K_JTAG_ERR_ACCESS_EXCEPTION = -10, /* Write to ROM */
  OR1K_JTAG_ERR_INVALID_LENGTH = -11,
  OR1K_JTAG_ERR_OUT_OF_MEMORY = -12,
};

/* Global variable identifying the debug interface version */
extern enum or1k_dbg_if_version_enum  or1k_dbg_if_version;

/* Global OR1K JTAG functions */
extern void      or1k_jtag_init (char *args);
extern void      or1k_jtag_close ();
extern ULONGEST  or1k_jtag_read_spr (unsigned int  sprnum);
extern void      or1k_jtag_write_spr (unsigned int  sprnum,
				      ULONGEST      data);
extern int       or1k_jtag_read_mem (CORE_ADDR  addr,
				     gdb_byte  *bdata,
				     int        len);
extern int       or1k_jtag_write_mem (CORE_ADDR       addr,
				      const gdb_byte *bdata,
				      int             len);
extern void      or1k_jtag_stall ();
extern void      or1k_jtag_unstall ();
extern void      or1k_jtag_wait (int  fast);


#endif /* OR1K_JTAG__H */
