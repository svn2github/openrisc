#include "spr-defs.h"
#include "board.h"

#define TRAP_ON_ERROR 0
#define LOOP_ON_ERROR 0
#define EXIT_NOP_ON_ERROR 1
#define PRINT_AND_RESET_ON_ERROR 1
	
        .extern _src_beg
        .extern _dst_beg
        .extern _dst_end        
        .extern int_main
	.extern int_error
        .extern tick_interrupt
        .extern _crc32
        .extern _bstart
	.extern _bend
        .global _calc_mycrc32
        .global _mycrc32
        .global _mysize

        .section .stack, "aw", @nobits
.space  STACK_SIZE
_stack:
        .section .crc
_mycrc32:
        .word   0xcccccccc
_mysize:
        .word 0xdddddddd

.if SELF_CHECK
_calc_mycrc32:
        l.addi  r3,r0,0
        l.movhi r4,hi(_calc_mycrc32)
        l.ori   r4,r4,lo(_calc_mycrc32)
        l.movhi r5,hi(_mysize)
        l.ori   r5,r5,lo(_mysize)
        l.lwz   r5,0(r5)
        l.addi  r1,r1,-4
        l.sw    0(r1),r9
        
        /* unsigned long crc32 (unsigned long crc, const unsigned char *buf, unsigned long len); */
        l.jal           _crc32
        l.nop
                                
        l.movhi r3,hi(_mycrc32)
        l.ori   r3,r3,lo(_mycrc32)
        l.lwz   r3,0(r3)
        
        l.xor     r11,r3,r11
        l.lwz   r9,0(r1)
        l.jr    r9
        l.addi  r1,r1,4
.endif

        .org 0x100

        .section .reset, "ax"


_reset:
	l.movhi r0, 0
	/* Clear status register, set supervisor mode */
	l.ori r1, r0, SPR_SR_SM
	l.mtspr r0, r1, SPR_SR
	/* Clear timer  */
	l.mtspr r0, r0, SPR_TTMR
	/* Jump to start routine */
        l.movhi r3,hi(_start)
        l.ori   r3,r3,lo(_start)
        l.jr    r3
        l.nop

        .section .vectors, "ax"
        .org 0x200

_buserr:
.if TRAP_ON_ERROR
	/* Just trap */	
	l.trap 0
.endif	
.if EXIT_NOP_ON_ERROR
	l.nop 0x1
.endif
.if LOOP_ON_ERROR
	l.j 0
	l.nop
.endif
.if PRINT_AND_RESET_ON_ERROR	
	l.mfspr r4, r0, SPR_EPCR_BASE
	l.j 	_int_error /* This will reset */
	l.ori 	r3, r0, 0x2
.endif	
        .section .vectors, "ax"
        .org 0x500

_tickint:
#define TIMER_RELOAD_VALUE (SPR_TTMR_IE | SPR_TTMR_RT | ((IN_CLK/TICKS_PER_SEC) & SPR_TTMR_PERIOD))
	/* Simply load timer_ticks variable and increment */
	.extern _timer_ticks
	l.addi 	r1, r1, -136 /* 128 + what we need (8),avoid area used by gcc*/
	l.sw 	0(r1), r25	
	l.sw 	4(r1), r26
	l.movhi r25, hi(timestamp)
	l.ori 	r25, r25, lo(timestamp)
	l.lwz 	r26, 0(r25)			/* Load variable addr.*/
	l.addi 	r26, r26, 1			/* Increment variable */
	l.sw 	0(r25), r26			/* Store variable */
	l.movhi	r25, hi(TIMER_RELOAD_VALUE) 	/* Load timer value */
	l.ori 	r25, r25, lo(TIMER_RELOAD_VALUE)
	l.mtspr r0, r25, SPR_TTMR 		/* Reset timer */
	l.lwz 	r25, 0(r1)
	l.lwz	r26, 4(r1)
	l.addi 	r1, r1, 136
	l.rfe

        .section .vectors, "ax"
        .org 0x600

_alignerr:	
.if TRAP_ON_ERROR
	/* Just trap */	
	l.trap 0
.endif	
.if EXIT_NOP_ON_ERROR
	l.nop 0x1
.endif
.if LOOP_ON_ERROR
	l.j 0
	l.nop
.endif
.if PRINT_AND_RESET_ON_ERROR		
	l.mfspr r4, r0, SPR_EPCR_BASE
	l.j _int_error /* This will reset */
	l.ori r3, r0, 0x6
