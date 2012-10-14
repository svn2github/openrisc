#as:
#objdump: -dr
#name: store

.*: +file format .*

Disassembly of section .text:

00000000 <l_sw>:
   0:	d7 e1 0f fc 	l.sw 0xfffffffc\(r1\),r1

00000004 <l_lwz>:
   4:	84 21 ff 9c 	l.lwz r1,0xffffff9c\(r1\)
