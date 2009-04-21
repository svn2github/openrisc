/*
    ata.c -- ATA driver
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

#include <string.h>

/* get prototype for 'printf'
   get IN_CLOCK (from 'board.h')
*/
#include "common.h"
#include "ata.h"


/*
  static constants (yuck, yuck, yuck)
*/
static int usage[2] = {0, 0};
static int hardsect_size[2] = {512, 512};
static int current_pio_mode = 9999;

/*
  local defines
*/
#define TYPE(devid) (devid >> 4)
#define REV(devid)  (devid & 0xf)

#define BASE(inode)    ( (unsigned long) (MAJOR(inode->i_rdev) << 24) )
#define DEVICE(inode)  ( MINOR(inode->i_rdev) )

#define IN_CLK_MHZ (IN_CLK / 1000000)
#define TO_TICKS(time) ( ((time + (IN_CLK_MHZ >> 1)) / IN_CLK_MHZ) -1 )


/*
       A T A _ O P E N

       opens ata device;
       - identifies ata-device-id
       - programs ata-host

       MAJOR: atahost identification (base address)
       MINOR: atadevice identification (MSB=0: device0, MSB=1: device1)
*/
int ata_open(struct inode *inode, struct file *filp)
{
  unsigned int atahost, device;
  unsigned short buf[256];
  unsigned short dev_pio;
  unsigned long sectors;

  char txt[41];

printf( "Checking device-id\n" );
  /* check device-id (MINOR number)                                   */
  if ( (device = DEVICE(inode)) > 1 )
      return EOPENIDEV;

printf( "Searching for hostcontroller: " );
  /* get atahost id                                                   */
  atahost = ata_ioctl(inode, filp, ATA_IOCTL_IDENTIFY_HOST, 0);
  if (atahost)
  {
    printf( "OCIDEC-%1d, revision 0x%02X found at 0x%08lX.\n", TYPE(atahost), REV(atahost), BASE(inode) );
  }
  else
  {
    printf( "No OCIDEC device found.\n" );
    return EOPENIHOST;
  }


  /* check if the host has been opened already                        */
  if (!usage[device]++)
  {
printf( "Opening device for the first time\n" );
      /* no, take device out of reset                                 */
      ata_ioctl(inode, filp, ATA_IOCTL_SET_RST, CLR | ARG_HW_RST);
      ata_ioctl(inode, filp, ATA_IOCTL_SET_RST, CLR | ARG_SW_RST);

      /* program PIO timing registers (set to PIO mode 0)             */
      ata_ioctl(inode, filp, ATA_IOCTL_SET_PIO, ARG_PIO0);

      /* enable host controller (for old OCIDEC cores)                */
      ata_ioctl(inode, filp, ATA_IOCTL_ENABLE_HOST, 0);
  }

  /* select requested device                                          */
  ata_ioctl(inode, filp, ATA_IOCTL_SELECT_DEVICE, 0);

printf( "Executing IdentifyDevice command\n" );
  /* identify requested ata-devices                                   */
  ata_ioctl(inode, filp, ATA_IOCTL_EXEC_CMD, IDENTIFY_DEVICE);

  /* read block from ata-device                                 */
  buf[0] = 256;
  ata_ioctl(inode, filp, ATA_IOCTL_READ, (unsigned long) buf);

  /* check if a valid device was found                                */
  /* bit 15 in identify_device word0 must be '0',
     or be equal to 0x848a (CFA feature set)                          */
  if ( (buf[0] & 0x8000) && (buf[0] != 0x848a) )
  {
    ata_release(inode, filp);
    return EOPENNODEV;
  }

  /* Then check specific configuration word                           */
  if (buf[0] == 0x848a)
    printf( "Found CompactFLASH device.\n" );
  else
    switch(buf[2]) {
       case 0x37c8:
           /* device does require SET_FEATURES
              IDENTIFY_DEVICE response is incomplete                  */
       case 0x738c:
           /* device does require SET_FEATURES
              IDENTIFY_DEVICE response is complete                    */
           ata_ioctl(inode, filp, ATA_IOCTL_SET_FEATURES, 0); //FIXME
           break;
       case 0x8c73:
           /* device does not require SET_FEATURES
              IDENTIFY_DEVICE response is incomplete                  */
       case 0xc837:
           /* device does not require SET_FEATURES
              IDENTIFY_DEVICE response is complete                    */
           break;

       default:
           ata_release(inode, filp);
           return EOPENNODEV;
     }

  /* checks ok                                                        */

  /* display device model, serialnumber, and firmware revision        */
  memcpy(txt, &buf[27], 40);
  txt[40] = '\0';
  printf( "Model       : %s\n", txt );

  memcpy(txt, &buf[10], 20);
  txt[20] = '\0';
  printf( "Serial      : %s\n", txt );

  memcpy( txt, &buf[23], 8);
  txt[8] = '\0';
  printf( "Firmware rev: %s\n", txt );

  /* display size in MBytes                                           */
  sectors = (buf[61] << 16) | buf[60];
  printf( "              %ld MBytes ", sectors >> 11 ); /* assuming 512bytes per sector */

  /* get supported pio modes                                          */
  dev_pio = buf[64];

  /* program ocidec timing registers to highest possible pio mode     */
  if (dev_pio & PIO4)
  {
      printf("PIO-4 supported\n");
      ata_ioctl(inode, filp, ATA_IOCTL_SET_PIO, ARG_PIO4);
  }
  else if (dev_pio & PIO3)
  {
      printf("PIO-3 supported\n");
      ata_ioctl(inode, filp, ATA_IOCTL_SET_PIO, ARG_PIO3);
  }
  else
  {
      printf("PIO-2 supported\n");
      ata_ioctl(inode, filp, ATA_IOCTL_SET_PIO, ARG_PIO2);
  }

  return 0;
}


