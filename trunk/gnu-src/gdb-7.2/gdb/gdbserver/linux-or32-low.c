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

#ifdef HAVE_SYS_REG_H
#include <sys/reg.h>
#endif


/* -------------------------------------------------------------------------- */
/*!Initialize the register data.
  
  Should be automagically created from a data file in gdb/regformats, but for
  now we do it manually.                                                      */
/* -------------------------------------------------------------------------- */
static void
init_registers_or32 ()
{
  struct reg regs_or32[] = {
    { "r0", 0, 32 },
    { "sp", 32, 32 },
    { "fp", 64, 32 },
    { "r3", 96, 32 },
    { "r4", 128, 32 },
    { "r5", 160, 32 },
    { "r6", 192, 32 },
    { "r7", 224, 32 },
    { "r8", 256, 32 },
    { "lr", 288, 32 },
    { "r10", 320, 32 },
    { "r11", 352, 32 },
    { "r12", 384, 32 },
    { "r13", 416, 32 },
    { "r14", 448, 32 },
    { "r15", 480, 32 },
    { "r16", 512, 32 },
    { "r17", 544, 32 },
    { "r18", 576, 32 },
    { "r19", 608, 32 },
    { "r20", 640, 32 },
    { "r21", 672, 32 },
    { "r22", 704, 32 },
    { "r23", 736, 32 },
    { "r24", 768, 32 },
    { "r25", 800, 32 },
    { "r26", 832, 32 },
    { "r27", 864, 32 },
    { "r28", 896, 32 },
    { "r29", 928, 32 },
    { "r30", 960, 32 },
    { "r31", 992, 32 },
    { "ppc", 1024, 32 },
    { "npc", 1056, 32 },
    { "sr", 1088, 32 },
  };
  static const char *expedite_regs_or32[] = { "sp", "lr", "npc", 0 };

  set_register_cache (regs_or32, sizeof (regs_or32) / sizeof (regs_or32[0]));
  gdbserver_expedite_regs = expedite_regs_or32;
  gdbserver_xmltarget     = NULL;
}


/*! OpenRISC has 32 general purpose registers followed by PPC, NPC and SR in
    that order. */
#define or32_num_regs  (32 + 3)


/* -------------------------------------------------------------------------- */
/*!Provide the ptrace "address" of a register.

   Not properly supported by uClibc/Linux just yet. The register offsets are
   pure guesswork.                                                            */
/* -------------------------------------------------------------------------- */
static int or32_regmap[] = {
#ifdef PT_R0
  PT_R0,  PT_SP,  PT_FP,  PT_R3,  PT_R4,  PT_R5,  PT_R6,  PT_R7,
  PT_R8,  PT_LR,  PT_R10, PT_R11, PT_R12, PT_R13, PT_R14, PT_R15,
  PT_R16, PT_R17, PT_R18, PT_R19, PT_R20, PT_R21, PT_R22, PT_R23,
  PT_R24, PT_R25, PT_R26, PT_R27, PT_R28, PT_R29, PT_R30, PT_R31,
  PT_PPC, PT_NPC, PT_SR
#else
  4 *  0, 4 *  1, 4 *  2, 4 *  3, 4 *  4, 4 *  5, 4 *  6, 4 *  7,
  4 *  8, 4 *  9, 4 * 10, 4 * 11, 4 * 12, 4 * 13, 4 * 14, 4 * 15,
  4 * 16, 4 * 17, 4 * 18, 4 * 19, 4 * 20, 4 * 21, 4 * 22, 4 * 23,
  4 * 24, 4 * 25, 4 * 26, 4 * 27, 4 * 28, 4 * 29, 4 * 30, 4 * 31,
  4 * 32, 4 * 33, 4 * 34
#endif
};


/* -------------------------------------------------------------------------- */
/*!Predicate to indicate if a register can be read.

   For now, we believe all OR32 registers are readable.

   @param[in] regno  Register to read.

   @return  Non-zero (TRUE) if the register can be read, zero (FALSE)
            otherwise.                                                        */
/* -------------------------------------------------------------------------- */
static int
or32_cannot_fetch_register (int  regno)
{
  return (regno >= or32_num_regs);

}	/* or32_cannot_fetch_register () */


/* -------------------------------------------------------------------------- */
/*!Predicate to indicate if a register can be written.

   For now, we believe all OR32 registers are writable.

   @param[in] regno  Register to write.

   @return  Non-zero (TRUE) if the register can be written, zero (FALSE)
            otherwise.                                                        */
/* -------------------------------------------------------------------------- */
static int
or32_cannot_store_register (int  regno)
{
  return (regno >= or32_num_regs);

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
  supply_register_by_name (regcache, "pc", &npc);

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
  NULL,					/* Insert matchpoint (unused) */
  NULL,					/* Remove matchpoint (unused) */
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
