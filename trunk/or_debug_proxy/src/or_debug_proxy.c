/*$$HEADER*/
/******************************************************************************/
/*                                                                            */
/*                    H E A D E R   I N F O R M A T I O N                     */
/*                                                                            */
/******************************************************************************/

// Project Name                   : OpenRISC Debug Proxy
// File Name                      : or_debug_proxy.c
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

/*$$DESCRIPTION*/
/******************************************************************************/
/*                                                                            */
/*                           D E S C R I P T I O N                            */
/*                                                                            */
/******************************************************************************/
//
// The entry point for the OpenRISC debug proxy console application. Is 
// compilable under both Linux/Unix systems and Cygwin Windows.
//

#include <assert.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/types.h>

// Windows includes
#ifdef CYGWIN_COMPILE
#include <windows.h>
#include "win_FTCJTAG.h"
#include "win_FTCJTAG_ptrs.h"
#else
#include <signal.h>
void catch_sigint(int sig_num); // First param must be "int"
#endif

#include "gdb.h"

#ifdef USB_ENDPOINT_ENABLED
#include "usb_functions.h"
#endif

#ifdef VPI_ENDPOINT_ENABLED
#include "vpi_functions.h"
#endif

#include "or_debug_proxy.h"

// Defines of endpoint numbers
#define ENDPOINT_TARGET_NONE 0
#define ENDPOINT_TARGET_USB 1
#define ENDPOINT_TARGET_VPI 2

static int endpoint_target; // Either VPI interface via sockets, or the USB device

#define GDB_PROTOCOL_JTAG  1
#define GDB_PROTOCOL_RSP   2
#define GDB_PROTOCOL_NONE  3

int err; // Global error value

/* Currently selected scan chain - just to prevent unnecessary transfers. */
int current_chain = -1;

/* The chain that should be currently selected. */
int dbg_chain = -1;

int main(int argc,  char *argv[]) {

  char *s;
  int gdb_protocol = GDB_PROTOCOL_NONE;
  endpoint_target = ENDPOINT_TARGET_NONE;
  int inp_arg = 1;

  // Check we were compiled with at least one endpoint enabled
#ifndef USB_ENDPOINT_ENABLED
#ifndef VPI_ENDPOINT_ENABLED
  printf("No endpoints enabled.\nRecompile the proxy with at least one endpoint enabled\n");
  exit(0);
#endif
#endif

  // init our global error number
  err = DBG_ERR_OK;

  // Parse input options
  if (argc < 3)
    {
      print_usage();
      exit(1);
    }

  err = DBG_ERR_OK;
  srand(getpid());

  // Parse through the input, check what we've been given

  while ( argv[inp_arg] != NULL )
    {
      if(strcmp(argv[inp_arg], "-r") == 0) 
	{
	  gdb_protocol = GDB_PROTOCOL_RSP;
	  endpoint_target = ENDPOINT_TARGET_USB;
	}
      else if(strcmp(argv[inp_arg], "-v") == 0) 
	{
	  gdb_protocol = GDB_PROTOCOL_RSP;
	  endpoint_target = ENDPOINT_TARGET_VPI;
	}
      else
	{
	  serverPort = strtol(argv[2],&s,10);
	}
      
      inp_arg++;
    }

  if(endpoint_target == ENDPOINT_TARGET_NONE || gdb_protocol == GDB_PROTOCOL_NONE || serverPort > 65535 || *s != '\0')
    {
      print_usage();
      exit(1);
    }

#ifdef CYGWIN_COMPILE
  // Load the FTCJTAG DLL function pointers
  if (getFTDIJTAGFunctions() < 0){
    exit(-1);
  }
#endif

#ifndef CYGWIN_COMPILE
  // Install a signal handler to exit gracefully
  // when we receive a sigint
  signal(SIGINT, catch_sigint);
#endif
  
  /* Initialise connection to our OR1k system */
  current_chain = -1;
#ifdef USB_ENDPOINT_ENABLED
  /* USB Endpoint */
  if (endpoint_target == ENDPOINT_TARGET_USB)
    {
      printf("\nConnecting to OR1k via USB debug cable\n\n");
      if ((err = usb_dbg_reset())) goto JtagIfError;
      dbg_test(); // Perform some tests
    }
#endif
#ifdef VPI_ENDPOINT_ENABLED
  /* RTL simulation endpoint */
  if (endpoint_target == ENDPOINT_TARGET_VPI){
    printf("\nConnecting to OR1k RTL simulation\n\n");
    // Connect to the (hopefully) already running RTL simulation server running via VPI
    vpi_fd = vpi_connect();
    
    if ((err = vpi_dbg_reset())) goto JtagIfError;
    vpi_dbg_test(); // Perform some tests
  }
#endif
  
  /* We have a connection to the target system.  Now establish server connection. */
  if(gdb_protocol == GDB_PROTOCOL_RSP)
    { // Connect to RSP server
      /* RSP always starts stalled as though we have just reset the processor. */
      // rsp_exception (EXCEPT_TRAP);
      handle_rsp ();
    // if((server_fd = GetServerSocket("or1ksim","tcp", serverPort))) {
    }else {
    fprintf(stderr,"Cannot start RSP Proxy server on port %d\n", serverPort);
    exit(-1);
  }


JtagIfError:
  fprintf(stderr,"Connection via USB debug cable failed (err = %d).\nPlease ensure the device is attached and correctly installed\n\n", err);
  exit(-1);
  return 0;
}


