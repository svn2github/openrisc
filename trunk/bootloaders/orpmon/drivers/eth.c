#include "board.h"
#if OC_LAN==1

#include "common.h"
#include "support.h"
#include "uart.h"
#include "eth.h"
#include "int.h"
#include "spr-defs.h"

//#define PRINT_PACKETS
//#define ETHPHY_10MBPS
//#define ETHPHY_RESET_ON_INIT

extern int printf (const char *fmt, ...);
extern void lolev_ie(void);
extern void lolev_idis(void);

int tx_next;  /* Next buffer to be given to the user */
int tx_last;  /* Next buffer to be checked if packet sent */
int tx_full;
int rx_next;  /* Next buffer to be checked for new packet and given to the user */
void (*receive)(volatile unsigned char *add, int len); /* Pointer to function to be called 
                                        when frame is received */
int eth_monitor_enabled;

static void
print_packet(unsigned long add, int len)
{
  int truncate = (len > 256);
  int length_to_print = truncate ? 256 : len;

  int i;
  printf("\nipacket: add = %lx len = %d\n", add, len);
  for(i = 0; i < length_to_print; i++) {
    if(!(i % 8))
      printf(" ");
    if(!(i % 16))
      printf("\n");
    printf(" %.2x", *(((unsigned char *)add) + i));

  }
  printf("\n");

  if (truncate)
    printf("\ttruncated....\n");
  /*
  int i;
  printf("ipacket: add = %lx len = %d\n", add, len);
  for(i = 0; i < len; i++) {
      if(!(i % 16)) printf("\n");
      else if(!(i % 8)) printf(" "); // space here, so output is like wireshark
      printf(" %.2x", *(((unsigned char *)add) + i));
  }
  printf("\n");
  */
}

void init_tx_bd_pool(void)
{
  eth_bd  *bd;
  int i;
  unsigned long eth_data_base = ETH_DATA_BASE;

  bd = (eth_bd *)ETH_BD_BASE;

  for(i = 0; i < ETH_TXBD_NUM; i++) {
    /* Set Tx BD status */
    bd[i].len_status = 0 << 16 | ETH_TX_BD_PAD | ETH_TX_BD_CRC | ETH_RX_BD_IRQ;

    /* Initialize Tx buffer pointer */
    bd[i].addr = eth_data_base + (i * ETH_MAXBUF_LEN);
  }

  bd[i-1].len_status |= ETH_TX_BD_WRAP; // Last Tx BD - Wrap
}

void init_rx_bd_pool(void)
{
  eth_bd  *bd;
  int i;
  /* Set ethernet data buffers just above that of stack */
  unsigned long eth_data_base = ETH_DATA_BASE;

  bd = (eth_bd *)ETH_BD_BASE + ETH_TXBD_NUM;

  for(i = 0; i < ETH_RXBD_NUM; i++){

    /* Set Rx BD status */
    bd[i].len_status = 0 << 16 | ETH_RX_BD_EMPTY | ETH_RX_BD_IRQ;

    /* Initialize Rx buffer pointer */
    bd[i].addr = eth_data_base + ((ETH_TXBD_NUM + i) * ETH_MAXBUF_LEN);
  }

  bd[i-1].len_status |= ETH_RX_BD_WRAP; // Last Rx BD - Wrap
}


