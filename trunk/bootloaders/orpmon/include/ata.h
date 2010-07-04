/*
/////////////////////////////////////////////////////////////////////
////                                                             ////
////  Include file for OpenCores ATA Controller (OCIDEC)         ////
////                                                             ////
////  File    : ata.h                                            ////
////  Function: c-include file                                   ////
////                                                             ////
////  Authors: Richard Herveille (rherveille@opencores)          ////
////           www.opencores.org                                 ////
////                                                             ////
/////////////////////////////////////////////////////////////////////
////                                                             ////
//// Copyright (C) 2002 Richard Herveille                        ////
////                    rherveille@opencores.org                 ////
////                                                             ////
//// This source file may be used and distributed without        ////
//// restriction provided that this copyright statement is not   ////
//// removed from the file and that any derivative work contains ////
//// the original copyright notice and the associated disclaimer.////
////                                                             ////
////     THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY     ////
//// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED   ////
//// TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS   ////
//// FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL THE AUTHOR      ////
//// OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,         ////
//// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES    ////
//// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE   ////
//// GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR        ////
//// BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF  ////
//// LIABILITY, WHETHER IN  CONTRACT, STRICT LIABILITY, OR TORT  ////
//// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT  ////
//// OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE         ////
//// POSSIBILITY OF SUCH DAMAGE.                                 ////
////                                                             ////
/////////////////////////////////////////////////////////////////////
*/

/*
 * Definitions for the Opencores ATA Controller Core
 */

#ifndef __OC_ATA_H
#define __OC_ATA_H


#ifndef REG32
#define REG32(adr) *((volatile unsigned long *)(adr))
#endif


typedef unsigned long basetype;

/* --- Register definitions --- */

/* ----- Core Registers                                              */
#define ATA_CTRL  0x00         /* Control register                   */
#define ATA_STAT  0x04         /* Status register                    */
#define ATA_PCTR  0x08         /* PIO command timing register        */
#define ATA_PFTR0 0x0c         /* PIO Fast Timing register Device0   */
#define ATA_PFTR1 0x10         /* PIO Fast Timing register Device1   */
#define ATA_DTR0  0x14         /* DMA Timing register Device2        */
#define ATA_DTR1  0x18         /* DMA Timing register Device1        */
#define ATA_DTXB  0x3c         /* DMA Transmit buffer                */
#define ATA_RXB   0x3c         /* DMA Receive buffer                 */

/* ----- ATA Registers                                               */
#define ATA_ASR   0x78         /* Alternate Status Register      (R) */
#define ATA_CR    0x5c         /* Command Register               (W) */
#define ATA_CHR   0x54         /* Cylinder High Register       (R/W) */
#define ATA_CLR   0x50         /* Cylinder Low Register        (R/W) */
#define ATA_DR    0x40         /* Data Register                      */
#define ATA_DCR   0x78         /* Device Control Register        (W) */
#define ATA_DHR   0x58         /* Device/Head Register         (R/W) */
#define ATA_ERR   0x44         /* Error Register                 (R) */
#define ATA_FR    0x44         /* Features Register              (W) */
#define ATA_SCR   0x48         /* Sector Count Register        (R/W) */
#define ATA_SNR   0x4c         /* Sector Number Register       (R/W) */
#define ATA_SR    0x5c         /* Status Register                (R) */
#define ATA_DA    0x7c         /* Device Address Register        (R) */
             /* ATA/ATAPI-5 does not describe Device Status Register */

/* ----------------------------                                       */
/* ----- Bits definitions -----                                       */
/* ----------------------------                                       */

/* ----- Core Control register                                        */
                                /* bits 31-16 are reserved            */
#define ATA_DMA_EN  (0<<15)     /* DMAen, DMA enable bit              */
                                /* bit 14 is reserved                 */
#define ATA_DMA_WR  (1<<14)     /* DMA Write transaction              */
#define ATA_DMA_RD  (0<<14)     /* DMA Read transaction               */
                                /* bits 13-10 are reserved            */
