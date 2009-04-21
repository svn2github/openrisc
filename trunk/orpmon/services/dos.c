/*
    dos.c -- provides simple access to FAT (dos) partitions
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


//#include "common.h"

#include <stddef.h>
#include <string.h>
#include "dos.h"
#include "ata.h"


/*
  D O S _ O P E N
*/
int dos_open(struct dosparam *params)
{
  int error, start_sector, partition;
  unsigned char buf[512];

  struct inode *inode = &params->inode;
  struct file  *filp  = &params->filp;
  struct request *request = &params->request;

  if( (error = ata_open(inode, filp)) )
    return error;


  /* device opened, read MBR                                          */
  request->cmd = READ;
  request->sector = 0;
  request->nr_sectors = 1;
  request->buffer = buf;

  /* skip bootload (446) bytes                                        */
  /* currently only support the first partition (partition 4)         */
  /* This is OK, because CompactFLASH devices only have 1 partition   */
  if ( (error = ata_request(inode, filp, request)) )
    return error;

  partition = 0;    /* first partition                                */
  partition *= 16;  /* 16 bytes per partition table                   */
  partition += 446; /* skip bootloader, go to partition table         */
  start_sector = buf[partition +11] << 24 |
                 buf[partition +10] << 16 |
		 buf[partition +9] << 8   |
		 buf[partition +8];

  /* device opened, read boot-sector                                  */
  request->cmd = READ;
  request->sector = start_sector;
  request->nr_sectors = 1;
  request->buffer = buf;

  if ( (error = ata_request(inode, filp, request)) )
    return error;

  /* get the necessary data from the boot-sector                      */
  params->bytes_per_sector = (buf[12]<<8) | buf[11];
  params->fats = buf[16];
  params->sectors_per_fat = (buf[23]<<8) | buf[22];
  params->root_entries = (buf[18]<<8) | buf[17];
  params->sectors_per_cluster = buf[13];


  /* set start of current directory to start of root-directory        */
  params->ssector = start_sector + params->fats * params->sectors_per_fat +1;

  /* set current sector to start of root-directory                    */
  params->csector = params->ssector;

  /* set start-entry number                                           */
  params->sentry = 0;

  return 0;
}

/*
  D O S _ R E L E A S E
*/
int dos_release(struct dosparam *params)
{
  return 0;
}

/*
  D O S _ N A M E C M P
*/
int dos_namecmp(const char *sname, const char *name, const char *ext)
{
  char fname[9], fext[4], *p;

  /* filename :                                                       */
  /* copy the filename                                                */
  strncpy(fname, sname, 8);

  /* check if we copied the '.' already, if so terminate string       */
  if ( (p = strchr(fname, '.')) )
    *p = '\0';

  /* fill remaining chars with ' '                                    */
  strncat(fname, "        ", 8-strlen(fname) );

  fname[9] = '\0';

  /* file-extension                                                   */
  /* search for the '.' in the filename                               */
  if ( (p = strchr(sname, '.')) )
    strncpy(fext, p, 3);
  else
    fext[0] = fext[1] = fext[2] = ' ';

  fext[4] = '\0';

  return ( strcmp(fname, name) && strcmp(fext, ext) );
}


/*
  D O S _ D I R _ F I N D _ E N T R Y
*/
struct dos_dir_entry *dos_dir_find_entry(struct dosparam *params, const char *name)
{
  struct dos_dir_entry *entry;
  unsigned long entry_no = 0;

  /* go to start of current directory                                 */
  if (params->csector != params->ssector)
    dos_dir_cluster_reset(params);

  /* search for the requested entry                                   */
  while ( (entry = dos_dir_get_entry(params, entry_no)) && dos_namecmp(name, entry->name, entry->ext) )
    entry_no++;

  return entry;
}


/*
  D O S _ D I R _ G E T _ E N T R Y
*/
struct dos_dir_entry *dos_dir_get_entry(struct dosparam *params, unsigned long entry)
{
  char *buf = params->cbuf;

  if (entry < params->sentry)
    buf = dos_dir_cluster_reset(params);

  while ( entry >= (params->sentry + entries_per_cluster(params)) )
      if ( !(buf = dos_dir_cluster_read_nxt(params)) )
        return NULL;

  return (struct dos_dir_entry*)(buf + ( (entry - params->sentry) * sizeof(struct dos_dir_entry)) );
}


/*
  D O S _ R E A D _ C L U S T E R
*/
char *dos_read_cluster(struct dosparam *params, unsigned long ssector)
{
  int error;

  struct inode *inode = &params->inode;
  struct file  *filp  = &params->filp;
  struct request *request = &params->request;

  request->cmd = READ;
  request->sector = ssector;
  request->nr_sectors = params->sectors_per_cluster;
  request->buffer = params->cbuf;

  if ( (error = ata_request(inode, filp, request)) )
    return NULL;

  params->csector = ssector;

  return params->cbuf;
}


/*
  D O S _ D I R _ C L U S T E R _ R E A D _ N X T
*/
char *dos_dir_cluster_read_nxt(struct dosparam *params)
{
  char *p;
  unsigned long nxt_cluster_start;

  /* TODO: add FAT lookup */

  nxt_cluster_start = params->csector + params->sectors_per_cluster;

  if ( !(p = dos_read_cluster(params, nxt_cluster_start)) )
    return NULL;


  params->sentry += entries_per_cluster(params);

  return p;
}


/*
  D O S _ D I R _ C L U S T E R _ R E S E T
*/
char *dos_dir_cluster_reset(struct dosparam *params)
{
  params->sentry = 0;
  return dos_read_cluster(params, params->ssector);
}


