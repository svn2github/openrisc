/* Support file for c based tests */
#include "spr_defs.h"
#include "board.h"
#include "mc.h"
	
	.global		_stack_top
	.section	.vectors, "ax"

	.org    0x100
_reset_vector:
	l.nop
	l.nop
	l.addi  r2,r0,0x0
	l.addi  r3,r0,0x0
	l.addi  r4,r0,0x0
	l.addi  r5,r0,0x0
	l.addi  r6,r0,0x0
	l.addi  r7,r0,0x0
	l.addi  r8,r0,0x0
	l.addi  r9,r0,0x0
	l.addi  r10,r0,0x0
	l.addi  r11,r0,0x0
	l.addi  r12,r0,0x0
	l.addi  r13,r0,0x0
	l.addi  r14,r0,0x0
	l.addi  r15,r0,0x0
	l.addi  r16,r0,0x0
	l.addi  r17,r0,0x0
	l.addi  r18,r0,0x0
	l.addi  r19,r0,0x0
	l.addi  r20,r0,0x0
	l.addi  r21,r0,0x0
	l.addi  r22,r0,0x0
	l.addi  r23,r0,0x0
	l.addi  r24,r0,0x0
	l.addi  r25,r0,0x0
	l.addi  r26,r0,0x0
	l.addi  r27,r0,0x0
	l.addi  r28,r0,0x0
	l.addi  r29,r0,0x0
	l.addi  r30,r0,0x0
	l.addi  r31,r0,0x0
	
	l.movhi r3,hi(_start)
	l.ori   r3,r3,lo(_start)
	l.jr    r3
	l.nop
	
.org 0x200
 
_except_200:
	l.nop
    l.addi  r1,r1,-116				// free 29 words of stack (stack is r1)
    l.sw    0x18(r1),r9				// save register r9(return addr) to stack
    l.jal   store_regs				// save registers r3-r31 (except r9) to stack (r9 is changed here)
    l.nop

    l.movhi r9,hi(end_except)		// set return addr to end_except instruction
    l.ori   r9,r9,lo(end_except)	// set return addr to end_except instruction
	l.j	buserr_except
	l.nop

.org 0x300
 
_except_300:
	l.nop
    l.addi  r1,r1,-116				// free 29 words of stack (stack is r1)
    l.sw    0x18(r1),r9				// save register r9(return addr) to stack
    l.jal   store_regs				// save registers r3-r31 (except r9) to stack (r9 is changed here)
    l.nop

    l.movhi r9,hi(end_except)		// set return addr to end_except instruction
    l.ori   r9,r9,lo(end_except)	// set return addr to end_except instruction
	l.j	dpf_except
	l.nop

.org 0x400
 
_except_400:
	l.nop
    l.addi  r1,r1,-116				// free 29 words of stack (stack is r1)
    l.sw    0x18(r1),r9				// save register r9(return addr) to stack
    l.jal   store_regs				// save registers r3-r31 (except r9) to stack (r9 is changed here)
    l.nop

    l.movhi r9,hi(end_except)		// set return addr to end_except instruction
    l.ori   r9,r9,lo(end_except)	// set return addr to end_except instruction
	l.j	ipf_except
	l.nop

.org 0x500
 
_except_500:
	l.nop
	l.j	vTickHandler
	l.nop

.org 0x600
 
_except_600:
	l.nop
    l.addi  r1,r1,-116				// free 29 words of stack (stack is r1)
    l.sw    0x18(r1),r9				// save register r9(return addr) to stack
    l.jal   store_regs				// save registers r3-r31 (except r9) to stack (r9 is changed here)
    l.nop

    l.movhi r9,hi(end_except)		// set return addr to end_except instruction
    l.ori   r9,r9,lo(end_except)	// set return addr to end_except instruction
	l.j	align_except
	l.nop

.org 0x700
 
_except_700:
	l.nop
    l.addi  r1,r1,-116				// free 29 words of stack (stack is r1)
    l.sw    0x18(r1),r9				// save register r9(return addr) to stack
    l.jal   store_regs				// save registers r3-r31 (except r9) to stack (r9 is changed here)
    l.nop

    l.movhi r9,hi(end_except)		//set return addr to end_except instruction
    l.ori   r9,r9,lo(end_except)	//set return addr to end_except instruction
	l.j	illegal_except
	l.nop

.org 0x800
 
