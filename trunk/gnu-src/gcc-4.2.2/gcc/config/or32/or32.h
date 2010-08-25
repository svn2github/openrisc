/* Definitions of target machine for GNU compiler.  OpenRISC 1000 version.
   Copyright (C) 1987, 1988, 1992, 1995, 1996, 1999, 2000, 2001, 2002, 
   2003, 2004, 2005 Free Software Foundation, Inc.
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

#ifndef _OR32_H_
#define _OR32_H_

/* Target CPU builtins */
#define TARGET_CPU_CPP_BUILTINS()		\
  do						\
    {						\
      builtin_define_std ("OR32");		\
      builtin_define_std ("or32");		\
      builtin_assert ("cpu=or32");		\
      builtin_assert ("machine=or32");		\
    }						\
  while (0)


/* A string corresponding to the installation directory for target libraries
   and includes. Make it available to SPEC definitions via EXTRA_SPECS */
#define CONC_DIR(dir1, dir2) dir1 "/../../" dir2
#define TARGET_PREFIX CONC_DIR (STANDARD_EXEC_PREFIX, DEFAULT_TARGET_MACHINE)

#define EXTRA_SPECS                                   \
  { "target_prefix", TARGET_PREFIX }

#undef CPP_SPEC
#define CPP_SPEC "%{mor32-newlib*:-idirafter %(target_prefix)/newlib-include}"

/* Make sure we pick up the crtinit.o and crtfini.o files. */
#undef STARTFILE_SPEC
#define STARTFILE_SPEC "%{!shared:%{mor32-newlib*:%(target_prefix)/newlib/crt0.o} \
                        %{!mor32-newlib*:crt0.o%s} crtinit.o%s}"

#undef ENDFILE_SPEC
#define ENDFILE_SPEC "crtfini.o%s"

/* Specify the newlib library path if necessary */
#undef LINK_SPEC
#define LINK_SPEC "%{mor32-newlib*:-L%(target_prefix)/newlib}"

/* Override previous definitions (linux.h). Newlib doesn't have a profiling
   version of the library, but it does have a debugging version (libg.a) */
#undef LIB_SPEC 
#define LIB_SPEC "%{!mor32-newlib*:%{!p:%{!pg:-lc}}%{p:-lc_p}%{pg:-lc_p}} \
                  %{mor32-newlib:%{!g:-lc -lor32 -u free -lc}            \
                                 %{g:-lg -lor32 -u free -lg}}            \
                  %{mor32-newlib-uart:%{!g:-lc -lor32uart -u free -lc}   \
                                 %{g:-lg -lor32uart -u free -lg}}"

/* Old definition of LIB_SPEC, not longer used. */
/* Which library to get.  The only difference from the default is to get
   libsc.a if -sim is given to the driver.  Repeat -lc -lsysX
   {X=sim,linux}, because libsysX needs (at least) errno from libc, and
   then we want to resolve new unknowns in libc against libsysX, not
   libnosys.  */
/* #define LIB_SPEC \ */
/*  "%{sim*:-lc -lsyssim -lc -lsyssim}\ */
/*   %{!sim*:%{g*:-lg}\ */
/*     %{!p:%{!pg:-lc}}%{p:-lc_p}%{pg:-lc_p} -lbsp}\ */
/*   -lnosys" */

#define TARGET_VERSION fprintf (stderr, " (OpenRISC 1000)");

/* Run-time compilation parameters selecting different hardware subsets.  */

extern int target_flags;

/* Default target_flags if no switches specified.  */
#ifndef TARGET_DEFAULT
#define TARGET_DEFAULT (MASK_HARD_MUL)
#endif

#undef TARGET_ASM_NAMED_SECTION
#define TARGET_ASM_NAMED_SECTION  default_elf_asm_named_section

/* Target machine storage layout */

/* Define this if most significant bit is lowest numbered
   in instructions that operate on numbered bit-fields.
   This is not true on the or32.  */
#define BITS_BIG_ENDIAN 0

/* Define this if most significant byte of a word is the lowest numbered.  */
#define BYTES_BIG_ENDIAN 1

/* Define this if most significant word of a multiword number is numbered.  */
#define WORDS_BIG_ENDIAN 1

/* Number of bits in an addressable storage unit */
#define BITS_PER_UNIT 8

#define BITS_PER_WORD 32
#define SHORT_TYPE_SIZE 16
#define INT_TYPE_SIZE 32
#define LONG_TYPE_SIZE 32
#define LONG_LONG_TYPE_SIZE 64
#define FLOAT_TYPE_SIZE 32
#define DOUBLE_TYPE_SIZE 64
#define LONG_DOUBLE_TYPE_SIZE 64

/* Width of a word, in units (bytes).  */
#define UNITS_PER_WORD 4

/* Width in bits of a pointer.
   See also the macro `Pmode' defined below.  */
#define POINTER_SIZE 32

/* Allocation boundary (in *bits*) for storing pointers in memory.  */
#define POINTER_BOUNDARY 32

/* Allocation boundary (in *bits*) for storing arguments in argument list.  */
#define PARM_BOUNDARY 32

/* Boundary (in *bits*) on which stack pointer should be aligned.  */
#define STACK_BOUNDARY 32

/* Allocation boundary (in *bits*) for the code of a function.  */
#define FUNCTION_BOUNDARY 32

/* Alignment of field after `int : 0' in a structure.  */
#define EMPTY_FIELD_BOUNDARY 8

/* Every structure's size must be a multiple of this.  */
#define STRUCTURE_SIZE_BOUNDARY 32

/* A bitfield declared as `int' forces `int' alignment for the struct.  */
#define PCC_BITFIELD_TYPE_MATTERS 1

/* No data type wants to be aligned rounder than this.  */
#define BIGGEST_ALIGNMENT 32

/* The best alignment to use in cases where we have a choice.  */
#define FASTEST_ALIGNMENT 32

/* Make strings word-aligned so strcpy from constants will be faster.  */
/*
#define CONSTANT_ALIGNMENT(EXP, ALIGN)  				\
  ((TREE_CODE (EXP) == STRING_CST || TREE_CODE (EXP) == CONSTRUCTOR)    \
    && (ALIGN) < FASTEST_ALIGNMENT      				\
   ? FASTEST_ALIGNMENT : (ALIGN))
*/