.endif	
	
        .org 0x700
_illinsn:
.if TRAP_ON_ERROR
	/* Just trap */	
	l.trap 0
.endif	
.if EXIT_NOP_ON_ERROR
	l.nop 0x1
.endif
.if LOOP_ON_ERROR
	l.j 0
	l.nop
.endif
.if PRINT_AND_RESET_ON_ERROR		
	l.mfspr r4, r0, SPR_EPCR_BASE
	l.j _int_error /* This will reset */
	l.ori r3, r0, 0x7
.endif
        .org 0x800

_userint:
        l.addi  r1,r1,-256  /*(128 + 128) */
        l.sw    0x0(r1),r2
	l.addi 	r2, r1, 256
	l.sw	0x4(r1), r3
        l.movhi r3,hi(_int_wrapper)
        l.ori   r3,r3,lo(_int_wrapper)
        l.jr    r3
        l.nop

        .section .text
_start:
        /* Copy form flash to sram */
        l.movhi r3,hi(_src_beg)
        l.ori   r3,r3,lo(_src_beg)
        l.movhi r4,hi(_vec_start)
        l.ori   r4,r4,lo(_vec_start)
        l.movhi r5,hi(_vec_end)
        l.ori   r5,r5,lo(_vec_end)
        l.sub   r5,r5,r4
        l.sfeqi r5,0
        l.bf    2f
        l.nop
1:      l.lwz   r6,0(r3)
        l.sw    0(r4),r6
        l.addi  r3,r3,4
        l.addi  r4,r4,4
        l.addi  r5,r5,-4
        l.sfgtsi r5,0
        l.bf    1b
        l.nop
2:
        l.movhi r4,hi(_dst_beg)
        l.ori   r4,r4,lo(_dst_beg)
        l.movhi r5,hi(_dst_end)
        l.ori   r5,r5,lo(_dst_end)
1:      l.sfgeu r4,r5
        l.bf    1f
        l.nop
        l.lwz   r8,0(r3)
        l.sw    0(r4),r8
        l.addi  r3,r3,4
        l.bnf   1b
        l.addi  r4,r4,4 
1:
        l.addi  r3,r0,0
        l.addi  r4,r0,0
3:

	/* Instruction cache enable */
	/* Check if IC present and skip enabling otherwise */
	l.mfspr r24,r0,SPR_UPR
	l.andi  r26,r24,SPR_UPR_ICP
	l.sfeq  r26,r0
	l.bf    .L8
	l.nop
	
	/* Disable IC */
	l.mfspr r6,r0,SPR_SR
	l.addi  r5,r0,-1
	l.xori  r5,r5,SPR_SR_ICE
	l.and   r5,r6,r5
	l.mtspr r0,r5,SPR_SR
	
	/* Establish cache block size
	If BS=0, 16;
	If BS=1, 32;
	r14 contain block size
	*/
	l.mfspr r24,r0,SPR_ICCFGR
	l.andi  r26,r24,SPR_ICCFGR_CBS
	l.srli  r28,r26,7
	l.ori   r30,r0,16
	l.sll   r14,r30,r28
	
	/* Establish number of cache sets
	r16 contains number of cache sets
	r28 contains log(# of cache sets)
	*/
	l.andi  r26,r24,SPR_ICCFGR_NCS
	l.srli  r28,r26,3
	l.ori   r30,r0,1
	l.sll   r16,r30,r28
	
	/* Invalidate IC */
	l.addi  r6,r0,0
	l.sll   r5,r14,r28
	
.L7:
	l.mtspr r0,r6,SPR_ICBIR
	l.sfne  r6,r5
	l.bf    .L7
	l.add   r6,r6,r14
	
	/* Enable IC */
	l.mfspr r6,r0,SPR_SR
	l.ori   r6,r6,SPR_SR_ICE
	l.mtspr r0,r6,SPR_SR
	l.nop
	l.nop
	l.nop
	l.nop
	l.nop
	l.nop
	l.nop
	l.nop

