#include <stdio.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/time.h>

/*
 * l.nop constants
 *
 */
#define NOP_NOP         0x0000      /* Normal nop instruction */
#define NOP_EXIT        0x0001      /* End of simulation */
#define NOP_REPORT      0x0002      /* Simple report */
#define NOP_PRINTF      0x0003      /* Simprintf instruction */
#define NOP_REPORT_FIRST 0x0400     /* Report with number */
#define NOP_REPORT_LAST 0x03ff      /* Report with number */


/*
 * getpid -- only one process, so just return 1.
 */
#define __MYPID 1
int
getpid()
{
	return __MYPID;
}


int
_DEFUN (gettimeofday, (ptimeval, ptimezone),
     struct timeval *ptimeval _AND
     void *ptimezone)
{
  return -1;
}


/*
 * kill -- go out via exit...
 */
int
kill(pid, sig)
     int pid;
     int sig;
{
	if(pid == __MYPID)
		_exit(sig);

	errno = ESRCH;
	
	return -1;
}

void 
_exit(int val)
{
  printf("Program completed with status code: %d.\n", val);
  /* Simulation exit signal */
  asm("l.add r3,r0,%0": : "r" (val));
  asm("l.nop %0": :"K" (NOP_EXIT));
  
  for(;;);
}
