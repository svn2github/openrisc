/* Definitions of target machine for GNU compiler, OR32 cpu. */

#ifndef GCC_OR32_PROTOS_H
#define GCC_OR32_PROTOS_H

/* The following are only needed when handling the machine definition. */

#ifdef RTX_CODE
extern void        or32_expand_prologue (void);
extern void        or32_expand_epilogue (int sibcall);
extern const char *or32_output_move_double (rtx *operands);
extern void        or32_expand_conditional_branch (rtx               *operands,
						   enum machine_mode  mode);
extern int         or32_emit_cmove (rtx  dest,
				    rtx  op,
				    rtx  true_cond,
				    rtx  false_cond);
extern const char *or32_output_bf (rtx * operands);
extern const char *or32_output_cmov (rtx * operands);
extern void        or32_expand_sibcall (rtx  result ATTRIBUTE_UNUSED,
					rtx  addr,
					rtx  args_size);
extern void        or32_emit_set_const32 (rtx  op0,
					  rtx  op1);
#endif

#endif
