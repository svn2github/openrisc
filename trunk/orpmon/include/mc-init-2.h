#ifndef _MC_INIT_2_H_
#define _MC_INIT_2_H_

/* clock period in [ns] */
#define SYS_CLK_PERIOD    (1000000000/IN_CLK)

/* FLASH timings: worst cases in ns, from data sheets */
#  define FLASH_WA_TIME   150 /* write access*/
#  define FLASH_WE_DELAY  30  /* write enable*/
#  define FLASH_WH_TIME   0   /* write hold */
#  define FLASH_RA_TIME   150 /* read access*/
#  define FLASH_PRA_TIME  25  /* page read access time */
#  define FLASH_RT_TIME   35  /* read turnaround time */

/* SDRAM timings: worst cases in ns, from data sheets */
#  define SDRAM_tRCD      23
#  define SDRAM_tWR       20
#  define SDRAM_tRC       60
#  define SDRAM_tRFC      60 /* sometimes the same as tRC */
#  define SDRAM_tRAS      50 /* use the worst case minimal value */
#  define SDRAM_tRP       23
#  define SDRAM_tRRD      15
#  define SDRAM_tREF      ((64000000/8192)+1)

#  define FLASH_BAR_VAL   FLASH_BASE_ADDR
#  define FLASH_AMR_VAL   (~(FLASH_SIZE-1))     /* address mask register */
#  define SDRAM_BASE_ADDR 0x00000000
#  define SDRAM_SIZE      0x02000000
#  define SDRAM_BAR_VAL   SDRAM_BASE_ADDR
#  define SDRAM_AMR_VAL   (~(SDRAM_SIZE-1))

/* independant from flash properties, always 0 ;)   */
#  define FLASH_OE_DELAY  0              /* output enable */
#  define FLASH_OE_VAL    ((FLASH_OE_DELAY+(SYS_CLK_PERIOD-1))/SYS_CLK_PERIOD)

// define FLASH_WTR_VAL   0x00011009 /* write timings */
#  define FLASH_WTR_VAL   ((0x000003ff & ((FLASH_WA_TIME-1) /SYS_CLK_PERIOD)) | \
                           (0x0000f000 & ((FLASH_WE_DELAY-1)/SYS_CLK_PERIOD)) | \
                           (0x001f0000 & ((FLASH_WH_TIME)   /SYS_CLK_PERIOD)))

// define FLASH_RTR_VAL   0x01002009 /* read timings */
#  define FLASH_RTR_VAL   ((0x000003ff & ((FLASH_RA_TIME-1) /SYS_CLK_PERIOD)) | \
                           (0x0000f000 &  (FLASH_OE_VAL                    )) | \
                           (0x001f0000 & ((FLASH_PRA_TIME-1)/SYS_CLK_PERIOD)) | \
                           (0x1f000000 & ((FLASH_RT_TIME-1  /SYS_CLK_PERIOD))))

/* round this value down:
 * if it's 30 / 10 -> we want 2, so it's ok, 31 / 10 -> we want 3
 *
 * define SDRAM_RCDR_VAL  0x00000002
 */
#  define SDRAM_RCDR_VAL  ((SDRAM_tRCD-1)/SYS_CLK_PERIOD)

// prviously undefined
#  define SDRAM_WRTR_VAL  ((SDRAM_tWR+(SYS_CLK_PERIOD-1)/SYS_CLK_PERIOD)-2)
#  if SDRAM_WRTR_VAL<0
#    undef  SDRAM_WRTR_VAL
#    define SDRAM_WRTR_VAL 0
#  endif

// define SDRAM_RCTR_VAL  0x00000006
#  define SDRAM_RCTR_VAL  ((SDRAM_tRC+(SYS_CLK_PERIOD-1)/SYS_CLK_PERIOD)-2)
#  if SDRAM_RCTR_VAL<0
#    undef  SDRAM_RCTR_VAL
#    define SDRAM_RCTR_VAL 0
#  endif

