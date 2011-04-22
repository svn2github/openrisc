/*$$HEADER*/
/******************************************************************************/
/*                                                                            */
/*                    H E A D E R   I N F O R M A T I O N                     */
/*                                                                            */
/******************************************************************************/

// Project Name                   : OpenRISC Debug Proxy
// File Name                      : usb_functions.c
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
// Code calling the FT2232 JTAG MPSSE driver functions. Does a majority of the 
// fiddly work when interfacing to the system via its debug module.
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
// 100408		Fixed up retries                                jb

#include <assert.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <netinet/in.h>

#ifdef CYGWIN_COMPILE
#include <windows.h>
#include "win_FTCJTAG.h"
#else // We're on Linux or Mac OS X
#include "WinTypes.h" // We still use things from here in this file - TODO: remove this dependency
#include "FT2232cMpsseJtag.h"
#endif

#include "gdb.h"

#include "usb_functions.h"

#include "or_debug_proxy.h"

#include "usb_driver_calls.h"

// Global JTAG signals for reading/writing
DWORD dwNumBytesReturned = 0;

/* Crc of current read or written data.  */
uint32_t crc_r, crc_w = 0;

/* Number of retries for a command */
uint32_t retries;

/* Generates new crc, sending in new bit input_bit */
uint32_t crc_calc(uint32_t crc, uint32_t input_bit) {
	uint32_t d = (input_bit) ? 0xfffffff : 0x0000000;
	uint32_t crc_32 = ((crc >> 31)&1) ? 0xfffffff : 0x0000000;
	crc <<= 1;
	return crc ^ ((d ^ crc_32) & DBG_CRC_POLY);
}

// Speedy bit reversing algorithms for base2 lengths
// Thanks to: http://aggregate.org/MAGIC/#Bit%20Reversal
uint32_t bit_reverse_swar_2(uint32_t x)
{
  x=(((x&0xaaaaaaaa)>>1)|((x&0x55555555)<<1));
  return x;
}
uint32_t bit_reverse_swar_4(uint32_t x)
{
  x=(((x&0xaaaaaaaa)>>1)|((x&0x55555555)<<1));
  x=(((x&0xcccccccc)>>2)|((x&0x33333333)<<2));
  return x;
}
static inline uint32_t bit_reverse_swar_8(uint32_t x)
{
  x=(((x&0xaaaaaaaa)>>1)|((x&0x55555555)<<1));
  x=(((x&0xcccccccc)>>2)|((x&0x33333333)<<2));
  x=(((x&0xf0f0f0f0)>>4)|((x&0x0f0f0f0f)<<4));
  return x;
}
uint32_t bit_reverse_swar_16(uint32_t x)
{
  x=(((x&0xaaaaaaaa)>>1)|((x&0x55555555)<<1));
  x=(((x&0xcccccccc)>>2)|((x&0x33333333)<<2));
  x=(((x&0xf0f0f0f0)>>4)|((x&0x0f0f0f0f)<<4));
  x=(((x&0xff00ff00)>>8)|((x&0x00ff00ff)<<8));
  return x;
}
uint32_t bit_reverse_swar_32(uint32_t x)
{
  x=(((x&0xaaaaaaaa)>>1)|((x&0x55555555)<<1));
  x=(((x&0xcccccccc)>>2)|((x&0x33333333)<<2));
  x=(((x&0xf0f0f0f0)>>4)|((x&0x0f0f0f0f)<<4));
  x=(((x&0xff00ff00)>>8)|((x&0x00ff00ff)<<8));
  x=(((x&0xffff0000)>>16)|((x&0x0000ffff)<<16)); // We could be on 64-bit arch!
  return x;
}
  
uint32_t bit_reverse_data(uint32_t data, uint32_t length){
  if (length == 2) return bit_reverse_swar_2(data);
  if (length == 4) return bit_reverse_swar_4(data);
  if (length == 8) return bit_reverse_swar_8(data);
  if (length == 16) return bit_reverse_swar_16(data);
  if (length == 32) return bit_reverse_swar_32(data);
  // Long and laborious way - hopefully never gets called anymore!
  uint32_t i,reverse=0;
  for (i=0;i<length;i++) reverse |= (((data>>i)&1)<<(length-1-i));
  return reverse;
}
// Constants that are used a lot, and were 5 bits, so might as well 
// precalculate them to save reversing them each time.
// These are from or_debug_proxy.h, so if the original values change these
// should be recalculated!!
const uint8_t DI_GO_5BITREVERSED = 0x00;
const uint8_t DI_READ_CMD_5BITREVERSED = 0x10;
const uint8_t DI_WRITE_CMD_5BITREVERSED = 0x08;
const uint8_t DI_READ_CTRL_5BITREVERSED = 0x18;
const uint8_t DI_WRITE_CTRL_5BITREVERSED = 0x04;


void usb_dbg_test() {
  
  uint32_t npc, ppc, r1; 
  unsigned char stalled;
  uint32_t zero = 0;

  printf("Stalling OR1K CPU0\n");
  err = dbg_cpu0_write_ctrl(0, 0x01);      // stall or1k
  
  err = dbg_cpu0_read_ctrl(0, &stalled);
  if (!(stalled & 0x1)) {
    printf("OR1K CPU0 should be stalled\n");   // check stall or1k
    exit(1);
  }

  if (err)
    {
      printf("Error %d occured when attempting to stall CPU\n", err);
      goto do_exit;
    }
  
  /* Clear Debug Reason Register (DRR) 0x3015 */
  if (!err)
	  err = dbg_cpu0_write((6 << 11) + 21, &zero, 4);
  if (err)
  {
	  printf("Error %d occured when writing CPU DRR register\n",err);
	  goto do_exit;
	  
  }
  if (!err)
	  err = dbg_cpu0_read((0 << 11) + 16, &npc, 4);  /* Read NPC */
  if (err)
  {
	  printf("Error %d occured when reading CPU NPC\n",err);
	  goto do_exit;
	  
  }
  if (!err)
	  err = dbg_cpu0_read((0 << 11) + 18, &ppc, 4);  /* Read PPC */
  if (err)
  {
	  printf("Error %d occured when reading CPU PPC\n",err);
	  goto do_exit;
	  
  }
  if (!err)
	  err = dbg_cpu0_read(0x401, &r1, 4);  /* Read R1 */
  if (err)
  {
	  printf("Error %d occured when reading CPU GPR1\n",err);
	  goto do_exit;
	  
  }

  printf("Read      npc = %.8x ppc = %.8x r1 = %.8x\n", npc, ppc, r1);
  
  /*
  // Memory test - attempt to read and write massive arrays
  char biggest_array[65000];
  int i;
  printf("Testing 65000 byte array write\n");
  printf("Filling array...\n");
  for(i=0;i<65000;i++) biggest_array[i] = i;
  printf("Writing array\n");
  err = usb_dbg_wb_write_block32(0, (uint32_t *)biggest_array, 65000);
  printf("err = %d\n",err); 
  */
  return;

do_exit:
  printf("Exiting\n");
  FT2232_USB_JTAG_CloseDevice();
  exit(1);
}



