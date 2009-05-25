/* Remote debugging interface for various or1k debugging protocols.
   Currently supported or1k targets are: simulator, jtag, dummy.

   Copyright 1993-1995, 2000 Free Software Foundation, Inc.
   Copyright 2008 Embecosm Limited

   Contributed by Cygnus Support.  Written by Marko Mlinar
   <markom@opencores.org>
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
   with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/*--------------------------------------------------------------------------*/
/*!Updated for GDB 6.8 by Jeremy Bennett. All code converted to ANSI C style
   and in general to GDB format. All global OpenRISC specific functions and
   variables should now have a prefix of or1k_ or OR1K_.

   This original code defined three possible remote targets "jtag", "sim" and
   "dummy", all using the OpenRISC 1000 remote protocol. However only "jtag"
   is actually implemented, and in this version, all the redundant code is
   stripped out. The intention is that in time all remote debugging will be
   via the Remote Serial Protocol.

   Commenting compatible with Doxygen added throughout. */
/*---------------------------------------------------------------------------*/


#include "defs.h"
#include "inferior.h"
#include "bfd.h"
#include "symfile.h"
#include "gdb_wait.h"
#include "gdbcmd.h"
#include "gdbcore.h"
#include "target.h"
#include "gdb_string.h"
#include "event-loop.h"
#include "event-top.h"
#include "inf-loop.h"
#include "regcache.h"

#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include "or1k-tdep.h"
#include "or1k-jtag.h"
#include "exceptions.h"
#include "frame.h"
#include "opcode/or32.h"
#include "solib.h"

#include <arpa/inet.h>


/*! The current OR1K target */
static struct target_ops  or1k_target;

/*! Are we single stepping */
static int  or1k_is_single_stepping;

/*! Cached OR1K debug register values (ignores counters for now). */
static struct {
  unsigned long int  dvr[OR1K_MAX_MATCHPOINTS];
  struct {
    enum {
      OR1K_CT_DISABLED = 0,		/* Disabled */
      OR1K_CT_FETCH    = 1,		/* Compare to fetch EA */
      OR1K_CT_LEA      = 2,		/* Compare to load EA */
      OR1K_CT_SEA      = 3,		/* Compare to store EA */
      OR1K_CT_LDATA    = 4,		/* Compare to load data */
      OR1K_CT_SDATA    = 5,		/* Compare to store data */
      OR1K_CT_AEA      = 6,		/* Compare to load/store EA */
      OR1K_CT_ADATA    = 7		/* Compare to load/store data */
    }    ct;				/* Compare to what? */
    int  sc;				/* Signed comparision */
    enum {
      OR1K_CC_MASKED   = 0,
      OR1K_CC_EQ       = 1,
      OR1K_CC_LT       = 2,
      OR1K_CC_LE       = 3,
      OR1K_CC_GT       = 4,
      OR1K_CC_GE       = 5,
      OR1K_CC_NE       = 6,
      OR1K_CC_RESERVED = 7
    }    cc;				/* Compare operation */
    int  dp;				/* DVR/DCP present */
  }                  dcr[OR1K_MAX_MATCHPOINTS];
  unsigned long int  dmr1;
  unsigned long int  dmr2;
  unsigned long int  dsr;
  unsigned long int  drr;
} or1k_dbgcache;

/*! Old SIGINT handler.  */
static void (*or1k_old_intr_handler) (int) = NULL;

/* Forward declaration of global functions to access SPRs. */

ULONGEST  or1k_read_spr (unsigned int  regnum);
void      or1k_write_spr (unsigned int  regnum,
			  ULONGEST      data);

/* Forward declaration of support functions to handle user interrupt */

static void  or1k_interrupt_query();
static void  or1k_interrupt (int signo);
static void  or1k_interrupt_twice (int signo);

/* Forward declaration of support functions to handle breakpoints */

static unsigned char  or1k_gdb_to_dcr_type (enum target_hw_bp_type  gdb_type);
static int            or1k_set_breakpoint (CORE_ADDR  addr);
static int            or1k_clear_breakpoint (CORE_ADDR  addr);
static int            or1k_watchpoint_gc ();
static int            or1k_stopped_watchpoint_info (CORE_ADDR *addr_p,
						    int       *mp_p);

/* Forward declarations of support functions for the remote operations */

static int        or1k_regnum_to_sprnum (int regnum);
static void       or1k_commit_debug_registers();
static void       or1k_start_remote (struct ui_out *uiout,
				     void          *arg);

/* Forward declarations of functions for the remote operations */

static void     or1k_files_info (struct target_ops *target);
static void     or1k_open (char *name,
			   int   from_tty);
static void     or1k_close (int quitting);
static void     or1k_detach (char *args,
			     int   from_tty);
static void     or1k_fetch_registers (struct regcache *regcache,
				      int              regnum);
static void     or1k_store_registers (struct regcache *regcache,
				      int              regnum);
static void     or1k_prepare_to_store (struct regcache *regcache);
static LONGEST  or1k_xfer_partial (struct target_ops  *ops,
				   enum target_object  object,
				   const char         *annex,
				   gdb_byte           *readbuf,
				   const gdb_byte     *writebuf,
				   ULONGEST            offset,
				   LONGEST             len);
static int      or1k_insert_breakpoint (struct bp_target_info *bpi);
static int      or1k_remove_breakpoint (struct bp_target_info *bpi);
static int      or1k_can_use_hw_matchpoint (int  type,
					    int  count,
					    int  othertype);
static int      or1k_insert_hw_breakpoint (struct bp_target_info *bpi);
static int      or1k_remove_hw_breakpoint (struct bp_target_info *bpi);
static int      or1k_insert_watchpoint (CORE_ADDR  addr,
					int        len,
					int        type);
static int      or1k_remove_watchpoint (CORE_ADDR  addr,
					int        len,
					int        type);
static int      or1k_stopped_by_watchpoint();
static int      or1k_stopped_data_address (struct target_ops *target,
					   CORE_ADDR         *addr_p);
static int      or1k_region_ok_for_hw_watchpoint (CORE_ADDR  addr,
						  int        len);
static void     or1k_resume (ptid_t              ptid,
			     int                 step,
			     enum target_signal  sig);
static ptid_t   or1k_wait (ptid_t                    remote_ptid,
			   struct target_waitstatus *status);
static void     or1k_stop ();
static void     or1k_kill ();
static void     or1k_create_inferior (char  *execfile,
				      char  *args,
				      char **env,
				      int    from_tty);
static void     or1k_mourn_inferior ();
static void     or1k_rcmd (char           *command,
			   struct ui_file *outbuf);


/*---------------------------------------------------------------------------*/
/*!Ask the user about an interrupt

  Ctrl-C has been received. */
/*---------------------------------------------------------------------------*/

static void
or1k_interrupt_query()
{
  target_terminal_ours ();

  if (query ("OpenRISC remote JTAG interrupted while waiting for the program\n"
	     "Give up (and stop debugging it)? "))
    {
      const struct gdb_exception e = {
	.reason  = RETURN_QUIT,
	.error   = GENERIC_ERROR,
	.message = "OpenRISC remote JTAG debugging interrupted"
      };

      or1k_mourn_inferior ();
      throw_exception (e);
    }

  target_terminal_inferior ();

}	/* or1k_interrupt_query() */


/*---------------------------------------------------------------------------*/
/*!The command line interface's stop routine from an interrupt

   Attempt to stop the processor. Set a more severe interrupt routine, so that
   a second ctrl-C will force more aggressive behavior.

   @param[in] signo  The signal which triggered this handle (always SIGINT) */
/*---------------------------------------------------------------------------*/

