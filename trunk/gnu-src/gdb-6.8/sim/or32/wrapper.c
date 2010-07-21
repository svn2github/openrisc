/* GDB Simulator wrapper for Or1ksim, the OpenRISC architectural simulator

   Copyright 1988-2008, Free Software Foundation, Inc.
   Copyright (C) 2010 Embecosm Limited

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

/*---------------------------------------------------------------------------*/
/* This is a wrapper for Or1ksim, suitable for use as a GDB simulator.

   The code tries to follow the GDB coding style.

   Commenting is Doxygen compatible.                                         */
/*---------------------------------------------------------------------------*/

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

#include "ansidecl.h"
#include "gdb/callback.h"
#include "gdb/remote-sim.h"
#include "sim-utils.h"
#include "targ-vals.h"

#include "or1ksim.h"
#include "or32sim.h"

/*!A global record of the simulator description */
static SIM_DESC  global_sd = NULL;

/*!The last reason we stopped */
enum sim_stop  last_reason = sim_running;

/*!The last return code from a resume/step call */
static unsigned long int  last_rc = 0;


/* ------------------------------------------------------------------------- */
/*!Create a fully initialized simulator instance.

   This function is called when the simulator is selected from the gdb command
   line.

   While the simulator configuration can be parameterized by (in decreasing
   precedence) argv's SIM-OPTION, argv's TARGET-PROGRAM and the abfd argument,
   the successful creation of the simulator shall not dependent on the
   presence of any of these arguments/options.

   For a hardware simulator the created simulator shall be sufficiently
   initialized to handle, without restrictions any client requests (including
   memory reads/writes, register fetch/stores and a resume).

   For a process simulator, the process is not created until a call to
   sim_create_inferior.

   This function creates a socket pair, which will be used to communicate to
   and from the process model, then forks off a process to run the SystemC
   model. Communication is via RSP packets across the socket pair, but with
   simplified hand-shaking.

   @note We seem to capable of being called twice. We use the static
         "global_sd" variable to keep track of this. Second and subsequent
         calls do nothing, but return the previously opened simulator
         description. 

   @param[in] kind  Specifies how the simulator shall be used.  Currently
                    there are only two kinds: stand-alone and debug.

   @param[in] callback  Specifies a standard host callback (defined in
                        callback.h).

   @param[in] abfd      When non NULL, designates a target program.  The
                        program is not loaded.

   @param[in] argv      A standard ARGV pointer such as that passed from the
                        command line.  The syntax of the argument list is is
                        assumed to be ``SIM-PROG { SIM-OPTION } [
                        TARGET-PROGRAM { TARGET-OPTION } ]''.

			The trailing TARGET-PROGRAM and args are only valid
                        for a stand-alone simulator.

			The argument list is null terminated!

   @return On success, the result is a non NULL descriptor that shall be
           passed to the other sim_foo functions.                            */
/* ------------------------------------------------------------------------- */
SIM_DESC
sim_open (SIM_OPEN_KIND                kind,
	  struct host_callback_struct *callback,
	  struct bfd                  *abfd,
	  char                        *argv[])
{
  int   argc;
  char *config_file;
  char *image_file;

  /* If the global_sd is currently set, we just return it. */
  if (NULL != global_sd)
    {
      return  global_sd;
    }

  /* Eventually we should use the option parser built into the GDB simulator
     (see common/sim-options.h). However since this is minimally documented,
     and we have only the one option, for now we do it ourselves. */

  /* Count the number of arguments */
  for (argc = 0; NULL != argv[argc]; argc++)
    {
      /* printf ("argv[%d] = %s\n", argc, argv[argc]); */
    }

  /* Configuration file may be passed using the -f <filename> */
  if ((argc > 2) && (0 == strcmp (argv[1], "-f")))
    {
      config_file = argv[2];
      image_file  = argc > 3 ? argv[3] : NULL;
    }
  else
    {
      config_file = NULL;
      image_file  = argc > 1 ? argv[1] : NULL;
    }

  /* Initialize the simulator. No class image nor upcalls */
  if (0 == or1ksim_init (config_file, image_file, NULL, NULL, NULL))
    {
      global_sd = (SIM_DESC) malloc (sizeof (*global_sd));

      global_sd->callback = callback;
      global_sd->is_debug = (kind == SIM_OPEN_DEBUG);
      global_sd->myname   = (char *)xstrdup (argv[0]);
    }

  return  global_sd;

}	/* sim_open () */


