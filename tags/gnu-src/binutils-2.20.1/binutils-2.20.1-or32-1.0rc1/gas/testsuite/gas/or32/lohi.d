#as:
#objdump: -dr
#name: lohi

.*: +file format .*

Disassembly of section .text:

00000000 <l_lo>:
   0:	9c 21 be ef 	l.addi r1,r1,0xffffbeef

00000004 <l_hi>:
   4:	18 20 de ad 	l.movhi r1,0xdead
