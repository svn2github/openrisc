/*
    atabug.c -- ATA debugging
    Copyright (C) 2002 Richard Herveille, rherveille@opencores.org

    This file is part of OpenRISC 1000 Reference Platform Monitor (ORPmon)

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include "support.h"
#include "common.h"
#include "atabug.h"
#include "ata.h"

#include <ctype.h>

static int ata_num_commands;
static command_struct ata_command[MAX_ATA_COMMANDS];

/* inode struct for ata */
static struct inode _inode;
static struct inode *inode = &_inode;

/* file struct for ata */
static struct file _filp;
static struct file *filp = &_filp;

/* buffer for ata-data */
static unsigned char buf[512];

/**********************************************************************/
/*                                                                    */
/*      A T A B U G                                                   */
/*                                                                    */
/**********************************************************************/
/*
	A T A _ I N I T

	initializes the ATA core, and registers it with ORPmon
*/
void module_ata_init(void)
{
	ata_num_commands = 0;

	register_command("atabug", "",
			 "ATA debugger. Type 'atabug help' for help", atabug);

	register_ata_command("help", "", "Display this help message",
			     atabug_help);
	register_ata_command("exit", "", "Exit atabug and return to ORPmon",
			     atabug_exit);
	register_ata_command("open", "<device> [<mode>]",
			     "Opens the requested device. Device=<0|1>, Mode=<r>eadonly|read<w>rite.",
			     ata_open_cmd);
	register_ata_command("close", "", "Closes the device.", ata_close_cmd);
	register_ata_command("reset", "<mode>", "Reset ata device(s).",
			     ata_reset_cmd);
	register_ata_command("enable", "",
			     "Enables ATA host controller, clears all resets",
			     ata_enable_cmd);
	register_ata_command("dump_dev_regs", "",
			     "Dump the (readable) ata device registers.",
			     ata_dump_device_regs_cmd);
	register_ata_command("dump_host_regs", "",
			     "Dump the ata host registers.",
			     ata_dump_host_regs_cmd);
	register_ata_command("exec_cmd", "<cmd>",
			     "Execute ata command <cmd> (hex)",
			     ata_exec_cmd_cmd);
	register_ata_command("identify_device", "",
			     "Dumps device's IDENTIFY DEVICE block.",
			     ata_identify_device_cmd);
	register_ata_command("program_timing", "<PIO mode>",
			     "Programs the device to the selected PIO mode.",
			     ata_set_piomode_cmd);
	register_ata_command("read_sectors", "<startsector> [<sectorcount>]",
			     "Reads sector", ata_read_sectors_cmd);
	register_ata_command("read_mbr", "<partition>",
			     "Reads the Master Boot Record.", ata_read_mbr_cmd);
	register_ata_command("read_dosboot", "<sector>",
			     "Reads the device's bootsector (FAT).",
			     ata_read_dosboot_cmd);
	register_ata_command("select_device", "<device_no>",
			     "Select ata device. device_no=<0|1>",
			     ata_select_device_cmd);
}

int atabug(int argc, char **argv)
{

	/* take care of commandline options                                 */
	if (argc == 0) {
		/* start atabug                                                   */
		while (!ata_mon_command()) ;
	} else
		return execute_ata_command(argv[0], argc - 1, &argv[1]);

	return 0;
}

int atabug_exit(int argc, char **argv)
{
	ata_close_cmd(argc, argv);
	return -2;
}

/*
 The next code is graceously taken from the "common.c" file
 and slightly modified to suit the big list of ATA commands

 Better would be if we could access the routines in 'common.c'
 directly, using our own set of commands.
*/

/* Process command-line, generate arguments */
int ata_mon_command(void)
{
	char c = '\0';
	char str[1000];
	char *pstr = str;
	char *command_str;
	char *argv[20];
	int argc = 0;

	/* Show prompt */
	printf("\natabug> ");

	/* Get characters from UART */
	c = getc();
	while (c != '\r' && c != '\f' && c != '\n') {
		if (c == '\b')
			pstr--;
		else
			*pstr++ = c;
		putc(c);
		c = getc();
	}
	*pstr = '\0';
	printf("\n");

	/* Skip leading blanks */
	pstr = str;
	while (isspace(*pstr))
		pstr++;

	/* Get command from the string */
	command_str = pstr;

	while (1) {
		/* Go to next argument */
		while (isgraph(*pstr))
			pstr++;
		if (*pstr) {
			*pstr++ = '\0';
			while (isspace(*pstr))
				pstr++;
			argv[argc++] = pstr;
		} else
			break;
	}

	return execute_ata_command(command_str, argc, argv);
}

