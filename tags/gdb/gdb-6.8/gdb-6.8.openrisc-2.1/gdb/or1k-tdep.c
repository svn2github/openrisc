/* Target-dependent code for the or1k architecture, for GDB, the GNU Debugger.

   Copyright 1988-2008, Free Software Foundation, Inc.
   Copyright (C) 2008 Embecosm Limited

   Contributed by Alessandro Forin(af@cs.cmu.edu at CMU
   and by Per Bothner(bothner@cs.wisc.edu) at U.Wisconsin.
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

/*-----------------------------------------------------------------------------
   This version for the OpenRISC 1000 architecture is a rewrite by Jeremy
   Bennett of the old GDB 5.3 interface to make use of gdbarch for GDB 6.8.

   The code tries to follow the GDB coding style.

   Commenting is Doxygen compatible.

   Much has been stripped out in the interests of getting a basic working
   system. This is described as the OpenRISC 1000 target architecture, so
   should work with 16, 32 and 64 bit versions of that architecture and should
   work whether or not they have floating point and/or vector registers.

   There was never a capability to run simulator commands (no remote target
   implemented the required function), so that has been removed.

   The info trace command has been removed. The meaning of this is not clear -
   it relies on a value in register 255 of the debug group, which is
   undocumented.

   All the hardware trace has been removed for the time being. The new debug
   interface does not support hardware trace, so there is no plan to reinstate
   this functionality.

   Support for multiple contexts (which was rudimentary, and not working) has
   been removed. */
/*---------------------------------------------------------------------------*/

#include "demangle.h"
#include "defs.h"
#include "gdb_string.h"
#include "frame.h"
#include "inferior.h"
#include "symtab.h"
#include "value.h"
#include "gdbcmd.h"
#include "language.h"
#include "gdbcore.h"
#include "symfile.h"
#include "objfiles.h"
#include "gdbtypes.h"
#include "target.h"
#include "regcache.h"

#include "opcode/or32.h"
#include "or1k-tdep.h"

#include "safe-ctype.h"
#include "block.h"
#include "reggroups.h"
#include "arch-utils.h"
#include "frame.h"
#include "frame-unwind.h"
#include "frame-base.h"
#include "dwarf2-frame.h"
#include "trad-frame.h"

#include <inttypes.h>


/* Forward declarations of support functions for the architecture definition */

static unsigned long int
                     or1k_fetch_instruction (struct frame_info *next_frame,
					     CORE_ADDR          addr);
static void          or1k_store_instruction( struct frame_info *next_frame,
					   CORE_ADDR          addr,
					   unsigned long int  insn);

/* Forward declaration of support functions for frame handling */

static int   or1k_frame_size (struct frame_info *next_frame,
			      CORE_ADDR          func_start_addr);
static int   or1k_frame_fp_loc (struct frame_info *next_frame,
				CORE_ADDR          func_start_addr);
static int   or1k_frame_size_check (struct frame_info *next_frame,
				    CORE_ADDR          func_start_addr);
static int   or1k_link_address (struct frame_info *next_frame,
				CORE_ADDR          func_start_addr);
static int   or1k_get_saved_reg (struct frame_info *next_frame,
				 CORE_ADDR          instr_start_addr,
				 int               *reg_offset);
static struct trad_frame_cache
            *or1k_frame_unwind_cache (struct frame_info  *next_frame,
				      void              **this_prologue_cache);
static void  or1k_frame_this_id (struct frame_info  *next_frame,
				 void              **this_prologue_cache,
				 struct frame_id    *this_id);
static void  or1k_frame_prev_register (struct frame_info  *next_frame,
				       void              **this_prologue_cache,
				       int                 regnum,
				       int                *optimizedp,
				       enum lval_type     *lvalp,
				       CORE_ADDR          *addrp,
				       int                *realregp,
				       gdb_byte           *bufferp);
static CORE_ADDR 
             or1k_frame_base_address (struct frame_info  *next_frame,
				      void              **this_prologue_cache);

/* Forward declarations of functions which define the architecture */

static enum return_value_convention
                        or1k_return_value (struct gdbarch  *gdbarch,
					   struct type     *type,
					   struct regcache *regcache,
					   gdb_byte        *readbuf,
					   const gdb_byte  *writebuf);
static const gdb_byte  *or1k_breakpoint_from_pc (struct gdbarch *gdbarch,
						 CORE_ADDR      *bp_addr,
						 int            *bp_size);
static int     or1k_single_step_through_delay (struct gdbarch    *gdbarch,
					       struct frame_info *this_frame);
static void             or1k_pseudo_register_read (struct gdbarch  *gdbarch,
						   struct regcache *regcache,
						   int              regnum,
						   gdb_byte        *buf);
static void             or1k_pseudo_register_write (struct gdbarch  *gdbarch,
						    struct regcache *regcache,
						    int              regnum,
						    const gdb_byte  *buf);
static const char      *or1k_register_name (struct gdbarch *gdbarch,
					    int             regnum);
static struct type     *or1k_register_type (struct gdbarch *arch,
					    int             regnum);
static void             or1k_registers_info (struct gdbarch    *gdbarch,
					     struct ui_file    *file,
					     struct frame_info *frame,
					     int                regnum,
					     int                all);
static int              or1k_register_reggroup_p (struct gdbarch  *gdbarch,
						  int              regnum,
						  struct reggroup *group);
static CORE_ADDR        or1k_skip_prologue (struct gdbarch *gdbarch,
					    CORE_ADDR       pc);
static CORE_ADDR        or1k_frame_align (struct gdbarch *gdbarch,
					  CORE_ADDR       sp);
static CORE_ADDR        or1k_unwind_pc (struct gdbarch    *gdbarch,
					struct frame_info *next_frame);
static CORE_ADDR        or1k_unwind_sp (struct gdbarch    *gdbarch,
					struct frame_info *next_frame);
static CORE_ADDR        or1k_push_dummy_call (struct gdbarch  *gdbarch,
					      struct value    *function,
					      struct regcache *regcache,
					      CORE_ADDR        bp_addr,
					      int              nargs,
					      struct value   **args,
					      CORE_ADDR        sp,
					      int              struct_return,
					      CORE_ADDR        struct_addr);
static struct frame_id  or1k_unwind_dummy_id (struct gdbarch    *gdbarch,
					      struct frame_info *next_frame);
static const struct frame_unwind *
                        or1k_frame_sniffer (struct frame_info *next_frame);

/* Forward declaration of architecture set up functions */

static struct gdbarch *or1k_gdbarch_init (struct gdbarch_info  info,
					  struct gdbarch_list *arches);

static void            or1k_dump_tdep (struct gdbarch *gdbarch,
				       struct ui_file *file);

/* Forward declarations of functions which extend GDB */

static const char     *or1k_spr_group_name (int  group);
static char           *or1k_spr_register_name (int   group,
					       int   index,
					       char *name);
static int             or1k_groupnum_from_name (char *group_name);
static int             or1k_regnum_from_name (int   group,
					      char *name);
static int             or1k_tokenize (char  *str,
				      char **tok);
static char           *or1k_parse_spr_params (char *args,
					      int  *group,
					      int  *index,
					      int   is_set);
static ULONGEST        or1k_read_spr (unsigned int  regnum);
static void            or1k_write_spr (unsigned int  regnum,
				       ULONGEST      data);
static void            info_spr_command (char *args,
					 int   from_tty);
static void            or1k_spr_command (char *args,
					 int   from_tty);
static void            info_matchpoints_command (char *args,
						 int   from_tty);



/* Support functions for the architecture definition */


/*----------------------------------------------------------------------------*/
/*!Get an instruction from a frame

   This reads from memory, but if the instruction has been substituted by a
   software breakpoint, returns the instruction that has been replaced, NOT
   the break point instruction

   Depending on whether this has a frame available we use a frame based memory
   access or independent memory access. Underneath they are both the same, but
   annoyingly save_frame_unwind_memory inverts the status returned!

   @param[in] next_frame  Information about the next frame.
   @param[in] addr        Address from which to get the instruction

   @return  The instruction */
/*---------------------------------------------------------------------------*/

static unsigned long int
or1k_fetch_instruction (struct frame_info *next_frame,
			CORE_ADDR          addr) 
{
  char  buf[OR1K_INSTLEN];
  int   status;

  struct frame_info *this_frame = get_prev_frame (next_frame);
  if (NULL != this_frame)
    {
      status = !(safe_frame_unwind_memory (this_frame, addr, buf,
					   OR1K_INSTLEN));
    }
  else
    {
      status = read_memory_nobpt (addr, buf, OR1K_INSTLEN);
    }

  if (0 != status)
    {
      memory_error (status, addr);
    }

  return  (unsigned long int)(extract_unsigned_integer (buf, OR1K_INSTLEN));

}	/* or1k_fetch_instruction() */


/*----------------------------------------------------------------------------*/
/*!Store an instruction in a frame

   This writes to memory. Unlike its counterpart to fetch the instruction it
   does nothing about breakpoints

   Depending on whether this has a frame available we use a frame based memory
   access or independent memory access.

   @param[in] next_frame  Information about the next frame. Here for
                          compatibility with the fetch function, but ignored.
   @param[in] addr        Address to which to put the instruction
   @param[in] insn        The instruction to be written */
/*---------------------------------------------------------------------------*/

static void
or1k_store_instruction (struct frame_info *next_frame,
			CORE_ADDR          addr,
			unsigned long int  insn) 
{
  write_memory_unsigned_integer( addr, sizeof( insn ), insn );

}	/* or1k_store_instruction() */





/* Support functions for frame handling */


/*----------------------------------------------------------------------------*/
/*!Return the size of the new stack frame

   Given the function start address, find the size of the stack frame. We are
   looking for the instruction

   @verbatim
   l.addi  r1,r1,-<frame_size>
   @endverbatim

   If this is not found at the start address, then this must be frameless
   invocation, for which we return size 0.

   @see or1k_frame_unwind_cache() for details of the OR1K prolog

   @param[in]  next_frame       The NEXT frame (i.e. inner from here, the one
                                THIS frame called), or NULL if this
                                information is not available.
   @param[in]  instr_addr       Function start address

   @return  The size of the new stack frame, or zero if this is frameless */
/*---------------------------------------------------------------------------*/

static int
or1k_frame_size (struct frame_info *next_frame,
		 CORE_ADDR          instr_addr) 
{
  uint32_t  instr  = or1k_fetch_instruction (next_frame, instr_addr);
  int       opcode = OR1K_OPCODE1 (instr);
  int       rd;
  int       ra;
  int       imm;

  if (OR1K_OP_ADDI != opcode)
    {
      return  0;
    }

  rd  = OR1K_D_REG (instr);
  ra  = OR1K_A_REG (instr);
  imm = OR1K_IMM (instr);

  if((OR1K_SP_REGNUM == rd) && (OR1K_SP_REGNUM == ra))
    {
      return  -imm;		/* Falling stack */
    }
  else
    {
      return 0;
    }
}	/* or1k_frame_size() */


/*----------------------------------------------------------------------------*/
/*!Return the offset from the stack pointer of the saved FP location

   Given the function start address, find the size of the stack frame. We are
   looking for the instruction

   @verbatim
   l.sw    <save_loc>(r1),r2
   @endverbatim

   If this is not found at the start address + 4, then this is an error.

   @see or1k_frame_unwind_cache() for details of the OR1K prolog

   @param[in]  next_frame   The NEXT frame (i.e. inner from here, the one THIS
                            frame called), or NULL if this information is not
                            available.
   @param[in]  instr_addr   Address where we find this instruction (function
                            start + OR1K_INSTLEN)

   @return  The offset from the stack pointer where the old frame pointer is
   saved or -1 if we don't find this instruction. */
/*--------------------------------------------------------------------------*/