static void
or1k_interrupt (int signo)
{
  quit_flag = 1;		/* For passive stops */
  or1k_stop ();			/* Actively stall the processor */

  /* If this doesn't work, try more severe steps. */
  signal (signo, or1k_interrupt_twice);

}	/* or1k_interrupt() */


/*---------------------------------------------------------------------------*/
/*!More aggressive interrupt handler

   The user typed ^C twice. We ask if they want to kill everything. If they
   don't we put this signal handler back in place, so another ctrl-C will
   bring us back heer.

   @param[in] signo  The signal which triggered this handle (always SIGINT) */
/*---------------------------------------------------------------------------*/

static void
or1k_interrupt_twice (int signo)
{
  /* Restore the old interrupt handler */
  if (NULL != or1k_old_intr_handler)
    {
      signal (signo, or1k_old_intr_handler);
    }

  or1k_interrupt_query();

  /* If we carry on keep this as the signal handler */
  signal (signo, or1k_interrupt_twice);

}	/* or1k_interrupt_twice() */


/*---------------------------------------------------------------------------*/
/*!Translate GDB watchpoint type into data control register compare to bits

   @param[in] gdb_type  GDB watchpoint type

   @return  The corresponding data control register compare to bits */
/*---------------------------------------------------------------------------*/

static unsigned char
or1k_gdb_to_dcr_type (enum target_hw_bp_type  gdb_type)
{
  switch (gdb_type)
    {
    case hw_write:  return  OR1K_CT_SEA;
    case hw_read:   return  OR1K_CT_LEA;
    case hw_access: return  OR1K_CT_AEA;

    default:
      error  ("or1k_gdb_to_dcr_type: Invalid type %d\n", gdb_type );
      return  -1;
    }
}	/* or1k_gdb_to_dcr_type */


/*---------------------------------------------------------------------------*/
/*!Find the first free matchpoint

  @return  The first free matchpoint, or -1 if none is available. */
/*---------------------------------------------------------------------------*/

static int
or1k_first_free_matchpoint ()
{
  struct gdbarch_tdep *tdep = gdbarch_tdep (current_gdbarch);
  int i;
 
  /* Search for unused matchpoint.  */
  for (i = 0; i < tdep->num_matchpoints; i++)
    {
      if (!or1k_dbgcache.dcr[i].dp)
	{
	  return  i;
	}
    }

  return  -1;

}	/* or1k_first_free_matchpoint() */


/*---------------------------------------------------------------------------*/
/*!Set a breakpoint.

   @param[in] addr  The address at which to set the breakpoint

   @return  0 if the breakpoint was set, non-zero otherwise */
/*---------------------------------------------------------------------------*/

static int
or1k_set_breakpoint (CORE_ADDR  addr)
{
  int mp = or1k_first_free_matchpoint();

  if (mp < 0)
    {
      return  1;
    }

  /* Set the value register to the address where we should breakpoint and the
     control register for unsigned match to the fetched effective address. */
  or1k_dbgcache.dvr[mp]    = addr;

  or1k_dbgcache.dcr[mp].dp = 1;
  or1k_dbgcache.dcr[mp].cc = OR1K_CC_EQ;
  or1k_dbgcache.dcr[mp].sc = 0;
  or1k_dbgcache.dcr[mp].ct = OR1K_CT_FETCH;

  /* No chaining here. Watchpoint triggers a break */
  or1k_dbgcache.dmr1 &= ~(OR1K_DMR1_CW << (OR1K_DMR1_CW_SZ * mp));
  or1k_dbgcache.dmr2 |= (1 << (mp + OR1K_DMR2_WGB_OFF));

  return 0;

}	/* or1k_set_breakpoint() */


/*---------------------------------------------------------------------------*/
/*!See if a matchpoint has the given qualities

   The fields in the matchpoint DVR and DCR registers must match and the
   matchpoint must be in use.

   @param[in] mp    The matchpoint of interest
   @param[in] addr  The address to compare
   @param[in] cc    The condition code to compare
   @param[in] sc    The signedness to compare
   @param[in] ct    The comparision type to compare

   @return  1 (true) if the fields are the same and the matchpoint is in use */
/*---------------------------------------------------------------------------*/

static int
or1k_matchpoint_equal (int            mp,
		       CORE_ADDR      addr,
		       unsigned char  cc,
		       unsigned char  sc,
		       unsigned char  ct)
{
  int res =  or1k_dbgcache.dcr[mp].dp          &&
            (or1k_dbgcache.dcr[mp].cc == cc  ) &&
	    (or1k_dbgcache.dcr[mp].sc == sc  ) &&
	    (or1k_dbgcache.dcr[mp].ct == ct  ) &&
            (or1k_dbgcache.dvr[mp]    == addr);

  return  res;

}	/* or1k_matchpoint_equal() */


/*---------------------------------------------------------------------------*/
/*!Clear a breakpoint.

   @param[in] addr  The address at which to clear the breakpoint

   @return  0 if the breakpoint was cleared, non-zero otherwise */
/*---------------------------------------------------------------------------*/

static int
or1k_clear_breakpoint (CORE_ADDR  addr)
{
  struct gdbarch_tdep *tdep = gdbarch_tdep (current_gdbarch);
  int                  mp;
 
  /* Search for matching breakpoint.  */
  for (mp = 0; mp < tdep->num_matchpoints; mp++)
    {
      if (or1k_matchpoint_equal (mp, addr, OR1K_CC_EQ, 0, OR1K_CT_FETCH))
	{
	  break;
	}
    }

  if (mp >= tdep->num_matchpoints)
    {
      return 1;
    }

  /* Mark the matchpoint unused and clear its bits in the assign to counter,
     watchpoint generating break and breakpoint status bits in DMR2. */
  or1k_dbgcache.dcr[mp].dp = 0;
 
  or1k_dbgcache.dmr2 &= ~(1 << (mp + OR1K_DMR2_AWTC_OFF));
  or1k_dbgcache.dmr2 &= ~(1 << (mp + OR1K_DMR2_WGB_OFF));
  or1k_dbgcache.dmr2 &= ~(1 << (mp + OR1K_DMR2_WBS_OFF));

  return 0;

}	/* or1k_clear_breakpoint() */


/*---------------------------------------------------------------------------*/
/*!Garbage collect the HW watchpoints

   Some functions require more than one OpenRISC 1000 HW watchpoint to be
   chained together. Chained wathcpoints must be adjacent, but setting of
   breakpoints (which uses a single HW matchpoint/watchpoint) can lead to
   fragmentation of the watchpoint list.

   This function allows the currently used matchpoints to be shuffled up. In
   other words we have a GARBAGE COLLECTOR (omninous music, thunder
   clouds gather, lightning flashes).

   For added fun, watchpoints 8 and 9 cannot be moved, and hence nothing that
   depends on them.

   @return  The number of free watchpoints */
/*---------------------------------------------------------------------------*/

