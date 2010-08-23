#include "common.h"
#include "support.h"
#include "flash.h"
#include "net.h"
#include "uart.h"
#include "spr-defs.h"

#ifndef MAX_IMAGES
#define MAX_IMAGES 20
#endif

extern unsigned long fprog_addr;
extern char *tftp_filename;

static flash_cfg_struct gcfg = 
  { BOARD_DEF_IP, BOARD_DEF_MASK, BOARD_DEF_GW,  BOARD_DEF_TBOOT_SRVR };

// Not booting from flash, so just set these from board.h -- jb
//static flash_cfg_struct __attribute__ ((section(".config"))) gcfg = { BOARD_DEF_IP, BOARD_DEF_MASK, BOARD_DEF_GW,  BOARD_DEF_TBOOT_SRVR };
//static flash_cfg_struct __attribute__ ((section(".config"))) gcfg = { 0, 0, 0, 0 };

#define FLASH_IMAGES_BASE 0xf0300000

#define ALIGN(addr,size) ((addr + (size-1))&(~(size-1)))
// If the image buffer is word aligned, then uncomment this, but it was set up 
// so that the tftp images would download quicker
#define COPY_AND_BOOT_WORD_ALIGNED
#ifdef COPY_AND_BOOT_WORD_ALIGNED
void copy_and_boot(unsigned long src, 
		   unsigned long dst, 
		   unsigned long len,
		   int tx_next)
{
  __asm__ __volatile__("   ;\
        l.addi  r8,r0,0x1  ;\
        l.mtspr r0,r8,0x11  ;\
        l.nop               ;\
        l.nop               ;\
        l.nop               ;\
        l.nop               ;\
        l.nop               ;\
2:                          ;\
        l.sfgeu r4,r5       ;\
        l.bf    1f          ;\
        l.nop               ;\
        l.lwz   r8,0(r3)    ;\
        l.sw    0(r4),r8    ;\
        l.addi  r3,r3,4     ;\
        l.j     2b          ;\
        l.addi  r4,r4,4     ;\
1:      l.sw    0x0(r0),r6  ;\
        l.ori   r8,r0,0x100 ;\
        l.jr    r8          ;\
        l.nop");
}
#else
void copy_and_boot(unsigned long src, 
		   unsigned long dst, 
		   unsigned long len,
		   int tx_next)
{
  __asm__ __volatile__("   ;\
        l.addi  r8,r0,0x1  ;\
        l.mtspr r0,r8,0x11  ;\
        l.nop               ;\
        l.nop               ;\
        l.nop               ;\
        l.nop               ;\
        l.nop               ;\
2:                          ;\
        l.sfgeu r4,r5       ;\
        l.bf    1f          ;\
        l.nop               ;\
        l.lbz   r8,0(r3)    ;\
        l.sb    0(r4),r8    ;\
        l.addi  r3,r3,1     ;\
        l.j     2b          ;\
        l.addi  r4,r4,1     ;\
1:      l.sw    0x0(r0),r6  ;\
        l.ori   r8,r0,0x100 ;\
        l.jr    r8          ;\
        l.nop");
}
#endif
/* WARNING: stack and non-const globals should not be used in this function 
   -- it may corrupt what have we loaded;
   start_addr should be 0xffffffff if only copying should be made
   no return, when start_addr != 0xffffffff, if successful */
