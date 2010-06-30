#include <errno.h>

/*
 * While the heap grows upwards, the stack grows downwards.
 * Evnetually these two things may colide and sbrk()
 * won't even be able to return properly. To mitigate this
 * we reserve upto STACK_BUFFER _words_ at the top of memory.
 * Note this doesn't actually solve the problem, it just
 * provides an error margin. The real solution is to use
 * an OS with a proper virtual memory manager.
 */
#define STACK_BUFFER 16384

#ifndef NULL
#define NULL ((void *)0)
#endif


/*
 * sbrk -- changes heap size size. Get nbytes more
 *         RAM. We just increment a pointer in what's
 *         left of memory on the board.
 */
void *sbrk(int nbytes)
{
	/* symbols defined by linker map */
	extern int _end;   // start of free memory
	extern int _stack; // end of free memory
	
	static unsigned long *heap_ptr = NULL;
	void *base;
	int nwords = (nbytes + 3) / 4;

	if (heap_ptr == NULL)
		heap_ptr = (unsigned long *)&_end;

	if ( ((unsigned long *)&_stack - (heap_ptr + nwords)) > STACK_BUFFER ) {
		base = heap_ptr;
		heap_ptr += nwords;
		
		return (void *)base;
	} else {
		errno = ENOMEM;
//		write(1, "Failed to extend heap.", 23);
		
		return (void *)-1;
	}
}
