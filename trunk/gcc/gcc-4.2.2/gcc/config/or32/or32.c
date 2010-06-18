/* Subroutines for insn-output.c for GNU compiler.  OpenRISC 1000 version.
   Copyright (C) 1987, 1992, 1997, 1999, 2000, 2001, 2002, 2003, 2004,
   2005  Free Software Foundation, Inc
   Contributed by Damjan Lampret <damjanl@bsemi.com> in 1999.
   Major optimizations by Matjaz Breskvar <matjazb@bsemi.com> in 2005.

This file is part of GNU CC.

GNU CC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 1, or (at your option)
any later version.

GNU CC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU CC; see the file COPYING.  If not, write to
the Free Software Foundation, 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.  */

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
#include "optabs.h"
#include "toplev.h"
#include "recog.h"
#include "ggc.h"
#include "except.h"
#include "c-pragma.h"
#include "integrate.h"
#include "tm_p.h"
#include "target.h"
#include "target-def.h"
#include "debug.h"
#include "langhooks.h"

/* Set thist to nonzero if you want l.nop instruction in delay slot
   of l.jr instruction in epilogue. */
#define  NOP_DELAY_SLOT_FILL 0

/* This is the pseudo register number that holds the comparison flags */

#define FLAGS_REG 32

/* Save information from a "cmpxx" operation until the branch or scc is
   emitted.  */
rtx or32_compare_op0, or32_compare_op1;

/* used in function prologue/epilogue generation */
extern int leaf_function;

rtx or32_expand_compare (enum rtx_code code, rtx op0, rtx op1);
void output_ascii_pseudo_op (FILE *, const unsigned char *, int);

/* Local function prototypes */
static bool or32_save_reg_p (int regno);
HOST_WIDE_INT or32_compute_frame_size (HOST_WIDE_INT size);
static rtx emit_frame_insn (rtx insn);
static rtx indexed_memory (rtx base, HOST_WIDE_INT disp);
static int or32_emit_int_cmove (rtx dest, rtx op, rtx true_cond,
				rtx false_cond);

static void or32_output_function_prologue (FILE * file, HOST_WIDE_INT vars);
#undef TARGET_ASM_FUNCTION_PROLOGUE
#define TARGET_ASM_FUNCTION_PROLOGUE or32_output_function_prologue

static void or32_output_function_epilogue (FILE * file, HOST_WIDE_INT vars);
#undef TARGET_ASM_FUNCTION_EPILOGUE
#define TARGET_ASM_FUNCTION_EPILOGUE or32_output_function_epilogue

static bool or32_function_ok_for_sibcall (tree decl, tree exp);
#undef TARGET_FUNCTION_OK_FOR_SIBCALL
#define TARGET_FUNCTION_OK_FOR_SIBCALL or32_function_ok_for_sibcall

static bool or32_pass_by_reference (CUMULATIVE_ARGS *, enum machine_mode, tree, bool);
#undef TARGET_PASS_BY_REFERENCE
#define TARGET_PASS_BY_REFERENCE or32_pass_by_reference

static int or32_arg_partial_bytes (CUMULATIVE_ARGS *, enum machine_mode, tree, bool);
#undef TARGET_ARG_PARTIAL_BYTES
#define TARGET_ARG_PARTIAL_BYTES or32_arg_partial_bytes

/* Initialize the GCC target structure.  */

/* Define this macro if an argument declared as `char' or `short' in a
   prototype should actually be passed as an `int'.  In addition to
   avoiding errors in certain cases of mismatch, it also makes for
   better code on certain machines. */
#undef TARGET_PROMOTE_PROTOTYPES
#define TARGET_PROMOTE_PROTOTYPES hook_bool_tree_true

/* Define this if function arguments should also be promoted using the above
   procedure.  */
#undef TARGET_PROMOTE_FUNCTION_ARGS
#define TARGET_PROMOTE_FUNCTION_ARGS hook_bool_tree_true
   
/* Likewise, if the function return value is promoted.  */
#undef TARGET_PROMOTE_FUNCTION_RETURN
#define TARGET_PROMOTE_FUNCTION_RETURN hook_bool_tree_true