static int
or1k_watchpoint_gc ()
{
  struct gdbarch_tdep *tdep          = gdbarch_tdep (current_gdbarch);
  unsigned long int    bits;
  int                  first_free;
  int                  first_used;
  int                  first_fixed;
 
  /* Find the last moveable watchpoint by starting from the top and working
     down until we find a point where the watchpoint is not dependent on its
     predecessor. */

  for (first_fixed = tdep->num_matchpoints; first_fixed > 0; first_fixed--)
    {
      bits = (or1k_dbgcache.dmr1 >> (OR1K_DMR1_CW_SZ * first_fixed)) &
	     OR1K_DMR1_CW;

      if ((OR1K_DMR1_CW_AND != bits) && (OR1K_DMR1_CW_OR != bits))
	{
	  break;		/* Chain is broken */
	}
    }

  /* Give up if there aren't two to be considered */
  if (first_fixed < 2)
    {
      return  first_fixed;
    }

  /* Move matchpoints, and hence unused HW watchpoints to the top (i.e to
     first_fixed - 1). If we move a matchpoint, then all the HW watchpoints
     above must be shuffled down. Although HW watchpoints refer to
     matchpoints, it is a fixed reference based on index. So long as the HW
     watchpoint and the matchpoint are moved together, and the ordering
     remains unchanged all will be OK. Any chained watchpoints will be
     adjacent, and will remain adjacent after this exercise. */

  first_free = 0;
  first_used = 0;

  while (1)
    {
      /* Find the first free matchpoint */
      for( ; first_free < first_fixed; first_free++ )
	{
	  if (!(or1k_dbgcache.dcr[first_free].dp))
	    {
	      break;
	    }
	}

      if (first_free > (first_fixed - 2))
	{
	  return first_fixed - first_free;	/* No more to move */
	}

      /* Find the first fixed breakpoint */
      for (first_used = first_free + 1; first_used < first_fixed; first_used++)
	{
	  if (or1k_dbgcache.dcr[first_used].dp)
	    {
	      break;
	    }
	}

      if (first_used >  (first_fixed - 1))
	{
	  return first_fixed - first_free;	/* No more to move */
	}

      /* Move matchpoint in DVR and DCR registers. */
      or1k_dbgcache.dvr[first_free]    = or1k_dbgcache.dvr[first_used];
      or1k_dbgcache.dcr[first_free]    = or1k_dbgcache.dcr[first_used];
      or1k_dbgcache.dcr[first_used].dp = 0;

      /* Copy chaining bits in DMR1.  */
      bits = (or1k_dbgcache.dmr1 >> (OR1K_DMR1_CW_SZ * first_used)) &
	     OR1K_DMR1_CW;
      or1k_dbgcache.dmr1 &= ~(OR1K_DMR1_CW << (OR1K_DMR1_CW_SZ * first_used));
      or1k_dbgcache.dmr1 &= ~(OR1K_DMR1_CW << (OR1K_DMR1_CW_SZ * first_free));
      or1k_dbgcache.dmr1 |=  (bits         << (OR1K_DMR1_CW_SZ * first_free));
			 
      /* Copy assign to counter, watchpoint generating break and breakpoint
	 status bits in DMR2 */

      bits = (or1k_dbgcache.dmr2 >> (OR1K_DMR2_AWTC_OFF + first_used)) & 1;
      or1k_dbgcache.dmr2 &= ~(1    << (OR1K_DMR2_AWTC_OFF + first_used));
      or1k_dbgcache.dmr2 &= ~(1    << (OR1K_DMR2_AWTC_OFF + first_free));
      or1k_dbgcache.dmr2 |=  (bits << (OR1K_DMR2_AWTC_OFF + first_free));

      bits = (or1k_dbgcache.dmr2 >> (OR1K_DMR2_WGB_OFF + first_used)) & 1;
      or1k_dbgcache.dmr2 &= ~(1    << (OR1K_DMR2_WGB_OFF + first_used));
      or1k_dbgcache.dmr2 &= ~(1    << (OR1K_DMR2_WGB_OFF + first_free));
      or1k_dbgcache.dmr2 |=  (bits << (OR1K_DMR2_WGB_OFF + first_free));

      bits = (or1k_dbgcache.dmr2 >> (OR1K_DMR2_WBS_OFF + first_used)) & 1;
      or1k_dbgcache.dmr2 &= ~(1    << (OR1K_DMR2_WBS_OFF + first_used));
      or1k_dbgcache.dmr2 &= ~(1    << (OR1K_DMR2_WBS_OFF + first_free));
      or1k_dbgcache.dmr2 |=  (bits << (OR1K_DMR2_WBS_OFF + first_free));

      first_free++;
    }
}	/* or1k_watchpoint_gc() */
  

/*---------------------------------------------------------------------------*/
/*!Find if we were stopped by a watchpoint, and if so which address

   This is true if a triggered breakpoint was ANDed with the previous
   watchpoint in the chain. If so, the previous matchpoint has the start
   address.

   This is an internal utility for use by or1k_stopped_watchpoint() and
   or1k_stopped_data_address().

   @note This will only find the FIRST watchpoint triggered.

   @param[out] addr_p  Where to put the address associated with the
                       watchpoint if non-NULL
   @param[out] mp_p    Where to put the matchpoint which triggered the WP

   @return  1 (true) if we get the address, 0 (false) otherwise */
/*---------------------------------------------------------------------------*/

static int
or1k_stopped_watchpoint_info (CORE_ADDR *addr_p,
			      int       *mp_p)
{
  struct gdbarch_tdep *tdep = gdbarch_tdep (current_gdbarch);
  int                  mp;

  /* Cannot be the first matchpoint (it has to AND with one previous) */
  for( mp = 1 ; mp < tdep->num_matchpoints ; mp++ )
    {
      if (((or1k_dbgcache.dmr2 >> (mp + OR1K_DMR2_WBS_OFF)) & 1) &&
	  (OR1K_DMR1_CW_AND ==
	   ((or1k_dbgcache.dmr1 >> (mp * OR1K_DMR1_CW_SZ)) & OR1K_DMR1_CW)))
	{
	  if (NULL != addr_p)
	    {
	      *addr_p = or1k_dbgcache.dvr[mp - 1];
	    }

	  if (NULL != mp_p)
	    {
	      *mp_p = mp;
	    }

	  return  1;
	}
    }

  return  0;

}	/* or1k_stopped_watchpoint_info() */


/*---------------------------------------------------------------------------*/
/*!Convert a register number to SPR number

   OR1K debug unit has GPRs mapped to SPRs, which are not compact, so we are
   mapping them for GDB.

   Rewritten by CZ 12/09/01

   @param[in] regnum  The register

   @return  The corresponding SPR or -1 on failure */
/*---------------------------------------------------------------------------*/

static int
or1k_regnum_to_sprnum (int regnum) 
{
  /* The GPRs map as a block */
  if (regnum < OR1K_MAX_GPR_REGS)
    {
      return  OR1K_SPR (OR1K_SPG_SYS, OR1K_SPG_SYS_GPR + regnum);
    }

  /* The "special" registers */
  switch (regnum)
    {
    case OR1K_PPC_REGNUM: return  OR1K_NPC_SPRNUM;
    case OR1K_NPC_REGNUM: return  OR1K_NPC_SPRNUM;
    case OR1K_SR_REGNUM:  return  OR1K_SR_SPRNUM;

    default:
      error ("or1k_regnum_to_sprnum: invalid register number!");
      break;
    }

  return -1;

}	/* or1k_regnum_to_sprnum() */


/*---------------------------------------------------------------------------*/
/*!Sync debug registers

   Synchronizes debug registers in memory with those on target. This used to
   track if there was any change, but there always is with debuggers, so now
   we just write them every time.

   The old code did a lot of this via low-level manipulation of the JTAG
   interface. For now it is replaced by straightward reading/writing of the
   SPRs. */
/*---------------------------------------------------------------------------*/

