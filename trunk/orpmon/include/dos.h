/*
    dos.h -- dos services (C-header file)
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

#ifndef __OC_DOS_H
#define __OC_DOS_H

#include "ata.h"

struct dosparam {
  unsigned short bytes_per_sector;
  unsigned char  fats;             // number of FATs
  unsigned char  sectors_per_cluster;
  unsigned short sectors_per_fat;
  unsigned short root_entries;     // number of entries in the root sector

  unsigned long ssector;           // startsector of current directory
  unsigned long csector;           // startsector of current cluster
  unsigned long sentry;            // startentry of current cluster
  unsigned char cbuf[8192];        // cluster buffer. This should be dynamically alocated

  /* ata driver structures */
  struct inode inode;
  struct file filp;
  struct request request;
};


struct dos_dir_entry {
  unsigned char name[8];
  unsigned char ext[3];

  unsigned char attribute;  // attributes

  unsigned char reserved[10];

  unsigned short time;      // time of creation/modification
  unsigned short date;      // date of creation/modification

  unsigned short sc;        // startcluster
  unsigned long  size;      // file size
};


#define ATT_ARC 0x20
#define ATT_DIR 0x10
#define ATT_LAB 0x08
#define ATT_SYS 0x04
#define ATT_HID 0x02
#define ATT_RW  0x01


/*
  macros
*/
#define bytes_per_cluster(param)   (unsigned long)(param->sectors_per_cluster * param->bytes_per_sector)
#define entries_per_cluster(param) (unsigned long)(bytes_per_cluster(param) / sizeof(struct dos_dir_entry) )


/*
  prototypes
*/
int dos_open(struct dosparam *params);
int dos_release(struct dosparam *params);

struct dos_dir_entry *dos_dir_get_entry(struct dosparam *params, unsigned long entry);
struct dos_dir_entry *dos_dir_find_entry(struct dosparam *params, const char *name);

char *dos_read_cluster(struct dosparam *params, unsigned long ssector);
char *dos_dir_cluster_read_nxt(struct dosparam *params);
char *dos_dir_cluster_reset(struct dosparam *params);

#endif
