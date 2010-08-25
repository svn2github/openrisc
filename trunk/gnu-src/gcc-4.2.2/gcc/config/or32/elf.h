/* Definitions for rtems targeting an OpenRisc OR32 using COFF
   Copyright (C) 1996, 1997, 2005 Free Software Foundation, Inc.
   Contributed by Joel Sherrill (joel@OARcorp.com).

This file is part of GNU CC.

GNU CC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GNU CC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU CC; see the file COPYING.  If not, write to
the Free Software Foundation, 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.  */

#undef  TARGET_VERSION
#define TARGET_VERSION  fputs (" (OR32/ELF)", stderr);

/* Use ELF */
#undef  OBJECT_FORMAT_ELF
#define OBJECT_FORMAT_ELF

/* Offer DBX (STABS) debugging info */
#undef  DBX_DEBUGGING_INFO
#define DBX_DEBUGGING_INFO

/* Also offer DWARF2 debugging info */
#undef  DWARF2_DEBUGGING_INFO
#define DWARF2_DEBUGGING_INFO

/* Make DBX (STABS) the default debugging info */
#undef  PREFERRED_DEBUGGING_TYPE
#define PREFERRED_DEBUGGING_TYPE DWARF2_DEBUG

/* JPB: Make this match or32.h */
#undef  USER_LABEL_PREFIX
#define USER_LABEL_PREFIX ""