int execute_ata_command(char *command_str, int argc, char **argv)
{
	int i, found = 0;

	for (i = 0; i < ata_num_commands; i++)
		if (!strcmp(command_str, ata_command[i].name)) {
			switch (ata_command[i].func(argc, argv)) {
			case -1:
				printf
				    ("Missing/wrong parameters, usage: %s %s\n",
				     ata_command[i].name,
				     ata_command[i].params);
				break;

			case -2:
				return -1;
			}

			found++;
			break;
		}

	if (!found)
		printf("Unknown command. Type 'ata help' for help.\n");

	return 0;
}

void register_ata_command(const char *name, const char *params,
			  const char *help, int (*func) (int argc,
							 char *argv[]))
{
	if (ata_num_commands < MAX_ATA_COMMANDS) {
		ata_command[ata_num_commands].name = name;
		ata_command[ata_num_commands].params = params;
		ata_command[ata_num_commands].help = help;
		ata_command[ata_num_commands].func = func;
		ata_num_commands++;
	} else
		printf("ata-command '%s' ignored; MAX_COMMANDS limit reached\n",
		       name);
}

int atabug_help(int argc, char **argv)
{
	int i;

	for (i = 0; i < ata_num_commands; i++)
		printf("%-15s %-17s -%s\n", ata_command[i].name,
		       ata_command[i].params, ata_command[i].help);

	return 0;
}

/**********************************************************************/
/*                                                                    */
/*      A T A B U G   C O M M A N D S E T                             */
/*                                                                    */
/**********************************************************************/

/*
	A T A _ C L O S E

	closes the ata_device
*/
int ata_close_cmd(int argc, char **argv)
{
	inode->i_rdev = (ATA_BASE_ADDR >> 16) | (*argv[0] - '0');

	return ata_release(inode, filp);
}

/*
	A T A _ D U M P _ D E V I C E _ R E G S

	Dumps the (readable) ata-registers.
	Exception: status register is not read, this could mask an interrupt
*/
int ata_dump_device_regs_cmd(int argc, char **argv)
{
	if (argc)
		printf("\nWARNING: Ignoring invalid argument(s)\n\n");

	printf("Alternate status register : 0x%02lX\n",
	       REG32(ATA_BASE_ADDR + ATA_ASR));
	printf("Cylinder high register    : 0x%02lX\n",
	       REG32(ATA_BASE_ADDR + ATA_CHR));
	printf("Cylinder low register     : 0x%02lX\n",
	       REG32(ATA_BASE_ADDR + ATA_CLR));
	printf("Device head register      : 0x%02lX\n",
	       REG32(ATA_BASE_ADDR + ATA_DHR));
	printf("Error register            : 0x%02lX\n",
	       REG32(ATA_BASE_ADDR + ATA_ERR));
	printf("Sector count register     : 0x%02lX\n",
	       REG32(ATA_BASE_ADDR + ATA_SCR));
	printf("Sector number register    : 0x%02lX\n",
	       REG32(ATA_BASE_ADDR + ATA_SNR));
	printf("Status register (see alternate status register)\n");

	return 0;
}

/*
	A T A _ D U M P _ H O S T _ R E G S

	Dumps the ata-host registers
*/
int ata_dump_host_regs_cmd(int argc, char **argv)
{
	if (argc)
		printf("\nWARNING: Ignoring invalid argument(s)\n\n");

	printf("Control register                   CTRL : 0x%08lX\n",
	       REG32(ATA_BASE_ADDR + ATA_CTRL));
	printf("Status register                    STAT : 0x%08lX\n",
	       REG32(ATA_BASE_ADDR + ATA_STAT));
	printf("Pio command timing register        PCTR : 0x%08lX\n",
	       REG32(ATA_BASE_ADDR + ATA_PCTR));
	printf("Pio fast timing register (device0) PFTR0: 0x%08lX\n",
	       REG32(ATA_BASE_ADDR + ATA_PFTR0));
	printf("Pio fast timing register (device1) PFTR1: 0x%08lX\n",
	       REG32(ATA_BASE_ADDR + ATA_PFTR1));
	printf("Dma timing register (device0)      DTR0 : 0x%08lX\n",
	       REG32(ATA_BASE_ADDR + ATA_DTR0));
	printf("Dma timing register (device1)      DTR1 : 0x%08lX\n",
	       REG32(ATA_BASE_ADDR + ATA_DTR1));

	return 0;
}

