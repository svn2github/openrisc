/*
 *	Copyright 1994, 1995, 2000 Neil Russell.
 *	(See License)
 *	Copyright 2000, 2001 DENX Software Engineering, Wolfgang Denk, wd@denx.de
 */

#include "common.h"
#include "support.h"
#include "net.h"
#include "tftp.h"
#include "bootp.h"

#undef	ET_DEBUG

#define WELL_KNOWN_PORT	69		/* Well known TFTP port #		*/
#define TIMEOUT		2		/* Seconds to timeout for a lost pkt	*/
#define TIMEOUT_COUNT	10		/* # of timeouts before giving up	*/
					/* (for checking the image size)	*/
#define HASHES_PER_LINE	65		/* Number of "loading" hashes per line	*/

/*
 *	TFTP operations.
 */
#define TFTP_RRQ	1
#define TFTP_WRQ	2
#define TFTP_DATA	3
#define TFTP_ACK	4
#define TFTP_ERROR	5


static int	TftpServerPort;		/* The UDP port at their end		*/
static int	TftpOurPort;		/* The UDP port at our end		*/
static int	TftpTimeoutCount;
static unsigned	TftpBlock;
static unsigned	TftpLastBlock;
static int	TftpState;
#define STATE_RRQ	1
#define STATE_DATA	2
#define STATE_TOO_LARGE	3
#define STATE_BAD_MAGIC	4

char *tftp_filename;

#ifdef CFG_DIRECT_FLASH_TFTP
extern flash_info_t flash_info[CFG_MAX_FLASH_BANKS];
#endif

static __inline__ void
store_block (unsigned block, unsigned char * src, unsigned len)
{
	unsigned long offset = block * 512, newsize = offset + len;
#ifdef CFG_DIRECT_FLASH_TFTP
	int i, rc = 0;

	for (i=0; i<CFG_MAX_FLASH_BANKS; i++) {
		/* start address in flash? */
		if (global.src_addr + offset >= flash_info[i].start[0]) {
			rc = 1;
			break;
		}
	}

	if (rc) { /* Flash is destination for this packet */
		rc = flash_write ((unsigned char *)src, (unsigned long)(global.src_addr+offset), len);
		switch (rc) {
		case 0:	/* OK */
			break;
		case 1: printf ("Timeout writing to Flash\n");
			break;
		case 2:	printf ("Flash not Erased\n");
			break;
		case 4: printf ("Can't write to protected Flash sectors\n");
			break;
		case 8: printf ("Outside available Flash\n");
			break;
		case 16:printf ("Size must be aligned (multiple of 8?)\n");
			break;
		default:
			printf ("%s[%d] FIXME: rc=%d\n",__FILE__,__LINE__,rc);
			break;
		}
		if (rc) {
			NetState = NETLOOP_FAIL;
			return;
		}
	}
	else
#endif /* CFG_DIRECT_FLASH_TFTP */
	(void)memcpy((void *)(global.src_addr + offset), src, len);

	if (NetBootFileXferSize < newsize)
		NetBootFileXferSize = newsize;
}

static void TftpSend (void);
static void TftpTimeout (void);

/**********************************************************************/

static void
TftpSend (void)
{
	volatile unsigned char *	pkt;
	volatile unsigned char *	xp;
	int			len = 0;

	/*
	 *	We will always be sending some sort of packet, so
	 *	cobble together the packet headers now.
	 */
	pkt = NetTxPacket + ETHER_HDR_SIZE + IP_HDR_SIZE;

	switch (TftpState) {

	case STATE_RRQ:
		xp = pkt;
		*((unsigned short *)pkt)++ = SWAP16c(TFTP_RRQ);
		strcpy ((char *)pkt, tftp_filename);
		pkt += strlen(tftp_filename) + 1;
		strcpy ((char *)pkt, "octet");
		pkt += 5 /*strlen("octet")*/ + 1;
		len = pkt - xp;
		break;

	case STATE_DATA:
		xp = pkt;
		*((unsigned short *)pkt)++ = SWAP16c(TFTP_ACK);
		*((unsigned short *)pkt)++ = SWAP16(TftpBlock);
		len = pkt - xp;
		break;

	case STATE_TOO_LARGE:
		xp = pkt;
		*((unsigned short *)pkt)++ = SWAP16c(TFTP_ERROR);
		*((unsigned short *)pkt)++ = SWAP16(3);
		strcpy ((char *)pkt, "File too large");
		pkt += 14 /*strlen("File too large")*/ + 1;
		len = pkt - xp;
		break;

	case STATE_BAD_MAGIC:
		xp = pkt;
		*((unsigned short *)pkt)++ = SWAP16c(TFTP_ERROR);
		*((unsigned short *)pkt)++ = SWAP16(2);
		strcpy ((char *)pkt, "File has bad magic");
		pkt += 18 /*strlen("File has bad magic")*/ + 1;
		len = pkt - xp;
		break;
	}

	NetSetEther (NetTxPacket, NetServerEther, PROT_IP);
	NetSetIP (NetTxPacket + ETHER_HDR_SIZE, NetServerIP,
					TftpServerPort, TftpOurPort, len);
	NetSendPacket (NetTxPacket, ETHER_HDR_SIZE + IP_HDR_SIZE + len);
}