static void
or1k_commit_debug_registers()
{
  struct gdbarch_tdep *tdep = gdbarch_tdep (current_gdbarch);
  int                  i;

  /* Matchpoints (breakpoints, watchpoints).  */
  for (i = 0; i < tdep->num_matchpoints; i++)
    {
      unsigned long int  dcr = 0;

      dcr |= or1k_dbgcache.dcr[i].dp ? OR1K_DCR_DP : 0;
      dcr |= (or1k_dbgcache.dcr[i].cc << OR1K_DCR_CC_OFF) & OR1K_DCR_CC;
      dcr |= or1k_dbgcache.dcr[i].sc ? OR1K_DCR_SC : 0;
      dcr |= (or1k_dbgcache.dcr[i].ct << OR1K_DCR_CT_OFF) & OR1K_DCR_CT;

      or1k_jtag_write_spr (OR1K_DVR0_SPRNUM + i, or1k_dbgcache.dvr[i]);
      or1k_jtag_write_spr (OR1K_DCR0_SPRNUM + i, dcr);
    }

  or1k_jtag_write_spr (OR1K_DMR1_SPRNUM, or1k_dbgcache.dmr1);
  or1k_jtag_write_spr (OR1K_DMR2_SPRNUM, or1k_dbgcache.dmr2);
  or1k_jtag_write_spr (OR1K_DSR_SPRNUM,  or1k_dbgcache.dsr);
  or1k_jtag_write_spr (OR1K_DRR_SPRNUM,  or1k_dbgcache.drr);

}	/* or1k_commit_debug_registers() */


/*---------------------------------------------------------------------------*/
/*!Start the remote target

   This is a wrapper for GDB's start_remote function, suitable for use with
   catch_exception.

   @param[in] uiout  UI handle - not used
   @param[in] arg_p  Reference to the generic argument supplied through
                     catch_exception. In this case a pointer to the from_tty
		     parameter. */
/*--------------------------------------------------------------------------*/

static void
or1k_start_remote (struct ui_out *uiout,
		   void          *arg_p)
{
  int  from_tty = *((int *)arg_p);

  start_remote( from_tty );

}	/* or1k_start_remote() */


/*---------------------------------------------------------------------------*/
/*!Print info on this target.

  @param[in] target  GDB ops for the target - ignored. */
/*---------------------------------------------------------------------------*/

static void
or1k_files_info (struct target_ops *target)
{
  const char *status_name[] =
    {
      "UNDEFINED",
      "CONNECTING",
      "DISCONNECTING",
      "RUNNING",
      "STOPPED"
    };

  char *file = "nothing";

  if (exec_bfd)
    {
      file = bfd_get_filename (exec_bfd);
    }

  printf_filtered ("File \"%s\"", file);

  if (exec_bfd)
    {
      printf_filtered ("attached to %s running program %s: ",
		       target_shortname, file);
    }
}	/* or1k_files_info() */


/*---------------------------------------------------------------------------*/
/*!Open a connection to the remote server

   Initialise the connection, then push the current target.

   @param[in] name      The arguments passed to the target command on GDB -
                        usually the address of the remote server
   @param[in] from_tty  1 (true) if the GDB session is being run from a
                        terminal and so can receive messages. 0 (false)
                        otherwise. */
/*---------------------------------------------------------------------------*/

static void
or1k_open (char *name,
	   int   from_tty)
{
  struct gdbarch_tdep *tdep     = gdbarch_tdep (current_gdbarch);
  int                  i;
  unsigned int         tmp;
  struct gdb_exception ex;

  /* Check nothing's still running. This will call or1k_close if anything is,
     which will close the connection. Having done this, any remaining target
     instances should be unpushed from the stack */
  target_preopen (from_tty);
  unpush_target (&or1k_target);

  /* Initialize the connection to the remote target. */
  or1k_jtag_init (name);

  /* Make sure we have system and debug groups implemented. */
  tmp = or1k_jtag_read_spr (OR1K_UPR_SPRNUM);
  if (0 == (tmp & OR1K_SPR_UPR_UP))
    {
      error ("or1k_open: system group missing");
    }
  if (0 == (tmp & OR1K_SPR_UPR_DUP))
    {
      error ("or1k_open: debug group missing");
    }

  /* Determine number of gpr_regs (this is a bit simplified - the bit just
     means "less than 32", but we assume that means 16) and the number of
     supported watchpoints. */
  tmp                = or1k_jtag_read_spr (OR1K_CPUCFGR_SPRNUM);
  tdep->num_gpr_regs = (0 == (tmp & OR1K_SPR_CPUCFGR_CGF)) ? 32 : 16;

  tmp                   = or1k_jtag_read_spr (OR1K_DCFGR_SPRNUM);
  tdep->num_matchpoints = (tmp & OR1K_SPR_DCFGR_NDP) + 1;

  /* Stall the processor. A pause after stalling is appropriate for real
     hardware. */
  or1k_jtag_stall ();
  usleep (1000);

  /* Initialize the debug register cache. The DSR is set to stop when
     breakpoint occurs. The cache will be written out before unstalling the
     processor. */
  for (i = 0; i < tdep->num_matchpoints; i++)
    {
      or1k_dbgcache.dvr[i] = 0;
      memset (&or1k_dbgcache.dcr[i], 0, sizeof (or1k_dbgcache.dcr[i]));
    }

  or1k_dbgcache.dmr1 = 0;
  or1k_dbgcache.dmr2 = 0;
  or1k_dbgcache.dsr  = OR1K_DSR_TE;
  or1k_dbgcache.drr  = 0;
    
  /* The remote target connection stalls the target, so all aspects of a
     running connection are there, except it doesn't actually have
     execution. So we call target_mark_running, but then immediately mark it
     as not having execution */
  push_target (&or1k_target);
  target_mark_running (&or1k_target);
  target_has_execution = 0;

  /* Get rid of any old shared library symbols. */
  no_shared_libraries (NULL, 0);

  /* We don't have a concept of processs or threads, so we use the null PTID
     for the target. Set this here before the start_remote() code uses it. */
  inferior_ptid = null_ptid;

  /* Start the remote target. Uses our own wrapper, so we can wrap it to
     catch the exeception if it goes tits up. */
  ex = catch_exception (uiout, or1k_start_remote, &from_tty, RETURN_MASK_ALL);
  if (ex.reason < 0)
    {
      pop_target ();
      throw_exception (ex);
    }

}	/* or1k_open() */


/*---------------------------------------------------------------------------*/
/*!Close a connection to the remote server

   Use the target extra op is there is one. Don't mourn the inferior - it
   calls us, so we'll get a never ending loop!

   param[in] quitting  If true (1) GDB is going to shut down, so don't worry
   to wait for everything to complete. */
/*--------------------------------------------------------------------------*/

static void
or1k_close (int quitting)
{
  or1k_jtag_close ();

}	/* or1k_close() */


/*---------------------------------------------------------------------------*/
/*!Detach from the remote server

   There is only one remote connection, so no argument should be given.

   @param[in] args      Any args given to the detach command
   @param[in] from_tty  True (1) if this GDB session is run from a terminal,
   so messages can be output. */
/*---------------------------------------------------------------------------*/

static void
or1k_detach (char *args,
	     int   from_tty)
{
  if (args)
    {
      error ("or1k_detach: \"detach\" takes no arg when remote debugging\n");
    }

  or1k_close (1);

  if (from_tty)
    {
      printf_unfiltered ("Ending remote or1k debugging\n");
    }
}	/* or1k_detach() */


/*---------------------------------------------------------------------------*/
/*!Fetch remote registers

   The remote registers are fetched into the register cache

   @param[in] regcache  The register cache to use
   @param[in] regnum    The desired register number, or -1 if all are wanted */
/*---------------------------------------------------------------------------*/

static void
or1k_fetch_registers (struct regcache *regcache,
		      int              regnum)
{
  unsigned long int  val;
  char               buf[sizeof (CORE_ADDR) ];

  if (-1 == regnum)
    {
      /* Get the lot */
      for (regnum = 0; regnum < OR1K_NUM_REGS; regnum++)
	{
	  or1k_fetch_registers (regcache, regnum);
	}
      return;
    }

  if (regnum >= OR1K_NUM_REGS)
    {
      error ("or1k_fetch_registers: invalid register number");
    }

  /* Convert to SPRNUM and read.  */
  val = or1k_jtag_read_spr (or1k_regnum_to_sprnum (regnum));

  /* We got the number the register holds, but gdb expects to see a value in
     the target byte ordering.

     This is new for GDB 6.8. Not sure if I need get_thread_regcache() using
     the ptid instead here. The old code used supply_register(), but that no
     longer exists, so we hope regcache_raw_supply will do instead. */

  store_unsigned_integer (buf, sizeof (CORE_ADDR), val);
  regcache_raw_supply (regcache, regnum, buf);

}	/* or1k_fetch_registers() */