static int
or1k_frame_fp_loc (struct frame_info *next_frame,
		   CORE_ADDR          instr_addr) 
{
  uint32_t  instr  = or1k_fetch_instruction (next_frame, instr_addr);
  int       opcode = OR1K_OPCODE1 (instr);
  int       ra;
  int       rb;
  int       imm;

  if (OR1K_OP_SW != opcode)
    {
      return  -1;
    }

  ra  = OR1K_A_REG (instr);
  rb  = OR1K_B_REG (instr);
  imm = OR1K_IMM2 (instr);

  if((OR1K_SP_REGNUM != ra) || (OR1K_FP_REGNUM != rb))
    {
      return  -1;
    }

  return imm;

}	/* or1k_frame_fp_loc() */


/*----------------------------------------------------------------------------*/
/*!Check the frame size is what expected

   Given the function start address, find the setting of the frame
   pointer. This should choose a frame size matching that used earlier to set
   the stack pointer. We look for the instruction:

   @verbatim
   l.addi  r2,r1,<frame_size>
   @endverbatim

   If this is not found at the start address + 8, with the expected frame size
   then this is an error.

   There is no return value - the function raises an error if the instruction
   is not found.

   @see or1k_frame_unwind_cache() for details of the OR1K prolog

   @param[in]  next_frame  The NEXT frame (i.e. inner from here, the one THIS
                           frame called), or NULL if this information is not
                           available.
   @param[in]  instr_addr  Address where we find this instruction (function
                           start + 2*OR1K_INSTLEN)

			   @return  The frame size found, or -1 if the instruction was not there. */
/*---------------------------------------------------------------------------*/

static int
or1k_frame_size_check (struct frame_info *next_frame,
		       CORE_ADDR          instr_addr) 
{
  uint32_t  instr  = or1k_fetch_instruction (next_frame, instr_addr);
  int       opcode = OR1K_OPCODE1 (instr);
  int       rd;
  int       ra;
  int       imm;

  if (OR1K_OP_ADDI != opcode)
    {
      return  -1;
    }

  rd  = OR1K_D_REG (instr);
  ra  = OR1K_A_REG (instr);
  imm = OR1K_IMM (instr);

  if((OR1K_SP_REGNUM != ra) || (OR1K_FP_REGNUM != rd))
    {
      return -1;
    }

  return  imm;

}	/* or1k_frame_size_check() */


/*----------------------------------------------------------------------------*/
/*!See if the link (return) address is saved as expected

   Given the function start address, find the saving of the link address. The
   location (as an offset from the stack pointer) should be 4 less than the
   offset where the frame pointer was saved. We look for the instruction:

   @verbatim
   l.sw    <save_loc-4>(r1),r9
   @endverbatim

   This instruction may be missing - leaf functions do not necessarily save
   the return address on the stack.

   @see or1k_frame_unwind_cache() for details of the OR1K prolog

   @param[in]  next_frame  The NEXT frame (i.e. inner from here, the one THIS
                           frame called), or NULL if this information is not
                           available.
   @param[in]  instr_addr  Address where we find this instruction (function
                           start + 12)

			   @return  The link offset if the instruction was found, -1 otherwise */
/*---------------------------------------------------------------------------*/

static int
or1k_link_address (struct frame_info *next_frame,
		   CORE_ADDR          instr_addr) 
{
  uint32_t  instr  = or1k_fetch_instruction (next_frame, instr_addr);
  int       opcode = OR1K_OPCODE1 (instr);
  int       ra;
  int       rb;
  int       imm;

  if (OR1K_OP_SW != opcode)
    {
      return  -1;
    }

  ra  = OR1K_A_REG (instr);
  rb  = OR1K_B_REG (instr);
  imm = OR1K_IMM2 (instr);

  if((OR1K_SP_REGNUM != ra) || (OR1K_LR_REGNUM != rb))
    {
      return  -1;
    }

  return  imm;

}	/* or1k_link_address() */


/*----------------------------------------------------------------------------*/
/*!Get a saved register's details

   Given an address, see if it contains an instruction to save a register with
   the specified offset from the stack pointer. The locations increment by 4
   from the location where the FP was saved for each callee saved register. We
   look for the instruction:

   @verbatim
   l.sw    x(r1),ry
   @endverbatim

   If this is found with the expected offset (x), then the register number
   (y) is returned. If not -1 is returned (not a register). The register
   must be one of the 10 callee saved registers (r10, r12, r14, r16, r18, r20,
   r22, r24, r26, r28, r30).

   @see or1k_frame_unwind_cache() for details of the OR1K prolog

   @param[in]  next_frame  The NEXT frame (i.e. inner from here, the one THIS
                           frame called), or NULL if this information is not
                           available.
   @param[in]  instr_addr  Location of this instruction
   @param[out] reg_offset  Offset where the register is saved

   @return  The register number if this instruction is found, otherwise -1 */
/*---------------------------------------------------------------------------*/

static int
or1k_get_saved_reg (struct frame_info *next_frame,
		    CORE_ADDR          instr_addr,
		    int               *reg_offset) 
{
  uint32_t  instr  = or1k_fetch_instruction (next_frame, instr_addr);
  int       opcode = OR1K_OPCODE1 (instr);
  int       ra;
  int       rb;
  int       imm;

  if (OR1K_OP_SW != opcode)
    {
      return -1;
    }

  ra  = OR1K_A_REG (instr);
  rb  = OR1K_B_REG (instr);
  imm = OR1K_IMM2 (instr);

  if(OR1K_SP_REGNUM != ra)
    {
      return -1;
    }

  if ((1 == (rb % 2)) || rb < 10)
    {
      return  -1;			/* Not a callee saved register */
    }

  *reg_offset = imm;
  return  rb;

}	/* or1k_get_saved_reg() */


/*----------------------------------------------------------------------------*/
/*!Initialize a prologue (unwind) cache

   Build up the information (saved registers etc) for the given frame if it
   does not already exist.

   The OR1K has a falling stack frame and a simple prolog. The Stack pointer
   is R1 and the frame pointer R2. The frame base is therefore the address
   held in R2 and the stack pointer (R1) is the frame base of the NEXT frame.

   @verbatim
   l.addi  r1,r1,-frame_size	# SP now points to end of new stack frame
   l.sw    save_loc(r1),r2      # old FP saved in new stack frame
   l.addi  r2,r1,frame_size     # FP now points to base of new stack frame
   l.sw    save_loc-4(r1),r9    # Link (return) address
   l.sw    x(r1),ry             # Save any callee saved regs
   @endverbatim

   The frame pointer is not necessarily saved right at the end of the stack
   frame - OR1K saves enough space for any args to called functions right at
   the end. The offsets x for the various registers saved always rise in
   increments of 4, starting at save_loc+4.

   This prolog is used, even for -O3 with GCC.

   All this analysis must allow for the possibility that the PC is in the
   middle of the prologue. Data should only be set up insofar as it has been
   computed.

   A suite of "helper" routines are used, allowing reuse for
   or1k_skip_prologue().

   Reportedly, this is only valid for frames less than 0x7fff in size.

   @param[in]     next_frame           The NEXT frame (i.e. inner from here,
                                       the one THIS frame called)
   @param[in,out] this_prologue_cache  The prologue cache. If not supplied, we
                                       build it.

				       @return  The prolog cache (duplicates the return through the argument) */
/*---------------------------------------------------------------------------*/

static struct trad_frame_cache *
or1k_frame_unwind_cache (struct frame_info  *next_frame,
			 void              **this_prologue_cache) 
{
  struct gdbarch          *gdbarch;
  struct trad_frame_cache *info;

  CORE_ADDR                this_pc;
  CORE_ADDR                this_sp;
  int                      frame_size;
  int                      fp_save_offset;
  int                      tmp;

  CORE_ADDR                start_iaddr;
  CORE_ADDR                saved_regs_iaddr;
  CORE_ADDR                prologue_end_iaddr;
  CORE_ADDR                end_iaddr;
  
  int                      regnum;

  /* Nothing to do if we already have this info */
  if (NULL != *this_prologue_cache)
    {
      return *this_prologue_cache;
    }

  gdbarch = get_frame_arch (next_frame);

  /* Get a new prologue cache and populate it with default values */
  info                 = trad_frame_cache_zalloc (next_frame);
  *this_prologue_cache = info;

  /* Find the start address of THIS function (which is a NORMAL frame, even if
     the NEXT frame is the sentinel frame) and the end of its prologue.  */
  start_iaddr        = frame_func_unwind (next_frame, NORMAL_FRAME);
  prologue_end_iaddr = skip_prologue_using_sal (start_iaddr);

  /* Return early if GDB couldn't find the function.  */
  if (start_iaddr == 0)
    {
      return  info;
    }

  /* Unwind key registers for THIS frame. */
  this_pc = or1k_unwind_pc (gdbarch, next_frame);
  this_sp = or1k_unwind_sp (gdbarch, next_frame);

  /* The frame base of THIS frame is its stack pointer. This is the same
     whether we are frameless or not. */
  trad_frame_set_this_base (info, this_sp);

  /* We should only examine code that is in the prologue and which has been
     executed. This is all code up to (but not including) end_iaddr. */
  end_iaddr = (this_pc > prologue_end_iaddr) ? prologue_end_iaddr : this_pc;

  /* The default is to find the PC of the PREVIOUS frame in the link register
     of this frame. This may be changed if we find the link register was saved
     on the stack. */
  trad_frame_set_reg_realreg (info, OR1K_NPC_REGNUM, OR1K_LR_REGNUM);

  /* All the following analysis only occurs if we are in the prologue and have
     executed the code. Get THIS frame size (which implies framelessness if
     zero) */

  if (end_iaddr > start_iaddr)
    {
      frame_size = or1k_frame_size (next_frame, start_iaddr);
    }
  else
    {
      frame_size = 0;
    }

  /* If we are not frameless, check the other standard components are present
     as expected */
  if ((0 != frame_size) && (end_iaddr > (start_iaddr + OR1K_INSTLEN)))
    {
      int  i;

      /* If we are not frameless, the frame pointer of the PREVIOUS frame can
	 be found at offset fp_save_offset from the stack pointer in THIS
	 frame. */
      fp_save_offset = or1k_frame_fp_loc (next_frame,
					  start_iaddr + OR1K_INSTLEN);
      if (-1 == fp_save_offset)
	{
	  error ("or1k_frame_unwind_cache: "
		 "invalid frame pointer save instruction at address %08llx\n",
		 (long long unsigned int)(ULONGEST)(start_iaddr + OR1K_INSTLEN));
	}
      else
	{
	  trad_frame_set_reg_addr (info, OR1K_FP_REGNUM,
				   this_sp + fp_save_offset);
	}

      /* The frame pointer should be set up to match the allocated stack
	 size */
      if (end_iaddr > (start_iaddr + (2 * OR1K_INSTLEN)))
	{
	  tmp = or1k_frame_size_check (next_frame,
				       start_iaddr + (2 * OR1K_INSTLEN));

	  if (-1 == tmp)
	    {
	      error ("or1k_frame_unwind_cache: "
		     "no frame pointer set up instruction at address %08llx\n",
		     (long long unsigned int)(ULONGEST)(start_iaddr + (2 * OR1K_INSTLEN)));
	    }
	  else if (frame_size != tmp)
	    {
	      error ("or1k_frame_unwind_cache: "
		     "frame pointer set to wrong size  at address %08llx: "
		     "expected %d, got %d\n",
		     (long long unsigned int)(ULONGEST)(start_iaddr + (2* OR1K_INSTLEN)), frame_size,
		     tmp);
	    }
	  else
	    {
	      /* If we have got this far, the stack pointer of the PREVIOUS
		 frame is the frame pointer of THIS frame. */
	      trad_frame_set_reg_realreg (info, OR1K_SP_REGNUM, OR1K_FP_REGNUM);
	    }
	}
      /* If the link register is saved in the THIS frame, it holds the value
	 of the PC in the PREVIOUS frame. This overwrites the previous
	 information about finding the PC in the link register. */
      if (end_iaddr > (start_iaddr + (2 * OR1K_INSTLEN)))
	{
	  tmp = or1k_link_address (next_frame,
				   start_iaddr + (3 * OR1K_INSTLEN));
	  if ((-1 != tmp) && (tmp == (fp_save_offset - OR1K_INSTLEN)))
	    {
	      trad_frame_set_reg_addr (info, OR1K_LR_REGNUM, this_sp + tmp);
	      trad_frame_set_reg_addr (info, OR1K_NPC_REGNUM, this_sp + tmp);
	      saved_regs_iaddr = start_iaddr + (3 * OR1K_INSTLEN);
	    }
	  else
	    {
	      saved_regs_iaddr = start_iaddr + (2 * OR1K_INSTLEN);
	    }

	  /* Retrieve any saved register information */
	  for (i = OR1K_INSTLEN;
	       saved_regs_iaddr + i < end_iaddr;
	       i += OR1K_INSTLEN)
	    {
	      regnum = or1k_get_saved_reg (next_frame, saved_regs_iaddr + i,
					   &tmp);

	      if ((regnum < 0) || (tmp != (fp_save_offset + i)))
		{
		  break;			/* End of register saves */
		}

	      /* The register in the PREVIOUS frame can be found at this
		 location in THIS frame */
	      trad_frame_set_reg_addr (info, regnum,
				       this_sp + fp_save_offset + i);
	    }
	}
    }

  /* Build the frame ID */
  trad_frame_set_id (info, frame_id_build (this_sp, start_iaddr));

  return info;

}	/* or1k_frame_unwind_cache() */


