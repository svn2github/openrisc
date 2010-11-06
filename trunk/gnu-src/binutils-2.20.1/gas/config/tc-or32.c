/* Assembly backend for the OpenRISC 1000.
   Copyright (C) 2002, 2003, 2005, 2007, 2009
   Copyright (C) 2010 Embecosm Limited
   Free Software Foundation, Inc.
   Contributed by Damjan Lampret <lampret@opencores.org>.
   Modified bu Johan Rydberg, <johan.rydberg@netinsight.se>.
  	       Gyorgy Jeney <nog@sdf.lonestar.org>
   Based upon a29k port.

   This file is part of GAS, the GNU Assembler.

   GAS is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   GAS is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GAS; see the file COPYING.  If not, write to
   the Free Software Foundation, 51 Franklin Street - Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* tc-a29k.c used as a template.  */

#include "safe-ctype.h"
#include "as.h"
#include "opcode/or32.h"
#include "struc-symbol.h"
#include "elf/or32.h"

#define DEBUG 0

#ifndef REGISTER_PREFIX
#define REGISTER_PREFIX   '%'
#endif

/* Make it easier to clone this machine desc into another one.  */
#define machine_opcode  or32_opcode
#define machine_opcodes or32_opcodes
#define machine_ip      or32_ip
#define machine_it      or32_it

/* Handle of the OPCODE hash table.  */
static struct hash_control *op_hash = NULL;

struct machine_it
{
  char *          error;
  unsigned long   opcode;
  struct nlist *  nlistp;
  expressionS     exp;
  int             pcrel;
  int             reloc;
}
the_insn;

const pseudo_typeS md_pseudo_table[] =
{
  {"align",   s_align_bytes,  4 },
  {"space",   s_space,        0 },
  {"cputype", s_ignore,       0 },
  {"reg",     s_lsym,         0 },  /* Register equate, same as equ.  */
  {"sect",    s_ignore,       0 },  /* Creation of coff sections.  */
  {"proc",    s_ignore,       0 },  /* Start of a function.  */
  {"endproc", s_ignore,       0 },  /* Function end.  */
  {"half",    cons,           2 },
  {"word",    cons,           4 },
  {NULL,      0,              0 },
};

int md_short_jump_size  = 4;
int md_long_jump_size   = 4;

/* This array holds the chars that always start a comment.
   If the pre-processor is disabled, these aren't very useful.  */
const char comment_chars[] = "#";

/* This array holds the chars that only start a comment at the beginning of
   a line.  If the line seems to have the form '# 123 filename'
   .line and .file directives will appear in the pre-processed output.  */
/* Note that input_file.c hand checks for '#' at the beginning of the
   first line of the input file.  This is because the compiler outputs
   #NO_APP at the beginning of its output.  */
/* Also note that comments like this one will always work.  */
const char line_comment_chars[] = "#";

/* We needed an unused char for line separation to work around the
   lack of macros, using sed and such.  */
const char line_separator_chars[] = ";";

/* Chars that can be used to separate mant from exp in floating point nums.  */
const char EXP_CHARS[] = "eE";

/* Chars that mean this number is a floating point constant.
   As in 0f12.456
   or    0d1.2345e12.  */
const char FLT_CHARS[] = "rRsSfFdDxXpP";

/* "l.jalr r9" precalculated opcode.  */
static unsigned long jalr_r9_opcode;

static void machine_ip (char *);


/* Set bits in machine opcode according to insn->encoding
   description and passed operand.  */