#define ATA_BELEC1  (1<< 9)     /* Big-Little endian conversion       */
                                /* enable bit for Device1             */
#define ATA_BELEC0  (1<< 8)     /* Big-Little endian conversion       */
                                /* enable bit for Device0             */
#define ATA_IDE_EN  (1<< 7)     /* IDE core enable bit                */
#define ATA_FTE1    (1<< 6)     /* Device1 Fast PIO Timing Enable bit */
#define ATA_FTE0    (1<< 5)     /* Device0 Fast PIO Timing Enable bit */
#define ATA_PWPP    (1<< 4)     /* PIO Write Ping-Pong Enable bit     */
#define ATA_IORDY_FTE1 (1<< 3)  /* Device1 Fast PIO Timing IORDY      */
                                /* enable bit                         */
#define ATA_IORDY_FTE0 (1<< 2)  /* Device0 Fast PIO Timing IORDY      */
                                /* enable bit                         */
#define ATA_IORDY   (1<< 1)     /* PIO Command Timing IORDY enable bit*/
#define ATA_RST     (1<< 0)     /* ATA Reset bit                      */

/* ----- Core Status register                                         */
#define ATA_DEVID   0xf0000000  /* bits 31-28 Device-ID               */
#define ATA_REVNO   0x0f000000  /* bits 27-24 Revision number         */
                                /* bits 23-16 are reserved            */
#define ATA_DMA_TIP (1<<15)     /* DMA Transfer in progress           */
                                /* bits 14-10 are reserved            */
#define ATA_DRBE    (1<<10)     /* DMA Receive buffer empty           */
#define ATA_DTBF    (1<< 9)     /* DMA Transmit buffer full           */
#define ATA_DMARQ   (1<< 8)     /* DMARQ Line status                  */
#define ATA_PIO_TIP (1<< 7      /* PIO Transfer in progress           */
#define ATA_PWPPF   (1<< 6)     /* PIO write ping-pong full           */
                                /* bits 5-1 are reserved              */
#define ATA_IDEIS  (1<< 0)      /* IDE Interrupt status               */


/* -----  Core Timing registers                                       */
#define ATA_TEOC       24       /* End of cycle time          DMA/PIO */
#define ATA_T4         16       /* DIOW- data hold time           PIO */
#define ATA_T2          8       /* DIOR-/DIOW- pulse width        PIO */
#define ATA_TD          8       /* DIOR-/DIOW- pulse width        DMA */
#define ATA_T1          0       /* Address valid to DIOR-/DIOW-   PIO */
#define ATA_TM          0       /* CS[1:0]valid to DIOR-/DIOW-    DMA */


/* ----- ATA (Alternate) Status Register                              */
#define ATA_SR_BSY  0x80        /* Busy                               */
#define ATA_SR_DRDY 0x40        /* Device Ready                       */
#define ATA_SR_DF   0x20        /* Device Fault                       */
#define ATA_SR_DSC  0x10        /* Device Seek Complete               */
#define ATA_SR_DRQ  0x08        /* Data Request                       */
#define ATA_SR_COR  0x04        /* Corrected data (obsolete)          */
#define ATA_SR_IDX  0x02        /*                (obsolete)          */
#define ATA_SR_ERR  0x01        /* Error                              */

/* ----- ATA Device Control Register                                  */
                                /* bits 7-3 are reserved              */
#define ATA_DCR_RST 0x04        /* Software reset   (RST=1, reset)    */
#define ATA_DCR_IEN 0x02        /* Interrupt Enable (IEN=0, enabled)  */
                                /* always write a '0' to bit0         */

/* ----- ATA Device Address Register                                  */
/* All values in this register are one's complement (i.e. inverted)   */
#define ATA_DAR_WTG 0x40        /* Write Gate                         */
#define ATA_DAR_H   0x3c        /* Head Select                        */
#define ATA_DAR_DS1 0x02        /* Drive select 1                     */
#define ATA_DAR_DS0 0x01        /* Drive select 0                     */

