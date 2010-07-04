/* Support file for or32 tests.  This file should is included
   in each test. It calls main() function and add support for
   basic functions */

#ifndef SUPPORT_H
#define SUPPORT_H

#include <stdarg.h>
#include <stddef.h>
#include <limits.h>

#include "string.h"

/* Register access macros */
#define REG8(add) *((volatile unsigned char *)(add))
#define REG16(add) *((volatile unsigned short *)(add))
#define REG32(add) *((volatile unsigned long *)(add))

/* For writing into SPR. */
void mtspr(unsigned long spr, unsigned long value);

/* For reading SPR. */
unsigned long mfspr(unsigned long spr);

/* Function to be called at entry point - not defined here.  */
int main (int, char **);

/* Prints out a value */
void report(unsigned long value);

/* Calculates a 32-bit CRC */
unsigned long crc32 (unsigned long crc, const unsigned char *buf, unsigned long len);

/* return value by making a syscall */
extern void exit (int i) __attribute__ ((__noreturn__));

/* some stdlib functions */

/* defined in 'string.h'
extern void *memcpy (void *dest, const void *src, unsigned long n);
extern void *memmove (void *dest, const void *src, unsigned long n);
int memcmp (void *dstvoid, const void *srcvoid, unsigned long length);
extern void *memset (void * dstvoid, const char data, unsigned long length);
extern void *memchr(const void *s, int c, unsigned long n);
extern int strlen (const char *src);
extern int strcmp (const char *s1, const char *s2);
extern char *strcpy (char *dst0, char *src0);
*/
unsigned long strtoul (const char *str, char **endptr, int base);

/* defined in 'ctype.h'
#define isspace(c) ((c) == ' ' || (c) == '\t')
*/

extern volatile unsigned long timestamp;
extern void reset_timer (void);
extern unsigned long get_timer (unsigned long base);
extern void set_timer (unsigned long t);
extern void sleep(unsigned long sleep_time);

#endif