static void
encode (const struct machine_opcode *insn,
	unsigned long *opcode,
	signed long param_val,
	char param_ch)
{
  int opc_pos = 0;
  int param_pos = 0;
  char *enc;

#if DEBUG
  printf ("    encode:  opcode=%.8x  param_val=%.8x abs=%.8x param_ch=%c\n",
	  (unsigned int) *opcode,(unsigned int) param_val, abs (param_val), param_ch);
#endif
  for (enc = insn->encoding; *enc != '\0'; enc++)
    if (*enc == param_ch)
      {
	if (enc - 2 >= insn->encoding && (*(enc - 2) == '0') && (*(enc - 1) == 'x'))
	  continue;
	else
	  param_pos ++;
      }

  opc_pos = 32;

  for (enc = insn->encoding; *enc != '\0';)
    {
      if ((*enc == '0') && (*(enc + 1) == 'x'))
	{
	  int tmp = strtol (enc, NULL, 16);

	  opc_pos -= 4;
	  *opcode |= tmp << opc_pos;
	  enc += 3;
	}
      else if ((*enc == '0') || (*enc == '-'))
	{
	  opc_pos--;
	  enc++;
	}
      else if (*enc == '1')
	{
	  opc_pos--;
	  *opcode |= 1 << opc_pos;
	  enc++;
	}
      else if (*enc == param_ch)
	{
	  opc_pos--;
	  param_pos--;
	  *opcode |= ((param_val >> param_pos) & 0x1) << opc_pos;
	  enc++;
	}
      else if (ISALPHA (*enc))
	{
	  opc_pos--;
	  enc++;
	}
      else
	enc++;
    }

#if DEBUG
  printf ("    opcode=%.8lx\n", *opcode);
#endif
}

/* This function is called once, at assembler startup time.  It should
   set up all the tables, etc., that the MD part of the assembler will
   need.  */

void
md_begin (void)
{
  const char *retval = NULL;
  int lose = 0;
  int skipnext = 0;
  unsigned int i;

  /* Hash up all the opcodes for fast use later.  */
  op_hash = hash_new ();

  for (i = 0; i < or32_num_opcodes; i++)
    {
      const char *name = machine_opcodes[i].name;

      if (skipnext)
        {
          skipnext = 0;
          continue;
        }

      retval = hash_insert (op_hash, name, (void *) &machine_opcodes[i]);
      if (retval != NULL)
        {
          fprintf (stderr, "internal error: can't hash `%s': %s\n",
                   machine_opcodes[i].name, retval);
          lose = 1;
        }
    }

  if (lose)
    as_fatal (_("Broken assembler.  No assembly attempted."));

  encode (&machine_opcodes[insn_index ("l.jalr")], &jalr_r9_opcode, 9, 'B');
}

/* Returns non zero if instruction is to be used.  */

static int
check_invalid_opcode (unsigned long opcode)
{
  return opcode == jalr_r9_opcode;
}

/* Assemble a single instruction.  Its label has already been handled
   by the generic front end.  We just parse opcode and operands, and
   produce the bytes of data and relocation.  */

void
md_assemble (char *str)
{
  char *toP;

#if DEBUG
  printf ("NEW INSTRUCTION\n");
#endif

  know (str);
  machine_ip (str);
  toP = frag_more (4);

  /* Put out the opcode.  */
  md_number_to_chars (toP, the_insn.opcode, 4);

  /* Put out the symbol-dependent stuff.  */
  if (the_insn.reloc != BFD_RELOC_NONE)
    {
      fix_new_exp (frag_now,
                   (toP - frag_now->fr_literal),
                   4,   /* size */
                   &the_insn.exp,
                   the_insn.pcrel,
                   the_insn.reloc);
    }
}

static int mask_or_shift = 0;

/*----------------------------------------------------------------------------*/
/*!Parse an operand

   We use some semantic information about the operand to determine its type.

   @param[in]  s         Pointer to start of the operand string
   @param[out] operandp  The parsed operand expression
   @param[in]  opt       Non-zero (TRUE) if this operand is optional, zero
                         (FALSE) otherwise.
   @param[in]  is_reg    Non-zero (TRUE) if this operand is a register, zero
                         (FALSE) otherwise.

   @return  Pointer to the operand string immediately after the operand just
            parsed.                                                           */
