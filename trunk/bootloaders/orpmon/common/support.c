/* Support */

#include "spr-defs.h"
#include "support.h"
#include "common.h"
#include "int.h"

#include <ctype.h>

volatile unsigned long timestamp = 0;

void int_main(void);

/* return value by making a syscall */
void exit (int i)
{
  asm("l.add r3,r0,%0": : "r" (i));
  asm("l.nop %0": :"K" (NOP_EXIT));
  while (1);
}

/* activate printf support in simulator */
void __printf(const char *fmt, ...)
{
#if 0
  va_list args;
  va_start(args, fmt);
  __asm__ __volatile__ ("  l.addi\tr3,%1,0\n \
                           l.addi\tr4,%2,0\n \
                           l.nop %0": :"K" (NOP_PRINTF), "r" (fmt), "r"  (args) : "r3", "r4");
#endif
}

/* print long */
void report(unsigned long value)
{
  asm("l.addi\tr3,%0,0": :"r" (value));
  asm("l.nop %0": :"K" (NOP_REPORT));
}

/* just to satisfy linker */
void __main(void)
{
}

/* For writing into SPR. */
void mtspr(unsigned long spr, unsigned long value)
{
  asm("l.mtspr\t\t%0,%1,0": : "r" (spr), "r" (value));
}

/* For reading SPR. */
unsigned long mfspr(unsigned long spr)
{
  unsigned long value;
  asm("l.mfspr\t\t%0,%1,0" : "=r" (value) : "r" (spr));
  return value;
}



/* Parses hex or decimal number */
unsigned long strtoul (const char *str, char **endptr, int base)
{

  {

    unsigned long number = 0;
    char *pos = (char *) str;
    char *fail_char = (char *) str;


  while (isspace(*pos)) pos++;	/* skip leading whitespace */

  if ((base == 16) && (*pos == '0')) { /* handle option prefix */
    ++pos;
    fail_char = pos;
    if ((*pos == 'x') || (*pos == 'X')) ++pos;
  }

  if (base == 0) {		/* dynamic base */
    base = 10;		/* default is 10 */
    if (*pos == '0') {
      ++pos;
      base -= 2;		/* now base is 8 (or 16) */
      fail_char = pos;
      if ((*pos == 'x') || (*pos == 'X')) {
        base += 8;	/* base is 16 */
        ++pos;
      }
    }
  }

  /* check for illegal base */
  if ( !((base < 2) || (base > 36)) )
    while (1) {
      int digit = 40;
      if ((*pos >= '0') && (*pos <= '9')) {
          digit = (*pos - '0');
      } else if (*pos >= 'a') {
          digit = (*pos - 'a' + 10);
      } else if (*pos >= 'A') {
          digit = (*pos - 'A' + 10);
      } else break;

      if (digit >= base) break;

      fail_char = ++pos;
      number = number * base + digit;
    }

  if (endptr) *endptr = fail_char; {
    return number;
  }
  }
}
 
unsigned long get_timer (unsigned long base)
{
  /*
__printf("%s - %s: %d\n", __FILE__, __FUNCTION__, __LINE__);
__printf("   timestamp = %.8lx base = %.8lx\n", timestamp, base);
  */
  return (timestamp - base);
}
 
void set_timer (unsigned long t)
{
  timestamp = t;
}

