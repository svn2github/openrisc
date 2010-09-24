#ifndef _BOARD_H_
#define _BOARD_H_

#define CFG_IN_FLASH    	0
//#define MC_ENABLED      	1

//LAN controller 
//#define SMC91111_LAN		1
#define OC_LAN			1

/* BOARD
 * 0 - bender
 * 1 - marvin
 * 2 - ORSoC A3PE1500 board
 * 3 - ORSoC A3P1000 board
 */
#define BOARD                   2

/* Ethernet IP and TFTP config
 * 0 - JB ORSoC board
 * 1 - AE ORSoC board
 * 2 - JB Southpole board
 * 3 - JB ORSoC board 2
 * 4 - Unassigned
 */
#define IPCONFIG                 3

#if BOARD==0
// Nibbler on bender1

#  define FLASH_BASE_ADDR         0xf0000000
#  define FLASH_SIZE              0x02000000
#  define FLASH_BLOCK_SIZE        0x00020000
#  define START_ADD               0x0
#  define CONFIG_OR32_MC_VERSION  2
#  define IN_CLK             	  25000000
#  define BOARD_DEF_NAME       	  "bender"
// Flash Organization on board
// FLASH_ORG_XX_Y
// where XX - flash bit size
//       Y  - number of parallel devices connected
#  define FLASH_ORG_16_1          1
#elif BOARD==1
//Marvin
#  define FLASH_BASE_ADDR         0xf0000000
#  define FLASH_SIZE              0x04000000
#  define FLASH_BLOCK_SIZE        0x00040000
#  define START_ADD               0x0
#  define CONFIG_OR32_MC_VERSION  1
#  define IN_CLK		  50000000
#  define FLASH_ORG_16_2          1
#  define BOARD_DEF_NAME       	  "marvin"
#elif BOARD==2
//ORSoC usbethdev board

#  define FLASH_BASE_ADDR         0xf0000000
#  define FLASH_SIZE              0x04000000
#  define FLASH_BLOCK_SIZE        0x00040000
#  define START_ADD               0x0
#  define SDRAM_SIZE              0x02000000
#  define SDRAM_ROW_SIZE          0x00000400
#  define SDRAM_BANK_SIZE         0x00800000
#  define IN_CLK		  20000000

#  define FLASH_ORG_16_2          1
#  define BOARD_DEF_NAME       	  "ORSoC devboard"
#elif BOARD==3
//ORSoC A3P1000 usbethdev board

#  define FLASH_BASE_ADDR         0xf0000000
#  define FLASH_SIZE              0x04000000
#  define FLASH_BLOCK_SIZE        0x00040000
#  define START_ADD               0x0
#  define SDRAM_SIZE              0x02000000
#  define SDRAM_ROW_SIZE          0x00000400
#  define SDRAM_BANK_SIZE         0x00800000
#  define IN_CLK		  25000000
#  define FLASH_ORG_16_2          1
#  define BOARD_DEF_NAME       	  "ORSoC A3P1000 devboard"

#else
//Custom Board

#  define FLASH_BASE_ADDR         0xf0000000
#  define FLASH_SIZE              0x04000000
#  define FLASH_BLOCK_SIZE        0x00040000
#  define START_ADD               0x0
#  define IN_CLK		  25000000
#  define FLASH_ORG_16_2          1
#  define BOARD_DEF_NAME       	  "custom"

#endif


// IP tboot configs
#if IPCONFIG==0

#define BOARD_DEF_IP    	0xc0a8649b // 192.168.100.155
#define BOARD_DEF_MASK  	0xffffff00 // 255.255.255.0
#define BOARD_DEF_GW    	0xc0a86401 // 192.168.100.1
#define BOARD_DEF_TBOOT_SRVR 	0xc0a86469 //"192.168.100.105"
#define BOARD_DEF_IMAGE_NAME    "boot.img"
#define BOARD_DEF_LOAD_SPACE    0xa00000
#define ETH_MDIOPHYADDR      	0x00
#define ETH_MACADDR0      	0x00
#define ETH_MACADDR1      	0x12
#define ETH_MACADDR2      	0x34
#define ETH_MACADDR3      	0x56
#define ETH_MACADDR4      	0x78
#define ETH_MACADDR5      	0x9a

#elif IPCONFIG==1

#define BOARD_DEF_IP    	0xc0a8649c // 192.168.100.156
#define BOARD_DEF_MASK  	0xffffff00 // 255.255.255.0
#define BOARD_DEF_GW    	0xc0a86401 // 192.168.100.1
#define BOARD_DEF_TBOOT_SRVR 	0xc0a864e3 //"192.168.100.227"
#define BOARD_DEF_IMAGE_NAME    "boot.img"
#define BOARD_DEF_LOAD_SPACE    0xa00000
#define ETH_MDIOPHYADDR      	0x00
#define ETH_MACADDR0      	0xad
#define ETH_MACADDR1      	0xda
#define ETH_MACADDR2      	0x34
#define ETH_MACADDR3      	0x56
#define ETH_MACADDR4      	0x78
#define ETH_MACADDR5      	0x9b

