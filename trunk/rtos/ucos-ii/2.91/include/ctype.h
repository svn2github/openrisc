/*
    ctype.h -- character types
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


#ifndef __CTYPE_H
#define __CTYPE_H

/* basic types                                              */
#define __CT_d 0x01               /* numeric digit          */
#define __CT_u 0x02               /* upper case             */
#define __CT_l 0x04               /* lower case             */
#define __CT_c 0x08               /* control character      */
#define __CT_s 0x10               /* whitespace             */
#define __CT_p 0x20               /* punctuation            */
#define __CT_x 0x40               /* hexadecimal            */
#define __CT_b 0x80               /* blank (is also space)  */

/* combination types                                        */
#define __CT_lx (__CT_l | __CT_x) /* lower case hexadecimal */
#define __CT_ux (__CT_u | __CT_x) /* upper case hexadecimal */

#define __CT_space    (__CT_s | __CT_b)
#define __CT_alphanum (__CT_l | __CT_u | __CT_d)
#define __CT_graph    (__CT_l | __CT_u | __CT_d | __CT_p)
#define __CT_print    (__CT_l | __CT_u | __CT_d | __CT_p | __CT_b)

extern const unsigned char __ctype_table[256];

#define _toupper(c)      ( (c) ^ 0x20 )
#define _tolower(c)      ( (c) ^ 0x20 )
#define toupper(c)       ( islower(c) ? _tolower(c) : (c) )
#define tolower(c)       ( isupper(c) ? _toupper(c) : (c) )
#define toascii(c)       ( (c) & 0x7F )


/* standard defenitions are taken from man-pages            */
/*#define isalnum(c)       ( isalpha(c) || isdigit(c) )     */
#define isalnum(c)       ( __ctype_table[(int) c] & __CT_alphanum )
/*#define isalpha(c)       ( isupper(c) || islower(c) )     */
#define isalpha(c)       ( __ctype_table[(int) c] & __CT_ul )
#define isascii(c)       ( (c) & ~0x7F )
#define isblank(c)       ( __ctype_table[(int) c] & __CT_b )
#define iscntrl(c)       ( __ctype_table[(int) c] & __CT_c )
#define isdigit(c)       ( __ctype_table[(int) c] & __CT_d )
#define isgraph(c)       ( __ctype_table[(int) c] & __CT_graph )
#define islower(c)       ( __ctype_table[(int) c] & __CT_l )
#define isprint(c)       ( __ctype_table[(int) c] & __CT_print)
#define ispunct(c)       ( __ctype_table[(int) c] & __CT_p )
#define isspace(c)       ( __ctype_table[(int) c] & __CT_space )
#define isupper(c)       ( __ctype_table[(int) c] & __CT_u )
#define isxdigit(c)      ( __ctype_table[(int) c] & __CT_x )

#endif /* __CTYPE_H */
