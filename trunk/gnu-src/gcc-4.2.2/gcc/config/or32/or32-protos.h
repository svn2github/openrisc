/* Definitions of target machine for GNU compiler, OR32 cpu. */

#ifndef GCC_OR32_PROTOS_H
#define GCC_OR32_PROTOS_H

#include "rtl.h"

extern enum machine_mode  or32_cc_mode (enum rtx_code, rtx, rtx);
extern int                or32_branch_cost (void);
extern void               or32_expand_prologue (void);
extern void               or32_expand_epilogue (int);
extern void               or32_expand_sibcall (rtx, rtx, rtx);
extern void               or32_emit_set_const32 (rtx, rtx);
extern int                or32_emit_cmove (rtx, rtx, rtx, rtx);
extern const char *       or32_output_cmov (rtx *);
extern const char *       or32_output_bf (rtx *);
extern void               or32_print_operand (FILE *, rtx, int);
extern void               or32_print_operand_address (FILE *, register rtx);
extern const char *       or32_output_move_double (rtx * operands);
extern int                or32_register_move_cost (enum machine_mode, enum reg_class, enum reg_class);
extern int                or32_memory_move_cost (enum machine_mode, enum reg_class, int);
extern enum rtx_code      or32_reverse_condition (enum machine_mode, enum rtx_code);
extern void               or32_expand_branch (enum rtx_code code, rtx label);
extern int                or32_print_operand_punct_valid_p (int);

#endif