int copy_memory_run (register unsigned long src_addr, 
		     register unsigned long dst_addr, 
		     register unsigned long length, 
		     register int erase, 
		     register unsigned long start_addr)
{
  unsigned long i, flags;

  register char *dst = (char *) dst_addr;
  register const char *src = (const char *) src_addr;

  if (dst_addr >= FLASH_BASE_ADDR) {
    if (dst_addr + length >= FLASH_BASE_ADDR + FLASH_SIZE) {
      printf ("error: region does not fit into flash.\n");
       return 1;
    }
#ifndef CFG_IN_FLASH
    fl_program (src_addr, dst_addr, length, erase, 1 /* do verify */);
#else
    /* we must disable interrupts! */
    flags=mfspr(SPR_SR);
    mtspr(SPR_SR,flags & ~(SPR_SR_TEE | SPR_SR_IEE));
    
    printf("Unlocking flash... ");
    for(i = 0; i < length; i += FLASH_BLOCK_SIZE)
      fl_ext_unlock(dst_addr + i);
    printf("done\n");
    
    printf("Erasing flash... ");
    for(i = 0; i < length; i += FLASH_BLOCK_SIZE)
      fl_ext_erase(dst_addr+i);
    printf("done\n");
    
    printf("Programing flash:\n\t");
    for (i = 0; i < length; i += INC_ADDR) {
      if(((i+INC_ADDR) % 1000) == 0)
	printf("#");
      if((i % (65*1000)) == 0)
	printf("\n\t");
      if (fl_ext_program (dst_addr + i, reg_read(src_addr + i))) {
	printf("error programing at 0x%08lx!\n", dst_addr+i); 
	return 1;
      }
    }
    printf("Verifying flash... ");
    for(i = 0; i < length; i += INC_ADDR) {
      if( reg_read(dst_addr+i) != reg_read(src_addr + i)) {
        printf ("error at %08lx: %08lx != %08lx\n", src_addr + i, 
		reg_read(src_addr + i), reg_read(dst_addr + i));
	return 1;
      }
    }
    printf("OK!\n");
    mtspr(SPR_SR, flags);
#endif
    if(start_addr == 0xffffffff)
      return 0;
  } 
  else {
    while (length--) *dst++ = *src++;
    if (start_addr == 0xffffffff)
      return 0;
  }
  /* Run the program */
  ((void (*)(void)) start_addr)();
  return 0; /* just to satisfy the cc */
}

void bf_jump(unsigned long addr)
{
  asm("l.jr   r3");
  asm("l.nop  0x0");
}

int boot_flash_cmd(int argc, char *argv[])
{
  unsigned long addr,val,jaddr;
  addr = 17;
  val = 0;
  /* clear SR */

  asm("l.mtspr %0,%1,0": : "r" (addr), "r" (val));
  /* jump */
  if(argc == 0)
    bf_jump(FLASH_BASE_ADDR+0x100);
  else {
    jaddr = strtoul(argv[0], 0, 0);
    bf_jump(jaddr);
  }
  return 0;
}

void
init_load (void)
{
#if 0 // JB - removing flash stuff
#  ifdef CFG_IN_FLASH
  copy_memory_run((unsigned long)&fl_word_program, (unsigned long)&fprog_addr, 
		  95, 0, 0xffffffff);
  copy_memory_run((unsigned long)&fl_block_erase, (unsigned long)&fprog_addr+96,
		  119, 0, 0xffffffff);
  copy_memory_run((unsigned long)&fl_unlock_one_block,
		  (unsigned long)&fprog_addr+96+120, 
		  115, 0, 0xffffffff);

  fl_ext_program = (t_fl_ext_program)&fprog_addr;
  fl_ext_erase = (t_fl_erase)&fprog_addr+96;
  fl_ext_unlock = (t_fl_erase)&fprog_addr+96+120;

#    if 0
  printf("fl_word_program(): 0x%x\tfl_ext_program(): 0x%x\n",
	 &fl_word_program, fl_ext_program);
  printf("fl_block_erase: 0x%x\tfl_ext_erase(): 0x%x\n",
	 &fl_block_erase, fl_ext_erase);
  printf("fl_unlock_one_block(): 0x%x\tfl_ext_unlock(): 0x%x\n",
	 &fl_unlock_one_block, fl_ext_unlock);
#    endif

#  else /* not CFG_IN_FLASH */
  fl_ext_program = (t_fl_ext_program)&fl_word_program;
  fl_ext_erase = (t_fl_erase)&fl_block_erase;
  fl_ext_unlock = (t_fl_erase)&fl_unlock_one_block;
#  endif /* CFG_IN_FLASH */
#endif

  /*
  global.ip = gcfg.eth_ip;
  global.gw_ip = gcfg.eth_gw;
  global.mask = gcfg.eth_mask;
  global.srv_ip = gcfg.tftp_srv_ip;
  global.src_addr = 0x100000;
  tftp_filename = "boot.img";
  */

  global.ip = BOARD_DEF_IP;
  global.gw_ip = BOARD_DEF_GW;
  global.mask = BOARD_DEF_MASK;
  global.srv_ip = BOARD_DEF_TBOOT_SRVR;
  global.src_addr = BOARD_DEF_LOAD_SPACE;
  tftp_filename = BOARD_DEF_IMAGE_NAME;

  /*memcpy(tftp_filename, gcfg.tftp_filename, strlen(gcfg.tftp_filename));
    tftp_filename[strlen(gcfg.tftp_filename)] = '\0';*/
}