/*---------------------------------------------------------------------------*/
/*!Write up to 32-bits to the JTAG bus via the USB device

Write up to 32-bits to the JTAG bus.

@param[in] stream  This should store the data to be written to the bus
@param[in] num_bits  Number of bits to write on the JTAG bus
@param[in] dwTapControllerState  State to leave the JTAG TAP in when done    */
/*---------------------------------------------------------------------------*/
void usb_write_stream (uint32_t stream, uint32_t num_bits, DWORD dwTapControllerState){
  FTC_STATUS Status = FTC_SUCCESS;
  uint32_t i,num_bytes;
  WriteDataByteBuffer WriteDataBuffer;
  
  if ((num_bits/8)>0)num_bytes=(num_bits/8);
  else num_bytes=1;
  // First clear the buffer for the amount of data we're shifting in
  // Bytewise copy the stream into WriteDataBuffer, LSB first

  // This means if we want to send things MSb first, we need to pass them
  // to this function wrapped with a call to bit_reverse_data(data,length)

  for (i=0; i<(num_bytes)+1;i++)
    WriteDataBuffer[i] = ((stream >>(8*i)) & (0xff));
  
  // Now generate the CRC for what we're sending
  // data should be presented MSb first (at bit0)
  for (i=0;i<num_bits;i++)
    crc_w = crc_calc(crc_w,((stream>>i)&1));
  
  
  if (DEBUG_USB_DRVR_FUNCS) 
    {
      printf("\nusb_write_stream: num_bytes=%d, WriteDataBuffer contents for %d bits:",num_bytes,num_bits);
      for (i=0;i<num_bytes+1;i++)
	printf("%x",WriteDataBuffer[num_bytes-i]);
      printf("\n");
    }  

  //Status = pFT2232cMpsseJtag->JTAG_WriteDataToExternalDevice(ftHandle, false, num_bits, 
  //		       &WriteDataBuffer, num_bytes, dwTapControllerState);
  // Platform independant function call
  Status = FT2232_USB_JTAG_WriteDataToExternalDevice(false, num_bits, 
  		       &WriteDataBuffer, num_bytes, dwTapControllerState);

  if (Status != FTC_SUCCESS)
    printf("Write to USB device failed: status code: %ld\n",Status);
    
}

/*---------------------------------------------------------------------------*/
/*!Read up to 32-bits off the JTAG bus via the USB device

The return value of this should remain uint32_t as we're returning all the 
data in a signal variable. We never need more than 32-bits in a single read
when using this function.

@param[in] num_bits  Number of bits to read off from the USB
@param[in] dwTapControllerState  State to leave the JTAG TAP in when done
@return: The data read from the USB device                                   */
/*---------------------------------------------------------------------------*/
uint32_t usb_read_stream(uint32_t num_bits, DWORD dwTapControllerState){
  ReadDataByteBuffer ReadDataBuffer;
  FTC_STATUS Status = FTC_SUCCESS;
  uint32_t returnVal =0;
  uint32_t i;

  // Platform independant driver call
  Status = FT2232_USB_JTAG_ReadDataFromExternalDevice(false, num_bits, &ReadDataBuffer, &dwNumBytesReturned, dwTapControllerState);

  if (DEBUG_USB_DRVR_FUNCS) 
    printf("usb_read_stream: returned Status: 0x%lx from reading %u bits, \n",
	   Status,num_bits);

  if (Status == FTC_SUCCESS){    
    
    for(i=0;i<num_bits;i++){
      returnVal |= ((ReadDataBuffer[i/8]>>(i%8))&0x1)<<(num_bits-1-i);
    }
  }
  // Generate the CRC for read
  for (i=0;i<num_bits;i++)
      crc_r = crc_calc(crc_r, ((returnVal>>(num_bits-1-i))&1)); 

  return returnVal;
}

/* Sets TAP instruction register */
void usb_set_tap_ir(uint32_t ir) {

  WriteDataByteBuffer WriteDataBuffer;
  FTC_STATUS Status = FTC_SUCCESS;
  
  WriteDataBuffer[0] = ir;

  // Using global ftHandle, writing to TAP instruction register 4 bits, 
  //Status = pFT2232cMpsseJtag->JTAG_WriteDataToExternalDevice(ftHandle, true, 
  //JI_SIZE, &WriteDataBuffer, 1, RUN_TEST_IDLE_STATE);
  // Platform independant driver call

  Status = FT2232_USB_JTAG_WriteDataToExternalDevice(true, JI_SIZE, &WriteDataBuffer, 1, RUN_TEST_IDLE_STATE);

  if (DEBUG_USB_DRVR_FUNCS) 
    printf("USB JTAG write of %x to instruction register returned Status: 0x%lx\n",
	   ir, Status);
  current_chain = -1;
}

static uint32_t id_read_at_reset = 0;

