#include <errno.h>
#include "uart.h"

// borrowed from unistd.h which cannot be included directly
// because it declares read() and write() as external functions.
#define STDIN_FILENO    0       /* standard input file descriptor */
#define STDOUT_FILENO   1       /* standard output file descriptor */
#define STDERR_FILENO   2       /* standard error file descriptor */


/*
 * Enable AUTO_ECHO to have the uart read function
 * echo back every character it reads make it
 * suitible for use with terminal emulator programs.
 */
//#define AUTO_ECHO
#undef AUTO_ECHO

int errno;

/*
 * read  -- read bytes from the UART.
 */
int
_DEFUN (read, (file, ptr, len),
        int   fd  _AND
        char *buf   _AND
        int   nbytes)
{
  return 0;				/* Minimal implementation */
	int i;

	if(fd != STDIN_FILENO) {
		errno = EBADF;
			
		return -1;
	}

	for (i = 0; i < nbytes; i++) {
		*(buf + i) = uart_getc();
#ifdef AUTO_ECHO
		uart_putc( *(buf + i) );
#endif
		if ((*(buf + i) == '\n') || (*(buf + i) == '\r')) {
			(*(buf + i)) = 0;
			break;
		}
	}
	
	return i;
}


#define NOP_PUTC        0x0004      /* JPB: Simputc instruction */

/*
 * write -- write bytes to the UART.
 */
int
_DEFUN (write, (file, buf, len),
        int   file  _AND
        char *buf   _AND
        int   nbytes)
{
	int i;
	
	if(file != STDOUT_FILENO && file != STDERR_FILENO) {
		errno = EBADF;
			
		return -1;
	}

	for (i = 0; i < nbytes; i++) {
	  __asm__ __volatile__ ("\tl.add\tr3,r0,%0" : : "r" (buf[i]));
	  __asm__ __volatile__ ("\tl.nop\t%0" : : "K" (NOP_PUTC));


		/* if (*(buf + i) == '\n') { */
		/* 	uart_putc ('\r'); */
		/* } */
		/* uart_putc (*(buf + i)); */
	}
	
	return nbytes;
}

/*
 * open -- open a file descriptor. We don't have a filesystem, so
 *         we return an error.
 */
int
_DEFUN (open, (file, flags, mode),
        char *file  _AND
        int   flags _AND
        int   mode)
{
	errno = ENOSYS;
	
	return -1;
}

/*
 * close -- close a file descriptor. 
 */
int
_DEFUN (close, (file),
        int file)
{
	if(file == STDIN_FILENO  ||
	   file == STDERR_FILENO ||
	   file == STDOUT_FILENO)
	   	errno = EIO;
	else
		errno = EBADF;
	
	return -1;
}


/*
 * isatty -- returns 1 if connected to a terminal device,
 *           returns 0 if not. We class the serial port
 *           as a terminal.
 */
int
_DEFUN (isatty, (file),
        int file)
{
	return (file == STDIN_FILENO  ||
	        file == STDERR_FILENO ||
	        file == STDOUT_FILENO) ? 1 : 0;
}

/*
 * lseek -- move read/write pointer. Serial port
 *          is non-seekable, so we return an error.
 */
long
lseek(fd,  offset, whence)
     int fd;
     long offset;
     int whence;
{
	if(fd == STDIN_FILENO  ||
	   fd == STDERR_FILENO ||
	   fd == STDOUT_FILENO)
	   	errno = ESPIPE;
	else
		errno = EBADF;

	return ((long)-1);
}

/*
 * fstat -- get status of a file. Since we have no file
 *          system, we just return an error.
 */
int
fstat(fd, buf)
     int fd;
     struct stat *buf;
{
	if(fd == STDIN_FILENO  ||
	   fd == STDERR_FILENO ||
	   fd == STDOUT_FILENO)
	   	errno = EIO;
	else
		errno = EBADF;

	return (-1);
}


