#ifndef _BOARD_H_
#define _BOARD_H_

#define MC_ENABLED	    0

#define IC_ENABLE 	    0
#define IC_SIZE         8192
#define DC_ENABLE 	    0
#define DC_SIZE         8192

#define SYS_CLK			25000000
#define IN_CLK  	    25000000

//#define UART_NUM_CORES	2
#undef  UART_NUM_CORES

#define UART0_BAUD_RATE	115200
#define UART0_BASE		0x90000000
#define UART0_IRQ		2

//#define GPIO_NUM_CORES	2
#undef  GPIO_NUM_CORES

#define GPIO0_BASE		0x91000000
#define GPIO0_IRQ		3

#endif