/*---------------------------------------------------------------------------*/
/*!Store remote registers

   The remote registers are written from the register cache

   @param[in] regcache  The register cache to use
   @param[in] regnum    The desired register number, or -1 if all are wanted */
/*---------------------------------------------------------------------------*/

static void
or1k_store_registers (struct regcache *regcache,
		      int              regnum)
{
  ULONGEST         res;

  if (-1 == regnum)
    {
      for (regnum = 0; regnum < OR1K_NUM_REGS; regnum++)
	{
	  or1k_store_registers (regcache, regnum);
	}
      return;
    }

  if (regnum >= OR1K_NUM_REGS)
    {
      error ("or1k_store_registers: invalid register number");
    }

  regcache = get_current_regcache();
  regcache_cooked_read_unsigned (regcache, regnum, &res);
  or1k_jtag_write_spr (or1k_regnum_to_sprnum (regnum), res);

}	/* or1k_store_registers() */


/*---------------------------------------------------------------------------*/
/*!Prepare to store registers

   This is a null function for the OpenRisc 1000 architecture

   @param[in] regcache  The register cache to use */
/*---------------------------------------------------------------------------*/

static void
or1k_prepare_to_store (struct regcache *regcache)
{
  return;

}	/* or1k_prepare_to_store() */


/*---------------------------------------------------------------------------*/
/*!Transfer some data to or from the target

   One and only one of readbuf or writebuf should be non-NULL, which indicates
   whether this is a read or write operation.

   OR1K reads and writes are in words, so we may need to do partial
   read/writes at each end of the transfer.

   @param[in]  ops       The target_ops vector to use
   @param[in]  object    The type of object to transfer
   @param[in]  annex     Additional object specific information (not used)
   @param[out] readbuf   Buffer for read data
   @param[in]  writebuf  Buffer of data to write
   @param[in]  offset    Offset into object
   @param[in]  len       Max bytes to transer

   @return  Number of bytes transferred or -1 if not supported */
/*---------------------------------------------------------------------------*/

LONGEST  or1k_xfer_partial (struct target_ops  *ops,
				 enum target_object  object,
				 const char         *annex,
				 gdb_byte           *readbuf,
				 const gdb_byte     *writebuf,
				 ULONGEST            offset,
				 LONGEST             len)
{
  /* Only memory transfer is currently supported */
  if (TARGET_OBJECT_MEMORY != object)
    {
      return  -1;
    }

  if (NULL == writebuf)
    {
      /* Must be a read */
      gdb_assert (NULL != readbuf);
      
      return  or1k_jtag_read_mem (offset, readbuf, len);
    }
  else
    {
      return  or1k_jtag_write_mem (offset, writebuf, len);
    }
}	/* or1k_xfer_partial() */
   

/*---------------------------------------------------------------------------*/
/*!Insert a breakpoint

   Try to insert a breakpoint. Use hardware breakpoints if they are supported,
   software breakpoints otherwise.

   @param[in] bpi  Break point info with details of the address and a cache to
                   hold the existing memory value if overwriting.

		   @return  0 if the breakpoint was set, non-zero otherwise */
/*---------------------------------------------------------------------------*/

static int
or1k_insert_breakpoint (struct bp_target_info *bpi)
{
  if (0 == or1k_set_breakpoint (bpi->placed_address))
    {
      return 0;
    }
  else
    {
      return  memory_insert_breakpoint (bpi);
    }
}	/* or1k_insert_breakpoint() */


/*---------------------------------------------------------------------------*/
/*!Remove a breakpoint

   @param[in] bpi  Break point info with details of the address and a cache to
                   hold the existing memory value if overwriting.

		   @return  0 if the breakpoint was cleared, non-zero otherwise */
/*---------------------------------------------------------------------------*/

static int
or1k_remove_breakpoint (struct bp_target_info *bpi)
{
  /* First try to remove HW breakpoint at address */
  if (0 != or1k_clear_breakpoint (bpi->placed_address))
    {
      return  memory_remove_breakpoint (bpi);
    }
  else
    {
      return  0;
    }
}	/* or1k_remove_breakpoint() */


/*---------------------------------------------------------------------------*/
/*!Tell whether we can support a hardware breakpoint or watchpoint

   There are only a fixed number of hardware breakpoints available. Check if
   there are any left. Hardware watchpoints need TWO matchpoints.

   This is only an approximation. An exact response would need full knowledge
   of all the HW resources requested and would need to guarantee those
   resources were not subsequently cannabalized for non-HW breakpoints and
   watchpoints.

   The algorithm is to multiply count by 1 for breakpoints and 2 for
   watchpoints and add 2 if othertype is set. Any matchpoints already used are
   ignored (we cannot know if they relate to the question currently being
   asked). This approach means we will never reject when there is HW
   breakpoint resource available, but we may sometimes accept when there turns
   out to be no HW resource available. But GDB handles that OK (see section
   5.1.2 in the user guide).

   @param[in] type       What type of matchpoint are we asking about? This
                         function is apparently only called for
                         bp_hardware_watchpoint, bp_read_watchpoint,
                         bp_write_watchpoint (but the enum is called
                         access_watchpoint) or bp_hardware_breakpoint
   @param[in] count      How many of this type of matchpoint have been used
                         (including this one)
   @param[in] othertype  1 (true) if this is a call about a watchpoint and
                         watchpoints (but not breakpoints) of other types have
                         been set.

   @return  the number of watchpoints of this type that can be set if count
            watchpoints can be set, 0 if watchpoints of this type cannot be
            set and negative if watchpoints of this type can be set, but there
            are none available. */
/*--------------------------------------------------------------------------*/

static int
or1k_can_use_hw_matchpoint (int  type,
			    int  count,
			    int  othertype)
{
  struct gdbarch_tdep *tdep = gdbarch_tdep (current_gdbarch);

  switch (type)
    {
    case bp_hardware_breakpoint:
      if (count <= tdep->num_matchpoints)
	{
	  return  tdep->num_matchpoints;
	}
      else
	{
	  return -1;
	}

    case bp_hardware_watchpoint:
    case bp_read_watchpoint:
    case bp_access_watchpoint:
      if ((2 * (othertype + count)) <= tdep->num_matchpoints)
	{
	  return  tdep->num_matchpoints / (2 * (othertype + count));
	}
      else
	{
	  return  -1;
	}

    default:
      warning( "Request to use unknown hardware matchpoint type: %d\n",
	       type );
      return  0;
    }

}	/* or1k_can_use_hw_matchpoint() */


/*---------------------------------------------------------------------------*/
/*!Insert a hardware breakpoint

   Try to insert a hardware breakpoint if they are supported.

   @param[in] bpi  Break point info with details of the address and a cache to
                   hold the existing memory value if overwriting.

		   @return  0 if the breakpoint was set, non-zero otherwise */
/*---------------------------------------------------------------------------*/

static int
or1k_insert_hw_breakpoint (struct bp_target_info *bpi)
{
  return  or1k_set_breakpoint (bpi->placed_address);

}	/* or1k_insert_hw_breakpoint() */


/*---------------------------------------------------------------------------*/
/*!Remove a hardware breakpoint

   @param[in] bpi  Break point info with details of the address and a cache to
                   hold the existing memory value if overwriting.

		   @return  0 if the breakpoint was cleared, non-zero otherwise */