/*----------------------------------------------------------------------------*/
/*!Find the frame ID of this frame

   Given a GDB frame (called by THIS frame), determine the address of oru
   frame and from this create a new GDB frame struct. The info required is
   obtained from the prologue cache for THIS frame.

   @param[in] next_frame            The NEXT frame (i.e. inner from here, the
                                    one THIS frame called)
   @param[in]  this_prologue_cache  Any cached prologue for THIS function.
   @param[out] this_id              Frame ID of our own frame.

   @return  Frame ID for THIS frame */
/*---------------------------------------------------------------------------*/

static void
or1k_frame_this_id (struct frame_info  *next_frame,
		    void              **this_prologue_cache,
		    struct frame_id    *this_id) 
{
  struct trad_frame_cache *info =
    or1k_frame_unwind_cache (next_frame, this_prologue_cache);

  trad_frame_get_id (info, this_id);

}	/* or1k_frame_this_id() */


/*----------------------------------------------------------------------------*/
/*!Get a register from THIS frame

   Given a pointer to the NEXT frame, return the details of a register in the
   PREVIOUS frame.

   @param[in] next_frame            The NEXT frame (i.e. inner from here, the
                                    one THIS frame called)
   @param[in]  this_prologue_cache  Any cached prologue associated with THIS
                                    frame, which may therefore tell us about
                                    registers in the PREVIOUS frame.
   @param[in]  regnum               The register of interest in the PREVIOUS
                                    frame
   @param[out] optimizedp           True (1) if the register has been
                                    optimized out.
   @param[out] lvalp                What sort of l-value (if any) does the
                                    register represent
   @param[out] addrp                Address in THIS frame where the register's
                                    value may be found (-1 if not available)
   @param[out] realregp             Register in this frame where the
                                    register's value may be found (-1 if not
                                    available)
   @param[out] bufferp              If non-NULL, buffer where the value held
   in the register may be put */
/*--------------------------------------------------------------------------*/

static void
or1k_frame_prev_register (struct frame_info  *next_frame,
			  void              **this_prologue_cache,
			  int                 regnum,
			  int                *optimizedp,
			  enum lval_type     *lvalp,
			  CORE_ADDR          *addrp,
			  int                *realregp,
			  gdb_byte           *bufferp) 
{
  struct trad_frame_cache *info =
    or1k_frame_unwind_cache (next_frame, this_prologue_cache);

  trad_frame_get_register (info, next_frame, regnum, optimizedp, lvalp, addrp,
			   realregp, bufferp);

}	/* or1k_frame_prev_register() */


/*----------------------------------------------------------------------------*/
/*!Return the base address of the frame

   The commenting in the GDB source code could mean our stack pointer or our
   frame pointer, since we have a falling stack, but index within the frame
   using negative offsets from the FP.

   This seems to be the function used to determine the value of $fp, but the
   value required seems to be the stack pointer, so we return that, even if
   the value of $fp will be wrong.

   @param[in] next_frame            The NEXT frame (i.e. inner from here, the
                                    one THIS frame called)
   @param[in]  this_prologue_cache  Any cached prologue for THIS function.

   @return  The frame base address */
/*---------------------------------------------------------------------------*/

static CORE_ADDR
or1k_frame_base_address (struct frame_info  *next_frame,
			 void              **this_prologue_cache) 
{
  return  frame_unwind_register_unsigned (next_frame, OR1K_SP_REGNUM);

}	/* or1k_frame_base_address() */




/* Functions defining the architecture */


/*----------------------------------------------------------------------------*/
/*!Determine the return convention used for a given type

   Optionally, fetch or set the return value via "readbuf" or "writebuf"
   respectively using "regcache" for the register values.

   The OpenRISC 1000 returns scalar values via R11 and (for 64 bit values on
   32 bit architectures) R12. Structs and unions are returned by reference,
   with the address in R11

   Throughout use read_memory(), not target_read_memory(), since the address
   may be invalid and we want an error reported (read_memory() is
   target_read_memory() with error reporting).

   @todo This implementation is labelled OR1K, but in fact is just for the 32
         bit version, OR32. This should be made explicit

   @param[in]  gdbarch   The GDB architecture being used
   @param[in]  type      The type of the entity to be returned
   @param[in]  regcache  The register cache
   @param[in]  readbuf   Buffer into which the return value should be written
   @param[out] writebuf  Buffer from which the return value should be written

   @return  The type of return value */
/*---------------------------------------------------------------------------*/

static enum return_value_convention
or1k_return_value (struct gdbarch  *gdbarch,
		   struct type     *type,
		   struct regcache *regcache,
		   gdb_byte        *readbuf,
		   const gdb_byte  *writebuf) 
{
  enum type_code  rv_type = TYPE_CODE (type);
  unsigned int    rv_size = TYPE_LENGTH (type);
  ULONGEST        tmp;

  /* Deal with struct/union and large scalars first. Large (> 4 byte) scalars
     are returned via a pointer (despite what is says in the architecture
     document). Result pointed to by R11 */

  if((TYPE_CODE_STRUCT == rv_type) ||
     (TYPE_CODE_UNION  == rv_type) ||
     (rv_size          >  4))
    {
      if (readbuf)
	{
	  regcache_cooked_read_unsigned (regcache, OR1K_RV_REGNUM, &tmp);
	  read_memory (tmp, readbuf, rv_size);
	}
      if (writebuf)
	{
	  regcache_cooked_read_unsigned (regcache, OR1K_RV_REGNUM, &tmp);
	  write_memory (tmp, writebuf, rv_size);
	}

      return RETURN_VALUE_ABI_RETURNS_ADDRESS;
    }

  /* 1-4 byte scalars are returned in R11 */

  if (readbuf)
    {
      regcache_cooked_read_unsigned (regcache, OR1K_RV_REGNUM, &tmp);
      store_unsigned_integer (readbuf, rv_size, tmp);
    }
  if (writebuf)
    {
      gdb_byte buf[4];
      memset (buf, 0, sizeof (buf));	/* Pad with zeros if < 4 bytes */

      if (BFD_ENDIAN_BIG == gdbarch_byte_order (gdbarch))
	{
	  memcpy (buf + sizeof (buf) - rv_size, writebuf, rv_size);
	}
      else
	{
	  memcpy (buf,                          writebuf, rv_size);
	}

      regcache_cooked_write (regcache, OR1K_RV_REGNUM, buf);
    }

  return RETURN_VALUE_REGISTER_CONVENTION;

}	/* or1k_return_value() */


/*----------------------------------------------------------------------------*/
/*!Determine the instruction to use for a breakpoint.

   Given the address at which to insert a breakpoint (bp_addr), what will
   that breakpoint be?

   For or1k, we have a breakpoint instruction. Since all or1k instructions
   are 32 bits, this is all we need, regardless of address.

   @param[in]  gdbarch  The GDB architecture being used
   @param[in]  bp_addr  The breakpoint address in question
   @param[out] bp_size  The size of instruction selected

   @return  The chosen breakpoint instruction */
/*---------------------------------------------------------------------------*/

static const gdb_byte *
or1k_breakpoint_from_pc (struct gdbarch *gdbarch,
			 CORE_ADDR      *bp_addr,
			 int            *bp_size) 
{ 
  static const gdb_byte breakpoint[] = OR1K_BRK_INSTR_STRUCT;

  *bp_size = OR1K_INSTLEN;
  return breakpoint;

}	/* or1k_breakpoint_from_pc() */


/*----------------------------------------------------------------------------*/
/*!Determine if we are executing a delay slot

   Looks at the instruction at the previous instruction to see if it was one
   with a delay slot.

   @param[in] gdbarch     The GDB architecture being used
   @param[in] this_frame  Information about THIS frame

   @return  1 (true) if this instruction is executing a delay slot, 0 (false)
   otherwise. */
/*--------------------------------------------------------------------------*/

static int
or1k_single_step_through_delay( struct gdbarch    *gdbarch,
				struct frame_info *this_frame )
{
  struct regcache   *regcache = get_current_regcache ();
  ULONGEST           val;
  CORE_ADDR          ppc;
  int                index;

  /* Get and decode the previous instruction. */
  regcache_cooked_read_unsigned (regcache, OR1K_PPC_REGNUM, &val);
  ppc        = (CORE_ADDR)val;
  index      = insn_decode (or1k_fetch_instruction (this_frame, ppc));

  /* We are only executing a delay slot if the previous instruction was a
     branch or jump. */
  return or32_opcodes[index].flags & OR32_IF_DELAY;

}	/* or1k_single_step_through_delay() */


/*----------------------------------------------------------------------------*/
/*!Read a pseudo register

   Since we have no pseudo registers this is a null function for now.

   @todo The floating point and vector registers ought to be done as
         pseudo-registers.

   @param[in]  gdbarch   The GDB architecture to consider
   @param[in]  regcache  The cached register values as an array
   @param[in]  regnum    The register to read
   @param[out] buf       A buffer to put the result in */
/*---------------------------------------------------------------------------*/

static void
or1k_pseudo_register_read (struct gdbarch  *gdbarch,
			   struct regcache *regcache,
			   int              regnum,
			   gdb_byte        *buf) 
{
  return;

}	/* or1k_pseudo_register_read() */


/*----------------------------------------------------------------------------*/
/*!Write a pseudo register

   Since we have no pseudo registers this is a null function for now.

   @todo The floating point and vector registers ought to be done as
         pseudo-registers.

   @param[in] gdbarch   The GDB architecture to consider
   @param[in] regcache  The cached register values as an array
   @param[in] regnum    The register to read
   @param[in] buf       A buffer with the value to write */
/*---------------------------------------------------------------------------*/

static void
or1k_pseudo_register_write (struct gdbarch  *gdbarch,
			    struct regcache *regcache,
			    int              regnum,
			    const gdb_byte  *buf) 
{
  return;

}	/* or1k_pseudo_register_write() */


