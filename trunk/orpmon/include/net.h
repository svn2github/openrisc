/*
 *	LiMon Monitor (LiMon) - Network.
 *
 *	Copyright 1994 - 2000 Neil Russell.
 *	(See License)
 *
 *
 * History
 *	9/16/00	  bor  adapted to TQM823L/STK8xxL board, RARP/TFTP boot added
 */

#ifndef __NET_H__
#define __NET_H__


// TFTP running checksum defined here... tftp.h not in common include/ path
extern unsigned long TFTP_CHKSUM;

/*
 *	The number of receive packet buffers, and the required packet buffer
 *	alignment in memory.
 *
 */

#define PKTBUFSRX	4
#define PKTALIGN	32

/****** from cpu_arch.h ************/

/* Byte swapping stuff (not needed on PPC). */

#define SWAP16(x)	(x)
#define SWAP16c(x)	(x)
#define SWAP32(x)	(x)

/****** end from cpu_arch.h **************/

typedef unsigned long		IPaddr_t;



/*
 * The current receive packet handler.  Called with a pointer to the
 * application packet, and a protocol type (PORT_BOOTPC or PORT_TFTP).
 * All other packets are dealt with without calling the handler.
 */
typedef void	rxhand_f(unsigned char *, unsigned, unsigned, unsigned);

/*
 *	A timeout handler.  Called after time interval has expired.
 */
typedef void	thand_f(void);

#ifdef CONFIG_NET_MULTI

#define NAMESIZE 16

enum eth_state_t {
	ETH_STATE_INIT,
	ETH_STATE_PASSIVE,
	ETH_STATE_ACTIVE
};

struct eth_device {
	char name[NAMESIZE];
	unsigned char enetaddr[6];
	int iobase;
	int state;

	int  (*init) (struct eth_device*, bd_t*);
	int  (*send) (struct eth_device*, volatile void* pachet, int length);
	int  (*recv) (struct eth_device*);
	void (*halt) (struct eth_device*);

	struct eth_device *next;
	void *priv;
};

extern int eth_initialize(bd_t *bis);		/* Initialize network subsystem */
extern int eth_register(struct eth_device* dev);/* Register network device	*/
extern void eth_try_another(void);		/* Change the device		*/
#endif

/**********************************************************************/
/*
 *	Protocol headers.
 */

/*
 *	Ethernet header
 */
typedef struct {
	unsigned char		et_dest[6];	/* Destination node		*/
	unsigned char		et_src[6];	/* Source node			*/
	unsigned short		et_protlen;	/* Protocol or length		*/
	unsigned char		et_dsap;	/* 802 DSAP			*/
	unsigned char		et_ssap;	/* 802 SSAP			*/
	unsigned char		et_ctl;		/* 802 control			*/
	unsigned char		et_snap1;	/* SNAP				*/
	unsigned char		et_snap2;
	unsigned char		et_snap3;
	unsigned short		et_prot;	/* 802 protocol			*/
} Ethernet_t;

#define ETHER_HDR_SIZE	14		/* Ethernet header size		*/
#define E802_HDR_SIZE	22		/* 802 ethernet header size	*/
#define PROT_IP		0x0800		/* IP protocol			*/
#define PROT_ARP	0x0806		/* IP ARP protocol		*/
#define PROT_RARP	0x8035		/* IP ARP protocol		*/

#define IPPROTO_ICMP	 1	/* Internet Control Message Protocol	*/
#define IPPROTO_UDP	17	/* User Datagram Protocol		*/

/*
 *	Internet Protocol (IP) header.
 */
typedef struct {
	unsigned char		ip_hl_v;	/* header length and version	*/
	unsigned char		ip_tos;		/* type of service		*/
	unsigned short		ip_len;		/* total length			*/
	unsigned short		ip_id;		/* identification		*/
	unsigned short		ip_off;		/* fragment offset field	*/
	unsigned char		ip_ttl;		/* time to live			*/
	unsigned char		ip_p;		/* protocol			*/
	unsigned short		ip_sum;		/* checksum			*/
	IPaddr_t	ip_src;		/* Source IP address		*/
	IPaddr_t	ip_dst;		/* Destination IP address	*/
	unsigned short		udp_src;	/* UDP source port		*/
	unsigned short		udp_dst;	/* UDP destination port		*/
	unsigned short		udp_len;	/* Length of UDP packet		*/
	unsigned short		udp_xsum;	/* Checksum			*/
} IP_t;

#define IP_HDR_SIZE_NO_UDP	(sizeof (IP_t) - 8)
#define IP_HDR_SIZE		(sizeof (IP_t))


/*
 *	Address Resolution Protocol (ARP) header.
 */
typedef struct
{
	unsigned short		ar_hrd;		/* Format of hardware address*/
#   define ARP_ETHER	    1		        /* Ethernet  hardware address*/
	unsigned short		ar_pro;		/* Format of protocol address*/
	unsigned char		ar_hln;		/* Length of hardware address*/
	unsigned char		ar_pln;		/* Length of protocol address*/
	unsigned short		ar_op;		/* Operation		*/
#   define ARPOP_REQUEST    1		/* Request  to resolve  address	*/
#   define ARPOP_REPLY	    2		/* Response to previous request	*/

#   define RARPOP_REQUEST   3		/* Request  to resolve  address	*/
#   define RARPOP_REPLY	    4		/* Response to previous request */

	/*
         * The remaining fields are variable in size, according to
         * the sizes above, and are defined as appropriate for
         * specific hardware/protocol combinations.
	 */
	unsigned char		ar_data[0];
#if 0
	unsigned char		ar_sha[];	/* Sender hardware address*/
	unsigned char		ar_spa[];	/* Sender protocol address*/
	unsigned char		ar_tha[];	/* Target hardware address*/
	unsigned char		ar_tpa[];	/* Target protocol address*/
#endif /* 0 */
} ARP_t;