/*
       A T A _ R E L E A S E

       closes ata device;
*/
int ata_release(struct inode *inode, struct file *filp)
{
  /* decrease usage count */
  if (!--usage[DEVICE(inode)])
  {
    /* reset atahost */
    ata_ioctl(inode, filp, ATA_IOCTL_SET_RST, SET | ARG_HW_RST);
  }

  return 0;
}


/*
       A T A _ R E A D _ D P O R T

       closes ata device;
*/
int ata_read_dport(unsigned long base)
{
  /* device ready to transfer data ?? */
  while ( !ata_dev_datrdy(base) );

  return REG32(base + ATA_DR);
}


/*
	A T A _ I O C T L
*/
int ata_ioctl(struct inode *inode, struct file *filp, unsigned command, unsigned long argument)
{
    unsigned long base = BASE(inode);

    switch (command)
    {
        case ATA_IOCTL_ENABLE_HOST:
	/* enables the ATA host controller (sets IDE_EN bit)          */
	{
	    REG32(base + ATA_CTRL) |= ATA_IDE_EN;
	    return 0;
	}


	case ATA_IOCTL_EXEC_CMD:
	/* sends a command to the ATA device                          */
	{
	  while( ata_dev_busy(base) ); // wait for device ready
	  ata_cmd(base) = argument & 0xff;
	  return 0;
	}


	case ATA_IOCTL_READ:
	{
          unsigned short *buf_ptr = (short *)argument;
	  int n, cnt;

	  cnt = buf_ptr[0];
	  for (n=0; n < cnt; n++)
	  {
	    while( !ata_dev_datrdy(base) );     // wait for data
            *buf_ptr++ = ata_read_dport(base);  // read data
	  }

	  return 0;
	}


	case ATA_IOCTL_IDENTIFY_HOST:
	/* reads OCIDEC status register configuration bits            */
	    return (REG32(base + ATA_STAT) >> 24);


        case ATA_IOCTL_SELECT_DEVICE:
        /* selects ATA device                                         */
	{
	  while( ata_dev_busy(base) ); // wait for device ready
	     
	  /* sets the DEV bit in the device/head register             */
          if ( DEVICE(inode) )
            REG32(base + ATA_DHR) |= ATA_DHR_DEV;  /* select device1  */
          else
            REG32(base + ATA_DHR) &= ~ATA_DHR_DEV; /* select device0  */

          return 0;
	}


        case ATA_IOCTL_SET_FTE:
	{
	  /* set FTE bits                                             */
	  if ( MINOR(inode->i_rdev) )
	    if (argument)
	        REG32(base + ATA_CTRL) |= ATA_IORDY_FTE0;
	      else
	        REG32(base + ATA_CTRL) &= ~ATA_IORDY_FTE0;
	  else
	    if (argument)
	        REG32(base + ATA_CTRL) |= ATA_IORDY_FTE1;
	      else
	        REG32(base + ATA_CTRL) &= ~ATA_IORDY_FTE1;

	  return 0;
	}


	case ATA_IOCTL_SET_PIO:
        /* programs the PIO timing registers                          */
	{
	  unsigned long timing;
	  unsigned short buf[256];

	  /* do we need to read the identify_device block ??          */
          if (argument & (ARG_PIO4 | ARG_PIO3) )
	  {
            /* identify requested ata-devices                         */
            ata_ioctl(inode, filp, ATA_IOCTL_EXEC_CMD, IDENTIFY_DEVICE);

            /* read block from ata-device                             */
            buf[0] = 256;
            ata_ioctl(inode, filp, ATA_IOCTL_READ, (unsigned long) buf);
	  }

	  /* program register transfer timing registers               */
	  /* set the slowest pio mode for register transfers          */
	  if (argument < current_pio_mode)
	  {
            switch (argument)
	    {
	      case ARG_PIO4:
		if ( (buf[53] & 0x01) && buf[68] )
	          timing = ata_calc_pio_timing(buf[68], PIO4_RT1, PIO4_RT2, PIO4_RT4, PIO4_RT2I, PIO4_RT9);
		else
	          timing = ata_calc_pio_timing(PIO4_RT0, PIO4_RT1, PIO4_RT2, PIO4_RT4, PIO4_RT2I, PIO4_RT9);
	        break;

	      case ARG_PIO3:
		if ( (buf[53] & 0x01) && buf[68] )
	          timing = ata_calc_pio_timing(buf[68], PIO3_RT1, PIO3_RT2, PIO3_RT4, PIO3_RT2I, PIO3_RT9);
		else
	          timing = ata_calc_pio_timing(PIO3_RT0, PIO3_RT1, PIO3_RT2, PIO3_RT4, PIO3_RT2I, PIO3_RT9);
	        break;

	      case ARG_PIO2:
	        timing = ata_calc_pio_timing(PIO2_RT0, PIO2_RT1, PIO2_RT2, PIO2_RT4, PIO2_RT2I, PIO2_RT9);
	        break;

	      case ARG_PIO1:
	        timing = ata_calc_pio_timing(PIO1_RT0, PIO1_RT1, PIO1_RT2, PIO1_RT4, PIO1_RT2I, PIO1_RT9);
	        break;

	      default: /* PIO mode 0 */
	        timing = ata_calc_pio_timing(PIO0_RT0, PIO0_RT1, PIO0_RT2, PIO0_RT4, PIO0_RT2I, PIO0_RT9);
	        break;
	    }

            /* program IORDY bit                                      */
	    if (argument & (ARG_PIO4 | ARG_PIO3) )
	        REG32(base + ATA_CTRL) |= ATA_IORDY;
	    else
	        REG32(base + ATA_CTRL) &= ~ATA_IORDY;

	    /* program register transfer timing registers             */
            REG32(base + ATA_PCTR) = timing;

	    current_pio_mode = argument;
	  }

	  /* Program data transfer timing registers                   */
	  if ( TYPE(ata_ioctl(inode, filp, ATA_IOCTL_IDENTIFY_HOST, 0)) > 1 )
	  {
	    switch (argument)
	    {
	      case ARG_PIO4:
		if ( (buf[53] & 0x01) && buf[68] )
	          timing = ata_calc_pio_timing(buf[68], PIO4_DT1, PIO4_DT2, PIO4_DT4, PIO4_DT2I, PIO4_DT9);
		else
	          timing = ata_calc_pio_timing(PIO4_DT0, PIO4_DT1, PIO4_DT2, PIO4_DT4, PIO4_DT2I, PIO4_DT9);
	        break;

	      case ARG_PIO3:
		if ( (buf[53] & 0x01) && buf[68] )
	          timing = ata_calc_pio_timing(buf[68], PIO3_DT1, PIO3_DT2, PIO3_DT4, PIO3_DT2I, PIO3_DT9);
		else
	          timing = ata_calc_pio_timing(PIO3_DT0, PIO3_DT1, PIO3_DT2, PIO3_DT4, PIO3_DT2I, PIO3_DT9);
	        break;

	      case ARG_PIO2:
	        timing = ata_calc_pio_timing(PIO2_DT0, PIO2_DT1, PIO2_DT2, PIO2_DT4, PIO2_DT2I, PIO2_DT9);
	        break;

	      case ARG_PIO1:
	        timing = ata_calc_pio_timing(PIO1_DT0, PIO1_DT1, PIO1_DT2, PIO1_DT4, PIO1_DT2I, PIO1_DT9);
	        break;

	      default: /* PIO mode 0 */
	        timing = ata_calc_pio_timing(PIO0_DT0, PIO0_DT1, PIO0_DT2, PIO0_DT4, PIO0_DT2I, PIO0_DT9);
	        break;
	    }

	    /* program data transfer timing registers, set IORDY bit  */
	    if ( MINOR(inode->i_rdev) )
	    {
	      REG32(base + ATA_PFTR1) = timing;

	      /* program IORDY bit                                    */
	      if ( argument & (ARG_PIO4 | ARG_PIO3) )
	        REG32(base + ATA_CTRL) |= ATA_IORDY_FTE1;
	      else
	        REG32(base + ATA_CTRL) &= ~ATA_IORDY_FTE1;
	    }
	    else
	    {
	      REG32(base + ATA_PFTR0) = timing;

	      /* program IORDY bit                                    */
	      if ( argument & (ARG_PIO4 | ARG_PIO3) )
	        REG32(base + ATA_CTRL) |= ATA_IORDY_FTE1;
	      else
	        REG32(base + ATA_CTRL) &= ~ATA_IORDY_FTE1;
	    }

	    /* enable data transfer timing                            */
	    ata_ioctl(inode, filp, ATA_IOCTL_SET_FTE, 1);
	  }

	  return 0;
        }


        case ATA_IOCTL_SET_RST:
        /* sets or clears reset bits                                  */
	  if (argument & SET)
            switch (argument & ~SET) {
                case ARG_HW_RST:  /* ata hardware reset               */
                    REG32(base + ATA_CTRL) |= ATA_RST;
                    return 0;

                case ARG_SW_RST:  /* ata software reset               */
                    REG32(base + ATA_DCR) |= ATA_DCR_RST;
                    return 0;

                case ARG_DEV_RST: /* ata device reset                 */
                    REG32(base + ATA_CR) = DEVICE_RESET;
                    return 0;

                default:
                    return EIOCTLIARG;
            }
	  else
	  {
	    switch(argument & ~SET) {
                case ARG_HW_RST:  /* ata hardware reset               */
                    REG32(base + ATA_CTRL) &= ~ATA_RST;
                    return 0;

                case ARG_SW_RST:  /* ata software reset               */
                    REG32(base + ATA_DCR) &= ~ATA_DCR_RST;
                    return 0;

                case ARG_DEV_RST: /* ata device reset                 */
                    return 0;

                default:
                    return EIOCTLIARG;
            }
	  }


        default:
	    printf( "FIXME: Unsupported IOCTL call %1d\n", command );
	    return EINVAL;
    }
}


