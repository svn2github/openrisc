#include "common.h"
#include "uart.h"
#include "eth.h"
#include "support.h"
#include "spr-defs.h"
#include "net.h"

#if 1
extern int tx_pointer_index;
unsigned long dest_mac_addr[6];

void show_tx_bd(int start, int max)
{
	int i;

	for (i = start; i <= max; i++) {
		/* Read Tx BD */
		printf("LEN:%04lx", REG32(ETH_BD_BASE + (i << 3)) >> 16);
		printf(" RD:%04lx",
		       (REG32(ETH_BD_BASE + (i << 3)) >> 15) & 0x1);
		printf(" IRQ:%04lx",
		       (REG32(ETH_BD_BASE + (i << 3)) >> 14) & 0x1);
		printf(" WR:%04lx",
		       (REG32(ETH_BD_BASE + (i << 3)) >> 13) & 0x1);
		printf(" PAD:%04lx",
		       (REG32(ETH_BD_BASE + (i << 3)) >> 12) & 0x1);
		printf(" CRC:%04lx",
		       (REG32(ETH_BD_BASE + (i << 3)) >> 11) & 0x1);
		printf(" UR:%04lx", (REG32(ETH_BD_BASE + (i << 3)) >> 8) & 0x1);
		printf(" RTRY:%04lx",
		       (REG32(ETH_BD_BASE + (i << 3)) >> 4) & 0xf);
		printf(" RL:%04lx", (REG32(ETH_BD_BASE + (i << 3)) >> 3) & 0x1);
		printf(" LC:%04lx", (REG32(ETH_BD_BASE + (i << 3)) >> 2) & 0x1);
		printf(" DF:%04lx", (REG32(ETH_BD_BASE + (i << 3)) >> 1) & 0x1);
		printf(" CS:%04lx", (REG32(ETH_BD_BASE + (i << 3)) >> 0) & 0x1);
		printf("\nTx Buffer Pointer: %08lx\n",
		       REG32(ETH_BD_BASE + (i << 3) + 4));
	}
}

void show_rx_bd(int start, int max)
{
	int i;
	unsigned long rx_bd_base, rx_bd_num;

	rx_bd_num = REG32(ETH_REG_BASE + ETH_TX_BD_NUM);
	rx_bd_base = ETH_BD_BASE + (rx_bd_num << 3);

	for (i = start; i <= max; i++) {
		/* Read Rx BD */
		printf("LEN:%04lx", REG32(rx_bd_base + (i << 3)) >> 16);
		printf(" E:%04lx", (REG32(rx_bd_base + (i << 3)) >> 15) & 0x1);
		printf(" IRQ:%04lx",
		       (REG32(rx_bd_base + (i << 3)) >> 14) & 0x1);
		printf(" WR:%04lx", (REG32(rx_bd_base + (i << 3)) >> 13) & 0x1);
		printf(" M:%04lx", (REG32(rx_bd_base + (i << 3)) >> 7) & 0x1);
		printf(" OR:%04lx", (REG32(rx_bd_base + (i << 3)) >> 6) & 0x1);
		printf(" IS:%04lx", (REG32(rx_bd_base + (i << 3)) >> 5) & 0x1);
		printf(" DN:%04lx", (REG32(rx_bd_base + (i << 3)) >> 4) & 0x1);
		printf(" TL:%04lx", (REG32(rx_bd_base + (i << 3)) >> 3) & 0x1);
		printf(" SF:%04lx", (REG32(rx_bd_base + (i << 3)) >> 2) & 0x1);
		printf(" CRC:%04lx", (REG32(rx_bd_base + (i << 3)) >> 1) & 0x1);
		printf(" LC:%04lx", (REG32(rx_bd_base + (i << 3)) >> 0) & 0x1);
		printf("\nRx Buffer Pointer: %08lx\n",
		       REG32(rx_bd_base + (i << 3) + 4));
	}
}

void show_buffer(unsigned long start_addr, unsigned long len)
{
	show_mem(start_addr, start_addr + len - 1);
}

void show_rx_buffs(int max, int show_all)
{

	int i;
	unsigned long rx_bd_base, rx_bd_num;

	rx_bd_num = REG32(ETH_REG_BASE + ETH_TX_BD_NUM);
	rx_bd_base = ETH_BD_BASE + (rx_bd_num << 3);

	for (i = 0; i <= max; i++) {
		if (!(REG32(rx_bd_base + (i << 3)) & ETH_RX_BD_EMPTY)
		    || show_all) {
			printf("Rx BD No. %04x located at %08lx\n", i,
			       rx_bd_base + (i << 3));
			show_rx_bd(i, i);
			show_buffer(REG32(rx_bd_base + (i << 3) + 4),
				    REG32(rx_bd_base + (i << 3)) >> 16);
			printf("\n");
		}
		if (REG32(rx_bd_base + (i << 3)) & ETH_RX_BD_WRAP)
			return;
	}
}