int dbg_reset()
{
#ifdef USB_ENDPOINT_ENABLED
  if (endpoint_target == ENDPOINT_TARGET_USB) return usb_dbg_reset();
#endif
#ifdef VPI_ENDPOINT_ENABLED
  if (endpoint_target == ENDPOINT_TARGET_VPI) return vpi_dbg_reset();
#endif
  return DBG_ERR_INVALID_ENDPOINT;
}

void dbg_test() {
#ifdef USB_ENDPOINT_ENABLED
  if (endpoint_target == ENDPOINT_TARGET_USB) usb_dbg_test();
#endif
#ifdef VPI_ENDPOINT_ENABLED
  if (endpoint_target == ENDPOINT_TARGET_VPI) vpi_dbg_test();
#endif
}

/* Set TAP instruction register */
int dbg_set_tap_ir(uint32_t ir) {
#ifdef USB_ENDPOINT_ENABLED
  if (endpoint_target == ENDPOINT_TARGET_USB) usb_set_tap_ir(ir);
#endif
  return DBG_ERR_INVALID_ENDPOINT;
}

/* Sets scan chain.  */
int dbg_set_chain(uint32_t chain) {
#ifdef USB_ENDPOINT_ENABLED
  if (endpoint_target == ENDPOINT_TARGET_USB) return usb_dbg_set_chain(chain);
#endif
#ifdef VPI_ENDPOINT_ENABLED
  if (endpoint_target == ENDPOINT_TARGET_VPI) return vpi_dbg_set_chain(chain);
#endif
  return DBG_ERR_INVALID_ENDPOINT;
} 

/* sends out a command with 32bit address and 16bit length, if len >= 0 */
int dbg_command(uint32_t type, uint32_t adr, uint32_t len) {
  // This is never called by any of the VPI functions, so only USB endpoint
#ifdef USB_ENDPOINT_ENABLED
 if (endpoint_target == ENDPOINT_TARGET_USB) return usb_dbg_command(type,adr,len);
#endif
 return DBG_ERR_INVALID_ENDPOINT;
}

/* writes a ctrl reg */
int dbg_ctrl(uint32_t reset, uint32_t stall) {
#ifdef USB_ENDPOINT_ENABLED
  if (endpoint_target == ENDPOINT_TARGET_USB) return usb_dbg_ctrl(reset, stall);
#endif
#ifdef VPI_ENDPOINT_ENABLED
  if (endpoint_target == ENDPOINT_TARGET_VPI) return vpi_dbg_ctrl(reset, stall);
#endif
  return DBG_ERR_INVALID_ENDPOINT;
}

