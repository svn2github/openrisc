#ifndef _MC_INIT_1_H_
#define _MC_INIT_1_H_

#if IN_CLK==25000000
#  define MC_CSR_VAL      	0x0B000300
#  define MC_MASK_VAL     	0x000003f0
#  define FLASH_BASE_ADDR 	0xf0000000
#  define FLASH_TMS_VAL   	0x00000103
#  define SDRAM_BASE_ADDR 	0x00000000
#  define SDRAM_TMS_VAL   	0x19220057
#elif IN_CLK==50000000 /* marvin on bender2 board - 50mhz */
#  define MC_CSR_VAL      	0x0B000300
#  define MC_MASK_VAL     	0x000003f0
#  define FLASH_BASE_ADDR 	0xf0000000
#  define FLASH_TMS_VAL   	0x00000810
#  define SDRAM_BASE_ADDR 	0x00000000
#  define SDRAM_TMS_VAL   	0x2a570700
#elif IN_CLK==100000000
#  define MC_CSR_VAL      	0x0B000300
#  define MC_MASK_VAL     	0x000003f0
#  define FLASH_BASE_ADDR 	0xf0000000
#  define FLASH_TMS_VAL   	0x00000810
#  define SDRAM_BASE_ADDR 	0x00000000
#  define SDRAM_TMS_VAL   	0x2a570700
#elif IN_CLK==120000000 /* marvin on 120mhz */
#  define MC_CSR_VAL      	0x0B000300
#  define MC_MASK_VAL     	0x000003f0
#  define FLASH_BASE_ADDR 	0xf0000000
#  define FLASH_TMS_VAL   	0x00000810
#  define SDRAM_BASE_ADDR 	0x00000000
#  define SDRAM_TMS_VAL   	0x2a570700
#else
#  error  No MC initialize values for this frequency
#endif

#endif /* _MC_INIT_1_H_ */ 