/* Resets JTAG, and sets DEBUG scan chain */
int usb_dbg_reset() {

  // uint32_t err;
  uint32_t id;
  uint32_t reinit_count=0;
  int err;
 retry_jtag_init:
  err = init_usb_jtag();
  if (err)
  {
#ifdef DEBUG_USB_DRVR_FUNCS
	  printf("init_usb_jtag error %d\n",err);
#endif
	  return DBG_ERR_CRC;
  }
  
  // Set ID code instruction in IR
  usb_set_tap_ir(JI_IDCODE);
  
  // Now read out the IDCODE for the device
  id = usb_read_stream(32, RUN_TEST_IDLE_STATE);
  
  // if read ID was rubbish retry init - this is probably NOT the best way to do this...
  if ((id == 0xffffffff) | (id == 0x00000002) | (id == 0x00000000)) { 
    // Platform independant driver call
    FT2232_USB_JTAG_CloseDevice();
    if (reinit_count++ > 4){
      printf("JTAG TAP ID read error. Unable to detect TAP controller. \nPlease ensure debugger is connected to target correctly.\n");
      exit(1);
    }
    goto retry_jtag_init;
  }
  
  printf("JTAG ID = %08x\n", id & 0xffffffff);
  
  /* select debug scan chain and stay in it forever */
  usb_set_tap_ir(JI_DEBUG);

  id_read_at_reset = id; // Store this to check later if there's errors
  
  current_chain = -1;
  return DBG_ERR_OK;
}

static void reset_tap(void)
{
  uint32_t id = 0;
  reinit_usb_jtag();
  
  
  while (id != id_read_at_reset)
    {
      usb_set_tap_ir(JI_IDCODE);
      id = usb_read_stream(32, RUN_TEST_IDLE_STATE);
      //printf("reset_tap: read ID %.8x\n",id);
    }
  //Return the chain to DEBUG mode
  usb_set_tap_ir(JI_DEBUG);
  
}


/* counts retries and returns zero if we should abort */
static int retry_no = 0;
int retry_do() {

  unsigned char stalled;
  int tap_id_reads = 0;

  //printf("RETRY\n");
  if (retry_no == 0)
    printf("Communication error. Retrying\n");

  retry_no++;
  
  // Odds are if we're having a communication problem, we should 
  // just reconnect to the processor. It's probably just been reset.
  /*
    FT2232_USB_JTAG_CloseDevice();// Close the device
    
    if (init_usb_jtag() > 0)
    {
    if (retry_no >= NUM_HARD_RETRIES)
    return 1;
    else
    return 0;
    }
  */
  
  // Try a readback of the TAP ID
 read_tap:
  // Set ID code instruction in IR
  usb_set_tap_ir(JI_IDCODE);
  
  // Now read out the IDCODE for the device
  uint32_t id = usb_read_stream(32, RUN_TEST_IDLE_STATE);

  //Return the chain to DEBUG mode
  usb_set_tap_ir(JI_DEBUG);

  if((id&0xffffffff) != (id_read_at_reset&0xffffffff))
    {
      if (tap_id_reads == 10)
	{
	  // Pretty big problem - can't even read the ID of the TAP anymore
	  // So return error
	  printf("Unable to read JTAG TAP ID - read %08x, expected %08x\n", 
		 id, id_read_at_reset);

	  return 1;
	}
      
      tap_id_reads++;
      
      goto read_tap;
    }

  current_chain = -1;
  
  if (retry_no == 1)
    sleep(1);

  else if ( retry_no < NUM_SOFT_RETRIES)
    return 0; // Just attempt again    
  
  if ((retry_no >= NUM_SOFT_RETRIES) && (retry_no < NUM_SOFT_RETRIES + NUM_HARD_RETRIES) )
      {
	// Attempt a stall of the processor and then we'll try again
	//usb_dbg_test();
	printf("Resetting or1k\n");
	err = dbg_cpu0_write_ctrl(0, 0x02);      // reset or1k
	
	printf("Stalling or1k\n");
	err = dbg_cpu0_write_ctrl(0, 0x01);      // stall or1k
	
	err = dbg_cpu0_read_ctrl(0, &stalled);
	if (!(stalled & 0x1)) {
	  printf("or1k should be stalled\n");   // check stall or1k
	  FT2232_USB_JTAG_CloseDevice();// Close the device
	  exit(1);
	}

	//retry_no++;
	return 0;
      }
    else // Unable to get the processor going again, return 1
      return 1;

}

/* resets retry counter */
void retry_ok() {
	retry_no = 0;
}

/* Sets scan chain.  */
int usb_dbg_set_chain(int chain) {
  uint32_t status, crc_generated, crc_read;
  dbg_chain = chain;  
  
 try_again:
  if (current_chain == chain) return DBG_ERR_OK;
  current_chain = -1;
  if (DEBUG_CMDS) printf("\n");
  if (DEBUG_CMDS) printf("set_chain %i\n", chain);
  
  crc_w = 0xffffffff; // reset crc write variable

  // CRC generated in usb_read/write_stream functions

  usb_write_stream((((bit_reverse_data(chain,DBGCHAIN_SIZE) & 0xf) << 1) | 1),
  		   DBGCHAIN_SIZE+1 , PAUSE_TEST_DATA_REGISTER_STATE);  
  
  usb_write_stream(bit_reverse_data(crc_w, DBG_CRC_SIZE),DBG_CRC_SIZE,
		   PAUSE_TEST_DATA_REGISTER_STATE);
  
  crc_r = 0xffffffff; // reset the crc_r variable
  
  status = (usb_read_stream(DC_STATUS_SIZE, 
			    PAUSE_TEST_DATA_REGISTER_STATE) & 0xf);
  
  crc_generated = crc_r;

  crc_read = usb_read_stream(DBG_CRC_SIZE, RUN_TEST_IDLE_STATE);
  //printf("%x %x %x\n",status, crc_generated, crc_read);

  /* CRCs must match, otherwise retry */
  if (crc_read != crc_generated) {
     if (!retry_do()) goto try_again;
    else return DBG_ERR_CRC;
  }
  /* we should read expected status value, otherwise retry */
  if (status != 0) {
    if (!retry_do()) goto try_again;
    else return status;
  }
  
  /* reset retry counter */
  retry_ok();
  
  current_chain = chain;
  return DBG_ERR_OK;
}