static void
TftpHandler (unsigned char * pkt, unsigned dest, unsigned src, unsigned len)
{
	if (dest != TftpOurPort) {
		return;
	}
	if (TftpState != STATE_RRQ && src != TftpServerPort) {
		return;
	}

	if (len < 2) {
		return;
	}
	len -= 2;
	switch (SWAP16(*((unsigned short *)pkt)++)) {

	case TFTP_RRQ:
	case TFTP_WRQ:
	case TFTP_ACK:
		break;
	default:
		break;

	case TFTP_DATA:
		if (len < 2)
			return;
		len -= 2;
		TftpBlock = SWAP16(*(unsigned short *)pkt);
		if (((TftpBlock - 1) % 10) == 0) {
			putc ('#');
			TftpTimeoutCount = 0;
		} else if ((TftpBlock % (10 * HASHES_PER_LINE)) == 0) {
			printf ("\n\t ");
		}

		if (TftpState == STATE_RRQ) {
			TftpState = STATE_DATA;
			TftpServerPort = src;
			TftpLastBlock = 0;

			if (TftpBlock != 1) {	/* Assertion */
				printf ("\nTFTP error: "
					"First block is not block 1 (%d)\n"
					"Starting again\n\n",
					TftpBlock);
				NetStartAgain ();
				break;
			}
		}

		if (TftpBlock == TftpLastBlock) {
			/*
			 *	Same block again; ignore it.
			 */
			break;
		}

		TftpLastBlock = TftpBlock;
		NetSetTimeout (TIMEOUT * TICKS_PER_SEC, TftpTimeout);

		store_block (TftpBlock - 1, pkt + 2, len);

		/*
		 *	Acknoledge the block just received, which will prompt
		 *	the server for the next one.
		 */
		TftpSend ();

		if (len < 512) {
			/*
			 *	We received the whole thing.  Try to
			 *	run it.
			 */
			printf ("\ndone\n");
			NetState = NETLOOP_SUCCESS;
		}
		break;

	case TFTP_ERROR:
		printf ("\nTFTP error: '%s' (%d)\n",
					pkt + 2, SWAP16(*(unsigned short *)pkt));
		printf ("Starting again\n\n");
		NetStartAgain ();
		break;
	}
}


static void
TftpTimeout (void)
{
	if (++TftpTimeoutCount >= TIMEOUT_COUNT) {
		printf ("\nRetry count exceeded; starting again\n");
		NetStartAgain ();
	} else {
		printf ("T ");
		NetSetTimeout (TIMEOUT * TICKS_PER_SEC, TftpTimeout);
		TftpSend ();
	}
}


void
TftpStart (void)
{
#ifdef ET_DEBUG
	printf ("\nServer ethernet address %02x:%02x:%02x:%02x:%02x:%02x\n",
		NetServerEther[0],
		NetServerEther[1],
		NetServerEther[2],
		NetServerEther[3],
		NetServerEther[4],
		NetServerEther[5]
	);
#endif /* DEBUG */

	printf ("TFTP from server ");	print_IPaddr (NetServerIP);
	printf ("; our IP address is ");	print_IPaddr (NetOurIP);

	// Check if we need to send across this subnet
	if (NetOurGatewayIP && NetOurSubnetMask) {
	    IPaddr_t OurNet 	= NetOurIP    & NetOurSubnetMask;
	    IPaddr_t ServerNet 	= NetServerIP & NetOurSubnetMask;

	    if (OurNet != ServerNet) {
		printf ("; sending through gateway ");
		print_IPaddr (NetOurGatewayIP) ;
	    }
	}
	putc ('\n');

	printf ("Filename '%s'.", tftp_filename);

	if (NetBootFileSize) {
	    printf (" Size is %d%s kB => %x Bytes",
		NetBootFileSize/2,
		(NetBootFileSize%2) ? ".5" : "",
		NetBootFileSize<<9);
	}

	putc ('\n');

	printf ("Load address: 0x%lx\n", global.src_addr);

	printf ("Loading: *\b");

	NetSetTimeout (TIMEOUT * TICKS_PER_SEC, TftpTimeout);
	NetSetHandler (TftpHandler);

	TftpServerPort = WELL_KNOWN_PORT;
	TftpTimeoutCount = 0;
	TftpState = STATE_RRQ;
	TftpOurPort = 1024 + (get_timer(0) % 3072);

	TftpSend ();
}

