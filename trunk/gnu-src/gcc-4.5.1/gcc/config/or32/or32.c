/* Subroutines for insn-output.c for GNU compiler.  OpenRISC 1000 version.
   Copyright (C) 1987, 1992, 1997, 1999, 2000, 2001, 2002, 2003, 2004,
   2005, 2006, 2007, 2008, 2009, 2010  Free Software Foundation, Inc
   Copyright (C) 2010 Embecosm Limited

   Contributed by Damjan Lampret <damjanl@bsemi.com> in 1999.
   Major optimizations by Matjaz Breskvar <matjazb@bsemi.com> in 2005.
   Updated for GCC 4.5 by Jeremy Bennett <jeremy.bennett@embecoms.com> in 2010

   This file is part of GNU CC.

   This program is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the Free
   Software Foundation; either version 3 of the License, or (at your option)
   any later version.
  
   This program is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
   FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
   more details.
  
   You should have received a copy of the GNU General Public License along
   with this program.  If not, see <http://www.gnu.org/licenses/>. */

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "tm.h"
#include "rtl.h"
#include "tree.h"
#include "obstack.h"
#include "regs.h"
#include "hard-reg-set.h"
#include "real.h"
#include "insn-config.h"
#include "conditions.h"
#include "output.h"
#include "insn-attr.h"
#include "flags.h"
#include "reload.h"
#include "function.h"
#include "expr.h"
#include "toplev.h"
#include "recog.h"
#include "ggc.h"
#include "except.h"
#include "integrate.h"
#include "tm_p.h"
#include "target.h"
#include "target-def.h"
#include "debug.h"
#include "langhooks.h"
#include "df.h"
#include "dwarf2.h"


/* ========================================================================== */
/* Local macros                                                               */

/* Construct a l.movhi instruction for the given reg and value */
#define OR32_MOVHI(rd, k)						\
  ((0x6 << 26) | ((rd) << 21) | (k))

/* Construct a l.ori instruction for the given two regs and value */
#define OR32_ORI(rd, ra, k)						\
  ((0x2a << 26) | ((rd) << 21) | ((ra) << 16) | (k))

/* Construct a l.lwz instruction for the given two registers and offset */
#define OR32_LWZ(rd, ra, i)						\
  ((0x21 << 26) | ((rd) << 21) | ((ra) << 16) | (i))

/* Construct a l.jr instruction for the given register */
#define OR32_JR(rb)							\
  ((0x11 << 26) | ((rb) << 11))

/* ========================================================================== */
/* Static variables (i.e. global to this file only.                           */


/* Save information from a "cmpxx" pattern until the branch or scc is
   emitted. These record the two operands of the "cmpxx" */
rtx  or32_compare_op0;
rtx  or32_compare_op1;

/*!Stack layout we use for pushing and poping saved registers */
static struct
{
  bool save_lr_p;
  int lr_save_offset;
  bool save_fp_p;
  int fp_save_offset;
  int gpr_size;
  int gpr_offset;
  int total_size;
  int vars_size;
  int args_size;
  HOST_WIDE_INT mask;
}  frame_info;


/* ========================================================================== */
/* Local (i.e. static) utility functions */


/* -------------------------------------------------------------------------- */
/*!Must the current function save a register?

   @param[in] regno  The register to consider.

   @return  Non-zero (TRUE) if current function must save "regno", zero
            (FALSE) otherwise.                                                */
/* -------------------------------------------------------------------------- */
static bool
or32_save_reg_p (int regno)
{
  /* No need to save the faked cc0 register.  */
  if (regno == OR32_FLAGS_REG)
    return false;

  /* Check call-saved registers.  */
  if (df_regs_ever_live_p(regno) && !call_used_regs[regno])
    return true;

  /* We need to save the old frame pointer before setting up a new
     one.  */
  if (regno == FRAME_POINTER_REGNUM && frame_pointer_needed)
    return true;

  /* We need to save the incoming return address if it is ever clobbered
     within the function.  */
  if (regno == LINK_REGNUM && df_regs_ever_live_p(regno))
    return true;

  return false;

}	/* or32_save_reg_p () */


/* -------------------------------------------------------------------------- */
/*!Compute full frame size and layout.

   Store information in "frame_info".

   @param[in] size  The size of the function's local variables.

   @return  Total size of stack frame.                                        */
/* -------------------------------------------------------------------------- */
static HOST_WIDE_INT
or32_compute_frame_size (HOST_WIDE_INT size)
{
  HOST_WIDE_INT args_size;
  HOST_WIDE_INT vars_size;
  HOST_WIDE_INT stack_offset;

  int regno;

  args_size = crtl->outgoing_args_size;
  vars_size = OR32_ALIGN (size, 4);

  frame_info.args_size = args_size;
  frame_info.vars_size = vars_size;

  /* If the function has local variables, we're committed to
     allocating it anyway.  Otherwise reclaim it here.  */
  /* FIXME: Verify this.  Got if from the MIPS port.  */
  if (vars_size == 0 && current_function_is_leaf)
    args_size = 0;

  stack_offset = args_size;

  /* Save link register right after possible outgoing arguments.  */
  if (or32_save_reg_p (LINK_REGNUM))
    {
      frame_info.lr_save_offset = stack_offset;
      frame_info.save_lr_p = true;
      stack_offset = stack_offset + UNITS_PER_WORD;
    }
  else
    frame_info.save_lr_p = false;

  /* Save frame pointer right after possible link register.  */
  if (or32_save_reg_p (FRAME_POINTER_REGNUM))
    {
      frame_info.fp_save_offset = stack_offset;
      frame_info.save_fp_p = true;
      stack_offset = stack_offset + UNITS_PER_WORD;
    }
  else
    frame_info.save_fp_p = false;

  frame_info.gpr_size = 0;
  frame_info.mask = 0;
  frame_info.gpr_offset = stack_offset;

  for (regno = 0; regno <= OR32_LAST_INT_REG; regno++)
    {
      if (regno == LINK_REGNUM || regno == FRAME_POINTER_REGNUM)
	/* These have already been saved if so needed.  */
	continue;

      if (or32_save_reg_p (regno))
	{
	  frame_info.gpr_size += UNITS_PER_WORD;
	  frame_info.mask |= (1 << regno);
	}
    }

  frame_info.total_size = ((frame_info.save_fp_p ? UNITS_PER_WORD : 0)
			   + (frame_info.save_lr_p ? UNITS_PER_WORD : 0)
			   + args_size + frame_info.gpr_size + vars_size);

  return frame_info.total_size;

}	/* or32_compute_frame_size () */


/* -------------------------------------------------------------------------- */
/*!Emit a frame related insn.

   Same as emit_insn, but sets RTX_FRAME_RELATED_P to one. Getting this right
   will matter for DWARF 2 output, if prologues are handled via the "prologue"
   pattern rather than target hooks.

   @param[in] insn  The insn to emit.

   @return  The RTX for the emitted insn.                                     */
/* -------------------------------------------------------------------------- */
static rtx
emit_frame_insn (rtx insn)
{
  insn = emit_insn (insn);
  RTX_FRAME_RELATED_P (insn) = 1;
  return (insn);

}	/* emit_frame_insn () */


/* -------------------------------------------------------------------------- */
/*!Generate the RTX for an indexed memory access

   Generate a RTX for the indexed memory address based on a base address and a
   displacement

   @param[in] base  The base address RTX
   @param[in] disp  The displacement

   @return  The RTX for the generated address.                                */
/* -------------------------------------------------------------------------- */
static rtx
indexed_memory (rtx            base,
		HOST_WIDE_INT  disp)
{
  return gen_rtx_MEM (Pmode, gen_rtx_PLUS (Pmode, base, GEN_INT (disp)));

}	/* indexed_memory () */


/* -------------------------------------------------------------------------- */
/*!Generate insn patterns to do an integer compare of operands.

   @param[in] code  RTX for the condition code.
   @param[in] op0   RTX for the first operand.
   @param[in] op1   RTX for the second operand.

   @return  RTX for the comparison.                                           */
/* -------------------------------------------------------------------------- */
static rtx
or32_expand_int_compare (enum rtx_code  code,
			 rtx            op0,
			 rtx            op1)
{
  enum machine_mode cmpmode;
  rtx tmp, flags;

  cmpmode = SELECT_CC_MODE (code, op0, op1);
  flags = gen_rtx_REG (cmpmode, OR32_FLAGS_REG);

  /* This is very simple, but making the interface the same as in the
     FP case makes the rest of the code easier.  */
  tmp = gen_rtx_COMPARE (cmpmode, op0, op1);
  emit_insn (gen_rtx_SET (VOIDmode, flags, tmp));

  /* Return the test that should be put into the flags user, i.e.
     the bcc, scc, or cmov instruction.  */
  return gen_rtx_fmt_ee (code, VOIDmode, flags, const0_rtx);

}	/* or32_expand_int_compare () */


/* -------------------------------------------------------------------------- */
/*!Generate insn patterns to do an integer compare of operands.

   We only deal with the case where the comparison is an integer
   comparison. This wrapper function potentially allows reuse for non-integer
   comparison in the future.

   @param[in] code  RTX for the condition code.
   @param[in] op0   RTX for the first operand.
   @param[in] op1   RTX for the second operand.

   @return  RTX for the comparison.                                           */
