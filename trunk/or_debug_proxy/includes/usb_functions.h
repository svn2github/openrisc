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


#ifndef CYGWIN_COMPILE
#include "WinTypes.h"
#endif
#include <stdint.h> // for uint_t types
/* Function prototypes */
uint32_t crc_calc(uint32_t crc, uint32_t input_bit);
/* Crc of current read or written data.  */
extern uint32_t crc_r, crc_w;

int retry_do();
void retry_ok();

void usb_close_device_handle();
void usb_write_stream (uint32_t stream, uint32_t num_bits, DWORD dwTapControllerState);
void usb_readwritewrite_stream (uint32_t num_bits);
uint32_t usb_read_stream(uint32_t num_bits, DWORD dwTapControllerState);
uint32_t bit_reverse_data(uint32_t data, uint32_t length);

void usb_set_tap_ir(uint32_t ir);
void usb_dbg_test();
int usb_dbg_reset();
int usb_dbg_set_chain(int chain);
int usb_dbg_command(uint32_t type, uint32_t adr, uint32_t len);
int usb_dbg_ctrl(uint32_t reset, uint32_t stall);
int usb_dbg_ctrl_read(uint32_t *reset, uint32_t *stall);
int usb_dbg_go(unsigned char *data, uint16_t len, uint32_t read);

int usb_dbg_wb_read32(uint32_t adr, uint32_t *data);
int usb_dbg_wb_read_block32(uint32_t adr, uint32_t *data, uint32_t len);
int usb_dbg_wb_write8(uint32_t adr, uint8_t data);
int usb_dbg_wb_write32(uint32_t adr, uint32_t data);
int usb_dbg_wb_write_block32(uint32_t adr, uint32_t *data, uint32_t len);
int usb_dbg_cpu0_read(uint32_t adr, uint32_t *data);
int usb_dbg_cpu0_read_ctrl(uint32_t adr, unsigned char *data);
int usb_dbg_cpu0_write(uint32_t adr, uint32_t data);
int usb_dbg_cpu0_write_ctrl(uint32_t adr, unsigned char data);
