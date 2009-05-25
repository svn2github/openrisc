/* Instruction printing code for the OpenRISC 1000

   Copyright (C) 2002, 2005, 2007 Free Software Foundation, Inc.
   Copyright (C) 2008 Embecosm Limited

   Contributed by Damjan Lampret <lampret@opencores.org>.
   Modified from a29k port.
   Contributor Jeremy Bennett <jeremy.bennett@embecosm.com>

   This file is part of the GNU opcodes library.

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
   This version for the OpenRISC 1000 architecture is an update by Jeremy
   Bennett for GDB 6.8. All code converted to ANSI C style and in general to
   GDB format.

   Signed immediate constants are printed as such (the old version printed
   them as unsigned hex). Where appropriate symbolic names are used for
   immediate branch and jump targets.
   --------------------------------------------------------------------------*/


#include "dis-asm.h"
#include "opcode/or32.h"
#include "safe-ctype.h"
#include <string.h>
#include <stdlib.h>


#define EXTEND29(x) ((x) & (unsigned long) 0x10000000 ? ((x) | (unsigned long) 0xf0000000) : ((x)))

/* A cop out - this is in defs.h, but including that from the main GDB
   component would be a pain. Really the dependency on CORE_ADDR ought to be
   added to the or1k-tdep.c, rather than being here. Something for the
   future. */
typedef bfd_vma CORE_ADDR;


/* Function to look up an address as a symbol. Found in the GDB area */
extern int
build_address_symbolic (CORE_ADDR   addr,
			int         do_demangle,
			char      **name,
			int        *offset,
			char      **filename,
			int        *line,
			int        *unmapped);


/*!----------------------------------------------------------------------------
   Map 4 bytes of instruction into one 32-bit word for big-endian target

   @param[in]  insn_ch  The 4 bytes of the instruction
   @param[out] insn     The 32-bit word
   --------------------------------------------------------------------------*/

static void
find_bytes_big (unsigned char *insn_ch,
		unsigned long *insn)
{
  *insn =
    ((unsigned long) insn_ch[0] << 24) +
    ((unsigned long) insn_ch[1] << 16) +
    ((unsigned long) insn_ch[2] << 8) +
    ((unsigned long) insn_ch[3]);

}	/* find_bytes_big() */


/*!----------------------------------------------------------------------------
   Map 4 bytes of instruction into one 32-bit word for little-endian target

   @param[in]  insn_ch  The 4 bytes of the instruction
   @param[out] insn     The 32-bit word
   --------------------------------------------------------------------------*/

static void
find_bytes_little (unsigned char *insn_ch,
		   unsigned long *insn)
{
  *insn =
    ((unsigned long) insn_ch[3] << 24) +
    ((unsigned long) insn_ch[2] << 16) +
    ((unsigned long) insn_ch[1] << 8) +
    ((unsigned long) insn_ch[0]);

}	/* find_bytes_little() */


/*!----------------------------------------------------------------------------
   Extract an instruction field

   @param[in] param_ch     The character identifying the field
   @param[in] enc_initial  A string with 1 char for each bit in the
                           instruction, the char indicating the field the bit
                           belongs to
   @param[in] insn         The instruction

   @return  The value of the field
   --------------------------------------------------------------------------*/

static unsigned long
or32_extract (char               param_ch,
	      char              *enc_initial,
	      unsigned long int  insn)
{
  char              *enc;
  unsigned long int  ret       = 0;
  int                opc_pos   = 0;
  int                param_pos = 0;

  for (enc = enc_initial; *enc != '\0'; enc++)
    {
      if (*enc == param_ch)
	{
	  if ((enc - 2    >= enc_initial) &&
	      (*(enc - 2) == '0')         &&
	      (*(enc - 1) == 'x'))
	    {
	      continue;
	    }
	  else
	    {
	      param_pos++;
	    }
	}
    }

  opc_pos = 32;

  for (enc = enc_initial; *enc != '\0'; )
    {
      if ((*enc == '0') && (*(enc + 1) == 'x'))
	{
	  opc_pos -= 4;

	  if ((param_ch == '0') || (param_ch == '1'))
	    {
	      unsigned long tmp = strtoul (enc, NULL, 16);

	      if (param_ch == '0') {
		tmp = 15 - tmp;
	      }

	      ret |= tmp << opc_pos;
	    }

	  enc += 3;
	}
      else if ((*enc == '0') || (*enc == '1'))
	{
	  opc_pos--;

	  if (param_ch == *enc) {
	    ret |= 1 << opc_pos;
	  }

	  enc++;
	}
      else if (*enc == param_ch)
	{
	  opc_pos--;
	  param_pos--;
	  ret += ((insn >> opc_pos) & 0x1) << param_pos;

	  if (!param_pos               &&
	      letter_signed (param_ch) &&
	      ret >> (letter_range (param_ch) - 1))
	    {
	      ret |= 0xffffffff << letter_range(param_ch);
	    }
	  enc++;
	}
      else if (ISALPHA (*enc))
	{
	  opc_pos--;
	  enc++;
	}
      else if (*enc == '-')
	{
	  opc_pos--;
	  enc++;
	}
      else
	{
	  enc++;
	}
    }

  return ret;

}	/* or32_extract() */