void show_tx_buffs(int max)
{
	int i;

	for (i = 0; i <= max; i++) {
		if (1) {
			printf("Tx BD No. %04x located at %08x\n", i,
			       ETH_BD_BASE + (i << 3));
			show_tx_bd(i, i);
			show_buffer(REG32(ETH_BD_BASE + (i << 3) + 4),
				    REG32(ETH_BD_BASE + (i << 3)) >> 16);
			printf("\n");
		}
		if (REG32(ETH_BD_BASE + (i << 3)) & ETH_TX_BD_WRAP)
			return;
	}
}

void show_phy_reg(unsigned long start_addr, unsigned long stop_addr)
{

	unsigned long addr;

	if (start_addr == stop_addr) {
		printf("\nSet MII RGAD ADDRESS to %08lx", start_addr);
		printf("\nMII Command = Read Status\n");
	}

	for (addr = start_addr; addr <= stop_addr; addr++) {
		REG32(ETH_REG_BASE + ETH_MIIADDRESS) = addr << 8;
		REG32(ETH_REG_BASE + ETH_MIICOMMAND) = ETH_MIICOMMAND_RSTAT;

		printf("PHY %04lx",
		       REG32(ETH_REG_BASE + ETH_MIIADDRESS) & 0x1f);
		printf(", addr %04lx",
		       REG32(ETH_REG_BASE + ETH_MIIADDRESS) >> 8);
		printf(": %08lx\n", REG32(ETH_REG_BASE + ETH_MIIRX_DATA));
	}
}

void set_phy_reg(unsigned long addr, unsigned long val)
{
	printf("\nSet MII RGAD ADDRESS to %08lx", addr);

	REG32(ETH_REG_BASE + ETH_MIIADDRESS) = addr << 8;

	printf("\nMII Command = Write Control Data\n");
	REG32(ETH_REG_BASE + ETH_MIICOMMAND) = ETH_MIICOMMAND_WCTRLDATA;

	REG32(ETH_REG_BASE + ETH_MIITX_DATA) = val;

	show_phy_reg(addr, addr);
}

void send_packet(unsigned long len, unsigned long start_data,
		 int num_of_packets)
{
	unsigned long i, TxBD;

	while (num_of_packets--) {
		unsigned long *data = (unsigned long *)eth_get_tx_buf();

		/* Set dest & src address */
		*data++ = dest_mac_addr[0] << 24 |
		    dest_mac_addr[1] << 16 |
		    dest_mac_addr[2] << 8 | dest_mac_addr[3] << 0;

		*data++ = dest_mac_addr[4] << 24 |
		    dest_mac_addr[5] << 16 |
		    ETH_MACADDR0 << 8 | ETH_MACADDR1 << 0;

		*data++ = ETH_MACADDR2 << 24 |
		    ETH_MACADDR3 << 16 | ETH_MACADDR4 << 8 | ETH_MACADDR5 << 0;

		/* Write data to buffer */
		for (i = 12; i < len; i += 4)
			*data++ =
			    (i + start_data - 12) << 24 | (i + start_data + 1 -
							   12) << 16 | (i +
									start_data
									+ 2 -
									12) << 8
			    | (i + start_data + 3 - 12);

		eth_send(data, len);
		printf(".");
	}
}

int eth_init_cmd(int argc, char *argv[])
{
	if (argc)
		return -1;
	eth_init(0);
	return 0;
}

int show_txbd_cmd(int argc, char *argv[])
{
	int i;
	int start, max;

	if (argc == 1)
		show_tx_bd(strtoul(argv[0], NULL, 0),
			   strtoul(argv[0], NULL, 0));
	else if (argc == 2)
		show_tx_bd(strtoul(argv[0], NULL, 0),
			   strtoul(argv[1], NULL, 0));
	else
		show_tx_bd(0, 63);
	return 0;
}

int show_rxbd_cmd(int argc, char *argv[])
{
	if (argc == 1)
		show_rx_bd(strtoul(argv[0], NULL, 0),
			   strtoul(argv[0], NULL, 0));
	else if (argc == 2)
		show_rx_bd(strtoul(argv[0], NULL, 0),
			   strtoul(argv[1], NULL, 0));
	else
		show_rx_bd(0, 63);
	return 0;
}

int send_packet_cmd(int argc, char *argv[])
{
	if (argc == 1)
		send_packet(strtoul(argv[0], NULL, 0), 31, 1);
	else if (argc == 2)
		send_packet(strtoul(argv[0], NULL, 0),
			    strtoul(argv[1], NULL, 0), 1);
	else if (argc == 3)
		send_packet(strtoul(argv[0], NULL, 0),
			    strtoul(argv[1], NULL, 0), strtoul(argv[2], NULL,
							       0));
	else
		return -1;
	return 0;
}

