/* Simple setjmp/longjmp for the OpenRISC 1000 (OR32 ISA).
   Damjan Lampret, OpenCores.org, Aug 15 2000.  */

/* Until OR1K Arch is fixed, we just save entire register file. Should be fixed eventually. */

	.align	4
	.proc	setjmp
        .global	setjmp
	.extern setjmp
setjmp:
	l.sw	0(r3),r1
	l.sw	4(r3),r2
	l.sw	8(r3),r3
	l.sw	12(r3),r4
	l.sw	16(r3),r5
	l.sw	20(r3),r6
	l.sw	24(r3),r7
	l.sw	28(r3),r8
	l.sw	32(r3),r9
	l.sw	36(r3),r10
	l.sw	40(r3),r11
	l.sw	44(r3),r12
	l.sw	48(r3),r13
	l.sw	52(r3),r14
	l.sw	56(r3),r15
	l.sw	60(r3),r16
	l.sw	64(r3),r17
	l.sw	68(r3),r18
	l.sw	72(r3),r19
	l.sw	76(r3),r20
	l.sw	80(r3),r21
	l.sw	84(r3),r22
	l.sw	88(r3),r23
	l.sw	92(r3),r24
	l.sw	96(r3),r25
	l.sw	100(r3),r26
	l.sw	104(r3),r27
	l.sw	108(r3),r28
	l.sw	112(r3),r29
	l.sw	116(r3),r30
	l.sw	120(r3),r31
/*
        l.addi  r4,r0,SPR_SR
        l.mfsr  r4,r4
        l.sw    124(r3),r4
*/	
	l.jr	r11
	l.addi	r3,r0,0
.endproc setjmp

	.align	4
	.proc	longjmp
        .global	longjmp
longjmp:
	l.lw	r1,0(r3)
	l.lw	r2,4(r3)
	l.lw	r5,16(r3)
	l.lw	r6,20(r3)
	l.lw	r7,24(r3)
	l.lw	r8,28(r3)
	l.lw	r9,32(r3)
	l.lw	r10,36(r3)
	l.lw	r11,40(r3)
	l.lw	r12,44(r3)
	l.lw	r13,48(r3)
	l.lw	r14,52(r3)
	l.lw	r15,56(r3)
	l.lw	r16,60(r3)
	l.lw	r17,64(r3)
	l.lw	r18,68(r3)
	l.lw	r19,72(r3)
	l.lw	r20,76(r3)
	l.lw	r21,80(r3)
	l.lw	r22,84(r3)
	l.lw	r23,88(r3)
	l.lw	r24,92(r3)
	l.lw	r25,96(r3)
	l.lw	r26,100(r3)
	l.lw	r27,104(r3)
	l.lw	r28,108(r3)
	l.lw	r29,112(r3)
	l.lw	r30,116(r3)
	l.lw	r31,120(r3)
/*
        l.lw    r5,124(r3)
        l.addi  r3,r0,SPR_ESR_BASE
        l.mtsr  r3,r5
*/	
	l.lw	r5,16(r3)
	l.sfne	r4,r0
	l.bf	1f
	l.nop

	l.addi	r3,r0,1
	
1:	l.addi	r3,r4,0
	l.jr	r11
	l.nop
.endproc longjmp
