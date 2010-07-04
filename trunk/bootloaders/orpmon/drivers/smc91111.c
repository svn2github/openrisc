/*------------------------------------------------------------------------
 . smc91111.c
 . This is a driver for SMSC's 91C111 single-chip Ethernet device.
 .
 . (C) Copyright 2005
 .
 . Copyright (C) 2001 Standard Microsystems Corporation (SMSC)
 .	 Developed by Simple Network Magic Corporation (SNMC)
 . Copyright (C) 1996 by Erik Stahlman (ES)
 .
 . This program is free software; you can redistribute it and/or modify
 . it under the terms of the GNU General Public License as published by
 . the Free Software Foundation; either version 2 of the License, or
 . (at your option) any later version.
 .
 . This program is distributed in the hope that it will be useful,
 . but WITHOUT ANY WARRANTY; without even the implied warranty of
 . MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 . GNU General Public License for more details.
 .
 . You should have received a copy of the GNU General Public License
 . along with this program; if not, write to the Free Software
 . Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307	 USA
 .
 . Information contained in this file was obtained from the LAN91C111
 . manual from SMC.  To get a copy, if you really want one, you can find
 . information under www.smsc.com.
 .
 .
 . author:
 .	Javier Castillo			( javier.castillo@urjc.es )
 .
 . Sources:
 .    o	  smc91111.c by Erik Stahlman
 .
 . History:
 . 06/05/05  Javier Castillo  Modified smc91111.c to work with OR1200
 ----------------------------------------------------------------------------*/

#include "board.h" 

#if SMC91111_LAN==1

#include "common.h"
#include "smc91111.h"
#include "support.h"
#include "net.h"

#ifndef CONFIG_SMC_AUTONEG_TIMEOUT
#define CONFIG_SMC_AUTONEG_TIMEOUT 10
#endif

#define ETH_ZLEN 60
#define MEMORY_WAIT_TIME 16
#define SMC_ALLOC_MAX_TRY 5
#define SMC_TX_TIMEOUT 30
#define SWAP16(x)  ((((x) & 0x00ff) << 8) | ( (x) >> 8))


typedef  unsigned char  byte;
typedef  unsigned short word;

//Do nothing but needed to compile
int tx_next;
int rx_next;

void (*receive)(volatile unsigned char *add, int len); /* Pointer to function to be called 
                                        when frame is received */

static void __delay(unsigned long loops)
{
        __asm__ __volatile__ ("1: l.sfeqi %0,0; \
                                l.bnf   1b; \
                                l.addi %0,%0,-1;"
                                : "=r" (loops) : "0" (loops));
}

static void udelay(unsigned long usecs)
{
        /* Sigh */
        __delay(usecs);
}

static void smc_wait_ms(unsigned int ms){
	udelay(ms*1000);
}

static void
print_packet(unsigned long add, int len)
{
  int i;
 
  printf("ipacket: add = %lx len = %d\n", add, len);
  for(i = 0; i < len; i++) {
      if(!(i % 16))
          printf("\n");
      printf(" %.2x", *(((unsigned char *)add) + i));
  }
  printf("\n");
}

/*------------------------------------------------------------
 . Reads a register from the MII Management serial interface
 .-------------------------------------------------------------*/