struct gcc_target targetm = TARGET_INITIALIZER;

/* Stack layout we use for pushing and poping saved registers */
struct or32_frame_info
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
};

static struct or32_frame_info frame_info;

/* Add a REG_MAYBE_DEAD note to the insn.  */
static void
or32_maybe_dead (rtx insn)
{
  REG_NOTES (insn) = gen_rtx_EXPR_LIST (REG_MAYBE_DEAD,
					const0_rtx, REG_NOTES (insn));
}

int
or32_print_operand_punct_valid_p (int code)
{
  switch (code)
    {
    case '(':			/* idea taken from sparc; output nop for %( if
				   not optimizing or the slot is not filled. */
    case '%':
      return 1;
    }
  return 0;
}

void
or32_print_operand_address (FILE * file, register rtx addr)
{
  register rtx reg1, reg2, breg, ireg;
  rtx offset;

  switch (GET_CODE (addr))
    {
    case MEM:
      if (GET_CODE (XEXP (addr, 0)) == REG)
	fprintf (file, "%s", reg_names[REGNO (addr)]);
      else
	abort ();
      break;

    case REG:
      fprintf (file, "0(%s)", reg_names[REGNO (addr)]);
      break;

    case PLUS:
      reg1 = 0;
      reg2 = 0;
      ireg = 0;
      breg = 0;
      offset = 0;
      if (GET_CODE (XEXP (addr, 0)) == REG)
	{
	  offset = XEXP (addr, 1);
	  addr = XEXP (addr, 0);
	}
      else if (GET_CODE (XEXP (addr, 1)) == REG)
	{
	  offset = XEXP (addr, 0);
	  addr = XEXP (addr, 1);
	}
      output_address (offset);
      fprintf (file, "(%s)", reg_names[REGNO (addr)]);
      break;

    default:
      /* fprintf(file, "{%d}", GET_CODE (addr)); */
      output_addr_const (file, addr);
    }
}

/* Calulcate and return stack size for current function.  */
static int
calculate_stack_size (int vars, int *lr_save_area,
		      int *fp_save_area, int *gpr_save_area, int *save_area)
{
  int regno;

  *gpr_save_area = 0;
  for (regno = 0; regno < FIRST_PSEUDO_REGISTER; regno++)
    {
      if (regs_ever_live[regno] && !call_used_regs[regno])
	*gpr_save_area += 4;
    }

  *lr_save_area = (!current_function_is_leaf
		   || regs_ever_live[LINK_REGNUM]) ? 4 : 0;
  *fp_save_area = frame_pointer_needed ? 4 : 0;

  *save_area = (OR32_ALIGN (current_function_outgoing_args_size, 4)
		+ *lr_save_area + *fp_save_area);

  return
    (OR32_ALIGN (current_function_outgoing_args_size, 4)
     + *lr_save_area + *fp_save_area + *gpr_save_area + OR32_ALIGN (vars, 4));
}

/* Set up the stack and frame pointer (if desired) for the
   function.  */
