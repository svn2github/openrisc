#ifndef _COMMON_H_
#define _COMMON_H_

#include "board.h"

/* max number of images saved in flash */
#ifndef MAX_IMAGES
#define MAX_IMAGES 40
#endif

#ifdef	DEBUG
#define debug(fmt,args...) printf (fmt ,##args)
#else
#define debug(fmt,args...) __printf (fmt ,##args)
#endif

/* Stack top */
extern unsigned long _stack_top;

/* A Board Information structure that is given to a program when
 * ppcboot starts it up. */
typedef struct bd_info {
  enum bi_console_type_t {
    CT_NONE,
    CT_UART,
    CT_CRT,
    CT_SIM,
  } bi_console_type;
  unsigned long	bi_memstart;	/* start of  DRAM memory		*/
  unsigned long	bi_memsize;	/* size	 of  DRAM memory in bytes	*/
  unsigned long	bi_flashstart;	/* start of FLASH memory		*/
  unsigned long	bi_flashsize;	/* size	 of FLASH memory		*/
  unsigned long	bi_flashoffset; /* reserved area for startup monitor	*/
  unsigned long	bi_sramstart;	/* start of  SRAM memory		*/
  unsigned long	bi_sramsize;	/* size	 of  SRAM memory		*/
  unsigned long	bi_bootflags;	/* boot / reboot flag (for LynxOS)	*/
  unsigned long	bi_ip_addr;	/* IP Address				*/
  unsigned short bi_ethspeed;	/* Ethernet speed in Mbps		*/
  unsigned long bi_intfreq;	/* Internal Freq, in MHz		*/
  unsigned long bi_busfreq;	/* Bus Freq, in MHz			*/
  unsigned long bi_baudrate;	/* Console Baudrate			*/
} bd_t;

typedef struct {
  unsigned long src_addr;
  unsigned long dst_addr;
  unsigned long start_addr;
  unsigned long length;
  unsigned long ip;
  unsigned long gw_ip;
  unsigned long mask;
  unsigned long srv_ip;
  unsigned char eth_add[6];
  unsigned long erase_method; /* 0 = do not erase, 1 = fully, 2 = as needed */
} global_struct;


/* structure for command interpreter                                  */
typedef struct {
  const char *name;
  const char *params;
  const char *help;
  int (*func)(int argc, char *argv[]);
} command_struct;

// Keep a CRC during TFTP receive
#define TFTP_CALC_CRC

typedef struct  {
  unsigned long eth_ip;
  unsigned long eth_mask;
  unsigned long eth_gw;
  unsigned long tftp_srv_ip;
  char tftp_filename[64];
  unsigned long img_number;
  unsigned long img_start_addr[MAX_IMAGES];
  unsigned long img_length[MAX_IMAGES];
} flash_cfg_struct;

extern bd_t bd;
extern global_struct global;

/* stdio */
extern int getc (void);
extern int testc (void);
extern int ctrlc (void);
extern void putc (const char c);
extern int printf (const char *fmt, ...);
extern void show_mem (int start, int stop);
extern unsigned long parse_ip (char *ip);

/* simulator stdout */
extern void __printf (const char *fmt, ...);

/* Reports a 32bit value to the simulator */
extern void report(unsigned long value);

/* Commands stuff */
#if HELP_ENABLED
#define register_command(name,params,help,funct) register_command_func (name, params, help, funct)
#else /* !HELP_ENABLED */
#define register_command(name,params,help,funct) register_command_func (name, "", "", funct)
#endif /* HELP_ENABLED */

extern void register_command_func (const char *name, const char *params, const char *help, int (*func)(int argc, char *argv[]));

/* Redirects console */
extern void change_console_type (enum bi_console_type_t con_type);

/* OR1k specific */
/* For writing into SPR. */
extern void mtspr(unsigned long spr, unsigned long value);

/* For reading SPR. */
extern unsigned long mfspr(unsigned long spr);


#endif	/* _COMMON_H_ */

