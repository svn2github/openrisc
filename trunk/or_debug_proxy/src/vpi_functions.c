/*$$HEADER*/
/******************************************************************************/
/*                                                                            */
/*                    H E A D E R   I N F O R M A T I O N                     */
/*                                                                            */
/******************************************************************************/

// Project Name                   : OpenRISC Debug Proxy
// File Name                      : vpi_functions.c
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
// Implements communications with a Verilog RTL simulation via functions running
// in the simulator, attached via VPI.
//


/*$$CHANGE HISTORY*/
/******************************************************************************/
/*                                                                            */
/*                         C H A N G E  H I S T O R Y                         */
/*                                                                            */
/******************************************************************************/

// Date		Version	Description
//------------------------------------------------------------------------
// 081101		First revision           			jb

#include <assert.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <errno.h>
#include <netdb.h> 

#include "gdb.h"

#include "or_debug_proxy.h"

#include "vpi_functions.h"

// Definition of MSG_WAITALL -- appears to be missing from Cygwin's sys/socket.h
#ifdef CYGWIN_COMPILE
#define MSG_WAITALL 0
#endif

// socket stuff for connection to the VPI interface
int vpiPort = VPI_PORT;
int vpi_fd; // should be the descriptor for our connection to the VPI server

#ifdef RTL_SIM
#define DBG_VPI 1
#else
#define DBG_VPI 0
#endif

//Protocol ensuring synchronisation with the jp-rtl_sim program

// 1. jp-rtl_sim will first write a command byte
// 2. jp-rtl_sim will then send the address if it's a read or write
//    or the data to write if it's a dbg_cpu_wr_ctrl (stall & reset bits)
// 3. will then send data if we're writing and we sent address in 2
// 4. wait for response from vpi functions

// commands:
// 4'h1 jtag set instruction register (input: instruction value)
// 4'h2 set debug chain (dbg_set_command here) (input: chain value)
// 4'h3 cpu_ctrl_wr (input: ctrl value (2 bits))
// 4'h4 cpu_ctrl_rd (output: ctrl value (2bits))
// 4'h5 cpu wr reg (inputs: address, data)
// 4'h6 cpu rd reg (input: address; output: data)
// 4'h7 wb wr 32 (inputs: address, data)
// 4'h8 wb rd 32 (input: address; output: data)
// 4'h9 wb wr block 32 (inputs: address, length, data)
// 4'ha wb rd block 32 (inputs: address, length; output: data)
// 4'hb reset
// 4'hc read jtag id (output: data)

#define CMD_JTAG_SET_IR 0x1
#define CMD_SET_DEBUG_CHAIN 0x2
#define CMD_CPU_CTRL_WR 0x3
#define CMD_CPU_CTRL_RD 0x4
#define CMD_CPU_WR_REG 0x5
#define CMD_CPU_RD_REG 0x6
#define CMD_WB_WR32 0x7
#define CMD_WB_RD32 0x8
#define CMD_WB_BLOCK_WR32 0x9
#define CMD_WB_BLOCK_RD32 0xa
#define CMD_RESET 0xb
#define CMD_READ_JTAG_ID 0xc


void send_command_to_vpi(char CMD)
{
  // first thing we do  is send a command
  // and wait for an ack
  uint32_t n;
  char cmd_resp;

  n = write(vpi_fd,&CMD, 1); // send the command to the sim

  if (n < 0)   perror("ERROR writing to VPI socket");
  
  n = recv(vpi_fd,&cmd_resp,1, MSG_WAITALL); // block and wait for the ack
  
  if (n < 0)   perror("ERROR Reading from VPI socket");
  
  if (cmd_resp != CMD) perror("Response from RTL sim incorrect"); //check it acked with cmd

  return;
}

void send_address_to_vpi(uint32_t address)
{
  // Now send address
  uint32_t n;

  char* send_buf;

  address = htonl(address);

  send_buf = (char *) &address;
  
  n = write(vpi_fd,send_buf, 4); // send the address to the sim
  
  if (n < 0)   perror("ERROR writing to VPI socket");
  
  return;
}

void send_data_to_vpi(uint32_t data)
{
  // Now send data
  uint32_t n;

  data = htonl(data);

  char* send_buf;

  send_buf = (char *) &data;
  
  n = write(vpi_fd,send_buf, 4); // Write the data to the socket
  
  if (n < 0)   perror("ERROR writing to VPI socket");
  
  return;
  
}

void send_block_data_to_vpi(uint32_t len, uint32_t *data)
{
  // Now send data
  uint32_t n, i;
  
  for (i = 0; i < (len/4); i++)
    data[i] = htonl(data[i]);
  
  char* send_buf;
  
  send_buf = (char *) data;
  
  n = write(vpi_fd,send_buf, len); // Write the data to the socket
  
  if (n < 0)   perror("ERROR writing to VPI socket");
  
  return;
  
}