/* One use of this macro is to increase alignment of medium-size
   data to make it all fit in fewer cache lines.  Another is to
   cause character arrays to be word-aligned so that `strcpy' calls
   that copy constants to character arrays can be done inline.  */         
/*
#define DATA_ALIGNMENT(TYPE, ALIGN)                                     \
  ((((ALIGN) < FASTEST_ALIGNMENT)                                       \
    && (TREE_CODE (TYPE) == ARRAY_TYPE                                  \
        || TREE_CODE (TYPE) == UNION_TYPE                               \
        || TREE_CODE (TYPE) == RECORD_TYPE)) ? FASTEST_ALIGNMENT : (ALIGN))
*/ /* CHECK - btw code gets bigger with this one */

/* Define this if move instructions will actually fail to work
   when given unaligned data.  */
#define STRICT_ALIGNMENT 1 /* CHECK */

/* Align an address */
#define OR32_ALIGN(n,a) (((n) + (a) - 1) & ~((a) - 1))

/* Define if operations between registers always perform the operation
   on the full register even if a narrower mode is specified.  */
#define WORD_REGISTER_OPERATIONS  /* CHECK */
 

/* Define if loading in MODE, an integral mode narrower than BITS_PER_WORD
   will either zero-extend or sign-extend.  The value of this macro should
   be the code that says which one of the two operations is implicitly
   done, NIL if none.  */
#define LOAD_EXTEND_OP(MODE) ZERO_EXTEND

/* Define this macro if it is advisable to hold scalars in registers
   in a wider mode than that declared by the program.  In such cases,
   the value is constrained to be within the bounds of the declared
   type, but kept valid in the wider mode.  The signedness of the
   extension may differ from that of the type. */
#define PROMOTE_MODE(MODE, UNSIGNEDP, TYPE)     \
  if (GET_MODE_CLASS (MODE) == MODE_INT         \
      && GET_MODE_SIZE (MODE) < UNITS_PER_WORD) \
    (MODE) = SImode;
  /* CHECK */

/*
 * brings 0.4% improvment in static size for linux
 *
#define PROMOTE_FOR_CALL_ONLY
*/

/* Define this macro if it is as good or better to call a constant
   function address than to call an address kept in a register.  */
#define NO_FUNCTION_CSE 1 /* check */
   
/* Standard register usage.  */

/* Number of actual hardware registers.
   The hardware registers are assigned numbers for the compiler
   from 0 to just below FIRST_PSEUDO_REGISTER.
   All registers that the compiler knows about must be given numbers,
   even those that are not normally considered general registers.  */
#define FIRST_PSEUDO_REGISTER 33
#define LAST_INT_REG (FIRST_PSEUDO_REGISTER - 1)

/* 1 for registers that have pervasive standard uses
   and are not available for the register allocator.
   On the or32, these are r1 as stack pointer and 
   r2 as frame/arg pointer.  r9 is link register, r0
   is zero, r10 is linux thread */
#define FIXED_REGISTERS { \
  1, 1, 1, 0, 0, 0, 0, 0, \
  0, 1, 1, 0, 0, 0, 0, 0, \
  0, 0, 0, 0, 0, 0, 0, 0, \
  0, 0, 0, 0, 0, 0, 0, 0, 1}
/* 1 for registers not available across function calls.
   These must include the FIXED_REGISTERS and also any
   registers that can be used without being saved.
   The latter must include the registers where values are returned
   and the register where structure-value addresses are passed.
   Aside from that, you can include as many other registers as you like.  */
#define CALL_USED_REGISTERS { \
  1, 1, 1, 1, 1, 1, 1, 1, \
  1, 1, 1, 1, 0, 1, 0, 1, \
  0, 1, 0, 1, 0, 1, 0, 1, \
  0, 1, 0, 1, 0, 1, 0, 1, 1}

/* stack pointer: must be FIXED and CALL_USED */
/* frame pointer: must be FIXED and CALL_USED */

/* Return number of consecutive hard regs needed starting at reg REGNO
   to hold something of mode MODE.
   This is ordinarily the length in words of a value of mode MODE
   but can be less for certain modes in special long registers.
   On the or32, all registers are one word long.  */
#define HARD_REGNO_NREGS(REGNO, MODE)   \
 ((GET_MODE_SIZE (MODE) + UNITS_PER_WORD - 1) / UNITS_PER_WORD)

/* Value is 1 if hard register REGNO can hold a value of machine-mode MODE. */
#define HARD_REGNO_MODE_OK(REGNO, MODE) 1

/* Value is 1 if it is a good idea to tie two pseudo registers
   when one has mode MODE1 and one has mode MODE2.
   If HARD_REGNO_MODE_OK could produce different values for MODE1 and MODE2,
   for any hard reg, then this must be 0 for correct output.  */
#define MODES_TIEABLE_P(MODE1, MODE2)  1

/* A C expression returning the cost of moving data from a register of class
   CLASS1 to one of CLASS2.  */
#define REGISTER_MOVE_COST or32_register_move_cost

/* A C expressions returning the cost of moving data of MODE from a register to
   or from memory.  */
#define MEMORY_MOVE_COST or32_memory_move_cost

/* Specify the cost of a branch insn; roughly the number of extra insns that
   should be added to avoid a branch. */
#define BRANCH_COST or32_branch_cost()

/* Specify the registers used for certain standard purposes.
   The values of these macros are register numbers.  */

/* Register to use for pushing function arguments.  */
#define STACK_POINTER_REGNUM 1

/* Base register for access to local variables of the function.  */
#define FRAME_POINTER_REGNUM 2

/* Link register. */
#define LINK_REGNUM 9

/* Value should be nonzero if functions must have frame pointers.
   Zero means the frame pointer need not be set up (and parms
   may be accessed via the stack pointer) in functions that seem suitable.
   This is computed in `reload', in reload1.c.  */
#define FRAME_POINTER_REQUIRED 0

/* De ne this macro if debugging can be performed even without a frame pointer.
   If this macro is de ned, GCC will turn on the `-fomit-frame-pointer' option
   whenever `-O' is specifed.
 */
/*
#define CAN_DEBUG_WITHOUT_FP 
 */