#define ARP_HDR_SIZE	(8+20)		/* Size assuming ethernet	*/

/*
 * ICMP stuff (just enough to handle (host) redirect messages)
 */
#define ICMP_REDIRECT		5	/* Redirect (change route)	*/

/* Codes for REDIRECT. */
#define ICMP_REDIR_NET		0	/* Redirect Net			*/
#define ICMP_REDIR_HOST		1	/* Redirect Host		*/

typedef struct icmphdr {
	unsigned char		type;
	unsigned char		code;
	unsigned short		checksum;
	union {
		struct {
			unsigned short	id;
			unsigned short	sequence;
		} echo;
		unsigned long	gateway;
		struct {
			unsigned short	__unused;
			unsigned short	mtu;
		} frag;
	} un;
} ICMP_t;



/*
 * Maximum packet size; used to allocate packet storage.
 * TFTP packets can be 524 bytes + IP header + ethernet header.
 * Lets be conservative, and go for 38 * 16.  (Must also be
 * a multiple of 32 bytes).
 */
/*
 * AS.HARNOIS : Better to set PKTSIZE to maximum size because
 * traffic type is not always controlled
 * maximum packet size =  1518
 * maximum packet size and multiple of 32 bytes =  1536
 */
#define PKTSIZE			1518
#define PKTSIZE_ALIGN		1536
/*#define PKTSIZE		608*/

/*
 * Maximum receive ring size; that is, the number of packets
 * we can buffer before overflow happens. Basically, this just
 * needs to be enough to prevent a packet being discarded while
 * we are processing the previous one.
 */
#define RINGSZ		4
#define RINGSZ_LOG2	2

/**********************************************************************/
/*
 *	Globals.
 */

/* net.c */
/** BOOTP EXTENTIONS **/
extern IPaddr_t		NetOurGatewayIP;	/* Our gateway IP addresse	*/
extern IPaddr_t		NetOurSubnetMask;	/* Our subnet mask (0 = unknown)*/
extern IPaddr_t		NetOurDNSIP;	 /* Our Domain Name Server (0 = unknown)*/
extern char		NetOurNISDomain[32];	/* Our NIS domain		*/
extern char		NetOurHostName[32];	/* Our hostname			*/
extern char		NetOurRootPath[64];	/* Our root path		*/
extern unsigned short		NetBootFileSize;	/* Our boot file size in blocks	*/
/** END OF BOOTP EXTENTIONS **/
extern unsigned long		NetBootFileXferSize;	/* size of bootfile in bytes	*/
extern unsigned char		NetOurEther[6];		/* Our ethernet address		*/
extern unsigned char		NetServerEther[6];	/* Boot server enet address	*/
extern IPaddr_t		NetOurIP;		/* Our    IP addr (0 = unknown)	*/
extern IPaddr_t		NetServerIP;		/* Server IP addr (0 = unknown)	*/
extern volatile unsigned char * NetTxPacket;		/* THE transmit packet		*/
extern volatile unsigned char * NetRxPackets[PKTBUFSRX];/* Receive packets		*/
extern volatile unsigned char * NetRxPkt;		/* Current receive packet	*/
extern int		NetRxPktLen;		/* Current rx packet length	*/
extern unsigned		NetIPID;		/* IP ID (counting)		*/
extern unsigned char		NetBcastAddr[6];	/* Ethernet boardcast address	*/

extern int		NetState;		/* Network loop state		*/
#define NETLOOP_CONTINUE	1
#define NETLOOP_RESTART		2
#define NETLOOP_SUCCESS		3
#define NETLOOP_FAIL		4


typedef enum { BOOTP, RARP, ARP, TFTP, DHCP } proto_t;

/* from net/net.c */
extern char	BootFile[128];			/* Boot File name		*/

/* Initialize the network adapter */
extern int	NetLoop(proto_t protocol);

/* Shutdown adapters and cleanup */
extern void	NetStop(void);

/* Load failed.	 Start again. */
extern void	NetStartAgain(void);

/* Copy ethernet address */
extern void	NetCopyEther(volatile unsigned char *, unsigned char *);

/* Set ethernet header */
extern void	NetSetEther(volatile unsigned char *, unsigned char *, unsigned long);

/* Set IP header */
extern void	NetSetIP(volatile unsigned char *, IPaddr_t, int, int, int);

/* Checksum */
extern int	NetCksumOk(unsigned char *, int);	/* Return true if cksum OK	*/
extern unsigned	NetCksum(unsigned char *, int);		/* Calculate the checksum	*/

/* Set callbacks */
extern void	NetSetHandler(rxhand_f *);	/* Set RX packet handler	*/
extern void	NetSetTimeout(int, thand_f *);	/* Set timeout handler		*/

/* Transmit "NetTxPacket" */
extern void	NetSendPacket(volatile unsigned char *, int);

/* Processes a received packet */
extern void	NetReceive(volatile unsigned char *, int);

/* Print an IP address on the console */
extern void	print_IPaddr (IPaddr_t);

/* Convert an IP address to a string */
extern void	ip_to_string (IPaddr_t x, char *s);

/* read an IP address from a environment variable */
extern IPaddr_t getenv_IPaddr (char *);

/* copy a filename (allow for "..." notation, limit length) */
extern void	copy_filename (unsigned char *dst, unsigned char *src, int size);

/* converts IP from unsigned long to string */
extern char         *inet_ntoa(unsigned long);
extern unsigned long inet_aton(const char *);

/**********************************************************************/

#endif /* __NET_H__ */