void get_data_from_vpi(uint32_t* data)
{
  
  uint32_t n;
  
  uint32_t inc_data;
  
  char* recv_buf;
  
  recv_buf = (char*) &inc_data;
  
  n = recv(vpi_fd,recv_buf,4, MSG_WAITALL); // block and wait for the data
  
  if (n < 0)   perror("ERROR Reading from VPI socket");
  
  inc_data = ntohl(inc_data);

  if (DBG_VPI) printf("get_data_from_vpi: 0x%.8x\n",inc_data);

  *data = inc_data;
  
  return;

}

void get_block_data_from_vpi(uint32_t len, uint32_t* data)
{
  uint32_t n, i;
  
  char* recv_buf;

  recv_buf = (char *) data;
  
  n = recv(vpi_fd, recv_buf, len, MSG_WAITALL); // block and wait for the data
  
  if (n < 0)   perror("ERROR Reading from VPI socket");
    
  // re-order host compliant style the recieved words
  if (DBG_VPI) printf("get_block_data_from_vpi: %d bytes",len);
  for (i = 0;i < (len/4); i++)
    {
      data[i] = ntohl(data[i]);

      if (DBG_VPI) printf("0x%.8x ",data[i]);
    }
  if (DBG_VPI) printf("\n");
  
  return;

}

void get_response_from_vpi()
{
  // Basically just wait for the response from VPI
  // by blocking wait on recv
  
  uint32_t n;
  char tmp;
  
  n = recv(vpi_fd,&tmp,1, MSG_WAITALL); // block and wait
  
  if (n < 0)   perror("ERROR Reading from VPI socket");
  
  return;
}

/* Resets JTAG
   Writes TRST=0
   and    TRST=1 */
static void jp2_reset_JTAG() {
  
  debug2("\nreset(");

  send_command_to_vpi(CMD_RESET);

  get_response_from_vpi();
  
  debug2(")\n");

}


int vpi_connect() {
    int sockfd;
    struct sockaddr_in serv_addr;
    struct hostent *server;   
    socklen_t flags;
    char sTemp[256];

    debug2("Started vpi_connect()\n");
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    //sockfd = socket(PF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) {
      perror("ERROR opening socket");
      goto socket_setup_exit;
    }

    server = gethostbyname("localhost");

    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
	goto socket_setup_exit;
    }

    /*
    if(fcntl(sockfd, F_GETFL, &flags) < 0) {
      sprintf(sTemp, "Unable to get flags for socket %d", sockfd);
      perror(sTemp);
      close(sockfd);
      goto socket_setup_exit;
    }

    // Set the nonblocking flag
    if(fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) < 0) {
      sprintf(sTemp, "Unable to set flags for socket %d to value 0x%08x", 
	      sockfd, flags | O_NONBLOCK);
      perror(sTemp);
      close(sockfd);
      goto socket_setup_exit;
    }
    */
    
    bzero((char *) &serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;

    bcopy(server->h_addr,(char *)&serv_addr.sin_addr.s_addr,server->h_length);

    serv_addr.sin_port = htons(vpiPort);

    printf("Initialising connection with simulation\n");
    
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
      {
        perror("Error connecting");
	goto socket_setup_exit;
      }

    return sockfd;

 socket_setup_exit:
    if (vpi_fd) close(vpi_fd);    
    exit(1);

}

/* Resets JTAG, and sets DEBUG scan chain */
 int vpi_dbg_reset() {
   
   uint32_t id;
   
   jp2_reset_JTAG();

  /* set idcode jtag chain */
  send_command_to_vpi(CMD_JTAG_SET_IR);

  send_data_to_vpi(JI_IDCODE);

  get_response_from_vpi();

  /* now read out the jtag id */
  send_command_to_vpi(CMD_READ_JTAG_ID);
  
  //id = get_data_from_vpi();  
  get_data_from_vpi((uint32_t *)&id);  
  
  get_response_from_vpi();
 
  printf("JTAG ID = %08x\n", (unsigned int) id);

  /* now set the chain to debug */
  send_command_to_vpi(CMD_JTAG_SET_IR);
  
  send_data_to_vpi(JI_DEBUG);
  
  get_response_from_vpi();

  current_chain = -1;
  return DBG_ERR_OK;
}

void vpi_dbg_test() {

  uint32_t npc, ppc, r1;
  unsigned char stalled;
  
  printf("  Stall or1k\n");
  dbg_cpu0_write_ctrl(0, 0x01);      // stall or1k

  dbg_cpu0_read_ctrl(0, &stalled);
  if (!(stalled & 0x1)) {
    printf("  or1k stall failed. Exiting\n");   // check stall or1k
    if (vpi_fd) close(vpi_fd);    
    exit(1);
  }
  
  debug2("  Reading npc\n");
  dbg_cpu0_read((0 << 11) + 16, &npc); 
  debug2("  Reading ppc\n");
  dbg_cpu0_read((0 << 11) + 18, &ppc); 
  debug2("  Reading r1\n");
  dbg_cpu0_read(0x401, &r1);
  printf("  Read      npc = %.8x ppc = %.8x r1 = %.8x\n", npc, ppc, r1);
    
}