/*---------------------------------------------------------------------------*/

static int
or1k_remove_hw_breakpoint (struct bp_target_info *bpi)
{
  return  or1k_clear_breakpoint (bpi->placed_address);

}	/* or1k_remove_hw_breakpoint() */


/*---------------------------------------------------------------------------*/
/*!Set a data watchpoint.

   A GDB data watchpoint uses a pair of HW watchpoints. The first looks for
   accesses greater than or equal to the start address, the second looks for
   accesses less than or equal to the end address. This allows watching of any
   length of object.

   However because of the way the OpenRISC 1000 chains its watchpoints, a
   suitable adjacent pair must be found. So the watchpoint garbage collector
   is used to shuffle them all up.

   @param[in] addr  Address to be watched
   @param[in] len   Length of the entity to be watched
   @param[in] type  hw_write (0) for a write watchpoint, hw_read (1) for a
                    read watchpoint, or hw_access (2) for a read/write
		    watchpoint.

   @return  0 if the watchpoint was set, -1 otherwise (it must be -1, not just
   any non-zero number, since breakpoint.c tests explicitly for -1 */
/*--------------------------------------------------------------------------*/

static int
or1k_insert_watchpoint (CORE_ADDR  addr,
			int        len,
			int        type)
{
  unsigned char        dcr_ct = or1k_gdb_to_dcr_type (type);
  int                  first_free;
 
  if (len < 1)
    {
      return  -1;
    }

  /* Garbage collect and see if we have two adjacent watchpoints available. */

  if( or1k_watchpoint_gc() < 2 )
    {
      return  -1;
    }


  /* Set lower bound at first free matchpoint. */
  first_free               = or1k_first_free_matchpoint();

  or1k_dbgcache.dvr[first_free]     = addr;
  or1k_dbgcache.dcr[first_free].dp  = 1;
  or1k_dbgcache.dcr[first_free].cc  = OR1K_CC_GE;
  or1k_dbgcache.dcr[first_free].sc  = 0;
  or1k_dbgcache.dcr[first_free].ct  = dcr_ct;

  /* Set upper watchpoint bound at next matchpoint. GC guarantees
     it is free. */
  first_free++;

  or1k_dbgcache.dvr[first_free]     = addr + len - 1;
  or1k_dbgcache.dcr[first_free].dp  = 1;
  or1k_dbgcache.dcr[first_free].cc  = OR1K_CC_LE;
  or1k_dbgcache.dcr[first_free].sc  = 0;
  or1k_dbgcache.dcr[first_free].ct  = dcr_ct;
   
  /* Set && chaining of the second matchpoint to the first. This is the only
     one which should generate a breakpoint, since it only occurs when BOTH
     matchpoints trigger. */
  or1k_dbgcache.dmr1 &= ~(OR1K_DMR1_CW     << (OR1K_DMR1_CW_SZ * first_free));
  or1k_dbgcache.dmr1 |=  (OR1K_DMR1_CW_AND << (OR1K_DMR1_CW_SZ * first_free));
  or1k_dbgcache.dmr2 |= (1 << (first_free + OR1K_DMR2_WGB_OFF));

  return 0;

}	/* or1k_insert_watchpoint() */


/*---------------------------------------------------------------------------*/
/*!Remove a data watchpoint.

   @param[in] addr  Address being watched
   @param[in] len   Length of the entity being watched
   @param[in] type  0 for a write watchpoint, 1 for a read watchpoint, or 2
                    for a read/write watchpoint

		    @return  0 if the watchpoint was cleared, non-zero otherwise */
/*---------------------------------------------------------------------------*/

static int
or1k_remove_watchpoint (CORE_ADDR  addr,
			int        len,
			int        type)
{
  struct gdbarch_tdep *tdep = gdbarch_tdep (current_gdbarch);
  unsigned char        dcr_ct;
  CORE_ADDR            end_addr;
  int                  mp;

  if (len < 1)
    {
      return -1;
    }

  dcr_ct   = or1k_gdb_to_dcr_type (type);
  end_addr = addr + len - 1;

  /* Find the right one. PS. Don't be tempted to compare mp against
     tdep->num_matchpoints - 1. It's unsigned and can be 0, so you'd go
     on for ever! */
  for (mp = 0; (mp + 1) < tdep->num_matchpoints; mp++)
    {
      if (or1k_matchpoint_equal( mp,     addr,     OR1K_CC_GE, 0, dcr_ct) &&
	  or1k_matchpoint_equal( mp + 1, end_addr, OR1K_CC_LE, 0, dcr_ct))
	{
	  break;
	}
    }
 
  if ((mp + 1) >= tdep->num_matchpoints)
    {
      return -1;
    }

  /* Mark each matchpoint unused and clear the bits for the second watchpoint
     in the assign to counter, watchpoint generating break and breakpoint
     status bits in DMR2. */

  or1k_dbgcache.dcr[mp].dp = 0;

  mp++;
  or1k_dbgcache.dcr[mp].dp = 0;
  or1k_dbgcache.dmr2 &= ~(1 << (mp + OR1K_DMR2_AWTC_OFF));
  or1k_dbgcache.dmr2 &= ~(1 << (mp + OR1K_DMR2_WGB_OFF));
  or1k_dbgcache.dmr2 &= ~(1 << (mp + OR1K_DMR2_WBS_OFF));

  return 0;

}	/* or1k_remove_watchpoint() */


/*---------------------------------------------------------------------------*/
/*!Report if we stopped due to a watchpoint

   All the information is calculated by or1k_stopped_data_address, so we must
   reuse that.

   @return  1 (true) if we were stopped by a watchpoint, 0 (false) otherwise */
/*---------------------------------------------------------------------------*/

static int
or1k_stopped_by_watchpoint()
{
  return  or1k_stopped_watchpoint_info (NULL, NULL);

}	/* or1k_stopped_by_watchpoint() */


/*---------------------------------------------------------------------------*/
/*!Address for which we were stopped by a remote watchpoint

   This is true if a triggered breakpoint was ANDed with the previous
   watchpoint in the chain. If so, the previous matchpoint has the start
   address.

   This function is called once when a watchpoint is hit, and must clear the
   watchpoint status

   @param[in]  target  The target_ops we are using. Not clear why we need
                       this!
   @param[out] addr_p  Where to put the address associated with the
                       watchpoint.

		       @return  1 (true) if we get the address, 0 (false) otherwise */
/*---------------------------------------------------------------------------*/

static int
or1k_stopped_data_address (struct target_ops *target,
			   CORE_ADDR         *addr_p)
{
  int  mp;

  /* Clear the result info if there was a watchpoint */
  if( or1k_stopped_watchpoint_info (addr_p, &mp))
    {
      or1k_dbgcache.dmr2 &= ~(1 << (mp + OR1K_DMR2_WBS_OFF));
      return  1;
    }
  else
    {
      return  0;
    }
}	/* or1k_stopped_data_address() */


/*---------------------------------------------------------------------------*/
/*!Can we put a HW watchpoint of the given size at the given address

   We can always use a HW watchpoint, so long as there are HW watchpoints
   available. This doesn't have to check availablility (see
   or1k_can_use_hw_matchpoint().

   @param[in] addr  The address of interest
   @param[in] len   The length of the region to watch

   @return  1 (true) to indicate we always can do a HW watchpoint */
/*---------------------------------------------------------------------------*/

static int
or1k_region_ok_for_hw_watchpoint (CORE_ADDR  addr,
				  int        len)
{
  return  1;

}	/* or1k_region_ok_for_hw_watchpoint */