/* ------------------------------------------------------------------------- */
/*!Destroy a simulator instance.

   This may involve freeing target memory and closing any open files and
   mmap'd areas.

   We cannot assume sim_kill () has already been called.

   @param[in] sd        Simulation descriptor from sim_open ().
   @param[in] quitting  Non-zero if we cannot hang on errors.                */
/* ------------------------------------------------------------------------- */
void
sim_close (SIM_DESC  sd,
	   int       quitting)
{
  if (NULL != sd)
    {
      free (sd->myname);
      free (sd);
    }
}	/* sim_close () */


/* ------------------------------------------------------------------------- */
/*!Load program PROG into the simulators memory.

   Hardware simulator: Normally, each program section is written into
   memory according to that sections LMA using physical (direct)
   addressing.  The exception being systems, such as PPC/CHRP, which
   support more complicated program loaders.  A call to this function
   should not effect the state of the processor registers.  Multiple
   calls to this function are permitted and have an accumulative
   effect.

   Process simulator: Calls to this function may be ignored.

   @todo Most hardware simulators load the image at the VMA using
         virtual addressing.

   @todo For some hardware targets, before a loaded program can be executed,
         it requires the manipulation of VM registers and tables.  Such
         manipulation should probably (?) occure in sim_create_inferior ().

   @param[in] sd        Simulation descriptor from sim_open ().
   @param[in] prog      The name of the program
   @param[in] abfd      If non-NULL, the BFD for the file has already been
                        opened. 
   @param[in] from_tty  Not sure what this does. Probably indicates this is a
                        command line load? Anyway we don't use it.

   @return  A return code indicating success.                                */
/* ------------------------------------------------------------------------- */
SIM_RC
sim_load (SIM_DESC    sd,
	  char       *prog,
	  struct bfd *abfd,
	  int         from_tty)
{
  bfd *prog_bfd;

  /* Use the built in loader, which will in turn use our write function. */
  prog_bfd = sim_load_file (sd, sd->myname, sd->callback, prog, abfd,
			    sd->is_debug, 0, sim_write);

  if (NULL == prog_bfd)
    {
      return SIM_RC_FAIL;
    }

  /* If the BFD was not already open, then close the loaded program. */
  if (NULL == abfd)
    {
      bfd_close (prog_bfd);
    }

  return  SIM_RC_OK;

}	/* sim_load () */


/* ------------------------------------------------------------------------- */
/*!Prepare to run the simulated program.

   Hardware simulator: This function shall initialize the processor
   registers to a known value.  The program counter and possibly stack
   pointer shall be set using information obtained from ABFD (or
   hardware reset defaults).  ARGV and ENV, dependant on the target
   ABI, may be written to memory.

   Process simulator: After a call to this function, a new process
   instance shall exist. The TEXT, DATA, BSS and stack regions shall
   all be initialized, ARGV and ENV shall be written to process
   address space (according to the applicable ABI) and the program
   counter and stack pointer set accordingly.

   ABFD, if not NULL, provides initial processor state information.
   ARGV and ENV, if non NULL, are NULL terminated lists of pointers.

   @param[in] sd    Simulation descriptor from sim_open ().
   @param[in] abfd  If non-NULL provides initial processor state information.
   @param[in] argv  Vector of arguments to the program. We don't use this
   @param[in] env   Vector of environment data. We don't use this.

   @return  A return code indicating success.                                */
/* ------------------------------------------------------------------------- */
SIM_RC
sim_create_inferior (SIM_DESC     sd,
		     struct bfd  *abfd,
		     char       **argv  ATTRIBUTE_UNUSED,
		     char       **env   ATTRIBUTE_UNUSED)
{
  /* We set the starting PC if we have that data */
  unsigned long int  pc     = (NULL == abfd) ? 0 : bfd_get_start_address (abfd);
  unsigned char     *pc_ptr = (unsigned char *)(&pc);
  
  sim_store_register (sd, OR32_NPC_REGNUM, pc_ptr, sizeof (pc));
  return  SIM_RC_OK;

}	/* sim_create_inferior () */


/* ------------------------------------------------------------------------- */
/*!Fetch bytes from the simulated program's memory.

   @param[in]  sd   Simulation descriptor from sim_open (). We don't use
                    this.
   @param[in]  mem  The address in memory to fetch from.
   @param[out] buf  Where to put the read data
   @param[in]  len  Number of bytes to fetch

   @return  Number of bytes read, or zero if error.                          */