/*----------------------------------------------------------------------------*/
/*!Return the register name for the OpenRISC 1000 architecture

   This version converted to ANSI C, made static and incorporates the static
   table of register names (this is the only place it is referenced).

   @todo The floating point and vector registers ought to be done as
         pseudo-registers.

   @param[in] gdbarch  The GDB architecture being used
   @param[in] regnum    The register number

   @return  The textual name of the register */
/*---------------------------------------------------------------------------*/

static const char *
or1k_register_name (struct gdbarch *gdbarch,
		    int             regnum) 
{
  static char *or1k_gdb_reg_names[OR1K_TOTAL_NUM_REGS] =
    {
      /* general purpose registers */
      "gpr0",  "gpr1",  "gpr2",  "gpr3",  "gpr4",  "gpr5",  "gpr6",  "gpr7",
      "gpr8",  "gpr9",  "gpr10", "gpr11", "gpr12", "gpr13", "gpr14", "gpr15",
      "gpr16", "gpr17", "gpr18", "gpr19", "gpr20", "gpr21", "gpr22", "gpr23",
      "gpr24", "gpr25", "gpr26", "gpr27", "gpr28", "gpr29", "gpr30", "gpr31",

      /* previous program counter, next program counter and status register */
      "ppc",   "npc",   "sr"

      /* Floating point and vector registers may appear as pseudo registers in
	 the future. */
    };

  return or1k_gdb_reg_names[regnum];

}	/* or1k_register_name() */


/*----------------------------------------------------------------------------*/
/*!Identify the type of a register

   @todo I don't fully understand exactly what this does, but I think this
         makes sense!

   @param[in] arch     The GDB architecture to consider
   @param[in] regnum   The register to identify

   @return  The type of the register */
/*---------------------------------------------------------------------------*/

static struct type *
or1k_register_type (struct gdbarch *arch,
		    int             regnum) 
{
  static struct type *void_func_ptr = NULL;
  static struct type *void_ptr      = NULL;

  /* Set up the static pointers once, the first time*/
  if (NULL == void_func_ptr)
    {
      void_ptr = lookup_pointer_type (builtin_type_void);
      void_func_ptr =
	lookup_pointer_type (lookup_function_type (builtin_type_void));
    }

  if((regnum >= 0) && (regnum < OR1K_TOTAL_NUM_REGS))
    {
      switch (regnum)
	{
	case OR1K_PPC_REGNUM:
	case OR1K_NPC_REGNUM:
	  return void_func_ptr;		/* Pointer to code */

	case OR1K_SP_REGNUM:
	case OR1K_FP_REGNUM:
	  return void_ptr;			/* Pointer to data */

	default:
	  return builtin_type_int32;	/* Data */
	}
    }

  internal_error (__FILE__, __LINE__,
		  _("or1k_register_type: illegal register number %d"), regnum);

}	/* or1k_register_type() */


/*----------------------------------------------------------------------------*/
/*!Handle the "info register" command

   Print the identified register, unless it is -1, in which case print all
   the registers. If all is 1 means all registers, otherwise only the core
   GPRs.

   @todo At present all registers are printed with the default method. Should
         there be something special for FP registers?

   @param[in] gdbarch  The GDB architecture being used
   @param[in] file     File handle for use with any custom I/O
   @param[in] frame    Frame info for use with custom output
   @param[in] regnum   Register of interest, or -1 if all registers
   @param[in] all      1 if all means all, 0 if all means just GPRs
  
   @return  The aligned stack frame address */
/*---------------------------------------------------------------------------*/

static void
or1k_registers_info (struct gdbarch    *gdbarch,
		     struct ui_file    *file,
		     struct frame_info *frame,
		     int                regnum,
		     int                all) 
{
  if (-1 == regnum)
    {
      /* Do all (valid) registers */
      unsigned int  lim = all ? OR1K_NUM_REGS : OR1K_MAX_GPR_REGS;

      for (regnum = 0; regnum < lim; regnum++) {
	if ('\0' != *(or1k_register_name (gdbarch, regnum)))
	  {
	    or1k_registers_info (gdbarch, file, frame, regnum, all);
	  }
      }
    }
  else
    {
      /* Do one specified register - if it is part of this architecture */
      if ('\0' == *(or1k_register_name (gdbarch, regnum)))
	{
	  error ("Not a valid register for the current processor type");
	}
      else
	{
	  default_print_registers_info (gdbarch, file, frame, regnum, all);
	}
    }
}	/* or1k_registers_info() */


/*----------------------------------------------------------------------------*/
/*!Identify if a register belongs to a specified group

   Return true if the specified register is a member of the specified
   register group.
  
   These are the groups of registers that can be displayed via "info reg".
  
   @todo The Vector and Floating Point registers ought to be displayed as
         pseudo-registers.
  
   @param[in] gdbarch  The GDB architecture to consider
   @param[in] regnum   The register to consider
   @param[in] group    The group to consider

   @return  True (1) if regnum is a member of group */
/*---------------------------------------------------------------------------*/

static int
or1k_register_reggroup_p (struct gdbarch  *gdbarch,
			  int              regnum,
			  struct reggroup *group) 
{
  struct gdbarch_tdep *tdep = gdbarch_tdep (gdbarch);

  /* All register group */
  if (group == all_reggroup)
    {
      return ((regnum >= 0) &&
	      (regnum < OR1K_TOTAL_NUM_REGS) &&
	      (or1k_register_name (gdbarch, regnum)[0] != '\0'));
    }

  /* For now everything except the PC */
  if (group == general_reggroup)
    {
      return ((regnum >= OR1K_ZERO_REGNUM) &&
	      (regnum <  tdep->num_gpr_regs) &&
	      (regnum != OR1K_PPC_REGNUM) &&
	      (regnum != OR1K_NPC_REGNUM));
    }

  if (group == float_reggroup)
    {
      return 0;			/* No float regs.  */
    }

  if (group == vector_reggroup)
    {
      return 0;			/* No vector regs.  */
    }

  /* For any that are not handled above.  */
  return default_register_reggroup_p (gdbarch, regnum, group);

}	/* or1k_register_reggroup_p() */


/*----------------------------------------------------------------------------*/
/*!Skip a function prolog

   If the input address, PC, is in a function prologue, return the address of
   the end of the prologue, otherwise return the input  address.

   @see For details of the stack frame, see the function
   or1k_frame_unwind_cache().

   This function reuses the helper functions from or1k_frame_unwind_cache() to
   locate the various parts of the prolog.

   This is very tricky. Essentially we look for the parts of a prolog.  If we
   get a mismatch, we never know if it is because we are not in prolog, or
   because the prolog is broken.

   @param[in] gdbarch  The GDB architecture being used
   @param[in] pc       Current program counter

   @return  The address of the end of the prolog if the PC is in a function
   prologue, otherwise the input  address. */
/*--------------------------------------------------------------------------*/

static CORE_ADDR
or1k_skip_prologue (struct gdbarch *gdbarch,
		    CORE_ADDR       pc) 
{
  enum {
    OR1K_FRAME_SIZE,
    OR1K_FP_SAVED,
    OR1K_NEW_FP,
    OR1K_LR_SAVE,
    OR1K_REG_SAVE,
    OR1K_NO_PROLOGUE
  } start_pos = OR1K_NO_PROLOGUE;

  CORE_ADDR  addr     = pc;
  int        frame_size;
  int        fp_save_offset;
  int        tmp;
  int        i;

  CORE_ADDR  start_addr;
  CORE_ADDR  end_addr;
  
  /* Try using SAL first */
  if (find_pc_partial_function (pc, NULL, &start_addr, &end_addr))
    {
      CORE_ADDR  prologue_end = skip_prologue_using_sal( pc );

      if (prologue_end > pc)
	{
	  return  prologue_end;
	}
      else
	{
	  return  pc;
	}
    }

  frame_size = or1k_frame_size (NULL, addr);

  if (0 != frame_size)
    {
      /* We seem to have the start of a prolog */
      start_pos = OR1K_FRAME_SIZE;
      addr += OR1K_INSTLEN;
    }

  /* Look for the previous frame pointer being saved. If we are in a frame,
     then this must be here. */
  fp_save_offset = or1k_frame_fp_loc (NULL, addr);

  switch (start_pos) 
   {
    case OR1K_FRAME_SIZE:
      if (-1 == fp_save_offset)
	{
	  error ("or1k_skip_prolog: "
		 "old frame pointer not saved at address %08llx: giving up\n",
		 (long long unsigned int)(ULONGEST)addr);
	}
      else
	{
	  addr += OR1K_INSTLEN;
	}

      break;

    default:
      start_pos  = OR1K_FP_SAVED;
      addr      += OR1K_INSTLEN;
      break;
    }

  /* Look for new FP being set up. This must match the frame_size if that is
     known. */
  tmp = or1k_frame_size_check (NULL, addr);
  switch (start_pos) 
   {
    case OR1K_FRAME_SIZE:
      if (frame_size != tmp)
	{
	  error ("or1k_skip_prolog: "
		 "frame pointer set to wrong size  at address %08llx: "
		 "expected %d, got %d\n", (long long unsigned int)(ULONGEST)addr, frame_size, tmp);
	}
      else
	{
	  addr += OR1K_INSTLEN;
	}

      break;

    case OR1K_FP_SAVED:
      if (-1 == tmp)
	{
	  error ("or1k_skip_prolog: "
		 "no frame pointer set up instruction at address %08llx\n",
		 (long long unsigned int)(ULONGEST)addr);
	}
      else
	{
	  addr += OR1K_INSTLEN;
	}

      break;

    default:
      if (-1 != tmp)
	{
	  start_pos  = OR1K_NEW_FP;
	  addr      += OR1K_INSTLEN;
	}
    }

  /* Look for the link register being saved. If we are in a prolog sequence,
     and is there then it should save to a particular location. */
  tmp = or1k_link_address (NULL, addr);
  switch (start_pos) 
   {
    case OR1K_FRAME_SIZE:
    case OR1K_FP_SAVED:
      if ((-1 != tmp) && (tmp != fp_save_offset - OR1K_INSTLEN))
	{
	  error ("or1k_skip_prolog: "
		 "link address saved to wrong offset at address %08llx: "
		 "expected %d, got %d\n", (long long unsigned int)(ULONGEST)addr,
		 fp_save_offset - OR1K_INSTLEN, tmp);
	}
      else
	{
	  addr += OR1K_INSTLEN;
	}

      break;

    default:
      if (-1 != tmp)
	{
	  start_pos  = OR1K_LR_SAVE;
	  addr      += OR1K_INSTLEN;
	}
    }

  /* Skip saved registers */
  for (i = 0;; i += OR1K_INSTLEN) 
   {
      int  regnum = or1k_get_saved_reg (NULL, addr, &tmp);

      switch (start_pos) 
	{
	case OR1K_FRAME_SIZE:
	case OR1K_FP_SAVED:
	  if (-1  != regnum)
	   {
	      if (tmp != fp_save_offset + ((i - 1) * OR1K_INSTLEN))
		{
		  error ("or1k_skip_prolog: callee register saved to wrong "
			 "offset at address %08llx: "
			 "expected %d, got %d\n", (long long unsigned int)(ULONGEST)addr,
			 fp_save_offset + ((i - 1) * OR1K_INSTLEN), tmp);
		}
	      else
		{
		  addr += 4;
		}
	    }
	  else
	   {
	      return  addr;
	    }

	  break;

	case OR1K_NEW_FP:
	case OR1K_LR_SAVE:
	case OR1K_REG_SAVE:
	  if (-1 != regnum)
	   {
	      addr += 4;
	    }
	  else
	   {
	      return  addr;
	    }

	default:
	  if (-1 != regnum)
	   {
	      start_pos  = OR1K_REG_SAVE;
	      addr += 4;
	    }
	  else
	   {
	      return  pc;		/* Not in a prolog */
	    }

	  break;
	}
    }
}	/* or1k_skip_prologue() */


