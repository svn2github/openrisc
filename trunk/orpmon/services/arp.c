/*
 * (C) Copyright 2000
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include "common.h"
#include "support.h"
#include "net.h"
#include "bootp.h"
#include "tftp.h"
#include "arp.h"

#define TIMEOUT		5		/* Seconds before trying ARP again */
#define TIMEOUT_COUNT	1		/* # of timeouts before giving up  */

static void ArpHandler(unsigned char *pkt, unsigned dest, unsigned src, unsigned len);
static void ArpTimeout(void);

int	ArpTry = 0;

/*
 *	Handle a ARP received packet.
 */
static void
ArpHandler(unsigned char *pkt, unsigned dest, unsigned src, unsigned len)
{
	/* Check if the frame is really an ARP reply */
	if (memcmp (NetServerEther, NetBcastAddr, 6) != 0) {
#ifdef	DEBUG
		printf("Got good ARP - start TFTP\n");
#endif
		TftpStart ();
	}
}


/*
 *	Timeout on ARP request.
 */
static void
ArpTimeout(void)
{
	if (ArpTry >= TIMEOUT_COUNT) {
		printf("\nRetry count exceeded; starting again\n");
		NetStartAgain ();
	} else {
		NetSetTimeout (TIMEOUT * TICKS_PER_SEC, ArpTimeout);
		ArpRequest ();
	}
}


void
ArpRequest (void)
{
	int i;
	volatile unsigned char *pkt;
	ARP_t *	arp;
#ifdef	DEBUG
	printf("ARP broadcast %d\n", ++ArpTry);
#endif	
	pkt = NetTxPacket;

	NetSetEther(pkt, NetBcastAddr, PROT_ARP);
	pkt += ETHER_HDR_SIZE;

	arp = (ARP_t *)pkt;

	arp->ar_hrd = ARP_ETHER;
	arp->ar_pro = PROT_IP;
	arp->ar_hln = 6;
	arp->ar_pln = 4;
	arp->ar_op  = ARPOP_REQUEST;
	NetCopyEther(&arp->ar_data[0], NetOurEther);	/* source ET addr	*/
	*(IPaddr_t *)(&arp->ar_data[6]) = NetOurIP;	/* source IP addr	*/
	for (i=10; i<16; ++i) {
		arp->ar_data[i] = 0;			/* dest ET addr = 0	*/
	}

	if((NetServerIP & NetOurSubnetMask) != (NetOurIP & NetOurSubnetMask)) {
	    *(IPaddr_t *)(&arp->ar_data[16]) = NetOurGatewayIP;
	} else {
	    *((IPaddr_t *)(&(arp->ar_data[16]))) = NetServerIP;
	}

	NetSendPacket(NetTxPacket, ETHER_HDR_SIZE + ARP_HDR_SIZE);

	NetSetTimeout(TIMEOUT * TICKS_PER_SEC, ArpTimeout);
	NetSetHandler(ArpHandler);
}