static void
or32_output_function_prologue (FILE * file, HOST_WIDE_INT vars)
{
  int save_area;
  int gpr_save_area;
  int lr_save_area;
  int fp_save_area;
  int stack_size;
  int regno;

  if (TARGET_MASK_SCHED_LOGUE)
    return;

#if 0
  save_area = 0;

  for (regno = 0; regno < FIRST_PSEUDO_REGISTER; regno++)
    {
      if (regs_ever_live[regno] && !call_used_regs[regno])
	{
	  save_area += 1;
	}
    }

  if (save_area != 0)
    fprintf (file, "\tl.nop \t0x%x\n", 0x100 + save_area);

  return;
#endif

  if (vars < 0)
    abort ();

  stack_size = calculate_stack_size
    (vars, &lr_save_area, &fp_save_area, &gpr_save_area, &save_area);

  fprintf (file,
	   "\n\t# gpr_save_area %d vars %ld current_function_outgoing_args_size %d\n",
	   gpr_save_area, vars, current_function_outgoing_args_size);

  if (stack_size >= 0x8000)
    {
      fprintf (file, "\tl.movhi   \tr%d,hi(%d)\n", GP_ARG_RETURN, stack_size);
      fprintf (file, "\tl.ori   \tr%d,r%d,lo(%d)\n", GP_ARG_RETURN,
	       GP_ARG_RETURN, stack_size);
      fprintf (file, "\tl.sub   \tr%d,r%d,r%d\n", STACK_POINTER_REGNUM,
	       STACK_POINTER_REGNUM, GP_ARG_RETURN);
    }
  else if (stack_size > 0)
    {
      fprintf (file, "\tl.addi   \tr%d,r%d,%d\n", STACK_POINTER_REGNUM,
	       STACK_POINTER_REGNUM, -stack_size);
    }

  if (fp_save_area)
    {
      fprintf (file, "\tl.sw     \t%d(r%d),r%d\n",
	       OR32_ALIGN (current_function_outgoing_args_size, 4)
	       + lr_save_area, STACK_POINTER_REGNUM, FRAME_POINTER_REGNUM);
      if (stack_size >= 0x8000)
	fprintf (file, "\tl.add   \tr%d,r%d,r%d\n", FRAME_POINTER_REGNUM,
		 STACK_POINTER_REGNUM, GP_ARG_RETURN);
      else
	fprintf (file, "\tl.addi   \tr%d,r%d,%d\n", FRAME_POINTER_REGNUM,
		 STACK_POINTER_REGNUM, stack_size);
    }

  if (lr_save_area)
    {
      fprintf (file, "\tl.sw     \t%d(r%d),r%d\n",
	       OR32_ALIGN (current_function_outgoing_args_size, 4),
	       STACK_POINTER_REGNUM, LINK_REGNUM);
    }

  save_area = (OR32_ALIGN (current_function_outgoing_args_size, 4)
	       + lr_save_area + fp_save_area);

  for (regno = 0; regno < FIRST_PSEUDO_REGISTER; regno++)
    {
      if (regs_ever_live[regno] && !call_used_regs[regno])
	{
	  fprintf (file, "\tl.sw    \t%d(r%d),r%d\n", save_area,
		   STACK_POINTER_REGNUM, regno);
	  save_area += 4;
	}
    }
}

/* Do any necessary cleanup after a function to restore stack, frame,
   and regs.  */
static void
or32_output_function_epilogue (FILE * file, HOST_WIDE_INT vars)
{
  int save_area;
  int gpr_save_area;
  int lr_save_area;
  int fp_save_area;
  int stack_size;
  int regno;

  if (TARGET_MASK_SCHED_LOGUE)
    return;

#if 0
  save_area = 0;

  for (regno = 0; regno < FIRST_PSEUDO_REGISTER; regno++)
    {
      if (regs_ever_live[regno] && !call_used_regs[regno])
	{
	  save_area += 1;
	}
    }

  fprintf (file, "\tl.nop \t0x%x\n", 0x200 + save_area);
  return;
#endif

  stack_size = calculate_stack_size
    (vars, &lr_save_area, &fp_save_area, &gpr_save_area, &save_area);

  if (lr_save_area)
    {
      fprintf (file, "\tl.lwz    \tr%d,%d(r%d)\n", LINK_REGNUM,
	       OR32_ALIGN (current_function_outgoing_args_size, 4),
	       STACK_POINTER_REGNUM);
    }
  if (fp_save_area)
    {
      fprintf (file, "\tl.lwz    \tr%d,%d(r%d)\n", FRAME_POINTER_REGNUM,
	       OR32_ALIGN (current_function_outgoing_args_size, 4)
	       + lr_save_area, STACK_POINTER_REGNUM);
    }
  save_area = (OR32_ALIGN (current_function_outgoing_args_size, 4)
	       + lr_save_area + fp_save_area);

  for (regno = 0; regno < FIRST_PSEUDO_REGISTER; regno++)
    {
      if (regs_ever_live[regno] && !call_used_regs[regno])
	{
	  fprintf (file, "\tl.lwz    \tr%d,%d(r%d)\n", regno, save_area,
		   STACK_POINTER_REGNUM);
	  save_area += 4;
	}
    }

  if (stack_size >= 0x8000)
    {
      fprintf (file, "\tl.movhi   \tr3,hi(%d)\n", stack_size);
      fprintf (file, "\tl.ori   \tr3,r3,lo(%d)\n", stack_size);

      if (!TARGET_MASK_ALIGNED_JUMPS)
	fprintf (file, "\tl.jr  \tr%d\n", LINK_REGNUM);
      else
	fprintf (file, "\t.balignl 0x8,0x15000015,0x4;l.jr  \tr%d\n",
		 LINK_REGNUM);

      fprintf (file, "\tl.add   \tr%d,r%d,r3\n", STACK_POINTER_REGNUM,
	       STACK_POINTER_REGNUM);
    }
  else if (stack_size > 0)
    {
      if (!TARGET_MASK_ALIGNED_JUMPS)
	fprintf (file, "\tl.jr  \tr%d\n", LINK_REGNUM);
      else
	fprintf (file, "\t.balignl 0x8,0x15000015,0x4;l.jr  \tr%d\n",
		 LINK_REGNUM);

      fprintf (file, "\tl.addi   \tr%d,r%d,%d\n", STACK_POINTER_REGNUM,
	       STACK_POINTER_REGNUM, stack_size);
    }
  else
    {
      if (!TARGET_MASK_ALIGNED_JUMPS)
	fprintf (file, "\tl.jr  \tr%d\n", LINK_REGNUM);
      else
	fprintf (file, "\t.balignl 0x8,0x15000015,0x4;l.jr  \tr%d\n",
		 LINK_REGNUM);

      fprintf (file, "\tl.nop\n");
    }

#if 0
  fprintf (file, ".endproc _%s\n", get_function_name ());
#endif
}