/* -------------------------------------------------------------------------- */
static rtx
or32_expand_compare (enum rtx_code code, rtx op0, rtx op1)
{
  return or32_expand_int_compare (code, op0, op1);

}	/* or32_expand_compare () */


/* -------------------------------------------------------------------------- */
/*!Emit insns to use the l.cmov instruction

   Emit a compare and then cmov. Only works for integer first operand.

   @param[in] dest        RTX for the destination operand.
   @param[in] op          RTX for the comparison operation
   @param[in] true_cond   RTX to move to dest if condition is TRUE.
   @param[in] false_cond  RTX to move to dest if condition is FALSE.
   
   @return  Non-zero (TRUE) if insns were emitted, zero (FALSE) otherwise.    */
/* -------------------------------------------------------------------------- */
static int
or32_emit_int_cmove (rtx  dest,
		     rtx  op,
		     rtx  true_cond,
		     rtx  false_cond)
{
  rtx condition_rtx, cr;

  if ((GET_MODE (or32_compare_op0) != SImode) &&
      (GET_MODE (or32_compare_op0) != HImode) &&
      (GET_MODE (or32_compare_op0) != QImode))
    {
      return 0;
    }

  /* We still have to do the compare, because cmov doesn't do a compare, it
     just looks at the FLAG bit set by a previous compare instruction.  */
  condition_rtx = or32_expand_compare (GET_CODE (op),
				       or32_compare_op0, or32_compare_op1);

  cr = XEXP (condition_rtx, 0);

  emit_insn (gen_cmov (dest, condition_rtx, true_cond, false_cond, cr));

  return 1;

}	/* or32_emit_int_cmove () */


/* -------------------------------------------------------------------------- */
/*!Calculate stack size for current function.

   We need space for:
   - any callee-saved registers that are live in the function
   - any local variables
   - the return address (if saved)
   - the frame pointer (if saved)
   - any outgoing arguments.

   We also return information on whether the return address and frame pointer
   must be saved, the space required to save callee-saved registers and the
   sapce required to save the return address, frame pointer and outgoing
   arguments.

   Throughout adjust for OR32 alignment requirements.

   @param[in]  vars           Bytes required for local variables (if any).
   @param[out] lr_save_area   Space required for return address (if any).
   @param[out] fp_save_area   Space required for frame pointer (if any).
   @param[out] gpr_save_area  Space required for callee-saved registers (if
                              any).
   @param[out] save_area      Space required for outgoing arguments (if any) +
                              return address (if any) and frame pointer (if
                              any).

   @return  Total space required (if any).                                    */
/* -------------------------------------------------------------------------- */
static int
calculate_stack_size (int  vars,
		      int *lr_save_area,
		      int *fp_save_area,
		      int *gpr_save_area,
		      int *save_area)
{
  int regno;

  *gpr_save_area = 0;
  for (regno = 0; regno < FIRST_PSEUDO_REGISTER; regno++)
    {
      if (df_regs_ever_live_p(regno) && !call_used_regs[regno])
	*gpr_save_area += 4;
    }

  *lr_save_area = (!current_function_is_leaf
		   || df_regs_ever_live_p(LINK_REGNUM)) ? 4 : 0;
  *fp_save_area = frame_pointer_needed ? 4 : 0;
  *save_area    = (OR32_ALIGN (crtl->outgoing_args_size, 4)
		   + *lr_save_area + *fp_save_area);

  return *save_area + *gpr_save_area + OR32_ALIGN (vars, 4);

}	/* calculate_stack_size () */


/* -------------------------------------------------------------------------- */
/*!Is this a value suitable for an OR32 address displacement?

   Must be an integer (signed) which fits into 16-bits. If the result is a
   double word, we had better also check that we can also get at the second
   word.

   @param[in] mode  Mode of the result for which this displacement will be
                    used.
   @param[in] x     RTX for an expression.

   @return  Non-zero (TRUE) if this is a valid 16-bit offset, zero (FALSE)
            otherwise.                                                        */
/* -------------------------------------------------------------------------- */
static int
or32_legitimate_displacement_p (enum machine_mode  mode,
				rtx                x)
{
  if (CONST_INT == GET_CODE(x))
    {
      HOST_WIDE_INT  disp = INTVAL (x);

      /* Allow for a second access 4 bytes further on if double. */
      if ((DFmode == mode) || (DImode == mode))
	{
	  return  (-32768 < disp) && (disp <= 32763);
	}
      else
	{
	  return  (-32768 < disp) && (disp <= 32767);
	}
    }
  else
    {
      return  0;
    }
}	/* or32_legitimate_displacement_p () */


/* -------------------------------------------------------------------------- */
/*!Can this register be used as a base register?

   We need a strict version, for which the register must either be a hard
   register, or already renumbered to a hard register.

   For the non-strict version, any register (other than the flag register will
   do).

   @todo The code from the old port does not allow r0 as a base when strict,
         and does when non-strict. Surely it is always a valid register?

   @param[in] regno   The register to test
   @param[in] strict  Non-zero (TRUE) if this is a strict check, zero (FALSE)
                      otherwise.

   @return  Non-zero (TRUE) if this register can be used as a base register,
            zero (FALSE) otherwise.                                           */
/* -------------------------------------------------------------------------- */
static bool
or32_regnum_ok_for_base_p (HOST_WIDE_INT  num,
			   bool           strict)
{
  if (strict)
    {
      return (num < FIRST_PSEUDO_REGISTER)
	? (num > 0) && (num <= OR32_LAST_INT_REG)
	: (reg_renumber[num] > 0) && (reg_renumber[num] <= OR32_LAST_INT_REG);
    }
  else
    {
      return (num <= OR32_LAST_INT_REG) || (num >= FIRST_PSEUDO_REGISTER);
    }
}	/* or32_regnum_ok_for_base_p () */


/* -------------------------------------------------------------------------- */
/*!Emit a move from SRC to DEST.

   Assume that the move expanders can handle all moves if !can_create_pseudo_p
   ().  The distinction is important because, unlike emit_move_insn, the move
   expanders know how to force Pmode objects into the constant pool even when
   the constant pool address is not itself legitimate.

   @param[in] dest  Destination of the move.
   @param[in] src   Source for the move.

   @return  RTX for the move.                                                 */
/* -------------------------------------------------------------------------- */
rtx
or32_emit_move (rtx dest, rtx src)
{
  return (can_create_pseudo_p ()
	  ? emit_move_insn (dest, src)
	  : emit_move_insn_1 (dest, src));

}	/* or32_emit_move () */


/* -------------------------------------------------------------------------- */
/*!Emit an instruction of the form (set TARGET (CODE OP0 OP1)).

   @param[in] code    The code for the operation.
   @param[in] target  Destination for the set operation.
   @param[in] op0     First operand.
   @param[in] op1     Second operand.                                         */
/* -------------------------------------------------------------------------- */
static void
or32_emit_binary (enum rtx_code  code,
		  rtx            target,
		  rtx            op0,
		  rtx            op1)
{
  emit_insn (gen_rtx_SET (VOIDmode, target,
			  gen_rtx_fmt_ee (code, GET_MODE (target), op0, op1)));

}	/* or32_emit_binary () */


/* -------------------------------------------------------------------------- */
/*!Compute the result of an operation into a new register.

   Compute ("code" "op0" "op1") and store the result in a new register of mode
   "mode".

   @param[in] mode  Mode of the result
   @parma[in] code  RTX for the operation to perform
   @param[in] op0   RTX for the first operand
   @param[in] op1   RTX for the second operand
   
   @return  The RTX for the new register.                                     */
/* -------------------------------------------------------------------------- */
static rtx
or32_force_binary (enum machine_mode  mode,
		   enum rtx_code      code,
		   rtx                op0,
		   rtx                op1)
{
  rtx  reg;

  reg = gen_reg_rtx (mode);
  or32_emit_binary (code, reg, op0, op1);

  return reg;

}	/* or32_force_binary () */


/* ========================================================================== */
/* Global support functions                                                   */

/* -------------------------------------------------------------------------- */
/* Return the size in bytes of the trampoline code.

   Padded to TRAMPOLINE_ALIGNMENT bits. The code sequence is documented in
   or32_trampoline_init ().

   This is just the code size. the static chain pointer and target function
   address immediately follow.

   @return  The size of the trampoline code in bytes.                         */
/* -------------------------------------------------------------------------- */
int
or32_trampoline_code_size (void)
{
  const int  TRAMP_BYTE_ALIGN = TRAMPOLINE_ALIGNMENT / 8;

  /* Five 32-bit code words are needed */
  return (5 * 4 + TRAMP_BYTE_ALIGN - 1) / TRAMP_BYTE_ALIGN * TRAMP_BYTE_ALIGN;

}	/* or32_trampoline_code_size () */


/* ========================================================================== */
/* Functions to support the Machine Description                               */


/* -------------------------------------------------------------------------- */
/*!Expand a prologue pattern.

   Called after register allocation to add any instructions needed for the
   prologue.  Using a prologue insn is favored compared to putting all of the
   instructions in output_function_prologue(), since it allows the scheduler
   to intermix instructions with the saves of the caller saved registers.  In
   some cases, it might be necessary to emit a barrier instruction as the last
   insn to prevent such scheduling.

   For the OR32 this is currently controlled by the -mlogue option. It should
   be the default, once it is proved to work.                                 */
