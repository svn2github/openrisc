/*$$HEADER*/
/******************************************************************************/
/*                                                                            */
/*                    H E A D E R   I N F O R M A T I O N                     */
/*                                                                            */
/******************************************************************************/

// Project Name                   : OpenRISC Debug Proxy
// File Name                      : or_debug_proxy.h
// Prepared By                    : jb
// Project Start                  : 2008-10-01

/*$$COPYRIGHT NOTICE*/
/******************************************************************************/
/*                                                                            */
/*                      C O P Y R I G H T   N O T I C E                       */
/*                                                                            */
/******************************************************************************/
/*
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; 
  version 2.1 of the License, a copy of which is available from
  http://www.gnu.org/licenses/old-licenses/lgpl-2.1.txt.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

/*$$CHANGE HISTORY*/
/******************************************************************************/
/*                                                                            */
/*                         C H A N G E  H I S T O R Y                         */
/*                                                                            */
/******************************************************************************/

// Date		Version	Description
//------------------------------------------------------------------------
// 081101		First revision           			jb

#ifndef _OR_DEBUG_PROXY_H_
#define _OR_DEBUG_PROXY_H_

#ifndef DEBUG_CMDS
#define DEBUG_CMDS 0  // Output the actual commands being sent to the debug unit
#endif

#ifndef DEBUG_USB_DRVR_FUNCS
#define DEBUG_USB_DRVR_FUNCS 0 // Generate debug output from the USB driver functions
#endif

#ifndef DEBUG_GDB_BLOCK_DATA
#define DEBUG_GDB_BLOCK_DATA 0  // GDB Socket Block data print out
#endif

// This one is defined sometimes in the makefile, so check first
#ifndef DEBUG_GDB
#define DEBUG_GDB 0  // GDB RSP Debugging output enabled
#endif

#ifndef DEBUG_GDB_DUMP_DATA
#define DEBUG_GDB_DUMP_DATA 0  // GDB Socket Debugging - output all data we return to GDB client
#endif

#define Boolean uint32_t
#define false 0
#define true 1

/* Selects crc trailer size in bits. Currently supported: 8 */
#define CRC_SIZE (8)

#include <stdint.h>

extern int serverPort;
extern int server_fd;

extern int vpi_fd; // should be the descriptor for our connection to the VPI server

extern int current_chain;
extern int dbg_chain;

#define DBGCHAIN_SIZE           4 // Renamed from DC_SIZE due to definition clash with something in <windows.h> --jb 090302
#define DC_STATUS_SIZE    4

#define DC_WISHBONE       0
#define DC_CPU0           1
#define DC_CPU1           2

// Defining access types for wishbone
#define DBG_WB_WRITE8           0
#define DBG_WB_WRITE16          1
#define DBG_WB_WRITE32          2
#define DBG_WB_READ8            4
#define DBG_WB_READ16           5
#define DBG_WB_READ32           6

// Defining access types for wishbone
#define DBG_CPU_WRITE            2
#define DBG_CPU_READ             6

// Manually figure the 5-bit reversed values again if they change
#define DI_GO          0
#define DI_READ_CMD    1
#define DI_WRITE_CMD   2
#define DI_READ_CTRL   3
#define DI_WRITE_CTRL  4

#define DBG_CRC_SIZE      32
#define DBG_CRC_POLY      0x04c11db7

#define DBG_ERR_OK        0
#define DBG_ERR_INVALID_ENDPOINT 3
#define DBG_ERR_CRC       8

#define NUM_SOFT_RETRIES  3
#define NUM_HARD_RETRIES  3
#define NUM_ACCESS_RETRIES 10

/* setup connection with the target */
void dbg_test();
/* perform a reset of the debug chain (not of system!) */
int dbg_reset();
/* set instruction register of JTAG TAP */
int dbg_set_tap_ir(uint32_t ir);
/* Set "scan chain" of debug unit (NOT JTAG TAP!) */
int dbg_set_chain(uint32_t chain);
/* read a byte from wishbone */
int dbg_wb_write8(uint32_t adr, uint8_t data);
/* read a word from wishbone */
int dbg_wb_read32(uint32_t adr, uint32_t *data);
/* write a word to wishbone */
int dbg_wb_write32(uint32_t adr, uint32_t data);
/* read a block from wishbone */
int dbg_wb_read_block32(uint32_t adr, uint32_t *data, uint32_t len);
/* write a block to wishbone */
int dbg_wb_write_block32(uint32_t adr, uint32_t *data, uint32_t len);
/* read a register from cpu */
int dbg_cpu0_read(uint32_t adr, uint32_t *data, uint32_t length);
/* read a register from cpu module */
int dbg_cpu0_read_ctrl(uint32_t adr, unsigned char *data);
/* write a cpu register */
int dbg_cpu0_write(uint32_t adr, uint32_t *data, uint32_t length);
/* write a cpu module register */
int dbg_cpu0_write_ctrl(uint32_t adr, unsigned char data);

void print_usage(); // Self explanatory

void check(char *fn, uint32_t l, uint32_t i);

/* Possible errors are listed here.  */
enum enum_errors  /* modified <chris@asics.ws> CZ 24/05/01 */
{
  /* Codes > 0 are for system errors */

  ERR_NONE = 0,
  ERR_CRC = -1,
  ERR_MEM = -2,
  JTAG_PROXY_INVALID_COMMAND = -3,
  JTAG_PROXY_SERVER_TERMINATED = -4,
  JTAG_PROXY_NO_CONNECTION = -5,
  JTAG_PROXY_PROTOCOL_ERROR = -6,
  JTAG_PROXY_COMMAND_NOT_IMPLEMENTED = -7,
  JTAG_PROXY_INVALID_CHAIN = -8,
  JTAG_PROXY_INVALID_ADDRESS = -9,
  JTAG_PROXY_ACCESS_EXCEPTION = -10, /* Write to ROM */
  JTAG_PROXY_INVALID_LENGTH = -11,
  JTAG_PROXY_OUT_OF_MEMORY = -12,
};

#endif /* _OR_DEBUG_PROXY_H_ */