/* Compuate full frame size and layout.  SIZE is the size of the
   functions local variables.  Store information in FRAME_INFO and
   return total size of stack frame.  */

HOST_WIDE_INT
or32_compute_frame_size (HOST_WIDE_INT size)
{
  HOST_WIDE_INT args_size;
  HOST_WIDE_INT vars_size;
  HOST_WIDE_INT stack_offset;
  int regno;

  args_size = current_function_outgoing_args_size;
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

  for (regno = 0; regno <= LAST_INT_REG; regno++)
    {
      if (regno == LINK_REGNUM || regno == FRAME_POINTER_REGNUM)
	/* These has already been saved if so needed.  */
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
}


/* Return true if current function must save REGNO.  */
static bool
or32_save_reg_p (int regno)
{
  /* No need to save the faked cc0 register.  */
  if (regno == FLAGS_REG)
    return false;

  /* Check call-saved registers.  */
  if (regs_ever_live[regno] && !call_used_regs[regno])
    return true;

  /* We need to save the old frame pointer before setting up a new
     one.  */
  if (regno == FRAME_POINTER_REGNUM && frame_pointer_needed)
    return true;

  /* We need to save the incoming return address if it is ever clobbered
     within the function.  */
  if (regno == LINK_REGNUM && regs_ever_live[regno])
    return true;

  return false;
}

/* Emit a frame related insn.  Same as emit_insn, but sets
   RTX_FRAME_RELATED_P to one.  */

static rtx
emit_frame_insn (rtx insn)
{
  insn = emit_insn (insn);
  RTX_FRAME_RELATED_P (insn) = 1;
  return (insn);
}

static rtx
indexed_memory (rtx base, HOST_WIDE_INT disp)
{
  return gen_rtx_MEM (Pmode, gen_rtx_PLUS (Pmode, base, GEN_INT (disp)));
}

/* Called after register allocation to add any instructions needed for
   the prologue.  Using a prologue insn is favored compared to putting
   all of the instructions in output_function_prologue(), since it
   allows the scheduler to intermix instructions with the saves of the
   caller saved registers.  In some cases, it might be necessary to
   emit a barrier instruction as the last insn to prevent such
   scheduling.  */

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

      for (regno = 0; regno <= LAST_INT_REG; regno++)
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
}