int tftp_cmd (int argc, char *argv[])
{
  switch (argc) {
    case 0: tftp_filename = "boot.img";
	          break;
    case 3: global.src_addr = strtoul (argv[2], 0, 0);
    case 2: global.srv_ip = parse_ip (argv[1]);
    case 1: tftp_filename = &argv[0][0];
            break;
  }
  NetLoop(TFTP);
  return 0;
}

int tftp_conf_cmd(int argc, char *argv[])
{
  switch(argc) {
  case 0:
    printf("Image filename: %s", tftp_filename);
    printf("\nSrc addr: 0x%lx", global.src_addr);
    printf("\nServer IP: %s", inet_ntoa(global.srv_ip));
    return 0;
  case 3:
    global.src_addr = strtoul(argv[2], 0, 0);
    global.srv_ip = inet_aton(argv[1]);
    tftp_filename = argv[0];
    tftp_filename[strlen(argv[0])] = '\0';
    break;
  case 2:
    global.srv_ip = inet_aton(argv[1]);
    tftp_filename = argv[0];
    break;
  case 1:
    tftp_filename = argv[0];
    break;
  }
  return 0;
}

void save_global_cfg(flash_cfg_struct cfg)
{
  unsigned long dst = (unsigned long)&gcfg, src = (unsigned long)&cfg;
  unsigned long i, end, flags;

  end = (unsigned long)&cfg + sizeof(flash_cfg_struct);

  printf("Saving global cfg from 0x%lx (end: 0x%lx) to 0x%lx...", src, end, dst);

  /* we must disable interrupts! */
  flags=mfspr(SPR_SR);
  mtspr(SPR_SR,flags & ~(SPR_SR_TEE | SPR_SR_IEE));
  /*  printf("Unlocking... ");*/
  for(i = 0; (src+i <= end); i += FLASH_BLOCK_SIZE) {
    fl_ext_unlock(dst+i);
  }
  /*  printf("done\n");*/
  /*  printf("Erasing... ");*/
  for(i = 0; (src+i <= end); i += FLASH_BLOCK_SIZE)
    fl_ext_erase(dst);
  /*  printf("done\n");*/
  /*  printf("Programing... ");*/
  for(i = 0; (src+i <= end); i +=INC_ADDR) {
    if(fl_ext_program(dst+i, reg_read(src+i))) {
      printf("Error ocurred while saving.\n");
      return;
    }
  }
  printf("done\n");

  /* and than enable it back */
  mtspr(SPR_SR, flags);
  return;
}

int save_conf_cmd(int argc, char *argv[])
{
  flash_cfg_struct newCfg;

  newCfg = gcfg;

  newCfg.eth_ip = global.ip;
  newCfg.eth_mask = global.mask;
  newCfg.eth_gw = global.gw_ip;
  newCfg.tftp_srv_ip = global.srv_ip;
  /*  memcpy(newCfg.tftp_filename, tftp_filename, strlen(tftp_filename));*/

  save_global_cfg(newCfg);
  return 0;
}
int copy_cmd (int argc, char *argv[])
{
  switch (argc) {
    case 3: global.src_addr = strtoul (argv[2], 0, 0);
    case 2: global.length = strtoul (argv[2], 0, 0);
    case 1: global.src_addr = strtoul (argv[2], 0, 0);
    case 0: return copy_memory_run (global.src_addr, global.dst_addr, 
				    global.length, global.erase_method, 
				    0xffffffff);
  }
  return -1;
}