/* -------------------------------------------------------------------------- */
void
or32_expand_prologue (void)
{
  int total_size = or32_compute_frame_size (get_frame_size ());
  rtx sp_rtx;
  rtx value_rtx;

  if (!total_size)
    /* No frame needed.  */
    return;

  sp_rtx = gen_rtx_REG (Pmode, STACK_POINTER_REGNUM);

  if (total_size > 32767)
    {
      value_rtx = gen_rtx_REG (Pmode, GP_ARG_RETURN);
      emit_frame_insn (gen_rtx_SET (Pmode, value_rtx, GEN_INT (total_size)));
    }
  else
    value_rtx = GEN_INT (total_size);

  /* Update the stack pointer to reflect frame size.  */
  emit_frame_insn
    (gen_rtx_SET (Pmode, stack_pointer_rtx,
		  gen_rtx_MINUS (Pmode, stack_pointer_rtx, value_rtx)));

  if (frame_info.save_fp_p)
    {
      emit_frame_insn
	(gen_rtx_SET (Pmode,
		      indexed_memory (stack_pointer_rtx,
				      frame_info.fp_save_offset),
		      frame_pointer_rtx));

      emit_frame_insn
	(gen_rtx_SET (Pmode, frame_pointer_rtx,
		      gen_rtx_PLUS (Pmode, frame_pointer_rtx, value_rtx)));
    }
  if (frame_info.save_lr_p)
    {

      emit_frame_insn
	(gen_rtx_SET (Pmode,
		      indexed_memory (stack_pointer_rtx,
				      frame_info.lr_save_offset),
		      gen_rtx_REG (Pmode, LINK_REGNUM)));
    }
  if (frame_info.gpr_size)
    {
      int offset = 0;
      int regno;

      for (regno = 0; regno <= OR32_LAST_INT_REG; regno++)
	{
	  HOST_WIDE_INT disp = frame_info.gpr_offset + offset;

	  if (!(frame_info.mask & (1 << regno)))
	    continue;

	  emit_frame_insn
	    (gen_rtx_SET (Pmode,
			  indexed_memory (stack_pointer_rtx, disp),
			  gen_rtx_REG (Pmode, regno)));
	  offset = offset + UNITS_PER_WORD;
	}
    }
}	/* or32_expand_prologue () */


/* -------------------------------------------------------------------------- */
/*!Expand an epilogue pattern.

   Called after register allocation to add any instructions needed for the
   epilogue.  Using an epilogue insn is favored compared to putting all of the
   instructions in output_function_epilogue(), since it allows the scheduler
   to intermix instructions with the restores of the caller saved registers.
   In some cases, it might be necessary to emit a barrier instruction as the
   first insn to prevent such scheduling.

   For the OR32 this is currently controlled by the -mlogue option. It should
   be the default, once it is proved to work.

   @param[in] sibcall  Non-zero (TRUE) if this is a sibcall return, which can
                       benefit from tail call optimization. Zero (FALSE)
                       otherwise.                                             */
/* -------------------------------------------------------------------------- */
void
or32_expand_epilogue (int sibcall)
{
  int total_size = or32_compute_frame_size (get_frame_size ());
  rtx value_rtx;

  if (total_size > 32767)
    {
      value_rtx = gen_rtx_REG (Pmode, 3);

      emit_insn (gen_rtx_SET (Pmode, value_rtx, GEN_INT (total_size)));
    }
  else
    value_rtx = GEN_INT (total_size);

  if (frame_info.save_lr_p)
    {
      emit_insn
        (gen_rtx_SET (Pmode, gen_rtx_REG (Pmode, LINK_REGNUM),
                      indexed_memory (stack_pointer_rtx,
				      frame_info.lr_save_offset)));
    }
  if (frame_info.save_fp_p)
    {
      emit_insn
	(gen_rtx_SET (Pmode, gen_rtx_REG (Pmode, FRAME_POINTER_REGNUM),
		      indexed_memory (stack_pointer_rtx,
				      frame_info.fp_save_offset)));
    }

  if (frame_info.gpr_size)
    {
      int offset = 0;
      int regno;

      for (regno = 0; regno <= OR32_LAST_INT_REG; regno++)
	{
	  HOST_WIDE_INT disp = frame_info.gpr_offset + offset;

	  if (!(frame_info.mask & (1 << regno)))
	    continue;

	  emit_insn
	    (gen_rtx_SET (Pmode, gen_rtx_REG (Pmode, regno),
			  indexed_memory (stack_pointer_rtx, disp)));
	  offset = offset + UNITS_PER_WORD;
	}
    }

  if (total_size)
    {
      emit_insn (gen_rtx_SET (Pmode, stack_pointer_rtx,
			      gen_rtx_PLUS (Pmode,
					    stack_pointer_rtx, value_rtx)));
    }

  if (!sibcall)
    emit_jump_insn (gen_return_internal (gen_rtx_REG( Pmode, 9)));

}	/* or32_expand_epilogue () */


/* -------------------------------------------------------------------------- */
/*!Generate assembler code for a movdi/movdf pattern

   @param[in] operands  Operands to the movdx pattern.

   @return  The assembler string to output (always "", since we've done the
            output here).                                                     */
/* -------------------------------------------------------------------------- */
const char *
or32_output_move_double (rtx *operands)
{
  rtx xoperands[3];

  switch (GET_CODE (operands[0]))
    {
    case REG:
      if (GET_CODE (operands[1]) == REG)
	{
	  if (REGNO (operands[0]) == REGNO (operands[1]) + 1)
	    {
	      output_asm_insn ("\tl.or    \t%H0, %H1, r0", operands);
	      output_asm_insn ("\tl.or    \t%0, %1, r0", operands);
	      return "";
	    }
	  else
	    {
	      output_asm_insn ("\tl.or    \t%0, %1, r0", operands);
	      output_asm_insn ("\tl.or    \t%H0, %H1, r0", operands);
	      return "";
	    }
	}
      else if (GET_CODE (operands[1]) == MEM)
	{
	  xoperands[1] = XEXP (operands[1], 0);
	  if (GET_CODE (xoperands[1]) == REG)
	    {
	      xoperands[0] = operands[0];
	      if (REGNO (xoperands[0]) == REGNO (xoperands[1]))
		{
		  output_asm_insn ("\tl.lwz   \t%H0, 4(%1)", xoperands);
		  output_asm_insn ("\tl.lwz   \t%0, 0(%1)", xoperands);
		  return "";
		}
	      else
		{
		  output_asm_insn ("\tl.lwz   \t%0, 0(%1)", xoperands);
		  output_asm_insn ("\tl.lwz   \t%H0, 4(%1)", xoperands);
		  return "";
		}
	    }
	  else if (GET_CODE (xoperands[1]) == PLUS)
	    {
	      if (GET_CODE (xoperands[2] = XEXP (xoperands[1], 1)) == REG)
		{
		  xoperands[0] = operands[0];
		  xoperands[1] = XEXP (xoperands[1], 0);
		  if (REGNO (xoperands[0]) == REGNO (xoperands[2]))
		    {
		      output_asm_insn ("\tl.lwz   \t%H0, %1+4(%2)",
				       xoperands);
		      output_asm_insn ("\tl.lwz   \t%0, %1(%2)", xoperands);
		      return "";
		    }
		  else
		    {
		      output_asm_insn ("\tl.lwz   \t%0, %1(%2)", xoperands);
		      output_asm_insn ("\tl.lwz   \t%H0, %1+4(%2)",
				       xoperands);
		      return "";
		    }
		}
	      else if (GET_CODE (xoperands[2] = XEXP (xoperands[1], 0)) ==
		       REG)
		{
		  xoperands[0] = operands[0];
		  xoperands[1] = XEXP (xoperands[1], 1);
		  if (REGNO (xoperands[0]) == REGNO (xoperands[2]))
		    {
		      output_asm_insn ("\tl.lwz   \t%H0, %1+4(%2)",
				       xoperands);
		      output_asm_insn ("\tl.lwz   \t%0, %1(%2)", xoperands);
		      return "";
		    }
		  else
		    {
		      output_asm_insn ("\tl.lwz   \t%0, %1(%2)", xoperands);
		      output_asm_insn ("\tl.lwz   \t%H0, %1+4(%2)",
				       xoperands);
		      return "";
		    }
		}
	      else
		abort ();
	    }
	  else
	    abort ();
	}
      else if (GET_CODE (operands[1]) == CONST_INT)
	{
	  if (INTVAL (operands[1]) < 0)
	    output_asm_insn ("\tl.addi  \t%0, r0, -1", operands);
	  else
	    output_asm_insn ("\tl.or    \t%0, r0, r0", operands);
	  output_asm_insn ("\tl.movhi \t%H0, hi(%1)", operands);
	  output_asm_insn ("\tl.ori   \t%H0, %H0, lo(%1)", operands);
	  return "";
	}
      else
	abort ();
    case MEM:
      xoperands[0] = XEXP (operands[0], 0);
      if (GET_CODE (xoperands[0]) == REG)
	{
	  xoperands[1] = operands[1];
	  output_asm_insn ("\tl.sw    \t0(%0), %1", xoperands);
	  output_asm_insn ("\tl.sw    \t4(%0), %H1", xoperands);
	  return "";
	}
      else if (GET_CODE (xoperands[0]) == PLUS)
	{
	  if (GET_CODE (xoperands[1] = XEXP (xoperands[0], 1)) == REG)
	    {
	      xoperands[0] = XEXP (xoperands[0], 0);
	      xoperands[2] = operands[1];
	      output_asm_insn ("\tl.sw    \t%0(%1), %2", xoperands);
	      output_asm_insn ("\tl.sw    \t%0+4(%1), %H2", xoperands);
	      return "";
	    }
	  else if (GET_CODE (xoperands[1] = XEXP (xoperands[0], 0)) == REG)
	    {
	      xoperands[0] = XEXP (xoperands[0], 1);
	      xoperands[2] = operands[1];
	      output_asm_insn ("\tl.sw    \t%0(%1), %2", xoperands);
	      output_asm_insn ("\tl.sw    \t%0+4(%1), %H2", xoperands);
	      return "";
	    }
	  else
	    abort ();
	}
      else
	{
	  fprintf (stderr, "  O/p error %s\n",
		   GET_RTX_NAME (GET_CODE (xoperands[0])));
	  return "";
	  /* abort (); */
	}
    default:
      abort ();
    }
}	/* or32_output_move_double () */


