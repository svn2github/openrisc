#ifndef _BOARD_H_
#define _BOARD_H_

#define CFG_IN_FLASH    	1
#define MC_ENABLED      	1

//LAN controller 
//#define SMC91111_LAN		1
#define OC_LAN			1

/* BOARD
 * 0 - bender
 * 1 - marvin
 */
#define BOARD                   1

#if BOARD==0
// Nibbler on bender1

#  define IC_ENABLE       	1
#  define IC_SIZE         	4096
#  define DC_ENABLE       	1
#  define DC_SIZE         	2048
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

#  define IC_ENABLE       	0
#  define IC_SIZE         	8192
#  define DC_ENABLE       	0
#  define DC_SIZE         	8192
#  define FLASH_BASE_ADDR         0xf0000000
#  define FLASH_SIZE              0x04000000
#  define FLASH_BLOCK_SIZE        0x00040000
#  define START_ADD               0x0
#  define CONFIG_OR32_MC_VERSION  1
/*#  define IN_CLK		  100000000*/
#  define IN_CLK		  50000000
#  define FLASH_ORG_16_2          1
#  define BOARD_DEF_NAME       	  "marvin"
#else

//Custom Board
#  define IC_ENABLE       	0
#  define IC_SIZE         	8192
#  define DC_ENABLE       	0
#  define DC_SIZE         	8192
#  define FLASH_BASE_ADDR         0xf0000000
#  define FLASH_SIZE              0x04000000
#  define FLASH_BLOCK_SIZE        0x00040000
#  define START_ADD               0x0
#  define CONFIG_OR32_MC_VERSION  1
#  define IN_CLK		  25000000
#  define FLASH_ORG_16_2          1
#  define BOARD_DEF_NAME       	  "custom"

#endif

#define UART_BAUD_RATE  	115200

#define TICKS_PER_SEC   	100

#define STACK_SIZE		0x10000

#if     CONFIG_OR32_MC_VERSION==1
// Marvin, Bender MC
#  include "mc-init-1.h"
#elif   CONFIG_OR32_MC_VERSION==2
// Highland MC
#  include "mc-init-2.h"
#else
#  error "no memory controler chosen"
#endif

#define UART_BASE       	0x90000000
#define UART_IRQ        	2
#define ETH_BASE        	0x92000000
#define ETH_IRQ         	4
#define MC_BASE_ADDR    	0x93000000
#define SPI_BASE        	0xb9000000
#define CRT_BASE_ADDR   	0x97000000
#define ATA_BASE_ADDR   	0x9e000000
#define KBD_BASE_ADD    	0x94000000
#define KBD_IRQ         	5

#define SANCHO_BASE_ADD         0x98000000
#define ETH_DATA_BASE  		0xa8000000 /*  Address for ETH_DATA */

#if 1
#define BOARD_DEF_IP    	0x0100002a /* 1.0.0.42 */
#define BOARD_DEF_MASK  	0xffffff00 /* 255.255.255.0 */
#define BOARD_DEF_GW    	0x01000001 /* 1.0.0.1 */
#define BOARD_DEF_TBOOT_SRVR 	"1.0.0.66"
#else
#define BOARD_DEF_IP    	0x0aed012a /* 10.237.1.42 */
#define BOARD_DEF_MASK  	0xffffff00 /* 255.255.255.0 */
#define BOARD_DEF_GW    	0x0aed0101 /* 10.0.0.1 */
#define BOARD_DEF_TBOOT_SRVR 	"10.237.1.27"
#endif


#define ETH_MACADDR0      	0x00
#define ETH_MACADDR1      	0x12
#define ETH_MACADDR2      	0x34
#define ETH_MACADDR3      	0x56
#define ETH_MACADDR4      	0x78
#define ETH_MACADDR5      	0x9a

#define CRT_ENABLED	    	1
#define FB_BASE_ADDR    	0xa8000000

/* Whether online help is available -- saves space */
#define HELP_ENABLED    	1

/* Whether self check is enabled */
#define SELF_CHECK     		0

/* Whether we have keyboard suppport */
#define KBD_ENABLED    		1

/* Keyboard buffer size */
#define KBDBUF_SIZE    		256

/* Which console is used (CT_NONE, CT_SIM, CT_UART, CT_CRT) */
#define CONSOLE_TYPE   		CT_UART

#endif
