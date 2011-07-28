#include "spr_defs.h"
#include "support.h"

#include "tick.h"

void tick_init(unsigned int period) {
	mtspr(SPR_TTMR, period & SPR_TTMR_PERIOD); 
	
   	mtspr(SPR_TTMR, mfspr(SPR_TTMR) & ~(SPR_TTMR_IP));    			// clears interrupt
    mtspr(SPR_TTMR, mfspr(SPR_TTMR) | SPR_TTMR_RT | SPR_TTMR_IE);	// restart after match, enable interrupt
	
    // set OR1200 to accept exceptions
    mtspr(SPR_SR, mfspr(SPR_SR) | SPR_SR_TEE);
}