int set_dest_addr_cmd(int argc, char *argv[])
{
	if (argc == 3) {
		dest_mac_addr[0] = (strtoul(argv[0], NULL, 0) >> 8) & 0xff;
		dest_mac_addr[1] = (strtoul(argv[0], NULL, 0) >> 0) & 0xff;
		dest_mac_addr[2] = (strtoul(argv[1], NULL, 0) >> 8) & 0xff;
		dest_mac_addr[3] = (strtoul(argv[1], NULL, 0) >> 0) & 0xff;
		dest_mac_addr[4] = (strtoul(argv[2], NULL, 0) >> 8) & 0xff;
		dest_mac_addr[5] = (strtoul(argv[2], NULL, 0) >> 0) & 0xff;
	} else
		return -1;
	return 0;
}

int init_txbd_pool_cmd(int argc, char *argv[])
{
#if 0
	if (argc == 1)
		init_tx_bd_pool(strtoul(argv[0], NULL, 0));
	else
		return -1;
#endif
	return 0;
}

int init_rxbd_pool_cmd(int argc, char *argv[])
{
	//if (argc == 1) init_rx_bd_pool(strtoul (argv[0], NULL, 0));
	//else return -1;
	init_rx_bd_pool();

	return 0;
}

int show_phy_reg_cmd(int argc, char *argv[])
{
	if (argc == 1)
		show_phy_reg(strtoul(argv[0], NULL, 0),
			     strtoul(argv[0], NULL, 0));
	else if (argc == 2)
		show_phy_reg(strtoul(argv[0], NULL, 0),
			     strtoul(argv[1], NULL, 0));
	else
		show_phy_reg(0, 30);
	return 0;
}

int set_phy_reg_cmd(int argc, char *argv[])
{
	if (argc == 2)
		set_phy_reg(strtoul(argv[0], NULL, 0),
			    strtoul(argv[1], NULL, 0));
	else
		return -1;
	return 0;
}

int show_mac_regs_cmd(int argc, char *argv[])
{
	if (argc)
		return -1;
	printf("\n %08x", ETH_REG_BASE + ETH_MODER);
	printf(" MODER: %08lx", REG32(ETH_REG_BASE + ETH_MODER));

	printf("\n %08x", ETH_REG_BASE + ETH_INT);
	printf(" INT: %08lx", REG32(ETH_REG_BASE + ETH_INT));

	printf("\n %08x", ETH_REG_BASE + ETH_INT_MASK);
	printf(" INT_MASK: %08lx", REG32(ETH_REG_BASE + ETH_INT_MASK));

	printf("\n %08x", ETH_REG_BASE + ETH_IPGT);
	printf(" IPGT: %08lx", REG32(ETH_REG_BASE + ETH_IPGT));

	printf("\n %08x", ETH_REG_BASE + ETH_IPGR1);
	printf(" IPGR1: %08lx", REG32(ETH_REG_BASE + ETH_IPGR1));

	printf("\n %08x", ETH_REG_BASE + ETH_IPGR2);
	printf(" IPGR2: %08lx", REG32(ETH_REG_BASE + ETH_IPGR2));

	printf("\n %08x", ETH_REG_BASE + ETH_PACKETLEN);
	printf(" PACKETLEN: %08lx", REG32(ETH_REG_BASE + ETH_PACKETLEN));

	printf("\n %08x", ETH_REG_BASE + ETH_COLLCONF);
	printf(" COLLCONF: %08lx", REG32(ETH_REG_BASE + ETH_COLLCONF));

	printf("\n %08x", ETH_REG_BASE + ETH_TX_BD_NUM);
	printf(" TX_BD_NUM: %08lx", REG32(ETH_REG_BASE + ETH_TX_BD_NUM));

	printf("\n %08x", ETH_REG_BASE + ETH_CTRLMODER);
	printf(" CTRLMODER: %08lx", REG32(ETH_REG_BASE + ETH_CTRLMODER));

	printf("\n %08x", ETH_REG_BASE + ETH_MIIMODER);
	printf(" MIIMODER: %08lx", REG32(ETH_REG_BASE + ETH_MIIMODER));

	printf("\n %08x", ETH_REG_BASE + ETH_MIICOMMAND);
	printf(" MIICOMMAND: %08lx", REG32(ETH_REG_BASE + ETH_MIICOMMAND));

	printf("\n %08x", ETH_REG_BASE + ETH_MIIADDRESS);
	printf(" MIIADDRESS: %08lx", REG32(ETH_REG_BASE + ETH_MIIADDRESS));

	printf("\n %08x", ETH_REG_BASE + ETH_MIITX_DATA);
	printf(" MIITX_DATA: %08lx", REG32(ETH_REG_BASE + ETH_MIITX_DATA));

	printf("\n %08x", ETH_REG_BASE + ETH_MIIRX_DATA);
	printf(" MIIRX_DATA: %08lx", REG32(ETH_REG_BASE + ETH_MIIRX_DATA));

	printf("\n %08x", ETH_REG_BASE + ETH_MIISTATUS);
	printf(" MIISTATUS: %08lx", REG32(ETH_REG_BASE + ETH_MIISTATUS));

	printf("\n %08x", ETH_REG_BASE + ETH_MAC_ADDR0);
	printf(" MAC_ADDR0: %08lx", REG32(ETH_REG_BASE + ETH_MAC_ADDR0));

	printf("\n %08x", ETH_REG_BASE + ETH_MAC_ADDR1);
	printf(" MAC_ADDR1: %08lx", REG32(ETH_REG_BASE + ETH_MAC_ADDR1));

	printf("\n %08x", ETH_REG_BASE + ETH_HASH_ADDR0);
	printf(" ETH_HASH_ADDR0: %08lx", REG32(ETH_REG_BASE + ETH_HASH_ADDR0));

	printf("\n %08x", ETH_REG_BASE + ETH_HASH_ADDR1);
	printf(" ETH_HASH_ADDR1: %08lx", REG32(ETH_REG_BASE + ETH_HASH_ADDR1));

	printf("\n");
	return 0;
}