void eth_traffic_monitor(void)
{
 volatile oeth_regs *regs;
  regs = (oeth_regs *)(ETH_REG_BASE);
  
  uint	int_events;
  int serviced;
  int i;
  
  serviced = 0;
  
  /* Get the interrupt events that caused us to be here.
   */
  int_events = regs->int_src;
  regs->int_src = int_events;
  
  printf("\n=tx_ | %x | %x | %x | %x | %x | %x | %x | %x\n",
	 ((oeth_bd *)(ETH_BD_BASE))->len_status,
	 ((oeth_bd *)(ETH_BD_BASE+8))->len_status,
	 ((oeth_bd *)(ETH_BD_BASE+16))->len_status,
	 ((oeth_bd *)(ETH_BD_BASE+24))->len_status,
	 ((oeth_bd *)(ETH_BD_BASE+32))->len_status,
	 ((oeth_bd *)(ETH_BD_BASE+40))->len_status,
	 ((oeth_bd *)(ETH_BD_BASE+48))->len_status,
	 ((oeth_bd *)(ETH_BD_BASE+56))->len_status);
  
  printf("=rx_ | %x | %x | %x | %x | %x | %x | %x | %x\n",
	 ((oeth_bd *)(ETH_BD_BASE+64))->len_status,
	 ((oeth_bd *)(ETH_BD_BASE+64+8))->len_status,
	 ((oeth_bd *)(ETH_BD_BASE+64+16))->len_status,
	 ((oeth_bd *)(ETH_BD_BASE+64+24))->len_status,
	 ((oeth_bd *)(ETH_BD_BASE+64+32))->len_status,
	 ((oeth_bd *)(ETH_BD_BASE+64+40))->len_status,
	 ((oeth_bd *)(ETH_BD_BASE+64+48))->len_status,
	 ((oeth_bd *)(ETH_BD_BASE+64+56))->len_status);
  
  printf("=int | txb %d | txe %d | rxb %d | rxe %d | busy %d\n",
	 (int_events & ETH_INT_TXB) > 0,
	 (int_events & ETH_INT_TXE) > 0,
	 (int_events & ETH_INT_RXF) > 0,
	 (int_events & ETH_INT_RXE) > 0,
	 (int_events & ETH_INT_BUSY) > 0);


  /* Check for receive busy, i.e. packets coming but no place to
   * put them. 
   */
  if (int_events & ETH_INT_BUSY ) {
    serviced |= 0x4;
    printf("eth: busy!");
  }	

  /* Handle receive event
   */
  if (int_events & (ETH_INT_RXF | ETH_INT_RXE)) {
    serviced |= 0x1; 
    volatile oeth_bd *rx_bdp;
    rx_bdp = ((oeth_bd *)ETH_BD_BASE) + ETH_TXBD_NUM;
    /* Find RX buffers marked as having received data */
    for(i = 0; i < ETH_RXBD_NUM; i++)
      {
	if(!(rx_bdp[i].len_status & ETH_RX_BD_EMPTY))
	  {
	    print_packet(rx_bdp[i].addr, rx_bdp[i].len_status >> 16);
	    rx_bdp[i].len_status &= ~ETH_RX_BD_STATS;  /* Clear stats */
	    rx_bdp[i].len_status |= ETH_RX_BD_EMPTY; /* Mark RX BD as empty */
	  }
      }
  }
  
  /* Handle transmit event
   */
  if (int_events & (ETH_INT_TXB | ETH_INT_TXE)) {
    serviced |= 0x2;
    volatile oeth_bd *tx_bd;
    tx_bd = (volatile oeth_bd *)ETH_BD_BASE; /* Search from beginning*/
    for(i = 0; i < ETH_TXBD_NUM; i++)
      {
	/* Looking for buffer NOT ready for transmit */
	if((!(tx_bd[i].len_status & (ETH_TX_BD_READY))) &&
	   (tx_bd[i].len_status & (ETH_TX_BD_IRQ)))
	  {
	    print_packet(tx_bd[i].addr, (tx_bd[i].len_status >> 16));
	    tx_bd[i].len_status &= ~ETH_TX_BD_IRQ;
	  }
      }
    
  }
  
  if (serviced == 0)
    printf("\neth interrupt called but nothing serviced\n");
  
}


/* Ethernet interrupt handler */
void eth_int (void)
{
  printf("eth-int!\n");
  if (eth_monitor_enabled)
    eth_traffic_monitor();
  //REG32(ETH_REG_BASE + ETH_INT) = 0xffffffff;
  
}