/* -------------------------------------------------------------------------- */
/*!Expand a conditional branch

   @param[in] operands  Operands to the branch.
   @param[in] mode      Mode of the comparison.                               */
/* -------------------------------------------------------------------------- */
void
or32_expand_conditional_branch (rtx               *operands,
				enum machine_mode  mode)
{
  rtx tmp;
  enum rtx_code test_code = GET_CODE(operands[0]);

  switch (mode)
    {
    case SImode:
      tmp = or32_expand_compare (test_code, operands[1], operands[2]);
      tmp = gen_rtx_IF_THEN_ELSE (VOIDmode,
				  tmp,
				  gen_rtx_LABEL_REF (VOIDmode, operands[3]),
				  pc_rtx);
      emit_jump_insn (gen_rtx_SET (VOIDmode, pc_rtx, tmp));
      return;
      
    case SFmode:
      tmp = or32_expand_compare (test_code, operands[1], operands[2]);
      tmp = gen_rtx_IF_THEN_ELSE (VOIDmode,
				  tmp,
				  gen_rtx_LABEL_REF (VOIDmode, operands[3]),
				  pc_rtx);
      emit_jump_insn (gen_rtx_SET (VOIDmode, pc_rtx, tmp));
      return;
      
    default:
      abort ();
    }

}	/* or32_expand_conditional_branch () */


/* -------------------------------------------------------------------------- */
/*!Emit a conditional move

   move "true_cond" to "dest" if "op" of the operands of the last comparison
   is nonzero/true, "false_cond" if it is zero/false.

   @param[in] dest        RTX for the destination operand.
   @param[in] op          RTX for the comparison operation
   @param[in] true_cond   RTX to move to dest if condition is TRUE.
   @param[in] false_cond  RTX to move to dest if condition is FALSE.
   
   @return  Non-zero (TRUE) if the hardware supports such an operation, zero
            (FALSE) otherwise.                                                */
/* -------------------------------------------------------------------------- */
int
or32_emit_cmove (rtx  dest,
		 rtx  op,
		 rtx  true_cond,
		 rtx  false_cond)
{
  enum machine_mode result_mode = GET_MODE (dest);

  if (GET_MODE (true_cond) != result_mode)
    return 0;

  if (GET_MODE (false_cond) != result_mode)
    return 0;

  /* First, work out if the hardware can do this at all */
  return or32_emit_int_cmove (dest, op, true_cond, false_cond);

}	/* or32_emit_cmove () */


/* -------------------------------------------------------------------------- */
/*!Output the assembler for a branch on flag instruction.

   @param[in] operands  Operands to the branch.
   
   @return  The assembler string to use.                                      */
/* -------------------------------------------------------------------------- */
const char *
or32_output_bf (rtx * operands)
{
  enum rtx_code code;
  enum machine_mode mode_calc, mode_got;

  code      = GET_CODE (operands[1]);
  mode_calc = SELECT_CC_MODE (code, or32_compare_op0, or32_compare_op1);
  mode_got  = GET_MODE (operands[2]);

  if (!TARGET_MASK_ALIGNED_JUMPS)
    {
      if (mode_calc != mode_got)
	return "l.bnf\t%l0%(";
      else
	return "l.bf\t%l0%(";
    }
  else
    {
      if (mode_calc != mode_got)
	return ".balignl\t0x8,0x15000015,0x4\n\tl.bnf\t%l0%(";
      else
	return ".balignl 0x8,0x15000015,0x4;\n\tl.bf\t%l0%(";
    }
}	/* or32_output_bf () */


/* -------------------------------------------------------------------------- */
/*!Output the assembler for a conditional move instruction.

   @param[in] operands  Operands to the conditional move.
   
   @return  The assembler string to use.                                      */
/* -------------------------------------------------------------------------- */
const char *
or32_output_cmov (rtx * operands)
{
  enum rtx_code code;
  enum machine_mode mode_calc, mode_got;

  code      = GET_CODE (operands[1]);
  mode_calc = SELECT_CC_MODE (code, or32_compare_op0, or32_compare_op1);
  mode_got  = GET_MODE (operands[4]);

  if (mode_calc != mode_got)
    return "\tl.cmov\t%0,%3,%2";	/* reversed */
  else
    return "\tl.cmov\t%0,%2,%3";

}	/* or32_output_cmov () */


/* -------------------------------------------------------------------------- */
/*!Expand a sibcall pattern.

   For now this is very simple way for sibcall support (i.e tail call
   optimization).

   @param[in] result     Not sure. RTX for the result location?
   @param[in] addr       Not sure. RXT for the address to call?
   @param[in] args_size  Not sure. RTX for the size of the args (in bytes?)?  */
/* -------------------------------------------------------------------------- */
void
or32_expand_sibcall (rtx  result ATTRIBUTE_UNUSED,
		     rtx  addr,
		     rtx  args_size)
{
  emit_call_insn (gen_sibcall_internal (addr, args_size));

}	/* or32_expand_sibcall () */


/* -------------------------------------------------------------------------- */
/*!Load a 32-bit constant.

   We know it can't be done in one insn when we get here, the movsi expander
   guarantees this.

   @param[in] op0  RTX for the destination.
   @param[in] op1  RTX for the (constant) source.                             */
/* -------------------------------------------------------------------------- */
void
or32_emit_set_const32 (rtx  op0,
		       rtx  op1)
{
  enum machine_mode mode = GET_MODE (op0);
  rtx temp;

  /* Sanity check that we really can't do it in one instruction. I.e that we
     don't have a 16-bit constant. */
  if (GET_CODE (op1) == CONST_INT)
    {
      HOST_WIDE_INT val = INTVAL (op1) & GET_MODE_MASK (mode);

      if ((-32768 <= val) && (val <= 32767))
	{
	  abort ();
	}
    }

  /* Full 2-insn decomposition is needed.  */
  if (reload_in_progress || reload_completed)
    temp = op0;
  else
    temp = gen_reg_rtx (mode);

  if (GET_CODE (op1) == CONST_INT)
    {
      /* Emit them as real moves instead of a HIGH/LO_SUM,
         this way CSE can see everything and reuse intermediate
         values if it wants.  */
      emit_insn (gen_rtx_SET (VOIDmode, temp,
			      GEN_INT (INTVAL (op1)
				       & ~(HOST_WIDE_INT) 0xffff)));

      emit_insn (gen_rtx_SET (VOIDmode,
			      op0,
			      gen_rtx_IOR (mode, temp,
					   GEN_INT (INTVAL (op1) & 0xffff))));
    }
  else
    {
      /* since or32 bfd can not deal with relocs that are not of type
         OR32_CONSTH_RELOC + OR32_CONST_RELOC (ie move high must be
         followed by exactly one lo_sum)
       */
      emit_insn (gen_movsi_insn_big (op0, op1));
    }
}	/* or32_emit_set_const32 () */


/* ========================================================================== */
/* Target hook functions.

   These are initialized at the end of this file, to avoid having to
   predeclare all the functions. They are only needed here, so are static.    */


/* -------------------------------------------------------------------------- */
/*!Set up the stack and frame pointer (if desired) for the function.

   If defined, a function that outputs the assembler code for entry to a
   function. The prologue is responsible for setting up the stack frame,
   initializing the frame pointer register, saving registers that must be
   saved, and allocating "size" additional bytes of storage for the local
   variables. "size" is an integer. "file" is a stdio stream to which the
   assembler code should be output.

   The label for the beginning of the function need not be output by this
   macro. That has already been done when the macro is run.

   To determine which registers to save, the macro can refer to the array
   "regs_ever_live": element "r" is nonzero if hard register "r" is used
   anywhere within the function.  This implies the function prologue should
   save register r, provided it is not one of the call-used
   registers. (TARGET_ASM_FUNCTION_EPILOGUE must likewise use
   "regs_ever_live".)

   On machines that have "register windows", the function entry code does not
   save on the stack the registers that are in the windows, even if they are
   supposed to be preserved by function calls; instead it takes appropriate
   steps to “push” the register stack, if any non-call-used registers are used
   in the function.

   On machines where functions may or may not have frame-pointers, the
   function entry code must vary accordingly; it must set up the frame pointer
   if one is wanted, and not otherwise. To determine whether a frame pointer
   is in wanted, the macro can refer to the variable frame_pointer_needed. The
   variable’s value will be 1 at run time in a function that needs a frame
   pointer. See the section on "Eliminating Frame Pointer and Arg Pointer" in
   the "Target Description Macros and Functions" chapter of the GCC internals
   manual.

   The function entry code is responsible for allocating any stack space
   required for the function. This stack space consists of the regions listed
   below. In most cases, these regions are allocated in the order listed, with
   the last listed region closest to the top of the stack (the lowest address
   if STACK_GROWS_DOWNWARD is defined, and the highest address if it is not
   defined). You can use a different order for a machine if doing so is more
   convenient or required for compatibility reasons. Except in cases where
   required by standard or by a debugger, there is no reason why the stack
   layout used by GCC need agree with that used by other compilers for a
   machine.

   @param[in] file  File handle for any generated code.
   @param[in] size  Number of bytes of storage needed for local variables.    */