#define INITIAL_FRAME_POINTER_OFFSET(DEPTH)						\
{ int regno;										\
  int offset = 0;									\
  for( regno=0; regno < FIRST_PSEUDO_REGISTER;  regno++ )				\
    if( regs_ever_live[regno] && !call_used_regs[regno] )				\
      offset += 4;									\
  (DEPTH) = (!current_function_is_leaf || regs_ever_live[LINK_REGNUM] ? 4 : 0)	+	\
		(frame_pointer_needed ? 4 : 0)					+	\
		offset								+	\
		OR32_ALIGN(current_function_outgoing_args_size,4)		+	\
		OR32_ALIGN(get_frame_size(),4);						\
}

/* Base register for access to arguments of the function.  */
#define ARG_POINTER_REGNUM FRAME_POINTER_REGNUM

/* Register in which static-chain is passed to a function.  */
#define STATIC_CHAIN_REGNUM 0

/* Register in which address to store a structure value
   is passed to a function.  */
/*#define STRUCT_VALUE_REGNUM 0*/

/* Pass address of result struct to callee as "invisible" first argument */
#define STRUCT_VALUE 0

/* -----------------------[ PHX start ]-------------------------------- */

/* Define the classes of registers for register constraints in the
   machine description.  Also define ranges of constants.

   One of the classes must always be named ALL_REGS and include all hard regs.
   If there is more than one class, another class must be named NO_REGS
   and contain no registers.

   The name GENERAL_REGS must be the name of a class (or an alias for
   another name such as ALL_REGS).  This is the class of registers
   that is allowed by "g" or "r" in a register constraint.
   Also, registers outside this class are allocated only when
   instructions express preferences for them.

   GENERAL_REGS and BASE_REGS classess are the same on or32.

   The classes must be numbered in nondecreasing order; that is,
   a larger-numbered class must never be contained completely
   in a smaller-numbered class.

   For any two classes, it is very desirable that there be another
   class that represents their union.  */
   
/* The or32 has only one kind of registers, so NO_REGS, GENERAL_REGS
   and ALL_REGS are the only classes.  */

enum reg_class 
{ 
  NO_REGS,
  GENERAL_REGS,
  CR_REGS,
  ALL_REGS,
  LIM_REG_CLASSES 
};

#define N_REG_CLASSES (int) LIM_REG_CLASSES

/* Give names of register classes as strings for dump file.   */

#define REG_CLASS_NAMES							\
{									\
  "NO_REGS",								\
  "GENERAL_REGS",   							\
  "ALL_REGS"								\
}


/* Define which registers fit in which classes.
   This is an initializer for a vector of HARD_REG_SET
   of length N_REG_CLASSES.  */

/* An initializer containing the contents of the register classes,
   as integers which are bit masks.  The Nth integer specifies the
   contents of class N.  The way the integer MASK is interpreted is
   that register R is in the class if `MASK & (1 << R)' is 1.

   When the machine has more than 32 registers, an integer does not
   suffice.  Then the integers are replaced by sub-initializers,
   braced groupings containing several integers.  Each
   sub-initializer must be suitable as an initializer for the type
   `HARD_REG_SET' which is defined in `hard-reg-set.h'.  */

#define REG_CLASS_CONTENTS			     \
{						     \
  { 0x00000000, 0x00000000 }, /* NO_REGS */	     \
  { 0xffffffff, 0x00000001 }, /* GENERAL_REGS */     \
  { 0xffffffff, 0x00000000 }  /* ALL_REGS */	     \
}

/* The same information, inverted:
   Return the class number of the smallest class containing
   reg number REGNO.  This could be a conditional expression
   or could index an array.  */

#define REGNO_REG_CLASS(REGNO)			\
 ((REGNO) < 32 ? GENERAL_REGS			\
  : NO_REGS)

/* The class value for index registers, and the one for base regs.  */
#define INDEX_REG_CLASS GENERAL_REGS
#define BASE_REG_CLASS GENERAL_REGS

/* Get reg_class from a letter such as appears in the machine description.  */

#define REG_CLASS_FROM_LETTER(C) NO_REGS

#if 1
/* The letters I, J, K, L and M in a register constraint string
   can be used to stand for particular ranges of immediate operands.
   This macro defines what the ranges are.
   C is the letter, and VALUE is a constant value.
   Return 1 if VALUE is in the range specified by C.  */

#define CONST_OK_FOR_LETTER_P(VALUE, C)  \
    (  (C) == 'I' ? ((VALUE) >=-32768 && (VALUE) <=32767) \
     : (C) == 'J' ? ((VALUE) >=0 && (VALUE) <=0) \
     : (C) == 'K' ? ((VALUE) >=0 && (VALUE) <=65535) \
     : (C) == 'L' ? ((VALUE) >=0 && (VALUE) <=31) \
     : (C) == 'M' ? (((VALUE) & 0xffff) == 0 )		\
     : (C) == 'N' ? ((VALUE) >=-33554432 && (VALUE) <=33554431) \
     : (C) == 'O' ? ((VALUE) >=0 && (VALUE) <=0) \
     : 0 )
#else

/* The letters I, J, K, L, M, N, and P in a register constraint string
   can be used to stand for particular ranges of immediate operands.
   This macro defines what the ranges are.
   C is the letter, and VALUE is a constant value.
   Return 1 if VALUE is in the range specified by C.

   `I' is a signed 16-bit constant
   `J' is a constant with only the high-order 16 bits nonzero
   `K' is a constant with only the low-order 16 bits nonzero
   `L' is a signed 16-bit constant shifted left 16 bits
   `M' is a constant that is greater than 31
   `N' is a positive constant that is an exact power of two
   `O' is the constant zero
   `P' is a constant whose negation is a signed 16-bit constant */

#define CONST_OK_FOR_LETTER_P(VALUE, C)					\
   ( (C) == 'I' ? (unsigned HOST_WIDE_INT) ((VALUE) + 0x8000) < 0x10000	\
   : (C) == 'J' ? ((VALUE) & (~ (unsigned HOST_WIDE_INT) 0xffff0000)) == 0 \
   : (C) == 'K' ? ((VALUE) & (~ (HOST_WIDE_INT) 0xffff)) == 0		\
   : (C) == 'L' ? (((VALUE) & 0xffff) == 0				\
		   && ((VALUE) >> 31 == -1 || (VALUE) >> 31 == 0))	\
   : (C) == 'M' ? (VALUE) > 31						\
   : (C) == 'N' ? (VALUE) > 0 && exact_log2 (VALUE) >= 0		\
   : (C) == 'O' ? (VALUE) == 0						\
   : (C) == 'P' ? (unsigned HOST_WIDE_INT) ((- (VALUE)) + 0x8000) < 0x10000 \
   : 0)