.L8:
	/* Data cache enable */
        /* Check if DC present and skip enabling otherwise */
        l.mfspr r24,r0,SPR_UPR
        l.andi  r26,r24,SPR_UPR_DCP
        l.sfeq  r26,r0
        l.bf    .L10
        l.nop
        /* Disable DC */
        l.mfspr r6,r0,SPR_SR
        l.addi  r5,r0,-1
        l.xori  r5,r5,SPR_SR_DCE
	l.and   r5,r6,r5
        l.mtspr r0,r5,SPR_SR
        /* Establish cache block size
           If BS=0, 16;
           If BS=1, 32;
           r14 contain block size
        */
        l.mfspr r24,r0,SPR_DCCFGR
        l.andi  r26,r24,SPR_DCCFGR_CBS
        l.srli  r28,r26,7
        l.ori   r30,r0,16
        l.sll   r14,r30,r28
        /* Establish number of cache sets
           r16 contains number of cache sets
           r28 contains log(# of cache sets)
        */
	l.andi  r26,r24,SPR_DCCFGR_NCS
	l.srli  r28,r26,3
        l.ori   r30,r0,1
        l.sll   r16,r30,r28
        /* Invalidate DC */
        l.addi  r6,r0,0
        l.sll   r5,r14,r28
.L9:
        l.mtspr r0,r6,SPR_DCBIR
        l.sfne  r6,r5
        l.bf    .L9
	l.add   r6,r6,r14
        /* Enable DC */
        l.mfspr r6,r0,SPR_SR
        l.ori   r6,r6,SPR_SR_DCE
        l.mtspr r0,r6,SPR_SR

.L10:
	/* Set up stack */
        l.movhi r1,hi(_stack-4)
        l.ori   r1,r1,lo(_stack-4)
        l.addi  r2,r0,-3
        l.and   r1,r1,r2

	
        l.movhi r3,hi(main)
        l.ori   r3,r3,lo(main)
        l.jr    r3
	l.nop

_int_wrapper:

        l.sw    0x8(r1), r4
        l.sw    0xc(r1), r5
        l.sw    0x10(r1), r6
        l.sw    0x14(r1), r7
        l.sw    0x18(r1), r8
        l.sw    0x1c(r1), r9
        l.sw    0x20(r1), r10
        l.sw    0x24(r1), r11
        l.sw    0x28(r1), r12
        l.sw    0x2c(r1), r13
        l.sw    0x30(r1), r14
        l.sw    0x34(r1), r15
        l.sw    0x38(r1), r16
        l.sw    0x3c(r1), r17
        l.sw    0x40(r1), r18
        l.sw    0x44(r1), r19
        l.sw    0x48(r1), r20
        l.sw    0x4c(r1), r21
        l.sw    0x50(r1), r22
        l.sw    0x54(r1), r23
        l.sw    0x58(r1), r24
        l.sw    0x5c(r1), r25
        l.sw    0x60(r1), r26
        l.sw    0x64(r1), r27
        l.sw    0x68(r1), r28
        l.sw    0x6c(r1), r29
        l.sw    0x70(r1), r30
        l.sw    0x74(r1), r31
	
        l.movhi r3,hi(int_main)
        l.ori   r3,r3,lo(int_main)
        l.jalr  r3
        l.nop

	l.lwz   r3,0x4(r1)
        l.lwz   r4,0x8(r1)
        l.lwz   r5,0xc(r1)
        l.lwz   r6,0x10(r1)
        l.lwz   r7,0x14(r1)
        l.lwz   r8,0x18(r1)
        l.lwz   r9,0x1c(r1)
        l.lwz   r10,0x20(r1)
        l.lwz   r11,0x24(r1)
        l.lwz   r12,0x28(r1)
        l.lwz   r13,0x2c(r1)
        l.lwz   r14,0x30(r1)
        l.lwz   r15,0x34(r1)
        l.lwz   r16,0x38(r1)
        l.lwz   r17,0x3c(r1)
        l.lwz   r18,0x40(r1)
        l.lwz   r19,0x44(r1)
        l.lwz   r20,0x48(r1)
        l.lwz   r21,0x4c(r1)
        l.lwz   r22,0x50(r1)
        l.lwz   r23,0x54(r1)
        l.lwz   r24,0x58(r1)
        l.lwz   r25,0x5c(r1)
        l.lwz   r26,0x60(r1)
        l.lwz   r27,0x64(r1)
        l.lwz   r28,0x68(r1)
        l.lwz   r29,0x6c(r1)
        l.lwz   r30,0x70(r1)
	l.lwz   r31,0x74(r1)	

	l.lwz  	r2, 0x0(r1)
        l.addi  r1,r1,256
        l.rfe
        l.nop
	

	/* Jump to error function. Clobber r2 */
_int_error:
	l.movhi r2,hi(int_error)
        l.ori   r2,r2,lo(int_error)
        l.jr  r2
        l.nop
