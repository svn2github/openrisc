#include "port_spr_defs.h"
	
	.file		"portasm.S"
	.section	.text

.text
.global	vPortDisableInterrupts
.type	vPortDisableInterrupts, %function
vPortDisableInterrupts:
	l.mfspr	r3, r0, SPR_SR		# get current SR			
	l.addi	r4, r0, SPR_SR_TEE	
	l.xori	r4, r4, 0xffffffff						
	l.and	r3, r3, r4			# disable Tick Timer Interrupt
	l.addi	r4, r0, SPR_SR_IEE	
	l.xori	r4, r4, 0xffffffff						
	l.and	r3, r3, r4			# disable External Interrupt
	l.mtspr	r0, r3, SPR_SR		# update SR					
	l.jr	r9
	l.nop
.size	vPortDisableInterrupts, .-vPortDisableInterrupts


.text
.global	vPortEnableInterrupts
.type	vPortEnableInterrupts, %function
vPortEnableInterrupts:
	l.mfspr	r3, r0, SPR_SR		# get current SR			
	l.ori	r3, r3, SPR_SR_TEE	# enable Tick Timer Interrup
	l.ori	r3, r3, SPR_SR_IEE	# enable External Interrupt	
	l.mtspr	r0, r3, SPR_SR		# update SR					
	l.jr	r9
	l.nop
.size	vPortEnableInterrupts, .-vPortEnableInterrupts


.macro portSAVE_REGISTER
	l.addi	r1, r1, -116			
	l.sw    0x00(r1), r3
	l.sw    0x04(r1), r4
	l.sw    0x08(r1), r5
	l.sw    0x0c(r1), r6
	l.sw    0x10(r1), r7
	l.sw    0x14(r1), r8
	l.sw    0x18(r1), r9
	l.sw    0x1c(r1), r10
	l.sw    0x20(r1), r11
	l.sw    0x24(r1), r12
	l.sw    0x28(r1), r13
	l.sw    0x2c(r1), r14
	l.sw    0x30(r1), r15
	l.sw    0x34(r1), r16
	l.sw    0x38(r1), r17
	l.sw    0x3c(r1), r18
	l.sw    0x40(r1), r19
	l.sw    0x44(r1), r20
	l.sw    0x48(r1), r21
	l.sw    0x4c(r1), r22
	l.sw    0x50(r1), r23
	l.sw    0x54(r1), r24
	l.sw    0x58(r1), r25
	l.sw    0x5c(r1), r26
	l.sw    0x60(r1), r27
	l.sw    0x64(r1), r28
	l.sw    0x68(r1), r29
	l.sw    0x6c(r1), r30
	l.sw    0x70(r1), r31
.endm


.macro portRESTORE_REGISTER
	l.lwz	r3 , 0x00(r1)
	l.lwz	r4 , 0x04(r1)
	l.lwz	r5 , 0x08(r1)
	l.lwz	r6 , 0x0c(r1)
	l.lwz	r7 , 0x10(r1)
	l.lwz	r8 , 0x14(r1)
	l.lwz	r9 , 0x18(r1)
	l.lwz	r10, 0x1c(r1)
	l.lwz	r11, 0x20(r1)
	l.lwz	r12, 0x24(r1)
	l.lwz	r13, 0x28(r1)
	l.lwz	r14, 0x2c(r1)
	l.lwz	r15, 0x30(r1)
	l.lwz	r16, 0x34(r1)
	l.lwz	r17, 0x38(r1)
	l.lwz	r18, 0x3c(r1)
	l.lwz	r19, 0x40(r1)
	l.lwz	r20, 0x44(r1)
	l.lwz	r21, 0x48(r1)
	l.lwz	r22, 0x4c(r1)
	l.lwz	r23, 0x50(r1)
	l.lwz	r24, 0x54(r1)
	l.lwz	r25, 0x58(r1)
	l.lwz	r26, 0x5c(r1)
	l.lwz	r27, 0x60(r1)
	l.lwz	r28, 0x64(r1)
	l.lwz	r29, 0x68(r1)
	l.lwz	r30, 0x6c(r1)
	l.lwz	r31, 0x70(r1)
	l.addi	r1, r1, 116			
.endm


.macro portSAVE_CONTEXT
	.global	pxCurrentTCB			
	# make rooms in stack			
	l.addi	r1, r1, -128			
	# early save r3-r5, these are clobber register
	l.sw	0x08(r1), r3			
	l.sw	0x0C(r1), r4			
	l.sw	0x10(r1), r5			
	# save SPR_ESR_BASE(0), SPR_EPCR_BASE(0)
	l.mfspr	r3, r0, SPR_ESR_BASE	
	l.mfspr	r4, r0, SPR_EPCR_BASE	
	l.sw	0x78(r1), r3			
	l.sw	0x7C(r1), r4			
	l.sw	0x00(r1), r9			
	# Save Context					
	l.sw	0x04(r1), r2			
	l.sw	0x14(r1), r6			
	l.sw	0x18(r1), r7			
	l.sw	0x1C(r1), r8			
	l.sw	0x20(r1), r10			
	l.sw	0x24(r1), r11			
	l.sw	0x28(r1), r12			
	l.sw	0x2C(r1), r13			
	l.sw	0x30(r1), r14			
	l.sw	0x34(r1), r15			
	l.sw	0x38(r1), r16			
	l.sw	0x3C(r1), r17			
	l.sw	0x40(r1), r18			
	l.sw	0x44(r1), r19			
	l.sw	0x48(r1), r20			
	l.sw	0x4C(r1), r21			
	l.sw	0x50(r1), r22			
	l.sw	0x54(r1), r23			
	l.sw	0x58(r1), r24			
	l.sw	0x5C(r1), r25			
	l.sw	0x60(r1), r26			
	l.sw	0x64(r1), r27			
	l.sw	0x68(r1), r28			
	l.sw	0x6C(r1), r29			
	l.sw	0x70(r1), r30			
	l.sw	0x74(r1), r31			
	# Save the top of stack in TCB	
	l.movhi	r3, hi(pxCurrentTCB)	
	l.ori	r3, r3, lo(pxCurrentTCB)
	l.lwz	r3, 0x0(r3)				
	l.sw	0x0(r3), r1				
	# restore clobber register		
	l.lwz	r3, 0x08(r1)			
	l.lwz	r4, 0x0C(r1)			
	l.lwz	r5, 0x10(r1)		