/*---------------------------------------------------------------------------*/
/*!Resume execution of the target process.

   "step" says whether to single-step or to run free; "siggnal" is the signal
   value (e.g. SIGINT) to be given to the target, or zero for no signal.

   When we enter this routine the target should be stalled.

   In general GDB sorts out issues of restarting across breakpoints. However
   have to deal with the special case where a breakpoint occurred in the delay
   slot of a branch instruction. In this case the branch should be restarted,
   BUT ONLY if the branch had originally been executed.

   For now we ignore this case (it really is very hard.

   @param[in]  ptid  The process or thread ID - ignored here
   @param[in]  step  If true (1) single step the target
   @param[in]  sig   Signal to give to the target */
/*---------------------------------------------------------------------------*/

static void
or1k_resume (ptid_t              ptid,
	     int                 step,
	     enum target_signal  sig)
{
  /* Note if we are single stepping. For use when waiting */
  or1k_is_single_stepping = step;

  /* The debug reason register and the watchpoint break status
     are both cleared before resuming. */
  or1k_dbgcache.drr   = 0;
  or1k_dbgcache.dmr2 &= ~OR1K_DMR2_WBS_MASK;

  /* Set single stepping if required */
  if (step)
    {
      or1k_dbgcache.dmr1 |= OR1K_DMR1_ST;
    }
  else
    {
      or1k_dbgcache.dmr1 &= ~OR1K_DMR1_ST;
    }

  or1k_commit_debug_registers();

  /* We can now continue normally, independent of step. If this is called for
     the first time due to a run command, we won't actually be executing, so
     start the target running. */

  or1k_jtag_write_spr( OR1K_NPC_SPRNUM, read_pc());
  target_has_execution = 1;
  or1k_jtag_unstall ();

}	/* or1k_resume() */


/*---------------------------------------------------------------------------*/
/*!Wait until the remote server stops, and return a wait status.

   Seems to assume that the remote target is identified solely by PID with no
   thread or lightweight thread component.

   This is mostly a case of just sorting out the type of exception. GDB will
   automatically sort out restarting with breakpoints removed etc.

   @note The old code differentiated a high priority and a low priority
         interrupt, which no longer exists on the OR1K. The old code also
         omitted tick timer and floating point exceptions, which have been
         added.

   @param[in]  remote_ptid  The process/thread ID of the remote target
   @param[out] status       Status of the waiting process

   @return  The remote_ptid unchanged */
/*---------------------------------------------------------------------------*/

static ptid_t
or1k_wait (ptid_t                    remote_ptid,
	   struct target_waitstatus *status)
{
  unsigned long int  addr;
  char               buf[OR1K_INSTLEN];
  int                res;

  /* Set new signal handler (so we can interrupt with ctrl-C) and then wait
     for the OR1K to stall. */
  or1k_old_intr_handler = signal (SIGINT, or1k_interrupt);
  or1k_jtag_wait (or1k_is_single_stepping);
  signal (SIGINT, or1k_old_intr_handler);

  /* Registers and frame caches are now all unreliable */
  registers_changed();

  /* Refresh the debug registers that may have changed */
  or1k_dbgcache.drr  = or1k_jtag_read_spr (OR1K_DRR_SPRNUM);
  or1k_dbgcache.dmr2 = or1k_jtag_read_spr (OR1K_DMR2_SPRNUM);

  /* If we got a trap, then it was a breakpoint/watchpoint of some sort (but
     not single step, which does not set the trap exception). We need to back
     up the program counter, so the instruction will be re-executed. This
     must be the value of the PPC, in case we have just done a branch. */

  if (OR1K_DRR_TE == (or1k_dbgcache.drr & OR1K_DRR_TE))
    {
      write_pc (or1k_jtag_read_spr (OR1K_PPC_SPRNUM));
    }

  /* Single step does not set trap exception, so we set it manually to
     simplify our code. */
  if (OR1K_DMR1_ST == (or1k_dbgcache.dmr1 & OR1K_DMR1_ST))
    {
      or1k_dbgcache.drr |= OR1K_DRR_TE;
    }

  status->kind = TARGET_WAITKIND_STOPPED; 

  /* Map OR1K exception to GDB signal. If multiple signals are set, only the
     least significant in the DRR is used (the expression here yields the LSB
     in a 2's complement machine) */
  switch (or1k_dbgcache.drr & (-or1k_dbgcache.drr))
    {
    case OR1K_DRR_RSTE:  status->value.sig = TARGET_SIGNAL_PWR;  break;
    case OR1K_DRR_BUSEE: status->value.sig = TARGET_SIGNAL_BUS;  break;
    case OR1K_DRR_DPFE:  status->value.sig = TARGET_SIGNAL_SEGV; break;
    case OR1K_DRR_IPFE:  status->value.sig = TARGET_SIGNAL_SEGV; break;
    case OR1K_DRR_TTE:   status->value.sig = TARGET_SIGNAL_ALRM; break;
    case OR1K_DRR_AE:    status->value.sig = TARGET_SIGNAL_BUS;  break;
    case OR1K_DRR_IIE:   status->value.sig = TARGET_SIGNAL_ILL;  break;
    case OR1K_DRR_INTE:  status->value.sig = TARGET_SIGNAL_INT;  break;
    case OR1K_DRR_DME:   status->value.sig = TARGET_SIGNAL_SEGV; break;
    case OR1K_DRR_IME:   status->value.sig = TARGET_SIGNAL_SEGV; break;
    case OR1K_DRR_RE:    status->value.sig = TARGET_SIGNAL_FPE;  break;
    case OR1K_DRR_SCE:   status->value.sig = TARGET_SIGNAL_USR2; break;
    case OR1K_DRR_FPE:   status->value.sig = TARGET_SIGNAL_FPE;  break;
    case OR1K_DRR_TE:    status->value.sig = TARGET_SIGNAL_TRAP; break;

    default:
      /* This just means the target stopped without raising any
	 interrupt. This happens at reset and exit. The latter we distinquish
	 by looking at the instruction just executed, to see if it is
	 "l.nop NOP_EXIT". If this is the case, get the result from GPR 3 and
	 set the NPC to the reset vector, so a new start will behave well. */

      addr = or1k_jtag_read_spr (OR1K_PPC_SPRNUM);
      res  = read_memory_nobpt (addr, buf, OR1K_INSTLEN);
      if (0 != res)
	{
	  memory_error (res, addr);
	}

      if (OR1K_NOP_EXIT == 
	  (unsigned long int)(extract_unsigned_integer (buf, OR1K_INSTLEN)))
	{
	  ULONGEST  val;

	  regcache_cooked_read_unsigned(get_current_regcache(),
					OR1K_FIRST_ARG_REGNUM, &val);
	  status->value.integer = val;
	  status->kind          = TARGET_WAITKIND_EXITED;
	  write_pc (OR1K_RESET_VECTOR);
	}
      else
	{
	  status->value.sig     = TARGET_SIGNAL_DEFAULT;
	}
      break;
    }

  /* Clear the reason register */
  or1k_dbgcache.drr = 0;

  return remote_ptid;

}	/* or1k_wait() */


/*---------------------------------------------------------------------------*/
/*!Stop the remote process

   This is the generic stop called via the target vector. When a target
   interrupt is requested, either by the command line or the GUI, we will
   eventually end up here.

   Just stall the processor, put it into single step mode and unstall. */
/*---------------------------------------------------------------------------*/

static void
or1k_stop ()
{
  /* We should not stop the target immediately, since it can be in an
     unfinished state.  So we do a single step.  This should not affect
     anything.  */
  or1k_jtag_stall();

  /* HW STEP.  Set OR1K_DMR1_ST.  */
  or1k_dbgcache.dmr1 |= OR1K_DMR1_ST;
  or1k_commit_debug_registers();

  or1k_jtag_unstall();

}	/* or1k_stop () */


