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

#ifndef __OC_ATABUG_H
#define __OC_ATABUG_H


#define ATA_DEBUG


#define MAX_ATA_COMMANDS  25

struct partition {
  char  boot;
  char  start_head;
  short start_cylinder;
  char  start_sector;
  char  system;
  char  end_head;
  short end_cylinder;
  char  end_sector;
  int   start;
  int   sectors;
};

/* ----------------------------                                       */
/* ----- Prototypes       -----                                       */
/* ----------------------------                                       */
void module_ata_init (void);
int atabug(int argc, char **argv);
int atabug_exit(int argc, char **argv);
int atabug_help(int argc, char **argv);
void register_ata_command (const char *name, const char *params, const char *help, int (*func)(int argc, char *argv[]));
int ata_mon_command(void);
int execute_ata_command(char *pstr, int argc, char **argv);

int ata_close_cmd(int arc, char **argv);
int ata_dump_device_regs_cmd(int argc, char **argv);
int ata_dump_host_regs_cmd(int argc, char **argv);
int ata_dump_dataport_cmd(int argc, char **argv);
int ata_enable_cmd(int argc, char **argv);
int ata_exec_cmd_cmd(int argc, char **argv);
int ata_identify_device_cmd(int argc, char **argv);
int ata_open_cmd(int argc, char **argv);
int ata_read_sectors_cmd(int argc, char **argv);
int ata_read_mbr_cmd(int argc, char **argv);
int ata_read_dosboot_cmd(int argc, char **argv);
int ata_reset_cmd(int argc, char **argv);
int ata_select_device_cmd(int argc, char **argv);
int ata_set_piomode_cmd(int argc, char **argv);

unsigned char atabug_dump_data(unsigned char *buffer, int cnt);

#endif
