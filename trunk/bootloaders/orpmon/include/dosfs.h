/*
	DOSFS Embedded FAT-Compatible Filesystem
	(C) 2005 Lewin A.R.W. Edwards (sysadm@zws.com)
*/

#ifndef _DOSFS_H
#define _DOSFS_H
//#include <stdio.h>
//#include <sys/types.h>
//#include <stdint.h>
#include "board.h"
#include "typedefs.h"

#include "spr-defs.h"
#include "support.h"
//#include <stdlib.h>

#include "int.h"
#include "uart.h"
#include "screen.h"
//===================================================================
char *strcpy2(char *dest,  char* src);
int memcmp2(const void *s1, const void *s2, size_t  n);
int strcmp2(const char * s1, const char * s2);

 typedef struct
        {
           long int quot;
           long int rem;   
        } ldiv_t;

 typedef struct
        {
          int quot;
         int rem;   
        } div_t;
        

#define SECTOR_TO_BLOCK(a) ( (a) << 9)
unsigned long int DFS_ReadSector(unsigned char unit, unsigned char *buffer, unsigned long int sector, unsigned long int  count);
unsigned long int DFS_WriteSector(unsigned char unit, unsigned char *buffer, unsigned long int sector, unsigned long int  count);


 ldiv_t ldiv (long int numer, long int denom);
div_t div ( int numer,  int denom);

//===================================================================
// Configurable items
#define MAX_PATH		64		// Maximum path length (increasing this will
								// GREATLY increase stack requirements!)
#define DIR_SEPARATOR	'/'		// character separating directory components

// End of configurable items
//===================================================================

//===================================================================
// 32-bit error codes
#define DFS_OK			0			// no error
#define DFS_EOF			1			// end of file (not an error)
#define DFS_WRITEPROT	2			// volume is write protected
#define DFS_NOTFOUND	3			// path or file not found
#define DFS_PATHLEN		4			// path too long
#define DFS_ALLOCNEW	5			// must allocate new directory cluster
#define DFS_ERRMISC		0xffffffff	// generic error

//===================================================================
// File access modes
#define DFS_READ		1			// read-only
#define DFS_WRITE		2			// write-only

//===================================================================
// Miscellaneous constants
#define SECTOR_SIZE		512		// sector size in bytes

//===================================================================
// Internal subformat identifiers
#define FAT12			0
#define FAT16			1
#define FAT32			2

//===================================================================
// DOS attribute bits
#define ATTR_READ_ONLY	0x01
#define ATTR_HIDDEN		0x02
#define ATTR_SYSTEM		0x04
#define ATTR_VOLUME_ID	0x08
#define ATTR_DIRECTORY	0x10
#define ATTR_ARCHIVE	0x20
#define ATTR_LONG_NAME	(ATTR_READ_ONLY | ATTR_HIDDEN | ATTR_SYSTEM | ATTR_VOLUME_ID)


/*
	Directory entry structure
	note: if name[0] == 0xe5, this is a free dir entry
	      if name[0] == 0x00, this is a free entry and all subsequent entries are free
		  if name[0] == 0x05, the first character of the name is 0xe5 [a kanji nicety]

	Date format: bit 0-4  = day of month (1-31)
	             bit 5-8  = month, 1=Jan..12=Dec
				 bit 9-15 =	count of years since 1980 (0-127)
	Time format: bit 0-4  = 2-second count, (0-29)
	             bit 5-10 = minutes (0-59)
				 bit 11-15= hours (0-23)
*/
typedef struct _tagDIRENT {
	unsigned char name[11];			// filename
	unsigned char attr;				// attributes (see ATTR_* constant definitions)
	unsigned char reserved;			// reserved, must be 0
	unsigned char crttimetenth;		// create time, 10ths of a second (0-199 are valid)
	unsigned char crttime_l;			// creation time low byte
	unsigned char crttime_h;			// creation time high byte
	unsigned char crtdate_l;			// creation date low byte
	unsigned char crtdate_h;			// creation date high byte
	unsigned char lstaccdate_l;		// last access date low byte
	unsigned char lstaccdate_h;		// last access date high byte
	unsigned char startclus_h_l;		// high word of first cluster, low byte (FAT32)
	unsigned char startclus_h_h;		// high word of first cluster, high byte (FAT32)
	unsigned char wrttime_l;			// last write time low byte
	unsigned char wrttime_h;			// last write time high byte
	unsigned char wrtdate_l;			// last write date low byte
	unsigned char wrtdate_h;			// last write date high byte
	unsigned char startclus_l_l;		// low word of first cluster, low byte
	unsigned char startclus_l_h;		// low word of first cluster, high byte
	unsigned char filesize_0;			// file size, low byte
	unsigned char filesize_1;			//
	unsigned char filesize_2;			//
	unsigned char filesize_3;			// file size, high byte
} DIRENT, *PDIRENT;

