/*
    ctype.c -- character types
    Implements the usual ctype stuff (only valid for ASCII systems)
    Copyright (C) 2002 Richard Herveille, rherveille@opencores.org

    This file is part of OpenRISC 1000 Reference Platform Monitor (ORPmon)

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include <ctype.h>

const unsigned char __ctype_table[256] =
{
  /* only the first 128 characters are really defined                   0   1   2   3    4   5   6   7  */
       0, __CT_c,  __CT_c,  __CT_c,  __CT_c,  __CT_c,  __CT_c,  __CT_c, /*0  NUL SOH STX ETX  EOT ENQ ACK BEL */
  __CT_c, __CT_b,  __CT_s,  __CT_b,  __CT_s,  __CT_s,  __CT_c,  __CT_c, /*1  BS  HT  LF  VT   FF  CR  SO  SI  */
  __CT_c, __CT_c,  __CT_c,  __CT_c,  __CT_c,  __CT_c,  __CT_c,  __CT_c, /*2  DLE DC1 DC2 DC3  DC4 NAK SYN ETB */
  __CT_c, __CT_c,  __CT_c,  __CT_c,  __CT_c,  __CT_c,  __CT_c,  __CT_c, /*3  CAN EM  SUB ESC  FS  GS  RS  US  */
  __CT_b, __CT_p,  __CT_p,  __CT_p,  __CT_p,  __CT_p,  __CT_p,  __CT_p, /*4  SP  !   "   #    $   %   &   '   */
  __CT_p, __CT_p,  __CT_p,  __CT_p,  __CT_p,  __CT_p,  __CT_p,  __CT_p, /*5  (   )   *   +    ,   -   .   /   */
  __CT_d, __CT_d,  __CT_d,  __CT_d,  __CT_d,  __CT_d,  __CT_d,  __CT_d, /*6  0   1   2   3    4   5   6   7   */
  __CT_d, __CT_d,  __CT_p,  __CT_p,  __CT_p,  __CT_p,  __CT_p,  __CT_p, /*7  8   9   :   ;    <   =   >   ?   */
  __CT_p, __CT_ux, __CT_ux, __CT_ux, __CT_ux, __CT_ux, __CT_ux, __CT_u, /*8  @   A   B   C    D   E   F   G   */
  __CT_u, __CT_u,  __CT_u,  __CT_u,  __CT_u,  __CT_u,  __CT_u,  __CT_u, /*9  H   I   J   K    L   M   N   O   */
  __CT_u, __CT_u,  __CT_u,  __CT_u,  __CT_u,  __CT_u,  __CT_u,  __CT_u, /*a  P   Q   R   S    T   U   V   W   */
  __CT_u, __CT_u,  __CT_u,  __CT_p,  __CT_p,  __CT_p,  __CT_p,  __CT_p, /*b  X   Y   Z   [    \   ]   ^   _   */
  __CT_p, __CT_lx, __CT_lx, __CT_lx, __CT_lx, __CT_lx, __CT_lx, __CT_l, /*c  `   a   b   c    d   e   f   g   */
  __CT_l, __CT_l,  __CT_l,  __CT_l,  __CT_l,  __CT_l,  __CT_l,  __CT_l, /*d  h   i   j   k    l   m   n   o   */
  __CT_l, __CT_l,  __CT_l,  __CT_l,  __CT_l,  __CT_l,  __CT_l,  __CT_l, /*e  p   q   r   s    t   u   v   w   */
  __CT_l, __CT_l,  __CT_l,  __CT_p,  __CT_p,  __CT_p,  __CT_p,  __CT_c, /*f  x   y   z   {    |   }   ~   DEL */

  /* The other 128 characters are system dependant          */
  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,

  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0
};