/* sends out a command with 32bit address and 16bit length, if len >= 0 */
int usb_dbg_command(uint32_t type, uint32_t adr, uint32_t len) {
  uint32_t i,status, crc_generated, crc_read;

   // JTAG driver things
  FTC_STATUS Status = FTC_SUCCESS;
  WriteDataByteBuffer WriteDataBuffer;
  ReadDataByteBuffer ReadDataBuffer;
  retries = 0;
 try_again:
  usb_dbg_set_chain(dbg_chain);
  if (DEBUG_CMDS) printf("\n");
  if (DEBUG_CMDS) printf("comm %d %d %8x \n", type,len,adr);

  //Calculate CRCs first
  crc_w = 0xffffffff;

  for (i=0;i<DBGCHAIN_SIZE+1;i++)
    crc_w = crc_calc(crc_w, (DI_WRITE_CMD_5BITREVERSED >> i)  & 1);
    //crc_w = crc_calc(crc_w,
    //	     ((bit_reverse_data((DI_WRITE_CMD & 0xf),DBGCHAIN_SIZE+1))>>i)&1);
  
  for (i=0;i<4;i++)
    crc_w = crc_calc(crc_w,((bit_reverse_data(type,4))>>i)&1);
  
  for (i=0;i<32;i++)
    crc_w = crc_calc(crc_w,((bit_reverse_data(adr,32))>>i)&1);
  
  assert(len > 0);
  for (i=0;i<16;i++)
    crc_w = crc_calc(crc_w,((bit_reverse_data(len-1,16))>>i)&1);
  
  
  
  // Now pack the write data buffer
  // 1-bit 0, 4-bits cmd, 4-bit type, 32-bit adr, 32-bit CRC
  // [0]
  //bits 0-4
  WriteDataBuffer[0]=(DI_WRITE_CMD_5BITREVERSED);
  //bits 5-7 
  WriteDataBuffer[0] |= ((bit_reverse_data(type,4)&0x7)<<5);
  // [1]
  // bit 0 - last bit of type
  WriteDataBuffer[1] = ((bit_reverse_data(type,4)&0x08)>>3);
  //bits 1-7 - first 7 bits of adr
  WriteDataBuffer[1] |= ((bit_reverse_data(adr,32)&0x07f)<<1);
  //[2-4] 24 bits of adr
  for(i=0;i<3;i++)
    WriteDataBuffer[2+i] = (bit_reverse_data(adr,32)>>(7+(i*8)))&0xff;
  // [5] last bit of adr in bit 0, first 7 bits of len-1 follow
  WriteDataBuffer[5] = (bit_reverse_data(adr,32)>>31)&1;
  WriteDataBuffer[5] |= (bit_reverse_data(len-1,16)&0x7f)<<1;
  // [6] bits 7-14 of (len-1)
  WriteDataBuffer[6] = (bit_reverse_data(len-1,16)>>7)&0xff;
  // [7] - last bit of len-1 and first 7 bits of the CRC
  WriteDataBuffer[7] = (bit_reverse_data(len-1,16)>>15)&1;
  //Reverse the CRC first
  crc_w = bit_reverse_data(crc_w, DBG_CRC_SIZE);
  WriteDataBuffer[7] |= (crc_w&0x7f)<<1;
  //[8-10] next 24 bits (7-30) of crc_w
  WriteDataBuffer[8] = (crc_w>>7)&0xff;
  WriteDataBuffer[9] = (crc_w>>15)&0xff;
  WriteDataBuffer[10] = (crc_w>>23)&0xff;
  //[11] final bit of crc_w
  WriteDataBuffer[11] = (crc_w>>31)&1;
  
  //  Status = pFT2232cMpsseJtag->JTAG_WriteReadDataToFromExternalDevice(gFtHandle,false,89+4+32 , &WriteDataBuffer, 16, &ReadDataBuffer, &dwNumBytesReturned, RUN_TEST_IDLE_STATE);
  // Platform independant driver call
  Status = FT2232_USB_JTAG_WriteReadDataToFromExternalDevice(false,89+4+32 , &WriteDataBuffer, 16, &ReadDataBuffer, &dwNumBytesReturned, RUN_TEST_IDLE_STATE);
  
  if (Status != FTC_SUCCESS)
    printf("USB write fail - code %ld\b",Status);
  
  // Now look through the read data
  
  // From bit1 of ReadDataBuffer[11] we should have our 4-bit status
  status = (ReadDataBuffer[11] >> 1) & 0xf;
  
  // Now extract the received CRC
  crc_read = 0;
  //first 3 bits (0-2)
  crc_read |= (ReadDataBuffer[11] >> 5) & 0x7;
  // middle 3 bytes (bits 3 to 26)
  for (i=0;i<3;i++)
    crc_read |= ((ReadDataBuffer[12+i]&0xff) << ((i*8)+3));
  // last 5 bits from ReadDataBuffer[15]
  crc_read |= (ReadDataBuffer[15]&0x1f)<<27;
  
  // Now calculate CRC on status
  crc_r = 0xffffffff;
  for(i=0;i<DC_STATUS_SIZE;i++)
    crc_r = crc_calc(crc_r, (status>>i)&1);
  
  crc_generated = crc_r;
  // Now bit reverse status and crc_read as we unpacked them
  // with the MSb going to the LSb
  status = bit_reverse_data(status, DC_STATUS_SIZE);
  crc_read = bit_reverse_data(crc_read, DBG_CRC_SIZE);	  
  
  //printf("%x %x %x\n", status, crc_read, crc_generated);
  /* CRCs must match, otherwise retry */
  //uint32_t tries = 0;
  if (crc_read != crc_generated) {
    retries++;
    if (retries < 2)
      {
	if (DEBUG_USB_DRVR_FUNCS)
	  printf("usb_functions - usb_dbg_command - CRC fail. Going again\n");
	goto try_again;
      }
    else if (retries < 8)
      {
	reset_tap();
	goto try_again;
      }
    else return DBG_ERR_CRC;
  }
  /* we should read expected status value, otherwise retry */
  if (status != 0) {
    if (retries < 2)
      {
	if (DEBUG_USB_DRVR_FUNCS)
	  printf("usb_functions - usb_dbg_command - bad status (%d). Going again\n",status);
	goto try_again;
      }
    else if (retries < 8)
      {
	reset_tap();
	goto try_again;
      }
    else return status;
    
  }
  /* reset retry counter */
  retry_ok();

  return DBG_ERR_OK;
}