/*----------------------------------------------------------------------------*/
/*!Align the stack frame

   OpenRISC 1000 uses a falling stack frame, so this aligns down to the
   nearest 8 bytes. Useful when we'be building a dummy frame.

   @param[in] gdbarch  The GDB architecture being used
   @param[in] sp       Current stack pointer

   @return  The aligned stack frame address */
/*---------------------------------------------------------------------------*/

static CORE_ADDR
or1k_frame_align (struct gdbarch *gdbarch,
		  CORE_ADDR       sp) 
{
  return align_down (sp, OR1K_STACK_ALIGN);

}	/* or1k_frame_align() */


/*----------------------------------------------------------------------------*/
/*!Unwind the program counter from a stack frame

   This just uses the built in frame unwinder

   @param[in] gdbarch     The GDB architecture being used
   @param[in] next_frame  Frame info for the NEXT frame
  
   @return  The program counter for THIS frame */
/*---------------------------------------------------------------------------*/

static CORE_ADDR
or1k_unwind_pc (struct gdbarch    *gdbarch,
		struct frame_info *next_frame) 
{
  return frame_unwind_register_unsigned (next_frame, OR1K_NPC_REGNUM);

}	/* or1k_unwind_pc() */


/*----------------------------------------------------------------------------*/
/*!Unwind the stack pointer from a stack frame

   This just uses the built in frame unwinder

   @param[in] gdbarch     The GDB architecture being used
   @param[in] next_frame  Frame info for the NEXT frame
  
   @return  The stack pointer for THIS frame */
/*---------------------------------------------------------------------------*/

static CORE_ADDR
or1k_unwind_sp (struct gdbarch    *gdbarch,
		struct frame_info *next_frame) 
{
  return frame_unwind_register_unsigned (next_frame, OR1K_SP_REGNUM);

}	/* or1k_unwind_sp() */


/*----------------------------------------------------------------------------*/
/*!Create a dummy stack frame

   The arguments are placed in registers and/or pushed on the stack as per the
   OR1K ABI.

   @param[in] gdbarch        The architecture to use
   @param[in] function       Pointer to the function that will be called
   @param[in] regcache       The register cache to use
   @param[in] bp_addr        Breakpoint address
   @param[in] nargs          Number of ags to push
   @param[in] args           The arguments
   @param[in] sp             The stack pointer
   @param[in] struct_return  True (1) if this returns a structure
   @param[in] struct_addr    Address for returning structures

   @return  The updated stack pointer */
/*---------------------------------------------------------------------------*/

static CORE_ADDR
or1k_push_dummy_call (struct gdbarch  *gdbarch,
		      struct value    *function,
		      struct regcache *regcache,
		      CORE_ADDR        bp_addr,
		      int              nargs,
		      struct value   **args,
		      CORE_ADDR        sp,
		      int              struct_return,
		      CORE_ADDR        struct_addr) 
{
  int           argreg;
  int           argnum;
  int           first_stack_arg;
  int           stack_offset = 0;

  unsigned int  bpa = (gdbarch_tdep (gdbarch))->bytes_per_address;
  unsigned int  bpw = (gdbarch_tdep (gdbarch))->bytes_per_word;

  /* Return address */
  regcache_cooked_write_unsigned (regcache, OR1K_LR_REGNUM, bp_addr);

  /* Register for the next argument */
  argreg = OR1K_FIRST_ARG_REGNUM;

  /* Location for a returned structure. This is passed as a silent first
     argument. */

  if (struct_return)
    {
      regcache_cooked_write_unsigned (regcache, OR1K_FIRST_ARG_REGNUM, 
				      struct_addr);
      argreg++;
    }

  /* Put as many args as possible in registers */
  for (argnum = 0; argnum < nargs; argnum++)
    {
      char           *val;
      char            valbuf[sizeof (ULONGEST) ];

      struct value   *arg      = args[argnum];
      struct type    *arg_type = check_typedef (value_type (arg));
      int             len      = arg_type->length;
      enum type_code  typecode = arg_type->main_type->code;

      /* The EABI passes structures that do not fit in a register by
	 reference. In all other cases, pass the structure by value.  */
      if((len > bpw) &&
	 ((TYPE_CODE_STRUCT == typecode) || (TYPE_CODE_UNION == typecode)))
	{

	  store_unsigned_integer (valbuf, bpa, value_offset (arg));
	  len      = bpa;
	  val      = valbuf;
	}
      else
	{
	  val = (char *)value_contents (arg);
	}   

      if((len > bpw) && (argreg <= (OR1K_LAST_ARG_REGNUM - 1)))
	{

	  /* Big scalars use two registers, must be pair aligned. This code
	     breaks if we can have quad-word scalars (e.g. long double). */
	  ULONGEST regval = extract_unsigned_integer (val, len);

	  gdb_assert (len <= (bpw * 2));

	  argreg = 1 == (argreg & 1) ? argreg + 1 : argreg;
	  regcache_cooked_write_unsigned (regcache, argreg, regval >> bpw);
	  regcache_cooked_write_unsigned (regcache, argreg + 1,
					  regval && ((ULONGEST)(1 << bpw) - 1));
	  argreg += 2;
	}
      else if (argreg <= OR1K_LAST_ARG_REGNUM)
	{
	  regcache_cooked_write_unsigned (regcache, argreg,
					  extract_unsigned_integer (val, len));
	  argreg++;
	}
      else
	{
	  /* Run out of regs */
	  break;
	}
    }

  first_stack_arg = argnum;

  /* If we get here with argnum < nargs, then arguments remain to be placed on
     the stack. This is tricky, since they must be pushed in reverse order and
     the stack in the end must be aligned. The only solution is to do it in
     two stages, the first to compute the stack size, the second to save the
     args. */

  for (argnum = first_stack_arg; argnum < nargs; argnum++)
    {
      struct value   *arg      = args[argnum];
      struct type    *arg_type = check_typedef (value_type (arg));
      int             len      = arg_type->length;
      enum type_code  typecode = arg_type->main_type->code;

      if((len > bpw) &&
	 ((TYPE_CODE_STRUCT == typecode) || (TYPE_CODE_UNION == typecode)))
	{
	  /* Large structures are passed as addresses */
	  sp -= bpa;
	}
      else
	{
	/* Big scalars use more than one word. Code here allows for future
	 quad-word entities (e.g. long double) */
	  sp -= ((len + bpw - 1) / bpw) * bpw;
	}
    }

  sp           = gdbarch_frame_align (gdbarch, sp);
  stack_offset = 0;

  /* Push the remaining args on the stack */
  for (argnum = first_stack_arg; argnum < nargs; argnum++)
    {
      char           *val;
      char            valbuf[sizeof (ULONGEST) ];

      struct value   *arg      = args[argnum];
      struct type    *arg_type = check_typedef (value_type (arg));
      int             len      = arg_type->length;
      enum type_code  typecode = arg_type->main_type->code;

      /* The EABI passes structures that do not fit in a register by
	 reference. In all other cases, pass the structure by value.  */
      if((len > bpw) &&
	 ((TYPE_CODE_STRUCT == typecode) || (TYPE_CODE_UNION == typecode)))
	{

	  store_unsigned_integer (valbuf, bpa, value_offset (arg));
	  len      = bpa;
	  val      = valbuf;
	}
      else
	{
	  val = (char *)value_contents (arg);
	}   

      gdb_assert (len <= (bpw * 2));

      write_memory (sp + stack_offset, val, len);
      stack_offset += ((len + bpw - 1) / bpw) * bpw;
    }

  /* Save the updated stack pointer */
  regcache_cooked_write_unsigned (regcache, OR1K_SP_REGNUM, sp);

  return sp;

}	/* or1k_push_dummy_call() */


/*----------------------------------------------------------------------------*/
/*!Unwind a dummy stack frame

   Tear down a dummy frame created by or1k_push_dummy_call(). This data has to
   be constructed manually from the data in our hand. The frame_id info in
   next_frame is not complete, and a call to unwind it will just recurse to us
   (we think).

   The stack pointer and program counter can be unwound. From the program
   counter, the start of the function can be determined.

   @param[in] gdbarch     The architecture to use
   @param[in] next_frame  Information about the next frame

   @return  Frame ID of the preceding frame */
/*---------------------------------------------------------------------------*/

static struct frame_id
or1k_unwind_dummy_id (struct gdbarch    *gdbarch,
		      struct frame_info *next_frame) 
{
  CORE_ADDR  this_sp = gdbarch_unwind_sp (gdbarch, next_frame);
  CORE_ADDR  this_pc = gdbarch_unwind_pc (gdbarch, next_frame);

  CORE_ADDR  start_addr;
  CORE_ADDR  end_addr;
  
  /* Try using SAL to find the true function start. Otherwise the PC will
     have to be a proxy for the start of the function. */
  if (find_pc_partial_function (this_pc, NULL, &start_addr, &end_addr))
    {
      return  frame_id_build (this_sp, start_addr);
    }
  else
    {
      return  frame_id_build (this_sp, this_pc);
    }

}	/* or1k_unwind_dummy_id() */


/*----------------------------------------------------------------------------*/
/*!The OpenRISC 1000 registered frame sniffer

   This function just identifies our family of frame sniffing functions.
  
   @param[in] next_frame  The "next" (i.e. inner, newer from here, the one
                          THIS frame called) frame.

			  @return  A pointer to a struct identifying the sniffing functions */
/*---------------------------------------------------------------------------*/

static const struct frame_unwind *
or1k_frame_sniffer (struct frame_info *next_frame)
{
  static const struct frame_unwind or1k_frame_unwind = {
    .type          = NORMAL_FRAME,
    .this_id       = or1k_frame_this_id,
    .prev_register = or1k_frame_prev_register,
    .unwind_data   = NULL,
    .sniffer       = NULL,
    .prev_pc       = NULL,
    .dealloc_cache = NULL
  };

  return  &or1k_frame_unwind;

}	/* or1k_frame_sniffer() */


/*----------------------------------------------------------------------------*/
/*!Architecture initialization for OpenRISC 1000

   Looks for a candidate architecture in the list of architectures supplied
   using the info supplied. If none match, create a new architecture.

   @param[in] info    Information about the target architecture
   @param[in] arches  The list of currently know architectures

   @return  A structure describing the target architecture */
/*---------------------------------------------------------------------------*/

static struct gdbarch *
or1k_gdbarch_init (struct gdbarch_info  info,
		   struct gdbarch_list *arches) 
{
  static struct frame_base     or1k_frame_base;
  struct        gdbarch       *gdbarch;
  struct        gdbarch_tdep  *tdep;
  const struct  bfd_arch_info *binfo;

  /* Find a candidate among the list of pre-declared architectures.  */
  arches = gdbarch_list_lookup_by_info (arches, &info);
  if (NULL != arches)
    {
      return arches->gdbarch;
    }

  /* None found, create a new architecture from the information
     provided. Can't initialize all the target dependencies until we actually
     know which target we are talking to, but put in some defaults for now. */

  binfo                   = info.bfd_arch_info;
  tdep                    = xmalloc (sizeof *tdep);
  tdep->num_matchpoints   = OR1K_MAX_MATCHPOINTS;
  tdep->num_gpr_regs      = OR1K_MAX_GPR_REGS;
  tdep->bytes_per_word    = binfo->bits_per_word    / binfo->bits_per_byte;
  tdep->bytes_per_address = binfo->bits_per_address / binfo->bits_per_byte;
  gdbarch                 = gdbarch_alloc (&info, tdep);