void
images_info(void)
{
  int i;
  printf("Number of images: 0x%lx\n", gcfg.img_number);
  for(i = 0; i < gcfg.img_number; i++)
    printf("%d. image size: 0x%lx (at 0x%08lx)\n", i+1, 
	   gcfg.img_length[i], gcfg.img_start_addr[i]);
}

/*
 * get_good_addr() 
 *
 * Here we try to find the most suitable place for our image. We search for
 * a hole between images, that is big enough (but as small as possible).
 *
 */
unsigned long
get_good_addr(unsigned int size)
{
  unsigned long start_addr[MAX_IMAGES], end_addr[MAX_IMAGES];
  unsigned long free[MAX_IMAGES], st_addr[MAX_IMAGES];
  unsigned long tmpval;
  unsigned int i = 0, j;

  flash_cfg_struct myCfg;
  myCfg = gcfg;

  /* we are full */
  if(gcfg.img_number == MAX_IMAGES)
    return 0xffffffff;

  if(gcfg.img_number == 0)
    return FLASH_IMAGES_BASE;

  for(i = 0; i < MAX_IMAGES; i++) {
    start_addr[i] = 0;
    end_addr[i] = 0;
    free[i] = 0;
    st_addr[i] = 0;
  }

  for(i = 0; i < myCfg.img_number; i++) {
    start_addr[i] = myCfg.img_start_addr[i];
    end_addr[i] = ALIGN((myCfg.img_start_addr[i] + myCfg.img_length[i]), 
			FLASH_BLOCK_SIZE);
  }
  /*  printf("\n");
  for(i = 0; i < myCfg.img_number; i++)
    printf("start: 0x%08x, end: 0x%08x\n", start_addr[i], end_addr[i]);
    printf("\n");*/
  /* bubble sorting by start_addr */

  for(j = myCfg.img_number - 1; j > 0; j--)
    for(i = 0; i < j; i++)
      if(start_addr[i] > start_addr[i+1]) {
	tmpval = start_addr[i];
	start_addr[i] = start_addr[i+1];
	start_addr[i+1] = tmpval;
	tmpval = end_addr[i];
	end_addr[i] = end_addr[i+1];
	end_addr[i+1] = tmpval;
      }

  /*  for(i = 0; i < myCfg.img_number; i++) 
    printf("start: 0x%08x, end: 0x%08x\n", start_addr[i], end_addr[i]);
    printf("\n");*/

  /* now we calculate free space betwens segments */
  for(i = 1; i < myCfg.img_number; i++) {
    st_addr[i] = end_addr[i - 1];
    free[i] = start_addr[i] - end_addr[i - 1];
  }

  /* here we calcuta first position (starting with FLASH_IMAGES_BASE)... */
  st_addr[0] = FLASH_IMAGES_BASE + 0;
  free[0] = start_addr[0] - FLASH_IMAGES_BASE;
  /* ... and last one (ending with FLASH_IMAGES_BASE + FLASH_SIZE). */
  st_addr[myCfg.img_number] = end_addr[myCfg.img_number-1];
  free[myCfg.img_number] = (FLASH_IMAGES_BASE + FLASH_SIZE) - 
    end_addr[myCfg.img_number-1];

  /* yet another bubble sort by free (space) */
  for(j = myCfg.img_number; j > 0; j--)
    for(i = 0; i < j; i++)
      if(free[i] > free[i+1]) {
	tmpval = free[i];
	free[i] = free[i+1];
	free[i+1] = tmpval;
	tmpval = st_addr[i];
	st_addr[i] = st_addr[i+1];
	st_addr[i+1] = tmpval;
      }

  /* now we pick the smallest but just big enough for our size */
  for(i = 0; i <= myCfg.img_number; i++)
    if(free[i] >= size)
      return st_addr[i];

  /* there is not enough space (in one segment) left */
  return 0;
}