/* -------------------------------------------------------------------------- */
static void
or32_output_function_prologue (FILE          *file,
			       HOST_WIDE_INT  size)
{
  int save_area;
  int gpr_save_area;
  int lr_save_area;
  int fp_save_area;
  int stack_size;
  int regno;

  /* If we are doing the prologue using the "prologue" pattern in the machine
     description, do nothing more here.

     JPB 30-Aug-10: Surely that is not correct. If this option is set, we
     should never even be called! */
  if (TARGET_MASK_SCHED_LOGUE)
    return;

  if (size < 0)
    abort ();

  /* Work out and log the frame size */
  stack_size = calculate_stack_size (size, &lr_save_area, &fp_save_area,
				     &gpr_save_area, &save_area);

  fprintf (file,
	   "\n\t# gpr_save_area %d size %ld crtl->outgoing_args_size %d\n",
	   gpr_save_area, size, crtl->outgoing_args_size);

  /* Decrement the stack pointer by the total frame size (if we have a
     frame). */
  if (stack_size > 0)
    {
      /* Special code for large stack frames */
      if (stack_size >= 0x8000)
	{
	  fprintf (file, "\tl.movhi\tr%d,hi(%d)\n", GP_ARG_RETURN, stack_size);
	  fprintf (file, "\tl.ori\tr%d,r%d,lo(%d)\n", GP_ARG_RETURN,
		   GP_ARG_RETURN, stack_size);
	  fprintf (file, "\tl.sub\tr%d,r%d,r%d\n", STACK_POINTER_REGNUM,
		   STACK_POINTER_REGNUM, GP_ARG_RETURN);
	}
      else
	{
	  fprintf (file, "\tl.addi\tr%d,r%d,%d\n", STACK_POINTER_REGNUM,
		   STACK_POINTER_REGNUM, -stack_size);
	}

      /* Update the DWARF2 CFA using the new stack pointer. After this the CFA
	 will be the SP + frame size, i.e. the FP (or start of frame if we
	 don't actually have a FP). All register refs should relate to this. */
      if (dwarf2out_do_frame ())
	{
	  char *l = dwarf2out_cfi_label (false);

	  dwarf2out_def_cfa (l, STACK_POINTER_REGNUM, stack_size);
	}
    }

  /* Update the frame pointer if necessary */
  if (fp_save_area)
    {
      char *l     = dwarf2out_cfi_label (false);
      int  offset = OR32_ALIGN (crtl->outgoing_args_size, 4) + lr_save_area;

      fprintf (file, "\tl.sw\t%d(r%d),r%d\n", offset,
	       STACK_POINTER_REGNUM, FRAME_POINTER_REGNUM);

      if (stack_size >= 0x8000)
	fprintf (file, "\tl.add\tr%d,r%d,r%d\n", FRAME_POINTER_REGNUM,
		 STACK_POINTER_REGNUM, GP_ARG_RETURN);
      else
	fprintf (file, "\tl.addi\tr%d,r%d,%d\n", FRAME_POINTER_REGNUM,
		 STACK_POINTER_REGNUM, stack_size);

      /* The CFA is already pointing at the start of our frame (i.e. the new
	 FP). The old FP has been saved relative to the SP, so we need to use
	 stack_size to work out where. */
      dwarf2out_reg_save (l, FRAME_POINTER_REGNUM, offset - stack_size);
    }

  /* Save the return address if necessary */
  if (lr_save_area)
    {
      char *l     = dwarf2out_cfi_label (false);
      int  offset = OR32_ALIGN (crtl->outgoing_args_size, 4);

      fprintf (file, "\tl.sw\t%d(r%d),r%d\n", offset, STACK_POINTER_REGNUM,
	       LINK_REGNUM);

      /* The CFA is already pointing at the start of our frame (i.e. the new
	 FP). The LR has been saved relative to the SP, so we need to use
	 stack_size to work out where. */
      dwarf2out_reg_save (l, FRAME_POINTER_REGNUM, offset - stack_size);
    }

  save_area = (OR32_ALIGN (crtl->outgoing_args_size, 4)
	       + lr_save_area + fp_save_area);

  /* Save any callee saved registers */
  for (regno = 0; regno < FIRST_PSEUDO_REGISTER; regno++)
    {
      if (df_regs_ever_live_p(regno) && !call_used_regs[regno])
	{
	  char *l = dwarf2out_cfi_label (false);

	  fprintf (file, "\tl.sw\t%d(r%d),r%d\n", save_area,
		   STACK_POINTER_REGNUM, regno);

	  /* The CFA is already pointing at the start of our frame (i.e. the
	     new FP). The register has been saved relative to the SP, so we
	     need to use stack_size to work out where. */
	  dwarf2out_reg_save (l, FRAME_POINTER_REGNUM, save_area - stack_size);
	  save_area += 4;
	}
    }
}	/* or32_output_function_prologue () */


/* -------------------------------------------------------------------------- */
/*!Do any necessary cleanup after a function to restore stack, frame, and regs.

   This is a function that outputs the assembler code for exit from a
   function. The epilogue is responsible for restoring the saved registers and
   stack pointer to their values when the function was called, and returning
   control to the caller. This macro takes the same arguments as the macro
   TARGET_ASM_FUNCTION_PROLOGUE, and the registers to restore are determined
   from regs_ever_live and CALL_USED_REGISTERS in the same way (@see
   or32_output_function_prologue ()) .

   On some machines, there is a single instruction that does all the work of
   returning from the function. On these machines, give that instruction the
   name "return" (in the machine definition) and do not define the macro
   TARGET_ASM_FUNCTION_EPILOGUE at all.

   Do not define a pattern named "return" if you want the
   TARGET_ASM_FUNCTION_EPILOGUE to be used. If you want the target switches to
   control whether return instructions or epilogues are used, define a
   "return" pattern with a validity condition that tests the target switches
   appropriately. If the "return" pattern’s validity condition is false,
   epilogues will be used.

   On machines where functions may or may not have frame-pointers, the
   function exit code must vary accordingly. Sometimes the code for these two
   cases is completely different. To determine whether a frame pointer is
   wanted, the macro can refer to the variable frame_pointer_needed. The
   variable’s value will be 1 when compiling a function that needs a frame
   pointer.

   Normally, TARGET_ASM_FUNCTION_PROLOGUE and TARGET_ASM_FUNCTION_EPILOGUE
   must treat leaf functions specially. The C variable
   "current_function_is_leaf" is nonzero for such a function. See "Handling
   Leaf Functions" in the "Target Description Macros and Functions" section of
   the GCC internals manual.

   On some machines, some functions pop their arguments on exit while others
   leave that for the caller to do. For example, the 68020 when given "-mrtd"
   pops arguments in functions that take a fixed number of arguments.

   Your definition of the macro RETURN_POPS_ARGS decides which functions pop
   their own arguments. TARGET_ASM_FUNCTION_EPILOGUE needs to know what was
   decided.  The number of bytes of the current function’s arguments that this
   function should pop is available in "crtl->args.pops_args". See "How Scalar
   Function Values Are Returned" in the "Target Description Macros and
   Functions" section of the GCC internals manual.

   @param[in] file  File handle for any generated code.
   @param[in] size  Number of bytes of storage needed for local variables.    */
