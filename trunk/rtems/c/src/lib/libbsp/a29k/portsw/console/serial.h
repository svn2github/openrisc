/*
 *  $Id: serial.h,v 1.2 2001-09-27 11:59:36 chris Exp $
 */

int console_duartinit(unsigned int uart_num, unsigned int BAUDRate);
int console_sps_putc(unsigned int uart_num, int ch);
int console_sps_getc(unsigned int uart_num);
int console_sps_kbhit(unsigned int uart_num);