#endif

/* -----------------------[ PHX stop ]-------------------------------- */

/* Similar, but for floating constants, and defining letters G and H.
   Here VALUE is the CONST_DOUBLE rtx itself.  */

#define CONST_DOUBLE_OK_FOR_LETTER_P(VALUE, C) 1

/* Given an rtx X being reloaded into a reg required to be
   in class CLASS, return the class of reg to actually use.
   In general this is just CLASS; but on some machines
   in some cases it is preferable to use a more restrictive class.  */

#define PREFERRED_RELOAD_CLASS(X,CLASS)  (CLASS)

/* Return the maximum number of consecutive registers
   needed to represent mode MODE in a register of class CLASS.  */
/* On the or32, this is always the size of MODE in words,
   since all registers are the same size.  */
#define CLASS_MAX_NREGS(CLASS, MODE)	\
 ((GET_MODE_SIZE (MODE) + UNITS_PER_WORD - 1) / UNITS_PER_WORD)

/* Stack layout; function entry, exit and calling.  */

/* Define this if pushing a word on the stack
   makes the stack pointer a smaller address.  */
#define STACK_GROWS_DOWNWARD 1

/* Define this if the nominal address of the stack frame
   is at the high-address end of the local variables;
   that is, each additional local variable allocated
   goes at a more negative offset in the frame.  */
#define FRAME_GROWS_DOWNWARD 1

/* Offset within stack frame to start allocating local variables at.
   If FRAME_GROWS_DOWNWARD, this is the offset to the END of the
   first local allocated.  Otherwise, it is the offset to the BEGINNING
   of the first local allocated.  */
#define STARTING_FRAME_OFFSET 0

/* Offset of first parameter from the argument pointer register value.  */
#define FIRST_PARM_OFFSET(FNDECL) 0

/* Define this if stack space is still allocated for a parameter passed
   in a register.  The value is the number of bytes allocated to this
   area.  */
/*
#define REG_PARM_STACK_SPACE(FNDECL) (UNITS_PER_WORD * GP_ARG_NUM_REG)
*/
/* Define this if the above stack space is to be considered part of the
   space allocated by the caller.  */
/*
#define OUTGOING_REG_PARM_STACK_SPACE 
*/   
/* Define this macro if `REG_PARM_STACK_SPACE' is defined, but the
   stack parameters don't skip the area specified by it. */
/*
#define STACK_PARMS_IN_REG_PARM_AREA
*/
/* Define this if the maximum size of all the outgoing args is to be
   accumulated and pushed during the prologue.  The amount can be
   found in the variable current_function_outgoing_args_size.  */
#define ACCUMULATE_OUTGOING_ARGS 1

/* Value is 1 if returning from a function call automatically
   pops the arguments described by the number-of-args field in the call.
   FUNDECL is the declaration node of the function (as a tree),
   FUNTYPE is the data type of the function (as a tree),
   or for a library call it is an identifier node for the subroutine name.

   On the Vax, the RET insn always pops all the args for any function.  */
/* SIMON */
/*#define RETURN_POPS_ARGS(FUNDECL,FUNTYPE,SIZE) (SIZE)*/
#define RETURN_POPS_ARGS(FUNDECL,FUNTYPE,SIZE) 0

/* Minimum and maximum general purpose registers used to hold arguments.  */
#define GP_ARG_MIN_REG 3
#define GP_ARG_MAX_REG 8
#define GP_ARG_NUM_REG (GP_ARG_MAX_REG - GP_ARG_MIN_REG + 1) 

/* Return registers */
#define GP_ARG_RETURN 11 

/* Define how to find the value returned by a function.
   VALTYPE is the data type of the value (as a tree).
   If the precise function being called is known, FUNC is its FUNCTION_DECL;
   otherwise, FUNC is 0.  */

/* Return value is in R11.  */
#define FUNCTION_VALUE(VALTYPE, FUNC) LIBCALL_VALUE (TYPE_MODE (VALTYPE))

/* Define how to find the value returned by a library function
   assuming the value has mode MODE.  */

/* Return value is in R11.  */   

#define LIBCALL_VALUE(MODE)                                             \
  gen_rtx_REG(								\
	   ((GET_MODE_CLASS (MODE) != MODE_INT				\
	     || GET_MODE_SIZE (MODE) >= 4)				\
	    ? (MODE)							\
	    : SImode),							\
	    GP_ARG_RETURN)

/* Define this if PCC uses the nonreentrant convention for returning
   structure and union values.  */

/*#define PCC_STATIC_STRUCT_RETURN */

/* 1 if N is a possible register number for a function value.
   R3 to R8 are possible (set to 1 in CALL_USED_REGISTERS)  */

#define FUNCTION_VALUE_REGNO_P(N)  ((N) == GP_ARG_RETURN)
 
/* 1 if N is a possible register number for function argument passing. */
     
#define FUNCTION_ARG_REGNO_P(N) \
   ((N) >= GP_ARG_MIN_REG && (N) <= GP_ARG_MAX_REG)

/* A code distinguishing the floating point format of the target
   machine.  There are three defined values: IEEE_FLOAT_FORMAT,
   VAX_FLOAT_FORMAT, and UNKNOWN_FLOAT_FORMAT.  */
      
#define TARGET_FLOAT_FORMAT IEEE_FLOAT_FORMAT
#define FLOAT_WORDS_BIG_ENDIAN 1
       
/* Define a data type for recording info about an argument list
   during the scan of that argument list.  This data type should
   hold all necessary information about the function itself
   and about the args processed so far, enough to enable macros
   such as FUNCTION_ARG to determine where the next arg should go.

   On the vax, this is a single integer, which is a number of bytes
   of arguments scanned so far.  */

#define CUMULATIVE_ARGS int

/* Initialize a variable CUM of type CUMULATIVE_ARGS
   for a call to a function whose data type is FNTYPE.
   For a library call, FNTYPE is 0.

   On the vax, the offset starts at 0.  */

/* The regs member is an integer, the number of arguments got into
   registers so far.  */
#define INIT_CUMULATIVE_ARGS(CUM, FNTYPE, LIBNAME, FNDECL, N_NAMED_ARGS) \
 (CUM = 0)