  /* Target data types.  */
  set_gdbarch_short_bit             (gdbarch, 16);
  set_gdbarch_int_bit               (gdbarch, 32);
  set_gdbarch_long_bit              (gdbarch, 32);
  set_gdbarch_long_long_bit         (gdbarch, 64);
  set_gdbarch_float_bit             (gdbarch, 32);
  set_gdbarch_float_format          (gdbarch, floatformats_ieee_single);
  set_gdbarch_double_bit            (gdbarch, 64);
  set_gdbarch_double_format         (gdbarch, floatformats_ieee_double);
  set_gdbarch_long_double_bit       (gdbarch, 64);
  set_gdbarch_long_double_format    (gdbarch, floatformats_ieee_double);
  set_gdbarch_ptr_bit               (gdbarch, binfo->bits_per_address);
  set_gdbarch_addr_bit              (gdbarch, binfo->bits_per_address);
  set_gdbarch_char_signed           (gdbarch, 1);

  /* Information about the target architecture */
  set_gdbarch_return_value          (gdbarch, or1k_return_value);
  set_gdbarch_breakpoint_from_pc    (gdbarch, or1k_breakpoint_from_pc);
  set_gdbarch_single_step_through_delay
                                    (gdbarch, or1k_single_step_through_delay);
  set_gdbarch_have_nonsteppable_watchpoint
                                    (gdbarch, 1);
  switch (gdbarch_byte_order (gdbarch))
    {
    case BFD_ENDIAN_BIG:
      set_gdbarch_print_insn        (gdbarch, print_insn_big_or32);
      break;

    case BFD_ENDIAN_LITTLE:
      set_gdbarch_print_insn        (gdbarch, print_insn_little_or32);
      break;

    case BFD_ENDIAN_UNKNOWN:
      error ("or1k_gdbarch_init: Unknown endianism");
      break;
    }

  /* Register architecture */
  set_gdbarch_pseudo_register_read  (gdbarch, or1k_pseudo_register_read);
  set_gdbarch_pseudo_register_write (gdbarch, or1k_pseudo_register_write);
  set_gdbarch_num_regs              (gdbarch, OR1K_NUM_REGS);
  set_gdbarch_num_pseudo_regs       (gdbarch, OR1K_NUM_PSEUDO_REGS);
  set_gdbarch_sp_regnum             (gdbarch, OR1K_SP_REGNUM);
  set_gdbarch_pc_regnum             (gdbarch, OR1K_NPC_REGNUM);
  set_gdbarch_ps_regnum             (gdbarch, OR1K_SR_REGNUM);
  set_gdbarch_deprecated_fp_regnum  (gdbarch, OR1K_FP_REGNUM);

  /* Functions to supply register information */
  set_gdbarch_register_name         (gdbarch, or1k_register_name);
  set_gdbarch_register_type         (gdbarch, or1k_register_type);
  set_gdbarch_print_registers_info  (gdbarch, or1k_registers_info);
  set_gdbarch_register_reggroup_p   (gdbarch, or1k_register_reggroup_p);

  /* Functions to analyse frames */
  set_gdbarch_skip_prologue         (gdbarch, or1k_skip_prologue);
  set_gdbarch_inner_than            (gdbarch, core_addr_lessthan);
  set_gdbarch_frame_align           (gdbarch, or1k_frame_align);
  set_gdbarch_frame_red_zone_size   (gdbarch, OR1K_FRAME_RED_ZONE_SIZE);

  /* Functions to access frame data */
  set_gdbarch_unwind_pc             (gdbarch, or1k_unwind_pc);
  set_gdbarch_unwind_sp             (gdbarch, or1k_unwind_sp);

  /* Functions handling dummy frames */
  set_gdbarch_push_dummy_call       (gdbarch, or1k_push_dummy_call);
  set_gdbarch_unwind_dummy_id       (gdbarch, or1k_unwind_dummy_id);

  /* High level frame base sniffer */
  or1k_frame_base.unwind      = or1k_frame_sniffer (NULL);
  or1k_frame_base.this_base   = or1k_frame_base_address;
  or1k_frame_base.this_locals = or1k_frame_base_address;
  or1k_frame_base.this_args   = or1k_frame_base_address;
  frame_base_set_default            (gdbarch, &or1k_frame_base);

  /* Low level frame sniffers */
  frame_unwind_append_sniffer       (gdbarch, dwarf2_frame_sniffer);
  frame_unwind_append_sniffer       (gdbarch, or1k_frame_sniffer);


  return gdbarch;

}	/* or1k_gdbarch_init() */


/*----------------------------------------------------------------------------*/
/*!Dump the target specific data for this architecture

   @param[in] gdbarch  The architecture of interest
   @param[in] file     Where to dump the data */
/*---------------------------------------------------------------------------*/

static void
or1k_dump_tdep (struct gdbarch *gdbarch,
		struct ui_file *file) 
{
  struct gdbarch_tdep *tdep = gdbarch_tdep (gdbarch);

  if (NULL == tdep)
    {
      return;			/* Nothing to report */
    }

  fprintf_unfiltered (file, "or1k_dump_tdep: %d matchpoints available\n",
		      tdep->num_matchpoints);
  fprintf_unfiltered (file, "or1k_dump_tdep: %d general purpose registers\n",
		      tdep->num_gpr_regs);
  fprintf_unfiltered (file, "or1k_dump_tdep: %d bytes per word\n",
		      tdep->bytes_per_word);
  fprintf_unfiltered (file, "or1k_dump_tdep: %d bytes per address\n",
		      tdep->bytes_per_address);

}	/* or1k_dump_tdep() */



/* Functions to add extra commands to GDB */


/*----------------------------------------------------------------------------*/
/*!Returns a special purpose register group name

   @param[in]  group  The SPR group number

   @return  The SPR name (pointer to the name argument) */
/*---------------------------------------------------------------------------*/

static const char *
or1k_spr_group_name (int  group) 
{
  static const char *or1k_group_names[OR1K_NUM_SPGS] =
    {
      "SYS",
      "DMMU",
      "IMMU",
      "DCACHE",
      "ICACHE",
      "MAC",
      "DEBUG",
      "PERF",
      "POWER",
      "PIC",
      "TIMER",
      "FPU"
    };

  if ((0 <= group) && (group < OR1K_NUM_SPGS))
    {
      return or1k_group_names[group];
    }
  else
    {
      return "";
    }
}	/* or1k_spr_group_name() */


/*----------------------------------------------------------------------------*/
/*!Returns a special purpose register name

   @param[in]  group  The SPR group
   @param[in]  index  The index within the SPR group
   @param[out] name   Array to put the name in

   @return  The SPR name (pointer to the name argument) */
/*---------------------------------------------------------------------------*/

