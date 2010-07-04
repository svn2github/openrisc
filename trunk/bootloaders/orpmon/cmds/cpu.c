#include "common.h"
#include "support.h"
#include "spr_defs.h"

int ic_enable_cmd (int argc, char *argv[])
{
  unsigned long addr;
  unsigned long sr;

  if (argc) return -1;
  /* Invalidate IC */
  for (addr = 0; addr < 8192; addr += 16)
    asm("l.mtspr r0,%0,%1": : "r" (addr), "i" (SPR_ICBIR));  
  
  /* Enable IC */
  asm("l.mfspr %0,r0,%1": "=r" (sr) : "i" (SPR_SR));
  sr |= SPR_SR_ICE;
  asm("l.mtspr r0,%0,%1": : "r" (sr), "i" (SPR_SR));  
  asm("l.nop");
  asm("l.nop");
  asm("l.nop");
  asm("l.nop");
  return 0;
}

int ic_disable_cmd (int argc, char *argv[])
{
  unsigned long sr;

  if (argc) return -1;
  /* Disable IC */
  asm("l.mfspr %0,r0,%1": "=r" (sr) : "i" (SPR_SR));
  sr &= ~SPR_SR_ICE;
  asm("l.mtspr r0,%0,%1": : "r" (sr), "i" (SPR_SR));  
  asm("l.nop");
  asm("l.nop");
  asm("l.nop");
  asm("l.nop");
  return 0;
}

int dc_enable_cmd (int argc, char *argv[])
{
  unsigned long addr;
  unsigned long sr;

  if (argc) return -1;
  /* Invalidate DC */
  for (addr = 0; addr < 8192; addr += 16)
    asm("l.mtspr r0,%0,%1": : "r" (addr), "i" (SPR_DCBIR));  
  
  /* Enable DC */
  asm("l.mfspr %0,r0,%1": "=r" (sr) : "i" (SPR_SR));
  sr |= SPR_SR_DCE;
  asm("l.mtspr r0,%0,%1": : "r" (sr), "i" (SPR_SR));  
  asm("l.nop");
  asm("l.nop");
  asm("l.nop");
  asm("l.nop");
  return 0;
}

int dc_disable_cmd (int argc, char *argv[])
{
  unsigned long sr;

  if (argc) return -1;
  /* Disable DC */
  asm("l.mfspr %0,r0,%1": "=r" (sr) : "i" (SPR_SR));
  sr &= ~SPR_SR_DCE;
  asm("l.mtspr r0,%0,%1": : "r" (sr), "i" (SPR_SR));  
  asm("l.nop");
  asm("l.nop");
  asm("l.nop");
  asm("l.nop");
  return 0;
}

int mfspr_cmd (int argc, char *argv[])
{
  unsigned long val, addr;

  if (argc ==	1) {
    addr = strtoul (argv[0], 0, 0);
    /* Read SPR */
    asm("l.mfspr %0,%1,0": "=r" (val) : "r" (addr));
    printf ("\nSPR %04lx: %08lx", addr, val);
  } else return -1;
	return 0;
}

int mtspr_cmd (int argc, char *argv[])
{
  unsigned long val, addr;
  if (argc == 2) {
    addr = strtoul (argv[0], 0, 0);
    val = strtoul (argv[1], 0, 0);
    /* Write SPR */
    asm("l.mtspr %0,%1,0": : "r" (addr), "r" (val));
    asm("l.mfspr %0,%1,0": "=r" (val) : "r" (addr));
    printf ("\nSPR %04lx: %08lx", addr, val);
  } else return -1;
	return 0;
}

void module_cpu_init (void)
{
  register_command ("ic_enable", "", "enable instruction cache", ic_enable_cmd);
  register_command ("ic_disable", "", "disable instruction cache", ic_disable_cmd);
  register_command ("dc_enable", "", "enable data cache", dc_enable_cmd);
  register_command ("dc_disable", "", "disable data cache", dc_disable_cmd);
  register_command ("mfspr", "<spr_addr>", "show SPR", mfspr_cmd);
  register_command ("mtspr", "<spr_addr> <value>", "set SPR", mtspr_cmd);
}
