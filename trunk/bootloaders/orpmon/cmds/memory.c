#include "common.h"
#include "support.h"
#include "spr-defs.h"
#include "spincursor.h"

void show_mem(int start, int stop)
{
	unsigned long i = start;
	if ((i & 0xf) != 0x0)
		printf("\n%08lx: ", i);
	for (; i <= stop; i += 4) {
		if ((i & 0xf) == 0x0)
			printf("\n%08lx: ", i);
		/* Read one word */
		printf("%08lx ", REG32(i));
	}
	printf("\n");
}

void testram(unsigned long start_addr, unsigned long stop_addr,
	     unsigned long testno)
{
	unsigned long addr;
	unsigned long err_addr = 0;
	unsigned long err_no = 0;

	unsigned long stack_top = (unsigned long) &_src_addr;

	if (start_addr < stack_top)
	{
		printf("\n");
		printf("Warning: RAM test will overwrite stack.\n");
		printf("         Moving start of test to 0x%08x to avoid stack\n",
		       stack_top);
		start_addr = stack_top;
	}

	if (start_addr >= stop_addr)
	{
		printf("ram_test: Start of test must be after end (0x%08x !< 0x%08x)\n",
		       start_addr, stop_addr);
		printf("ram_test: Aborting.\n");
		return;
	}

	/* Test 1: Write locations with their addresses */
	if ((testno == 1) || (testno == 0)) {
		printf("\n1. Writing locations with their addresses: ");
		for (addr = start_addr; addr <= stop_addr; addr += 4)
			REG32(addr) = addr;

		/* Verify */
		for (addr = start_addr; addr <= stop_addr; addr += 4)
			if (REG32(addr) != addr) {
				err_no++;
				err_addr = addr;
			}
		if (err_no)
			printf("%04lx times failed. Last at location %08lx",
			       err_no, err_addr);
		else
			printf("Passed");
		err_no = 0;
	}

	/* Test 2: Write locations with their inverse address */
	if ((testno == 2) || (testno == 0)) {
		printf("\n2. Writing locations with their inverse addresses: ");
		for (addr = start_addr; addr <= stop_addr; addr += 4)
			REG32(addr) = ~addr;

		/* Verify */
		for (addr = start_addr; addr <= stop_addr; addr += 4)
			if (REG32(addr) != ~addr) {
				err_no++;
				err_addr = addr;
			}
		if (err_no)
			printf("%04lx times failed. Last at location %08lx",
			       err_no, err_addr);
		else
			printf("Passed");
		err_no = 0;
	}

	/* Test 3: Write locations with walking ones */
	if ((testno == 3) || (testno == 0)) {
		printf("\n3. Writing locations with walking ones: ");
		for (addr = start_addr; addr <= stop_addr; addr += 4)
			REG32(addr) = 1 << (addr >> 2);

		/* Verify */
		for (addr = start_addr; addr <= stop_addr; addr += 4)
			if (REG32(addr) != (1 << (addr >> 2))) {
				err_no++;
				err_addr = addr;
			}
		if (err_no)
			printf("%04lx times failed. Last at location %08lx",
			       err_no, err_addr);
		else
			printf("Passed");
		err_no = 0;
	}

	/* Test 4: Write locations with walking zeros */
	if ((testno == 4) || (testno == 0)) {
		printf("\n4. Writing locations with walking zeros: ");
		for (addr = start_addr; addr <= stop_addr; addr += 4)
			REG32(addr) = ~(1 << (addr >> 2));

		/* Verify */
		for (addr = start_addr; addr <= stop_addr; addr += 4)
			if (REG32(addr) != ~(1 << (addr >> 2))) {
				err_no++;
				err_addr = addr;
			}
		if (err_no)
			printf("%04lx times failed. Last at location %08lx",
			       err_no, err_addr);
		else
			printf("Passed");
		err_no = 0;
	}
}

