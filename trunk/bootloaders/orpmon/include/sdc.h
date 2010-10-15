#ifndef _SDC_H
#define _SDC_H
#include "board.h"
#include "typedefs.h"

#include "spr-defs.h"
#include "support.h"
//#include <stdlib.h>


#define REG32(add) *((volatile unsigned long *)(add))
#define DBGA(stuff...)	//	printf("SDC_HAL:" stuff)
/* Memory card sector size */
#define SECTOR_SIZE 512

#define SECTOR_TO_BLOCK(a) (a << 9)

//SD_CONTROLLER Register
#define WORD_0 0x00
#define WORD_1 0x40
#define WORD_2 0x80
#define WORD_3 0xC0


#define SD_ARG 0x00
#define SD_COMMAND 0x04 
#define SD_STATUS 0x08
#define SD_RESP1 0x0c

#define SD_CTRL 0x1c
#define SD_BLOCK 0x20
#define SD_POWER 0x24
#define SD_SOFTWARE_RST 0x28
#define SD_TIMEOUT 0x2c
#define SD_NORMAL_INT_STATUS 0x30
#define SD_ERROR_INT_STATUS 0x34
#define SD_NORMAL_INT_STATUS_ENABLE 0x38
#define SD_ERROR_INT_STATUS_ENABLE 0x3c
#define SD_NOMAL_INT_SIGNAL_ENABLE  0x40
#define SD_ERROR_INT_SIGNAL_ENABLE  0x44
#define SD_CAPABILITY  0x48
#define SD_CLOCK_D  0x4c
#define BD_STATUS 0x50
#define BD_ISR 0x54
#define BD_RX 0x60
#define BD_TX 0x80

#define CLK_CARD 25000000
#define CLK_CPU 25000000
#define CMD_TIMEOUT_MS ((CLK_CPU/CLK_CARD) * 512)
#define MAX_POL 1000
#define SD_REG(REG)  REG32(SDC_CONTROLLER_BASE+REG) 


//Commands
#define CMD0 0x000 // software reset
#define CMD2 0x200
#define CMD3 0x300
#define CMD7 0x700
#define CMD8  0x800
#define CMD9  0x900  // ask card to send card speficic data (CSD)
#define CMD16  0x1000 	// sets the block length used by the memory card
#define CMD17  0x1100 // read single block

#define ACMD41 0x2900
#define ACMD6 0x600
#define CMD55 0x3700
 
//CMD ARG
//CMD8
#define VHS  0x100 //2.7-3.6V
#define CHECK_PATTERN 0xAA
//ACMD41
#define BUSY 0x80000000
#define HCS 0x40000000
#define VOLTAGE_MASK 0xFFFFFF

//CMD7
#define READY_FOR_DATA 0x100
#define CARD_STATUS_STB  0x600

//Command setting
#define CICE 0x10
#define CRCE 0x08
#define RSP_48 0x2
#define RSP_146 0x1

//Status Mask
//Normal interupt status
#define CMD_COMPLETE 0x1
#define EI 0x8000

//Error interupt status
#define CMD_TIMEOUT 0x1
#define CCRC 0x1
#define CIE  0x8

#define CID_MID_MASK 0x7F8000
#define CID_OID_MASK 0x7FFF		
#define CID_B1 0x7F800000
#define CID_B2 0x7F8000
#define CID_B3 0x7F80
#define CID_B4 0x7F

#define RCA_RCA_MASK 0xFFFF0000

#define WRITE_OP  1
#define READ_OP  0


typedef struct {
	unsigned long int pad:18;
	unsigned  int cmdi:6;
	unsigned  int cmdt:2;
	unsigned  int dps:1;
	unsigned  int cice:1;
	unsigned  int crce:1;
	unsigned  int  rsvd:1;
	unsigned  int rts:2;
}sd_controller_csr ;



typedef struct {
	char mid:8;
	short oid:16;
	unsigned char pnm[5];
	char  prv:8;
	int psn:32;
	char rsv:4;
	short mdt:12;
}sd_card_cid;

typedef struct {
}sd_card_csd;

typedef struct  {
	int rca;
	int Voltage_window;
	char HCS_s;
    char Active;
	char phys_spec_2_0;
	 int  cid_reg;
	sd_card_csd * csd_reg;

}sd_card ;

extern volatile sd_card dev;

unsigned char sd_wait_rsp(void);
int setup_bd_transfer(boolean direction, int block_addr, volatile unsigned char *buff);
void reset_card(void);
int finnish_bd_transfer(void);
int sd_setup_transfer (sd_card sd_card_0);
int setBLockLength(void);

int memCardInit(void);

#endif // _SDC_H
        