_except_800:
	l.nop
    l.addi  r1,r1,-116			//free 29 words of stack (stack is r1)
    l.sw    0x18(r1),r9			//save register r9(return addr) to stack
    l.jal   store_regs			//save registers r3-r31 (except r9) to stack (r9 is changed here)
    l.nop

    l.movhi r9,hi(end_except)		//set return addr to end_except instruction
    l.ori   r9,r9,lo(end_except)	//set return addr to end_except instruction
    l.j ext_except			    //jmp to C interrupt handler (returns later to end_except)
    l.nop


.org 0x900
 
_except_900:
	l.nop
    l.addi  r1,r1,-116			//free 29 words of stack (stack is r1)
    l.sw    0x18(r1),r9			//save register r9(return addr) to stack
    l.jal   store_regs			//save registers r3-r31 (except r9) to stack (r9 is changed here)
    l.nop

    l.movhi r9,hi(end_except)		//set return addr to end_except instruction
    l.ori   r9,r9,lo(end_except)	//set return addr to end_except instruction
	l.j	dtlbmiss_except
	l.nop

.org 0xa00
 
_except_a00:
	l.nop
    l.addi  r1,r1,-116			//free 29 words of stack (stack is r1)
    l.sw    0x18(r1),r9			//save register r9(return addr) to stack
    l.jal   store_regs			//save registers r3-r31 (except r9) to stack (r9 is changed here)
    l.nop

    l.movhi r9,hi(end_except)		//set return addr to end_except instruction
    l.ori   r9,r9,lo(end_except)	//set return addr to end_except instruction
	l.j	itlbmiss_except
	l.nop

.org 0xb00
 
_except_b00:
	l.nop
    l.addi  r1,r1,-116			//free 29 words of stack (stack is r1)
    l.sw    0x18(r1),r9			//save register r9(return addr) to stack
    l.jal   store_regs			//save registers r3-r31 (except r9) to stack (r9 is changed here)
    l.nop

    l.movhi r9,hi(end_except)		//set return addr to end_except instruction
    l.ori   r9,r9,lo(end_except)	//set return addr to end_except instruction
	l.j	range_except
	l.nop

.org 0xc00
 
_except_c00:
	.global		PortCC
	l.nop
	l.sfeqi	r11, 0x0FCC
	l.bnf	1f
	l.nop
	l.j PortCC
	l.nop

1:
	l.addi	r1, r1, 4			//FIXME
    l.addi  r1,r1,-116			//free 29 words of stack (stack is r1), FIXME comment
	l.sw	-0x8(r1), r11
	l.lwz	r11, 112(r1)
    l.sw    0x18(r1),r9			//save register r9(return addr) to stack
    l.jal   store_regs			//save registers r3-r31 (except r9) to stack (r9 is changed here)
    l.nop

	l.lwz	r3, -0x8(r1)	
    l.movhi r9,hi(end_except)		//set return addr to end_except instruction
    l.ori   r9,r9,lo(end_except)	//set return addr to end_except instruction
	l.j	syscall_except
	l.nop

.org 0xd00
 
_except_d00:
	l.nop
    l.addi  r1,r1,-116			//free 29 words of stack (stack is r1)
    l.sw    0x18(r1),r9			//save register r9(return addr) to stack
    l.jal   store_regs			//save registers r3-r31 (except r9) to stack (r9 is changed here)
    l.nop

    l.movhi r9,hi(end_except)		//set return addr to end_except instruction
    l.ori   r9,r9,lo(end_except)	//set return addr to end_except instruction
	l.j	res1_except
	l.nop

.org 0xe00
 
_except_e00:
	l.nop
    l.addi  r1,r1,-116			//free 29 words of stack (stack is r1)
    l.sw    0x18(r1),r9			//save register r9(return addr) to stack
    l.jal   store_regs			//save registers r3-r31 (except r9) to stack (r9 is changed here)
    l.nop

    l.movhi r9,hi(end_except)		//set return addr to end_except instruction
    l.ori   r9,r9,lo(end_except)	//set return addr to end_except instruction
	l.j	trap_except
	l.nop

.org 0xf00
 
_except_f00:
	l.nop
    l.addi  r1,r1,-116			//free 29 words of stack (stack is r1)
    l.sw    0x18(r1),r9			//save register r9(return addr) to stack
    l.jal   store_regs			//save registers r3-r31 (except r9) to stack (r9 is changed here)
    l.nop

    l.movhi r9,hi(end_except)		//set return addr to end_except instruction
    l.ori   r9,r9,lo(end_except)	//set return addr to end_except instruction
	l.j	res2_except
	l.nop