/* writes a ctrl reg */
int usb_dbg_ctrl(uint32_t reset, uint32_t stall) {
  uint32_t i,status, crc_generated, crc_read;
  // JTAG driver things
  FTC_STATUS Status = FTC_SUCCESS;
  WriteDataByteBuffer WriteDataBuffer;
  ReadDataByteBuffer ReadDataBuffer;
  retries = 0;
try_again:
  usb_dbg_set_chain(dbg_chain);
	if (DEBUG_CMDS) printf("\n");
	if (DEBUG_CMDS) printf("ctrl\n");
	if (DEBUG_CMDS) printf("reset %x stall %x\n", reset, stall);

	crc_w = 0xffffffff;
	// Try packing everyhing we want to send into one write buffer
	//Calculate CRCs first
	for (i=0;i<DBGCHAIN_SIZE+1;i++)
	  crc_w = crc_calc(crc_w, (DI_WRITE_CTRL_5BITREVERSED>>i)&1);
	  //crc_w = crc_calc(crc_w, 
	  //	   ((bit_reverse_data((DI_WRITE_CTRL & 0xf),DBGCHAIN_SIZE+1))>>i)&1);
	crc_w = crc_calc(crc_w,(reset&1));
	crc_w = crc_calc(crc_w,(stall&1));
	for (i=0;i<50;i++)
	  crc_w = crc_calc(crc_w,0);
	

	
	// Now pack the write data buffer
	// 1-bit 0, 4-bits cmd, 52-bits CPU control register data (only first 2 matter)
	//bits 0-4
	WriteDataBuffer[0]=(DI_WRITE_CTRL_5BITREVERSED);
	// bit 5
	WriteDataBuffer[0] |= (reset)<<(DBGCHAIN_SIZE+1);
	// bit 6
	WriteDataBuffer[0] |= (stall)<<(DBGCHAIN_SIZE+2);
	// Clear the next 48 bits
	for (i=1; i<16;i++)   //actually clear more than just the next 50 bits
	  WriteDataBuffer[i] = 0;

	//Reverse the CRC first
	crc_w = bit_reverse_data(crc_w, DBG_CRC_SIZE);
	//Now load in the CRC, but take note of fact 
	// that bit 0 of buffer[7] is last 0 from cmd register data
	// fill up from WriteDataBuffer[7-11]
	for (i=0;i<4;i++)
	  WriteDataBuffer[7+i] = ((crc_w<<1)>>(i*8))&0xff;
	//Final bit, shift in and make sure is the only thing int he buffer
	WriteDataBuffer[11]=0|((crc_w>>31)&1);
	
	
	// Platform independant driver call
	Status = FT2232_USB_JTAG_WriteReadDataToFromExternalDevice(false,89+4+32 , &WriteDataBuffer, 16, &ReadDataBuffer, &dwNumBytesReturned, RUN_TEST_IDLE_STATE);

	
	if (Status != FTC_SUCCESS)
	  printf("USB write fail - code %ld\b",Status);
	
	// Now look through the read data

	// From bit1 of ReadDataBuffer[11] we should have our 4-bit status
	status = (ReadDataBuffer[11] >> 1) & 0xf;
	
	// Now extract the received CRC
	crc_read = 0;
	//first 3 bits (0-2)
	crc_read |= (ReadDataBuffer[11] >> 5) & 0x7;
	// middle 3 bytes (bits 3 to 26)
	for (i=0;i<3;i++)
	  crc_read |= ((ReadDataBuffer[12+i]&0xff) << ((i*8)+3));
	// last 5 bits from ReadDataBuffer[15]
	crc_read |= (ReadDataBuffer[15]&0x1f)<<27;


	// Now calculate CRC on status and crc_read
	crc_r = 0xffffffff;
	for(i=0;i<DC_STATUS_SIZE;i++)
	  crc_r = crc_calc(crc_r, (status>>i)&1);
	
	crc_generated = crc_r;
	// Now bit reverse status and crc_read as we unpacked them
	// with the MSb going to the LSb
	status = bit_reverse_data(status, DC_STATUS_SIZE);
	crc_read = bit_reverse_data(crc_read, DBG_CRC_SIZE);	  
	
	/* CRCs must match, otherwise retry */
	//printf("%x %x %x\n", status, crc_read, crc_generated);
	  if (crc_read != crc_generated) {
	  retries++;
	  if (retries < 2)
	    {
	      if (DEBUG_USB_DRVR_FUNCS)
		printf("usb_functions - usb_dbg_ctrl - CRC fail. Going again\n");
	      goto try_again;
	    }
	  else if (retries < 8)
	    {
	      reset_tap();
	      goto try_again;
	    }
	  else return DBG_ERR_CRC;
	}
	/* we should read expected status value, otherwise retry */
	retries = 0;
	if (status != 0) {
	  if (retries < 2)
	    {
	      if (DEBUG_USB_DRVR_FUNCS)
		printf("usb_functions - usb_dbg_ctrl - bad status (%d). Going again\n",status);
	      goto try_again;
	    }
	  else if (retries < 8)
	    {
	      reset_tap();
	      goto try_again;
	    }
	  else return status;

	}

	/* reset retry counter */
	retry_ok();
	return DBG_ERR_OK;
}


