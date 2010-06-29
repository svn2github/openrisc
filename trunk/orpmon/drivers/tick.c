
#include "common.h"
#include "support.h"
#include "spr_defs.h"
#include "spincursor.h"

void tick_init(void)
{
  mtspr(SPR_SR, SPR_SR_TEE | mfspr(SPR_SR));
  mtspr(SPR_TTMR, SPR_TTMR_IE | SPR_TTMR_RT | ((IN_CLK/TICKS_PER_SEC) & SPR_TTMR_PERIOD));
}

void tick_interrupt(void)
{
  timestamp++;
  mtspr(SPR_TTMR, SPR_TTMR_IE | SPR_TTMR_RT | ((IN_CLK/TICKS_PER_SEC) & SPR_TTMR_PERIOD));
  spincursor(); // Check if we want to spin the cursor...
}

/* 
  sleep for n timer-ticks
*/
void sleep(unsigned long sleep_time)
{
  unsigned long start_time;

  start_time = timestamp;
  
  while ( (timestamp - start_time) < sleep_time)
  ; /* do nothing */
}