/*----------------------------------------------------------------------------*/
static char *
parse_operand (char        *s,
	       expressionS *operandp,
	       int          opt,
	       int          is_reg)
{
  char *save = input_line_pointer;
  char *new_pointer;

#if DEBUG
  printf ("  PROCESS NEW OPERAND(%s): %s, %s\n", s,
	  opt ? "optional" : "not optional",
	  is_reg ? "register", "not register");
#endif

  input_line_pointer = s;

  if (strncasecmp (s, "HI(", 3) == 0)
    {
      mask_or_shift = BFD_RELOC_HI16;

      input_line_pointer += 3;
    }
  else if (strncasecmp (s, "LO(", 3) == 0)
    {
      mask_or_shift = BFD_RELOC_LO16;

      input_line_pointer += 3;
    }
  else
    mask_or_shift = 0;

  if ((*s == '(') && (*(s+1) == 'r'))
    s++;

  /* JPB 18-Aug-10: The old assumption was that any symbol starting with 'r'
     and followed by a digit was a register. That was fine when we prepended
     underscores, but not when that was dropped. We need to makes sure that
     this is a register operand. */
  if (is_reg)
    {
      if ((*s == 'r') && ISDIGIT (*(s + 1)))
	{
	  operandp->X_add_number = strtol (s + 1, NULL, 10);
	  operandp->X_op = O_register;
	  operandp->X_add_symbol = NULL;// Added to stop know() in machine_ip()
	  operandp->X_op_symbol = NULL; // Added to stop know() in machine_ip()
                                        // erroring out - it appears this
                                        // wasn't getting cleared sometimes. -
                                        // JB 100718
	}
      else
	{
	  as_bad (_("register expected"));
	}

      /* Skip the argument */
      for (; (*s != ',') && (*s != '\0');)
	s++;
      input_line_pointer = save;
      return s;
    }

  expression (operandp);

  if (operandp->X_op == O_absent)
    {
      if (! opt)
	as_bad (_("missing operand"));
      else
	{
	  operandp->X_add_number = 0;
	  operandp->X_op = O_constant;
	}
    }

  new_pointer = input_line_pointer;
  input_line_pointer = save;

#if DEBUG
  printf ("  %s=parse_operand(%s): operandp->X_op = %u\n", new_pointer, s,
          operandp->X_op);
#endif

  return new_pointer;
}

/* Instruction parsing.  Takes a string containing the opcode.
   Operands are at input_line_pointer.  Output is in the_insn.
   Warnings or errors are generated.  */

