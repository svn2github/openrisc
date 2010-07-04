#include "common.h"
#include "support.h"
#include "flash.h"

int fl_init (void)
{
  unsigned long tmp; 
  reg_write(FLASH_BASE_ADDR, 0x00ff00ff);
  reg_write(FLASH_BASE_ADDR, 0x00900090);
  tmp = reg_read(FLASH_BASE_ADDR) << 8;
  reg_write(FLASH_BASE_ADDR, 0x00900090);
  tmp = tmp | reg_read(FLASH_BASE_ADDR + 4);

  debug("id = %08x ", tmp);
  /*  if (tmp != 0x89188918) {
    printf ("bad ID\n");
    return 1;
    } else debug ("good ID\n");*/
  reg_write(FLASH_BASE_ADDR, 0x00ff00ff);

  return 0;
}

int check_error (unsigned long sr, unsigned long addr)
{
  if(check_error_bit(sr, FL_SR_ERASE_ERR)) {
    printf ("erase error at %08lx\n", addr);
    /* Clear status register */
    reg_write(FLASH_BASE_ADDR, 0x05D00050);
    return 1;
  } else if (check_error_bit(sr, FL_SR_PROG_ERR)) {
    printf ("program error at %08lx\n", addr);
    /* Clear status register */
    reg_write(FLASH_BASE_ADDR, 0x05D00050);
    return 1;
  } else if (check_error_bit(sr, FL_SR_PROG_LV)) {
    printf ("low voltage error\n");
    /* Clear status register */
    reg_write(FLASH_BASE_ADDR, 0x05D00050);
    return 1;
  } else if (check_error_bit(sr, FL_SR_LOCK)) {
    printf ("lock bit error at %08lx\n", addr);
    /* Clear status register */
    reg_write(FLASH_BASE_ADDR, 0x05D00050);
    return 1;
  }
  return 0;
}

int fl_block_erase (unsigned long addr)
{
  unsigned long sr;

  reg_write(addr & ~(FLASH_BLOCK_SIZE - 1), 0x00200020);
  reg_write(addr & ~(FLASH_BLOCK_SIZE - 1), 0x00D000D0);

  do {
#if 0
    reg_write(FLASH_BASE_ADDR, 0x00700070);
#endif
    sr = reg_read(addr & ~(FLASH_BLOCK_SIZE - 1));
  } while (fl_wait_busy(sr));

  /* Clear status register */
  reg_write(FLASH_BASE_ADDR, 0x00500050);
  reg_write(FLASH_BASE_ADDR, 0x00ff00ff);
#ifndef CFG_IN_FLASH
  return check_error(sr, addr);
#else
  return 0;
#endif
}

int fl_unlock_one_block (unsigned long addr)
{
  unsigned long sr = 0x0;

  reg_write((addr & ~(FLASH_BLOCK_SIZE - 1)), 0x00600060);
  reg_write((addr & ~(FLASH_BLOCK_SIZE - 1)), 0x00d000d0);

  do {
#if 0
    reg_write(FLASH_BASE_ADDR, 0x00700070);
#endif
    sr = reg_read(addr & ~(FLASH_BLOCK_SIZE - 1));
  } while (fl_wait_busy(sr));

  /* Clear status register */
  reg_write(FLASH_BASE_ADDR, 0x00500050);
  reg_write(FLASH_BASE_ADDR, 0x00ff00ff);

#ifndef CFG_IN_FLASH
  return check_error(sr, addr);
#else
  return 0;
#endif
}

int fl_unlock_blocks (void)
{
  unsigned long c;
  unsigned int i;

  for (i = 0, c = FLASH_BASE_ADDR; i < (FLASH_SIZE / FLASH_BLOCK_SIZE); 
       i++, c += FLASH_BLOCK_SIZE)
    if (fl_unlock_one_block (c)) return -1;

  return 0;
}

int fl_word_program (unsigned long addr, unsigned long val)
{
  unsigned long sr;

  reg_write(addr, 0x00400040);
  reg_write(addr, val);
  do {
#if FLASH_ORG_16_1 /* bender */
    reg_write(FLASH_BASE_ADDR, 0x00700070);
#endif
    sr = reg_read(addr & ~(FLASH_BLOCK_SIZE - 1));
  } while (fl_wait_busy(sr));
  /* Clear status register */

  reg_write(FLASH_BASE_ADDR, 0x00500050);
  reg_write(FLASH_BASE_ADDR, 0x00ff00ff);
#ifndef CFG_IN_FLASH
  return check_error(sr, addr);
#else
  return 0;
#endif
}

/* erase = 1 (whole chip), erase = 2 (required only) */
int fl_program (unsigned long src_addr, unsigned long dst_addr, unsigned long len, int erase, int verify)
{
  unsigned long tmp, taddr, tlen;
  unsigned long i;
  
  if (erase) {
    fl_unlock_blocks ();

    if (erase == 2) {
      taddr = dst_addr & ~(FLASH_BLOCK_SIZE - 1);
      tlen = (dst_addr + len + FLASH_BLOCK_SIZE - 1) / FLASH_BLOCK_SIZE;
    } else {
      taddr = FLASH_BASE_ADDR;
      tlen = FLASH_SIZE / FLASH_BLOCK_SIZE;
    }

    printf ("Erasing flash... ");
    for (i = 0, tmp = taddr; i < tlen; i++, tmp += FLASH_BLOCK_SIZE)
      if (fl_block_erase (tmp))
	return 1;

    printf ("done\n");

    if (verify) {  
      printf ("Writing test pattern... ");
      for (tmp = taddr; tmp < taddr + tlen * FLASH_BLOCK_SIZE; i++, tmp += INC_ADDR)
        if (fl_word_program (tmp, tmp))
	  return 1;

      printf ("done\n");
      
      printf ("Checking... ");

      for (tmp = taddr; tmp < taddr + tlen * FLASH_BLOCK_SIZE; i++, tmp += INC_ADDR)
        if (reg_read(tmp) != tmp) {
          printf ("failed on location %08lx: %08lx\n", tmp, REG32(tmp));
          return 1;
        }
      printf ("done\n");
    }
  }
  
  reg_write(FLASH_BASE_ADDR, 0x00ff00ff);

  printf ("Copying from %08lx-%08lx to %08lx-%08lx\n", src_addr, src_addr + len - 1, dst_addr, dst_addr + len - 1);

  tlen = len / 8;
  tmp = 0;

  printf ("Programing");
  for (i = 0; i < len; i += INC_ADDR) {
    if (fl_word_program (dst_addr + i, reg_read(src_addr + i)))
      return 1;

    if (i > tmp) {
      printf (".");
      tmp += tlen;
    }
  }

  printf (" done\n");

  if (verify) {
    printf ("Verifying");
    tmp = 0;
    for (i = 0; i < len; i += INC_ADDR) {
      if(reg_read(src_addr + i) != reg_read(dst_addr + i)) {
	printf ("error at %08lx: %lx != %lx\n", src_addr + i, 
		reg_read(src_addr + i), reg_read(dst_addr + i));
        return 1;
      }
      if (i > tmp) {
        printf (".");
        tmp += tlen;
      }
    }
  }

  printf (" done\n");
  return 0;
}