/*!----------------------------------------------------------------------------
   Test if an instruction matches an encoding

   @param[in] insn      The instruction
   @param[in] encoding  The encoding

   @return  1 (true) if they match, 0 otherwise
   --------------------------------------------------------------------------*/

static int
or32_opcode_match (unsigned long insn, char *encoding)
{
  unsigned long ones, zeros;

  ones  = or32_extract ('1', encoding, insn);
  zeros = or32_extract ('0', encoding, insn);
  
  if ((insn & ones) != ones)
    {
      return 0;
    }
    
  if ((~insn & zeros) != zeros)
    {
      return 0;
    }
  
  return 1;

}	/* or32_opcode_match() */


/*!----------------------------------------------------------------------------
   Print register to INFO->STREAM.

   Used only by print_insn.

   @param[in] param_ch  The character encoding the field of interest
   @param[in] encoding  The encoding
   @param[in] insn      The instruction
   @param[in] info      Info about the disassembly, in particular with the
                        stream to write to.
   --------------------------------------------------------------------------*/

static void
or32_print_register (char param_ch,
		     char *encoding,
		     unsigned long insn,
		     struct disassemble_info *info)
{
  int regnum = or32_extract (param_ch, encoding, insn);
  
  switch (param_ch)
    {
    case 'A':
    case 'B':
    case 'D':
      (*info->fprintf_func) (info->stream, "r%d", regnum);
      break;

    default:
      if (regnum < 16)
	{
	  (*info->fprintf_func) (info->stream, "r%d", regnum);
	}
      else if (regnum < 32)
	{
	  (*info->fprintf_func) (info->stream, "r%d", regnum-16);
	}
      else
	{
	  (*info->fprintf_func) (info->stream, "X%d", regnum);
	}
    }
}	/* or32_print_register() */



/*!----------------------------------------------------------------------------
   Print immediate to INFO->STREAM.

   Used only by print_insn.

   Fixed in GDB 6.8 to print signed immediate fields correctly and to set the
   target fields of jump and branch to symbols if possible.

   @param[in] memaddr   Address of this instruction
   @param[in] param_ch  The character encoding the field of interest
   @param[in] encoding  The encoding
   @param[in] insn      The instruction
   @param[in] info      Info about the disassembly, in particular with the
                        stream to write to.
   --------------------------------------------------------------------------*/

/* Print immediate to INFO->STREAM. Used only by print_insn.  */

static void
or32_print_immediate( bfd_vma                  memaddr,
		      char                     param_ch,
		      char                    *encoding,
		      unsigned long            insn,
		      struct disassemble_info *info)
{
  int imm = or32_extract(param_ch, encoding, insn);
  
  if ('N' == param_ch)
    {
      CORE_ADDR  target_addr = (CORE_ADDR)(memaddr + (4 * imm));
      char      *name        = NULL;
      char      *filename    = NULL;
      int        unmapped    = 0;
      int        offset      = 0;
      int        line        = 0;

      if( 0 == build_address_symbolic( target_addr, 1, &name, &offset,
				       &filename, &line, &unmapped ))
	{
	  (*info->fprintf_func) (info->stream, "<" );

	  if( unmapped )
	    {
	      (*info->fprintf_func) (info->stream, "*");
	    }

	  (*info->fprintf_func) (info->stream, "%s", name);

	  if( offset )
	    {
	      (*info->fprintf_func) (info->stream, "+%d", offset);
	    }

	  (*info->fprintf_func) (info->stream, ">" );
	}
      else
	{
	  (*info->fprintf_func) (info->stream, "0x%x", imm);
	}
    }
  else if (letter_signed(param_ch))
    {
      (*info->fprintf_func) (info->stream, "%d", imm);
    }
  else
    {
      (*info->fprintf_func) (info->stream, "0x%x", imm);
    }
}	/* or32_print_immediate() */