// define SDRAM_REFCTR_VAL 0x00000006
#  define SDRAM_REFCTR_VAL ((SDRAM_tRFC+(SYS_CLK_PERIOD-1)/SYS_CLK_PERIOD)-2)
#  if SDRAM_REFCTR_VAL<0
#    undef  SDRAM_REFCTR_VAL
#    define SDRAM_REFCTR_VAL 0
#  endif

// define SDRAM_RATR_VAL  0x00000006
#  define SDRAM_RATR_VAL  ((SDRAM_tRAS+(SYS_CLK_PERIOD-1)/SYS_CLK_PERIOD)-2)
#  if SDRAM_RATR_VAL<0
#    undef  SDRAM_RATR_VAL
#    define SDRAM_RATR_VAL 0
#  endif

// define SDRAM_PTR_VAL   0x00000001
#  define SDRAM_PTR_VAL   (((SDRAM_tRP+(SYS_CLK_PERIOD-1))/SYS_CLK_PERIOD)-2)
#  if SDRAM_PTR_VAL<0
#    undef  SDRAM_PTR_VAL
#    define SDRAM_PTR_VAL 0
#  endif

// define SDRAM_RRDR_VAL  0x00000000
#  define SDRAM_RRDR_VAL  (((SDRAM_tRRD+(SYS_CLK_PERIOD-1))/SYS_CLK_PERIOD)-2)
#  if SDRAM_RRDR_VAL<0
#    undef  SDRAM_RRDR_VAL
#    define SDRAM_RRDR_VAL 0
#  endif

/* 
 * we don't want to go to the edge with refresh delays
 * define SDRAM_RIR_VAL   0x00000300
 */
#  define SDRAM_RIR_VAL   ((SDRAM_tREF/SYS_CLK_PERIOD)-((SDRAM_tREF/SYS_CLK_PERIOD)+10)/10)


#  define MC_BAR_0        (0x00)
#  define MC_AMR_0        (0x04)
#  define MC_BAR_1        (0x08)
#  define MC_AMR_1        (0x0c)
#  define MC_BAR_2        (0x10)
#  define MC_AMR_2        (0x14)
#  define MC_BAR_3        (0x18)
#  define MC_AMR_3        (0x1c)
#  define MC_CCR_0        (0x20)
#  define MC_CCR_1        (0x24)
#  define MC_CCR_2        (0x28)
#  define MC_CCR_3        (0x2c)
#  define MC_WTR_0        (0x30)
#  define MC_RTR_0        (0x34)
#  define MC_WTR_1        (0x38)
#  define MC_RTR_1        (0x3c)
#  define MC_WTR_2        (0x40)
#  define MC_RTR_2        (0x44)
#  define MC_WTR_3        (0x48)
#  define MC_RTR_3        (0x4c)

#  define MC_BAR_4        (0x80)
#  define MC_AMR_4        (0x84)
#  define MC_BAR_5        (0x88)
#  define MC_AMR_5        (0x8c)
#  define MC_BAR_6        (0x90)
#  define MC_AMR_6        (0x94)
#  define MC_BAR_7        (0x98)
#  define MC_AMR_7        (0x9c)
#  define MC_CCR_4        (0xa0)
#  define MC_CCR_5        (0xa4)
#  define MC_CCR_6        (0xa8)
#  define MC_CCR_7        (0xac)

#  define MC_RATR         (0xb0)  /* row active time register  */
#  define MC_RCTR         (0xb4)
#  define MC_RRDR         (0xb8)
#  define MC_PTR          (0xbc)
#  define MC_WRTR         (0xc0)
#  define MC_REFCTR       (0xc4)
#  define MC_RCDR         (0xc8)
#  define MC_RIR          (0xcc)
#  define MC_SMBOR        (0xe0)
#  define MC_ORR          (0xe4)
#  define MC_OSR          (0xe8)
#  define MC_PCR          (0xec)
#  define MC_IIR          (0xf0)

/* POC register field definition */
#  define MC_POC_EN_BW_OFFSET             0
#  define MC_POC_EN_BW_WIDTH              2
#  define MC_POC_EN_MEMTYPE_OFFSET        2
#  define MC_POC_EN_MEMTYPE_WIDTH         2