static word smc_read_phy_register(byte phyreg)
{
	int oldBank;
	int i;
	byte mask;
	word mii_reg;
	byte bits[64];
	int clk_idx = 0;
	int input_idx;
	word phydata;
	byte phyaddr=SMC_PHY_ADDR;

	// 32 consecutive ones on MDO to establish sync
	for (i = 0; i < 32; ++i)
		bits[clk_idx++] = MII_MDOE | MII_MDO;

	// Start code <01>
	bits[clk_idx++] = MII_MDOE;
	bits[clk_idx++] = MII_MDOE | MII_MDO;

	// Read command <10>
	bits[clk_idx++] = MII_MDOE | MII_MDO;
	bits[clk_idx++] = MII_MDOE;

	// Output the PHY address, msb first
	mask = (byte)0x10;
	for (i = 0; i < 5; ++i)
		{
		if (phyaddr & mask)
			bits[clk_idx++] = MII_MDOE | MII_MDO;
		else
			bits[clk_idx++] = MII_MDOE;

		// Shift to next lowest bit
		mask >>= 1;
		}

	// Output the phy register number, msb first
	mask = (byte)0x10;
	for (i = 0; i < 5; ++i)
		{
		if (phyreg & mask)
			bits[clk_idx++] = MII_MDOE | MII_MDO;
		else
			bits[clk_idx++] = MII_MDOE;

		// Shift to next lowest bit
		mask >>= 1;
		}

	// Tristate and turnaround (2 bit times)
	bits[clk_idx++] = 0;
	//bits[clk_idx++] = 0;

	// Input starts at this bit time
	input_idx = clk_idx;

	// Will input 16 bits
	for (i = 0; i < 16; ++i)
		bits[clk_idx++] = 0;

	// Final clock bit
	bits[clk_idx++] = 0;

	// Save the current bank
	oldBank = REG16(ETH_BASE+BANK_SELECT);

	// Select bank 3
	SMC_SELECT_BANK( 3 );

	// Get the current MII register value
	mii_reg = REG16(ETH_BASE+MII_REG);

	// Turn off all MII Interface bits
	mii_reg &= ~(MII_MDOE|MII_MCLK|MII_MDI|MII_MDO);

	// Clock all 64 cycles
	for (i = 0; i < sizeof bits; ++i)
		{
		// Clock Low - output data
		REG16(ETH_BASE+MII_REG)= mii_reg | bits[i];
		udelay(500);


		// Clock Hi - input data
		REG16(ETH_BASE+MII_REG)= mii_reg | bits[i] | MII_MCLK;
		udelay(500);
		bits[i] |= REG16(ETH_BASE+MII_REG) & MII_MDI;
		}

	// Return to idle state
	// Set clock to low, data to low, and output tristated
	REG16(ETH_BASE+MII_REG)=mii_reg;
	udelay(500);

	// Restore original bank select
	SMC_SELECT_BANK( oldBank );

	// Recover input data
	phydata = 0;
	for (i = 0; i < 16; ++i)
		{
		phydata <<= 1;

		if (bits[input_idx++] & MII_MDI)
			phydata |= 0x0001;
		}

	return(phydata);
}

static void smc_write_phy_register(byte phyreg, word phydata){

	int oldBank;
	int i;
	word mask;
	word mii_reg;
	byte bits[65];
	int clk_idx=0;
	byte phyaddr=SMC_PHY_ADDR; 

	/*32 consecutives ones on MDO to establish sync*/
	for(i=0; i<32;i++)
		bits[clk_idx++]=MII_MDOE | MII_MDO;

	// Start code <01>
	bits[clk_idx++] = MII_MDOE;
	bits[clk_idx++] = MII_MDOE | MII_MDO;

	// Write command <01>
	bits[clk_idx++] = MII_MDOE;
	bits[clk_idx++] = MII_MDOE | MII_MDO;

	// Output the PHY address, msb first
	mask = (byte)0x10;
	for (i = 0; i < 5; ++i)
		{
		if (phyaddr & mask)
			bits[clk_idx++] = MII_MDOE | MII_MDO;
		else
			bits[clk_idx++] = MII_MDOE;

		// Shift to next lowest bit
		mask >>= 1;
		}

	// Output the phy register number, msb first
	mask = (byte)0x10;
	for (i = 0; i < 5; ++i)
		{
		if (phyreg & mask)
			bits[clk_idx++] = MII_MDOE | MII_MDO;
		else
			bits[clk_idx++] = MII_MDOE;

		// Shift to next lowest bit
		mask >>= 1;
		}

	// Tristate and turnaround (2 bit times)
	bits[clk_idx++] = 0;
	bits[clk_idx++] = 0;

	// Write out 16 bits of data, msb first
	mask = 0x8000;
	for (i = 0; i < 16; ++i)
		{
		if (phydata & mask)
			bits[clk_idx++] = MII_MDOE | MII_MDO;
		else
			bits[clk_idx++] = MII_MDOE;

		// Shift to next lowest bit
		mask >>= 1;
		}

	// Final clock bit (tristate)
	bits[clk_idx++] = 0;

	// Save the current bank
	oldBank = REG16(ETH_BASE+BANK_SELECT);

	// Select bank 3
	SMC_SELECT_BANK( 3 );

	// Get the current MII register value
	mii_reg = REG16(ETH_BASE+MII_REG);

	// Turn off all MII Interface bits
	mii_reg &= ~(MII_MDOE|MII_MCLK|MII_MDI|MII_MDO);

	// Clock all cycles
	for (i = 0; i < sizeof bits; ++i)
		{
		// Clock Low - output data
		REG16(ETH_BASE+MII_REG)= mii_reg | bits[i];
		udelay(50);


		// Clock Hi - input data
		REG16(ETH_BASE+MII_REG)= mii_reg | bits[i] | MII_MCLK;
		udelay(50);
		bits[i] |= REG16(ETH_BASE+MII_REG) & MII_MDI;
		}

	// Return to idle state
	// Set clock to low, data to low, and output tristated
        REG16(ETH_BASE+MII_REG)=mii_reg;
	udelay(50);

	// Restore original bank select
	SMC_SELECT_BANK( oldBank );

}


