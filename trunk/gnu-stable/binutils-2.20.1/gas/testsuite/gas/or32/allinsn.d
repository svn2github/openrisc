#as:
#objdump: -dr
#name: allinsn

.*: +file format .*

Disassembly of section .text:

00000000 <l_j>:
   0:	00 00 00 00 	l.j 0 <l_j>

00000004 <l_jal>:
   4:	04 00 00 00 	l.jal 4 <l_jal>

00000008 <l_jr>:
   8:	44 00 00 00 	l.jr r0

0000000c <l_jalr>:
   c:	48 00 00 00 	l.jalr r0

00000010 <l_bnf>:
  10:	0c 00 00 00 	l.bnf 10 <l_bnf>

00000014 <l_bf>:
  14:	10 00 00 00 	l.bf 14 <l_bf>

00000018 <l_trap>:
  18:	21 00 00 00 	l.trap 0x0

0000001c <l_rfe>:
  1c:	24 00 00 00 	l.rfe 

00000020 <l_sys>:
  20:	20 00 00 00 	l.sys 0x0

00000024 <l_nop>:
  24:	15 00 00 00 	l.nop 0x0

00000028 <l_movhi>:
  28:	18 00 00 00 	l.movhi r0,0x0

0000002c <l_mfspr>:
  2c:	b4 00 00 00 	l.mfspr r0,r0,0x0

00000030 <l_mtspr>:
  30:	c0 00 00 00 	l.mtspr r0,r0,0x0

00000034 <l_lwz>:
  34:	84 00 00 00 	l.lwz r0,0x0\(r0\)

00000038 <l_lws>:
  38:	88 00 00 00 	l.lws r0,0x0\(r0\)

0000003c <l_lbz>:
  3c:	8c 00 00 00 	l.lbz r0,0x0\(r0\)

00000040 <l_lbs>:
  40:	90 00 00 00 	l.lbs r0,0x0\(r0\)

00000044 <l_lhz>:
  44:	94 00 00 00 	l.lhz r0,0x0\(r0\)

00000048 <l_lhs>:
  48:	98 00 00 00 	l.lhs r0,0x0\(r0\)

0000004c <l_sw>:
  4c:	d4 00 00 00 	l.sw 0x0\(r0\),r0

00000050 <l_sb>:
  50:	d8 00 00 00 	l.sb 0x0\(r0\),r0

00000054 <l_sh>:
  54:	dc 00 00 00 	l.sh 0x0\(r0\),r0

00000058 <l_sll>:
  58:	e0 00 00 08 	l.sll r0,r0,r0

0000005c <l_slli>:
  5c:	b8 00 00 00 	l.slli r0,r0,0x0

00000060 <l_srl>:
  60:	e0 00 00 48 	l.srl r0,r0,r0

00000064 <l_srli>:
  64:	b8 00 00 40 	l.srli r0,r0,0x0

00000068 <l_sra>:
  68:	e0 00 00 88 	l.sra r0,r0,r0

0000006c <l_srai>:
  6c:	b8 00 00 80 	l.srai r0,r0,0x0

00000070 <l_ror>:
  70:	e0 00 00 c8 	l.ror r0,r0,r0

00000074 <l_rori>:
  74:	b8 00 00 c0 	l.rori r0,r0,0x0

00000078 <l_add>:
  78:	e0 00 00 00 	l.add r0,r0,r0

0000007c <l_addi>:
  7c:	9c 00 00 00 	l.addi r0,r0,0x0

00000080 <l_sub>:
  80:	e0 00 00 02 	l.sub r0,r0,r0

00000084 <l_and>:
  84:	e0 00 00 03 	l.and r0,r0,r0

00000088 <l_andi>:
  88:	a4 00 00 00 	l.andi r0,r0,0x0

0000008c <l_or>:
  8c:	e0 00 00 04 	l.or r0,r0,r0

00000090 <l_ori>:
  90:	a8 00 00 00 	l.ori r0,r0,0x0

00000094 <l_xor>:
  94:	e0 00 00 05 	l.xor r0,r0,r0

00000098 <l_xori>:
  98:	ac 00 00 00 	l.xori r0,r0,0x0

0000009c <l_mul>:
  9c:	e0 00 03 06 	l.mul r0,r0,r0

000000a0 <l_muli>:
  a0:	b0 00 00 00 	l.muli r0,r0,0x0

000000a4 <l_div>:
  a4:	e0 00 03 09 	l.div r0,r0,r0

000000a8 <l_divu>:
  a8:	e0 00 03 0a 	l.divu r0,r0,r0

000000ac <l_sfgts>:
  ac:	e5 40 00 00 	l.sfgts r0,r0

000000b0 <l_sfgtu>:
  b0:	e4 40 00 00 	l.sfgtu r0,r0

000000b4 <l_sfges>:
  b4:	e5 60 00 00 	l.sfges r0,r0

000000b8 <l_sfgeu>:
  b8:	e4 60 00 00 	l.sfgeu r0,r0

000000bc <l_sflts>:
  bc:	e5 80 00 00 	l.sflts r0,r0

000000c0 <l_sfltu>:
  c0:	e4 80 00 00 	l.sfltu r0,r0

000000c4 <l_sfles>:
  c4:	e5 a0 00 00 	l.sfles r0,r0

000000c8 <l_sfleu>:
  c8:	e4 a0 00 00 	l.sfleu r0,r0

000000cc <l_sfgtsi>:
  cc:	bd 40 00 00 	l.sfgtsi r0,0x0

000000d0 <l_sfgtui>:
  d0:	bc 40 00 00 	l.sfgtui r0,0x0

000000d4 <l_sfgesi>:
  d4:	bd 60 00 00 	l.sfgesi r0,0x0

000000d8 <l_sfgeui>:
  d8:	bc 60 00 00 	l.sfgeui r0,0x0

000000dc <l_sfltsi>:
  dc:	bd 80 00 00 	l.sfltsi r0,0x0

000000e0 <l_sfltui>:
  e0:	bc 80 00 00 	l.sfltui r0,0x0

000000e4 <l_sflesi>:
  e4:	bd a0 00 00 	l.sflesi r0,0x0

000000e8 <l_sfleui>:
  e8:	bc a0 00 00 	l.sfleui r0,0x0

000000ec <l_sfeq>:
  ec:	e4 00 00 00 	l.sfeq r0,r0

000000f0 <l_sfeqi>:
  f0:	bc 00 00 00 	l.sfeqi r0,0x0

000000f4 <l_sfne>:
  f4:	e4 20 00 00 	l.sfne r0,r0

000000f8 <l_sfnei>:
  f8:	bc 20 00 00 	l.sfnei r0,0x0
