/* GNU/Linux/or32 specific low level interface, for the remote server for GDB.
   Copyright (C) 2005, 2007, 2008, 2009, 2010 Free Software Foundation, Inc.
   Copyright (C) 2010 Embecosm Limited

   Contributor Jeremy Bennett <jeremy.bennett@embecosm.com>

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* -------------------------------------------------------------------------- */
/* OpenRISC 1000 specific low level Linux interface for GDB server.

   This implementation includes full Doxygen compatible documentation         */
/* -------------------------------------------------------------------------- */

#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif

#include "server.h"
#include "regdef.h"
#include "linux-low.h"

#include <asm/ptrace.h> /* For openrisc kernel ptrace register offsets */

/* Defined in auto-generated file reg-or32.c.  */
void init_registers_or32 (void);

#ifdef HAVE_SYS_REG_H
#include <sys/reg.h>
#endif


/*! Some useful GDB register numbers. */
#define GDB_REGNUM_R0    0
#define GDB_REGNUM_R31  31
#define GDB_REGNUM_PPC  32
#define GDB_REGNUM_NPC  33
#define GDB_REGNUM_SR   34

/*! This is the number of *GDB* registers. I.e. r0-r31, PPC, NPC and SR.  */
#define or32_num_regs  (GDB_REGNUM_SR + 1)


/* -------------------------------------------------------------------------- */
/*!Provide the ptrace "address" of a register.

   This must be in GDB order (r0-r1, ppc, npc, sr) to ptrace offset. As with
   regs_or32, we substitute r31 for r0 and NPC for PPC.                       */
/* -------------------------------------------------------------------------- */
static int or32_regmap[] = {
  GPR31, SP,    GPR2,  GPR3,  GPR4,  GPR5,  GPR6,  GPR7,
  GPR8,  GPR9,  GPR10, GPR11, GPR12, GPR13, GPR14, GPR15,
  GPR16, GPR17, GPR18, GPR19, GPR20, GPR21, GPR22, GPR23,
  GPR24, GPR25, GPR26, GPR27, GPR28, GPR29, GPR30, GPR31,
  PC,    PC,    SR
};


/* -------------------------------------------------------------------------- */
/*!Predicate to indicate if a register can be read.

   We mark r0 as not readable.

   @param[in] regno  Register to read.

   @return  Non-zero (TRUE) if the register can be read, zero (FALSE)
            otherwise.                                                        */
/* -------------------------------------------------------------------------- */
static int
or32_cannot_fetch_register (int  regno)
{
  return (GDB_REGNUM_R0 == regno) || (regno >= or32_num_regs);

}	/* or32_cannot_fetch_register () */


/* -------------------------------------------------------------------------- */
/*!Predicate to indicate if a register can be written.

   We mark r0 and the SR as not writeable.

   @param[in] regno  Register to write.

   @return  Non-zero (TRUE) if the register can be written, zero (FALSE)
            otherwise.                                                        */
/* -------------------------------------------------------------------------- */
static int
or32_cannot_store_register (int  regno)
{
  return (GDB_REGNUM_R0 == regno) || (GDB_REGNUM_SR == regno) ||
         (regno >= or32_num_regs);

}	/* or32_cannot_store_register () */


/* -------------------------------------------------------------------------- */
/*!Get the current program counter.

   On the OR32, this is NPC, the *next* program counter.

   @param[in] regcache  Current register cache.

   @return  The value of the NPC.                                             */
/* -------------------------------------------------------------------------- */
static CORE_ADDR
or32_get_pc (struct regcache *regcache)
{
  unsigned long int  npc;
  collect_register_by_name (regcache, "npc", &npc);  

    if (debug_threads)
    {
      fprintf (stderr, "stop pc is %08lx\n", npc);
    }
  
  return  npc;

}	/* or32_get_pc () */


/* -------------------------------------------------------------------------- */
/*!Set the current program counter.

   On the OR32, this is NPC, the *next* program counter.

   @param[in] regcache  Current register cache.
   @param[in] pc        The value of the program counter to set.              */
/* -------------------------------------------------------------------------- */
static void
or32_set_pc (struct regcache *regcache,
	     CORE_ADDR        pc)
{
  unsigned long int  npc = pc;
  supply_register_by_name (regcache, "npc", &npc);

}	/* or32_set_pc () */


/*! The value of a breakpoint instruction (l.trap  1). */
static const unsigned char or32_breakpoint [] = {0x21, 0x00, 0x00, 0x01};


/*! The length of a breakpoint instruction. */
#define OR32_BREAKPOINT_LEN  4


/* -------------------------------------------------------------------------- */
/*!Predicate to indicate if there is a breakpoint at the current address.

   For the OR32 we, just look for the l.trap 1 instruction.

   @param[in] where  The address to look for a breakpoint at.

   @return  Non-zero (TRUE) if there is a breakpoint, zero (FALSE) otherwise. */
/* -------------------------------------------------------------------------- */
static int
or32_breakpoint_at (CORE_ADDR where)
{
  unsigned char  insn[OR32_BREAKPOINT_LEN];

  (*the_target->read_memory) (where, insn, OR32_BREAKPOINT_LEN);

  return  (0 ==  bcmp (insn, or32_breakpoint, OR32_BREAKPOINT_LEN));

}	/* or32_breakpoint_at () */


/* -------------------------------------------------------------------------- */
/*!Data structure giving all the target specific functions.

   Details are in struct linux_target_ops in linux-low.h.                     */
/* -------------------------------------------------------------------------- */
struct linux_target_ops the_low_target = {
  init_registers_or32,			/* Arch initialization */
  or32_num_regs,			/* Num regs in arch */
  or32_regmap,				/* Reg offsets for ptrace */
  or32_cannot_fetch_register,		/* Predicate for reg reading */
  or32_cannot_store_register,		/* Predicate for reg writing */
  or32_get_pc,				/* Read the PC */
  or32_set_pc,				/* Write the PC */
  or32_breakpoint,			/* Breakpoint instruction bytes */
  OR32_BREAKPOINT_LEN,			/* Breakpoint length */
  NULL,					/* Breakpoint reinsertion (unused) */
  0,					/* Decrement PC after break (FALSE) */
  or32_breakpoint_at,			/* Predicate to check for breakpoint */
  NULL,					/* Insert watchpoint (unused) */
  NULL,					/* Remove watchpoint (unused) */
  NULL,					/* Predicate if stopped by watchpoint */
  NULL,					/* Data address for watchpoint stop */
  NULL,					/* ptrace PEEKUSR hook */
  NULL,					/* ptrace POKEUSR hook */
  NULL,					/* ptrace conversion predicate */
  NULL,					/* New process hook */
  NULL,					/* New thread hook */
  NULL,					/* Prepare to resume thread */
  NULL,					/* Target specific qSupported */
  NULL,					/* Tracepoint supported predicate */
  NULL,					/* Get thread area address */
  NULL,					/* Fast tracepoint jump pad */
  NULL,					/* Get bytecode operations vector */
};
