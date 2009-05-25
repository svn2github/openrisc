/*
 *  $Id: serial.h,v 1.2 2001-09-27 11:59:58 chris Exp $
 */

int console_uartinit(unsigned int BAUDRate);
int console_sps_putc(int ch);
int console_sps_getc();
int console_sps_kbhit();