/* ----- Device/Head Register                                         */
#define ATA_DHR_LBA 0x40        /* LBA/CHS mode ('1'=LBA mode)        */
#define ATA_DHR_DEV 0x10        /* Device       ('0'=dev0, '1'=dev1)  */
#define ATA_DHR_H   0x0f        /* Head Select                        */

/* ----- Error Register                                               */
#define ATA_ERR_BBK  0x80        /* Bad Block                          */
#define ATA_ERR_UNC  0x40       /* Uncorrectable Data Error           */
#define ATA_ERR_IDNF 0x10       /* ID Not Found                       */
#define ATA_ERR_ABT  0x04       /* Aborted Command                    */
#define ATA_ERR_TON  0x02       /* Track0 Not Found                   */
#define ATA_ERR_AMN  0x01       /* Address Mark Not Found             */


/* ----------------------------                                       */
/* ----- ATA commands     -----                                       */
/* ----------------------------                                       */
#define CFA_ERASE_SECTORS                0xC0
#define CFA_REQUEST_EXTENDED_ERROR_CODE  0x03
#define CFA_TRANSLATE_SECTOR             0x87
#define CFA_WRITE_MULTIPLE_WITHOUT_ERASE 0xCD
#define CFA_WRITE_SECTORS_WITHOUT_ERASE  0x38
#define CHECK_POWER_MODE                 0xE5
#define DEVICE_RESET                     0x08
#define DOWNLOAD_MICROCODE               0x92
#define EXECUTE_DEVICE_DIAGNOSTIC        0x90
#define FLUSH_CACHE                      0xE7
#define GET_MEDIA_STATUS                 0xDA
#define IDENTIFY_DEVICE                  0xEC
#define IDENTIFY_PACKET_DEVICE           0xA1
#define IDLE                             0xE3
#define IDLE_IMMEDIATE                   0xE1
#define INITIALIZE_DEVICE_PARAMETERS     0x91
#define MEDIA_EJECT                      0xED
#define MEDIA_LOCK                       0xDE
#define MEDIA_UNLOCK                     0xDF
#define NOP                              0x00
#define PACKET                           0xA0
#define READ_BUFFER                      0xE4
#define READ_DMA                         0xC8
#define READ_DMA_QUEUED                  0xC7
#define READ_MULTIPLE                    0xC4
#define READ_NATIVE_MAX_ADDRESS          0xF8
#define READ_SECTOR                      0x20
#define READ_SECTORS                     0x20
#define READ_VERIFY_SECTOR               0x40
#define READ_VERIFY_SECTORS              0x40
#define SECURITY_DISABLE_PASSWORD        0xF6
#define SECURITY_ERASE_PREPARE           0xF3
#define SECURITY_ERASE_UNIT              0xF4
#define SECURITY_FREEZE_LOCK             0xF5
#define SECURITY_SET_PASSWORD            0xF1
#define SECURITY_UNLOCK                  0xF2
#define SEEK                             0x70
#define SERVICE                          0xA2
#define SET_FEATURES                     0xEF
#define SET_MAX                          0xF9
#define SET_MULTIPLE_MODE                0xC6
#define SLEEP                            0xE6
#define SMART                            0xB0
#define STANDBY                          0xE2
#define STANDBY_IMMEDIATE                0xE0
#define WRITE_BUFFER                     0xE8
#define WRITE_DMA                        0xCA
#define WRITE_DMA_QUEUED                 0xCC
#define WRITE_MULTIPLE                   0xC5
#define WRITE_SECTOR                     0x30
#define WRITE_SECTORS                    0x30


