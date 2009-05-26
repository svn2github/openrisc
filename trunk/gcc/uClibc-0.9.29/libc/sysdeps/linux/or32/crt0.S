/*
 */

/* Stick in a dummy reference to main(), so that if an application
 * is linking when the main() function is in a static library (.a)
 * we can be sure that main() actually gets linked in */
.type _main,%function

.global __start
.global _start	
__start:
_start:	
	l.addi	r3,r1,0
	l.j	__uClibc_start
	l.nop
