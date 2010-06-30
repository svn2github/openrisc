/* Header for GDB Simulator wrapper for Or1ksim

   Copyright 1988-2008, Free Software Foundation, Inc.
   Copyright (C) 2010 Embecosm Limited

   Contributor Jeremy Bennett <jeremy.bennett@embecosm.com>

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the Free
   Software Foundation; either version 3 of the License, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
   FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
   more details.

   You should have received a copy of the GNU General Public License along
   with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/*---------------------------------------------------------------------------*/
/* This is a wrapper for Or1ksim, suitable for use as a GDB simulator.

   The code tries to follow the GDB coding style.

   Commenting is Doxygen compatible.                                         */
/*---------------------------------------------------------------------------*/

/*! GDB signal indiating breakpoint hit */
#define TARGET_SIGNAL_TRAP  5

/*! Number of registers */
#define  OR32_MAX_GPRS  32

/* Particular registers */
#define OR32_FIRST_ARG_REGNUM  3			/*!< First arg reg */
#define OR32_PPC_REGNUM        (OR32_MAX_GPRS + 0)	/*!< Previous PC */
#define OR32_NPC_REGNUM        (OR32_MAX_GPRS + 1)	/*!< Next PC */
#define OR32_SR_REGNUM         (OR32_MAX_GPRS + 2)	/*!< Supervision Reg */


/* ------------------------------------------------------------------------- */
/*!A structure to hold the state of a simulation instance.

   This is the typedef SIM_DESC.

   The entries are
   - the socket descriptor for reading/writing from/to the model process
   - a flag which is true if we are used for debug rather than standalone
   - the callback function supplied to the sim_open () function
   - this simulator's name (argv[0] supplied to sim_open ()
   - the PID of the child process running the model (0 if we are the child)  */
/* ------------------------------------------------------------------------- */
struct sim_state
{
  int                          sockd;
  int                          is_debug;
  struct host_callback_struct *callback;
  char                        *myname;
  int                          pid;
};