unsigned long ata_calc_pio_timing(short t0, short t1, short t2, short t4, short t2i, short t9)
{
    int teoc;

    teoc = t0 - t1 - t2;

    if (teoc < 0)
      teoc = 0;

    if (t9 > teoc)
      teoc = t9;

    if (t2i > teoc)
      teoc = t2i;

    t1   = TO_TICKS(t1);
    t2   = TO_TICKS(t2);
    t4   = TO_TICKS(t4);
    teoc = TO_TICKS(teoc);

    return (teoc << ATA_TEOC) | (t4 << ATA_T4) | (t2 << ATA_T2) | (t1 << ATA_T1);
}


/*
	A T A _ R E Q U E S T
*/
int ata_request(struct inode *inode, struct file *filp, struct request *request)
{
  unsigned int device = MINOR(inode->i_rdev);
  unsigned long base = BASE(inode);
  unsigned int sector_size;

  /* get the sector size for the current device                       */
  sector_size = hardsect_size[device] >> 1;

  /* set the device                                                   */
  ata_ioctl(inode, filp, ATA_IOCTL_SELECT_DEVICE, device);

  /* get the base address                                             */
  base = BASE(inode);

  switch (request->cmd) {
    case READ:
        {
            register unsigned long i, n;
            register unsigned short rd_data;
            register char *buf_ptr;

            /* program ata-device registers                           */
            REG32(base + ATA_DHR) = ((request->sector >> 24) & ATA_DHR_H) | ATA_DHR_LBA;
            REG32(base + ATA_CHR) = (request->sector >> 16) & 0xff;
            REG32(base + ATA_CLR) = (request->sector >> 8) & 0xff;
            REG32(base + ATA_SNR) = request->sector & 0xff;

            REG32(base + ATA_SCR) = request->nr_sectors;

            /* send 'read_sector(s)' command                          */
            REG32(base + ATA_CR) = READ_SECTORS;

            /* check status & error registers                         */
            if ( ata_astatus(base) & ATA_SR_ERR)
                return -1;

            /* read data from devices and store it in the buffer      */
            buf_ptr = request->buffer;

	    for (n=0; n < request->nr_sectors; n++)
            for (i=0; i < sector_size; i++)
	    {
	        rd_data = ata_read_dport(base);

                /* the data is byte reversed, swap high & low bytes   */
                *buf_ptr++ = rd_data;
                *buf_ptr++ = rd_data >> 8;
            }

	    return 0;
    }


    case WRITE:
        /* check if device may be written to                          */
	if (filp->f_mode & FMODE_WRITE)
	{
	  /* do the write stuff */
	}
	else
	  return EINVAL;


    default:
       return EINVAL;
  }

  return 0;
}