/* ------------------------------------------------------------------------- */
int
sim_read (SIM_DESC       sd  ATTRIBUTE_UNUSED,
	  SIM_ADDR       mem,
	  unsigned char *buf,
	  int            len)
{
  return  or1ksim_read_mem (buf, (unsigned int) mem, len);

}      /* sim_read () */


/* ------------------------------------------------------------------------- */
/*!Store bytes to the simulated program's memory.

   @param[in] sd   Simulation descriptor from sim_open (). We don't use
                   this.
   @param[in] mem  The address in memory to write to.
   @param[in] buf  The data to write
   @param[in] len  Number of bytes to write

   @return  Number of byte written, or zero if error.                        */
/* ------------------------------------------------------------------------- */
int
sim_write (SIM_DESC       sd  ATTRIBUTE_UNUSED,
	   SIM_ADDR       mem,
	   unsigned char *buf,
	   int            len)
{
  return  or1ksim_write_mem (buf, (unsigned int) mem, len);

}	/* sim_write () */


/* ------------------------------------------------------------------------- */
/*!Fetch a register from the simulation

   @param[in]  sd     Simulation descriptor from sim_open (). We don't use
                      this.
   @param[in]  regno  The register to fetch
   @param[out] buf    Buffer of length bytes to store the result. Data is
                      only transferred if length matches the register length
                      (the actual register size is still returned).
   @param[in]  len    Size of buf, which should match the size of the
                      register.

   @return  The actual size of the register, or zero if regno is not
            applicable. Legacy implementations return -1.
/* ------------------------------------------------------------------------- */
int
sim_fetch_register (SIM_DESC       sd  ATTRIBUTE_UNUSED,
		    int            regno,
		    unsigned char *buf,
		    int            len)
{
  return  or1ksim_read_reg (buf, regno, len);

}	/* sim_fetch_register () */


/* ------------------------------------------------------------------------- */
/*!Store a register to the simulation

   @param[in] sd     Simulation descriptor from sim_open (). We don't use
                     this.
   @param[in] regno  The register to store
   @param[in] buf    Buffer of length bytes with the data to store. Data is
                     only transferred if length matches the register length
                     (the actual register size is still returned).
   @param[in] len    Size of buf, which should match the size of the
                     register.

   @return  The actual size of the register, or zero if regno is not
            applicable. Legacy implementations return -1.
/* ------------------------------------------------------------------------- */
int
sim_store_register (SIM_DESC       sd  ATTRIBUTE_UNUSED,
		    int            regno,
		    unsigned char *buf,
		    int            len)
{
  return  or1ksim_write_reg (buf, regno, len);

}	/* sim_store_register () */


/* ------------------------------------------------------------------------- */
/* Print whatever statistics the simulator has collected.

   @param[in] sd       Simulation descriptor from sim_open (). We don't use
                       this.
   @param[in] verbose  Currently unused, and should always be zero.          */
/* ------------------------------------------------------------------------- */
void
sim_info (SIM_DESC  sd       ATTRIBUTE_UNUSED,
	  int       verbose  ATTRIBUTE_UNUSED)
{
}	/* sim_info () */


/* ------------------------------------------------------------------------- */
/*!Run (or resume) the simulated program.

   Hardware simulator: If the SIGRC value returned by
   sim_stop_reason() is passed back to the simulator via siggnal then
   the hardware simulator shall correctly deliver the hardware event
   indicated by that signal.  If a value of zero is passed in then the
   simulation will continue as if there were no outstanding signal.
   The effect of any other siggnal value is is implementation
   dependant.

   Process simulator: If SIGRC is non-zero then the corresponding
   signal is delivered to the simulated program and execution is then
   continued.  A zero SIGRC value indicates that the program should
   continue as normal.

   @param[in] sd       Simulation descriptor from sim_open ().
   @param[in] step     When non-zero indicates that only a single simulator
                       cycle should be emulated.
   @param[in] siggnal  If non-zero is a (HOST) SIGRC value indicating the type
                       of event (hardware interrupt, signal) to be delivered
                       to the simulated program.                             */
