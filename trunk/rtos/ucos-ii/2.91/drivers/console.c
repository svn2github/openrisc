/* 
   Functions to control a console

   Very basic right now.

   Author: Julius Baxter, julius@opencores.org

*/

/*
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include "includes.h"

/* ANSI escape code values */
/* String these two together to create the control sequence indicator "ESC[" */
/* ASCII escape value, ESC*/
#define ESC 0x1b
/* Left square bracket '[' */
#define CSI 0x5b

#define PUT_CSI1(X) putc(ESC);putc(CSI);putc(X)
#define PUT_CSI2(X, Y) putc(ESC);putc(CSI);putc(X);putc(Y)

#define ASCII_DEC_OFFSET 0x30

static int cWidth, cHeight;

static void console_place_cursor(int x, int y);

void
console_init (int width, int height)
{
  /* Save width and height */
  cWidth = width; 
  cHeight = height;

  console_clear();

  /* Hide cursor - stops cursor block flickering about as the 
   console is updated. */
  putc(ESC);putc(CSI);putc('?');putc('2');putc('5');putc('l');

}

void
console_finish (void)
{
  console_clear();

  /* Finishing up - reset console to show cursor. */
  putc(ESC);putc(CSI);putc('?');putc('2');putc('5');putc('h');

}

void
console_clear (void)
{
  /* Erase whole display */
  PUT_CSI2('2','J');
  console_place_cursor(1,1);
}

static void
console_place_cursor (int x, int y)
{
  /* First convert Y to a decimal string */
  /* This is a hacky way of doing it. */
  int yHunds = 0;
  int yTens = 0;
  int yOnes = 0;
  
  while (y >= 100){
    y -= 100; yHunds ++;
  }
  while (y >= 10){
    y -= 10; yTens ++;
  }
  while (y > 0){
    y --; yOnes++;
  }

  /* This is a hacky way of doing it. */
  int xHunds = 0;
  int xTens = 0;
  int xOnes = 0;
  
  while (x >= 100){
    x -= 100; xHunds ++;
  }
  while (x >= 10){
    x -= 10; xTens ++;
  }
  while (x > 0){
    x --; xOnes++;
  }
  
  /* Output control sequence */
  putc(ESC);
  putc(CSI);
  if (yHunds) putc(yHunds + ASCII_DEC_OFFSET);
  if (yTens) putc(yTens + ASCII_DEC_OFFSET);
  putc(yOnes + ASCII_DEC_OFFSET);
  putc(';');
  if (xHunds) putc(xHunds + ASCII_DEC_OFFSET);
  if (xTens) putc(xTens + ASCII_DEC_OFFSET);
  putc(xOnes + ASCII_DEC_OFFSET);
  putc('f');
}

void
console_puts (int x, int y, char* str)
{
  int i;
  OS_CPU_SR cpu_sr;
  
  if ((y >= cHeight) || (x >= cWidth))
    return; /* Nothing to do, out of our range */

  /* Ensure this output doesn't get interrupted, as stuff will be spewed
     out to random places if the console control sequences are interrupted
  */

  OS_ENTER_CRITICAL()

  console_place_cursor(x, y);
  
  /* Print string contents, don't overflow line,
     and don't handle wrapping.*/
  while((str[i]!=0) && (i+y < cWidth))
	putc(str[i++]);

  /* Return cursor to 1,1 */
  console_place_cursor(1,1);

  OS_EXIT_CRITICAL()
  
}


void
console_putc (int x, int y, char c)
{
  OS_CPU_SR cpu_sr;

  if ((y >= cHeight) || (x >= cWidth))
    return; /* Nothing to do, out of our range */    

  /* Ensure this output doesn't get interrupted, as stuff will be spewed
     out to random places if the console control sequences are interrupted
  */

  OS_ENTER_CRITICAL()

  console_place_cursor(x, y);
  
  putc(c);

  /* Return cursor to 1,1 */
  console_place_cursor(1,1);

  OS_EXIT_CRITICAL()  
    
}

