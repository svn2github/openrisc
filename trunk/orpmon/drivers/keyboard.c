#include "common.h"
#include "board.h"
#include "support.h"
#include "int.h"
#include "keyboard.h"

#if KBD_ENABLED

volatile int kbd_tail = 0;
volatile int kbd_head = 0;
volatile int kbd_buf[KBDBUF_SIZE];

static const unsigned char scan2ascii[2][0x40] = {
 {0x1f, 0x00, '1', '2',  '3', '4', '5', '6',   /* 0x00 */
  '7', '8', '9', '0',    '-', '=', '\b', '\t',
  'q', 'w', 'e', 'r',    't', 'y', 'u', 'i',   /* 0x10 */
  'o', 'p', '[', ']',    '\n', 0x00, 'a', 's',
  'd', 'f', 'g', 'h',    'j', 'k', 'l', ';',   /* 0x20 */
  '\'', '`', 0xff, '\\', 'z', 'x', 'c', 'v',
  'b', 'n', 'm', ',',    '.', '/', 0xff, 0x00, /* 0x30 */
  0x00, ' ', 0x00, 0x00, 0x00, 0x00,0x00,0x00},

 {0x00, 0x00, '!', '@',  '#', '$', '%', '^',  /* 0x00 */
  '&',  '*', '(', ')',   '_', '+', 0x00,0x00,
  'Q', 'W', 'E', 'R',    'T', 'Y', 'U', 'I',  /* 0X10 */
  'O', 'P', '{', '}',    '\n', 0x00, 'A', 'S',
  'D', 'F', 'G', 'H',    'J', 'K', 'L', ':',  /* 0X20 */
  '"', '~', 0xff, '|',   'Z', 'X', 'C', 'V',
  'B', 'N', 'M', '<',    '>', '?', 0xff, 0x00,/* 0x30 */
  0x00, ' ', 0x00, 0x00, 0x00, 0x00,0x00,0x00}};

static int shift_state = 0;

static void put_queue(int ch)
{
  //putc (ch);
  debug ("put_queue %c (%i,%i)\n", ch, kbd_head, kbd_tail);
	kbd_buf[kbd_head] = ch;
	kbd_head = (kbd_head + 1) % KBDBUF_SIZE;
}

static void handle_scancode (unsigned char scan)
{
  unsigned char c;
  if (scan >= 0x40) {
    scan &= 0x7f;
    if (scan >= 0x40) return;
    c = scan2ascii[shift_state][scan];
    if (c == 0xff) shift_state = 0;
    return;
  }
  c = scan2ascii[shift_state][scan];
  if (c == 0xff) shift_state = 1;
  else put_queue (c);
}

static void keyboard_interrupt(void)
{
	unsigned char status = REG8(KBD_BASE_ADD + 0x4);
	debug ("keyboard_interrupt\n");
	do {
		if (status & 0x01) handle_scancode(REG8(KBD_BASE_ADD + 0x0));
		status = REG8(KBD_BASE_ADD + 0x4);
	} while (status & 0x01);
}

static int kbd_wait_for_input(void)
{
  int     n;
  int     status, data;

  n = TIMEOUT_CONST;
  do {
    status = REG8(KBD_STATUS_REG);
     /*
      * Wait for input data to become available.  This bit will
      * then be cleared by the following read of the DATA
      * register.
      */

    if (!(status & KBD_OBF)) continue;

		data = REG8(KBD_DATA_REG);

    /*
     * Check to see if a timeout error has occurred.  This means
     * that transmission was started but did not complete in the
     * normal time cycle.  PERR is set when a parity error occurred
     * in the last transmission.
     */
    if (status & (KBD_GTO | KBD_PERR)) continue;

		return (data & 0xff);
  } while (--n);
  return (-1);	/* timed-out if fell through to here... */
}

static void kbd_write (int address, int data)
{
	int status;
	do {
		status = REG8(KBD_STATUS_REG);  /* spin until input buffer empty*/
	} while (status & KBD_IBF);
  REG8 (address) = data;               /* write out the data*/
}

int kbd_init(void)
{
	int_add (KBD_IRQ, keyboard_interrupt);

	/* Flush any pending input. */
	while (kbd_wait_for_input() != -1) continue;

	/*
	 * Test the keyboard interface.
	 * This seems to be the only way to get it going.
	 * If the test is successful a x55 is placed in the input buffer.
	 */
	kbd_write(KBD_CNTL_REG, KBD_SELF_TEST);
	if (kbd_wait_for_input() != 0x55) {
		printf ("initialize_kbd: keyboard failed self test.\n");
		return(-1);
	}

	/*
	 * Perform a keyboard interface test.  This causes the controller
	 * to test the keyboard clock and data lines.  The results of the
	 * test are placed in the input buffer.
	 */
	kbd_write(KBD_CNTL_REG, KBD_SELF_TEST2);
	if (kbd_wait_for_input() != 0x00) {
		printf ("initialize_kbd: keyboard failed self test 2.\n");
		return(-1);
	}
	
	/* Enable the keyboard by allowing the keyboard clock to run. */
	kbd_write(KBD_CNTL_REG, KBD_CNTL_ENABLE);

	/*
	 * Reset keyboard. If the read times out
	 * then the assumption is that no keyboard is
	 * plugged into the machine.
	 * This defaults the keyboard to scan-code set 2.
	 */
	kbd_write(KBD_DATA_REG, KBD_RESET);
	if (kbd_wait_for_input() != KBD_ACK) {
		printf ("initialize_kbd: reset kbd failed, no ACK.\n");
		return(-1);
	}

	if (kbd_wait_for_input() != KBD_POR) {
		printf ("initialize_kbd: reset kbd failed, not POR.\n");
		return(-1);
	}

	/*
	 * now do a DEFAULTS_DISABLE always
	 */
	kbd_write(KBD_DATA_REG, KBD_DISABLE);
	if (kbd_wait_for_input() != KBD_ACK) {
		printf ("initialize_kbd: disable kbd failed, no ACK.\n");
		return(-1);
	}

	/*
	 * Enable keyboard interrupt, operate in "sys" mode,
	 *  enable keyboard (by clearing the disable keyboard bit),
	 *  disable mouse, do conversion of keycodes.
	 */
	kbd_write(KBD_CNTL_REG, KBD_WRITE_MODE);
	kbd_write(KBD_DATA_REG, KBD_EKI|KBD_SYS|KBD_DMS|KBD_KCC);

	/*
	 * now ENABLE the keyboard to set it scanning...
	 */
	kbd_write(KBD_DATA_REG, KBD_ENABLE);
	if (kbd_wait_for_input() != KBD_ACK) {
		printf ("initialize_kbd: keyboard enable failed.\n");
		return(-1);
	}

  int_enable (KBD_IRQ);
	printf ("PS/2 keyboard initialized at 0x%x (irq = %d).\n", KBD_BASE_ADD, KBD_IRQ);
	return (1);
}

#endif /* KBD_ENABLED */