/* -------------------------------------------------------------------------- */
static void
or32_output_function_epilogue (FILE * file, HOST_WIDE_INT size)
{
  int save_area;
  int gpr_save_area;
  int lr_save_area;
  int fp_save_area;
  int stack_size;
  int regno;

  /* If we are doing the epilogue using the "epilogue" pattern in the machine
     description, do nothing more here.

     JPB 30-Aug-10: Surely that is not correct. If this option is set, we
     should never even be called! */
  if (TARGET_MASK_SCHED_LOGUE)
    return;

  /* Work out the frame size */
  stack_size = calculate_stack_size (size, &lr_save_area, &fp_save_area,
				     &gpr_save_area, &save_area);

  /* Restore the return address if necessary */
  if (lr_save_area)
    {
      fprintf (file, "\tl.lwz\tr%d,%d(r%d)\n", LINK_REGNUM,
	       OR32_ALIGN (crtl->outgoing_args_size, 4),
	       STACK_POINTER_REGNUM);
    }

  /* Restore the frame pointer if necessary */
  if (fp_save_area)
    {
      fprintf (file, "\tl.lwz\tr%d,%d(r%d)\n", FRAME_POINTER_REGNUM,
	       OR32_ALIGN (crtl->outgoing_args_size, 4)
	       + lr_save_area, STACK_POINTER_REGNUM);
    }

  save_area = (OR32_ALIGN (crtl->outgoing_args_size, 4)
	       + lr_save_area + fp_save_area);

  /* Restore any callee-saved registers */
  for (regno = 0; regno < FIRST_PSEUDO_REGISTER; regno++)
    {
      if (df_regs_ever_live_p(regno) && !call_used_regs[regno])
	{
	  fprintf (file, "\tl.lwz\tr%d,%d(r%d)\n", regno, save_area,
		   STACK_POINTER_REGNUM);
	  save_area += 4;
	}
    }

  /* Restore the stack pointer (if necessary) */
  if (stack_size >= 0x8000)
    {
      fprintf (file, "\tl.movhi\tr3,hi(%d)\n", stack_size);
      fprintf (file, "\tl.ori\tr3,r3,lo(%d)\n", stack_size);

      if (!TARGET_MASK_ALIGNED_JUMPS)
	fprintf (file, "\tl.jr\tr%d\n", LINK_REGNUM);
      else
	{
	  fprintf (file, "\t.balignl\t0x8,0x15000015,0x4\n");
	  fprintf (file, "\tl.jr\tr%d\n", LINK_REGNUM);
	}

      fprintf (file, "\tl.add\tr%d,r%d,r3\n", STACK_POINTER_REGNUM,
	       STACK_POINTER_REGNUM);
    }
  else if (stack_size > 0)
    {
      if (!TARGET_MASK_ALIGNED_JUMPS)
	fprintf (file, "\tl.jr\tr%d\n", LINK_REGNUM);
      else
	{
	  fprintf (file, "\t.balignl 0x8,0x15000015,0x4\n");
	  fprintf (file, "\tl.jr\tr%d\n", LINK_REGNUM);
	}

      fprintf (file, "\tl.addi\tr%d,r%d,%d\n", STACK_POINTER_REGNUM,
	       STACK_POINTER_REGNUM, stack_size);
    }
  else
    {
      if (!TARGET_MASK_ALIGNED_JUMPS)
	fprintf (file, "\tl.jr\tr%d\n", LINK_REGNUM);
      else
	{
	  fprintf (file, "\t.balignl\t0x8,0x15000015,0x4\n");
	  fprintf (file, "\tl.jr\tr%d\n", LINK_REGNUM);
	}

      fprintf (file, "\tl.nop\n");		/* Delay slot */
    }
}	/* or32_output_function_epilogue () */


/* -------------------------------------------------------------------------- */
/*!Define where a function returns values.

   Define this to return an RTX representing the place where a function
   returns or receives a value of data type ret type, a tree node representing
   a data type.  "func" is a tree node representing FUNCTION_DECL or
   FUNCTION_TYPE of a function being called. If "outgoing" is false, the hook
   should compute the register in which the caller will see the return
   value. Otherwise, the hook should return an RTX representing the place
   where a function returns a value.

   On many machines, only TYPE_MODE ("ret_type") is relevant. (Actually, on
   most machines, scalar values are returned in the same place regardless of
   mode.) The value of the expression is usually a reg RTX for the hard
   register where the return value is stored. The value can also be a parallel
   RTX, if the return value is in multiple places. See FUNCTION_ARG for an
   explanation of the parallel form. Note that the callee will populate every
   location specified in the parallel, but if the first element of the
   parallel contains the whole return value, callers will use that element as
   the canonical location and ignore the others. The m68k port uses this type
   of parallel to return pointers in both ‘%a0’ (the canonical location) and
   ‘%d0’.

   If TARGET_PROMOTE_FUNCTION_RETURN returns true, you must apply the same
   promotion rules specified in PROMOTE_MODE if valtype is a scalar type.

   If the precise function being called is known, "func" is a tree node
   (FUNCTION_DECL) for it; otherwise, "func" is a null pointer. This makes it
   possible to use a different value-returning convention for specific
   functions when all their calls are known.

   Some target machines have "register windows" so that the register in which
   a function returns its value is not the same as the one in which the caller
   sees the value. For such machines, you should return different RTX
   depending on outgoing.

   TARGET_FUNCTION_VALUE is not used for return values with aggregate data
   types, because these are returned in another way. See
   TARGET_STRUCT_VALUE_RTX and related macros.

   For the OR32, we can just use the result of LIBCALL_VALUE, since all
   functions return their result in the same place (register rv = r11).

   JPB 30-Aug-10: What about 64-bit scalar returns (long long int, double),
                  which also use rvh (=r12)?

   @param[in] ret_type  The return type of the function.
   @param[in] func      Tree representing function being called.
   @param[in] outgoing  Non-zero (TRUE) if the result represents where the
                        function places the results, zero (FALSE) if the
                        result represents where the caller sees the result.

   @return  A RTX representing where the result can be found.                 */
/* -------------------------------------------------------------------------- */
static rtx
or32_function_value (const_tree  ret_type,
		     const_tree  func ATTRIBUTE_UNUSED,
                     bool        outgoing ATTRIBUTE_UNUSED)
{
  return LIBCALL_VALUE (TYPE_MODE(ret_type));

}	/* or32_function_value () */


/* -------------------------------------------------------------------------- */
/*!Check if a function is suitable for tail call optimization.

   True if it is OK to do sibling call optimization for the specified call
   expression "exp". "decl" will be the called function, or NULL if this is an
   indirect call.

   It is not uncommon for limitations of calling conventions to prevent tail
   calls to functions outside the current unit of translation, or during PIC
   compilation. The hook is used to enforce these restrictions, as the sibcall
   md pattern can not fail, or fall over to a “normal” call. The criteria for
   successful sibling call optimization may vary greatly between different
   architectures.

   For the OR32, we currently allow sibcall optimization if the -msibcall
   argument is passed.

   JPB 30-Aug-10: Surely we should always allow this?

   @param[in] decl  The function for which we may optimize
   @param[in] exp   The call expression which is candidate for optimization.

   @return  Non-zero (TRUE) if sibcall optimization is permitted, zero (FALSE)
            otherwise.                                                        */
/* -------------------------------------------------------------------------- */
static bool
or32_function_ok_for_sibcall (tree  decl ATTRIBUTE_UNUSED,
			      tree  exp ATTRIBUTE_UNUSED)
{
  return TARGET_MASK_SIBCALL;

}	/* or32_function_ok_for_sibcall () */


/* -------------------------------------------------------------------------- */
/*!Should an argument be passed by reference.

   This target hook should return true if an argument at the position
   indicated by "cum" should be passed by reference. This predicate is queried
   after target independent reasons for being passed by reference, such as
   TREE_ADDRESSABLE ("type").

   If the hook returns TRUE, a copy of that argument is made in memory and a
   pointer to the argument is passed instead of the argument itself. The
   pointer is passed in whatever way is appropriate for passing a pointer to
   that type.

   For the OR32, all aggregates and arguments greater than 8 bytes are passed
   this way.

   @param[in] cum    Position of argument under consideration.
   @param[in[ mode   Not sure what this relates to.
   @param[in] type   Type of the argument.
   @param[in] named  Not sure what this relates to.

   @return  Non-zero (TRUE) if the argument should be passed by reference,
            zero (FALSE) otherwise.                                           */
/* -------------------------------------------------------------------------- */
static bool
or32_pass_by_reference (CUMULATIVE_ARGS   *cum ATTRIBUTE_UNUSED,
                        enum machine_mode  mode ATTRIBUTE_UNUSED,
			const_tree         type,
			bool               named ATTRIBUTE_UNUSED)
{
  return (type && (AGGREGATE_TYPE_P (type) || int_size_in_bytes (type) > 8));

}	/* or32_pass_by_reference () */


/* -------------------------------------------------------------------------- */
/*!Is a frame pointer required?

   This target hook should return TRUE if a function must have and use a frame
   pointer.  This target hook is called in the reload pass. If its return
   value is TRUE the function will have a frame pointer.

   This target hook can in principle examine the current function and decide
   according to the facts, but on most machines the constant false or the
   constant true suffices.  Use FALSE when the machine allows code to be
   generated with no frame pointer, and doing so saves some time or space. Use
   TRUE when there is no possible advantage to avoiding a frame pointer.

   In certain cases, the compiler does not know how to produce valid code
   without a frame pointer. The compiler recognizes those cases and
   automatically gives the function a frame pointer regardless of what
   TARGET_FRAME_POINTER_REQUIRED returns.  You don’t need to worry about them.

   In a function that does not require a frame pointer, the frame pointer
   register can be allocated for ordinary usage, unless you mark it as a fixed
   register. See FIXED_REGISTERS for more information.

   Default return value is false.

   For the OR32 we do not need the frame pointer, so the default would have
   sufficed.

   JPB 30-Aug-10: The version supplied returned TRUE, which is patently the
                  wrong answer. This function really could be eliminated and
                  the default used.

   @return  Non-zero (TRUE) if a frame pointer is not required, zero (FALSE)
            otherwise.                                                        */
/* -------------------------------------------------------------------------- */
static bool
or32_frame_pointer_required (void)
{
	return 1;

}	/* or32_frame_pointer_required () */