/* CSC register field definition */
#  define MC_CSC_EN_OFFSET                0
#  define MC_CSC_MEMTYPE_OFFSET           1
#  define MC_CSC_MEMTYPE_WIDTH            2
#  define MC_CSC_BW_OFFSET                4
#  define MC_CSC_BW_WIDTH                 2
#  define MC_CSC_MS_OFFSET                6
#  define MC_CSC_MS_WIDTH                 2
#  define MC_CSC_WP_OFFSET                8
#  define MC_CSC_BAS_OFFSET               9
#  define MC_CSC_KRO_OFFSET               10
#  define MC_CSC_PEN_OFFSET               11
#  define MC_CSC_SEL_OFFSET               16
#  define MC_CSC_SEL_WIDTH                8

#  define MC_CSC_MEMTYPE_SDRAM            0
#  define MC_CSC_MEMTYPE_SSRAM            1
#  define MC_CSC_MEMTYPE_ASYNC            2
#  define MC_CSC_MEMTYPE_SYNC             3

#  define MC_CSR_VALID                    0xFF000703LU
#  define MC_POC_VALID                    0x0000000FLU
#  define MC_BA_MASK_VALID                0x000003FFLU
#  define MC_CSC_VALID                    0x00FF0FFFLU
#  define MC_TMS_SDRAM_VALID              0x0FFF83FFLU
#  define MC_TMS_SSRAM_VALID              0x00000000LU
#  define MC_TMS_ASYNC_VALID              0x03FFFFFFLU
#  define MC_TMS_SYNC_VALID               0x01FFFFFFLU
#  define MC_TMS_VALID                    0xFFFFFFFFLU /* reg test compat. */

/* TMS register field definition SDRAM */
#  define MC_TMS_SDRAM_TRFC_OFFSET        24
#  define MC_TMS_SDRAM_TRFC_WIDTH         4
#  define MC_TMS_SDRAM_TRP_OFFSET         20
#  define MC_TMS_SDRAM_TRP_WIDTH          4
#  define MC_TMS_SDRAM_TRCD_OFFSET        17
#  define MC_TMS_SDRAM_TRCD_WIDTH         4
#  define MC_TMS_SDRAM_TWR_OFFSET         15
#  define MC_TMS_SDRAM_TWR_WIDTH          2
#  define MC_TMS_SDRAM_WBL_OFFSET         9
#  define MC_TMS_SDRAM_OM_OFFSET          7
#  define MC_TMS_SDRAM_OM_WIDTH           2
#  define MC_TMS_SDRAM_CL_OFFSET          4
#  define MC_TMS_SDRAM_CL_WIDTH           3
#  define MC_TMS_SDRAM_BT_OFFSET          3
#  define MC_TMS_SDRAM_BL_OFFSET          0
#  define MC_TMS_SDRAM_BL_WIDTH           3

/* TMS register field definition ASYNC */
#  define MC_TMS_ASYNC_TWWD_OFFSET        20
#  define MC_TMS_ASYNC_TWWD_WIDTH         6
#  define MC_TMS_ASYNC_TWD_OFFSET         16
#  define MC_TMS_ASYNC_TWD_WIDTH          4
#  define MC_TMS_ASYNC_TWPW_OFFSET        12
#  define MC_TMS_ASYNC_TWPW_WIDTH         4
#  define MC_TMS_ASYNC_TRDZ_OFFSET        8
#  define MC_TMS_ASYNC_TRDZ_WIDTH         4
#  define MC_TMS_ASYNC_TRDV_OFFSET        0
#  define MC_TMS_ASYNC_TRDV_WIDTH         8

/* TMS register field definition SYNC  */
#  define MC_TMS_SYNC_TTO_OFFSET          16
#  define MC_TMS_SYNC_TTO_WIDTH           9
#  define MC_TMS_SYNC_TWR_OFFSET          12
#  define MC_TMS_SYNC_TWR_WIDTH           4
#  define MC_TMS_SYNC_TRDZ_OFFSET         8
#  define MC_TMS_SYNC_TRDZ_WIDTH          4
#  define MC_TMS_SYNC_TRDV_OFFSET         0
#  define MC_TMS_SYNC_TRDV_WIDTH          8

#endif /* _MC_INIT_2_H_ */

