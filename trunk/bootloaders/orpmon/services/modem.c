#include "common.h"
#include "support.h"
#include "net.h"
#include "uart.h"
#include "spr-defs.h"
#include "flash.h"

#define SOH    0x01
#define STX    0x02
#define EOT    0x04
#define ACK    0x06
#define NAK    0x15
#define CAN    0x18
#define C      0x43

#define CPMEOF 0x1A

#define RETRY   10
#define TIMEOUT 3

/* update CRC */
unsigned short updcrc(register int c, register unsigned int crc)
{
	register int count;

	for (count = 8; --count >= 0;) {
		if (crc & 0x8000) {
			crc <<= 1;
			crc += (((c <<= 1) & 0400) != 0);
			crc ^= 0x1021;
		} else {
			crc <<= 1;
			crc += (((c <<= 1) & 0400) != 0);
		}
	}
	return crc;
}

static thand_f *mTimeHandler;
static unsigned long mTimeValue;
static int mTimeoutCount;

static unsigned long src_addr;

unsigned int length;
unsigned int modemMode = 0, bLen = 128;
unsigned short mycrc;
unsigned long bno = 0;

static void mStartTimeout(void);

void mSetTimeout(int iv, thand_f * f)
{
	if (iv == 0)
		mTimeHandler = (thand_f *) 0;
	else {
		mTimeHandler = f;
		mTimeValue = get_timer(0) + iv;
	}
}

static void mStartTimeout(void)
{
	if (++mTimeoutCount >= RETRY) {
		printf("...retry counter exceeded, quitting...\n");
		return;
	} else {
		printf(".");
		mSetTimeout(TIMEOUT * TICKS_PER_SEC, mStartTimeout);
		uart_putc(C);

	}
}

static void mReceiveTimeout(void)
{
	if (++mTimeoutCount >= RETRY) {
		uart_putc(NAK);
		printf("...");
		return;
	} else {
		mSetTimeout(TIMEOUT * TICKS_PER_SEC, mReceiveTimeout);
		uart_putc(NAK);
	}
}

int getBlock(unsigned char t)
{
	unsigned int i = 0, j = 0;
	unsigned char mybuf[133];
	unsigned char bNo, nBno;
	unsigned long dst_addr;

	unsigned char flags;
	flags = UART_LSR_FE | UART_LSR_PE | UART_LSR_OE | UART_LSR_BI;	/*frame,parity,overrun errors */

	mycrc = 0;

	switch (t) {
	case SOH:
		for (i = 0; i < 132; i++) {
			if ((REG8(UART_BASE + UART_LSR) & flags) == flags) {
				uart_putc(CAN);
			}
			mybuf[i] = uart_getc();
		}

		bNo = mybuf[0];	/* packet id */
		nBno = mybuf[1];	/* neg. packet id */

		if ((bNo == 0x00) && (nBno == 0xff) && (bno == 0)) {	/* start block */
			modemMode = 2;	/* ymodem */
			uart_putc(ACK);
			uart_putc(C);
			return 1;
		} else if ((0xff - bNo) == nBno) {	/* data block */
			for (i = 2, j = 0; i < 130; i++, j++) {
				length++;
				mycrc = updcrc(mybuf[i], mycrc);
				dst_addr =
				    src_addr + (bno * 0x8000) +
				    ((bNo - 1) * 0x80) + j;
				REG8(dst_addr) = mybuf[i];
			}

			mycrc = updcrc(mybuf[130], mycrc);
			mycrc = updcrc(mybuf[131], mycrc);

			if (mycrc == 0) {	/* CRC match! */
				uart_putc(ACK);
				for (i = 0; i < 128; i += 4) {
					/*      for(j=0; j<4; j++) {
					   tmp = tmp << 8;
					   tmp |= mybuf[i+j+2];
					   }
					   dst_addr = src_addr+(bno*0x8000)+((bNo-1)*128)+i;
					   fl_word_program(dst_addr, tmp); */
				}
				if (bNo == 0xff)
					bno++;
				return 1;
			} else {
				uart_putc(NAK);
				return -1;
			}
		} else {	/* packet id didn't match neg packet id! */
			uart_putc(NAK);
			return -1;
		}
		return 1;
		break;
	case EOT:
		if (modemMode == 2) {	/* ymodem */
			uart_putc(NAK);
			if (uart_getc() == EOT) {
				uart_putc(ACK);
				uart_putc(C);
			} else
				uart_putc(ACK);
		} else		/* zmodem */
			uart_putc(ACK);

		return 0;
		break;
	default:
		/* Unknown header */
		uart_putc(NAK);
		return -1;
	}
}

int mGetData(unsigned long saddr)
{
	int retval = 1;
	unsigned char c;

	length = 0;
	src_addr = saddr;
	modemMode = 1;
	bno = 0;

	printf("src_addr: 0x%lx\n", src_addr);
	if (fl_init() != 0) {
		printf("Flash init failed!\n");
		return (-1);
	}
#if 0
	printf("Unlocking flash...");
	for (i = 0, c = FLASH_BASE_ADDR; i < (FLASH_SIZE / FLASH_BLOCK_SIZE);
	     i++, c += FLASH_BLOCK_SIZE)
		if (fl_unlock_one_block(c))
			return 1;
	printf("done\n");

	printf("Erasing flash...");
	for (i = 0, c = FLASH_BASE_ADDR; i < (FLASH_SIZE / FLASH_BLOCK_SIZE);
	     i++, c += FLASH_BLOCK_SIZE)
		if (fl_block_erase(c))
			return 1;
	printf("done\n");
#endif
	printf("Waiting...");

	mTimeoutCount = 0;
	mSetTimeout(TIMEOUT * TICKS_PER_SEC, mStartTimeout);

	while (1) {
		if (mTimeHandler && (get_timer(0) > mTimeValue)) {
			thand_f *x;
			x = mTimeHandler;
			mTimeHandler = (thand_f *) 0;
			(*x) ();
		}
		c = uart_testc();
		if (c != 0)
			break;
	}

	while (retval != 0) {
		retval = getBlock(c);
		if (retval != 0)
			c = uart_getc();
	}

	if (modemMode == 2) {
		c = uart_getc();
		retval = getBlock(c);	/* last 'dummy' block for YModem */
		printf("... protocol: YModem, ");
	} else
		printf("... protocol: ZModem, ");
	return length;
}