/* Sets scan chain.  */
int vpi_dbg_set_chain(uint32_t chain) {
  //uint32_t status, crc_generated, crc_read;
  
  if (current_chain == chain)
    return DBG_ERR_OK;
  
  dbg_chain = chain;
  
  send_command_to_vpi(CMD_SET_DEBUG_CHAIN);
  
  send_data_to_vpi(chain);
  
  get_response_from_vpi();
  
  current_chain = chain;
  
  return DBG_ERR_OK;
}

/* writes a ctrl reg */
int vpi_dbg_ctrl(uint32_t reset, uint32_t stall) 
{
  
  debug("\n");
  debug2("ctrl\n");
  
  vpi_dbg_set_chain(dbg_chain);
  
  send_command_to_vpi(CMD_CPU_CTRL_WR);
  
  //send_data_to_vpi(((reset & 0x1) | ((stall&0x1)<<1)));
  send_data_to_vpi(((stall & 0x1) | ((reset&0x1)<<1)));
  
  get_response_from_vpi();

  return DBG_ERR_OK;
}

/* reads control register */
int vpi_dbg_ctrl_read(uint32_t *reset, uint32_t *stall) 
{
  
  uint32_t resp;
  
  vpi_dbg_set_chain(dbg_chain);
    
  debug("\n");
  debug2("ctrl\n");
  
  vpi_dbg_set_chain(dbg_chain);
  
  send_command_to_vpi(CMD_CPU_CTRL_RD);
  
  get_data_from_vpi((uint32_t *)&resp);
  
  if (DBG_VPI) printf(" dbg_ctrl_read: 0x%.8x\n",resp);

  get_response_from_vpi();
  
  *reset = (resp & 0x00000001);

  *stall = ((resp >> 1) & 0x00000001);
  
  return DBG_ERR_OK;
}

/* read a word from wishbone */
int vpi_dbg_wb_read32(uint32_t adr, uint32_t *data) 
{
  //uint32_t resp;

  vpi_dbg_set_chain(DC_WISHBONE);
  
  send_command_to_vpi(CMD_WB_RD32);
  
  send_address_to_vpi(adr);
  
  get_data_from_vpi(data);
  
  get_response_from_vpi();
  
  return 0;
}

/* write a word to wishbone */
int vpi_dbg_wb_write32(uint32_t adr, uint32_t data) 
{

  vpi_dbg_set_chain(DC_WISHBONE);
  
  send_command_to_vpi(CMD_WB_WR32);
  
  send_address_to_vpi(adr);

  send_data_to_vpi(data);
  
  get_response_from_vpi();
  
  return 0;
} 

/* read a block from wishbone */
int vpi_dbg_wb_read_block32(uint32_t adr, uint32_t *data, uint32_t len) 
{
  
  // len is in B Y T E S ! !

  if (DBG_VPI) printf("block read len: %d from addr: 0x%.8x\n",len, adr);

  vpi_dbg_set_chain(DC_WISHBONE);
  
  send_command_to_vpi(CMD_WB_BLOCK_RD32);

  send_data_to_vpi(adr);

  send_data_to_vpi(len);

  get_block_data_from_vpi(len, data);

  get_response_from_vpi();
      
  return DBG_ERR_OK;
}

/* write a block to wishbone */
int vpi_dbg_wb_write_block32(uint32_t adr, uint32_t *data, uint32_t len) 
{
  
  vpi_dbg_set_chain(DC_WISHBONE);
  
  send_command_to_vpi(CMD_WB_BLOCK_WR32);

  send_data_to_vpi(adr);
  
  send_data_to_vpi(len);

  send_block_data_to_vpi(len, data);

  get_response_from_vpi();

  return DBG_ERR_OK;
}

/* read a register from cpu */
int vpi_dbg_cpu0_read(uint32_t adr, uint32_t *data) 
{
  
  vpi_dbg_set_chain(DC_CPU0);
  
  send_command_to_vpi(CMD_CPU_RD_REG);
  
  send_address_to_vpi(adr);
  
  get_data_from_vpi(data);
  
  get_response_from_vpi();
  
  return 0;

}

/* write a cpu register */
int vpi_dbg_cpu0_write(uint32_t adr, uint32_t data) 
{

  vpi_dbg_set_chain(DC_CPU0);
  
  send_command_to_vpi(CMD_CPU_WR_REG);
  
  send_address_to_vpi(adr);
  
  send_data_to_vpi(data);
  
  get_response_from_vpi();
  
  return 0;
}


/* write a cpu module register */
int vpi_dbg_cpu0_write_ctrl(uint32_t adr, unsigned char data) {
  uint32_t err;
  if ((err = vpi_dbg_set_chain(DC_CPU0))) return err;
  if ((err = vpi_dbg_ctrl(data & 2, data &1))) return err;
  return DBG_ERR_OK;
}

/* read a register from cpu module */
int vpi_dbg_cpu0_read_ctrl(uint32_t adr, unsigned char *data) {
  uint32_t err;
  uint32_t r, s;
  if ((err = vpi_dbg_set_chain(DC_CPU0))) return err;
  if ((err = vpi_dbg_ctrl_read(&r, &s))) return err;
  *data = (r << 1) | s;
  return DBG_ERR_OK;
}