void eth_reset_phy(void)
{
  /* Reset Eth PHY via MDIO*/
  printf("Resetting PHY, waiting for link");
  REG32(ETH_REG_BASE + ETH_MIIADDRESS) = (0<<8)|(ETH_MDIOPHYADDR&0xff);
  REG32(ETH_REG_BASE + ETH_MIITX_DATA) = 0x8000; // Bit 15 = 1: RESET
  REG32(ETH_REG_BASE + ETH_MIICOMMAND) = ETH_MIICOMMAND_WCTRLDATA;
  while(REG32(ETH_REG_BASE + ETH_MIISTATUS) & ETH_MIISTATUS_BUSY);
  
  sleep(100);

  while(1){
    printf(".");
    REG32(ETH_REG_BASE + ETH_MIIADDRESS) = (0<<8)|(ETH_MDIOPHYADDR&0xff);
    REG32(ETH_REG_BASE + ETH_MIICOMMAND) = ETH_MIICOMMAND_RSTAT;
    while(REG32(ETH_REG_BASE + ETH_MIISTATUS) & ETH_MIISTATUS_BUSY);
    if(!(REG32(ETH_REG_BASE + ETH_MIIRX_DATA) & 0x8000)) // If reset clear
      break;
  }
}


void eth_init (void (*rec)(volatile unsigned char *, int))
{
  /* Reset ethernet core */
  REG32(ETH_REG_BASE + ETH_MODER) = ETH_MODER_RST;    /* Reset ON */
  REG32(ETH_REG_BASE + ETH_MODER) &= ~ETH_MODER_RST;  /* Reset OFF */

#ifdef ETHPHY_RESET_ON_INIT
  REG32(ETH_REG_BASE + ETH_MIIADDRESS) = 0<<8; // BMCR reg
  REG32(ETH_REG_BASE + ETH_MIITX_DATA) = 0x8000; // RESET bit
  REG32(ETH_REG_BASE + ETH_MIICOMMAND) = ETH_MIICOMMAND_WCTRLDATA;
  while(REG32(ETH_REG_BASE + ETH_MIISTATUS) & ETH_MIISTATUS_BUSY);

  while(1){
    REG32(ETH_REG_BASE + ETH_MIIADDRESS) = 1<<8;
    REG32(ETH_REG_BASE + ETH_MIICOMMAND) = ETH_MIICOMMAND_RSTAT;
    while(REG32(ETH_REG_BASE + ETH_MIISTATUS) & ETH_MIISTATUS_BUSY);
    if(REG32(ETH_REG_BASE + ETH_MIIRX_DATA) & 0x04)
      break;
  }
#endif
    
#ifdef ETHPHY_10MBPS
  // Set PHY to 10 Mbps full duplex
  REG32(ETH_REG_BASE + ETH_MIIADDRESS) = 0<<8;
  REG32(ETH_REG_BASE + ETH_MIITX_DATA) = 0x0100;
  REG32(ETH_REG_BASE + ETH_MIICOMMAND) = ETH_MIICOMMAND_WCTRLDATA;
  while(REG32(ETH_REG_BASE + ETH_MIISTATUS) & ETH_MIISTATUS_BUSY);

  while(1){
    REG32(ETH_REG_BASE + ETH_MIIADDRESS) = 1<<8;
    REG32(ETH_REG_BASE + ETH_MIICOMMAND) = ETH_MIICOMMAND_RSTAT;
    while(REG32(ETH_REG_BASE + ETH_MIISTATUS) & ETH_MIISTATUS_BUSY);
    if(REG32(ETH_REG_BASE + ETH_MIIRX_DATA) & 0x04)
      break;
  }
  
#endif


  /* Setting TX BD number */
  REG32(ETH_REG_BASE + ETH_TX_BD_NUM) = ETH_TXBD_NUM;

  /* Set min/max packet length */
  REG32(ETH_REG_BASE + ETH_PACKETLEN) = 0x00400600;

  /* Set IPGT register to recomended value */
  REG32(ETH_REG_BASE + ETH_IPGT) =  0x00000012;

  /* Set IPGR1 register to recomended value */
  REG32(ETH_REG_BASE + ETH_IPGR1) =  0x0000000c;

  /* Set IPGR2 register to recomended value */
  REG32(ETH_REG_BASE + ETH_IPGR2) =  0x00000012;

  /* Set COLLCONF register to recomended value */
  REG32(ETH_REG_BASE + ETH_COLLCONF) =  0x000f003f;

#if 0
  REG32(ETH_REG_BASE + ETH_CTRLMODER) = ETH_CTRLMODER_TXFLOW | ETH_CTRLMODER_RXFLOW;
#else
  REG32(ETH_REG_BASE + ETH_CTRLMODER) = 0;
#endif

  /* Initialize RX and TX buffer descriptors */
  init_rx_bd_pool();
  init_tx_bd_pool();

  /* Initialize tx pointers */
  tx_next = 0;
  tx_last = 0;
  tx_full = 0;

  /* Initialize rx pointers */
  rx_next = 0;

  /* Assign receive function pointer to what we were passed */
  receive = rec;

  /* Set local MAC address */
  REG32(ETH_REG_BASE + ETH_MAC_ADDR1) = ETH_MACADDR0 << 8 |
            ETH_MACADDR1;
  REG32(ETH_REG_BASE + ETH_MAC_ADDR0) = ETH_MACADDR2 << 24 |
            ETH_MACADDR3 << 16 |
            ETH_MACADDR4 << 8 |
            ETH_MACADDR5;

  /* Clear all pending interrupts */
  REG32(ETH_REG_BASE + ETH_INT) = 0xffffffff;

  /* Register interrupt handler */
  int_add (ETH_IRQ, eth_int);

  /* Promisc, IFG, CRCEn */
  REG32(ETH_REG_BASE + ETH_MODER) |= ETH_MODER_PAD | ETH_MODER_IFG | ETH_MODER_CRCEN;

  /* Enable interrupt sources */
#if 0
  REG32(ETH_REG_BASE + ETH_INT_MASK)= ETH_INT_MASK_TXB        |
                   ETH_INT_MASK_TXE        |
                   ETH_INT_MASK_RXF        |
                   ETH_INT_MASK_RXE        |
                   ETH_INT_MASK_BUSY       |
                   ETH_INT_MASK_TXC        |
                   ETH_INT_MASK_RXC;
  eth_monitor_enabled = 1;
#else
  REG32(ETH_REG_BASE + ETH_INT_MASK) = 0x00000000;
  printf("eth_init: monitor disabled at startup\n");
  eth_monitor_enabled = 0;
#endif

  /* Enable receiver and transmiter */
  REG32(ETH_REG_BASE + ETH_MODER) |= ETH_MODER_RXEN | ETH_MODER_TXEN;

}