/* reads control register */
int dbg_ctrl_read(uint32_t *reset, uint32_t *stall) {
#ifdef USB_ENDPOINT_ENABLED
    if (endpoint_target == ENDPOINT_TARGET_USB) return usb_dbg_ctrl_read(reset, stall);
#endif
#ifdef VPI_ENDPOINT_ENABLED
    if (endpoint_target == ENDPOINT_TARGET_VPI) return vpi_dbg_ctrl_read(reset, stall);
#endif
    return DBG_ERR_INVALID_ENDPOINT;
}

/* issues a burst read/write */
int dbg_go(unsigned char *data, uint16_t len, uint32_t read) {
  // Only USB endpouint32_t option here
#ifdef USB_ENDPOINT_ENABLED
  if (endpoint_target == ENDPOINT_TARGET_USB) return usb_dbg_go(data, len, read);
#endif
  return DBG_ERR_INVALID_ENDPOINT;
}

/* read a byte from wishbone */
int dbg_wb_read8(uint32_t adr, uint8_t *data) {
#ifdef USB_ENDPOINT_ENABLED
  if (endpoint_target == ENDPOINT_TARGET_USB) return usb_dbg_wb_read8(adr, data);
#endif
#ifdef VPI_ENDPOINT_ENABLED
  if (endpoint_target == ENDPOINT_TARGET_VPI) return vpi_dbg_wb_read32(adr, data);
#endif
  return DBG_ERR_INVALID_ENDPOINT;
}


/* read a word from wishbone */
int dbg_wb_read32(uint32_t adr, uint32_t *data) {
#ifdef USB_ENDPOINT_ENABLED
  if (endpoint_target == ENDPOINT_TARGET_USB) return usb_dbg_wb_read32(adr, data);
#endif
#ifdef VPI_ENDPOINT_ENABLED
  if (endpoint_target == ENDPOINT_TARGET_VPI) return vpi_dbg_wb_read32(adr, data);
#endif
  return DBG_ERR_INVALID_ENDPOINT;
}

/* write a word to wishbone */
int dbg_wb_write8(uint32_t adr, uint8_t data) {
#ifdef USB_ENDPOINT_ENABLED
  if (endpoint_target == ENDPOINT_TARGET_USB) return usb_dbg_wb_write8( adr, data);
#endif
  return DBG_ERR_INVALID_ENDPOINT;
}

/* write a word to wishbone */
int dbg_wb_write32(uint32_t adr, uint32_t data) {
#ifdef USB_ENDPOINT_ENABLED
  if (endpoint_target == ENDPOINT_TARGET_USB) return usb_dbg_wb_write32( adr, data);
#endif
#ifdef VPI_ENDPOINT_ENABLED
  if (endpoint_target == ENDPOINT_TARGET_VPI) return vpi_dbg_wb_write32( adr, data);
#endif
  return DBG_ERR_INVALID_ENDPOINT;
}

/* read a block from wishbone */
int dbg_wb_read_block32(uint32_t adr, uint32_t *data, uint32_t len) {
#ifdef USB_ENDPOINT_ENABLED
  if (endpoint_target == ENDPOINT_TARGET_USB) return usb_dbg_wb_read_block32( adr, data, len);
#endif
#ifdef VPI_ENDPOINT_ENABLED
  if (endpoint_target == ENDPOINT_TARGET_VPI) return vpi_dbg_wb_read_block32( adr, data, len);
#endif
  return DBG_ERR_INVALID_ENDPOINT;
}

/* write a block to wishbone */
int dbg_wb_write_block32(uint32_t adr, uint32_t *data, uint32_t len) {
#ifdef USB_ENDPOINT_ENABLED
  if (endpoint_target == ENDPOINT_TARGET_USB) return usb_dbg_wb_write_block32( adr, data, len);
#endif
#ifdef VPI_ENDPOINT_ENABLED
  if (endpoint_target == ENDPOINT_TARGET_VPI) return vpi_dbg_wb_write_block32( adr, data, len);
#endif
  return DBG_ERR_INVALID_ENDPOINT;
}