/* -------------------------------------------------------------------------- */
/*!How many bytes at the beginning of an argument must be put into registers.

   This target hook returns the number of bytes at the beginning of an
   argument that must be put in registers. The value must be zero for
   arguments that are passed entirely in registers or that are entirely pushed
   on the stack.

   On some machines, certain arguments must be passed partially in registers
   and partially in memory. On these machines, typically the first few words
   of arguments a re passed in registers, and the rest on the stack. If a
   multi-word argument (a double or a structure) crosses that boundary, its
   first few words must be passed in registers and the rest must be
   pushed. This macro tells the compiler when this occurs, and how many bytes
   should go in registers.

   FUNCTION_ARG for these arguments should return the first register to be
   used by the caller for this argument; likewise FUNCTION_INCOMING_ARG, for
   the called function.

   On the OR32 we never split argumetns between registers and memory.

   JPB 30-Aug-10: Is this correct? Surely we should allow this. The ABI spec
                  is incomplete on this point.

   @param[in] cum    Position of argument under consideration.
   @param[in[ mode   Not sure what this relates to.
   @param[in] type   Type of the argument.
   @param[in] named  Not sure what this relates to.

   @return  The number of bytes of the argument to go into registers          */
/* -------------------------------------------------------------------------- */
static int
or32_arg_partial_bytes (CUMULATIVE_ARGS   *cum ATTRIBUTE_UNUSED,
                        enum machine_mode  mode ATTRIBUTE_UNUSED,
                        tree               type ATTRIBUTE_UNUSED,
                        bool               named ATTRIBUTE_UNUSED)
{
  return 0;

}	/* or32_arg_partial_bytes () */


/* -------------------------------------------------------------------------- */
/*!Promote the mode of a function's arguments/return value.

   Like PROMOTE_MODE, but it is applied to outgoing function arguments or
   function return values. The target hook should return the new mode and
   possibly change "*punsignedp" if the promotion should change
   signedness. This function is called only for scalar or pointer types.

   "for_return" allows to distinguish the promotion of arguments and return
   values. If it is 1, a return value is being promoted and
   TARGET_FUNCTION_VALUE must perform the same promotions done here. If it is
   2, the returned mode should be that of the register in which an incoming
   parameter is copied, or the outgoing result is computed; then the hook
   should return the same mode as PROMOTE_MODE, though the signedness may be
   different.

   The default is to not promote arguments and return values. You can also
   define the hook to "default_promote_function_mode_always_promote" if you
   would like to apply the same rules given by PROMOTE_MODE.

   For the OR32, if the size of the mode is integral and less than 4, we
   promote to SImode, otherwise we return the mode we are supplied.

   @param[in]  type        Not sure. Type of the argument?
   @param[in]  mode        The mode of argument/return value to consider.
   @param[out] punsignedp  Signedness of the value.
   @param[in]  fntype      Not sure. Type of the function?
   @param[in]  for_return  1 if a return value, 2 if an incoming value.

   @return  The new mode.                                                     */
/* -------------------------------------------------------------------------- */
static enum machine_mode
or32_promote_function_mode (const_tree         type ATTRIBUTE_UNUSED,
			    enum machine_mode  mode,
			    int               *punsignedp ATTRIBUTE_UNUSED,
			    const_tree         fntype ATTRIBUTE_UNUSED,
			    int                for_return ATTRIBUTE_UNUSED)
{
  return (   (GET_MODE_CLASS (mode) == MODE_INT)
	  && (GET_MODE_SIZE (mode) < 4)) ? SImode : mode;

}	/* or32_promote_function_mode () */


/* -------------------------------------------------------------------------- */
/*!Is this a legitimate address?

  A function that returns whether x (an RTX) is a legitimate memory address on
  the target machine for a memory operand of mode mode.

  Legitimate addresses are defined in two variants: a strict variant and a
  non-strict one.  The strict parameter chooses which variant is desired by
  the caller.

  The strict variant is used in the reload pass. It must be defined so that
  any pseudo- register that has not been allocated a hard register is
  considered a memory reference.  This is because in contexts where some kind
  of register is required, a pseudo-register with no hard register must be
  rejected. For non-hard registers, the strict variant should look up the
  reg_renumber array; it should then proceed using the hard register number in
  the array, or treat the pseudo as a memory reference if the array holds -1.

  The non-strict variant is used in other passes. It must be defined to accept
  all pseudo-registers in every context where some kind of register is
  required.

  Normally, constant addresses which are the sum of a symbol_ref and an
  integer are stored inside a const RTX to mark them as constant. Therefore,
  there is no need to recognize such sums specifically as legitimate
  addresses. Normally you would simply recognize any const as legitimate.

  Usually PRINT_OPERAND_ADDRESS is not prepared to handle constant sums that
  are not marked with const. It assumes that a naked plus indicates
  indexing. If so, then you must reject such naked constant sums as
  illegitimate addresses, so that none of them will be given to
  PRINT_OPERAND_ADDRESS.

  On some machines, whether a symbolic address is legitimate depends on the
  section that the address refers to. On these machines, define the target
  hook TARGET_ENCODE_ SECTION_INFO to store the information into the
  symbol_ref, and then check for it here. When you see a const, you will have
  to look inside it to find the symbol_ref in order to determine the
  section. See the internals manual section on "Assembler Format" for more
  info.

  Some ports are still using a deprecated legacy substitute for this hook, the
  GO_IF_LEGITIMATE_ADDRESS macro. This macro has this syntax:

    #define GO_IF_LEGITIMATE_ADDRESS (mode, x, label )

  and should goto label if the address x is a valid address on the target
  machine for a memory operand of mode mode. Whether the strict or non-strict
  variants are desired is defined by the REG_OK_STRICT macro introduced
  earlier in this section. Using the hook is usually simpler because it limits
  the number of files that are recompiled when changes are made.

   The OR32 only has a single addressing mode, which is a base register with
   16-bit displacement. We can accept just 16-bit constants as addresses (they
   can use r0 as base address, and we can accept plain registers as addresses
   (they can use a displacement of zero).

   @param[in] mode    The mode of the address
   @param[in] x       The address (RTX)
   @param[in] strict  Non-zero (TRUE) if we are in "strict" mode, zero (FALSE)
                      otherwise.

   @return  Non-zero (TRUE) if this is a legitimate address, zero (FALSE)
            otherwise.                                                        */
/* -------------------------------------------------------------------------- */
static bool
or32_legitimate_address_p (enum machine_mode  mode,
			   rtx                x,
			   bool               strict)
{
  /* You might think 16-bit constants are suitable. They can be built into
     addresses using r0 as the base. However this seems to lead to defective
     code. So for now this is a placeholder, and this code is not used.

     if (or32_legitimate_displacement_p (mode, x))
       {
         return  1;
       }
  */

  /* Addresses consisting of a register and 16-bit displacement are also
     suitable. We need the mode, since for double words, we had better be
     able to address the full 8 bytes. */
  if (GET_CODE(x) == PLUS)
    {
      rtx reg = XEXP(x,0);

      /* If valid register... */
      if ((GET_CODE(reg) == REG)
	  && or32_regnum_ok_for_base_p (INTVAL (reg), strict))
	{
	  rtx offset = XEXP(x,1);

	  /* ...and valid offset */
	  if (or32_legitimate_displacement_p (mode, offset))
	    {
	      return 1;
	    }
	}
    }

  /* Addresses consisting of just a register are OK. They can be built into
     addresses using an offset of zero (and an offset of four if double
     word). */
  if (GET_CODE(x) == REG && or32_regnum_ok_for_base_p( INTVAL (x), strict))
    {
      return 1;
    }
 
  return 0;

}	/* or32_legitimate_address_p () */


/* -------------------------------------------------------------------------- */
/*!Initialize a trampoline for nested functions.

   A nested function is defined by *two* pieces of information, the address of
   the function (like any other function) and a pointer to the frame of the
   enclosing function. The latter is required to allow the nested function to
   access local variables in the enclosing function's frame.

   This represents a problem, since a function in C is represented as an
   address that can be held in a single variable as a pointer. Requiring two
   pointers will not fit.

   The solution is documented in "Lexical Closures for C++" by Thomas
   M. Breuel (USENIX C++ Conference Proceedings, October 17-21, 1988). The
   nested function is represented by a small block of code and data on the
   enclosing function's stack frame, which sets up a pointer to the enclosing
   function's stack frame (the static chain pointer) in a register defined by
   the ABI, and then jumps to the code of the function proper.

   The function can be represented as a single pointer to this block of code,
   known as a trampoline, which when called generates both pointers
   needed. The nested function (which knows it is a nested function at compile
   time) can then generate code to access the enclosing frame via the static
   chain register.

   There is a catch that the trampoline is set up as data, but executed as
   instructions. The former will be via the data cache, the latter via the
   instruction cache. There is a risk that a later trampoline will not be seen
   by the instruction cache, so the wrong code will be executed. So the
   instruction cache should be flushed for the trampoline address range.

   This hook is called to initialize a trampoline. "m_tramp" is an RTX for the
   memory block for the trampoline; "fndecl" is the FUNCTION_DECL for the
   nested function; "static_chain" is an RTX for the static chain value that
   should be passed to the function when it is called.

   If the target defines TARGET_ASM_TRAMPOLINE_TEMPLATE, then the first thing
   this hook should do is emit a block move into "m_tramp" from the memory
   block returned by assemble_trampoline_template. Note that the block move
   need only cover the constant parts of the trampoline. If the target
   isolates the variable parts of the trampoline to the end, not all
   TRAMPOLINE_SIZE bytes need be copied.

   If the target requires any other actions, such as flushing caches or
   enabling stack execution, these actions should be performed after
   initializing the trampoline proper.

   For the OR32, no static chain register is used. We choose to use the return
   value (rv) register. The rvh register is used as a temporary. The code is
   based on that for MIPS. The trampoline code is:

              l.movhi r12,hi(end_addr)
              l.ori   r12,lo(end_addr)
              l.lwz   r13,4(r12)
              l.jr    r13
              l.lwz   r11,0(r12)
      end_addr:
              .word   <static chain>
              .word   <nested_function>

   @note For the OR32 we need to flush the instruction cache, which is a
         privileged operation. Needs fixing.

   @param[in] m_tramp      The lowest address of the trampoline on the stack.
   @param[in] fndecl       Declaration of the enclosing function.
   @param[in] chain_value  Static chain pointer to pass to the nested
                           function.                                          */