static void smc_phy_configure (void){
	int timeout;
	byte phyaddr;
	word my_phy_caps;	/* My PHY capabilities */
	word my_ad_caps;	/* My Advertised capabilities */
	word status = 0;	/*;my status = 0 */
	int failed = 0;

	/* Get the detected phy address */
	phyaddr = SMC_PHY_ADDR;

	/* Reset the PHY, setting all other bits to zero */
	smc_write_phy_register (PHY_CNTL_REG, PHY_CNTL_RST);

	/* Wait for the reset to complete, or time out */
	timeout = 6;		/* Wait up to 3 seconds */
	while (timeout--) {
		if (!(smc_read_phy_register (PHY_CNTL_REG)
		      & PHY_CNTL_RST)) {
			/* reset complete */
			break;
		}

		smc_wait_ms (500);	/* wait 500 millisecs */
	}

	if (timeout < 1) {
		printf ("PHY reset timed out\n");
		goto smc_phy_configure_exit;
	}

	/* Read PHY Register 18, Status Output */
	/* lp->lastPhy18 = smc_read_phy_register(PHY_INT_REG); */

	/* Enable PHY Interrupts (for register 18) */
	/* Interrupts listed here are disabled */
	smc_write_phy_register (PHY_MASK_REG, 0xffff);

	/* Configure the Receive/Phy Control register */
	SMC_SELECT_BANK (0);
	REG16(ETH_BASE+RPC_REG)=RPC_DEFAULT;

	/* Copy our capabilities from PHY_STAT_REG to PHY_AD_REG */
	my_phy_caps = smc_read_phy_register (PHY_STAT_REG);
	my_ad_caps = PHY_AD_CSMA;	/* I am CSMA capable */

	if (my_phy_caps & PHY_STAT_CAP_T4)
		my_ad_caps |= PHY_AD_T4;

	if (my_phy_caps & PHY_STAT_CAP_TXF)
		my_ad_caps |= PHY_AD_TX_FDX;

	if (my_phy_caps & PHY_STAT_CAP_TXH)
		my_ad_caps |= PHY_AD_TX_HDX;

	if (my_phy_caps & PHY_STAT_CAP_TF)
		my_ad_caps |= PHY_AD_10_FDX;

	if (my_phy_caps & PHY_STAT_CAP_TH)
		my_ad_caps |= PHY_AD_10_HDX;

	/* Update our Auto-Neg Advertisement Register */
	smc_write_phy_register (PHY_AD_REG, my_ad_caps);

	/* Read the register back.  Without this, it appears that when */
	/* auto-negotiation is restarted, sometimes it isn't ready and */
	/* the link does not come up. */
	smc_read_phy_register(PHY_AD_REG);

	/* Restart auto-negotiation process in order to advertise my caps */
	smc_write_phy_register (PHY_CNTL_REG,
				PHY_CNTL_ANEG_EN | PHY_CNTL_ANEG_RST);

	/* Wait for the auto-negotiation to complete.  This may take from */
	/* 2 to 3 seconds. */
	/* Wait for the reset to complete, or time out */
	timeout = CONFIG_SMC_AUTONEG_TIMEOUT * 2;
	while (timeout--) {

		status = smc_read_phy_register (PHY_STAT_REG);
		if (status & PHY_STAT_ANEG_ACK) {
			/* auto-negotiate complete */
			break;
		}

		smc_wait_ms (500);	/* wait 500 millisecs */

		/* Restart auto-negotiation if remote fault */
		if (status & PHY_STAT_REM_FLT) {
			printf ("PHY remote fault detected\n");

			/* Restart auto-negotiation */
			printf ("PHY restarting auto-negotiation\n");
			smc_write_phy_register (PHY_CNTL_REG,
						PHY_CNTL_ANEG_EN |
						PHY_CNTL_ANEG_RST |
						PHY_CNTL_SPEED |
						PHY_CNTL_DPLX);
		}
	}

	if (timeout < 1) {
		printf ("PHY auto-negotiate timed out\n");
		failed = 1;
	}

	/* Fail if we detected an auto-negotiate remote fault */
	if (status & PHY_STAT_REM_FLT) {
		printf ("PHY remote fault detected\n");
		failed = 1;
	}

	/* Re-Configure the Receive/Phy Control register */
	REG16(ETH_BASE+RPC_REG)=RPC_DEFAULT;

smc_phy_configure_exit:	;

}