unsigned long
prepare_img_data(unsigned int num, unsigned int size)
{
  int i;
  unsigned long addr=0;
  flash_cfg_struct newCfg;

  newCfg = gcfg;

  if(newCfg.img_number >= MAX_IMAGES) {
    printf("Maximum images exceeded: %d\n", MAX_IMAGES);
    return 0xffffffff;
  }

  newCfg.img_number++;

  if((num > newCfg.img_number) || (num == 0))
    num = newCfg.img_number;

  addr = get_good_addr(size);
  if(addr == 0x00) {
    printf("Can not find suitable place in flash. (None of free segments are big enough)\n");
    return 0xffffffff;
  }

  if(num < newCfg.img_number)
    for(i=newCfg.img_number-1; i >= num; i--) {
      newCfg.img_length[i] = newCfg.img_length[i-1];
      newCfg.img_start_addr[i] = newCfg.img_start_addr[i-1];
    }

  newCfg.img_length[num-1] = size;
  newCfg.img_start_addr[num-1] = addr;

  save_global_cfg(newCfg);
  return addr;
}

int
del_image_cmd(int argc, char *argv[])
{
  unsigned num, i;
  flash_cfg_struct newCfg = gcfg;

  newCfg.img_number = gcfg.img_number;
  for(i = 0; i < MAX_IMAGES; i++)
    newCfg.img_length[i] = gcfg.img_length[i];

  printf("Number of images available: 0x%lx\n", newCfg.img_number);

  if(argc == 0) {
    newCfg.img_number = 0;
    for(i = 0; i < MAX_IMAGES; i++) {
      newCfg.img_length[i] = 0;
      newCfg.img_start_addr[i] = 0;
    }
    save_global_cfg(newCfg);
    return 0;
  }
  else {
    num = strtoul(argv[0], 0, 0);
  }

  if(newCfg.img_number == 0) {
    printf("Nothing to delete!\n");
    return 0;
  }
  if((num == 0) || (num > newCfg.img_number))
    num = newCfg.img_number;

  for(i=num-1; i < newCfg.img_number; i++) {
    newCfg.img_length[i] = newCfg.img_length[i+1];
    newCfg.img_start_addr[i] = newCfg.img_start_addr[i+1];
  }
  
  newCfg.img_number--;
  save_global_cfg(newCfg);
  return 0;
}

int
boot_cmd(int argc, char *argv[])
{
  int num;
  extern int tx_next;

  if(argc == 0) {
    images_info();
    return 0;
  }

  num = strtoul(argv[0],0,0);
  if(gcfg.img_number < num) {
    printf("There are only %lu images, you requested %d!\n", 
	   gcfg.img_number, num);
    return -1;
  }

  printf("Copying image number %d from 0x%lx, size: 0x%lx...",
	 num, gcfg.img_start_addr[num-1], gcfg.img_length[num-1]);

  printf("booting...\n");
  copy_and_boot(gcfg.img_start_addr[num-1], 0x0, gcfg.img_length[num-1], 
		tx_next);
  return 0;
}

int mGetData(unsigned long);

#if 0 // Disable sboot - JB
int sboot_cmd (int argc, char *argv[])
{
  int copied;
  unsigned int num = 0xffffffff, addr = 0x0;

  switch(argc) {
  case 0:
    num = 0xffffffff;
    break;
  case 1:
    num = strtoul(argv[0], 0, 0);
    break;
  }

  copied = mGetData(global.src_addr);
  if(copied <= 0) {
    printf("sboot: error while getting the image!");
    return -1;
  }
  printf("image size: 0x%x\n", copied);

  if(num != 0xffffffff) {
    addr = prepare_img_data(num, copied);
    if(addr == 0xffffffff)
      printf("Image not written to flash!\n");
    else {
      printf("Copying image to flash, image number: %d, dst_addr: 0x%x\n", 
	     num, addr);
      copy_memory_run(global.src_addr, gcfg.img_start_addr[num-1], copied, 2, 0xffffffff);
    }
  }

  return 0;
}
#endif