/* Returns pointer to next free buffer; NULL if none available */
void *eth_get_tx_buf ()
{
  eth_bd  *bd;
  unsigned long add;

  if(tx_full)
    return (void *)0;

  bd = (eth_bd *)ETH_BD_BASE;
  
  if(bd[tx_next].len_status & ETH_TX_BD_READY)
    return (void *)0;

  add = bd[tx_next].addr;

  tx_next++;

  if (tx_next == ETH_TXBD_NUM)
    tx_next = 0;

  // Disabled for now - highly unlikely we'll ever run out of TX buffer spots
  //if(tx_next == tx_last)
  //tx_full = 1;

  return (void *)add;
}

/* Send a packet at address */
void eth_send (void *buf, unsigned long len)
{
    eth_bd  *bd;
    int i;

#ifdef PRINT_PACKETS  
    printf("transmitted packet txbd %d:\t", tx_last);
  print_packet(buf, len);
#endif

  bd = (eth_bd *)ETH_BD_BASE;

  bd[tx_last].addr = (unsigned long)buf;

 retry_eth_send:
  bd[tx_last].len_status &= 0x0000ffff & ~ETH_TX_BD_STATS;
  
  if (eth_monitor_enabled) // enable IRQ when sending
    bd[tx_last].len_status |= ETH_TX_BD_IRQ;

  bd[tx_last].len_status |= len << 16 | ETH_TX_BD_READY;

  if (!eth_monitor_enabled) // If we're not running the network sniffer...
    {
      while (bd[tx_last].len_status & ETH_TX_BD_READY);

      //printf("packet tx\n");
      // Check it was sent OK 
      /*
	BUG with current implementation - constantly getting carrier sense
	lost, and deferred indication, so ignore it for now.
      if ( bd[tx_last].len_status & 0xf)
	{
	  printf("eth_send: carrier sense lost (txbd: 0x%x), deferred, retrying\n",
		 bd[tx_last].len_status);
	  sleep(1);
	  goto retry_eth_send;
	}
      */
    }

  tx_last++;

  if (tx_last == ETH_TXBD_NUM)
    tx_last = 0;

  tx_full = 0;

}

