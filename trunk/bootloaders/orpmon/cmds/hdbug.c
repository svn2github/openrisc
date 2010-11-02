/*
    hdbug.c -- harddisk debugging
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
#include "dos.h"
#include "hdbug.h"
#include <ctype.h>

static int hdbug_num_commands;
static command_struct hdbug_command[MAX_HDBUG_COMMANDS];

static struct dosparam _dos_params;
static struct dosparam *dos_params = &_dos_params;

/**********************************************************************/
/*                                                                    */
/*      H D B U G                                                     */
/*                                                                    */
/**********************************************************************/
/*
	H D B U G _ I N I T

	initializes the ata core, mounts the DOS file system, and
	provides methods for accessing DOS drives
*/
void module_hdbug_init(void)
{
	hdbug_num_commands = 0;

	register_command("hdbug", "<device> [<mode>]",
			 "Opens ata device <device> & mounts DOS filesystem",
			 hdbug_mount_cmd);
	register_hdbug_command("umount", "",
			       "Unmounts DOS filesystem & Closes device",
			       hdbug_umount_cmd);

	register_hdbug_command("dir", "", "dos 'dir' command.", hdbug_dir_cmd);
	register_hdbug_command("cd", "", "dos 'cd' command.", hdbug_cd_cmd);
	register_hdbug_command("help", "", "Display this help message",
			       hdbug_help);
	register_hdbug_command("exit", "", "Exit hdbug and return to ORPmon",
			       hdbug_umount_cmd);
}

/*
 The next code is graceously taken from the "common.c" file
 and slightly modified.

 Better would be if we could access the routines in 'common.c'
 directly, using our own set of commands.
*/

/* Process command-line, generate arguments */
int hdbug_mon_command(void)
{
	char c = '\0';
	char str[1000];
	char *pstr = str;
	char *command_str;
	char *argv[20];
	int argc = 0;

	/* Show prompt */
	printf("\nhdbug> ");

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

	return execute_hdbug_command(command_str, argc, argv);
}

int execute_hdbug_command(char *command_str, int argc, char **argv)
{
	int i, found = 0;

	for (i = 0; i < hdbug_num_commands; i++)
		if (!strcmp(command_str, hdbug_command[i].name)) {
			switch (hdbug_command[i].func(argc, argv)) {
			case -1:
				printf
				    ("Missing/wrong parameters, usage: %s %s\n",
				     hdbug_command[i].name,
				     hdbug_command[i].params);
				break;

			case -2:
				return -1;
			}

			found++;
			break;
		}

	if (!found)
		printf("Unknown command. Type 'hdbug help' for help.\n");

	return 0;
}

void register_hdbug_command(const char *name, const char *params,
			    const char *help, int (*func) (int argc,
							   char *argv[]))
{
	if (hdbug_num_commands < MAX_HDBUG_COMMANDS) {
		hdbug_command[hdbug_num_commands].name = name;
		hdbug_command[hdbug_num_commands].params = params;
		hdbug_command[hdbug_num_commands].help = help;
		hdbug_command[hdbug_num_commands].func = func;
		hdbug_num_commands++;
	} else
		printf
		    ("hdbug-command '%s' ignored; MAX_COMMANDS limit reached\n",
		     name);
}

int hdbug_help(int argc, char **argv)
{
	int i;

	for (i = 0; i < hdbug_num_commands; i++)
		printf("%-15s %-17s -%s\n", hdbug_command[i].name,
		       hdbug_command[i].params, hdbug_command[i].help);

	return 0;
}

/**********************************************************************/
/*                                                                    */
/*     H D B U G   C O M M A N D S E T                                */
/*                                                                    */
/**********************************************************************/