static inline void smc_wait_mmu_release_complete (void)
{
	int count = 0;

	/* assume bank 2 selected */
	while (REG16(ETH_BASE+MMU_CMD_REG) & MC_BUSY) {
		udelay (1);	/* Wait until not busy */
		if (++count > 200)
			break;
	}
}

static void eth_reset(){

	/* This resets the registers mostly to defaults, but doesn't
	   affect EEPROM.  That seems unnecessary */
	SMC_SELECT_BANK (0);
	REG16(ETH_BASE+RCR_REG)=RCR_SOFTRST;

	/* Setup the Configuration Register */
	/* This is necessary because the CONFIG_REG is not affected */
	/* by a soft reset */

	SMC_SELECT_BANK (1);
	REG16(ETH_BASE+CONFIG_REG)=CONFIG_DEFAULT;

	/* Release from possible power-down state */
	/* Configuration register is not affected by Soft Reset */
	REG16(ETH_BASE+CONFIG_REG)=REG16(ETH_BASE+CONFIG_REG) | CONFIG_EPH_POWER_EN;

	SMC_SELECT_BANK (0);

	/* this should pause enough for the chip to be happy */
	udelay (100);

	/* Disable transmit and receive functionality */
	REG16(ETH_BASE+RCR_REG)=RCR_CLEAR;
	REG16(ETH_BASE+TCR_REG)=TCR_CLEAR;

	/* set the control register */
	SMC_SELECT_BANK (1);
	REG16(ETH_BASE+CTL_REG)=CTL_DEFAULT;

	/* Reset the MMU */
	SMC_SELECT_BANK (2);
	smc_wait_mmu_release_complete ();
	REG16(ETH_BASE+MMU_CMD_REG)=MC_RESET;
	while (REG16(ETH_BASE+MMU_CMD_REG) & MC_BUSY)
		udelay (1);	/* Wait until not busy */

	/* Note:  It doesn't seem that waiting for the MMU busy is needed here,
	   but this is a place where future chipsets _COULD_ break.  Be wary
	   of issuing another MMU command right after this */

	/* Disable all interrupts */
	REG8(ETH_BASE+IM_REG)=0;
}	
	
void eth_init (void (*rec)(volatile unsigned char *, int)){
	
	receive = rec;
	  
	/*First make a SW reset of the chip*/  
	eth_reset();
	  
	/*Enable TX and Rx*/
	SMC_SELECT_BANK(0x0);
	REG16(ETH_BASE+TCR_REG)=TCR_DEFAULT;
	REG16(ETH_BASE+RCR_REG)=RCR_DEFAULT;
	REG16(ETH_BASE+RPC_REG)=RPC_DEFAULT;
	
	/*Configure PHY with autoneg*/
	smc_phy_configure();
		
	/*Set MAC address*/
	SMC_SELECT_BANK(0x1);
	REG8(ETH_BASE+ADDR0_REG)=ETH_MACADDR0;
	REG8(ETH_BASE+ADDR0_REG+1)=ETH_MACADDR1;
	REG8(ETH_BASE+ADDR0_REG+2)=ETH_MACADDR2;
	REG8(ETH_BASE+ADDR0_REG+3)=ETH_MACADDR3;
	REG8(ETH_BASE+ADDR0_REG+4)=ETH_MACADDR4;
	REG8(ETH_BASE+ADDR0_REG+5)=ETH_MACADDR5;	
	
}

void eth_halt(void) {
	/* no more interrupts for me */
	SMC_SELECT_BANK( 2 );
	REG8(ETH_BASE+IM_REG)=0;

	/* and tell the card to stay away from that nasty outside world */
	SMC_SELECT_BANK( 0 );
	REG8(ETH_BASE+RCR_REG)=RCR_CLEAR;
	REG8(ETH_BASE+TCR_REG)=TCR_CLEAR;
}