/* Define intermediate macro to compute the size (in registers) of an argument
   for the or32.  */ 

/* The ROUND_ADVANCE* macros are local to this file.  */
/* Round SIZE up to a word boundary.  */
#define ROUND_ADVANCE(SIZE) \
(((SIZE) + UNITS_PER_WORD - 1) / UNITS_PER_WORD)

/* Round arg MODE/TYPE up to the next word boundary.  */
#define ROUND_ADVANCE_ARG(MODE, TYPE) \
((MODE) == BLKmode				\
 ? ROUND_ADVANCE (int_size_in_bytes (TYPE))	\
 : ROUND_ADVANCE (GET_MODE_SIZE (MODE)))

/* Round CUM up to the necessary point for argument MODE/TYPE.  */
/* This is either rounded to nearest reg or nearest double-reg boundary */
#define ROUND_ADVANCE_CUM(CUM, MODE, TYPE) \
((((MODE) == BLKmode ? TYPE_ALIGN (TYPE) : GET_MODE_BITSIZE (MODE)) \
  > BITS_PER_WORD)	\
 ? (((CUM) + 1) & ~1)	\
 : (CUM))
  
/* Update the data in CUM to advance over an argument
   of mode MODE and data type TYPE.
   (TYPE is null for libcalls where that information may not be available.)  */
#define FUNCTION_ARG_ADVANCE(CUM, MODE, TYPE, NAMED) \
((CUM) = (ROUND_ADVANCE_CUM ((CUM), (MODE), (TYPE)) \
	  + ROUND_ADVANCE_ARG ((MODE), (TYPE))))
                       
/* Return boolean indicating arg of type TYPE and mode MODE will be passed in
   a reg.  This includes arguments that have to be passed by reference as the
   pointer to them is passed in a reg if one is available (and that is what
   we're given).
   When passing arguments NAMED is always 1.  When receiving arguments NAMED
   is 1 for each argument except the last in a stdarg/varargs function.  In
   a stdarg function we want to treat the last named arg as named.  In a
   varargs function we want to treat the last named arg (which is
   `__builtin_va_alist') as unnamed.
   This macro is only used in this file.  */
#define PASS_IN_REG_P(CUM, MODE, TYPE, NAMED) \
((NAMED)                         					\
 && ((ROUND_ADVANCE_CUM ((CUM), (MODE), (TYPE))				\
      + ROUND_ADVANCE_ARG ((MODE), (TYPE))				\
      <= GP_ARG_NUM_REG)))

/* Determine where to put an argument to a function.
   Value is zero to push the argument on the stack,
   or a hard register in which to store the argument.

   MODE is the argument's machine mode.
   TYPE is the data type of the argument (as a tree).
    This is null for libcalls where that information may
    not be available.
   CUM is a variable of type CUMULATIVE_ARGS which gives info about
    the preceding args and about the function being called.
   NAMED is nonzero if this argument is a named parameter
    (otherwise it is an extra parameter matching an ellipsis).  */
/* On the ARC the first MAX_ARC_PARM_REGS args are normally in registers
   and the rest are pushed.  */
#define FUNCTION_ARG(CUM, MODE, TYPE, NAMED) \
(PASS_IN_REG_P ((CUM), (MODE), (TYPE), (NAMED))				\
 ? gen_rtx_REG ((MODE), ROUND_ADVANCE_CUM ((CUM), (MODE), (TYPE)) + GP_ARG_MIN_REG)	\
 : 0)

/* Output assembler code to FILE to increment profiler label # LABELNO
   for profiling a function entry.  */

#define FUNCTION_PROFILER(FILE, LABELNO)                  \
  fprintf (FILE, "\tl.movhi\tr3,hi(.LP%d)\n", (LABELNO)); \
  fprintf (FILE, "\tl.ori\tr3,r3,lo(.LP%d)\n", (LABELNO)); \
  fprintf (FILE, "\tl.j\tmcount\n");                      \
  fprintf (FILE, "\tl.nop\n");

/* EXIT_IGNORE_STACK should be nonzero if, when returning from a function,
   the stack pointer does not matter.  The value is tested only in
   functions that have frame pointers.
   No definition is equivalent to always zero.  */

#define EXIT_IGNORE_STACK 0

/* If the memory address ADDR is relative to the frame pointer,
   correct it to be relative to the stack pointer instead.
   This is for when we don't use a frame pointer.
   ADDR should be a variable name.  */

#define FIX_FRAME_POINTER_ADDRESS(ADDR,DEPTH) \
{ int offset = -1;							\
  rtx regs = stack_pointer_rtx;						\
  if (ADDR == frame_pointer_rtx)					\
    offset = 0;								\
  else if (GET_CODE (ADDR) == PLUS && XEXP (ADDR, 1) == frame_pointer_rtx \
	   && GET_CODE (XEXP (ADDR, 0)) == CONST_INT)			\
    offset = INTVAL (XEXP (ADDR, 0));					\
  else if (GET_CODE (ADDR) == PLUS && XEXP (ADDR, 0) == frame_pointer_rtx \
	   && GET_CODE (XEXP (ADDR, 1)) == CONST_INT)			\
    offset = INTVAL (XEXP (ADDR, 1));					\
  else if (GET_CODE (ADDR) == PLUS && XEXP (ADDR, 0) == frame_pointer_rtx) \
    { rtx other_reg = XEXP (ADDR, 1);					\
      offset = 0;							\
      regs = gen_rtx_PLUS( Pmode, stack_pointer_rtx, other_reg); }	\
  else if (GET_CODE (ADDR) == PLUS && XEXP (ADDR, 1) == frame_pointer_rtx) \
    { rtx other_reg = XEXP (ADDR, 0);					\
      offset = 0;							\
      regs = gen_rtx_PLUS( Pmode, stack_pointer_rtx, other_reg); }	\
  if (offset >= 0)							\
    { int regno;							\
      extern char call_used_regs[];					\
      offset += 4; /* I don't know why??? */				\
      for (regno = 0; regno < FIRST_PSEUDO_REGISTER; regno++)		\
        if (regs_ever_live[regno] && ! call_used_regs[regno])		\
          offset += 4;							\
      ADDR = plus_constant (regs, offset + (DEPTH)); } }


/* Addressing modes, and classification of registers for them.  */

/* #define HAVE_POST_INCREMENT */
/* #define HAVE_POST_DECREMENT */