static void
machine_ip (char *str)
{
  char *s;
  const char *args;
  const struct machine_opcode *insn;
  unsigned long opcode;
  expressionS operand;
  int reloc = BFD_RELOC_NONE;

#if DEBUG
  printf ("machine_ip(%s)\n", str);
#endif

  s = str;
  for (; ISALNUM (*s) || *s == '.' || *s == '_'; ++s)
    if (ISUPPER (*s))
      *s = TOLOWER (*s);

  switch (*s)
    {
    case '\0':
      break;

    case ' ':     /* FIXME-SOMEDAY more whitespace.  */
      *s++ = '\0';
      break;

    default:
      as_bad (_("unknown opcode1: `%s'"), str);
      return;
    }

  if ((insn = (struct machine_opcode *) hash_find (op_hash, str)) == NULL)
    {
      as_bad (_("unknown opcode2 `%s'."), str);
      return;
    }

  opcode = 0;
  memset (&the_insn, '\0', sizeof (the_insn));
  the_insn.reloc = BFD_RELOC_NONE;

  reloc = BFD_RELOC_NONE;

  /* Build the opcode, checking as we go to make sure that the
     operands match.

     If an operand matches, we modify the_insn or opcode appropriately,
     and do a "continue".  If an operand fails to match, we "break".  */
  if (insn->args[0] != '\0')
    /* Prime the pump.  */
    s = parse_operand (s,
                       &operand,
                       (insn->args[0] == 'I') ||
                       (strcmp(insn->name, "l.nop") == 0),
		       'r' == insn->args[0]);

  for (args = insn->args;; ++args)
    {
#if DEBUG
      printf ("  args = %s\n", args);
#endif
      switch (*args)
        {
        case '\0':    /* End of args.  */
          /* We have have 0 args, do the bazoooka!  */
          if (args == insn->args)
	    encode (insn, &opcode, 0, 0);

          if (*s == '\0')
            {
              /* We are truly done.  */
              the_insn.opcode = opcode;
              if (check_invalid_opcode (opcode))
                as_bad (_("instruction not allowed: %s"), str);
              return;
            }
          as_bad (_("too many operands: %s"), s);
          break;

        case ',':   /* Must match a comma.  */
          if (*s++ == ',')
            {
              reloc = BFD_RELOC_NONE;

              /* Parse next operand.  */
              s = parse_operand (s, &operand, args[1] == 'I', 'r' == args[1]);
#if DEBUG
	      printf ("    ',' case: operand->X_add_number = %d, *args = %s, *s = %s\n",
		      (int) operand.X_add_number, args, s);
#endif
              continue;
            }
          break;

        case '(':   /* Must match a (.  */
          s = parse_operand (s, &operand, args[1] == 'I', 'r' == args[1]);
          continue;

        case ')':   /* Must match a ).  */
          continue;

        case 'r':   /* A general register.  */
          args++;

          if (operand.X_op != O_register)
            break;    /* Only registers.  */
#if DEBUG
	  printf("   (operand.X_add_symbol == 0)=%d",
		 (operand.X_add_symbol == NULL));
	  printf("   (operand.X_op_symbol(%x) == 0)=%d", 
		 (unsigned int)operand.X_op_symbol,
		 (operand.X_op_symbol == NULL));
#endif	  
          know (operand.X_add_symbol == NULL);
          know (operand.X_op_symbol == NULL);
          encode (insn, &opcode, operand.X_add_number, *args);
#if DEBUG
          printf ("    r: operand->X_op = %d\n", operand.X_op);
#endif
          continue;

        default:
          if (mask_or_shift)
	    {
#if DEBUG
	      printf ("mask_or_shift = %d\n", mask_or_shift);
#endif
	      reloc = mask_or_shift;
	    }
          mask_or_shift = 0;
          if (*s == '(')
            operand.X_op = O_constant;
          else if (*s == ')')
            s += 1;
#if DEBUG
          printf ("    default case: operand->X_add_number = %d, *args = %s, *s = %s\n",(int) operand.X_add_number, args, s);
#endif
          if (operand.X_op == O_constant)
            {
            unsigned long add = operand.X_add_number;
	      if (reloc == BFD_RELOC_NONE)
		{
		  bfd_vma v, mask;

		  mask = 0x3ffffff;
		  v = abs (operand.X_add_number) & ~ mask;
		  if (v)
		    as_bad (_("call/jmp target out of range (1)"));
		}

              if (reloc == BFD_RELOC_HI16)
                add = ((operand.X_add_number >> 16) & 0xffff);

              the_insn.pcrel = 0;
              encode (insn, &opcode, add, *args);
              continue;
            }

          if (reloc == BFD_RELOC_NONE)
            the_insn.reloc = BFD_RELOC_28_PCREL_S2;
          else
            the_insn.reloc = reloc;

          /* the_insn.reloc = insn->reloc;  */
#if DEBUG
          printf ("    reloc sym=%d\n", the_insn.reloc);
          printf ("    BFD_RELOC_NONE=%d\n", BFD_RELOC_NONE);
#endif
          the_insn.exp = operand;

          /*  the_insn.reloc_offset = 1;  */
          the_insn.pcrel = 1; /* Assume PC-relative jump.  */

          /* FIXME-SOON, Do we figure out whether abs later, after
             know sym val?  */
          if (reloc == BFD_RELOC_LO16 || reloc == BFD_RELOC_HI16)
            the_insn.pcrel = 0;

          if (reloc == BFD_RELOC_NONE)
            encode (insn, &opcode, operand.X_add_number, *args);
          else
            encode (insn, &opcode, 0, *args);
          continue;
        }

      /* Types or values of args don't match.  */
      as_bad (_("invalid operands"));
      return;
    }
}