static char *
or1k_spr_register_name (int   group,
			int   index,
			char *name) 
{
  char di;

  switch (group)
    {

    case OR1K_SPG_SYS:
      /* 1:1 names */
      switch (index)
	{
	case OR1K_SPG_SYS_VR:       sprintf (name, "VR"      ); return  name;
	case OR1K_SPG_SYS_UPR:      sprintf (name, "UPR"     ); return  name;
	case OR1K_SPG_SYS_CPUCFGR:  sprintf (name, "CPUCFGR" ); return  name;
	case OR1K_SPG_SYS_DMMUCFGR: sprintf (name, "DMMUCFGR"); return  name;
	case OR1K_SPG_SYS_IMMUCFGR: sprintf (name, "IMMUCFGR"); return  name;
	case OR1K_SPG_SYS_DCCFGR:   sprintf (name, "DCCFGR"  ); return  name;
	case OR1K_SPG_SYS_ICCFGR:   sprintf (name, "ICCFGR"  ); return  name;
	case OR1K_SPG_SYS_DCFGR:    sprintf (name, "DCFGR"   ); return  name;
	case OR1K_SPG_SYS_PCCFGR:   sprintf (name, "PCCFGR"  ); return  name;
	case OR1K_SPG_SYS_NPC:      sprintf (name, "NPC"     ); return  name;
	case OR1K_SPG_SYS_SR:       sprintf (name, "SR"      ); return  name;
	case OR1K_SPG_SYS_PPC:      sprintf (name, "PPC"     ); return  name;
	case OR1K_SPG_SYS_FPCSR:    sprintf (name, "FPCSR"   ); return  name;
	}

      /* Exception PC regs */
      if((OR1K_SPG_SYS_EPCR <= index) &&
	 (index             <= OR1K_SPG_SYS_EPCR_END))
	{
	  sprintf (name, "EPCR%d", index - OR1K_SPG_SYS_EPCR);
	  return  name;
	}

      /* Exception EA regs */
      if((OR1K_SPG_SYS_EEAR <= index) &&
	 (index             <= OR1K_SPG_SYS_EEAR_END))
	{
	  sprintf (name, "EEAR%d", index - OR1K_SPG_SYS_EEAR);
	  return  name;
	}

      /* Exception SR regs */
      if((OR1K_SPG_SYS_ESR <= index) &&
	 (index            <= OR1K_SPG_SYS_ESR_END))
	{
	  sprintf (name, "ESR%d", index - OR1K_SPG_SYS_ESR);
	  return  name;
	}

      /* GPRs */
      if((OR1K_SPG_SYS_GPR <= index) &&
	 (index            <= OR1K_SPG_SYS_GPR_END))
	{
	  sprintf (name, "GPR%d", index - OR1K_SPG_SYS_GPR);
	  return  name;
	}

      break;

    case OR1K_SPG_DMMU:
    case OR1K_SPG_IMMU:
      /* MMU registers. Use DMMU constants throughout, but these are identical
	 to the corresponding IMMU constants */
      di = OR1K_SPG_DMMU == group ? 'D' : 'I';

      /* 1:1 names */
      switch (index)
	{
	case OR1K_SPG_DMMU_DMMUCR:
	  sprintf (name, "%cMMUCR",  di); return  name;
	case OR1K_SPG_DMMU_DMMUPR:
	  sprintf (name, "%cMMUPR",  di); return  name;
	case OR1K_SPG_DMMU_DTLBEIR:
	  sprintf (name, "%cTLBEIR", di); return  name;
	}

      /* ATB Match registers */
      if((OR1K_SPG_DMMU_DATBMR <= index) &&
	 (index                <= OR1K_SPG_DMMU_DATBMR_END))
	{
	  sprintf (name, "%cATBMR%d", di, index - OR1K_SPG_DMMU_DATBMR);
	  return  name;
	}

      /* ATB Translate registers */
      if((OR1K_SPG_DMMU_DATBTR <= index) &&
	 (index                <= OR1K_SPG_DMMU_DATBTR_END))
	{
	  sprintf (name, "%cATBTR%d", di, index - OR1K_SPG_DMMU_DATBTR);
	  return  name;
	}

      /* TLB Way 1 Match registers */
      if((OR1K_SPG_DMMU_DTLBW1MR <= index) &&
	 (index                <= OR1K_SPG_DMMU_DTLBW1MR_END))
	{
	  sprintf (name, "%cTLBW1MR%d", di, index - OR1K_SPG_DMMU_DTLBW1MR);
	  return  name;
	}

      /* TLB Way 1 Translate registers */
      if((OR1K_SPG_DMMU_DTLBW1TR <= index) &&
	 (index                <= OR1K_SPG_DMMU_DTLBW1TR_END))
	{
	  sprintf (name, "%cTLBW1TR%d", di, index - OR1K_SPG_DMMU_DTLBW1TR);
	  return  name;
	}

      /* TLB Way 2 Match registers */
      if((OR1K_SPG_DMMU_DTLBW2MR <= index) &&
	 (index                <= OR1K_SPG_DMMU_DTLBW2MR_END))
	{
	  sprintf (name, "%cTLBW2MR%d", di, index - OR1K_SPG_DMMU_DTLBW2MR);
	  return  name;
	}

      /* TLB Way 2 Translate registers */
      if((OR1K_SPG_DMMU_DTLBW2TR <= index) &&
	 (index                <= OR1K_SPG_DMMU_DTLBW2TR_END))
	{
	  sprintf (name, "%cTLBW2TR%d", di, index - OR1K_SPG_DMMU_DTLBW2TR);
	  return  name;
	}

      /* TLB Way 3 Match registers */
      if((OR1K_SPG_DMMU_DTLBW3MR <= index) &&
	 (index                <= OR1K_SPG_DMMU_DTLBW3MR_END))
	{
	  sprintf (name, "%cTLBW3MR%d", di, index - OR1K_SPG_DMMU_DTLBW3MR);
	  return  name;
	}

      /* TLB Way 3 Translate registers */
      if((OR1K_SPG_DMMU_DTLBW3TR <= index) &&
	 (index                <= OR1K_SPG_DMMU_DTLBW3TR_END))
	{
	  sprintf (name, "%cTLBW3TR%d", di, index - OR1K_SPG_DMMU_DTLBW3TR);
	  return  name;
	}

      break;

    case OR1K_SPG_DC:
      /* Data cache registers. These do not have an exact correspondence with
	 their instruction cache counterparts, so must be done separately. */

      /* 1:1 names */
      switch (index)
	{
	case OR1K_SPG_DC_DCCR:  sprintf (name, "DCCR" ); return  name;
	case OR1K_SPG_DC_DCBPR: sprintf (name, "DCBPR"); return  name;
	case OR1K_SPG_DC_DCBFR: sprintf (name, "DCBFR"); return  name;
	case OR1K_SPG_DC_DCBIR: sprintf (name, "DCBIR"); return  name;
	case OR1K_SPG_DC_DCBWR: sprintf (name, "DCBWR"); return  name;
	case OR1K_SPG_DC_DCBLR: sprintf (name, "DCBLR"); return  name;
	}

      break;

    case OR1K_SPG_IC:
      /* Instruction cache registers */

      /* 1:1 names */
      switch (index)
	{
	case OR1K_SPG_IC_ICCR:  sprintf (name, "ICCR" ); return  name;
	case OR1K_SPG_IC_ICBPR: sprintf (name, "ICBPR"); return  name;
	case OR1K_SPG_IC_ICBIR: sprintf (name, "ICBIR"); return  name;
	case OR1K_SPG_IC_ICBLR: sprintf (name, "ICBLR"); return  name;
	}

      break;

    case OR1K_SPG_MAC:
      /* MAC registers */

      /* 1:1 names */
      switch (index)
	{
	case OR1K_SPG_MAC_MACLO: sprintf (name, "MACLO"); return  name;
	case OR1K_SPG_MAC_MACHI: sprintf (name, "MACHI"); return  name;
	}

      break;

    case OR1K_SPG_DEBUG:
      /* Debug registers */

      /* Debug Value registers */
      if((OR1K_SPG_DEBUG_DVR <= index) &&
	 (index                <= OR1K_SPG_DEBUG_DVR_END))
	{
	  sprintf (name, "DVR%d", index - OR1K_SPG_DEBUG_DVR);
	  return  name;
	}

      /* Debug Control registers */
      if((OR1K_SPG_DEBUG_DCR <= index) &&
	 (index                <= OR1K_SPG_DEBUG_DCR_END))
	{
	  sprintf (name, "DCR%d", index - OR1K_SPG_DEBUG_DCR);
	  return  name;
	}

      /* 1:1 names */
      switch (index)
	{
	case OR1K_SPG_DEBUG_DMR1:  sprintf (name, "DMR1" ); return  name;
	case OR1K_SPG_DEBUG_DMR2:  sprintf (name, "DMR2" ); return  name;
	case OR1K_SPG_DEBUG_DCWR0: sprintf (name, "DCWR0"); return  name;
	case OR1K_SPG_DEBUG_DCWR1: sprintf (name, "DCWR1"); return  name;
	case OR1K_SPG_DEBUG_DSR:   sprintf (name, "DSR"  ); return  name;
	case OR1K_SPG_DEBUG_DRR:   sprintf (name, "DRR"  ); return  name;
	}

      break;

    case OR1K_SPG_PC:
      /* Performance Counter registers */

      /* Performance Counters Count registers */
      if((OR1K_SPG_PC_PCCR <= index) &&
	 (index                <= OR1K_SPG_PC_PCCR_END))
	{
	  sprintf (name, "PCCR%d", index - OR1K_SPG_PC_PCCR);
	  return  name;
	}

      /* Performance Counters Mode registers */
      if((OR1K_SPG_PC_PCMR <= index) &&
	 (index                <= OR1K_SPG_PC_PCMR_END))
	{
	  sprintf (name, "PCMR%d", index - OR1K_SPG_PC_PCMR);
	  return  name;
	}

      break;

    case OR1K_SPG_PM:
      /* Power Management registers */

      /* 1:1 names */
      switch (index)
	{
	case OR1K_SPG_PM_PMR:  sprintf (name, "PMR"); return  name;
	}

      break;

    case OR1K_SPG_PIC:
      /* Programmable Interrupt Controller registers */

      /* 1:1 names */
      switch (index)
	{
	case OR1K_SPG_PIC_PICMR:  sprintf (name, "PICMR"); return  name;
	case OR1K_SPG_PIC_PICSR:  sprintf (name, "PICSR"); return  name;
	}

      break;

    case OR1K_SPG_TT:
      /* Tick Timer registers */

      /* 1:1 names */
      switch (index)
	{
	case OR1K_SPG_TT_TTMR:  sprintf (name, "TTMR"); return  name;
	case OR1K_SPG_TT_TTCR:  sprintf (name, "TTCR"); return  name;
	}

      break;

    case OR1K_SPG_FPU:

      break;
    }

  /* Not a recognized register */
  strcpy (name, "");
  return  name;

}	/* or1k_spr_register_name() */


/*----------------------------------------------------------------------------*/
/*!Get SPR group number from a name

   @param[in] group_name  SPR register group

   @return  The index, or negative if no match. */
/*----------------------------------------------------------------------------*/

static int
or1k_groupnum_from_name (char *group_name) 
{
  int  group;

  for (group = 0; group < OR1K_NUM_SPGS; group++)
    {
      if (0 == strcasecmp (group_name, or1k_spr_group_name (group)))
	{
	  return group;
	}
    }

  return -1;

}	/* or1k_groupnum_from_name() */


/*----------------------------------------------------------------------------*/
/*!Get register index in special purpose register group from name

   The name may either be SPR<group_num>_<index> or a known unique name. In
   either case the group number must match the supplied group number.

   @param[in] group  SPR register group
   @param[in] name   Register name

   @return  The index, or negative if no match. */
/*----------------------------------------------------------------------------*/

static int
or1k_regnum_from_name (int   group,
		       char *name) 
{
  /* Last valid register in each group. */
  static const int  or1k_spr_group_last[OR1K_NUM_SPGS] =
    {
      OR1K_SPG_SYS_LAST,
      OR1K_SPG_DMMU_LAST,
      OR1K_SPG_IMMU_LAST,
      OR1K_SPG_DC_LAST,
      OR1K_SPG_IC_LAST,
      OR1K_SPG_MAC_LAST,
      OR1K_SPG_DEBUG_LAST,
      OR1K_SPG_PC_LAST,
      OR1K_SPG_PM_LAST,
      OR1K_SPG_PIC_LAST,
      OR1K_SPG_TT_LAST,
      OR1K_SPG_FPU_LAST
    };

  int  i;
  char  spr_name[32];

  if (0 == strcasecmp (name, "SPR"))
    {
      char *ptr_c;      

      /* Skip SPR */
      name += 3;

      /* Get group number */
      i = (int) strtoul (name, &ptr_c, 10);
      if (*ptr_c != '_' || i != group)
	{
	  return -1;
	}

      /* Get index */
      ptr_c++;
      i = (int) strtoul (name, &ptr_c, 10);
      if (*ptr_c)
	{
	  return -1;
	}
      else
	{
	  return  i;
	}
    }

  /* Look for a "known" name in this group */
  for (i = 0; i <= or1k_spr_group_last[group]; i++)
    {
      char *s = or1k_spr_register_name (group, i, spr_name);

      if (0 == strcasecmp (name, s))
	{
	  return i;
	}
    }

  /* Failure */
  return -1;

}	/* or1k_regnum_from_name() */


/*----------------------------------------------------------------------------*/
/*!Get the next token from a string

   I can't believe there isn't a library argument for this, but strtok is
   deprecated.

   Take a string and find the start of the next token and its length. A token
   is anything containing non-blank characters.

   @param[in]  str  The string to look at (may be NULL).
   @param[out] tok  Pointer to the start of the token within str. May be NULL
                    if this result is not wanted (e.g. just the length is
		    wanted. If no token is found will be the NULL char at the
		    end of the string, if the original str was NULL, this will
		    be NULL.

		    @return  The length of the token found */
/*----------------------------------------------------------------------------*/

static int
or1k_tokenize (char  *str,
	       char **tok) 
{
  char *ptr;
  int   len;

  /* Deal with NULL argument */
  if (NULL == str)
    {
      if (NULL != tok)
	{
	  *tok = NULL;
	}
      return 0;
    }

  /* Find the start */
  for (ptr = str; ISBLANK (*ptr) ; ptr++)
    {
      continue;
    }

  /* Return the start pointer if requested */
  if (NULL != tok)
    {
      *tok = ptr;
    }

  /* Find the end and put in EOS */
  for (len = 0;  ('\0' != ptr[len]) && (!ISBLANK (ptr[len])); len++)
    {
      continue;
    }

  return len;

}	/* or1k_tokenize() */


/*----------------------------------------------------------------------------*/
/*!Parses args for spr commands

   Determines the special purpose register (SPR) name and puts result into
   group and index

   Syntax is:

   @verbatim
   <spr_args>    -> <group_ref> | <reg_name>
   <group_ref>   -> <group_id> <index>
   <group_id>    -> <group_num> | <group_name>
   @endverbatim

   Where the indices/names have to be valid.

   So to parse, we look for 1 or 2 args. If 1 it must be a unique register
   name. If 2, the first must be a group number or name and the second an
   index within that group.

   Also responsible for providing diagnostics if the arguments do not match.

   Rewritten for GDB 6.8 to use the new UI calls and remove assorted
   bugs. Syntax also slightly restricted to be more comprehensible.

   @param[in]  arg_str  The argument string
   @param[out] group    The group this SPR belongs in, or -1 to indicate
                        failure
   @param[out] index    Index of the register within the group, or -1 to
                        indicate the whole group
   @param[in]  is_set   1 (true) if we are called from the "spr" command (so
                        there is an extra arg) rather than the "info spr"
                        command. Needed to distinguish between the case where
                        info is sought from a register specified as group and
                        index and setting a uniquely identified register to a
                        value.

			@return  A pointer to any remaining args */
/*---------------------------------------------------------------------------*/