/*
	A T A _ E N A B L E

	clears reset bits
*/
int ata_enable_cmd(int argc, char **argv)
{
	if (argc != 0)
		printf("Ignoring invalid parameters\n");

	inode->i_rdev = (ATA_BASE_ADDR >> 16);

	// clear hardware reset bit
	if (ata_ioctl(inode, filp, ATA_IOCTL_SET_RST, CLR | ARG_HW_RST))
		return -1;

	// clear software reset bit
	if (ata_ioctl(inode, filp, ATA_IOCTL_SET_RST, CLR | ARG_SW_RST))
		return -1;

	// enable ATA Hostcontroller core
	if (ata_ioctl(inode, filp, ATA_IOCTL_ENABLE_HOST, 0))
		return -1;

	printf("ATA host controller enabled\n");

	return 0;
}

/*
	A T A _ E X E C _ C M D

	Executes the command; writes the command number in the command register
*/
int ata_exec_cmd_cmd(int argc, char **argv)
{
	if (argc != 1)
		return -1;

	inode->i_rdev = (ATA_BASE_ADDR >> 16);

	ata_ioctl(inode, filp, ATA_IOCTL_EXEC_CMD, strtoul(*argv, argv, 16));
	return 0;
}

/*
	A T A _ I D E N T I F Y _ D E V I C E

	Reads the identify_device block and dumps it to the screen
*/
int ata_identify_device_cmd(int argc, char **argv)
{
	unsigned char checksum;

	if (argc != 0)
		printf("Ignoring invalid parameters\n");

	/* check for busy flag                                            */
	if (ata_dev_busy(ATA_BASE_ADDR))
		printf("Selected ata device busy, ignoring command\n");
	else {
		/* execute identify device                                    */
		ata_ioctl(inode, filp, ATA_IOCTL_EXEC_CMD, IDENTIFY_DEVICE);

		/* read block from ata-device                                 */
		buf[0] = 0;
		buf[1] = 1;
		ata_ioctl(inode, filp, ATA_IOCTL_READ, (unsigned long)buf);

		/* dump data to the screen                                    */
		checksum = atabug_dump_data(buf, 512);

		if (buf[512] == 0xa5)
			printf("Checksum = 0x%02X (%s)\n", checksum,
			       checksum ? "error" : "OK");
		else
			printf("No checksum supported\n");
	}
	return 0;
}

/*
	A T A _ O P E N

	opens the ata_device
*/
int ata_open_cmd(int argc, char **argv)
{
	inode->i_rdev = (ATA_BASE_ADDR >> 16) | (*argv[0] - '0');

	filp->f_mode = FMODE_READ;

	if (*argv[1] == 'w')
		filp->f_mode |= FMODE_WRITE;

	switch (ata_open(inode, filp)) {
	case EOPENIDEV:
		printf("Error: Invalid device (invalid MINOR %02X)\n",
		       MINOR(inode->i_rdev));
		break;

	case EOPENNODEV:
		printf("Error: Requested device not found\n");
		break;

	case EOPENIHOST:
		printf("Error: Invalid host (invalid MAJOR %02X)\n",
		       MAJOR(inode->i_rdev));
	default:
		break;
	}

	return 0;
}

/*
	A T A _ S E T _ P I O M O D E

	Sets the device to the requested PIO mode
*/
int ata_set_piomode_cmd(int argc, char **argv)
{
	return 0;
}