char *
md_atof (int type, char * litP, int *  sizeP)
{
  return ieee_md_atof (type, litP, sizeP, TRUE);
}

/* Write out big-endian.  */

void
md_number_to_chars (char *buf, valueT val, int n)
{
  number_to_chars_bigendian (buf, val, n);
}

void
md_apply_fix (fixS * fixP, valueT * val, segT seg ATTRIBUTE_UNUSED)
{
  bfd_byte *buf;
  long insn;

  buf = (bfd_byte *) (fixP->fx_where + fixP->fx_frag->fr_literal);

#if DEBUG
  printf ("md_apply_fix *val=%x fixP->fx_r_type=%i sym=%s\n",(unsigned int)*val,
	   fixP->fx_r_type, fixP->fx_addsy ? fixP->fx_addsy->bsym->name : "(none)");
#endif

  if ((fixP->fx_addsy == (symbolS *) NULL) && !fixP->fx_pcrel)
    fixP->fx_done = 1;

  switch (fixP->fx_r_type)
    {
    case BFD_RELOC_32:      /* XXXXXXXX pattern in a word.  */
#if DEBUG
      printf ("reloc_const: val=%x\n", (unsigned int) *val);
#endif
      /* If we are deleting this reloc entry, we must fill in the
	       value now.  This can happen if we have a .word which is not
	       resolved when it appears but is later defined.  We also need
	       to fill in the value if this is an embedded PIC switch table
	       entry.  */
      if (fixP->fx_done)
        md_number_to_chars ((char *) buf, *val, 4);
      break;

    case BFD_RELOC_16:      /* XXXX0000 pattern in a word.  */
#if DEBUG
      printf ("reloc_const: val=%x\n", (unsigned int)*val);
#endif
      /* If we are deleting this reloc entry, we must fill in the
         value now.  */
      gas_assert (fixP->fx_size == 2);
      if (fixP->fx_done)
	      md_number_to_chars ((char *) buf, *val, 2);
      break;

    case BFD_RELOC_8:      /* XX000000 pattern in a word.  */
    case BFD_RELOC_LO16:      /* 0000XXXX pattern in a word.  */
#if DEBUG
      printf ("reloc_const: val=%x\n", (unsigned int)*val);
#endif
      break;

    case BFD_RELOC_HI16:    /* 0000XXXX pattern in a word.  */
#if DEBUG
      printf ("reloc_consth: val=%x\n", (unsigned int)*val);
#endif
      break;

    case BFD_RELOC_28_PCREL_S2:  /* 0000XXXX pattern in a word.  */
#if DEBUG
      printf("reloc_pcrel: *val=%x done=%d fixP->fx_pcrel=%i line=%i\n",
	     (unsigned int) *val, fixP->fx_done, fixP->fx_pcrel, fixP->fx_line);
#endif
      if ((*val & 0x3) != 0)
        as_bad_where (fixP->fx_file, fixP->fx_line,
                      _("Branch to odd address (%lx)"), (long) *val);

      /*
       * We need to save the bits in the instruction since fixup_segment()
       * might be deleting the relocation entry (i.e., a branch within
       * the current segment).
       */
      if (! fixP->fx_done)
        break;

      /* update old instruction data */
      if (target_big_endian)
        insn = (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];
      else
        insn = (buf[3] << 24) | (buf[2] << 16) | (buf[1] << 8) | buf[0];
      insn |= (*val >> 2) & 0x03ffffff;
      md_number_to_chars ((char *) buf, insn, 4);
#if DEBUG
      printf("Resulting instruction: %08x\n", (unsigned int)insn);
#endif
      break;

    case BFD_RELOC_VTABLE_INHERIT:
    case BFD_RELOC_VTABLE_ENTRY:
      fixP->fx_done = 0;
      break;

    case BFD_RELOC_NONE:
    default:
      as_bad (_("bad relocation type: 0x%02x"), fixP->fx_r_type);
      break;
    }

  fixP->fx_addnumber = *val; /* Remember value for emit_reloc.  */
}