/* #define HAVE_PRE_DECREMENT */
/* #define HAVE_PRE_INCREMENT */

/* Macros to check register numbers against specific register classes.  */

#define MAX_REGS_PER_ADDRESS 1

/* True if X is an rtx for a constant that is a valid address.  */
#define CONSTANT_ADDRESS_P(X) 						\
  (GET_CODE (X) == LABEL_REF || GET_CODE (X) == SYMBOL_REF              \
   || GET_CODE (X) == CONST_INT || GET_CODE (X) == CONST                \
   || GET_CODE (X) == HIGH)        

#define REGNO_OK_FOR_BASE_P(REGNO)                                          \
((REGNO) < FIRST_PSEUDO_REGISTER ? ((REGNO) > 0 && (REGNO) <= LAST_INT_REG) \
 : (reg_renumber[REGNO] > 0 && (reg_renumber[REGNO] <= LAST_INT_REG )))

#ifdef REG_OK_STRICT
/* Strict version, used in reload pass. This should not
 * accept pseudo registers.
 */
#define REG_OK_FOR_BASE_P(X) REGNO_OK_FOR_BASE_P(REGNO(X))
#else
/* Accept an int register or a pseudo reg. */
#define REG_OK_FOR_BASE_P(X) (REGNO(X) <= LAST_INT_REG || \
                              REGNO(X) >= FIRST_PSEUDO_REGISTER)
#endif

/*
 * OR32 doesn't have any indexed addressing.
 */
#define REG_OK_FOR_INDEX_P(X) 0
#define REGNO_OK_FOR_INDEX_P(X) 0

#define LEGITIMATE_ADDRESS_INTEGER_P(X,OFFSET)          \
 (GET_CODE (X) == CONST_INT && SMALL_INT(X))

#define LEGITIMATE_OFFSET_ADDRESS_P(MODE,X)             \
 (GET_CODE (X) == PLUS                                  \
  && GET_CODE (XEXP (X, 0)) == REG                      \
  && REG_OK_FOR_BASE_P (XEXP (X, 0))                    \
  && LEGITIMATE_ADDRESS_INTEGER_P (XEXP (X, 1), 0)      \
  && (((MODE) != DFmode && (MODE) != DImode)            \
      || LEGITIMATE_ADDRESS_INTEGER_P (XEXP (X, 1), 4)))

#define LEGITIMATE_NONOFFSET_ADDRESS_P(MODE,X)          \
             (GET_CODE(X) == REG && REG_OK_FOR_BASE_P(X))
/*
 * OR32 only has one addressing mode:
 * register + 16 bit signed offset.
 */
#define GO_IF_LEGITIMATE_ADDRESS(MODE,X,ADDR)           \
  if(LEGITIMATE_OFFSET_ADDRESS_P(MODE,X)) goto ADDR;    \
  if(LEGITIMATE_NONOFFSET_ADDRESS_P(MODE,X)) goto ADDR;

/*
 * We have to force symbol_ref's into registers here
 * because nobody else seems to want to do that!
 */
#define LEGITIMIZE_ADDRESS(X,OLDX,MODE,WIN) {}
/*
{ if (GET_CODE (x) == SYMBOL_REF)                               \
    (X) = copy_to_reg (X);                                      \
  if (memory_address_p (MODE, X))                               \
    goto WIN;                                                   \
}
*/

/*
 * OR32 addresses do not depend on the machine mode they are
 * being used in.
 */
#define GO_IF_MODE_DEPENDENT_ADDRESS(addr,label)

/* OR32 has 16 bit immediates.
 */
#define SMALL_INT(X) (INTVAL(X) >= -32768 && INTVAL(X) <= 32767)

#define LEGITIMATE_CONSTANT_P(x) (GET_CODE(x) != CONST_DOUBLE)

/* Specify the machine mode that this machine uses
   for the index in the tablejump instruction.  */
#define CASE_VECTOR_MODE SImode

/* Define as C expression which evaluates to nonzero if the tablejump
   instruction expects the table to contain offsets from the address of the
   table.
   Do not define this if the table should contain absolute addresses. */
/* #define CASE_VECTOR_PC_RELATIVE 1 */

/* Define this as 1 if `char' should by default be signed; else as 0.  */
#define DEFAULT_SIGNED_CHAR 1

/* This flag, if defined, says the same insns that convert to a signed fixnum
   also convert validly to an unsigned one.  */
#define FIXUNS_TRUNC_LIKE_FIX_TRUNC

/* Max number of bytes we can move from memory to memory
   in one reasonably fast instruction.  */
#define MOVE_MAX 4

/* Define this if zero-extension is slow (more than one real instruction).  */
/* #define SLOW_ZERO_EXTEND */

/* Nonzero if access to memory by bytes is slow and undesirable.  
   For RISC chips, it means that access to memory by bytes is no
   better than access by words when possible, so grab a whole word
   and maybe make use of that.  */
#define SLOW_BYTE_ACCESS 1

/* Define if shifts truncate the shift count
   which implies one can omit a sign-extension or zero-extension
   of a shift count.  */
/* #define SHIFT_COUNT_TRUNCATED */

/* Value is 1 if truncating an integer of INPREC bits to OUTPREC bits
   is done just by pretending it is already truncated.  */
#define TRULY_NOOP_TRUNCATION(OUTPREC, INPREC) 1

/* Specify the machine mode that pointers have.
   After generation of rtl, the compiler makes no further distinction
   between pointers and any other objects of this machine mode.  */
#define Pmode SImode

/* A function address in a call instruction
   is a byte address (for indexing purposes)
   so give the MEM rtx a byte's mode.  */
#define FUNCTION_MODE SImode

/* Given a comparison code (EQ, NE, etc.) and the first operand of a
   COMPARE, return the mode to be used for the comparison.
*/

#define SELECT_CC_MODE(OP, X, Y) or32_cc_mode ((OP), (X), (Y))

/* Can the condition code MODE be safely reversed?  This is safe in
   all cases on this port, because at present it doesn't use the
   trapping FP comparisons (fcmpo).  */
#define REVERSIBLE_CC_MODE(MODE) 1

/* Given a condition code and a mode, return the inverse condition.  */
#define REVERSE_CONDITION(CODE, MODE) or32_reverse_condition (MODE, CODE)


/* Control the assembler format that we output.  */