/* SET_FEATURES has a number of sub-commands (in Features Register)   */
#define CFA_ENABLE_8BIT_PIO_TRANSFER_MODE       0x01
#define ENABLE_WRITE_CACHE                      0x02
#define SET_TRANSFER_MODE_SECTOR_COUNT_REG      0x03
#define ENABLE_ADVANCED_POWER_MANAGEMENT        0x05
#define ENABLE_POWERUP_IN_STANDBY_FEATURE_SET   0x06
#define POWERUP_IN_STANDBY_FEATURE_SET_SPINUP   0x07
#define CFA_ENABLE_POWER_MODE1                  0x0A
#define DISABLE_MEDIA_STATUS_NOTIFICATION       0x31
#define DISABLE_READ_LOOKAHEAD                  0x55
#define ENABLE_RELEASE_INTERRUPT                0x5D
#define ENABLE_SERVICE_INTERRUPT                0x5E
#define DISABLE_REVERTING_TO_POWERON_DEFAULTS   0x66
#define CFA_DISABLE_8BIT_PIO_TRANSFER_MODE      0x81
#define DISABLE_WRITE_CACHE                     0x82
#define DISABLE_ADVANCED_POWER_MANAGEMENT       0x85
#define DISABLE_POWERUP_IN_STANDBY_FEATURE_SET  0x86
#define CFA_DISABLE_POWER_MODE1                 0x8A
#define ENABLE_MEDIA_STATUS_NOTIFICATION        0x95
#define ENABLE_READ_LOOKAHEAD_FEATURE           0xAA
#define ENABLE_REVERTING_TO_POWERON_DEFAULTS    0xCC
#define DISABLE_RELEASE_INTERRUPT               0xDD
#define DISABLE_SERVICE_INTERRUPT               0xDE

/* SET_MAX has a number of sub-commands (in Features Register)        */
#define SET_MAX_ADDRESS                         0x00
#define SET_MAX_SET_PASSWORD                    0x01
#define SET_MAX_LOCK                            0x02
#define SET_MAX_UNLOCK                          0x03
#define SET_MAX_FREEZE_LOCK                     0x04

/* SET_MAX has a number of sub-commands (in Features Register)        */
#define SMART_READ_DATA                         0xD0
#define SMART_ATTRIBITE_AUTOSAVE                0xD1
#define SMART_SAVE_ATTRIBUTE_VALUES             0xD3
#define SMART_EXECUTE_OFFLINE_IMMEDIATE         0xD4
#define SMART_READ_LOG                          0xD5
#define SMART_WRITE_LOG                         0xD6
#define SMART_ENABLE_OPERATIONS                 0xD8
#define SMART_DISABLE_OPERATIONS                0xD9
#define SMART_RETURN_STATUS                     0xDA

/* ----------------------------                                       */
/* ----- Structs          -----                                       */
/* ----------------------------                                       */

/* ----------------------------                                       */
/* ----- Macros           -----                                       */
/* ----------------------------                                       */

#define ata_astatus(base)      (REG32(base + ATA_ASR))
#define ata_status(base)       (REG32(base + ATA_SR))
#define ata_error(base)        (REG32(base + ATA_ERR))
#define ata_cmd(base)          (REG32(base + ATA_CR))

#define ata_dev_busy(base)     (ata_astatus(base) & ATA_SR_BSY)
#define ata_dev_cmdrdy(base)   (ata_astatus(base) & (~ATA_SR_BSY & ATA_SR_DRDY))
#define ata_dev_datrdy(base)   (ata_astatus(base) & ATA_SR_DRQ)



/*
  INTERNALS
*/

/* -------------------                                                */
/* ----- defines -----                                                */
/* -------------------                                                */
#define READ 0
#define WRITE 1

#define FMODE_READ 0
#define FMODE_WRITE 1

#define SET (1<<31)
#define CLR 0


#define PIO4 0x02
#define PIO3 0x01

/*define MAJOR, MINOR numbers                                         */
#define MAJOR(dev) (dev >> 8)
#define MINOR(dev) (dev & 0xff)

#define MINOR_DEV0 0x00
#define MINOR_DEV1 0X80


#define ATA_IOCTL_EXEC_CMD        0
#define ATA_IOCTL_READ            1
#define ATA_IOCTL_ENABLE_HOST     2
#define ATA_IOCTL_IDENTIFY_DEVICE 3
#define ATA_IOCTL_IDENTIFY_HOST   4
#define ATA_IOCTL_SELECT_DEVICE   5
#define ATA_IOCTL_SET_RST         6
#define ATA_IOCTL_SET_PIO         7
#define ATA_IOCTL_SET_FEATURES    8
#define ATA_IOCTL_SET_FTE         9