/* Should never be called for or32.  */

void
md_create_short_jump (char *    ptr       ATTRIBUTE_UNUSED,
		      addressT  from_addr ATTRIBUTE_UNUSED,
		      addressT  to_addr   ATTRIBUTE_UNUSED,
		      fragS *   frag      ATTRIBUTE_UNUSED,
		      symbolS * to_symbol ATTRIBUTE_UNUSED)
{
  as_fatal ("or32_create_short_jmp\n");
}

/* Should never be called for or32.  */

void
md_convert_frag (bfd *   headers ATTRIBUTE_UNUSED,
		 segT    seg     ATTRIBUTE_UNUSED,
		 fragS * fragP   ATTRIBUTE_UNUSED)
{
  as_fatal ("or32_convert_frag\n");
}

/* Should never be called for or32.  */

void
md_create_long_jump (char *    ptr       ATTRIBUTE_UNUSED,
		     addressT  from_addr ATTRIBUTE_UNUSED,
		     addressT  to_addr   ATTRIBUTE_UNUSED,
		     fragS *   frag      ATTRIBUTE_UNUSED,
		     symbolS * to_symbol ATTRIBUTE_UNUSED)
{
  as_fatal ("or32_create_long_jump\n");
}

/* Should never be called for or32.  */

int
md_estimate_size_before_relax (fragS * fragP   ATTRIBUTE_UNUSED,
			       segT    segtype ATTRIBUTE_UNUSED)
{
  as_fatal ("or32_estimate_size_before_relax\n");
  return 0;
}

/* Translate internal representation of relocation info to target format.

   On sparc/29k: first 4 bytes are normal unsigned long address, next three
   bytes are index, most sig. byte first.  Byte 7 is broken up with
   bit 7 as external, bits 6 & 5 unused, and the lower
   five bits as relocation type.  Next 4 bytes are long addend.  */
/* Thanx and a tip of the hat to Michael Bloom, mb@ttidca.tti.com.  */

#ifdef OBJ_AOUT
void
tc_aout_fix_to_chars (char *where,
		      fixS *fixP,
		      relax_addressT segment_address_in_file)
{
  long r_symbolnum;

#if DEBUG
  printf ("tc_aout_fix_to_chars\n");
#endif

  know (fixP->fx_r_type < BFD_RELOC_NONE);
  know (fixP->fx_addsy != NULL);

  md_number_to_chars
    (where,
     fixP->fx_frag->fr_address + fixP->fx_where - segment_address_in_file,
     4);

  r_symbolnum = (S_IS_DEFINED (fixP->fx_addsy)
     ? S_GET_TYPE (fixP->fx_addsy)
     : fixP->fx_addsy->sy_number);

  where[4] = (r_symbolnum >> 16) & 0x0ff;
  where[5] = (r_symbolnum >> 8) & 0x0ff;
  where[6] = r_symbolnum & 0x0ff;
  where[7] = (((!S_IS_DEFINED (fixP->fx_addsy)) << 7) & 0x80) | (0 & 0x60) | (fixP->fx_r_type & 0x1F);

  /* Also easy.  */
  md_number_to_chars (&where[8], fixP->fx_addnumber, 4);
}

#endif /* OBJ_AOUT */

const char *md_shortopts = "";