/* ------------------------------------------------------------------------- */
void
sim_resume (SIM_DESC  sd,
	    int       step,
	    int       siggnal)
{
  int  res;

  res = step ? or1ksim_step () : or1ksim_run (-1.0);

  switch (res)
    {
    case OR1KSIM_RC_HALTED:
      last_reason = sim_exited;
      (void) sim_fetch_register (sd, OR32_FIRST_ARG_REGNUM,
				 (unsigned char *) &last_rc, 4);
      break;

    case OR1KSIM_RC_BRKPT:
      last_reason = sim_stopped;
      last_rc     = TARGET_SIGNAL_TRAP;
      break;
    }
}	/* sim_resume () */


/* ------------------------------------------------------------------------- */
/*!Asynchronous request to stop the simulation.

   @param[in] sd  Simulation descriptor from sim_open (). We don't use this.

   @return  Non-zero indicates that the simulator is able to handle the
            request.                                                         */ 
/* ------------------------------------------------------------------------- */
int sim_stop (SIM_DESC  sd  ATTRIBUTE_UNUSED)
{
  return  0;			/* We don't support this */

}	/* sim_stop () */


/* ------------------------------------------------------------------------- */
/*!Fetch the REASON why the program stopped.

   The reason enumeration indicates why:

   - sim_exited:    The program has terminated. sigrc indicates the target
                    dependant exit status.

   - sim_stopped:   The program has stopped.  sigrc uses the host's signal
                    numbering as a way of identifying the reaon: program
                    interrupted by user via a sim_stop request (SIGINT); a
                    breakpoint instruction (SIGTRAP); a completed single step
                    (SIGTRAP); an internal error condition (SIGABRT); an
                    illegal instruction (SIGILL); Access to an undefined
                    memory region (SIGSEGV); Mis-aligned memory access
                    (SIGBUS).

		    For some signals information in addition to the signal
                    number may be retained by the simulator (e.g. offending
                    address), that information is not directly accessable via
                    this interface.

   - sim_signalled: The program has been terminated by a signal. The
                    simulator has encountered target code that causes the the
                    program to exit with signal sigrc.

   - sim_running:
   - sim_polling:   The return of one of these values indicates a problem
                    internal to the simulator.

   @param[in]  sd      Simulation descriptor from sim_open (). We don't use
                       this.
   @param[out] reason  The reason for stopping
   @param[out] sigrc   Supplementary information for some values of reason.  */
/* ------------------------------------------------------------------------- */
void
sim_stop_reason (SIM_DESC       sd      ATTRIBUTE_UNUSED,
		 enum sim_stop *reason,
		 int           *sigrc)
 {
   *reason = last_reason;
   *sigrc  = last_rc;

}	/* sim_stop_reason () */


/* ------------------------------------------------------------------------- */
/* Passthru for other commands that the simulator might support.

   Simulators should be prepared to deal with any combination of NULL
   or empty command.

   This implementation currently does nothing.

   @param[in] sd  Simulation descriptor from sim_open (). We don't use this.
   @param[in] cmd The command to pass through.                               */
/* ------------------------------------------------------------------------- */
void
sim_do_command (SIM_DESC  sd   ATTRIBUTE_UNUSED,
		char     *cmd  ATTRIBUTE_UNUSED)
{
}	/* sim_do_command () */


/* ------------------------------------------------------------------------- */
/* Set the default host_callback_struct

   @note Deprecated, but implemented, since it is still required for linking.

   This implementation currently does nothing.

   @param[in] ptr  The host_callback_struct pointer. Unused here.            */
/* ------------------------------------------------------------------------- */
void
sim_set_callbacks (struct host_callback_struct *ptr ATTRIBUTE_UNUSED)
{
}	/* sim_set_callbacks () */


/* ------------------------------------------------------------------------- */
/* Set the size of the simulator memory array.

   @note Deprecated, but implemented, since it is still required for linking.

   This implementation currently does nothing.

   @param[in] size  The memory size to use. Unused here.                     */
/* ------------------------------------------------------------------------- */
void
sim_size (int  size  ATTRIBUTE_UNUSED)
{
}	/* sim_size () */


/* ------------------------------------------------------------------------- */
/* Single step the simulator with tracing enabled.

   @note Deprecated, but implemented, since it is still required for linking.

   This implementation currently does nothing.

   @param[in] sd  The simulator description struct. Unused here.             */
/* ------------------------------------------------------------------------- */
void
sim_trace (SIM_DESC  sd  ATTRIBUTE_UNUSED)
{
}	/* sim_trace () */
