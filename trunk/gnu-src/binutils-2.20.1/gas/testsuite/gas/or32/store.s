	.text
	.global l_sw
l_sw:
	l.sw	-4(r1), r1
	.global	l_lwz
l_lwz:	
	l.lwz	r1, -100(r1)