struct option md_longopts[] =
{
  { NULL, no_argument, NULL, 0 }
};
size_t md_longopts_size = sizeof (md_longopts);

int
md_parse_option (int c ATTRIBUTE_UNUSED, char * arg ATTRIBUTE_UNUSED)
{
  return 0;
}

void
md_show_usage (FILE * stream ATTRIBUTE_UNUSED)
{
}

/* This is called when a line is unrecognized.  This is used to handle
   definitions of or32 style local labels.  */

int
or32_unrecognized_line (int c)
{
  int lab;
  char *s;

  if (c != '$'
      || ! ISDIGIT ((unsigned char) input_line_pointer[0]))
    return 0;

  s = input_line_pointer;

  lab = 0;
  while (ISDIGIT ((unsigned char) *s))
    {
      lab = lab * 10 + *s - '0';
      ++s;
    }

  if (*s != ':')
    /* Not a label definition.  */
    return 0;

  if (dollar_label_defined (lab))
    {
      as_bad (_("label \"$%d\" redefined"), lab);
      return 0;
    }

  define_dollar_label (lab);
  colon (dollar_label_name (lab, 0));
  input_line_pointer = s + 1;

  return 1;
}

/* Default the values of symbols known that should be "predefined".  We
   don't bother to predefine them unless you actually use one, since there
   are a lot of them.  */

symbolS *
md_undefined_symbol (char *name ATTRIBUTE_UNUSED)
{
  return NULL;
}

/* Parse an operand that is machine-specific.  */

void
md_operand (expressionS *expressionP)
{
#if DEBUG
  printf ("  md_operand(input_line_pointer = %s)\n", input_line_pointer);
#endif

  if (input_line_pointer[0] == REGISTER_PREFIX && input_line_pointer[1] == 'r')
    {
      /* We have a numeric register expression.  No biggy.  */
      input_line_pointer += 2;  /* Skip %r */
      (void) expression (expressionP);

      if (expressionP->X_op != O_constant
          || expressionP->X_add_number > 255)
        as_bad (_("Invalid expression after %%%%\n"));
      expressionP->X_op = O_register;
    }
  else if (input_line_pointer[0] == '&')
    {
      /* We are taking the 'address' of a register...this one is not
         in the manual, but it *is* in traps/fpsymbol.h!  What they
         seem to want is the register number, as an absolute number.  */
      input_line_pointer++; /* Skip & */
      (void) expression (expressionP);

      if (expressionP->X_op != O_register)
        as_bad (_("invalid register in & expression"));
      else
        expressionP->X_op = O_constant;
    }
  else if (input_line_pointer[0] == '$'
           && ISDIGIT ((unsigned char) input_line_pointer[1]))
    {
      long lab;
      char *name;
      symbolS *sym;

      /* This is a local label.  */
      ++input_line_pointer;
      lab = (long) get_absolute_expression ();

      if (dollar_label_defined (lab))
        {
          name = dollar_label_name (lab, 0);
          sym = symbol_find (name);
        }
      else
        {
          name = dollar_label_name (lab, 1);
          sym = symbol_find_or_make (name);
        }

      expressionP->X_op = O_symbol;
      expressionP->X_add_symbol = sym;
      expressionP->X_add_number = 0;
    }
  else if (input_line_pointer[0] == '$')
    {
      char *s;
      char type;
      int fieldnum, fieldlimit;
      LITTLENUM_TYPE floatbuf[8];

      /* $float(), $doubleN(), or $extendN() convert floating values
         to integers.  */
      s = input_line_pointer;

      ++s;

      fieldnum = 0;
      if (strncmp (s, "double", sizeof "double" - 1) == 0)
        {
          s += sizeof "double" - 1;
          type = 'd';
          fieldlimit = 2;
        }
      else if (strncmp (s, "float", sizeof "float" - 1) == 0)
        {
          s += sizeof "float" - 1;
          type = 'f';
          fieldlimit = 1;
        }
      else if (strncmp (s, "extend", sizeof "extend" - 1) == 0)
        {
          s += sizeof "extend" - 1;
          type = 'x';
          fieldlimit = 4;
        }
      else
	return;

      if (ISDIGIT (*s))
        {
          fieldnum = *s - '0';
          ++s;
        }
      if (fieldnum >= fieldlimit)
        return;

      SKIP_WHITESPACE ();
      if (*s != '(')
        return;
      ++s;
      SKIP_WHITESPACE ();

      s = atof_ieee (s, type, floatbuf);
      if (s == NULL)
        return;
      s = s;

      SKIP_WHITESPACE ();
      if (*s != ')')
        return;
      ++s;
      SKIP_WHITESPACE ();

      input_line_pointer = s;
      expressionP->X_op = O_constant;
      expressionP->X_unsigned = 1;
      expressionP->X_add_number = ((floatbuf[fieldnum * 2]
                                    << LITTLENUM_NUMBER_OF_BITS)
                                   + floatbuf[fieldnum * 2 + 1]);
    }
}