/*
	Partition table entry structure
*/
typedef struct _tagPTINFO {
	unsigned char		active;			// 0x80 if partition active
	unsigned char		start_h;		// starting head
	unsigned char		start_cs_l;		// starting cylinder and sector (low byte)
	unsigned char		start_cs_h;		// starting cylinder and sector (high byte)
	unsigned char		type;			// type ID byte
	unsigned char		end_h;			// ending head
	unsigned char		end_cs_l;		// ending cylinder and sector (low byte)
	unsigned char		end_cs_h;		// ending cylinder and sector (high byte)
	unsigned char		start_0;		// starting sector# (low byte)
	unsigned char		start_1;		//
	unsigned char		start_2;		//
	unsigned char		start_3;		// starting sector# (high byte)
	unsigned char		size_0;			// size of partition (low byte)
	unsigned char		size_1;			//
	unsigned char		size_2;			//
	unsigned char		size_3;			// size of partition (high byte)
} PTINFO, *PPTINFO;

/*
	Master Boot Record structure
*/
typedef struct _tagMBR {
	unsigned char bootcode[0x1be];	// boot sector
	PTINFO ptable[4] __attribute__((__packed__));
;			// four partition table structures
	unsigned char sig_55;				// 0x55 signature byte
	unsigned char sig_aa;				// 0xaa signature byte
} MBR, *PMBR;

/*
	BIOS Parameter Block structure (FAT12/16)
*/
typedef struct _tagBPB {
	unsigned char bytepersec_l;		// bytes per sector low byte (0x00)
	unsigned char bytepersec_h;		// bytes per sector high byte (0x02)
	unsigned char	secperclus;			// sectors per cluster (1,2,4,8,16,32,64,128 are valid)
	unsigned char reserved_l;			// reserved sectors low byte
	unsigned char reserved_h;			// reserved sectors high byte
	unsigned char numfats;			// number of FAT copies (2)
	unsigned char rootentries_l;		// number of root dir entries low byte (0x00 normally)
	unsigned char rootentries_h;		// number of root dir entries high byte (0x02 normally)
	unsigned char sectors_s_l;		// small num sectors low byte
	unsigned char sectors_s_h;		// small num sectors high byte
	unsigned char mediatype;			// media descriptor byte
	unsigned char secperfat_l;		// sectors per FAT low byte
	unsigned char secperfat_h;		// sectors per FAT high byte
	unsigned char secpertrk_l;		// sectors per track low byte
	unsigned char secpertrk_h;		// sectors per track high byte
	unsigned char heads_l;			// heads low byte
	unsigned char heads_h;			// heads high byte
	unsigned char hidden_0;			// hidden sectors low byte
	unsigned char hidden_1;			// (note - this is the number of MEDIA sectors before
	unsigned char hidden_2;			// first sector of VOLUME - we rely on the MBR instead)
	unsigned char hidden_3;			// hidden sectors high byte
	unsigned char sectors_l_0;		// large num sectors low byte
	unsigned char sectors_l_1;		//
	unsigned char sectors_l_2;		//
	unsigned char sectors_l_3;		// large num sectors high byte
} BPB, *PBPB;




/*
	Extended BIOS Parameter Block structure (FAT12/16)
*/
typedef struct _tagEBPB {
	unsigned char unit;				// int 13h drive#
	unsigned char head;				// archaic, used by Windows NT-class OSes for flags
	unsigned char signature;			// 0x28 or 0x29
	unsigned char serial_0;			// serial#
	unsigned char serial_1;			// serial#
	unsigned char serial_2;			// serial#
	unsigned char serial_3;			// serial#
	unsigned char label[11];			// volume label
	unsigned char system[8];			// filesystem ID
} EBPB, *PEBPB;

