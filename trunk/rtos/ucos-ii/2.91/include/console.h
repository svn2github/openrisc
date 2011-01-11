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

#ifndef _CONSOLE_H_
#define _CONSOLE_H_
void console_init(int width, int height);
void console_finish(void);
void console_puts(int x, int y, char* str);
void console_putc(int x, int y, char c);
void console_clear(void);
#endif