/* Round up a section size to the appropriate boundary.  */

valueT
md_section_align (asection * seg, valueT addr ATTRIBUTE_UNUSED)
{
  int align = bfd_get_section_alignment (stdoutput, seg);
  return ((addr + (1 << align) - 1) & (-1 << align));
}

/* Exactly what point is a PC-relative offset relative TO?
   On the 29000, they're relative to the address of the instruction,
   which we have set up as the address of the fixup too.  */

long
md_pcrel_from_section (fixS *fixP, segT sec)
{
#ifndef USE_REL
  if (TC_FORCE_RELOCATION (fixP)
      || (fixP->fx_addsy != (symbolS *) NULL
	  && S_GET_SEGMENT (fixP->fx_addsy) != sec))
    {
      /* If we can't adjust this relocation, or if it references a
	 local symbol in a different section (which
	 TC_FORCE_RELOCATION can't check), let the linker figure it
	 out.  */
      return 0;
    }
#endif

  return fixP->fx_where + fixP->fx_frag->fr_address;
}

/* Generate a reloc for a fixup.  */

arelent *
tc_gen_reloc (asection *seg ATTRIBUTE_UNUSED, fixS *fixp)
{
  arelent *reloc;

  reloc = xmalloc (sizeof (arelent));
  reloc->sym_ptr_ptr = xmalloc (sizeof (asymbol *));
  *reloc->sym_ptr_ptr = symbol_get_bfdsym (fixp->fx_addsy);
  reloc->address = fixp->fx_frag->fr_address + fixp->fx_where;
  reloc->howto = bfd_reloc_type_lookup (stdoutput, fixp->fx_r_type);

  if (reloc->howto == (reloc_howto_type *) NULL)
    {
      as_bad_where (fixp->fx_file, fixp->fx_line,
		    _("reloc %d not supported by object file format"),
		    (int) fixp->fx_r_type);
      return NULL;
    }

  if (fixp->fx_r_type == BFD_RELOC_VTABLE_ENTRY)
    reloc->address = fixp->fx_offset;
  else if (fixp->fx_pcrel)
  {
    bfd_vma pcrel_address;
    pcrel_address = reloc->address;
    if (OUTPUT_FLAVOR == bfd_target_elf_flavour)
    {
      /* At this point, fx_addnumber is "symbol offset - pcrel_address".
	       Relocations want only the symbol offset.  */
      reloc->addend = fixp->fx_addnumber;
    }
    else
    {
      if (OUTPUT_FLAVOR != bfd_target_aout_flavour)
        /* A gruesome hack which is a result of the gruesome gas reloc
           handling.  */
        reloc->addend = pcrel_address;
      else
        reloc->addend = -pcrel_address;
    }
  }
  else
    reloc->addend = fixp->fx_addnumber;
  return reloc;
}