/* read a register from cpu */
int dbg_cpu0_read(uint32_t adr, uint32_t *data, uint32_t length) {
#ifdef USB_ENDPOINT_ENABLED
  if (endpoint_target == ENDPOINT_TARGET_USB) return usb_dbg_cpu0_read( adr, data, length);
#endif
#ifdef VPI_ENDPOINT_ENABLED
  if (endpoint_target == ENDPOINT_TARGET_VPI) return vpi_dbg_cpu0_read( adr, data);
#endif
  return DBG_ERR_INVALID_ENDPOINT;
}

/* write a cpu register */
int dbg_cpu0_write(uint32_t adr, uint32_t *data, uint32_t length) {
#ifdef USB_ENDPOINT_ENABLED
  if (endpoint_target == ENDPOINT_TARGET_USB) return usb_dbg_cpu0_write( adr, data, length);
#endif
#ifdef VPI_ENDPOINT_ENABLED
  if (endpoint_target == ENDPOINT_TARGET_VPI) return vpi_dbg_cpu0_write( adr, data);
#endif
  return DBG_ERR_INVALID_ENDPOINT;
}

/* write a cpu module register */
int dbg_cpu0_write_ctrl(uint32_t adr, unsigned char data) {
#ifdef USB_ENDPOINT_ENABLED
  if (endpoint_target == ENDPOINT_TARGET_USB) return usb_dbg_cpu0_write_ctrl( adr, data);
#endif
#ifdef VPI_ENDPOINT_ENABLED
  if (endpoint_target == ENDPOINT_TARGET_VPI) return vpi_dbg_cpu0_write_ctrl( adr, data);
#endif
  return DBG_ERR_INVALID_ENDPOINT;
}


/* read a register from cpu module */
int dbg_cpu0_read_ctrl(uint32_t adr, unsigned char *data) {
#ifdef USB_ENDPOINT_ENABLED
  if (endpoint_target == ENDPOINT_TARGET_USB) return usb_dbg_cpu0_read_ctrl( adr, data);
#endif
#ifdef VPI_ENDPOINT_ENABLED
  if (endpoint_target == ENDPOINT_TARGET_VPI) return vpi_dbg_cpu0_read_ctrl( adr, data);
#endif
  return DBG_ERR_INVALID_ENDPOINT;
}


void test_sdram(void) {
	return;
}

// Close down gracefully when we receive any kill signals
void catch_sigint(int sig_num)
{
  // Close down any potentially open sockets and USB handles
#ifdef VPI_ENDPOINT_ENABLED
  if (vpi_fd) close(vpi_fd);
#endif
  if (server_fd) close(server_fd);
  gdb_close();
#ifdef USB_ENDPOINT_ENABLED
  usb_close_device_handle();
#endif
  printf("\nInterrupt signal received. Closing down connections and exiting\n\n");
  exit(0);
}

void print_usage()
{
  printf("Invalid or insufficient arguments\n");
  printf("\n");
  printf("OpenRISC GDB proxy server usage: or_debug_proxy -server_type port\n");
  printf("\n");
  printf("server_type:\n");
#ifdef USB_ENDPOINT_ENABLED
  printf("\t-r Start a server using RSP, connection to hadware target via\n\t   USB\n");
  printf("\t-j Start a server using legacy OR remote JTAG protocol, to\n\t   hardware target via USB\n");
#endif
#ifdef VPI_ENDPOINT_ENABLED
  printf("\t-v Start a server using RSP, connection to RTL sim. VPI server\n\t   target via sockets\n");
#endif
  printf("\n");
  printf("port:\n");
  printf("\tAny free port within the usable range of 0 - 65535\n");
  printf("\n");
  printf("Example:\n");
#ifdef USB_ENDPOINT_ENABLED
  printf("\tStart a GDB server on port 5555, using RSP, connecting to\n\thardware target via USB\n");
  printf("\tor_debug_proxy -r 5555\n");
  printf("\n");
#endif
#ifdef VPI_ENDPOINT_ENABLED
  printf("\tStart a GDB server on port 5555, using RSP, connecting to\n\trtl target via VPI\n");
  printf("\tor_debug_proxy -v 5555\n");
  printf("\n");
#endif
  fflush (stdout);
}
