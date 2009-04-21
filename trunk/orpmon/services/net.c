/*
 *	Copied from Linux Monitor (LiMon) - Networking.
 *
 *	Copyright 1994 - 2000 Neil Russell.
 *	(See License)
 *	Copyright 2000 Roland Borde
 *	Copyright 2000 Paolo Scaffardi
 *	Copyright 2000, 2001 Wolfgang Denk
 */

/*
 * General Desription:
 *
 * The user interface supports commands for BOOTP, RARP, and TFTP.
 * Also, we support ARP internally. Depending on available data,
 * these interact as follows:
 *
 * BOOTP:
 *
 *	Prerequisites:	- own ethernet address
 *	We want:	- own IP address
 *			- TFTP server IP address
 *			- name of bootfile
 *	Next step:	ARP
 *
 * RARP:
 *
 *	Prerequisites:	- own ethernet address
 *	We want:	- own IP address
 *			- TFTP server IP address
 *	Next step:	ARP
 *
 * ARP:
 *
 *	Prerequisites:	- own ethernet address
 *			- own IP address
 *			- TFTP server IP address
 *	We want:	- TFTP server ethernet address
 *	Next step:	TFTP
 *
 * DHCP:
 *
 *     Prerequisites:   - own ethernet address
 *     We want:         - IP, Netmask, ServerIP, Gateway IP
 *                      - bootfilename, lease time
 *     Next step:       - TFTP
 *
 * TFTP:
 *
 *	Prerequisites:	- own ethernet address
 *			- own IP address
 *			- TFTP server IP address
 *			- TFTP server ethernet address
 *			- name of bootfile (if unknown, we use a default name
 *			  derived from our own IP address)
 *	We want:	- load the boot file
 *	Next step:	none
 */


#include "common.h"
#include "support.h"
#include "net.h"
#include "bootp.h"
#include "tftp.h"
#include "rarp.h"
#include "arp.h"
#if OC_LAN==1
  #include "eth.h"
#else if SMC91111_LAN==1
  #include "smc91111.h"
#endif

#if 0
#define ET_DEBUG
#endif

/** BOOTP EXTENTIONS **/

IPaddr_t	NetOurSubnetMask=0;		/* Our subnet mask (0=unknown)	*/
IPaddr_t	NetOurGatewayIP=0;		/* Our gateways IP address	*/
IPaddr_t	NetOurDNSIP=0;			/* Our DNS IP address		*/
char		NetOurNISDomain[32]={0,};	/* Our NIS domain		*/
char		NetOurHostName[32]={0,};	/* Our hostname			*/
char		NetOurRootPath[64]={0,};	/* Our bootpath			*/
unsigned short		NetBootFileSize=0;		/* Our bootfile size in blocks	*/

/** END OF BOOTP EXTENTIONS **/