#elif IPCONFIG==2

#define BOARD_DEF_IP    	0xac1e0002 // 172.30.0.2
#define BOARD_DEF_MASK  	0xffffff00 // 255.255.255.0
#define BOARD_DEF_GW    	0xac1e0001 //"172.30.0.1"
#define BOARD_DEF_TBOOT_SRVR 	0xac1e0001 //"172.30.0.1"
#define BOARD_DEF_IMAGE_NAME    "boot.img"
#define BOARD_DEF_LOAD_SPACE    0xa00000
#define ETH_MDIOPHYADDR      	0x00
#define ETH_MACADDR0      	0x00
#define ETH_MACADDR1      	0x12
#define ETH_MACADDR2      	0x34
#define ETH_MACADDR3      	0x56
#define ETH_MACADDR4      	0x78
#define ETH_MACADDR5      	0x9c

#elif IPCONFIG==3 // ORSoC LAN

#define BOARD_DEF_IP    	0xc0a80103 // 192.168.1.3
#define BOARD_DEF_MASK  	0xffffff00 // 255.255.255.0
#define BOARD_DEF_GW    	0xc0a80101 // 192.168.1.1
#define BOARD_DEF_TBOOT_SRVR 	0xc0a80101 // 192.168.1.1
#define BOARD_DEF_IMAGE_NAME    "boot.img"
#define BOARD_DEF_LOAD_SPACE    0xa00000
#define ETH_MDIOPHYADDR      	0x00
#define ETH_MACADDR0      	0xad
#define ETH_MACADDR1      	0xaa
#define ETH_MACADDR2      	0x34
#define ETH_MACADDR3      	0x56
#define ETH_MACADDR4      	0x78
#define ETH_MACADDR5      	0x9d

#elif IPCONFIG==4 // Unassigned config...

#define BOARD_DEF_IP    	0x0a01010a // 10.1.1.10
#define BOARD_DEF_MASK  	0xffffff00 // 255.255.255.0
#define BOARD_DEF_GW    	0x0a010101 // 10.1.1.1
#define BOARD_DEF_TBOOT_SRVR 	0x0a010101 // 10.1.1.1
#define BOARD_DEF_IMAGE_NAME    "boot.img"
#define BOARD_DEF_LOAD_SPACE    0xa00000
#define ETH_MDIOPHYADDR      	0x00
#define ETH_MACADDR0      	0xad
#define ETH_MACADDR1      	0xaa
#define ETH_MACADDR2      	0x34
#define ETH_MACADDR3      	0x56
#define ETH_MACADDR4      	0x78
#define ETH_MACADDR5      	0x9d

#endif



#define UART_BAUD_RATE  	115200

#define TICKS_PER_SEC   	100


#define MS_PER_SEC 1000
#define US_PER_SEC 1000000
#define US_PER_TICK (US_PER_SEC/TICKS_PER_SEC)
#define TICKS_PER_US (TICKS_PER_SEC*1000000)

#define STACK_SIZE		0x10000

#if     CONFIG_OR32_MC_VERSION==1
// Marvin, Bender MC
#  include "mc-init-1.h"
#elif   CONFIG_OR32_MC_VERSION==2
// Highland MC
#  include "mc-init-2.h"
//#else
//#  error "no memory controler chosen"
#endif

#define UART_BASE       	0x90000000
#define UART_IRQ        	2
#define ETH_BASE        	0x92000000
#define ETH_IRQ         	4

#define SPI_BASE        	0xb0000000
#define CRT_BASE_ADDR   	0x97000000
#define ATA_BASE_ADDR   	0x9e000000
#define KBD_BASE_ADD    	0x94000000
#define KBD_IRQ         	5

#define SANCHO_BASE_ADD         0x98000000
/*  Address for ETH_DATA */
#define ETH_DATA_BASE  		(SDRAM_SIZE - (0x600 * 128)) 

#define CRT_ENABLED	    	0
#define FB_BASE_ADDR    	0xa8000000

/* Whether online help is available -- saves space */
#define HELP_ENABLED    	1

/* Whether self check is enabled */
#define SELF_CHECK     		0

/* Whether we have keyboard suppport */
#define KBD_ENABLED    		0

/* Keyboard buffer size */
#define KBDBUF_SIZE    		256

/* Which console is used (CT_NONE, CT_SIM, CT_UART, CT_CRT) */
#define CONSOLE_TYPE   		CT_UART

#endif