/*!----------------------------------------------------------------------------
   Print one instruction

   Print one instruction from MEMADDR on INFO->STREAM.

   @param[in] memaddr   Address of the instruction to print
   @param[in] info      Info about the disassembly, in particular with the
                        stream to write to.

   @return   The size of the instruction (always 4 on or32).
   --------------------------------------------------------------------------*/

static int
print_insn (bfd_vma                  memaddr,
	    struct disassemble_info *info)
{
  /* The raw instruction.  */
  unsigned char insn_ch[4];
  /* Address. Will be sign extened 27-bit.  */
  unsigned long addr;
  /* The four bytes of the instruction.  */
  unsigned long insn;

  typedef void (*find_byte_func_type) (unsigned char *, unsigned long *);

  find_byte_func_type find_byte_func = (find_byte_func_type) info->private_data;
  struct or32_opcode const *opcode;

  int status =
    (*info->read_memory_func) (memaddr, (bfd_byte *) &insn_ch[0], 4, info);

  if (status != 0)
    {
      (*info->memory_error_func) (status, memaddr, info);
      return -1;
    }

  (*find_byte_func) (&insn_ch[0], &insn);

  for (opcode = &or32_opcodes[0];
       opcode < &or32_opcodes[or32_num_opcodes];
       ++opcode)
    {
      if (or32_opcode_match (insn, opcode->encoding))
        {
          char *s;

          (*info->fprintf_func) (info->stream, "%-8s ", opcode->name);

          for (s = opcode->args; *s != '\0'; ++s)
            {
              switch (*s)
                {
                case '\0':
                  return 4;
      
                case 'r':
                  or32_print_register (*++s, opcode->encoding, insn, info);
                  break;

                case 'X':
                  addr = or32_extract ('X', opcode->encoding, insn) << 2;

                  /* Calulate the correct address.  XXX is this really correct
		     ??  */
                  addr = memaddr + EXTEND29 (addr);

                  (*info->print_address_func)
                    (addr, info);
                  break;

                default:
                  if (strchr (opcode->encoding, *s))
		    {
		      or32_print_immediate (memaddr, *s, opcode->encoding,
					    insn, info);
		    }
                  else
		    {
		      (*info->fprintf_func) (info->stream, "%c", *s);
		    }
		}
            }

          return 4;
        }
    }

  /* This used to be %8x for binutils.  */
  (*info->fprintf_func) (info->stream, ".word 0x%08lx", insn);
  return 4;

}	/* print_insn() */


/*!----------------------------------------------------------------------------
   Disassemble a big-endian or32 instruction

   Print one instruction from MEMADDR on INFO->STREAM.

   @param[in] memaddr   Address of the instruction to print
   @param[in] info      Info about the disassembly, in particular with the
                        stream to write to.

   @return   The size of the instruction (always 4 on or32).
   --------------------------------------------------------------------------*/

int
print_insn_big_or32 (bfd_vma                  memaddr,
		     struct disassemble_info *info)
{
  info->private_data = find_bytes_big;

  return print_insn (memaddr, info);

}	/* print_insn_big_or32() */


/*!----------------------------------------------------------------------------
   Disassemble a little-endian or32 instruction

   Print one instruction from MEMADDR on INFO->STREAM.

   @param[in] memaddr   Address of the instruction to print
   @param[in] info      Info about the disassembly, in particular with the
                        stream to write to.

   @return   The size of the instruction (always 4 on or32).
   --------------------------------------------------------------------------*/

int
print_insn_little_or32 (bfd_vma                  memaddr,
			struct disassemble_info *info)
{
  info->private_data = find_bytes_little;

  return print_insn (memaddr, info);

}	/* print_insn_little_or32() */