int eth_int_enable_cmd(int argc, char *argv[])
{
	if (argc)
		return -1;
	eth_int_enable();
	return 0;
}

int show_rx_buffs_cmd(int argc, char *argv[])
{
	if (argc == 0)
		show_rx_buffs(63, 0);
	else if (argc == 1)
		show_rx_buffs(63, 1);
	else
		return -1;
	return 0;
}

int show_tx_buffs_cmd(int argc, char *argv[])
{
	if (argc == 0)
		show_tx_buffs(63);
	else
		return -1;
	return 0;
}
#endif

int eth_conf_cmd(int argc, char *argv[])
{
	switch (argc) {
	case 0:
		printf("IP: %s", inet_ntoa(global.ip));
		printf("\nmask: %s", inet_ntoa(global.mask));
		printf("\nGW: %s", inet_ntoa(global.gw_ip));
		return 0;
	case 3:
		global.gw_ip = inet_aton(argv[2]);
	case 2:
		global.mask = inet_aton(argv[1]);
	case 1:
		global.ip = inet_aton(argv[0]);
		break;
	}
	printf("Restarting network with new parameters...");
	NetStartAgain();

	return 0;
}

void toggle_eth_traffic_monitor_cmd(void)
{
	eth_toggle_traffic_mon();
}

void module_eth_init(void)
{
#if 1
	register_command("eth_init", "", "init ethernet", eth_init_cmd);
	register_command("show_txbd", "[<start BD>] [<max>]",
			 "show Tx buffer desc", show_txbd_cmd);
	register_command("show_rxbd", "[<start BD>] [<max>]",
			 "show Rx buffer desc", show_rxbd_cmd);
	register_command("send_packet",
			 "<length> [<start data>] [<num_of_packets>]",
			 "create & send packet(s)", send_packet_cmd);
	register_command("set_dest_addr", "<addrhi> <addrmid> <addrlo>",
			 "set destination address (for send_packet)",
			 set_dest_addr_cmd);
	register_command("init_txbd_pool", "<max>",
			 "initialize Tx buffer descriptors",
			 init_txbd_pool_cmd);
	register_command("init_rxbd_pool", "<max>",
			 "initialize Rx buffer descriptors",
			 init_rxbd_pool_cmd);
	register_command("show_phy_reg", "[<start_addr>] [<end addr>]",
			 "show PHY registers", show_phy_reg_cmd);
	register_command("set_phy_reg", "<addr> <value>", "set PHY register",
			 set_phy_reg_cmd);
	register_command("show_mac_regs", "", "show all MAC registers",
			 show_mac_regs_cmd);
	register_command("eth_int_enable", "", "enable ethernet interrupt",
			 eth_int_enable_cmd);
	register_command("show_rx_buffs", "[<show_all>]",
			 "show receive buffers (optional arg will also show empty buffers)",
			 show_rx_buffs_cmd);
	register_command("show_tx_buffs", "", "show transmit buffers",
			 show_rx_buffs_cmd);
	register_command("eth_toggle_mon", "", "Toggle traffic monitor",
			 toggle_eth_traffic_monitor_cmd);
#endif
	/* Initialize controller */
	register_command("eth_conf", "[<ip> [<mask> [<gw>]]]",
			 "Get/set ethernet configuration", eth_conf_cmd);
#if 0
	eth_init();
	printf("Ethernet not initialized (run eth_init command)\n");
	init_rx_bd_pool(0);
	init_tx_bd_pool(3);
#endif
}