unsigned long		NetBootFileXferSize;	/* The actual transferred size of the bootfile (in bytes) */
unsigned char		NetOurEther[6];		/* Our ethernet address			*/
unsigned char		NetServerEther[6] =	/* Boot server enet address		*/
			{ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
IPaddr_t	NetOurIP;		/* Our IP addr (0 = unknown)		*/
IPaddr_t	NetServerIP;		/* Our IP addr (0 = unknown)		*/
volatile unsigned char *NetRxPkt;		/* Current receive packet		*/
int		NetRxPktLen;		/* Current rx packet length		*/
unsigned	NetIPID;		/* IP packet ID				*/
unsigned char		NetBcastAddr[6] =	/* Ethernet bcast address		*/
			{ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
int		NetState;		/* Network loop state			*/

char		BootFile[128];		/* Boot File name			*/

volatile unsigned char	PktBuf[(PKTBUFSRX+1) * PKTSIZE_ALIGN + PKTALIGN];

volatile unsigned char *NetRxPackets[PKTBUFSRX]; /* Receive packets			*/

static rxhand_f *packetHandler;		/* Current RX packet handler		*/
static thand_f *timeHandler;		/* Current timeout handler		*/
static unsigned long	timeValue;		/* Current timeout value		*/
volatile unsigned char *NetTxPacket = 0;	/* THE transmit packet			*/

static int net_check_prereq (proto_t protocol);

/**********************************************************************/
/*
 *	Main network processing loop.
 */
int
NetLoop(proto_t protocol)
{
#if 1
	if (!NetTxPacket) {
		int	i;

		/*
		 *	Setup packet buffers, aligned correctly.
		 */
		NetTxPacket = &PktBuf[0] + (PKTALIGN - 1);
		NetTxPacket -= (unsigned long)NetTxPacket % PKTALIGN;
		for (i = 0; i < PKTBUFSRX; i++) {
			NetRxPackets[i] = NetTxPacket + (i+1)*PKTSIZE_ALIGN;
		}
	}

	eth_halt();
	eth_init(NetReceive);

restart:

	NetCopyEther(NetOurEther, global.eth_add);

	NetState = NETLOOP_CONTINUE;

	/*
	 *	Start the ball rolling with the given start function.  From
	 *	here on, this code is a state machine driven by received
	 *	packets and timer events.
	 */

	if (protocol == TFTP) {			/* TFTP */
		NetOurIP	= global.ip;
		NetServerIP	= global.srv_ip;
		NetOurGatewayIP = global.gw_ip;
		NetOurSubnetMask= global.mask;

		if (net_check_prereq (protocol) != 0) {
			return 0;
		}

		/* always use ARP to get server ethernet address */
 		ArpTry = 0;
		ArpRequest ();

#if (CONFIG_COMMANDS & CFG_CMD_DHCP)
	} else if (protocol == DHCP) {
		if (net_check_prereq (protocol) != 0) {
			return 0;
		}

		/* Start with a clean slate... */
		NetOurIP = 0;
		NetServerIP = 0;
		DhcpRequest();		/* Basically same as BOOTP */

#endif	/* CFG_CMD_DHCP */

	} else {				/* BOOTP or RARP */

		/*
                 * initialize our IP addr to 0 in order to accept ANY
                 * IP addr assigned to us by the BOOTP / RARP server
		 */
		NetOurIP = 0;
		NetServerIP = 0;

		if (net_check_prereq (protocol) != 0) {
			return 0;
		}
#ifdef BOOTP
		if (protocol == BOOTP) {
			BootpTry = 0;
			BootpRequest ();
		} 
#endif
#ifdef RARP
    if {
			RarpTry	 = 0;
			RarpRequest ();
		}
#endif
	}

	NetBootFileXferSize = 0;

	/*
	 *	Main packet reception loop.  Loop receiving packets until
	 *	someone sets `NetQuit'.
	 */
	for (;;) {
//		WATCHDOG_RESET();
		/*
		 *	Check the ethernet for a new packet.  The ethernet
		 *	receive routine will process it.
		 */
			eth_rx();

		/*
		 *	Abort if ctrl-c was pressed.
		 */
		if (ctrlc()) {
		    eth_halt();
			printf("\nAbort\n");
			return 0;
		}


		/*
		 *	Check for a timeout, and run the timeout handler
		 *	if we have one.
		 */
		if (timeHandler && (get_timer(0) > timeValue)) {
			thand_f *x;

			x = timeHandler;
			timeHandler = (thand_f *)0;
			(*x)();
		}


		switch (NetState) {

		case NETLOOP_RESTART:
			goto restart;

		case NETLOOP_SUCCESS:
			if (NetBootFileXferSize > 0) {
				printf("Bytes transferred = %ld (%lx hex)\n",
					NetBootFileXferSize,
					NetBootFileXferSize);
			}
			eth_halt();
			return NetBootFileXferSize;

		case NETLOOP_FAIL:
			return 0;
		}
	}
#endif
}

/**********************************************************************/


#if 1
void
NetStartAgain(void)
{
	NetState = NETLOOP_RESTART;
}

/**********************************************************************/
/*
 *	Miscelaneous bits.
 */

void
NetSetHandler(rxhand_f * f)
{
	packetHandler = f;
}


void
NetSetTimeout(int iv, thand_f * f)
{
	if (iv == 0) {
		timeHandler = (thand_f *)0;
	} else {
		timeHandler = f;
		timeValue = get_timer(0) + iv;
	}
}


void
NetSendPacket(volatile unsigned char * pkt, int len)
{

#if OC_LAN==1
  unsigned char *p;

  p = eth_get_tx_buf();
  memcpy(p, (void *)pkt, len);
  eth_send(p, len);
#else if SMC91111_LAN==1
  eth_send(pkt, len);
#endif
}



void
NetReceive(volatile unsigned char * pkt, int len)
{
	Ethernet_t *et;
	IP_t	*ip;
	ARP_t	*arp;
	int	x;


	NetRxPkt = pkt;
	NetRxPktLen = len;
	et = (Ethernet_t *)pkt;

	x = SWAP16(et->et_protlen);

	if (x < 1514) {
		/*
		 *	Got a 802 packet.  Check the other protocol field.
		 */
		x = SWAP16(et->et_prot);
		ip = (IP_t *)(pkt + E802_HDR_SIZE);
		len -= E802_HDR_SIZE;
	} else {
		ip = (IP_t *)(pkt + ETHER_HDR_SIZE);
		len -= ETHER_HDR_SIZE;
	}

#ifdef ET_DEBUG
	printf("Receive from protocol 0x%x\n", x);
#endif

	switch (x) {

	case PROT_ARP:
		/*
		 * We have to deal with two types of ARP packets:
                 * - REQUEST packets will be answered by sending  our
                 *   IP address - if we know it.
                 * - REPLY packates are expected only after we asked
                 *   for the TFTP server's or the gateway's ethernet
                 *   address; so if we receive such a packet, we set
                 *   the server ethernet address
		 */
#ifdef ET_DEBUG
		printf("Got ARP\n");
#endif
		arp = (ARP_t *)ip;
		if (len < ARP_HDR_SIZE) {
			printf("bad length %d < %d\n", len, ARP_HDR_SIZE);
			return;
		}
		if (SWAP16(arp->ar_hrd) != ARP_ETHER) {
			return;
		}
		if (SWAP16(arp->ar_pro) != PROT_IP) {
			return;
		}
		if (arp->ar_hln != 6) {
			return;
		}
		if (arp->ar_pln != 4) {
			return;
		}

		if (NetOurIP == 0 ||
		    *((IPaddr_t *)&arp->ar_data[16]) != NetOurIP) {
			return;
		}

		switch (SWAP16(arp->ar_op)) {
		case ARPOP_REQUEST:		/* reply with our IP address	*/
#ifdef ET_DEBUG
			printf("Got ARP REQUEST, return our IP\n");
#endif
			NetSetEther((unsigned char *)et, et->et_src, PROT_ARP);
			arp->ar_op = SWAP16(ARPOP_REPLY);
			NetCopyEther(&arp->ar_data[10], &arp->ar_data[0]);
			NetCopyEther(&arp->ar_data[0], NetOurEther);
			*(IPaddr_t *)(&arp->ar_data[16]) =
						*(IPaddr_t *)(&arp->ar_data[6]);
			*(IPaddr_t *)(&arp->ar_data[6]) = NetOurIP;
			NetSendPacket((unsigned char *)et,((unsigned char *)arp-pkt)+ARP_HDR_SIZE);
			return;
		case ARPOP_REPLY:		/* set TFTP server eth addr	*/
#ifdef ET_DEBUG
			printf("Got ARP REPLY, set server/gtwy eth addr\n");
#endif
			NetCopyEther(NetServerEther, &arp->ar_data[0]);
			(*packetHandler)(0,0,0,0);	/* start TFTP */
			return;
		default:
#ifdef ET_DEBUG
			printf("Unexpected ARP opcode 0x%x\n", SWAP16(arp->ar_op));
#endif
			return;
		}

	case PROT_RARP:
#ifdef ET_DEBUG
		printf("Got RARP\n");
#endif
		arp = (ARP_t *)ip;
		if (len < ARP_HDR_SIZE) {
			printf("bad length %d < %d\n", len, ARP_HDR_SIZE);
			return;
		}

		if ((SWAP16(arp->ar_op) != RARPOP_REPLY) ||
			(SWAP16(arp->ar_hrd) != ARP_ETHER)   ||
			(SWAP16(arp->ar_pro) != PROT_IP)     ||
			(arp->ar_hln != 6) || (arp->ar_pln != 4)) {

			printf("invalid RARP header\n");
		} else {
			NetOurIP = *((IPaddr_t *)&arp->ar_data[16]);
			NetServerIP = *((IPaddr_t *)&arp->ar_data[6]);
			NetCopyEther(NetServerEther, &arp->ar_data[0]);

			(*packetHandler)(0,0,0,0);
		}
		break;

	case PROT_IP:
#ifdef ET_DEBUG
		printf("Got IP\n");
#endif
		if (len < IP_HDR_SIZE) {
			debug ("len bad %d < %d\n", len, IP_HDR_SIZE);
			return;
		}
		if (len < SWAP16(ip->ip_len)) {
			printf("len bad %d < %d\n", len, SWAP16(ip->ip_len));
			return;
		}
		len = SWAP16(ip->ip_len);
#ifdef ET_DEBUG
		printf("len=%d, v=%02x\n", len, ip->ip_hl_v & 0xff);
#endif
		if ((ip->ip_hl_v & 0xf0) != 0x40) {
			return;
		}
		if (ip->ip_off & SWAP16c(0x1fff)) { /* Can't deal w/ fragments */
			return;
		}
		if (!NetCksumOk((unsigned char *)ip, IP_HDR_SIZE_NO_UDP / 2)) {
			printf("checksum bad\n");
			return;
		}
		if (NetOurIP &&
		    ip->ip_dst != NetOurIP &&
		    ip->ip_dst != 0xFFFFFFFF) {
			return;
		}
		/*
		 * watch for ICMP host redirects
		 *
                 * There is no real handler code (yet). We just watch
                 * for ICMP host redirect messages. In case anybody
                 * sees these messages: please contact me
                 * (wd@denx.de), or - even better - send me the
                 * necessary fixes :-)
		 *
                 * Note: in all cases where I have seen this so far
                 * it was a problem with the router configuration,
                 * for instance when a router was configured in the
                 * BOOTP reply, but the TFTP server was on the same
                 * subnet. So this is probably a warning that your
                 * configuration might be wrong. But I'm not really
                 * sure if there aren't any other situations.
		 */
		if (ip->ip_p == IPPROTO_ICMP) {
			ICMP_t *icmph = (ICMP_t *)&(ip->udp_src);

			if (icmph->type != ICMP_REDIRECT)
				return;
			if (icmph->code != ICMP_REDIR_HOST)
				return;
			printf (" ICMP Host Redirect to ");
			print_IPaddr(icmph->un.gateway);
			putc(' ');
		} else if (ip->ip_p != IPPROTO_UDP) {	/* Only UDP packets */
			return;
		}

		/*
		 *	IP header OK.  Pass the packet to the current handler.
		 */
		(*packetHandler)((unsigned char *)ip +IP_HDR_SIZE,
						SWAP16(ip->udp_dst),
						SWAP16(ip->udp_src),
						SWAP16(ip->udp_len) - 8);

		break;
	}
}


/**********************************************************************/

static int net_check_prereq (proto_t protocol)
{
	switch (protocol) {
	case ARP:	/* nothing to do */
			break;

	case TFTP:
			if (NetServerIP == 0) {
			  printf	 ("*** ERROR: `serverip' not set\n");
				return (1);
			}

			if (NetOurIP == 0) {
				printf ("*** ERROR: `ipaddr' not set\n");
				return (1);
			}
			/* Fall through */

	case DHCP:
	case RARP:
	case BOOTP:
			if (memcmp(NetOurEther, "\0\0\0\0\0\0", 6) == 0) {
				printf ("*** ERROR: `ethaddr' not set\n");
				return (1);
			}
			/* Fall through */
	}
	return (0);	/* OK */
}
/**********************************************************************/

int
NetCksumOk(unsigned char * ptr, int len)
{
	return !((NetCksum(ptr, len) + 1) & 0xfffe);
}


unsigned
NetCksum(unsigned char * ptr, int len)
{
	unsigned long	xsum;

	xsum = 0;
	while (len-- > 0)
		xsum += *((unsigned short *)ptr)++;
	xsum = (xsum & 0xffff) + (xsum >> 16);
	xsum = (xsum & 0xffff) + (xsum >> 16);
	return (xsum & 0xffff);
}


void
NetCopyEther(volatile unsigned char * to, unsigned char * from)
{
	int	i;

	for (i = 0; i < 6; i++)
		*to++ = *from++;
}


void
NetSetEther(volatile unsigned char * xet, unsigned char * addr, unsigned long prot)
{
	volatile Ethernet_t *et = (Ethernet_t *)xet;

	NetCopyEther(et->et_dest, addr);
	NetCopyEther(et->et_src, NetOurEther);
	et->et_protlen = SWAP16(prot);
}


void
NetSetIP(volatile unsigned char * xip, IPaddr_t dest, int dport, int sport, int len)
{
	volatile IP_t *ip = (IP_t *)xip;

	/*
	 *	If the data is an odd number of bytes, zero the
	 *	byte after the last byte so that the checksum
	 *	will work.
	 */
	if (len & 1)
		xip[IP_HDR_SIZE + len] = 0;

	/*
	 *	Construct an IP and UDP header.
			(need to set no fragment bit - XXX)
	 */
	ip->ip_hl_v  = 0x45;		/* IP_HDR_SIZE / 4 (not including UDP) */
	ip->ip_tos   = 0;
	ip->ip_len   = SWAP16(IP_HDR_SIZE + len);
	ip->ip_id    = SWAP16(NetIPID++);
	ip->ip_off   = SWAP16c(0x4000);	/* No fragmentation */
	ip->ip_ttl   = 255;
	ip->ip_p     = 17;		/* UDP */
	ip->ip_sum   = 0;
	ip->ip_src   = NetOurIP;
	ip->ip_dst   = dest;
	ip->udp_src  = SWAP16(sport);
	ip->udp_dst  = SWAP16(dport);
	ip->udp_len  = SWAP16(8 + len);
	ip->udp_xsum = 0;
	ip->ip_sum   = ~NetCksum((unsigned char *)ip, IP_HDR_SIZE_NO_UDP / 2);
}

void copy_filename (unsigned char *dst, unsigned char *src, int size)
{
	if (*src && (*src == '"')) {
		++src;
		--size;
	}

	while ((--size > 0) && *src && (*src != '"')) {
		*dst++ = *src++;
	}
	*dst = '\0';
}

void ip_to_string (IPaddr_t x, char *s)
{
    char num[] = "0123456789ABCDEF";
    int i;

    x = SWAP32(x);
    
    for(i = 28; i >= 0; i -= 4)
      *s++ = num[((x >> i) & 0x0f)];
    *s = 0;
}

void print_IPaddr (IPaddr_t x)
{
    char tmp[12];

    ip_to_string(x, tmp);

    printf(tmp);
}

static unsigned int i2a(char* dest,unsigned int x) {
  register unsigned int tmp=x;
  register unsigned int len=0;
  if (x>=100) { *dest++=tmp/100+'0'; tmp=tmp%100; ++len; }
  if (x>=10) { *dest++=tmp/10+'0'; tmp=tmp%10; ++len; }
  *dest++=tmp+'0';
  return len+1;
}

char *inet_ntoa(unsigned long in) {
  static char buf[20];
  unsigned int len;
  unsigned char *ip=(unsigned char*)&in;

  len=i2a(buf,ip[0]); buf[len]='.'; ++len;
  len+=i2a(buf+ len,ip[1]); buf[len]='.'; ++len;
  len+=i2a(buf+ len,ip[2]); buf[len]='.'; ++len;
  len+=i2a(buf+ len,ip[3]); buf[len]=0;
  return buf;
}

unsigned long inet_aton(const char *cp)
{
  unsigned long a[4];
  unsigned long ret;
  char *p = (char *)cp;
  int i,d;
  if (strcmp(cp, "255.255.255.255") == 0)
    return -1;
 
  for(i = 0; i < 4; i++) {
    a[i] = strtoul(p, 0, 0);
    for(d=1; (p[d] != '.') && (i < 3); d++);
    p = &p[d+1];
  }

  ret = (a[0] << 24) | (a[1] << 16) | (a[2] << 8) | a[3]; 
  return ret;
}

#endif