/* reads control register */
int usb_dbg_ctrl_read(uint32_t *reset, uint32_t *stall) {
  uint32_t i, status, crc_generated, crc_read;
  
  // JTAG driver things
  FTC_STATUS Status = FTC_SUCCESS;
  WriteDataByteBuffer WriteDataBuffer;
  ReadDataByteBuffer ReadDataBuffer;
  retries = 0;
  
 try_again:
  usb_dbg_set_chain(dbg_chain);
  if (DEBUG_CMDS) printf("\n");
  if (DEBUG_CMDS) printf("ctrl_read\n");
  
  crc_w = 0xffffffff;
  // Try packing everyhing we want to send into one write buffer
  //Calculate CRCs first
  for (i=0;i<DBGCHAIN_SIZE+1;i++)
    crc_w = crc_calc(crc_w, (DI_READ_CTRL_5BITREVERSED>>i)&1);
    //crc_w = crc_calc(crc_w, 
    //((bit_reverse_data((DI_READ_CTRL & 0xf),DBGCHAIN_SIZE+1))>>i)&1);

  
  // Now pack the write data buffer
  // 1-bit 0, 4-bits cmd, 32-bit CRC
  //bits 0-4
  WriteDataBuffer[0]=(DI_READ_CTRL_5BITREVERSED);
  // Clear the next 48 bits
  for (i=1; i<16;i++)   //actually clear more than just the next 50 bits
    WriteDataBuffer[i] = 0;
  
  //Reverse the CRC first
  crc_w = bit_reverse_data(crc_w, DBG_CRC_SIZE);
  //Now load in the CRC
  //First 3 bits go in last 3 bits of buffer[0]
  WriteDataBuffer[0] |= (crc_w & 0x7)<<5;
  // The rest of crc_w goes in buffer[1-4]
  for (i=0;i<3;i++)
    WriteDataBuffer[1+i] = ((crc_w>>3)>>(i*8))&0xff;
  //Final bit of write buffer with CRC
  WriteDataBuffer[4] = ((crc_w>>3)>>(24))&0x1f;

  
  
  //Status = pFT2232cMpsseJtag->JTAG_WriteReadDataToFromExternalDevice(gFtHandle,false,5+32+52+4+32, &WriteDataBuffer, 16, &ReadDataBuffer, &dwNumBytesReturned, RUN_TEST_IDLE_STATE);
  // Platform independant driver call
  Status = FT2232_USB_JTAG_WriteReadDataToFromExternalDevice(false,5+32+52+4+32, &WriteDataBuffer, 16, &ReadDataBuffer, &dwNumBytesReturned, RUN_TEST_IDLE_STATE);
  if (Status != FTC_SUCCESS)
    printf("USB read fail - code %ld\b",Status);
  
  // Now look through the read data
  //0 - 1+4-bit status, 3-bits CRC
  //1,2,3 - CRC
  //4 - first 5 bits CRC, last 3, control reg bits (first 3)
  //5,6,7,8,9,10 - control reg data (48 bits)
  //11 - bit0 - control reg data, bit 1-4 status bits, bits 5-7 CRC
  //12, 13 14 - CRC
  // 15 bits 0-4 CRC
  // Starting from bit1 of ReadDataBuffer[11] we should have our 4-bit status
  status = (ReadDataBuffer[11] >> 1) & 0xf;
  
  //reset bit should be in ReadDataBuffer[4] as bit 5
  *reset = (ReadDataBuffer[4] >> 5) & 1;
  //stalled bit should be in ReadDataBuffer[4] as bit 6
  *stall = (ReadDataBuffer[4] >> 6) & 1;
  // Now extract the received CRC
  crc_read = 0;
  //first 3 bits (0-2) of CRC are in bits 5-7 of ReadDataBuffer[11]
  crc_read |= (ReadDataBuffer[11] >> 5) & 0x7;
  // middle 3 bytes (bits 3 to 26)
  for (i=0;i<3;i++)
    crc_read |= ((ReadDataBuffer[12+i]&0xff) << ((i*8)+3));
  // last 5 bits from ReadDataBuffer[15]
  crc_read |= (ReadDataBuffer[15]&0x1f)<<27;
  
  if (DEBUG_CMDS) printf("reset bit %x stalled bit %x:\n",
	 ((ReadDataBuffer[4] >> 5) & 1), ((ReadDataBuffer[4] >> 6) & 1));

  // Now calculate CRC on status and crc_read
  crc_r = 0xffffffff;
  
  crc_r = crc_calc(crc_r, ((ReadDataBuffer[4] >> 5) & 1));
  crc_r = crc_calc(crc_r, ((ReadDataBuffer[4] >> 6) & 1));
  for(i=0;i<50;i++)
    crc_r = crc_calc(crc_r,0);
  for(i=0;i<DC_STATUS_SIZE;i++)
    crc_r = crc_calc(crc_r, (status>>i)&1);
  
  crc_generated = crc_r;
  // Now bit reverse status and crc_read as we unpacked them
  // with the MSb going to the LSb
  status = bit_reverse_data(status, DC_STATUS_SIZE);
  crc_read = bit_reverse_data(crc_read, DBG_CRC_SIZE);	  

    
  /* CRCs must match, otherwise retry */
  //printf("%x %x %x\n", status, crc_generated, crc_read);
  if (crc_read != crc_generated) {
    retries++;
    if (retries < 2)
      {
	if (DEBUG_USB_DRVR_FUNCS)
	  printf("usb_functions - usb_dbg_ctrl_read - CRC fail. Going again\n");
	goto try_again;
      }
    else if (retries < 8)
      {
	reset_tap();
	goto try_again;
      }
    else return DBG_ERR_CRC;
  }
  /* we should read expected status value, otherwise retry */
  retries = 0;
  if (status != 0) {
    if (retries < 2)
      {
	if (DEBUG_USB_DRVR_FUNCS)
	  printf("usb_functions - usb_dbg_ctrl_read - bad status (%d). Going again\n",status);
	goto try_again;
      }
    else if (retries < 8)
      {
	reset_tap();
	goto try_again;
      }
    else return status;
    
  }
  
  /* reset retry counter */
  retry_ok();
  return DBG_ERR_OK;
}