void relocate_code(void* destination, void* function, int length_words)
{
  // Just copy the function word at a time from one place to another
  int i;
  unsigned long * p1 = (unsigned long*) destination;
  unsigned long * p2 = (unsigned long*) function;
  for(i=0;i<length_words;i++)
    p1[i] = p2[i];
}

// DC disable command in cpu.c
extern int dc_disable_cmd(int argc, char *argv[]);

int tboot_cmd (int argc, char *argv[])
{
  int copied;
  unsigned int num = 0xffffffff, addr = 0x0;
  extern int tx_next;
  // NetTxPacket wasn't getting cleared before we used it...
  NetTxPacket = 0; 
  NetBootFileSize = 0;

  switch (argc) {
  case 0:
    num = 0xffffffff;
    break;
  case 1:
    printf("argv[0] %p\n", argv[0]);
    num = strtoul(argv[0], 0, 0);
    printf("num %d\n", num);
    break;
  }

  // Disable data cache if present
  if (mfspr(SPR_SR) & SPR_SR_DCE)
    {
      printf("Disabling data cache\n");
      dc_disable_cmd(0, 0);
    }
  
  // Kick off copy
  copied =NetLoop(TFTP);
  if (copied <= 0) {
    printf("tboot: error while getting the image '%s'", tftp_filename);
    return -1;
  }

  if (global.src_addr > 0)
    {
      /* the point of no return */
      printf("tboot: copying 0x%lx -> 0x0, image size 0x%x...\n",
	     global.src_addr, copied);
    }

  // Disable timer: clear it all!
  mtspr (SPR_SR, mfspr(SPR_SR) & ~SPR_SR_TEE);
  mtspr(SPR_TTMR, 0);
  
  // Put the copyboot program at 24MB mark in memory
#define COPYBOOT_LOCATION (1024*1024*24) 
  printf("tboot: relocating copy loop to 0x%x ...\n", (unsigned long)COPYBOOT_LOCATION);  
  // Setup where we'll copy the relocation function to
  void (*relocated_function)(unsigned long, unsigned long, unsigned long, int)
    = (void*) COPYBOOT_LOCATION;
  // Now copy the function there, 32 words worth, increase this if needed...
  relocate_code((void*)COPYBOOT_LOCATION, copy_and_boot, 32);
  // Indicate we'll jump there...
  printf("tboot: Relocate (%d bytes from 0x%x to 0) and boot image, ...\n", copied, (unsigned long) global.src_addr);
  // Now do the copy and boot
  (*relocated_function)(global.src_addr, 0x0, 0x0 + copied, tx_next);

  return 0;
}




void module_load_init (void)
{

  register_command ("tftp_conf", "[ <file> [ <srv_ip> [ <src_addr>]]]", "TFTP configuration", tftp_conf_cmd);
  register_command ("tboot", "[<image number>]", "Bootstrap image downloaded via tftp", tboot_cmd);
#if 0
  register_command ("tftp", "[<file> [<srv_ip> [<src_addr>]]]",  "TFTP download", tftp_cmd);
  register_command ("copy", "[<dst_addr> [<src_addr [<length>]]]", "Copy memory", copy_cmd);
  register_command ("sboot", "[<image number>]", "Bootstrap image downloaded via serial (Y/X modem)", sboot_cmd);
  register_command ("boot", "[<image number>]", "Bootstrap image copied from flash.", boot_cmd);
  register_command ("del_image", "[<image number>]", "Delete image", del_image_cmd);
  register_command ("save_conf", "", "Save current configuration into flash", save_conf_cmd);
  register_command ("boot_flash", "[<start_addr>]", "Boot image from <start_addr> (default from flash)", boot_flash_cmd);
#endif
  init_load();
}
