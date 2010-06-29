#include "common.h"
#include "uart.h"
#include "screen.h"
#include "support.h"
#include "keyboard.h"
#include "spr_defs.h"
#include "int.h"

#include "build.h"

#define MAX_COMMANDS  100

extern unsigned long src_addr;

bd_t bd;

int num_commands = 0;

command_struct command[MAX_COMMANDS];

void putc (const char c)
{
  debug ("putc %i, %i = %c\n", bd.bi_console_type, c, c);
  switch (bd.bi_console_type) {
  case CT_NONE:
    break;
  case CT_UART:
    uart_putc (c);
    break;
#if CRT_ENABLED==1
  case CT_CRT:
    screen_putc (c);
    break;
#endif
  case CT_SIM:
    __printf ("%c", c);
    break;
  }
}

int getc ()
{
  int ch = 0;
  debug ("getc %i\n", bd.bi_console_type);
  switch (bd.bi_console_type) {
#if KBD_ENABLED==1
  case CT_CRT:
    while ((volatile int)kbd_head == (volatile int)kbd_tail);
    ch = kbd_buf[kbd_tail];
    kbd_tail = (kbd_tail + 1) % KBDBUF_SIZE;
    return ch;
#endif
  case CT_UART:
    return uart_getc ();
    break;
  case CT_NONE: /* just to satisfy the compiler */
  case CT_SIM:
    break;
  }
  return -1;
}

int testc ()
{
  debug ("testc %i\n", bd.bi_console_type);
  switch (bd.bi_console_type) {
#if KBD_ENABLED
  case CT_CRT:
    if (kbd_head == kbd_tail) return 0;
    else return getc ();
#endif
  case CT_UART:
    return uart_testc ();
    break;
  case CT_NONE: /* just to satisfy the compiler */
  case CT_SIM:
    break;
  }
  return -1;
}

int ctrlc ()
{
  if (testc ()) {
    switch (getc ()) {
      case 0x03:    /* ^C - Control C */
        return 1;
      default:
        break;
    }
  }
  return 0;
}

unsigned long parse_ip (char *ip)
{
  unsigned long num;
  num = strtoul (ip, &ip, 10) & 0xff;
  if (*ip++ != '.') return 0;
  num = (num << 8) | (strtoul (ip, &ip, 10) & 0xff);
  if (*ip++ != '.') return 0;
  num = (num << 8) | (strtoul (ip, &ip, 10) & 0xff);
  if (*ip++ != '.') return 0;
  num = (num << 8) | (strtoul (ip, &ip, 10) & 0xff);
  return num;
}

void change_console_type (enum bi_console_type_t con_type)
{
  debug ("Console change %i -> %i\n", bd.bi_console_type, con_type);
  /* Close previous */
  switch (bd.bi_console_type) {
  case CT_NONE:
  case CT_UART:
  case CT_CRT:
  case CT_SIM:
    break;
  }
  bd.bi_console_type = con_type;
  /* Initialize new */
  switch (bd.bi_console_type) {
  case CT_NONE:
    break;
  case CT_UART:
    uart_init ();
    break;
  case CT_CRT:
#if CRT_ENABLED==1
    screen_init ();
#endif
#if KBD_ENABLED
    kbd_init ();
#endif
    break;
  case CT_SIM:
    break;
  }
}

void register_command_func (const char *name, const char *params, const char *help, int (*func)(int argc, char *argv[]))
{
  debug ("register_command '%s'\n", name);
  if (num_commands < MAX_COMMANDS) {
    command[num_commands].name = name;
    command[num_commands].params = params;
    command[num_commands].help = help;
    command[num_commands].func = func;
    num_commands++;
  } else printf ("Command '%s' ignored; MAX_COMMANDS limit reached\n", name);
}

/* Process command and arguments by executing
   specific function. */
void mon_command(void)
{
  char c = '\0';
  char str[1000];
  char *pstr = str;
  char *command_str;
  char *argv[20];
  int argc = 0;

  /* Show prompt */
#ifdef XESS
  printf ("\norp-xsv> ");
#else
  printf ("\n" BOARD_DEF_NAME"> ");
#endif

  /* Get characters from UART */
  c = getc();
  while (c != '\r' && c != '\f' && c != '\n')
  {
    if (c == '\b')
      pstr--;
    else
      *pstr++ = c;
    putc(c);
    c = getc();
  }
  *pstr = '\0';
  printf ("\n");

  /* Skip leading blanks */
  pstr = str;
  while (*pstr == ' ' && *pstr != '\0') pstr++;

  /* Get command from the string */
  command_str = pstr;

  while (1) {
    /* Go to next argument */
    while (*pstr != ' ' && *pstr != '\0') pstr++;
    if (*pstr) {
      *pstr++ = '\0';
      while (*pstr == ' ') pstr++;
      argv[argc++] = pstr;
    }
    else
      break;
  }

  {
    int i, found = 0;

    for (i = 0; i < num_commands; i++)
      if (strcmp (command_str, command[i].name) == 0)
      {
        switch ( command[i].func(argc, &argv[0]) )
        {
          case -1:
            printf ("Missing/wrong parameters, usage: %s %s\n", command[i].name, command[i].params);
            break;
        }

        found++;
        break;
      }
    /* 'built-in' build command */
    if(strcmp(command_str, "build") == 0) {
      printf("Build tag: %s", BUILD_VERSION);
      found++;
    }
    if (!found)
      printf ("Unknown command. Type 'help' for help.\n");
  }
  
}