unsigned long eth_rx (void){

	int	packet_number;
	word	status;
	word	packet_length;
	word	packet_length_loop;
	int	is_error = 0;
	byte saved_pnr;
	word saved_ptr;
	word word_readed;
	byte *data;

	SMC_SELECT_BANK(2);
	/* save PTR and PTR registers */
	saved_pnr = REG8(ETH_BASE+PN_REG );
	saved_ptr = REG16(ETH_BASE+PTR_REG );

	packet_number = REG16(ETH_BASE+RXFIFO_REG );

	if ( packet_number & RXFIFO_REMPTY ) {
		return 0;
	}

	/*  start reading from the start of the packet */
	REG16(ETH_BASE+PTR_REG)=PTR_READ | PTR_RCV | PTR_AUTOINC;

	/* First two words are status and packet_length */
	status		= REG16(ETH_BASE+SMC91111_DATA_REG );
	packet_length	= REG16(ETH_BASE+SMC91111_DATA_REG );

	packet_length &= 0x07ff;  /* mask off top bits */

	#ifdef DEBUG
	printf("RCV: STATUS %4x LENGTH %4x\n", status, packet_length );
	#endif
	if ( !(status & RS_ERRORS ) ){
		/* Adjust for having already read the first two words */
		packet_length -= 4; /*4; */

		/* set odd length for bug in LAN91C111, */
		/* which never sets RS_ODDFRAME */
		/* TODO ? */
		#ifdef DEBUG
		printf(" Reading %d words and %d byte(s) \n",(packet_length >> 1 ), packet_length & 1 );
		#endif
		packet_length_loop=packet_length>>1;

                data=(byte*)NetRxPackets[0];
		while(packet_length_loop-->0){
 		  word_readed=REG16(ETH_BASE+SMC91111_DATA_REG);
	 	  *((word*)data)=SWAP16(word_readed);
		  data+=sizeof(word);
		}

	} else {
		/* error ... */
		/* TODO ? */
		is_error = 1;
	}

	while ( REG16(ETH_BASE+MMU_CMD_REG) & MC_BUSY )
		udelay(1); /* Wait until not busy */

	/*  error or good, tell the card to get rid of this packet */
	REG16(ETH_BASE+MMU_CMD_REG)=MC_RELEASE;

	while ( REG16(ETH_BASE+MMU_CMD_REG) & MC_BUSY )
		udelay(1); /* Wait until not busy */

	/* restore saved registers */

	REG8(ETH_BASE+PN_REG)=saved_pnr;

	REG16(ETH_BASE+PTR_REG)=saved_ptr;

	if (!is_error) {
		/* Pass the packet up to the protocol layers. */
		receive(NetRxPackets[0], packet_length);
		return packet_length;
	} else {
		return 0;
	}
}

#define CFG_HZ 1000
static int poll4int (byte mask, int timeout)
{
	int tmo = get_timer (0) + timeout * CFG_HZ;
	int is_timeout = 0;
	word old_bank = REG16(ETH_BASE+BSR_REG);
	#ifdef DEBUG
	printf ("Polling...\n");
	#endif
	SMC_SELECT_BANK (2);
	while ((REG16(ETH_BASE+SMC91111_INT_REG) & mask) == 0) {
		if (get_timer (0) >= tmo) {
			is_timeout = 1;
			break;
		}
	}

	/* restore old bank selection */
	SMC_SELECT_BANK (old_bank);

	if (is_timeout)
		return 1;
	else
		return 0;
}

