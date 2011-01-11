#ifndef _BOARD_H_
#define _BOARD_H_

#define SDRAM_BASE_ADDR 0x00000000

#ifndef IN_CLK
# define IN_CLK          20000000
#endif

#define UART_BAUD_RATE  115200

#define TICKS_PER_SEC   100

#define STACK_SIZE      0x10000

#define UART_BASE       0x90000000
#define UART_IRQ        2
#define ETH_BASE        0x92000000
#define ETH_IRQ         4

#define SPI_BASE        0xb9000000
#define CRT_BASE_ADDR   0x97000000
#define ATA_BASE_ADDR   0x9e000000
#define KBD_BASE_ADD    0x94000000
#define KBD_IRQ         5

#define ETH_DATA_BASE  0xa8000000 /*  Address for ETH_DATA */

#define BOARD_DEF_IP    0x0a010185
#define BOARD_DEF_MASK  0xff000000
#define BOARD_DEF_GW    0x0a010101

#define ETH_MACADDR0      0x00
#define ETH_MACADDR1      0x12
#define ETH_MACADDR2      0x34
#define ETH_MACADDR3      0x56
#define ETH_MACADDR4      0x78
#define ETH_MACADDR5      0x9a

#define CRT_ENABLED	    1
#define FB_BASE_ADDR    0xa8000000

/* Whether online help is available -- saves space */
#define HELP_ENABLED    1

/* Whether self check is enabled */
#define SELF_CHECK     0

/* Whether we have keyboard suppport */
#define KBD_ENABLED    0

/* Keyboard buffer size */
#define KBDBUF_SIZE    256

/* Which console is used (CT_NONE, CT_SIM, CT_UART, CT_CRT) */
#define CONSOLE_TYPE   CT_UART

#endif