/*
	H D B U G _ M O U N T

	opens the ata-device and mounts the dos filesystem
*/
int hdbug_mount_cmd(int argc, char **argv)
{
	int error;

	if (argc != 2)
		return -1;

	/* try to open the requested device (read-only)                     */
	dos_params->inode.i_rdev = (ATA_BASE_ADDR >> 16) | (**argv - '0');
	dos_params->filp.f_mode = FMODE_READ;

	/* open device                                                      */
	if ((error = dos_open(dos_params))) {
		switch (error) {
		case EINVAL:
			printf("Error, device busy.\n");	/* standard response to EINVAL */
			break;

		case EIOCTLIARG:
			printf("Error, invalid IOCTL call.\n");
			break;

		case EOPENIDEV:
			printf("Error, invalid device.\n");
			break;

		case EOPENIHOST:
			printf("Error, ata host controller not found.\n");
			break;

		case EOPENNODEV:
			printf("Error, ata-device not found.\n");
			break;

		default:
			printf("Unkown error.\n");
			break;
		}
	} else {
		printf("directory startsector: 0x%08lX\n", dos_params->ssector);
		printf("cluster startsector  : 0x%08lX\n", dos_params->csector);
		printf("cluster startentry   : 0x%08lX\n", dos_params->sentry);

		/* device is opened, filesystem is mounted, start command prompt  */
		while (!hdbug_mon_command()) ;
	}

	return 0;
}

/*
	H D B U G _ U M O U N T

	unmounts the dos filesystem and closes the ata-device
*/
int hdbug_umount_cmd(int argc, char **argv)
{
	dos_release(dos_params);

	return -2;
}

/*
	H D B U G _ C D
*/
int hdbug_cd_cmd(int argc, char **argv)
{
	struct dos_dir_entry *entry;

	switch (argc) {
	case 0:
		/* display present working directory                            */
		printf("FIXME: present working directory\n");
		return 0;

	case 1:
		break;

	default:
		printf("Too many arguments.\n");
		return 0;
	}

	/* search for the requested directory                               */
	if (!(entry = dos_dir_find_entry(dos_params, *argv))) {
		printf("The system cannot find the specified path.\n");
		return 0;
	}

	return 0;
}

/*
	H D B U G _ D I R
*/
int hdbug_dir_cmd(int argc, char **argv)
{
	register int i;

	/* read the directory structures from the current directory
	   and display the results                                        */

	/* TODO: Add sub-directories */

	/* get first cluster of current directory                         */
	dos_dir_cluster_reset(dos_params);

	for (i = 0; i < dos_params->root_entries; i++)
		hdbug_dir_print(dos_dir_get_entry(dos_params, i));

	return 0;
}

int hdbug_dir_print(struct dos_dir_entry *entry)
{
	unsigned long ltmp;
	unsigned short stmp;

	char txt[9];

	switch (entry->name[0]) {
	case 0x00:
		/* empty entry */
		break;

	case 0xe5:
		/* deleted/removed entry */
		break;

	default:
		/* check if entry is a label                                    */
		if (entry->attribute & ATT_LAB) {
			printf("LABEL: ");
			memcpy(txt, entry->name, 8);
			txt[8] = '\0';
			printf("%s", txt);
			memcpy(txt, entry->ext, 3);
			txt[3] = '\0';
			printf("%s\n", txt);
		} else {
			/* display date & time                                          */
			stmp = entry->date;
			swap(&stmp, sizeof(short));
			printf("%02d-%02d-%4d  ", stmp & 0x1f,
			       (stmp >> 5) & 0xf,
			       ((stmp >> 9) & 0xffff) + 1980);

			stmp = entry->time;
			swap(&stmp, sizeof(short));
			printf("%02d:%02d  ", (stmp >> 11) & 0x1f,
			       (stmp >> 5) & 0x3f);

			/* display directory bit                                        */
			printf("%s  ",
			       entry->attribute & ATT_DIR ? "<DIR>" : "     ");

			/* display filesize                                             */
			ltmp = entry->size;
			swap(&ltmp, sizeof(unsigned long));
			printf("%12ld  ", ltmp);

			/* replace the first 'space' in the name by an null char        */
			*(char *)memchr(entry->name, 0x20, 8) = '\0';
			printf("%s", entry->name);

			/* add extension                                                */
			if (entry->ext[0] != 0x20) {
				printf(".%3s", entry->ext);
			}

			printf("\n");
			break;
		}
	}

	return 0;
}

/*
	H D B U G   T O O L S
*/
inline void *swap(void *var, size_t size)
{
	switch (size) {
	case 1:
		return var;

	case 2:
		{
			unsigned short p = *(unsigned short *)var;
			*(unsigned short *)var = (p << 8) | (p >> 8);
			return var;
		}

	case 4:
		{
			unsigned long *p = (unsigned long *)var;
			*p = (*p << 24) | ((*p & 0x0000ff00) << 8) |
			    ((*p & 0x00ff0000) >> 8) | (*p >> 24);
			return var;
		}

	default:
		return NULL;
	}
}