void eth_send(void *buf, unsigned long len) {

	byte packet_no;
	byte *bufb;
	int length;
	int lengtht;
	int numPages;
	int try = 0;
	int time_out;
	byte status;
	byte saved_pnr;
	word saved_ptr;
	
	/* save PTR and PNR registers before manipulation */
	SMC_SELECT_BANK (2);
	saved_pnr = REG8( ETH_BASE+PN_REG );
	saved_ptr = REG16( ETH_BASE+PTR_REG );
	#ifdef DEBUG
	printf ("smc_hardware_send_packet\n");
	#endif
	length = ETH_ZLEN < len ? len : ETH_ZLEN;

	/* allocate memory
	 ** The MMU wants the number of pages to be the number of 256 bytes
	 ** 'pages', minus 1 ( since a packet can't ever have 0 pages :) )
	 **
	 ** The 91C111 ignores the size bits, but the code is left intact
	 ** for backwards and future compatibility.
	 **
	 ** Pkt size for allocating is data length +6 (for additional status
	 ** words, length and ctl!)
	 **
	 ** If odd size then last byte is included in this header.
	 */
	numPages = ((length & 0xfffe) + 6);
	numPages >>= 8;		/* Divide by 256 */

	if (numPages > 7) {
		printf ("Far too big packet error. \n");
		return;
	}

	/* now, try to allocate the memory */
	SMC_SELECT_BANK (2);
	REG16( ETH_BASE+MMU_CMD_REG)=MC_ALLOC | numPages;

	/* FIXME: the ALLOC_INT bit never gets set *
	 * so the following will always give a	   *
	 * memory allocation error.		   *
	 * same code works in armboot though	   *
	 * -ro
	 */

again:
	try++;
	time_out = MEMORY_WAIT_TIME;
	do {
		status = REG8( ETH_BASE+SMC91111_INT_REG);
		if (status & IM_ALLOC_INT) {
			/* acknowledge the interrupt */
			REG8( ETH_BASE+SMC91111_INT_REG)=IM_ALLOC_INT;
			break;
		}
	} while (--time_out);

	if (!time_out) {
		if (try < SMC_ALLOC_MAX_TRY)
			goto again;
		else
			return;
	}

	/* I can send the packet now.. */

	bufb = (byte *) buf;

	/* If I get here, I _know_ there is a packet slot waiting for me */
	packet_no = REG8( ETH_BASE+AR_REG);
	if (packet_no & AR_FAILED) {
		/* or isn't there?  BAD CHIP! */
		printf ("Memory allocation failed. \n");
		return;
	}

	/* we have a packet address, so tell the card to use it */
	REG8( ETH_BASE+PN_REG)=packet_no;
	/* do not write new ptr value if Write data fifo not empty */
	while ( saved_ptr & PTR_NOTEMPTY )
		printf ("Write data fifo not empty!\n");

	/* point to the beginning of the packet */
	REG16( ETH_BASE+PTR_REG)=PTR_AUTOINC;


	/* send the packet length ( +6 for status, length and ctl byte )
	   and the status word ( set to zeros ) */
	REG16(ETH_BASE+SMC91111_DATA_REG)=0;
	/* send the packet length ( +6 for status words, length, and ctl */
	REG16(ETH_BASE+SMC91111_DATA_REG)=length + 6;


	/* send the actual data */
	lengtht=length>>1;
	while(lengtht-->0){
 	  REG16(ETH_BASE+SMC91111_DATA_REG)=SWAP16(*((word*)bufb));
	  bufb+=sizeof(word);
	}
 

	/* Send the last byte, if there is one.	  */
	if ((length & 1) == 0) {
		REG16(ETH_BASE+SMC91111_DATA_REG)=0;
	} else {
		REG16(ETH_BASE+SMC91111_DATA_REG)=bufb[length - 1] | 0x2000;
	}

	/* and let the chipset deal with it */
	REG16(ETH_BASE+MMU_CMD_REG)=MC_ENQUEUE;

	/* poll for TX INT */
	/* if (poll4int (IM_TX_INT, SMC_TX_TIMEOUT)) { */
	/* poll for TX_EMPTY INT - autorelease enabled */
	if (poll4int(IM_TX_EMPTY_INT, SMC_TX_TIMEOUT)) {
		/* sending failed */
		printf ("TX timeout, sending failed...\n");

		/* release packet */
		/* no need to release, MMU does that now */

		/* wait for MMU getting ready (low) */
		while (REG16(ETH_BASE+MMU_CMD_REG) & MC_BUSY) {
			udelay (10);
		}

		printf ("MMU ready\n");


		return;
	} else {
		/* ack. int */
		REG8(ETH_BASE+SMC91111_INT_REG)=IM_TX_EMPTY_INT;
		/* SMC_outb (IM_TX_INT, SMC91111_INT_REG); */
		#ifdef DEBUG
		printf ("Sent packet of length %u\n",length);
		#endif

		/* release packet */
		/* no need to release, MMU does that now */

		/* wait for MMU getting ready (low) */
		while (REG16(ETH_BASE+MMU_CMD_REG) & MC_BUSY) {
			udelay (10);
		}

	}

	/* restore previously saved registers */
	REG8(ETH_BASE+PN_REG)=saved_pnr;
	REG16(ETH_BASE+PTR_REG)=saved_ptr, PTR_REG;

	return;

}

#endif
