/*$$HEADER*/
/******************************************************************************/
/*                                                                            */
/*                    H E A D E R   I N F O R M A T I O N                     */
/*                                                                            */
/******************************************************************************/

// Project Name                   : OpenRISC Debug Proxy
// File Name                      : usb_functions.h
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

//TODO: Make this port variable (at both ends!!)
// HARDCODED port for communications with functions attached to RTL sim via VPI
#define VPI_PORT 54002

#ifdef DEBUG_VPI
#define debug printf
#else
#define debug
#endif

#ifdef DEBUG2_VPI
#define debug2 printf
#else
#define debug2
#endif


/* VPI communication function prototyopes */
void get_response_from_vpi();
void get_data_from_vpi();
void get_block_data_from_vpi(uint32_t len, uint32_t *data);
void send_data_to_vpi(uint32_t data);
void send_block_data_to_vpi(uint32_t len, uint32_t *data);
void send_address_to_vpi(uint32_t address);
void send_command_to_vpi(char CMD);

int vpi_connect();

void vpi_dbg_test();
int vpi_dbg_reset();
int vpi_dbg_set_chain(uint32_t chain);
int vpi_dbg_ctrl(uint32_t reset, uint32_t stall);
int vpi_dbg_ctrl_read(uint32_t *reset, uint32_t *stall);

int vpi_dbg_cpu0_write_ctrl(uint32_t adr, unsigned char data);
int vpi_dbg_cpu0_write(uint32_t adr, uint32_t data);
int vpi_dbg_cpu0_read_ctrl(uint32_t adr, unsigned char *data);
int vpi_dbg_cpu0_read(uint32_t adr, uint32_t *data);
int vpi_dbg_wb_write_block32(uint32_t adr, uint32_t *data, uint32_t len);
int vpi_dbg_wb_read_block32(uint32_t adr, uint32_t *data, uint32_t len);
int vpi_dbg_wb_write32(uint32_t adr, uint32_t data);
int vpi_dbg_wb_read32(uint32_t adr, uint32_t *data);