.endm


.macro portRESTORE_CONTEXT
	l.movhi	r3, hi(pxCurrentTCB)	
	l.ori	r3, r3, lo(pxCurrentTCB)
	l.lwz	r3, 0x0(r3)				
	l.lwz	r1, 0x0(r3)				
	# restore context				
	l.lwz	r9, 0x00(r1)			
	l.lwz	r2, 0x04(r1)			
	l.lwz	r6, 0x14(r1)			
	l.lwz	r7, 0x18(r1)			
	l.lwz	r8, 0x1C(r1)			
	l.lwz	r10, 0x20(r1)			
	l.lwz	r11, 0x24(r1)			
	l.lwz	r12, 0x28(r1)			
	l.lwz	r13, 0x2C(r1)			
	l.lwz	r14, 0x30(r1)			
	l.lwz	r15, 0x34(r1)			
	l.lwz	r16, 0x38(r1)			
	l.lwz	r17, 0x3C(r1)			
	l.lwz	r18, 0x40(r1)			
	l.lwz	r19, 0x44(r1)			
	l.lwz	r20, 0x48(r1)			
	l.lwz	r21, 0x4C(r1)			
	l.lwz	r22, 0x50(r1)			
	l.lwz	r23, 0x54(r1)			
	l.lwz	r24, 0x58(r1)			
	l.lwz	r25, 0x5C(r1)			
	l.lwz	r26, 0x60(r1)			
	l.lwz	r27, 0x64(r1)			
	l.lwz	r28, 0x68(r1)			
	l.lwz	r29, 0x6C(r1)			
	l.lwz	r30, 0x70(r1)			
	l.lwz	r31, 0x74(r1)			
	# restore SPR_ESR_BASE(0), SPR_EPCR_BASE(0)
	l.lwz	r3, 0x78(r1)			
	l.lwz	r4, 0x7C(r1)			
	l.mtspr	r0, r3, SPR_ESR_BASE	
	l.mtspr	r0, r4, SPR_EPCR_BASE	
	# restore clobber register		
	l.lwz	r3, 0x08(r1)			
	l.lwz	r4, 0x0C(r1)			
	l.lwz	r5, 0x10(r1)			
	l.addi	r1, r1, 128			
	l.rfe							
	l.nop							
.endm


.text
.global	vPortTickHandler
.type	vPortTickHandler, %function
vPortTickHandler:
	portSAVE_CONTEXT	
		
	l.mfspr	r3, r0, SPR_TTMR
	l.movhi	r4, hi(SPR_TTMR_IP)	
	l.ori	r4, r4, lo(SPR_TTMR_IP)
	l.xori	r4, r4, 0xffffffff
	l.and	r3, r3, r4				# clear tick timer interrupt
	l.mtspr	r0, r3, SPR_TTMR

	l.jal	vTaskIncrementTick
	l.nop
.if configUSE_PREEMPTION == 0
	# do nothing
.else
	l.jal	vTaskSwitchContext
	l.nop
.endif

	portRESTORE_CONTEXT
.size	vPortTickHandler, .-vPortTickHandler


.text
.global	vPortSystemCall
.type	vPortSystemCall, %function
vPortSystemCall:
	l.sfeqi	r11, 0xFCC
	l.bf	1f
	l.nop
	
	l.sw	-120(r1), r11	
	l.lwz	r11, -4(r1)
	portSAVE_REGISTER

	l.jal	syscall_except
	l.lwz	r3, -4(r1)

	portRESTORE_REGISTER
	l.rfe
	l.nop

1:
	l.sw	-132(r1), r11	
	l.lwz	r11, -4(r1)
	portSAVE_CONTEXT	
	
	l.jal	syscall_except
	l.lwz	r3, -4(r1)
	
	portRESTORE_CONTEXT
.size	vPortSystemCall, .-vPortSystemCall


.text
.global	vPortExtIntHandler
.type	vPortExtIntHandler, %function
vPortExtIntHandler:
	portSAVE_CONTEXT	

    l.jal	int_main
	l.nop

.if configUSE_PREEMPTION == 0
	# do nothing
.else
	l.jal	vTaskSwitchContext
	l.nop
.endif
	
	portRESTORE_CONTEXT
.size	vPortExtIntHandler, .-vPortExtIntHandler


.text
.global	vPortMiscIntHandler
.type	vPortMiscIntHandler, %function
vPortMiscIntHandler:
	portSAVE_CONTEXT	

    l.jal	misc_int_handler
	l.lwz	r3, -4(r1)

.if configUSE_PREEMPTION == 0
	# do nothing
.else
	l.jal	vTaskSwitchContext
	l.nop
.endif
	
	portRESTORE_CONTEXT
.size	vPortMiscIntHandler, .-vPortMiscIntHandler