/* issues a burst read/write */
int usb_dbg_go(unsigned char *data, uint16_t len, uint32_t read) {
	uint32_t status, crc_generated, crc_read;
        int i,j;
	uint8_t data_byte;
	retries = 0;
	// JTAG driver things
	FTC_STATUS Status = FTC_SUCCESS;
	WriteDataByteBuffer WriteDataBuffer;
	ReadDataByteBuffer ReadDataBuffer;
	
 try_again:
	usb_dbg_set_chain(dbg_chain);
	if (DEBUG_CMDS) printf("\n");
	if (DEBUG_CMDS) printf("go len is %dbytes, read=%d (if 0, writing)\n", len, read);

	crc_w = 0xffffffff;
	// Try packing everyhing we want to send into one write buffer
	//Calculate CRCs first
	for (i=0;i<DBGCHAIN_SIZE+1;i++)
	  crc_w = crc_calc(crc_w, (DI_GO_5BITREVERSED>>i)&1);
	  //crc_w = crc_calc(crc_w, 
	  //	   ((bit_reverse_data((DI_GO & 0xf),DBGCHAIN_SIZE+1))>>i)&1);
	// Set first 5 bits of WriteDataBuffer up to 
	// be the GO command and 0 first bit
	WriteDataBuffer[0]=DI_GO_5BITREVERSED;
	
	if (read)
	  {
	    // Do GO command for a read
	    // 0 then 4-bit go command, then 32-bit CRC for the last 5 bits
	    // Then read (len+1)*8 + 4-bit status + 32-bit CRC

	    
	    // Reverse crc_w
	    crc_w = bit_reverse_data(crc_w,DBG_CRC_SIZE);	    

	    // Now pack in the 32-bit CRC as we're not 
	    // writing anything else after it
	     //First 3 bits of each byte go in last 3 bits of buffer[i], 
	    // next 5 go in buffer[i+1]
	    for(i=0;i<4;i++){
	      WriteDataBuffer[i] |= ((crc_w>>(i*8))&0x07)<<5;
	      WriteDataBuffer[i+1] = ((crc_w>>(i*8))>>3)&0x1f;
	    }
	      
	    // Should have data up to WriteDataBuffer[4] bit 4
	    // Read data should start at ReadDataBuffer[4] bit 5

	    //Clear the rest of the write buffer
	    for(i=5;i<(10+len);i++)
	      WriteDataBuffer[i]=0;
	  } 
	if (!read){
	  // If we're writing we put in the 5 command bits, (len+1)*8 data bits,
	  // and then the 32-bit CRC do first 3 bits, then next 5 bits in 
	  // each of the for loops iterations
	  for(i=0;i<len;i++){
	  
	    data_byte = bit_reverse_swar_8(data[i]);	    
	    
	    WriteDataBuffer[i] |= ((data_byte&0x07)<<5);
	    WriteDataBuffer[i+1] = ((data_byte>>3)&0x1f);
	    
	    // Now update the CRC
	    for(j=0;j<8;j++) 
	      crc_w = crc_calc(crc_w, (data_byte>>j)&1);
	  
	  }
	  
	  // Reverse crc_w
	  crc_w = bit_reverse_data(crc_w,DBG_CRC_SIZE);
	  
	  // If we have len=4 for example, we will write to 
	  // WriteDataBuffer[4]'s first 5 bits

	  // So now load in the 32-bit CRC from there
	  for(i=0;i<4;i++){
	    WriteDataBuffer[len+i] |= ((crc_w>>(i*8))&0x07)<<5;
	    WriteDataBuffer[len+i+1] = ((crc_w>>(i*8))>>3)&0x1f;
	  }
	  // Should have data up to WriteDataBuffer[4+len] bit 4
	  // Read data should start at ReadDataBuffer[4+len] bit 5

	}
	  	
	//Status = pFT2232cMpsseJtag->JTAG_WriteReadDataToFromExternalDevice(gFtHandle,false,(5+(len*8)+32+36), &WriteDataBuffer, (6+len+4), &ReadDataBuffer,&dwNumBytesReturned,RUN_TEST_IDLE_STATE);
	Status = FT2232_USB_JTAG_WriteReadDataToFromExternalDevice(false,(5+(len*8)+32+36), &WriteDataBuffer, (6+len+4), &ReadDataBuffer,&dwNumBytesReturned,RUN_TEST_IDLE_STATE);

	if (Status != FTC_SUCCESS)
	  printf("USB write fail - code %ld\n",Status);
	
	crc_r = 0xffffffff;

	if (read){
	  // Look through our data, starting from ReadDataBuffer[4] bit 5
	  // We receive len bytes, starting at ReadDataBuffer[4] bit 5, so
	  // unpack like so
	  if (DEBUG_USB_DRVR_FUNCS) printf("USB read data buffer: ");
	  for(i=0;i<len;i++){
	    // get first 3 bits
	    data[i] = (ReadDataBuffer[4+i]>>5)&0x07;
	    // then next 5 from next ReadDataBuffer byte
	    data[i] |= (ReadDataBuffer[4+i+1]&0x1f)<<3;
	    
	    // Now calculate the CRC for this byte
	    for(j=0;j<8;j++) 
	      crc_r = crc_calc(crc_r, (data[i]>>j)&1);

	    // Now bit reverse the byte as it was read in MSb first but 
	    // written to LSb first
	    data[i] = bit_reverse_data(data[i],8);
	    
	    if (DEBUG_USB_DRVR_FUNCS) printf("%.2x",data[i]);
	  }
	  if (DEBUG_USB_DRVR_FUNCS) printf("\n");
	
	  // Should be up to ReadDataBuffer[4+len] bit 5 for data
	  status = (ReadDataBuffer[4+len]>>5)&0x07;
	  status |= (ReadDataBuffer[4+len+1]&1)<<3;
	  // Now get out crc_read
	  crc_read = 0;
	  for(i=0;i<4;i++){
	    crc_read |= ((ReadDataBuffer[4+len+1+i]>>1)&0x7f)<<(i*8);
	    crc_read |= ((ReadDataBuffer[4+len+1+i+1]&0x1)<<7)<<(i*8);
	  }
	  
	  for(i=0;i<4;i++)
	    crc_r = crc_calc(crc_r, (status>>i)&1);	  
	}
	if (!read){
	  // Just extract our 4-bits of status and CRC
	  status = (ReadDataBuffer[4+len]>>5)&0x07;
	  status |= (ReadDataBuffer[4+len+1]&1)<<3;
	  
	  // extract crc_read from the ReadDataBuffer
	  crc_read = 0;
	  for(i=0;i<4;i++){
	    crc_read |= ((ReadDataBuffer[4+len+1+i]>>1)&0x7f)<<(i*8);
	    crc_read |= ((ReadDataBuffer[4+len+1+i+1]&1)<<7)<<(i*8);
	  }
	  // Calculate our own CRC from the status value
	  for(i=0;i<4;i++)
	    crc_r = crc_calc(crc_r, (status>>i)&1);
	  
	}
	
	crc_generated = crc_r;

	// Now bit reverse status and crc_read as we unpacked them
	// with the MSb going to the LSb
	status = bit_reverse_data(status, DC_STATUS_SIZE);
	crc_read = bit_reverse_data(crc_read, DBG_CRC_SIZE);	  

	/* CRCs must match, otherwise retry */
	
	//printf("%x %x %x\n", status, crc_read, crc_generated);
	
	if (crc_read != crc_generated) {
	  retries++;
	  if (retries < 8)
	    {
	      if (DEBUG_USB_DRVR_FUNCS) printf("usb_functions - usb_dbg_go - CRC fail (%d) try %d. Going again\n",status, retries);
	      reset_tap() ;	      
	      goto try_again;
	    }
	  else return DBG_ERR_CRC;
	}
	//if (crc_read == crc_generated)
	//tries = 0;
	/* we should read expected status value, otherwise retry */
	if (status != 0) {
	  retries++;
	  if (retries < 8)
	    {
	      if (DEBUG_USB_DRVR_FUNCS) printf("usb_functions - usb_dbg_go - bad status (%d) try %d. Going again\n",status, retries);
	      reset_tap();
	      goto try_again;
	      
	    }
	  else return status;

	}
	
	retry_ok();
	return DBG_ERR_OK;
}