#if HELP_ENABLED
extern unsigned long src_addr; // Stack section ends here
/* Displays help screen */
int help_cmd (int argc, char *argv[])
{
  int i;
  for (i = 0; i < num_commands; i++)
    printf ("%-10s %-20s - %s\n", command[i].name, command[i].params, command[i].help);

  // Build info....
  printf("Info: CPU@ %dMHz", IN_CLK/1000000);
#if IC_ENABLE==1
  printf(" IC=%dB",IC_SIZE);
#endif
#if DC_ENABLE==1
  printf(" DC=%dB",DC_SIZE);
#endif
  printf("\n");
  printf("Info: Stack section addr 0x%x\n",(unsigned long) &src_addr);
  printf("Build tag: %s", BUILD_VERSION);

  return 0;
}
#endif /* HELP_ENABLED */

void module_cpu_init (void);
void module_memory_init (void);
void module_eth_init (void);
void module_dhry_init (void);
void module_camera_init (void);
void module_load_init (void);
void tick_init(void);
void module_touch_init (void);
void module_ata_init (void);
void module_hdbug_init (void);


/* List of all initializations */
void mon_init (void)
{
  /* Set defaults */
  global.erase_method = 2; /* as needed */
  global.src_addr = (unsigned long)&src_addr;
  global.dst_addr = FLASH_BASE_ADDR;
  global.eth_add[0] = ETH_MACADDR0;
  global.eth_add[1] = ETH_MACADDR1;
  global.eth_add[2] = ETH_MACADDR2;
  global.eth_add[3] = ETH_MACADDR3;
  global.eth_add[4] = ETH_MACADDR4;
  global.eth_add[5] = ETH_MACADDR5;
  global.ip = BOARD_DEF_IP;
  global.gw_ip = BOARD_DEF_GW;
  global.mask = BOARD_DEF_MASK;

#define CPU_CMDS
#define MEM_CMDS
#define DHRY_CMDS
  //#define CAMERA_CMDS
#define LOAD_CMDS
    //#define TOUCHSCREEN_CMDS
    //#define ATA_CMDS
    //#define HDBUG_CMDS
#define TICK_CMDS
#define ETH_CMDS
#define LOAD_CMDS
  
  /* Init modules */
#ifdef CPU_CMDS
  module_cpu_init ();
#endif
#ifdef MEM_CMDS
  module_memory_init ();
#endif
#ifdef ETH_CMDS
  module_eth_init ();
#endif
#ifdef DHRY_CMDS
  module_dhry_init ();
#endif
#ifdef CAMERA_CMDS
  module_camera_init ();
#endif
#ifdef LOAD_CMDS
  module_load_init ();
#endif
#ifdef TOUCHSCREEN_CMDS
  module_touch_init ();
#endif
#ifdef ATA_CMDS
  module_ata_init ();
#endif
#ifdef HDBUG_CMDS
  module_hdbug_init ();
#endif

  tick_init();

}
int tboot_cmd (int argc, char *argv[]);
/* Main shell loop */
int main(int argc, char **argv)
{
  extern unsigned long calc_mycrc32 (void);
#if 0
  extern unsigned long mycrc32, mysize;
#endif
  timestamp = 0; // clear timer counter
  
  int_init ();
  change_console_type (CONSOLE_TYPE);
  mtspr(SPR_SR, mfspr(SPR_SR) | SPR_SR_IEE);

#if SELF_CHECK
  printf ("Self check... ");
  if ((t = calc_mycrc32 ()))
      printf ("FAILED!!!\n");
  else
      printf ("OK\n");
#endif /* SELF_CHECK */
  num_commands=0;
  mon_init ();

  if (HELP_ENABLED) register_command ("help", "", "shows this help", help_cmd);

#ifdef XESS
  printf ("\nORP-XSV Monitor (type 'help' for help)\n");
#else
  printf ("\n" BOARD_DEF_NAME " monitor (type 'help' for help)\n");
  printf("\tbuild: %s", BUILD_VERSION);
#endif

  while(1) mon_command();
  // Run tboot in sim for now:  tboot_cmd (0,0);
}