static char *
or1k_parse_spr_params (char *arg_str,
		       int  *group,
		       int  *index,
		       int   is_set) 
{
  struct {
    char              *str;
    int                len;
    unsigned long int  val;
    int                is_num;
  } arg[3] = {
    {
      .str    = NULL,
      .len    = 0,
      .val    = 0,
      .is_num = 0,
    },
   {
      .str    = NULL,
      .len    = 0,
      .val    = 0,
      .is_num = 0,
    },
   {
      .str    = NULL,
      .len    = 0,
      .val    = 0,
      .is_num = 0,
    }
  };

  int   num_args;
  char *trailer  = arg_str;
  char *tmp_str;
  int   i;

  char  spr_name[32];

  /* Break out the arguments. Note that the strings are NOT null terminated
     (we don't want to change arg_str), so we must rely on len. The stroul
     call will still work, since there is always a non-digit char (possibly EOS)
     after the last digit. */
  if (NULL == arg_str)
    {
      num_args = 0;
    }
  else
    {
      for (num_args = 0; num_args < 3; num_args++)
	{
	  arg[num_args].len = or1k_tokenize (trailer, &(arg[num_args].str));
	  trailer           = arg[num_args].str + arg[num_args].len;

	  if (0 == arg[num_args].len)
	    {
	      break;
	    }
	}
    }

  /* Patch nulls into the arg strings and see about values. Couldn't do this
     earlier, since we needed the next char clean to check later args. This
     means advancing trailer, UNLESS it was already at EOS */

  if((NULL != arg_str) && ('\0' != *trailer))
    {
      trailer++;
    }

  for (i = 0; i < num_args; i++)
    {
      (arg[i].str)[arg[i].len] = '\0';
      errno                    = 0;
      arg[i].val               = strtoul (arg[i].str, &tmp_str, 0);
      arg[i].is_num            = (0 == errno) && ('\0' == *tmp_str);
    }

  /* Deal with the case where we are setting a register, so the final argument
     should be disregarded (it is the trailer). Do this anyway if we get a
     third argument */
  if ((is_set & (num_args > 0)) || (num_args > 2))
    {
      trailer = arg[num_args - 1].str;
      num_args--;
    }

  /* Deal with different numbers of args */

  switch (num_args)
    {

    case 0:
      ui_out_message (uiout, 0,
		      "Usage: <command> <register>      |\n"
		      "       <command> <group>         |\n"
		      "       <command> <group> <index>\n"
		      "Valid groups are:\n");
      for (i = 0; i < OR1K_NUM_SPGS; i++)
	{
	  ui_out_field_string (uiout, NULL, or1k_spr_group_name  (i));
	  ui_out_spaces (uiout, 1);
	  ui_out_wrap_hint (uiout, NULL);
	}
      ui_out_field_string (uiout, NULL, "\n");

      *index = -1;
      return  trailer;

    case 1:
      /* See if it is a numeric group */
      if (arg[0].is_num)
	{
	  if (arg[0].val < OR1K_NUM_SPGS)
	    {
	      *group = arg[0].val;
	      *index = -1;
	      return trailer;
	    }
	  else
	    {
	      ui_out_message (uiout, 0,
			      "Group index should be in the range 0 - %d\n",
			      OR1K_NUM_SPGS);
	      *group = -1;
	      *index = -1;
	      return trailer;
	    }
	}	

      /* Is is it a group name? */
      *group = or1k_groupnum_from_name (arg[0].str);
      if (*group >= 0)
	{
	  *index = -1;
	  return trailer;
	}

      /* See if it is a valid register name in any group */
      for (*group = 0; *group < OR1K_NUM_SPGS; (*group)++)
	{
	  *index = or1k_regnum_from_name (*group, arg[0].str);
      
	  if (*index >= 0)
	    {
	      return  trailer;
	    }
	}

      /* Couldn't find it - print out a rude message */
      ui_out_message (uiout, 0,
		      "Group or register name not recognized.\n"
		      "Valid groups are:\n");
      for (i = 0; i < OR1K_NUM_SPGS; i++)
	{
	  ui_out_field_string (uiout, NULL, or1k_spr_group_name (i));
	  ui_out_spaces (uiout, 1);
	  ui_out_wrap_hint (uiout, NULL);
	}
      ui_out_field_string (uiout, NULL, "\n");

      *group = -1;
      *index = -1;
      return  trailer;

    case 2:
      /* See if first arg is a numeric group */
      if (arg[0].is_num)
	{
	  if (arg[0].val < OR1K_NUM_SPGS)
	    {
	      *group = arg[0].val;
	      *index = -1;
	    }
	  else
	    {
	      ui_out_message (uiout, 0,
			      "Group index should be in the range 0 - %d\n",
			      OR1K_NUM_SPGS - 1);
	      *group = -1;
	      *index = -1;
	      return trailer;
	    }
	}	
      else
	{
	  /* Is is it a group name? */
	  *group = or1k_groupnum_from_name (arg[0].str);
	  if (*group >= 0)
	    {
	      *index = -1;
	    }
	  else
	    {
	      ui_out_message (uiout, 0,
			      "Group name not recognized.\n"
			      "Valid groups are:\n");
	      for (i = 0; i < OR1K_NUM_SPGS; i++)
		{
		  ui_out_field_string (uiout, NULL, or1k_spr_group_name (i));
		  ui_out_spaces (uiout, 1);
		  ui_out_wrap_hint (uiout, NULL);
		}
	      ui_out_field_string (uiout, NULL, "\n");

	      *group = -1;
	      *index = -1;
	      return  trailer;
	    }
	}

      /* Is second arg an index or name? */
      if (arg[1].is_num)
	{
	  if (arg[1].val < OR1K_SPG_SIZE)
	    {
	      /* Check this really is a register */
	      if (0 != strlen (or1k_spr_register_name (*group, arg[1].val,
						       spr_name)))
		{
		  *index = arg[1].val;
		  return trailer;
		}
	      else
		{
		  ui_out_message (uiout, 0,
				  "No valid register at that index in group\n");
		  *group = -1;
		  *index = -1;
		  return  trailer;
		}
	    }
	  else
	    {
	      ui_out_message (uiout, 0,
			      "Register index should be in the range 0 - %d\n",
			      OR1K_SPG_SIZE - 1);
	      *group = -1;
	      *index = -1;
	      return  trailer;
	    }
	}

      /* Must be a name */
      *index = or1k_regnum_from_name (*group, arg[1].str);
      
      if (*index >= 0)
	{
	  return trailer;
	}

      /* Couldn't find it - print out a rude message */
      ui_out_message (uiout, 0, "Register name not recognized in group.\n");
      *group = -1;
      *index = -1;
      return  trailer;
    
    default:
      /* Anything else is an error */
      ui_out_message (uiout, 0, "Unable to parse arguments\n");
      *group = -1;
      *index = -1;
      return  trailer;
    }
}	/* or1k_parse_spr_params() */


/*---------------------------------------------------------------------------*/
/*!Read a special purpose register from the target

   This has to be done using the target remote command "readspr"

   @param[in] regnum  The register to read

   @return  The value read */
/*---------------------------------------------------------------------------*/

static ULONGEST
or1k_read_spr (unsigned int  regnum) 
{
  struct ui_file    *uibuf = mem_fileopen ();
  char               cmd[sizeof ("readspr ffff")];
  unsigned long int  data;
  char              *res;
  long int           len;

  /* Create the command string and pass it to target remote command function */
  sprintf (cmd, "readspr %4x", regnum);
  target_rcmd (cmd, uibuf);

  /* Get the output for the UI file as a string */
  res = ui_file_xstrdup (uibuf, &len);
  sscanf (res, "%lx", &data);

  /* Tidy up */
  xfree (res);
  ui_file_delete (uibuf);

  return  (ULONGEST)data;

}	/* or1k_read_spr() */


/*---------------------------------------------------------------------------*/
/*!Write a special purpose register on the target

   This has to be done using the target remote command "writespr"

   Since the SPRs may map to GPR's or the other GDB register (PPC, NPC, SR),
   any register cache is flushed.

   @param[in] regnum  The register to write
   @param[in] data  The value to write */
/*---------------------------------------------------------------------------*/

static void
or1k_write_spr (unsigned int  regnum,
		ULONGEST      data) 
{
  struct ui_file    *uibuf = mem_fileopen ();
  char               cmd[sizeof ("writespr ffff ffffffff")];
  char              *res;
  long int           len;

  /* Create the command string and pass it to target remote command function */
  sprintf (cmd, "writespr %4x %8llx", regnum, (long long unsigned int)data);
  target_rcmd (cmd, uibuf);

  /* Flush the register cache */
  registers_changed ();

  /* We ignore the result - Rcmd can put out its own error messages. Just
     tidy up */
  ui_file_delete (uibuf);

}	/* or1k_write_spr() */


/*----------------------------------------------------------------------------*/
/*!Show the value of a special purpose register or group

   This is a custom extension to the GDB info command.

   @param[in] args
   @param[in] from_tty  True (1) if GDB is running from a TTY, false (0)
   otherwise. */
/*---------------------------------------------------------------------------*/

static void
or1k_info_spr_command (char *args,
		       int   from_tty) 
{
  int  group;
  int  index;

  char  spr_name[32];

  or1k_parse_spr_params (args, &group, &index, 0);

  if (group < 0)
    {
      return;			/* Couldn't parse the args */
    }

  if (index >= 0)
    {
      ULONGEST  value = or1k_read_spr (OR1K_SPR (group, index));

      ui_out_field_fmt (uiout, NULL, "%s.%s = SPR%i_%i = %llu (0x%llx)\n",
			or1k_spr_group_name (group), 
			or1k_spr_register_name (group, index, spr_name), group,
			index, (long long unsigned int)value, (long long unsigned int)value);
    }
  else
    {
      /* Print all valid registers in the group */
      for (index = 0; index < OR1K_SPG_SIZE; index++)
	{
	  if (0 != strlen (or1k_spr_register_name (group, index, spr_name)))
	    {
	      ULONGEST  value = or1k_read_spr (OR1K_SPR (group, index));

	      ui_out_field_fmt (uiout, NULL,
				"%s.%s = SPR%i_%i = %llu (0x%llx)\n",
				or1k_spr_group_name (group),
				or1k_spr_register_name (group, index, spr_name),
				group, index, (long long unsigned int)value, (long long unsigned int)value);
	    }
	}
    }
}	/* or1k_info_spr_command() */


/*----------------------------------------------------------------------------*/
/*!Set a special purpose register

   This is a custom command added to GDB.

   @param[in] args
   @param[in] from_tty  True (1) if GDB is running from a TTY, false (0)
   otherwise. */
/*---------------------------------------------------------------------------*/

static void
or1k_spr_command (char *args,
		  int   from_tty) 
{
  int   group;
  int   index;
  char *tmp_str;
  char *nargs = or1k_parse_spr_params (args, &group, &index, 1);

  ULONGEST  old_val;
  ULONGEST  new_val;

  char  spr_name[32];

  /* Do we have a valid register spec? */
  if (index < 0)
    {
      return;		/* Parser will have printed the error message */
    }

  /* Do we have a value to set? */

  errno = 0;
  new_val = (ULONGEST)strtoul (nargs, &tmp_str, 0);

  if((0 != errno) || ('\0' != *tmp_str))
    {
      ui_out_message (uiout, 0, "Invalid value - register not changed\n");
      return;
    }

  old_val = or1k_read_spr (OR1K_SPR (group, index));

  or1k_write_spr (OR1K_SPR (group, index) , new_val);

  ui_out_field_fmt (uiout, NULL,
		    "%s.%s (SPR%i_%i) set to %llu (0x%llx), "
		    "was: %llu (0x%llx)\n",
		    or1k_spr_group_name (group),
		    or1k_spr_register_name (group, index, spr_name) , group,
		    index, (long long unsigned int)new_val, (long long unsigned int)new_val, (long long unsigned int)old_val, (long long unsigned int)old_val);

}	/* or1k_spr_command() */


/*----------------------------------------------------------------------------*/
/*!Main entry point for target architecture initialization

   In this version initializes the architecture via
   registers_gdbarch_init(). Add a command to set and show special purpose
   registers. */
/*---------------------------------------------------------------------------*/

void
_initialize_or1k_tdep (void) 
{
  /* Register this architecture. We should do this for or16 and or64 when
     they have their BFD defined. */
  gdbarch_register (bfd_arch_or32, or1k_gdbarch_init, or1k_dump_tdep);

  /* Initialize the automata for the assembler */
  build_automata();

  /* Commands to show and set special purpose registers */
  add_info ("spr", or1k_info_spr_command,
	    "Show the value of a special purpose register");
  add_com ("spr", class_support, or1k_spr_command,
	   "Set a special purpose register");

}	/* _initialize_or1k_tdep() */