/* Called after register allocation to add any instructions needed for
   the epilogue.  Using an epilogue insn is favored compared to
   putting all of the instructions in output_function_epilogue(),
   since it allows the scheduler to intermix instructions with the
   restores of the caller saved registers.  In some cases, it might be
   necessary to emit a barrier instruction as the first insn to
   prevent such scheduling.  */

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
      or32_maybe_dead
	(emit_insn
	 (gen_rtx_SET (Pmode, gen_rtx_REG (Pmode, LINK_REGNUM),
		       indexed_memory (stack_pointer_rtx,
				       frame_info.lr_save_offset))));
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

      for (regno = 0; regno <= LAST_INT_REG; regno++)
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
}


void
or32_print_operand (FILE * file, rtx x, int code)
{
  if (code == 'r' && GET_CODE (x) == MEM && GET_CODE (XEXP (x, 0)) == REG)
    fprintf (file, "%s", reg_names[REGNO (XEXP (x, 0))]);
  else if (code == '(')
    {
      if (dbr_sequence_length ())
	fprintf (file, "\t# delay slot filled");
      else
	fprintf (file, "\n\tl.nop\t\t\t# nop delay slot");
    }
  else if (code == 'C')
    {
      switch (GET_CODE (x))
	{
	case EQ:
	  fputs ("eq", file);
	  break;
	case NE:
	  fputs ("ne", file);
	  break;
	case GT:
	  fputs ("gts", file);
	  break;
	case GE:
	  fputs ("ges", file);
	  break;
	case LT:
	  fputs ("lts", file);
	  break;
	case LE:
	  fputs ("les", file);
	  break;
	case GTU:
	  fputs ("gtu", file);
	  break;
	case GEU:
	  fputs ("geu", file);
	  break;
	case LTU:
	  fputs ("ltu", file);
	  break;
	case LEU:
	  fputs ("leu", file);
	  break;
	default:
	  abort ();
	}
    }
  else if (code == 'H')
    {
      if (GET_CODE (x) == REG)
	fprintf (file, "%s", reg_names[REGNO (x) + 1]);
      else
	abort ();
    }
  else if (GET_CODE (x) == REG)
    fprintf (file, "%s", reg_names[REGNO (x)]);
  else if (GET_CODE (x) == MEM)
    output_address (XEXP (x, 0));
  else
    output_addr_const (file, x);
}

/* Generate assembler code for a movdi/movdf */

const char *
or32_output_move_double (rtx * operands)
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
	abort ();
    default:
      abort ();
    }
}

enum rtx_code
or32_reverse_condition (enum machine_mode mode ATTRIBUTE_UNUSED,
			enum rtx_code code)
{
  return reverse_condition (code);
}

enum machine_mode
or32_cc_mode (enum rtx_code code,
	      rtx op0 ATTRIBUTE_UNUSED, rtx op1 ATTRIBUTE_UNUSED)
{

  switch (code)
    {
    case EQ:
      return CCEQmode;
    case NE:
      return CCNEmode;
    case GEU:
      return CCGEUmode;
    case GTU:
      return CCGTUmode;
    case LTU:
      return CCLTUmode;
    case LEU:
      return CCLEUmode;
    case GE:
      return CCGEmode;
    case LT:
      return CCLTmode;
    case GT:
      return CCGTmode;
    case LE:
      return CCLEmode;

    default:
      abort ();
    }
}

/* Generate insn patterns to do an integer compare of OPERANDS.  */

static rtx
or32_expand_int_compare (enum rtx_code code, rtx op0, rtx op1)
{
  enum machine_mode cmpmode;
  rtx tmp, flags;

  cmpmode = SELECT_CC_MODE (code, op0, op1);
  flags = gen_rtx_REG (cmpmode, FLAGS_REG);

  /* This is very simple, but making the interface the same as in the
     FP case makes the rest of the code easier.  */
  tmp = gen_rtx_COMPARE (cmpmode, op0, op1);
  emit_insn (gen_rtx_SET (VOIDmode, flags, tmp));

  /* Return the test that should be put into the flags user, i.e.
     the bcc, scc, or cmov instruction.  */
  return gen_rtx_fmt_ee (code, VOIDmode, flags, const0_rtx);
}