/* A C string constant describing how to begin a comment in the target
   assembler language.  The compiler assumes that the comment will end at
   the end of the line.  */
#define ASM_COMMENT_START "#"

/* Output at beginning of assembler file.  */
/*
__PHX__ clenup
#ifndef ASM_FILE_START
#define ASM_FILE_START(FILE) do {\
fprintf (FILE, "%s file %s\n", ASM_COMMENT_START, main_input_filename);\
fprintf (FILE, ".file\t");   \
  output_quoted_string (FILE, main_input_filename);\
  fputc ('\n', FILE);} while (0)
#endif
*/
/* Output to assembler file text saying following lines
   may contain character constants, extra white space, comments, etc.  */

#define ASM_APP_ON ""

/* Output to assembler file text saying following lines
   no longer contain unusual constructs.  */

#define ASM_APP_OFF ""

/* Switch to the text or data segment.  */

/* Output before read-only data.  */
#define TEXT_SECTION_ASM_OP ".section .text"

/* Output before writable data.  */
#define DATA_SECTION_ASM_OP ".section .data"

/* Output before uninitialized data. */
#define BSS_SECTION_ASM_OP  ".section .bss"

/* How to refer to registers in assembler output.
   This sequence is indexed by compiler's hard-register-number (see above).  */

#define REGISTER_NAMES \
{"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7",   "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15" \
, "r16", "r17", "r18", "r19", "r20", "r21", "r22", "r23", "r24", "r25", "r26", "r27", "r28", "r29", "r30", "r31", "cc-flag"}

/* -------------------------------------------------------------------------- */
/* Debug things for DBX (STABS)                                               */
/*                                                                            */
/* Note. Our config.gcc includes dbxelf.h, which sets up appropriate          */
/*       defaults. Choice of which debug format to use is in our elf.h        */
/* -------------------------------------------------------------------------- */

/* Don't try to use the  type-cross-reference character in DBX data.
   Also has the consequence of putting each struct, union or enum
   into a separate .stabs, containing only cross-refs to the others.  */
/* JPB 24-Aug-10: Is this really correct. Can't GDB use this info? */
#define DBX_NO_XREFS
         
/* -------------------------------------------------------------------------- */
/* Debug things for DWARF2                                                    */
/*                                                                            */
/* Note. Choice of which debug format to use is in our elf.h                  */
/* -------------------------------------------------------------------------- */

/* We support frame unwind info including for exceptions handling. This needs
   INCOMING_RETURN_ADDR_RTX to be set and OBJECT_FORMAT_ELF to be defined (in
   elfos.h). Override any default value. */
#undef  DWARF2_UNWIND_INFO
#define DWARF2_UNWIND_INFO 1

/* We want frame info produced. Note that this is superfluous if
   DWARF2_UNWIND_INFO is non-zero, but we set so this so, we can produce frame
   info even when it is zero. Override any default value. */
#undef  DWARF2_FRAME_INFO
#define DWARF2_FRAME_INFO 1

/* Macro to idenfity where the incoming return address is on a function call
   before the start of the prologue (i.e. the link register). Used to produce
   DWARF2 frame debug info when DWARF2_UNWIND_INFO is non-zero. Override any
   default value. */
#undef  INCOMING_RETURN_ADDR_RTX
#define INCOMING_RETURN_ADDR_RTX gen_rtx_REG (Pmode, LINK_REGNUM)

/* We believe this works for our assembler. Override any default value */
#undef  DWARF2_ASM_LINE_DEBUG_INFO
#define DWARF2_ASM_LINE_DEBUG_INFO 1


/* Node: Label Output */

/* Globalizing directive for a label.  */
#define GLOBAL_ASM_OP "\t.global "

#define SUPPORTS_WEAK 1

/* This is how to output the definition of a user-level label named NAME,
   such as the label on a static function or variable NAME.  */

#define ASM_OUTPUT_LABEL(FILE,NAME)	\
 { assemble_name (FILE, NAME); fputs (":\n", FILE); }

/* The prefix to add to user-visible assembler symbols. */

/* Remove any previous definition (elfos.h).  */
/* We use -fno-leading-underscore to remove it, when necessary.  */
/* JPB: No prefix for global symbols */
#undef  USER_LABEL_PREFIX
#define USER_LABEL_PREFIX ""

/* Remove any previous definition (elfos.h).  */
#ifndef ASM_GENERATE_INTERNAL_LABEL
#define ASM_GENERATE_INTERNAL_LABEL(LABEL, PREFIX, NUM)	\
  sprintf (LABEL, "*%s%d", PREFIX, NUM)
#endif

/* This is how to output an assembler line defining an `int' constant.  */

#define ASM_OUTPUT_INT(FILE,VALUE)  	\
( 					\
	fprintf (FILE, "\t.word "),	\
  output_addr_const (FILE, (VALUE)),	\
  fprintf (FILE, "\n"))

#define ASM_OUTPUT_FLOAT(stream,value) \
   { long l;                                 \
      REAL_VALUE_TO_TARGET_SINGLE(value,l); \
      fprintf(stream,"\t.word 0x%08x\t\n# float %26.7e\n",l,value); }

#define ASM_OUTPUT_DOUBLE(stream,value)                         \
   { long l[2];                                                 \
      REAL_VALUE_TO_TARGET_DOUBLE(value,&l[0]);                 \
      fprintf(stream,"\t.word 0x%08x,0x%08x\t\n# float %26.16le\n",  \
              l[0],l[1],value); }

#define ASM_OUTPUT_LONG_DOUBLE(stream,value) \
   { long l[4];                                 \
      REAL_VALUE_TO_TARGET_DOUBLE(value,&l[0]); \
      fprintf(stream,"\t.word 0x%08x,0x%08x,0x%08x,0x%08x\t\n# float %26.18lle\n", \
              l[0],l[1],l[2],l[3],value); }

/* Likewise for `char' and `short' constants.  */

#define ASM_OUTPUT_SHORT(FILE,VALUE)  \
( fprintf (FILE, "\t.half "),			\
  output_addr_const (FILE, (VALUE)),		\
  fprintf (FILE, "\n"))

#define ASM_OUTPUT_CHAR(FILE,VALUE)  \
( fprintf (FILE, "\t.byte "),			\
  output_addr_const (FILE, (VALUE)),		\
  fprintf (FILE, "\n"))