/* Waits for packet and pass it to the upper layers */
unsigned long 
eth_rx (void)
{
  eth_bd  *bd;
  unsigned long len = 0;
  int i;

  bd = (eth_bd *)ETH_BD_BASE + ETH_TXBD_NUM;
  
  while(1) {

    int bad = 0;

    if(bd[rx_next].len_status & ETH_RX_BD_EMPTY)
      return len;

    if(bd[rx_next].len_status & ETH_RX_BD_OVERRUN) {
      printf("eth rx: ETH_RX_BD_OVERRUN\n");
      bad = 1;
    }
    if(bd[rx_next].len_status & ETH_RX_BD_INVSIMB) {
      printf("eth rx: ETH_RX_BD_INVSIMB\n");
      bad = 1;
    }
    if(bd[rx_next].len_status & ETH_RX_BD_DRIBBLE) {
      printf("eth rx: ETH_RX_BD_DRIBBLE\n");
      bad = 1;
    }
    if(bd[rx_next].len_status & ETH_RX_BD_TOOLONG) {
      printf("eth rx: ETH_RX_BD_TOOLONG\n");
      bad = 1;
    }
    if(bd[rx_next].len_status & ETH_RX_BD_SHORT) {
      printf("eth rx: ETH_RX_BD_SHORT\n");
      bad = 1;
    }
    if(bd[rx_next].len_status & ETH_RX_BD_CRCERR) {
      printf("eth rx: ETH_RX_BD_CRCERR\n");
      bad = 1;
    }
    if(bd[rx_next].len_status & ETH_RX_BD_LATECOL) {
      printf("eth rx: ETH_RX_BD_LATECOL\n");
      bad = 1;
    }

    if(!bad) {
#ifdef PRINT_PACKETS
      printf("received packet: rxbd %d\t",rx_next);
      print_packet(bd[rx_next].addr, bd[rx_next].len_status >> 16);
#endif

      receive((void *)bd[rx_next].addr, bd[rx_next].len_status >> 16); 
      len += bd[rx_next].len_status >> 16;
    }

    bd[rx_next].len_status &= ~ETH_RX_BD_STATS;
    bd[rx_next].len_status |= ETH_RX_BD_EMPTY;

    rx_next++;

    if (rx_next == ETH_RXBD_NUM)
      rx_next = 0;
  }
}

void eth_int_enable(void)
{
  REG32(ETH_REG_BASE + ETH_INT_MASK) =  ETH_INT_MASK_TXB        |
                                        ETH_INT_MASK_TXE        |
                                        ETH_INT_MASK_RXF        |
                                        ETH_INT_MASK_RXE        |
                                        ETH_INT_MASK_BUSY       |
                                        ETH_INT_MASK_TXC        |
                                        ETH_INT_MASK_RXC;
}

void eth_int_disable(void)
{
  REG32(ETH_REG_BASE + ETH_INT_MASK) = 0;
}

void eth_halt(void)
{
  /* Disable receiver and transmiter */
  REG32(ETH_REG_BASE + ETH_MODER) &= ~(ETH_MODER_RXEN | ETH_MODER_TXEN);
}


void eth_toggle_traffic_mon(void)
{
  if (!eth_monitor_enabled)
    {
      printf("eth_toggle_traffic_mon: traffic monitor ENABLED\n");
      printf("eth_toggle_traffic_mon: Warning - do not enable during tftp boot!\n");
      eth_monitor_enabled = 1;
      eth_int_enable();
    }
  else
    {
      eth_int_disable();
      eth_monitor_enabled = 0;
      printf("eth_toggle_traffic_mon: traffic monitor DISABLED\n");


    }

}
#endif