/* read a word from wishbone */
int usb_dbg_wb_read8(uint32_t adr, uint8_t *data) {
  if ((err = usb_dbg_set_chain(DC_WISHBONE))) return err;
  if ((err = usb_dbg_command(DBG_WB_READ8, adr, 1))) return err;
  if ((err = usb_dbg_go((unsigned char*)data, 1, 1))) return err;
  //*data = ntohl(*data);
  return err;
}

/* read a word from wishbone */
int usb_dbg_wb_read32(uint32_t adr, uint32_t *data) {
  // uint32_t err;
  if ((err = usb_dbg_set_chain(DC_WISHBONE))) return err;
  if ((err = usb_dbg_command(DBG_WB_READ32, adr, 4))) return err;
  if ((err = usb_dbg_go((unsigned char*)data, 4, 1))) return err;
  //*data = ntohl(*data);
  return err;
}

/* write a byte to wishbone */
int usb_dbg_wb_write8(uint32_t adr, uint8_t data) {
  if ((err = usb_dbg_set_chain(DC_WISHBONE))) return err;
  if ((err = usb_dbg_command(DBG_WB_WRITE8, adr, 1))) return err;
  if ((err = usb_dbg_go((unsigned char*)&data, 1, 0))) return err;
  return DBG_ERR_OK;
}


/* write a word to wishbone */
int usb_dbg_wb_write32(uint32_t adr, uint32_t data) {
  if ((err = usb_dbg_set_chain(DC_WISHBONE))) return err;
  if ((err = usb_dbg_command(DBG_WB_WRITE32, adr, 4))) return err;
  if ((err = usb_dbg_go((unsigned char*)&data, 4, 0))) return err;
  return DBG_ERR_OK;
}

/* read a block from wishbone */
int usb_dbg_wb_read_block32(uint32_t adr, uint32_t *data, uint32_t len) {
  if ((err = usb_dbg_set_chain(DC_WISHBONE))) return err;
  if ((err = usb_dbg_command(DBG_WB_READ32, adr, len))) return err;
  if ((err = usb_dbg_go((unsigned char*)data, len, 1))) return err;
  return DBG_ERR_OK;
}


/* write a block to wishbone */
int usb_dbg_wb_write_block32(uint32_t adr, uint32_t *data, uint32_t len) {
  if (DEBUG_CMDS) printf("usb_functions: wb_write_block %.8x %d bytes\n",adr, len);
  if ((err = usb_dbg_set_chain(DC_WISHBONE))) return err;
  if ((err = usb_dbg_command(DBG_WB_WRITE32, adr, len))) return err;
  if ((err = usb_dbg_go((unsigned char*)data, len, 0))) return err;
  return DBG_ERR_OK;
}


/* read a register from cpu */
int usb_dbg_cpu0_read(uint32_t adr, uint32_t *data, uint32_t length) {
  if ((err = usb_dbg_set_chain(DC_CPU0))) return err;
  if ((err = usb_dbg_command(DBG_CPU_READ, adr, length))) return err;
  if ((err = usb_dbg_go((unsigned char*)data, length, 1))) return err;
  int i;for(i=0;i<(length/4);i++)data[i]=ntohl(data[i]);
  return DBG_ERR_OK;
}

/* write a cpu register */
int usb_dbg_cpu0_write(uint32_t adr, uint32_t *data, uint32_t length) {
  int i;for(i=0;i<(length/4);i++){data[i]=ntohl(data[i]);}
  if ((err = usb_dbg_set_chain(DC_CPU0))) return err;
  if ((err = usb_dbg_command(DBG_CPU_WRITE, adr, length))) return err;
  if ((err = usb_dbg_go((unsigned char*)data, length, 0))) return err;
  return DBG_ERR_OK;
}

/* write a cpu module register */
int usb_dbg_cpu0_write_ctrl(uint32_t adr, unsigned char data) {  
  // no ensuring that or1k is stalled here, becuase we're call this from that function
  if ((err = usb_dbg_set_chain(DC_CPU0))) return err;
  if ((err = usb_dbg_ctrl(data >> 1, data & 0x1))) return err;
  return DBG_ERR_OK;
}

/* read a register from cpu module */
int usb_dbg_cpu0_read_ctrl(uint32_t adr, unsigned char *data) {
  // no ensuring that or1k is stalled here, becuase we're call this from that function
  uint32_t r, s;
  if ((err = usb_dbg_set_chain(DC_CPU0))) return err;
  if ((err = usb_dbg_ctrl_read(&r, &s))) return err;
  *data = (r << 1) | s;
  return DBG_ERR_OK;
}

/* Function to close the device handle. Is called when closing the app */
void usb_close_device_handle()
{
  // try unstalling the processor before quitting
  dbg_cpu0_write_ctrl(0, 0x00);      // unstall or1k
  FT2232_USB_JTAG_CloseDevice();
}