/*
	Extended BIOS Parameter Block structure (FAT32)
*/
typedef struct _tagEBPB32 {
	unsigned char fatsize_0;			// big FAT size in sectors low byte
	unsigned char fatsize_1;			//
	unsigned char fatsize_2;			//
	unsigned char fatsize_3;			// big FAT size in sectors high byte
	unsigned char extflags_l;			// extended flags low byte
	unsigned char extflags_h;			// extended flags high byte
	unsigned char fsver_l;			// filesystem version (0x00) low byte
	unsigned char fsver_h;			// filesystem version (0x00) high byte
	unsigned char root_0;				// cluster of root dir, low byte
	unsigned char root_1;				//
	unsigned char root_2;				//
	unsigned char root_3;				// cluster of root dir, high byte
	unsigned char fsinfo_l;			// sector pointer to FSINFO within reserved area, low byte (2)
	unsigned char fsinfo_h;			// sector pointer to FSINFO within reserved area, high byte (0)
	unsigned char bkboot_l;			// sector pointer to backup boot sector within reserved area, low byte (6)
	unsigned char bkboot_h;			// sector pointer to backup boot sector within reserved area, high byte (0)
	unsigned char reserved[12];		// reserved, should be 0

	unsigned char unit;				// int 13h drive#
	unsigned char head;				// archaic, used by Windows NT-class OSes for flags
	unsigned char signature;			// 0x28 or 0x29
	unsigned char serial_0;			// serial#
	unsigned char serial_1;			// serial#
	unsigned char serial_2;			// serial#
	unsigned char serial_3;			// serial#
	unsigned char label[11];			// volume label
	unsigned char system[8];			// filesystem ID
} EBPB32, *PEBPB32;

/*
	Logical Boot Record structure (volume boot sector)
*/
typedef struct _tagLBR {
	unsigned char jump[3];			// JMP instruction
	unsigned char oemid[8];			// OEM ID, space-padded
	BPB bpb  __attribute__((__packed__));					// BIOS Parameter Block
	union {
		EBPB ebpb  __attribute__((__packed__));				// FAT12/16 Extended BIOS Parameter Block
		EBPB32 ebpb32  __attribute__((__packed__));			// FAT32 Extended BIOS Parameter Block
	} ebpb;
	unsigned char code[420];			// boot sector code
	unsigned char sig_55;				// 0x55 signature byte
	unsigned char sig_aa;				// 0xaa signature byte
} LBR, *PLBR;

/*
	Volume information structure (Internal to DOSFS)
*/
typedef struct _tagVOLINFO {
	unsigned char unit;				// unit on which this volume resides
	unsigned char filesystem;			// formatted filesystem

// These two fields aren't very useful, so support for them has been commented out to
// save memory. (Note that the "system" tag is not actually used by DOS to determine
// filesystem type - that decision is made entirely on the basis of how many clusters
// the drive contains. DOSFS works the same way).
// See tag: OEMID in dosfs.c
//	unsigned char oemid[9];			// OEM ID ASCIIZ
//	unsigned char system[9];			// system ID ASCIIZ
	unsigned char label[12];			// volume label ASCIIZ
	unsigned long int  startsector;		// starting sector of filesystem
	unsigned char secperclus;			// sectors per cluster
	unsigned short int reservedsecs;		// reserved sectors
	unsigned long int numsecs;			// number of sectors in volume
	unsigned long int secperfat;			// sectors per FAT
	unsigned short int rootentries;		// number of root dir entries

	unsigned long int numclusters;		// number of clusters on drive

	// The fields below are PHYSICAL SECTOR NUMBERS.
	unsigned long int fat1;				// starting sector# of FAT copy 1
	unsigned long int rootdir;			// starting sector# of root directory (FAT12/FAT16) or cluster (FAT32)
	unsigned long int dataarea;			// starting sector# of data area (cluster #2)
} VOLINFO, *PVOLINFO;

/*
	Flags in DIRINFO.flags
*/
#define DFS_DI_BLANKENT		0x01	// Searching for blank entry

/*
	Directory search structure (Internal to DOSFS)
*/
typedef struct _tagDIRINFO {
	unsigned long int currentcluster;	// current cluster in dir
	unsigned char currentsector;		// current sector in cluster
	unsigned char currententry;		// current dir entry in sector
	unsigned char *scratch;			// ptr to user-supplied scratch buffer (one sector)
	unsigned char flags;				// internal DOSFS flags
} DIRINFO, *PDIRINFO;

/*
	File handle structure (Internal to DOSFS)
*/
typedef struct _tagFILEINFO {
	PVOLINFO volinfo  __attribute__((__packed__));			// VOLINFO used to open this file
	unsigned long int dirsector;			// physical sector containing dir entry of this file
	unsigned char diroffset;			// # of this entry within the dir sector
	unsigned char mode;				// mode in which this file was opened
	unsigned long int firstcluster;		// first cluster of file
	unsigned long int filelen;			// byte length of file

	unsigned long int cluster;			// current cluster
	unsigned long int pointer;			// current (BYTE) pointer
} FILEINFO, *PFILEINFO;

