/*
    atabug.h -- ATA debugging (C-header file)
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

/*
 * Definitions for the Opencores ATA Controller Core
 */

#ifndef __OC_HDBUG_H
#define __OC_HDBUG_H


#define HD_DEBUG


#define MAX_HDBUG_COMMANDS  10

/* ----------------------------                                       */
/* ----- Prototypes       -----                                       */
/* ----------------------------                                       */
void module_hdbug_init (void);
int hdbug(int argc, char **argv);
int hdbug_exit(int argc, char **argv);
int hdbug_help(int argc, char **argv);
void register_hdbug_command (const char *name, const char *params, const char *help, int (*func)(int argc, char *argv[]));
int hdbug_mon_command(void);
int execute_hdbug_command(char *pstr, int argc, char **argv);

int hdbug_umount_cmd(int arc, char **argv);
int hdbug_mount_cmd(int argc, char **argv);

int hdbug_cd_cmd(int argc, char **argv);

int hdbug_dir_cmd(int argc, char **argv);
int hdbug_dir_print(struct dos_dir_entry *entry);


inline void *swap(void *var, size_t size);

#endif