/* This is how to output an assembler line for a numeric constant byte.  */

#define ASM_OUTPUT_BYTE(FILE,VALUE)  \
  fprintf (FILE, "\t.byte 0x%02x\n", (VALUE))

/* This is how to output an insn to push a register on the stack.
   It need not be very fast code.  */

#define ASM_OUTPUT_REG_PUSH(FILE,REGNO)  \
  fprintf (FILE, "\tl.sub   \tr1,4\n\tl.sw    \t0(r1),%s\n", reg_names[REGNO])

/* This is how to output an insn to pop a register from the stack.
   It need not be very fast code.  */

#define ASM_OUTPUT_REG_POP(FILE,REGNO)  \
  fprintf (FILE, "\tl.lwz   \t%s,0(r1)\n\tl.addi   \tr1,4\n", reg_names[REGNO])

/* This is how to output an element of a case-vector that is absolute.
   (The Vax does not use such vectors,
   but we must define this macro anyway.)  */

#define ASM_OUTPUT_ADDR_VEC_ELT(FILE, VALUE)  \
  fprintf (FILE, "\t.word .L%d\n", VALUE)

/* This is how to output an element of a case-vector that is relative.  */

#define ASM_OUTPUT_ADDR_DIFF_ELT(FILE, BODY, VALUE, REL)  \
  fprintf (FILE, "\t.word .L%d-.L%d\n", VALUE, REL)

/* This is how to output an assembler line
   that says to advance the location counter
   to a multiple of 2**LOG bytes.  */

#define ASM_OUTPUT_ALIGN(FILE,LOG)  \
  if ((LOG) != 0) fprintf (FILE, "\t.align %d\n", 1 << (LOG))

/* This is how to output an assembler line
   that says to advance the location counter by SIZE bytes.  */

#ifndef ASM_OUTPUT_SKIP
#define ASM_OUTPUT_SKIP(FILE,SIZE)  \
  fprintf (FILE, "\t.space %d\n", (SIZE))
#endif

/* Need to split up .ascii directives to avoid breaking
   the linker. */

/* This is how to output a string.  */
#undef  ASM_OUTPUT_ASCII
#define ASM_OUTPUT_ASCII(STREAM, PTR, LEN)  \
  output_ascii_pseudo_op (STREAM, (const unsigned char *) (PTR), LEN)

/* Invoked just before function output. */
#define ASM_OUTPUT_FUNCTION_PREFIX(stream, fnname)              \
  fputs("\t.proc\t",stream); assemble_name(stream,fnname);         \
  fputs("\n",stream);

/* This says how to output an assembler line
   to define a global common symbol.  */
#define ASM_OUTPUT_COMMON(stream,name,size,rounded)             \
{ data_section();                                               \
  fputs(".global\t",stream); assemble_name(stream,name);        \
  fputs("\n",stream); assemble_name(stream,name);               \
  fprintf(stream,":\n\t.space %d\n",rounded); }

/* This says how to output an assembler line
   to define a local common symbol.  */

#define ASM_OUTPUT_LOCAL(FILE, NAME, SIZE, ROUNDED)  \
( fputs (".bss ", (FILE)),			\
  assemble_name ((FILE), (NAME)),		\
  fprintf ((FILE), ",%d,%d\n", (SIZE),(ROUNDED)))

/* This says how to output an assembler line to define a global common symbol
   with size SIZE (in bytes) and alignment ALIGN (in bits).  */
#ifndef ASM_OUTPUT_ALIGNED_COMMON
#define ASM_OUTPUT_ALIGNED_COMMON(FILE, NAME, SIZE, ALIGN)	\
{ data_section();                                           	\
  if ((ALIGN) > 8)                                          	\
	fprintf(FILE, "\t.align %d\n", ((ALIGN) / BITS_PER_UNIT)); \
  fputs(".global\t", FILE); assemble_name(FILE, NAME);      	\
  fputs("\n", FILE);                                        	\
  assemble_name(FILE, NAME);                                	\
  fprintf(FILE, ":\n\t.space %d\n", SIZE);		    	\
}
#endif /* ASM_OUTPUT_ALIGNED_COMMON */
  
/* This says how to output an assembler line to define a local common symbol
   with size SIZE (in bytes) and alignment ALIGN (in bits).  */

#ifndef ASM_OUTPUT_ALIGNED_LOCAL
#define ASM_OUTPUT_ALIGNED_LOCAL(FILE, NAME, SIZE, ALIGN) \
{ data_section();                                               \
  if ((ALIGN) > 8)                                              \
	fprintf(FILE, "\t.align %d\n", ((ALIGN) / BITS_PER_UNIT)); \
  assemble_name(FILE, NAME);                                    \
  fprintf(FILE, ":\n\t.space %d\n", SIZE);			\
}
#endif /* ASM_OUTPUT_ALIGNED_LOCAL */
                                                     
/* Store in OUTPUT a string (made with alloca) containing
   an assembler-name for a local static variable named NAME.
   LABELNO is an integer which is different for each call.  */

#define ASM_FORMAT_PRIVATE_NAME(OUTPUT, NAME, LABELNO)	\
( (OUTPUT) = (char *) alloca (strlen ((NAME)) + 10),	\
  sprintf ((OUTPUT), "%s.%d", (NAME), (LABELNO)))

/* Macro for %code validation. Returns nonzero if valid. */
#define PRINT_OPERAND_PUNCT_VALID_P(code) or32_print_operand_punct_valid_p(code)

/* Print an instruction operand X on file FILE.
   CODE is the code from the %-spec that requested printing this operand;
   if `%z3' was used to print operand 3, then CODE is 'z'.  */

#define PRINT_OPERAND(FILE, X, CODE) or32_print_operand(FILE, X, CODE)

/* Print a memory operand whose address is X, on file FILE.
   This uses a function in output-vax.c.  */

#define PRINT_OPERAND_ADDRESS(FILE, ADDR) or32_print_operand_address (FILE, ADDR)

/* These are stubs, and have yet to bee written. */

#define TRAMPOLINE_SIZE 26
#define TRAMPOLINE_TEMPLATE(FILE)
#define INITIALIZE_TRAMPOLINE(TRAMP,FNADDR,CXT)

extern GTY(()) rtx or32_compare_op0;
extern GTY(()) rtx or32_compare_op1;

#endif /* _OR32_H_ */