rtx
or32_expand_compare (enum rtx_code code, rtx op0, rtx op1)
{
  return or32_expand_int_compare (code, op0, op1);
}

void
or32_expand_branch (enum rtx_code code, rtx label)
{
  rtx tmp;

  switch (GET_MODE (or32_compare_op0))
    {
    case SImode:
      tmp = or32_expand_compare (code, or32_compare_op0, or32_compare_op1);
      tmp = gen_rtx_IF_THEN_ELSE (VOIDmode,
				  tmp,
				  gen_rtx_LABEL_REF (VOIDmode, label),
				  pc_rtx);
      emit_jump_insn (gen_rtx_SET (VOIDmode, pc_rtx, tmp));
      return;
      
    case SFmode:
      tmp = or32_expand_compare (code, or32_compare_op0, or32_compare_op1);
      tmp = gen_rtx_IF_THEN_ELSE (VOIDmode,
				  tmp,
				  gen_rtx_LABEL_REF (VOIDmode, label),
				  pc_rtx);
      emit_jump_insn (gen_rtx_SET (VOIDmode, pc_rtx, tmp));
      return;
      
    default:
      abort ();
    }

}

static int
or32_emit_int_cmove (rtx dest, rtx op, rtx true_cond, rtx false_cond)
{
  rtx condition_rtx, cr;

  if ((GET_MODE (or32_compare_op0) != SImode) &&
      (GET_MODE (or32_compare_op0) != HImode) &&
      (GET_MODE (or32_compare_op0) != QImode))
    return 0;

  /* We still have to do the compare, because cmov doesn't do a
     compare, it just looks at the FLAG bit set by a previous compare
     instruction.  */

  condition_rtx = or32_expand_compare (GET_CODE (op),
				       or32_compare_op0, or32_compare_op1);

  cr = XEXP (condition_rtx, 0);

  emit_insn (gen_cmov (dest, condition_rtx, true_cond, false_cond, cr));

  return 1;
}


/* Emit a conditional move: move TRUE_COND to DEST if OP of the
   operands of the last comparison is nonzero/true, FALSE_COND if it
   is zero/false.  Return 0 if the hardware has no such operation.  */

int
or32_emit_cmove (rtx dest, rtx op, rtx true_cond, rtx false_cond)
{
  enum machine_mode result_mode = GET_MODE (dest);

  if (GET_MODE (true_cond) != result_mode)
    return 0;

  if (GET_MODE (false_cond) != result_mode)
    return 0;

  /* First, work out if the hardware can do this at all */
  return or32_emit_int_cmove (dest, op, true_cond, false_cond);
}

const char *
or32_output_bf (rtx * operands)
{
  enum rtx_code code;
  enum machine_mode mode_calc, mode_got;

  code = GET_CODE (operands[1]);
  mode_calc = SELECT_CC_MODE (code, or32_compare_op0, or32_compare_op1);
  mode_got = GET_MODE (operands[2]);

  if (!TARGET_MASK_ALIGNED_JUMPS)
    {
      if (mode_calc != mode_got)
	return "l.bnf   \t%l0%(";
      else
	return "l.bf    \t%l0%(";
    }
  else
    {
      if (mode_calc != mode_got)
	return "\t.balignl 0x8,0x15000015,0x4;l.bnf   \t%l0%(";
      else
	return "\t.balignl 0x8,0x15000015,0x4;l.bf    \t%l0%(";
    }
}

const char *
or32_output_cmov (rtx * operands)
{
  enum rtx_code code;
  enum machine_mode mode_calc, mode_got;

  code = GET_CODE (operands[1]);
  mode_calc = SELECT_CC_MODE (code, or32_compare_op0, or32_compare_op1);
  mode_got = GET_MODE (operands[4]);

  if (mode_calc != mode_got)
    return "l.cmov  \t%0,%3,%2";	/* reversed */
  else
    return "l.cmov  \t%0,%2,%3";
}

/* Any function is ok for sibcall optimization if we allow this optimization
 */
static bool
or32_function_ok_for_sibcall (tree decl ATTRIBUTE_UNUSED,
			      tree exp ATTRIBUTE_UNUSED)
{
  return TARGET_MASK_SIBCALL;
}

