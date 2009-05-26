;; Predicate definitions for OR32

(define_predicate "cc_reg_operand"
  (match_code "subreg,reg")
{
  register_operand (op, mode);

  if (GET_CODE (op) == REG && REGNO (op) == 32)
    return 1;

  return 0;
})

(define_predicate "sibcall_insn_operand"
  (match_code "subreg,reg,symbol_ref")
{
  /* Disallow indirect through a virtual register.  This leads to
     compiler aborts when trying to eliminate them.  */
  if (GET_CODE (op) == REG
      && (op == arg_pointer_rtx
	  || op == frame_pointer_rtx
	  || (REGNO (op) >= FIRST_PSEUDO_REGISTER
	      && REGNO (op) <= LAST_VIRTUAL_REGISTER)))
    {

      fprintf (stderr, "0\n");
      return 0;
    }

  /* Explicitly allow SYMBOL_REF even if pic.  */
  if (GET_CODE (op) == SYMBOL_REF)
    return 1;

  /* Otherwise we can only allow register operands.  */
  return register_operand (op, Pmode);
})

(define_predicate "input_operand"
  (match_code "subreg,reg,const_int,mem,const")
{
  /* If both modes are non-void they must be the same.  */
  if (mode != VOIDmode && GET_MODE (op) != VOIDmode && mode != GET_MODE (op))
    return 0;

  /* Allow any one instruction integer constant, and all CONST_INT
     variants when we are working in DImode and !arch64.  */
  if (GET_MODE_CLASS (mode) == MODE_INT
      && ((GET_CODE (op) == CONST_INT)
	  && (CONST_OK_FOR_LETTER_P (INTVAL (op), 'K')
	      || CONST_OK_FOR_LETTER_P (INTVAL (op), 'M')
	      || CONST_OK_FOR_LETTER_P (INTVAL (op), 'I'))))
    return 1;

  if (register_operand (op, mode))
    return 1;

  /* If this is a SUBREG, look inside so that we handle
     paradoxical ones.  */
  if (GET_CODE (op) == SUBREG)
    op = SUBREG_REG (op);


  /* Check for valid MEM forms.  */
  if (GET_CODE (op) == MEM)
    return memory_address_p (mode, XEXP (op, 0));

  return 0;
})

(define_predicate "sym_ref_mem_operand"
  (match_code "mem")
{
  if (GET_CODE (op) == MEM)
    {
      rtx t1 = XEXP (op, 0);
      if (GET_CODE (t1) == SYMBOL_REF)
	return 1;
    }
  return 0;
})