/*
	Get starting sector# of specified partition on drive #unit
	NOTE: This code ASSUMES an MBR on the disk.
	scratchsector should point to a SECTOR_SIZE scratch area
	Returns 0xffffffff for any error.
	If pactive is non-NULL, this function also returns the partition active flag.
	If pptype is non-NULL, this function also returns the partition type.
	If psize is non-NULL, this function also returns the partition size.
*/
unsigned long int init_fat(VOLINFO *vis);

unsigned long int DFS_GetPtnStart(unsigned char unit, unsigned char *scratchsector, unsigned char pnum, unsigned char *pactive, unsigned char *pptype, unsigned long int *psize);

/*
	Retrieve volume info from BPB and store it in a VOLINFO structure
	You must provide the unit and starting sector of the filesystem, and
	a pointer to a sector buffer for scratch
	Attempts to read BPB and glean information about the FS from that.
	Returns 0 OK, nonzero for any error.
*/
unsigned long int DFS_GetVolInfo(unsigned char unit, unsigned char *scratchsector, unsigned long int startsector, PVOLINFO volinfo);

/*
	Open a directory for enumeration by DFS_GetNextDirEnt
	You must supply a populated VOLINFO (see DFS_GetVolInfo)
	The empty string or a string containing only the directory separator are
	considered to be the root directory.
	Returns 0 OK, nonzero for any error.
*/
unsigned long int DFS_OpenDir(PVOLINFO volinfo, unsigned char *dirname, PDIRINFO dirinfo);

/*
	Get next entry in opened directory structure. Copies fields into the dirent
	structure, updates dirinfo. Note that it is the _caller's_ responsibility to
	handle the '.' and '..' entries.
	A deleted file will be returned as a NULL entry (first char of filename=0)
	by this code. Filenames beginning with 0x05 will be translated to 0xE5
	automatically. Long file name entries will be returned as NULL.
	returns DFS_EOF if there are no more entries, DFS_OK if this entry is valid,
	or DFS_ERRMISC for a media error
*/
unsigned long int DFS_GetNext(PVOLINFO volinfo, PDIRINFO dirinfo, PDIRENT dirent);

/*
	Open a file for reading or writing. You supply populated VOLINFO, a path to the file,
	mode (DFS_READ or DFS_WRITE) and an empty fileinfo structure. You also need to
	provide a pointer to a sector-sized scratch buffer.
	Returns various DFS_* error states. If the result is DFS_OK, fileinfo can be used
	to access the file from this point on.
*/
unsigned long int DFS_OpenFile(PVOLINFO volinfo, unsigned char *path, unsigned char mode, unsigned char *scratch, PFILEINFO fileinfo);

/*
	Read an open file
	You must supply a prepopulated FILEINFO as provided by DFS_OpenFile, and a
	pointer to a SECTOR_SIZE scratch buffer.
	Note that returning DFS_EOF is not an error condition. This function updates the
	successcount field with the number of bytes actually read.
*/
unsigned long int DFS_ReadFile(PFILEINFO fileinfo, unsigned char *scratch, unsigned char *buffer, unsigned long int *successcount, unsigned long int len);

/*
	Write an open file
	You must supply a prepopulated FILEINFO as provided by DFS_OpenFile, and a
	pointer to a SECTOR_SIZE scratch buffer.
	This function updates the successcount field with the number of bytes actually written.
*/
unsigned long int DFS_WriteFile(PFILEINFO fileinfo, unsigned char *scratch, unsigned char *buffer, unsigned long int *successcount, unsigned long int len);

/*
	Seek file pointer to a given position
	This function does not return status - refer to the fileinfo->pointer value
	to see where the pointer wound up.
	Requires a SECTOR_SIZE scratch buffer
*/
void DFS_Seek(PFILEINFO fileinfo, unsigned long int offset, unsigned char *scratch);

/*
	Delete a file
	scratch must point to a sector-sized buffer
*/
unsigned long int DFS_UnlinkFile(PVOLINFO volinfo, unsigned char *path, unsigned char *scratch);

// If we are building a host-emulation version, include host support
#ifdef HOSTVER
#include "hostemu.h"
#endif

#endif // _DOSFS_H