/*
	A T A _ R E A D _ S E C T O R S

	Reads 1 sector from the device and dumps it to the screen
*/
int ata_read_sectors_cmd(int argc, char **argv)
{
	struct request request;
	unsigned long sector_cnt, sector;

	sector = strtoul(argv[0], argv, 10);

	switch (argc) {
	case 2:
		sector_cnt = strtoul(argv[1], argv, 10);
		break;

	case 1:
		sector_cnt = 1;
		break;

	default:
		return -1;
	}

	if (!sector_cnt) {
		printf("Invalid number of sectors.\n");
		return 0;
	}

	/* check for busy flag                                            */
	if (ata_dev_busy(ATA_BASE_ADDR))
		printf("Selected ata device busy, ignoring command\n");
	else {
		/* fill the request structure                                 */
		request.cmd = READ;
		request.sector = sector;
		request.nr_sectors = sector_cnt;
		request.buffer = buf;

		if (ata_request(inode, filp, &request)) {
			printf
			    ("Error while executing READ_SECTOR(S) command\n");
			printf
			    ("Status register = 0x%02lX, error register = 0x%02lX\n",
			     ata_astatus(ATA_BASE_ADDR),
			     ata_error(ATA_BASE_ADDR));
		} else {
			/* dump data to the screen                                    */
			atabug_dump_data(buf, 512 * sector_cnt);
		}
	}
	return 0;
}

/*
	A T A _ R E A D _ M B R

	Reads master boot record from the device and dumps it's contents to the screen
*/
int ata_read_mbr_cmd(int argc, char **argv)
{
	struct request request;
	unsigned int partition;

	// get requested partition number
	partition = 0;
	if (argc)
		partition = strtoul(*argv, argv, 10);

	/* check for busy flag                                            */
	if (ata_dev_busy(ATA_BASE_ADDR))
		printf("Selected ata device busy, ignoring command\n");
	else {
		/* fill the request structure                                 */
		request.cmd = READ;
		request.sector = 0;
		request.nr_sectors = 1;
		request.buffer = buf;

		if (ata_request(inode, filp, &request)) {
			printf("Error while reading master boot sector.\n");
			printf
			    ("Status register = 0x%02lX, error register = 0x%02lX\n",
			     ata_astatus(ATA_BASE_ADDR),
			     ata_error(ATA_BASE_ADDR));
		} else {
			printf("Skipping bootloader (446bytes)\n");
			printf("Partition %1d:\n", partition);

			// abuse partitionnumber to get offset in MBR record
			partition *= 16;
			partition += 446;

			printf("Bootindicator: 0x%2X (%s)\n", buf[partition],
			       buf[partition] ? "bootable" : "non-bootable");
			printf
			    ("Partition start (head: 0x%02X cyl: 0x%03X sect: 0x%02X)\n",
			     buf[partition + 1],
			     (buf[partition + 2] & 0xc0) << 2 | buf[partition +
								    3],
			     buf[partition + 2] & 0x3f);
			printf("Systemindicator: 0x%02X (", buf[partition + 4]);

			switch (buf[partition + 4]) {
			case 0:
				printf("Non DOS");
				break;
			case 1:
				printf("DOS FAT12");
				break;
			case 4:
				printf("DOS FAT16");
				break;
			case 5:
				printf("DOS extended");
				break;
			case 6:
				printf("DOS >32MByte");
				break;

			default:
				printf("unkown");
			};
			printf(")\n");
			printf
			    ("Partition end (head: 0x%02X cyl: 0x%03X sect: 0x%02X)\n",
			     buf[partition + 5],
			     (buf[partition + 6] & 0xc0) << 2 | buf[partition +
								    7],
			     buf[partition + 6] & 0x3f);
			printf("Physical Startsector: 0x%08X\n",
			       buf[partition + 11] << 24 | buf[partition +
							       10] << 16 |
			       buf[partition + 9] << 8 | buf[partition + 8]);
			printf("Sector count: 0x%08X\n",
			       buf[partition + 15] << 24 | buf[partition +
							       14] << 16 |
			       buf[partition + 13] << 8 | buf[partition + 12]);
		}
	}
	return 0;
}