#define ARG_HW_RST  0
#define ARG_SW_RST  1
#define ARG_DEV_RST 2

/* PIO numbers and PIO timing (in ns)                                 */
#define ARG_PIO4 4
#define ARG_PIO3 3
#define ARG_PIO2 2
#define ARG_PIO1 1
#define ARG_PIO0 0

/* register transfer timings                                          */
#define PIO0_RT0  600
#define PIO0_RT1  70
#define PIO0_RT2  290
#define PIO0_RT2I 0
#define PIO0_RT4  30
#define PIO0_RT9  20

#define PIO1_RT0  383
#define PIO1_RT1  50
#define PIO1_RT2  290
#define PIO1_RT2I 0
#define PIO1_RT4  20
#define PIO1_RT9  15

#define PIO2_RT0  330
#define PIO2_RT1  30
#define PIO2_RT2  290
#define PIO2_RT2I 0
#define PIO2_RT4  15
#define PIO2_RT9  10

#define PIO3_RT0  180
#define PIO3_RT1  30
#define PIO3_RT2  80
#define PIO3_RT2I 70
#define PIO3_RT4  10
#define PIO3_RT9  10

#define PIO4_RT0  120
#define PIO4_RT1  25
#define PIO4_RT2  70
#define PIO4_RT2I 25
#define PIO4_RT4  10
#define PIO4_RT9  10

/* data transfer timings                                              */
#define PIO0_DT0  600
#define PIO0_DT1  70
#define PIO0_DT2  165
#define PIO0_DT2I 0
#define PIO0_DT4  30
#define PIO0_DT9  20

#define PIO1_DT0  383
#define PIO1_DT1  50
#define PIO1_DT2  125
#define PIO1_DT2I 0
#define PIO1_DT4  20
#define PIO1_DT9  15

#define PIO2_DT0  240
#define PIO2_DT1  30
#define PIO2_DT2  100
#define PIO2_DT2I 0
#define PIO2_DT4  15
#define PIO2_DT9  10

#define PIO3_DT0  180
#define PIO3_DT1  30
#define PIO3_DT2  80
#define PIO3_DT2I 70
#define PIO3_DT4  10
#define PIO3_DT9  10

#define PIO4_DT0  120
#define PIO4_DT1  25
#define PIO4_DT2  70
#define PIO4_DT2I 25
#define PIO4_DT4  10
#define PIO4_DT9  10



/* error numbers                                                      */
#define EINVAL -1
#define EIOCTLIARG -2

#define EOPENIDEV  -3
#define EOPENIHOST -4
#define EOPENNODEV -5




/* ------------------------------                                     */
/* ----- structs & typedefs -----                                     */
/* ------------------------------                                     */
struct inode {
  unsigned short i_rdev;
};

struct file {
  unsigned long f_mode;
  unsigned long f_flags;
};

typedef unsigned int dev_t;

struct request {
  dev_t         rq_dev;
  int           cmd;
  unsigned long sector;
  unsigned long nr_sectors;
  unsigned char *buffer;
};


/* ----------------------------                                       */
/* ----- Prototypes       -----                                       */
/* ----------------------------                                       */
int ata_open(struct inode *inode, struct file *filp);
int ata_open_device_not_found(struct inode *inode);

int ata_release(struct inode *inode, struct file *filp);

int ata_ioctl(struct inode *inode, struct file *filp, unsigned command, unsigned long argument);
unsigned long ata_calc_pio_timing(short t0, short t1, short t2, short t4, short t2i, short t9);

int ata_read_dport(unsigned long base);

int ata_check_media_change(dev_t dev);

int ata_revalidate(dev_t dev);

int ata_request(struct inode *inode, struct file *filp, struct request *request);


#endif
