/*
	Register usage
	R0 = 0
	R1 = pointer to destination
	R2 = nr of words to be copied
	R3 = data
	R4 = pointer to SPI core
	R5 = spi control word
	R6 = jump adr
	R7 = nr of control words
	r8 = adr pointer
*/
	.equ spi_base,0xb0000000 ;
	.equ spi_ctrl,0x0520	; 
	.equ read_cmd, 0x03000000 ;

	.global start		;
	.text
start:	
	l.movhi r0,0x0		;
	l.ori r1,r0,0x0	; # R1 = 0x0
	l.movhi r4,hi(spi_base)	; # R4 = spi base
	l.ori r5,r0,lo(spi_ctrl); # R5 = spi control

	# set SPI regs, can be removed if reset state is changed in IP core
	#l.ori r3,r0,0x420	;
	#l.sw 0x10(r4),r3	; # tx at negedge, 32 bit transfer
	#l.movhi r3,0x0300	;
	#l.ori r3,r3,0x0	;
	#l.sw 0x0(r4),r3	; # TX = 0x03000000
	#l.sw 0x14(r4),r0	; # prescaler to zero
	
	# set slave select active
	l.ori r3,r0,0x1		; # R3 = 0x1

	# initiate SPI FLASH READ
	l.jal read		;
	l.sw 0x18(r4),r3	; # ss active

	# read 
	l.jal read		;
	l.sw 0x0(r4),r0		; # set TX to zero
	l.or r2,r3,r3		; # R2 = nr of words

loop1:	l.jal read		; # read adr pointer
	l.addi r1,r1,0x8	;
	l.jal read		; # read data
	l.or r8,r3,r3		; # copy adr pointer to r8
	l.sfeq r8,r0		;
	l.bnf loop1		;
	l.sw 0x0(r8),r3		; # write control data
	
loop2:	l.jal read		;
	l.addi r1,r1,0x4	;
	l.sw 0x0(r1),r3		; # write to RAM
	l.sfeq r1,r2		;
	l.bnf loop2		;
	l.ori r6,r0,0x100	;
	l.jr r6			;
	# set slave select inactive
	l.sw 0x18(r4),r0	; # set slave select inactive
	
read:
	l.sw 0x10(r4),r5	; # write spi control
w4busy:	l.lwz r3,0x10(r4)	;
	l.sfeqi r3,lo(spi_ctrl)	;
	l.bf w4busy		;
	l.nop			; 
	l.jr r9			;
	l.lwz r3,0x0(r4)	; # R3 = RX
	