// Do proper walking 0 as byte writes
void better_ram_test(unsigned long start_addr, unsigned long stop_addr,
		     unsigned test_no)
{
	unsigned long addr;
	unsigned long err_addr = 0;
	unsigned long err_no = 0;
	int b;
	printf("\nSetting memory contents to all 1'b1  ");
	//enable_spincursor();
	for (addr = start_addr; addr <= stop_addr; addr += 1)
		REG8(addr) = 0xff;
	//disable_spincursor();
	printf("\rVerifying memory contents all set to 1'b1:  ");
	//enable_spincursor();
	/* Verify */
	for (addr = start_addr; addr <= stop_addr; addr += 1) {
		if (REG8(addr) != 0xff) {
			err_no++;
			err_addr = addr;
			//disable_spincursor();
			printf("\n%04lx times failed. Last at location %08lx  ",
			       err_no, err_addr);
			//enable_spincursor();
		}
	}

	if (err_no == 0)
		printf("Passed");
	else
		printf("Finished");

	err_no = 0;

	printf("\nWalking zero through memory, verify just itself: ");
	for (addr = start_addr; addr <= stop_addr; addr += 1) {
		for (b = 0; b < 8; b++) {
			// Write, verify
			REG8(addr) = ~(1 << b);

			if (REG8(addr) != ~(1 << b)) {
				err_no++;
				err_addr = addr;
				printf
				    ("\n%04lx times failed. Last at location %08lx",
				     err_no, err_addr);
			}
			REG8(addr) = 0xff;
		}
	}
	if (err_no == 0)
		printf("Passed");
	else
		printf("Finished");
	err_no = 0;

	printf("\nWriting across rows, row size configured as %d bytes",
	       SDRAM_ROW_SIZE);
	unsigned long start_row = start_addr / SDRAM_ROW_SIZE;
	unsigned long end_row = stop_addr / SDRAM_ROW_SIZE;
	for (addr = start_row; addr <= end_row; addr += 1) {
		REG32(addr * SDRAM_ROW_SIZE) = addr;
	}

	printf("\rVerifying row write test: ");

	for (addr = start_row; addr <= end_row; addr += 1) {
		if (REG32(addr * SDRAM_ROW_SIZE) != addr) {
			err_no++;
			err_addr = addr * SDRAM_ROW_SIZE;
			printf
			    ("\n%04lx times failed. Last at location %08lx (row %d)",
			     err_no, err_addr, addr);
		}
	}

	if (err_no == 0)
		printf("Passed");
	else
		printf("Finished");
	err_no = 0;

}

int dm_cmd(int argc, char *argv[])
{
	unsigned long a1, a2;
	a1 = strtoul(argv[0], 0, 0);
	switch (argc) {
	case 1:
		show_mem(a1, a1);
		return 0;
	case 2:
		a2 = strtoul(argv[1], 0, 0);
		show_mem(a1, a2);
		return 0;
	default:
		return -1;
	}
}

int pm_cmd(int argc, char *argv[])
{
	unsigned long addr, stop_addr, value;
	if ((argc == 3) || (argc == 2)) {
		addr = strtoul(argv[0], 0, 0);

		if (argc == 2) {
			stop_addr = strtoul(argv[0], 0, 0);
			value = strtoul(argv[1], 0, 0);
		} else {
			stop_addr = strtoul(argv[1], 0, 0);
			value = strtoul(argv[2], 0, 0);
		}

		for (; addr <= stop_addr; addr += 4)
			REG32(addr) = value;

		/*show_mem(strtoul (argv[0], 0, 0), stop_addr); */
	} else
		return -1;
	return 0;
}

int ram_test_cmd(int argc, char *argv[])
{
	switch (argc) {
	case 2:
		testram(strtoul(argv[0], 0, 0), strtoul(argv[1], 0, 0), 0);
		return 0;
	case 3:
		testram(strtoul(argv[0], 0, 0), strtoul(argv[1], 0, 0),
			strtoul(argv[2], 0, 0));
		return 0;
	default:
		return -1;
	}
}

int better_ram_test_cmd(int argc, char *argv[])
{
	if (argc < 2)
		return -1;

	better_ram_test(strtoul(argv[0], 0, 0), strtoul(argv[1], 0, 0), 0);
	return 0;
}

unsigned long crc32(unsigned long crc, const unsigned char *buf,
		    unsigned long len)
{
	/* Create bitwise CRC table first */
	unsigned long crc_table[256];
	int i, k;
	for (i = 0; i < 256; i++) {
		unsigned long c = (unsigned long)i;
		for (k = 0; k < 8; k++)
			c = c & 1 ? 0xedb88320 ^ (c >> 1) : c >> 1;
		crc_table[i] = c;
	}

	/* Calculate crc on buf */
	crc = crc ^ 0xffffffffL;
	while (len--)
		crc = crc_table[((int)crc ^ (*buf++)) & 0xff] ^ (crc >> 8);
	return crc ^ 0xffffffffL;
}

int crc_cmd(int argc, char *argv[])
{
	unsigned long addr = global.src_addr;
	unsigned long len = global.length;
	unsigned long init_crc = 0;

	switch (argc) {
	case 3:
		init_crc = strtoul(argv[2], 0, 0);
	case 2:
		len = strtoul(argv[1], 0, 0);
	case 1:
		addr = strtoul(argv[0], 0, 0);
	case 0:
		printf("CRC [%08lx-%08lx] = %08lx\n", addr, addr + len - 1,
		       crc32(init_crc, (unsigned char *)addr, len));
		return 0;
	}
	return -1;
}

void module_memory_init(void)
{
	register_command("dm", "<start addr> [<end addr>]",
			 "display 32-bit memory location(s)", dm_cmd);
	register_command("pm", "<addr> [<stop_addr>] <value>",
			 "patch 32-bit memory location(s)", pm_cmd);
	register_command("ram_test", "<start_addr> <stop_addr> [<test_no>]",
			 "run a simple RAM test", ram_test_cmd);
	register_command("better_ram_test", "<start_addr> <stop_addr>",
			 "run a better RAM test", better_ram_test_cmd);

	register_command("crc", "[<src_addr> [<length> [<init_crc>]]]",
			 "Calculates a 32-bit CRC on specified memory region",
			 crc_cmd);
}