/* -------------------------------------------------------------------------- */
static void
or32_trampoline_init (rtx   m_tramp,
		      tree  fndecl,
		      rtx   chain_value)
{
  rtx  addr;				/* Start address of the trampoline */
  rtx  end_addr;			/* End address of the code block */

  rtx  high;				/* RTX for the high part of end_addr */
  rtx  low;				/* RTX for the low part of end_addr */
  rtx  opcode;				/* RTX for generated opcodes */
  rtx  mem;				/* RTX for trampoline memory */

  rtx trampoline[5];			/* The trampoline code */

  unsigned int  i;			/* Index into trampoline */
  unsigned int  j;			/* General counter */

  HOST_WIDE_INT  end_addr_offset;	  /* Offset to end of code */
  HOST_WIDE_INT  static_chain_offset;	  /* Offset to stack chain word */
  HOST_WIDE_INT  target_function_offset;  /* Offset to func address word */

  /* Work out the offsets of the pointers from the start of the trampoline
     code.  */
  end_addr_offset        = or32_trampoline_code_size ();
  static_chain_offset    = end_addr_offset;
  target_function_offset = static_chain_offset + GET_MODE_SIZE (ptr_mode);

  /* Get pointers in registers to the beginning and end of the code block.  */
  addr     = force_reg (Pmode, XEXP (m_tramp, 0));
  end_addr = or32_force_binary (Pmode, PLUS, addr, GEN_INT (end_addr_offset));

  /* Build up the code in TRAMPOLINE.

              l.movhi r12,hi(end_addr)
              l.ori   r12,lo(end_addr)
              l.lwz   r13,4(r12)
              l.jr    r13
              l.lwz   r11,0(r12)
       end_addr:
  */

  i = 0;

  /* Break out the high and low parts of the end_addr */
  high = expand_simple_binop (SImode, LSHIFTRT, end_addr, GEN_INT (16),
			      NULL, false, OPTAB_WIDEN);
  low  = convert_to_mode (SImode, gen_lowpart (HImode, end_addr), true);

  /* Emit the l.movhi, adding an operation to OR in the high bits from the
     RTX. */
  opcode = gen_int_mode (OR32_MOVHI (12, 0), SImode);
  trampoline[i++] = expand_simple_binop (SImode, IOR, opcode, high, NULL,
					 false, OPTAB_WIDEN); 
  
  /* Emit the l.ori, adding an operations to OR in the low bits from the
     RTX. */
  opcode = gen_int_mode (OR32_ORI (12, 12, 0), SImode);
  trampoline[i++] = expand_simple_binop (SImode, IOR, opcode, low, NULL,
					 false, OPTAB_WIDEN); 

  /* Emit the l.lwz of the function address. No bits to OR in here, so we can
     do the opcode directly. */
  trampoline[i++] =
    gen_int_mode (OR32_LWZ (13, 12, target_function_offset - end_addr_offset),
		  SImode);

  /* Emit the l.jr of the function. No bits to OR in here, so we can do the
     opcode directly. */
  trampoline[i++] = gen_int_mode (OR32_JR (13), SImode);

  /* Emit the l.lwz of the static chain. No bits to OR in here, so we can
     do the opcode directly. */
  trampoline[i++] =
    gen_int_mode (OR32_LWZ (STATIC_CHAIN_REGNUM, 12,
			    static_chain_offset - end_addr_offset), SImode);

  /* Copy the trampoline code.  Leave any padding uninitialized.  */
  for (j = 0; j < i; j++)
    {
      mem = adjust_address (m_tramp, SImode, j * GET_MODE_SIZE (SImode));
      or32_emit_move (mem, trampoline[j]);
    }

  /* Set up the static chain pointer field.  */
  mem = adjust_address (m_tramp, ptr_mode, static_chain_offset);
  or32_emit_move (mem, chain_value);

  /* Set up the target function field.  */
  mem = adjust_address (m_tramp, ptr_mode, target_function_offset);
  or32_emit_move (mem, XEXP (DECL_RTL (fndecl), 0));

  /* Flushing the trampoline from the instruction cache needs to be done
     here. */

}	/* or32_trampoline_init () */


/* -------------------------------------------------------------------------- */
/*!Provide support for DW_AT_calling_convention

   Define this to enable the dwarf attribute DW_AT_calling_convention to be
   emitted for each function. Instead of an integer return the enum value for
   the DW_CC_ tag.

   To support optional call frame debugging information, you must also define
   INCOMING_RETURN_ADDR_RTX and either set RTX_FRAME_RELATED_P on the prologue
   insns if you use RTL for the prologue, or call "dwarf2out_def_cfa" and
   "dwarf2out_reg_save" as appropriate from TARGET_ASM_FUNCTION_PROLOGUE if
   you don’t.

   For the OR32, it should be sufficient to return DW_CC_normal in all cases.

   @param[in] function  The function requiring debug information

   @return  The enum of the DW_CC tag.                                        */
/* -------------------------------------------------------------------------- */
static int
or32_dwarf_calling_convention (const_tree  function ATTRIBUTE_UNUSED)
{
  return  DW_CC_normal;

}	/* or32_dwarf_calling_convention () */


/* ========================================================================== */
/* Target hook initialization.

   In most cases these use the static functions declared above. They have
   defaults, so must be undefined first, before being redefined.

   The description of what they do is found with the function above, unless it
   is a standard function or a constant, in which case it is defined here (as
   with TARGET_ASM_NAMED_SECTION).

   The final declaration is of the global "targetm" structure. */


/* Default target_flags if no switches specified. */
#undef  TARGET_DEFAULT_TARGET_FLAGS
#define TARGET_DEFAULT_TARGET_FLAGS (MASK_HARD_MUL)

/* Output assembly directives to switch to section name. The section should
   have attributes as specified by flags, which is a bit mask of the SECTION_*
   flags defined in ‘output.h’. If decl is non-NULL, it is the VAR_DECL or
   FUNCTION_DECL with which this section is associated.

   For OR32, we use the default ELF sectioning. */
#undef  TARGET_ASM_NAMED_SECTION
#define TARGET_ASM_NAMED_SECTION  default_elf_asm_named_section

#undef  TARGET_ASM_FUNCTION_PROLOGUE
#define TARGET_ASM_FUNCTION_PROLOGUE or32_output_function_prologue

#undef  TARGET_ASM_FUNCTION_EPILOGUE
#define TARGET_ASM_FUNCTION_EPILOGUE or32_output_function_epilogue

#undef  TARGET_FUNCTION_VALUE
#define TARGET_FUNCTION_VALUE or32_function_value

#undef  TARGET_FUNCTION_OK_FOR_SIBCALL
#define TARGET_FUNCTION_OK_FOR_SIBCALL or32_function_ok_for_sibcall

#undef  TARGET_PASS_BY_REFERENCE
#define TARGET_PASS_BY_REFERENCE or32_pass_by_reference

#undef  TARGET_FRAME_POINTER_REQUIRED
#define TARGET_FRAME_POINTER_REQUIRED or32_frame_pointer_required

#undef  TARGET_ARG_PARTIAL_BYTES
#define TARGET_ARG_PARTIAL_BYTES or32_arg_partial_bytes

/* This target hook returns TRUE if an argument declared in a prototype as an
   integral type smaller than int should actually be passed as an int. In
   addition to avoiding errors in certain cases of mismatch, it also makes for
   better code on certain machines.

   The default is to not promote prototypes.

   For the OR32 we do require this, so use a utility hook, which always
   returns TRUE. */
#undef  TARGET_PROMOTE_PROTOTYPES
#define TARGET_PROMOTE_PROTOTYPES hook_bool_const_tree_true

#undef  TARGET_PROMOTE_FUNCTION_MODE
#define TARGET_PROMOTE_FUNCTION_MODE or32_promote_function_mode

#undef  TARGET_LEGITIMATE_ADDRESS_P
#define TARGET_LEGITIMATE_ADDRESS_P  or32_legitimate_address_p

#undef  TARGET_TRAMPOLINE_INIT
#define TARGET_TRAMPOLINE_INIT  or32_trampoline_init

#undef TARGET_DWARF_CALLING_CONVENTION
#define TARGET_DWARF_CALLING_CONVENTION  or32_dwarf_calling_convention

/* Trampoline stubs are yet to be written. */
/* #define TARGET_ASM_TRAMPOLINE_TEMPLATE */
/* #define TARGET_TRAMPOLINE_INIT */

/* Initialize the GCC target structure.  */
struct gcc_target targetm = TARGET_INITIALIZER;