/*
	A T A _ R E A D _ D O S B O O T

	Reads boot sector from the device and dumps it's contents to the screen
*/
int ata_read_dosboot_cmd(int argc, char **argv)
{
	struct request request;
	unsigned int sector;
	char txt[8];

	sector = 0;
	if (argc)
		sector = strtoul(*argv, argv, 0);

	/* check for busy flag                                            */
	if (ata_dev_busy(ATA_BASE_ADDR))
		printf("Selected ata device busy, ignoring command\n");
	else {
		/* fill the request structure                                 */
		request.cmd = READ;
		request.sector = sector;
		request.nr_sectors = 1;
		request.buffer = buf;

		if (ata_request(inode, filp, &request)) {
			printf("Error whilereading boot sector 0x%02X.\n",
			       sector);
			printf
			    ("Status register = 0x%02lX, error register = 0x%02lX\n",
			     ata_astatus(ATA_BASE_ADDR),
			     ata_error(ATA_BASE_ADDR));
		} else {
			printf("Reading boot sector 0x%02X\n", sector);
			printf("ID number: 0x%2X%2X%2X\n", buf[0], buf[1],
			       buf[2]);

			printf("OEM-name and number: ");
			memcpy(txt, &buf[3], 8);
			txt[8] = '\0';
			printf("%s\n", txt);

			printf("Bytes per sector: %5d\n",
			       (buf[12] << 8) | buf[11]);
			printf("Sectors per cluster: %3d\n", buf[13]);
			printf("Reserved IM-sectors: %5d\n",
			       (buf[15] << 8) | buf[14]);
			printf("Number of FATs: %3d\n", buf[16]);
			printf("Number of entries in the root-directory: %5d\n",
			       (buf[18] << 8) | buf[17]);
			printf("Number of logical sectors: %5d\n",
			       (buf[20] << 8) | buf[19]);
			printf("Medium descriptor byte: %02X\n", buf[21]);
			printf("Sectors per FAT: %5d\n",
			       (buf[23] << 8) | buf[22]);
			printf("Sectors per track: %5d\n",
			       (buf[25] << 8) | buf[24]);
			printf("Number of heads: %5d\n",
			       (buf[27] << 8) | buf[26]);
			printf("Number of hidden sectors: %5d\n",
			       (buf[29] << 8) | buf[28]);
		}
	}
	return 0;
}

/*
	A T A _ R E S E T

	resets the ATA device, using the select method
*/
int ata_reset_cmd(int argc, char **argv)
{
	if (argc != 1)
		return -1;

	return ata_ioctl(inode, filp, ATA_IOCTL_SET_RST, SET | (**argv - '0'));
}

/*
	A T A _ S E L E C T _ D E V I C E

	selects the ATA device; sets the DEV bit in the device/head register
*/
int ata_select_device_cmd(int argc, char **argv)
{
	if (argc != 1)
		return -1;

	inode->i_rdev = (ATA_BASE_ADDR >> 16) | (*argv[0] - '0');

	ata_ioctl(inode, filp, ATA_IOCTL_SELECT_DEVICE, **argv - '0');

	printf("Ata device %1d selected.\n",
	       REG32(ATA_BASE_ADDR + ATA_DHR) & ATA_DHR_DEV ? 1 : 0);
	return 0;
}

/**********************************************************************/
/*                                                                    */
/*      A T A B U G   T O O L S                                       */
/*                                                                    */
/**********************************************************************/

/*
 D U M P _ D A T A

 dumps byte-data in a buffer of type short to the screen
 and returns the byte-checksum

 *buffer = pointer to (short)buffer
 cnt     = number of bytes to display
*/
unsigned char atabug_dump_data(unsigned char *buffer, int cnt)
{
	int i, n, bytes_per_line = 16;
	unsigned char c, checksum;
	unsigned char *buf_ptr;

	/* prepare stored data for display & calculate checksum           */
	checksum = 0;
	buf_ptr = buffer;

	/* display data                                                   */
	for (i = 0; i < cnt; i += bytes_per_line) {
		printf("%3X ", i);

		/* print hexadecimal notation                                   */
		for (n = 0; n < bytes_per_line; n++)
			printf("%02X ", *buf_ptr++);

		buf_ptr -= bytes_per_line;	/* back to the start (of this block) */

		/* print ASCII notation & calculate checksum                    */
		for (n = 0; n < bytes_per_line; n++) {
			c = *buf_ptr++;
			printf("%c", isprint(c) ? c : '.');
			checksum += c;
		}
		printf("\n");
	}

	return checksum;
}