/*---------------------------------------------------------------------------*/
/*!Kill the process running on the board

   We just ignore the target. Mourning the inferior will cause the connection
   to be closed. */
/*---------------------------------------------------------------------------*/

static void
or1k_kill ()
{
  or1k_mourn_inferior ();

}	/* or1k_kill () */


/*---------------------------------------------------------------------------*/
/*!Start running on the target board by creating an inferior process

   @param[in] execfile  What to run
   @param[in] args      Arguments to pass to the inferior process
   @param[in] env       Environment for the inferior process
   @param[in] from_tty  1 (true) if this session can write to the terminal */
/*---------------------------------------------------------------------------*/

static void
or1k_create_inferior (char  *execfile,
		      char  *args,
		      char **env,
		      int    from_tty)
{
  CORE_ADDR entry_pt;

  if ((NULL != args) && ('\0' != args[0]))
    {
      warning ("or1k_create_inferior: "
	       "can't pass arguments to remote OR1K board - ignored");

      /* And don't try to use them on the next "run" command.  */
      execute_command ("set args", from_tty);
    }

  if ((NULL == execfile) || (NULL == exec_bfd))
    {
      warning (
	"or1k_create_inferior: no executable file symbols specified\n"
	"execution will proceed without symbol table. Use the \"file\"\n"
	"command to rectify this.\n");
    }

  /* The code won't actually start executing until it is unstalled in
     or1k_resume(), so we'll call target_mark_running() there. Tidy up from
     last time we were running. */
  init_wait_for_inferior ();

}	/* or1k_create_inferior () */


/*---------------------------------------------------------------------------*/
/*!Mourn the inferior process

   We don't atually want to unpush the target - we should be able to call run
   again to restart it. Use the generic mourn and mark it as exited (it it was
   running). */
/*---------------------------------------------------------------------------*/

static void
or1k_mourn_inferior ()
{
  generic_mourn_inferior ();

  if( target_has_execution )
    {
      target_mark_exited (&or1k_target);
    }
}	/* or1k_mourn_inferior () */


/*---------------------------------------------------------------------------*/
/*!Send a command to the remote target

   The target can't actually run any remote commands, in the sense of having a
   command interpreter. However we use this as a GDB "standard" way of
   wrapping up commands to access the SPRs.

   Current commands supported are:
   - readspr  <regno>
   - writespr <regno> <value>

   The output is either the value read (for successful read) or OK (for
   successful write) or E01 to indicate and wrror. All values are represented
   as unsigned hex.

   @param[in]  command  The command to execute
   @param[out] output   The result of the command                            */
/*---------------------------------------------------------------------------*/

static void  or1k_rcmd (char           *command,
			struct ui_file *outbuf)
{
  /* Sort out which command */
  if (0 == strncmp ("readspr", command, strlen ("readspr")))
    {
      unsigned int  regno;		/* SPR to read */

      if (1 != sscanf (command, "readspr %4x", &regno))
	{
	  error ("or1k_rcmd: unrecognized readspr");
	  fputs_unfiltered ("E01", outbuf);
	}
      else
	{
	  char      strbuf[sizeof ("ffffffff")];
	  ULONGEST  data = or1k_jtag_read_spr (regno);

	  sprintf (strbuf, "%8llx", (long long unsigned int)data);
	  fputs_unfiltered (strbuf, outbuf);
	}
    }
  else if (0 == strncmp ("writespr", command, strlen ("writespr")))
    {
      unsigned int  regno;		/* SPR to write */
      long long unsigned int      data;		/* Value to write */

      if (2 != sscanf (command, "writespr %4x %8llx", &regno, &data))
	{
	  error ("or1k_rcmd: unrecognized writespr");
	  fputs_unfiltered ("E01", outbuf);
	}
      else
	{
	  or1k_jtag_write_spr (regno, data);
	  fputs_unfiltered ("OK", outbuf);
	}
    }
  else
    {
      error ("or1k_rcmd: unrecognized remote command");
      fputs_unfiltered ("E01", outbuf);
    }
}	/* or1k_rcmd () */


/*---------------------------------------------------------------------------*/
/*!Main entry point for the remote OpenRISC JTAG protocol

   Initializes the target ops for each version of the remote
   protocol. Currently only the JTAG protocol is implemented.

   @note Earlier versions of this code provided for a "sim" and "dummy"
         target. However these never had any functionality, so have been
         deleted.

	 @return  New CRC */
/*---------------------------------------------------------------------------*/

  void
_initialize_remote_or1k()
{
  /* Set up the JTAG ops. */

  or1k_target.to_shortname                   = "jtag";
  or1k_target.to_longname                    =
    "OpenRISC 1000 debugging over JTAG port";
  or1k_target.to_doc                         =
    "Debug a board using the OpenRISC 1000 JTAG debugging protocol. The\n"
    "target may be connected directly via the parallel port, or may be a\n"
    "remote connection over TCP/IP to another machine, an instance of the\n"
    "Or1ksim architectural simulator, or physical hardware connected by USB\n"
    "with its own standalone control program.\n"
    "\n"
    "The argument is the parallel port device (e.g. /dev/jp) to which it is\n"
    "connected locally or a URL of the form jtag://<hostname>:<port> or\n"
    "jtag://<hostname>:<service> identifying the remote connection.";

  or1k_target.to_files_info                  = or1k_files_info;
  or1k_target.to_stratum                     = process_stratum;
  or1k_target.to_has_all_memory              = 1;
  or1k_target.to_has_memory                  = 1;
  or1k_target.to_has_stack                   = 1;
  or1k_target.to_has_registers               = 1;
  or1k_target.to_has_execution               = 0;
  or1k_target.to_have_steppable_watchpoint   = 0;
  or1k_target.to_have_continuable_watchpoint = 0;

  or1k_target.to_open                        = or1k_open;
  or1k_target.to_close                       = or1k_close;
  or1k_target.to_detach                      = or1k_detach;

  or1k_target.to_fetch_registers             = or1k_fetch_registers;
  or1k_target.to_store_registers             = or1k_store_registers;
  or1k_target.to_prepare_to_store            = or1k_prepare_to_store;
  or1k_target.to_load                        = generic_load;
  or1k_target.to_xfer_partial                = or1k_xfer_partial;

  or1k_target.to_insert_breakpoint           = or1k_insert_breakpoint;
  or1k_target.to_remove_breakpoint           = or1k_remove_breakpoint;
  or1k_target.to_can_use_hw_breakpoint       = or1k_can_use_hw_matchpoint;
  or1k_target.to_insert_hw_breakpoint        = or1k_insert_hw_breakpoint;
  or1k_target.to_remove_hw_breakpoint        = or1k_remove_hw_breakpoint;
  or1k_target.to_insert_watchpoint           = or1k_insert_watchpoint;
  or1k_target.to_remove_watchpoint           = or1k_remove_watchpoint;
  or1k_target.to_stopped_by_watchpoint       = or1k_stopped_by_watchpoint;
  or1k_target.to_stopped_data_address        = or1k_stopped_data_address;
  or1k_target.to_region_ok_for_hw_watchpoint = or1k_region_ok_for_hw_watchpoint;

  or1k_target.to_resume                      = or1k_resume;
  or1k_target.to_wait                        = or1k_wait; 
  or1k_target.to_stop                        = or1k_stop;
  or1k_target.to_kill                        = or1k_kill;
  or1k_target.to_create_inferior             = or1k_create_inferior;
  or1k_target.to_mourn_inferior              = or1k_mourn_inferior;

  or1k_target.to_rcmd                        = or1k_rcmd;

  or1k_target.to_magic                       = OPS_MAGIC;

  /* Tell GDB about the new target */

  add_target (&or1k_target);

}	/* _initialize_remote_or1k() */


/* EOF */