/* All aggregates and arguments greater than 8 bytes are passed this way.  */
static bool
or32_pass_by_reference (CUMULATIVE_ARGS * cum ATTRIBUTE_UNUSED,
                        enum machine_mode mode ATTRIBUTE_UNUSED,
			                  tree type, bool named ATTRIBUTE_UNUSED)
{
  return (type && (AGGREGATE_TYPE_P (type) || int_size_in_bytes (type) > 8));
}

static int
or32_arg_partial_bytes (CUMULATIVE_ARGS * cum ATTRIBUTE_UNUSED,
                        enum machine_mode mode ATTRIBUTE_UNUSED,
                        tree type ATTRIBUTE_UNUSED,
                        bool named ATTRIBUTE_UNUSED)
{
  return 0;
}

/* For now this is very simple way for sibcall support */

void
or32_expand_sibcall (rtx result ATTRIBUTE_UNUSED, rtx addr, rtx args_size)
{
  emit_call_insn (gen_sibcall_internal (addr, args_size));
}


/* We know it can't be done in one insn when we get here,
   the movsi expander guarantees this.  */
void
or32_emit_set_const32 (rtx op0, rtx op1)
{
  enum machine_mode mode = GET_MODE (op0);
  rtx temp;


  if (GET_CODE (op1) == CONST_INT)
    {
      if (CONST_OK_FOR_LETTER_P (INTVAL (op1) & GET_MODE_MASK (mode), 'K') ||
	        CONST_OK_FOR_LETTER_P (INTVAL (op1), 'M') ||
	        CONST_OK_FOR_LETTER_P (INTVAL (op1), 'I'))
        abort ();
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

#if 0
      /* A symbol, emit in the traditional way.  */

      emit_insn (gen_rtx_SET (VOIDmode, temp, gen_rtx_HIGH (mode, op1)));
      emit_insn (gen_rtx_SET (VOIDmode,
			      op0, gen_rtx_LO_SUM (mode, temp, op1)));
#else
      /* since or32 bfd can not deal with relocs that are not of type
         OR32_CONSTH_RELOC + OR32_CONST_RELOC (ie move high must be
         followed by exactly one lo_sum)
       */
      emit_insn (gen_movsi_insn_big (op0, op1));
#endif
    }
}


/* Functions returning costs and making code size/performance tradeoffs */

int
or32_register_move_cost (enum machine_mode mode ATTRIBUTE_UNUSED,
			 enum reg_class from ATTRIBUTE_UNUSED,
			 enum reg_class to ATTRIBUTE_UNUSED)
{
  return 2;
}

/* A C expressions returning the cost of moving data of MODE from a register to
   or from memory.  */

int
or32_memory_move_cost (enum machine_mode mode ATTRIBUTE_UNUSED,
		       enum reg_class class ATTRIBUTE_UNUSED,
		       int in ATTRIBUTE_UNUSED)
{
  return 2;
}

/* Specify the cost of a branch insn; roughly the number of extra insns that
   should be added to avoid a branch.

   Set this to 3 on the or32 since that is roughly the average cost of an
   unscheduled conditional branch.  

   Cost of 2 and 3 give equal and ~0.7% bigger binaries

*/
int
or32_branch_cost (void)
{
  return 2;
}

/* Stolen from ../arm/arm.c */
#define MAX_ASCII_LEN 51

void
output_ascii_pseudo_op (FILE *stream, const unsigned char *p, int len)
{
  int i;
  int len_so_far = 0;

  fputs ("\t.ascii\t\"", stream);

  for (i = 0; i < len; i++)
    {
      int c = p[i];

      if (len_so_far >= MAX_ASCII_LEN)
        {
          fputs ("\"\n\t.ascii\t\"", stream);
          len_so_far = 0;
        }

      if (ISPRINT (c))
        {
          if (c == '\\' || c == '\"')
            {
              putc ('\\', stream);
              len_so_far++;
            }
          putc (c, stream);
          len_so_far++;
        }
      else
        {
          fprintf (stream, "\\%03o", c);
          len_so_far += 4;
        }
    }

  fputs ("\"\n", stream);
}