store_regs:		//save registers r3-r31 (except r9) to stack
        l.sw    0x00(r1),r3
        l.sw    0x04(r1),r4
        l.sw    0x08(r1),r5
        l.sw    0x0c(r1),r6
        l.sw    0x10(r1),r7
        l.sw    0x14(r1),r8
        l.sw    0x1c(r1),r10
        l.sw    0x20(r1),r11
        l.sw    0x24(r1),r12
        l.sw    0x28(r1),r13
        l.sw    0x2c(r1),r14
        l.sw    0x30(r1),r15
        l.sw    0x34(r1),r16
        l.sw    0x38(r1),r17
        l.sw    0x3c(r1),r18
        l.sw    0x40(r1),r19
        l.sw    0x44(r1),r20
        l.sw    0x48(r1),r21
        l.sw    0x4c(r1),r22
        l.sw    0x50(r1),r23
        l.sw    0x54(r1),r24
        l.sw    0x58(r1),r25
        l.sw    0x5c(r1),r26
        l.sw    0x60(r1),r27
        l.sw    0x64(r1),r28
        l.sw    0x68(r1),r29
        l.sw    0x6c(r1),r30
        l.sw    0x70(r1),r31
        l.jr    r9
        l.nop

end_except:		//load back registers from stack r3-r31
        l.lwz   r3,0x00(r1)
        l.lwz   r4,0x04(r1)
        l.lwz   r5,0x08(r1)
        l.lwz   r6,0x0c(r1)
        l.lwz   r7,0x10(r1)
        l.lwz   r8,0x14(r1)
        l.lwz   r9,0x18(r1)
        l.lwz   r10,0x1c(r1)
        l.lwz   r11,0x20(r1)
        l.lwz   r12,0x24(r1)
        l.lwz   r13,0x28(r1)
        l.lwz   r14,0x2c(r1)
        l.lwz   r15,0x30(r1)
        l.lwz   r16,0x34(r1)
        l.lwz   r17,0x38(r1)
        l.lwz   r18,0x3c(r1)
        l.lwz   r19,0x40(r1)
        l.lwz   r20,0x44(r1)
        l.lwz   r21,0x48(r1)
        l.lwz   r22,0x4c(r1)
        l.lwz   r23,0x50(r1)
        l.lwz   r24,0x54(r1)
        l.lwz   r25,0x58(r1)
        l.lwz   r26,0x5c(r1)
        l.lwz   r27,0x60(r1)
        l.lwz   r28,0x64(r1)
        l.lwz   r29,0x68(r1)
        l.lwz   r30,0x6c(r1)
        l.lwz   r31,0x70(r1)
        l.addi  r1,r1,116				//free stack places
        l.rfe							//recover SR register and prior PC (jumps back to program)
        l.nop


	.section .text

_start:
.if IC | DC
	/* Flush IC and/or DC */
	l.addi  r10,r0,0   
	l.addi  r11,r0,0   
	l.addi  r12,r0,0   
.if IC
	l.addi  r11,r0,IC_SIZE
.endif
.if DC
	l.addi	r12,r0,DC_SIZE
.endif
	l.sfleu	r12,r11
	l.bf	loop
	l.nop
	l.add	r11,r0,r12
loop:
.if IC
	l.mtspr r0,r10,SPR_ICBIR
.endif
.if DC
	l.mtspr r0,r10,SPR_DCBIR
.endif
	l.sfne  r10,r11
	l.bf    loop   
	l.addi  r10,r10,16

	/* Enable IC and/or DC */
	l.addi  r10,r0,(SPR_SR_SM)
.if IC
	l.ori	r10,r10,(SPR_SR_ICE)
.endif
.if DC
	l.ori	r10,r10,(SPR_SR_DCE)
.endif
	l.mtspr r0,r10,SPR_SR
	l.nop
	l.nop
	l.nop
	l.nop
	l.nop
.endif
	
	/* Set stack pointer */
	l.movhi r1, hi(_stack_top)
	l.ori   r1, r1, lo(_stack_top)

	/* clear BSS */
	l.movhi	r2, hi(_bss_beg)
	l.ori	r2, r2, lo(_bss_beg)
	l.movhi	r3, hi(_bss_end)
	l.ori	r3, r2, lo(_bss_end)
1:
	l.sfeq	r2, r3
	l.bf	__main
	l.noP

	l.sw 	0x0(r2), r0
	l.addi	r2, r2, 0x4
	l.j		1b
	l.nop

	/* Jump to main */
__main:
	l.movhi r2,hi(_main)
	l.ori   r2,r2,lo(_main)
	l.jr    r2
	l.nop
