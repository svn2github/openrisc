/* Remote debugging interface for JTAG debugging protocol.
   JTAG connects to or1k target ops. See or1k-tdep.c

   Copyright 1993-1995, 2000 Free Software Foundation, Inc.
   Copyright 2008 Embecosm Limited

   Contributed by Cygnus Support.  Written by Marko Mlinar
   <markom@opencores.org>
   Areas noted by (CZ) were modified by Chris Ziomkowski <chris@asics.ws>
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

/*---------------------------------------------------------------------------*/
/*!Updated for GDB 6.8 by Jeremy Bennett. All code converted to ANSI C style
   and in general to GDB format. All global OpenRISC specific functions and
   variables should now have a prefix of or1k_ or OR1K_.

   The coding for the local JTAG connections assumes a Xilinx JP1 port
   connecting via the local parallel port. Constants and routines processing
   these have the prefix jp1_ or JP1_. Remote connections use the OpenRISC
   remote JTAG protocol.

   The interface is layered

   The highest level is the public functions, which operate in terms of
   entities that are visible in GDB: open & close the connection, read and
   write SPRs, read and write memory, stall and unstall the
   processor. These functions always succeed Function prefixes: or1k_jtag_

   The next level is the abstraction provided by the OR1K Remote JTAG
   protocol: read/write a JTAG register, read/write a block of JTAG registers
   and select a scan chain. These functions may encounter errors and will deal
   with them, but otherwise return no error result. Static function prefixes:
   or1k_jtag_

   The next level is the two sets corresponding to the remote JTAG protocol,
   one for use with a locally connected (JP1) JTAG and one for a remote
   connection. These functions deal with errors and return an error code to
   indicate an error has occurred. Static function prefixes: jp1_ and jtr_
   respectively.

   The final level comes in separate flavours for local use (low level
   routines to drive JTAG) and remote use (to build and send/receive
   packets). These functions deal with errors and return an error code to
   indicate an error has occurred. Static function prefixes: jp1_ll_ and
   jtr_ll_ respectively.

   Errors are either dealt with silently or (if fatal) via the GDB error()
   function.

   @note Few people use the JP1 direct connection, and there is no
         confidence that this code works at all!

	 Commenting compatible with Doxygen added throughout. */
/*---------------------------------------------------------------------------*/


#include "defs.h"
#include "inferior.h"
#include "bfd.h"
#include "symfile.h"
#include "gdb_wait.h"
#include "gdbcmd.h"
#include "gdbcore.h"
#include "serial.h"
#include "target.h"
#include "gdb_string.h"
#include "or1k-tdep.h"
#include "or1k-jtag.h"

/* Added by CZ 24/05/01 */
#include <sys/poll.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/select.h>
#include <sys/time.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include <inttypes.h>

/* Wait times (us) */
#define OR1K_JTAG_FAST_WAIT     1	/*!< Wait us when single stepping */
#define OR1K_JTAG_SLOW_WAIT  1000	/*!< Wait us when not single stepping */

/* Bit numbers in packets for the JP1 JTAG pins */
#define JP1_TCK   0x01		/*!< JTAG TCK pin */
#define JP1_TRST  0x02		/*!< JTAG TRST pin */
#define JP1_TMS   0x04		/*!< JTAG TMS pin */
#define JP1_TDI   0x08		/*!< JTAG TDI pin */
#define JP1_TDO   0x80		/*!< JTAG TDO pin */

#define JP1_WAIT()
#define JP1_RETRY_WAIT()  (usleep (100))

#define JP1_NUM_RETRIES   16

/*! Selects crc trailer size in bits. Currently supported: 8 */
#define JP1_CRC_SIZE       8

static int  jp1_crc_r;			/*!< CRC of current read data */
static int  jp1_crc_w = 0;		/*!< CRC of current written data */

/*! Designates whether we are in SELECT_DR state */
static int or1k_select_dr = 0;

/*! Scan chain info. */
static int  jp1_chain_addr_size[] = { 0,  32, 0,   0,  5 };
static int  jp1_chain_data_size[] = { 0,  32, 0,  32, 32 };
static int  jp1_chain_is_valid[]  = { 0,   1, 0,   1,  1 };
static int  jp1_chain_has_crc[]   = { 0,   1, 0,   1,  1 };
static int  jp1_chain_has_rw[]    = { 0,   1, 0,   0,  1 };

/*! Currently selected scan chain - just to prevent unnecessary transfers. */
static int  or1k_jtag_current_chain;

/*! Information about the JTAG connection, if any */
struct {
  union {
    int lp;		/* Printer compatible device we have open.  */
    int fd;		/* Socket for remote or1k jtag interface */
  } device;
  enum {
    OR1K_JTAG_NOT_CONNECTED,
    OR1K_JTAG_LOCAL,
    OR1K_JTAG_REMOTE
  } location;
} or1k_jtag_connection;


/*! Global variable identifying the debug interface version. Assume
    consistency with GDB 5.3, unless otherwise corrected. */
enum or1k_dbg_if_version_enum  or1k_dbg_if_version = OR1K_DBG_IF_ORPSOC;

/* Forward declarations of the public interface functions */

void      or1k_jtag_init (char *args);
void      or1k_jtag_close ();
ULONGEST  or1k_jtag_read_spr (unsigned int  sprnum);
void      or1k_jtag_write_spr (unsigned int  sprnum,
			       ULONGEST      data);
int       or1k_jtag_read_mem (CORE_ADDR  addr,
			      gdb_byte  *bdata,
			      int        len);
int       or1k_jtag_write_mem (CORE_ADDR       addr,
			       const gdb_byte *bdata,
			       int             len);
void      or1k_jtag_stall ();
void      or1k_jtag_unstall ();
void      or1k_jtag_wait (int  fast);

/* Forward declarations for the low level JP1 routines. */

static int            jp1_ll_crc_calc (int  crc,
				       int  input_bit);
static void           jp1_ll_reset_jp1 ();
static unsigned char  jp1_ll_read_jp1 ();
static void           jp1_ll_write_jp1 (unsigned char  tms,
					unsigned char  tdi);
static ULONGEST       jp1_ll_read_stream (ULONGEST  stream,
					  int       len,
					  int       set_last_bit);
static void           jp1_ll_write_stream (ULONGEST  stream,
					   int       len,
					   int       set_last_bit);
static void           jp1_ll_prepare_control ();
static void           jp1_ll_prepare_data ();

/* Forward declarations of low level support functions for remote OR1K remote
   JTAG server. */

static int                    jtr_ll_connect (char *hostname,
					      char *port_or_service);
static void                   jtr_ll_close ();
static enum or1k_jtag_errors  jtr_ll_check (enum or1k_jtag_errors  result);
static enum or1k_jtag_errors  jtr_ll_read (int   fd,
					   void *buf,
					   int   len);
static enum or1k_jtag_errors  jtr_ll_write (int   fd,
					    void *buf,
					    int   len);
static enum or1k_jtag_errors  jtr_ll_response (int   fd,
					       void *resp_buf,
					       int   len);

/* Forward declarations for OR1K JTAG protocol functions for use with a local
   JP1 connection. */

static enum or1k_jtag_errors  jp1_read_jtag_reg (unsigned int  regnum,
						 ULONGEST     *data);
static enum or1k_jtag_errors  jp1_write_jtag_reg (unsigned int  regnum,
						  ULONGEST      data);
static enum or1k_jtag_errors  jp1_read_jtag_block (unsigned int  regnum,
						   uint32_t     *bdata,
						   int           count);
static enum or1k_jtag_errors  jp1_write_jtag_block (unsigned int    regnum,
						    const uint32_t *bdata,
						    int             count);
static enum or1k_jtag_errors  jp1_select_chain (int  chain);

/* Forward declarations for OR1K JTAG protocol functions for use with a remote
   JTAG server. */

static enum or1k_jtag_errors  jtr_read_jtag_reg (unsigned int  regnum,
						 ULONGEST     *data);
static enum or1k_jtag_errors  jtr_write_jtag_reg (unsigned long int  regnum,
						  ULONGEST           data);
static enum or1k_jtag_errors  jtr_read_jtag_block (CORE_ADDR  regnum,
						   uint32_t  *bdata,
						   int        count);
static enum or1k_jtag_errors  jtr_write_jtag_block (CORE_ADDR     regnum,
						    const uint32_t *bdata,
						    int             count);
static enum or1k_jtag_errors  jtr_select_chain (int  chain);

/* Forward declarations for OR1K JTAG protocol functions for use with either a
   local connection or remote server */

static void        or1k_jtag_read_jtag_reg (unsigned int  regnum,
					    ULONGEST     *data);
static void        or1k_jtag_write_jtag_reg (unsigned int  regnum,
					     ULONGEST      data);
static void        or1k_jtag_read_jtag_block (unsigned int  regnum,
					      uint32_t     *bdata,
					      int           count);
static void        or1k_jtag_write_jtag_block (unsigned int    regnum,
					       const uint32_t *bdata,
					       int             count);
static void        or1k_jtag_select_chain (int  chain);
static void        or1k_jtag_reset ();
static const char *or1k_jtag_err_name (enum or1k_jtag_errors  e);



/* These are the low level OpenRISC 1000 JTAG Protocol functions for use with
   a local JTAG connection. All these functions are static and have the prefix
   jp1_ll_ */

/*----------------------------------------------------------------------------*/
/*!Generates new crc, sending in new bit input_bit

   @note  Only actually calculates anything if the CRC size is 8.

   @param[in] crc        Current CRC
   @param[in] input_bit  New bit to incorporate

   @return  New CRC */
/*---------------------------------------------------------------------------*/

static int
jp1_ll_crc_calc (int  crc,
		 int  input_bit)
{
  int c;
  int new_crc;
  int d;

#if (JP1_CRC_SIZE == 8)
  d = input_bit&1;
  c = crc;

  /* Move queue left.  */
  new_crc = crc << 1;

  /* Mask upper five bits.  */
  new_crc &= 0xF8;

  /* Set lower three bits */
  new_crc |= (d ^ ((c >> 7) & 1));
  new_crc |= (d ^ ((c >> 0) & 1) ^ ((c >> 7) & 1)) << 1;
  new_crc |= (d ^ ((c >> 1) & 1) ^ ((c >> 7) & 1)) << 2;

  return new_crc;
#else
  return 0;
#endif
}


/*----------------------------------------------------------------------------*/
/*!Resets the local JTAG via JP1

   Only works if this is a local connection

   Writes:
   @verbatim
     TCK=0 TRST=0 TMS=0 TDI=0
     TCK=0 TRST=1 TMS=0 TDI=0
   @endverbatim
 
   JTAG reset is active low */
/*---------------------------------------------------------------------------*/

static void
jp1_ll_reset_jp1()
{
  unsigned char  data;
  int            lp = or1k_jtag_connection.device.lp;         /* CZ */

  if (OR1K_JTAG_LOCAL != or1k_jtag_connection.location)
    {
      error ("jp1_ll_reset_jp1 called without a local connection!");
    }

  data = 0;
  write (lp, &data, sizeof (data));
  JP1_WAIT ();

  data = JP1_TRST;
  write (lp, &data, sizeof (data) );
  JP1_WAIT ();

}	/* jp1_ll_reset_jp1() */


/*----------------------------------------------------------------------------*/
/*!Gets TD0 from JP1

   This is done using ioctl().

   Rewritten by Jeremy Bennett to work whatever the TMS and TDI bit positions
   in JP1.

   @todo I really do not believe this can work. ioctl() expects a FD
         (presumably or1k_jtag_connection.device.lp). A read() call would be the
         more usual way to actually obtain data, given the data is set using
         a write() call.

   Writes:
   @verbatim
     TCK=0 TRST=1 TMS=bit0 TDI=bit1
     TCK=1 TRST=1 TMS=bit0 TDI=bit1
   @endverbatim

   @return  Bit 0 is the value of TD0. */
/*---------------------------------------------------------------------------*/

static unsigned char
jp1_ll_read_jp1()
{
  int data;

  if (OR1K_JTAG_LOCAL != or1k_jtag_connection.location)
    {   /* CZ */
      error ("jp1_ll_read_jp1 called without a local connection!");
    }

  ioctl (data, 0x60b, &data);		/* Really!!!!! */
  data = ((data & JP1_TDO) != 0);

  /* Update the CRC for read */
  jp1_crc_r = jp1_ll_crc_calc (jp1_crc_r, data);
  return data;

}	/* jp1_ll_read_jp1() */


/*----------------------------------------------------------------------------*/
/*!Clocks a bit into the local JTAG via JP1

   Takes TMS and TDI values from the bottom two bits of the argument

   Rewritten by Jeremy Bennett to work whatever the TMS and TDI bit positions
   in JP1.

   Writes:
   @verbatim
     TCK=0 TRST=1 TMS=tms TDI=tdi
     TCK=1 TRST=1 TMS=tms TDI=tdi
   @endverbatim

   @param[in] tms  The JTAG TMS pin to be clocked in
   @param[in] tdi  The JTAG TDI pin to be clocked in */
/*---------------------------------------------------------------------------*/

static void
jp1_ll_write_jp1 (unsigned char  tms,
		  unsigned char  tdi)
{
  unsigned char  data;
  int            lp = or1k_jtag_connection.device.lp;    /* CZ */

  if (OR1K_JTAG_LOCAL != or1k_jtag_connection.location)
    {
      error ("jp1_ll_write_jp1 called without a local connection!");
    }

  data = (tms ? JP1_TDI : 0) |
         (tdi ? JP1_TMS : 0) |
          JP1_TRST;
  write (lp, &data, sizeof (data) );
  JP1_WAIT ();
 
  /* rise clock */
  data |= JP1_TCK;
  write (lp, &data, sizeof (data) );
  JP1_WAIT ();

  /* Update the CRC for this TDI bit */
  jp1_crc_w = jp1_ll_crc_calc (jp1_crc_w, tdi ? 1 : 0);

}	/* jp1_write_jtag() */


/*----------------------------------------------------------------------------*/
/*!Read a stream of bits from TDO whilst also writing TDI and optionally TMS

   Reads a bitstream of 1 or more bits from TDO, MS bit first whilst writing
   a bitstream of the same length LS bit first. Optionally (if
   set_last_bit is 1) writes the MS bit with TMS also set.

   @todo  The expression precedence in the original setting of TMS bit seemed
          in error (due to a misunderstanding of the relative precedence of +
          and <<. The changed expression should be correct, but needs testing.

   @note If len is > sizeof (ULONGEST), then then only 0's are written for
         all except the MS sizeof (ULONGEST)  bits of stream.

   @param[in] stream        string of bits to write to TDI
   @param[in] len           number of bits to write
   @param[in] set_last_bit  if true also set TMS bit with last TDI bit.

   @return  Bitstream read MS bit first from TDO. */
/*---------------------------------------------------------------------------*/

static ULONGEST
jp1_ll_read_stream (ULONGEST  stream,
		    int       len,
		    int       set_last_bit)
{
  int i;
  ULONGEST data = 0;		/* The resulting stream */

  /* Do nothing unless len is positive */
  if (len <= 0)
    {
      return  (ULONGEST)0;
    }

  /* Read all but last bit MS bit first while writing stream LS bit first */
  for (i = 0; i < len-1; i++)
    {     
      jp1_ll_write_jp1 (0, stream & 1);
      stream >>= 1;
      data   <<= 1;
      data    |= jp1_ll_read_jp1 ();
    }
 
  /* Last bit optionally sets TMS as well */
  jp1_ll_write_jp1 (set_last_bit, stream & 1);
  data <<= 1;
  data  |= jp1_ll_read_jp1 ();

  return data;

}	/* jp1_ll_read_stream() */


/*----------------------------------------------------------------------------*/
/*!Write a stream of bits to TDI and optionally TMS

   Writes bitstream of 1 or more bits to TDI, MS bit first. Optionally (if
   set_last_bit is 1) writes the last bit with TMS also set.

   @note If len is > sizeof (ULONGEST), then then only 0's are written for
         all the bits > sizeof (ULONGEST)

   @param[in] stream        string of bits to write to TDI
   @param[in] len           number of bits to write
   @param[in] set_last_bit  if true also set TMS bit with last TDI bit. */
/*---------------------------------------------------------------------------*/

static void
jp1_ll_write_stream (ULONGEST  stream,
		     int       len,
		     int       set_last_bit)
{
  int i;

  /* Do nothing if len is not positive */
  if (len <= 0)
    {
      return;
    }

  /* All but last bit to TDI only */
  for (i = len - 1; i > 0; i--)
    {
      jp1_ll_write_jp1 (0, (stream >> i) & 1);
    }

  /* Last bit optionally sets TMS as well */
  jp1_ll_write_jp1 (set_last_bit, stream & 1);

}	/* jp1_ll_write_stream() */


/*----------------------------------------------------------------------------*/
/*!Force JTAG SELECT_IR state

   Should be called before every control write.

   If we are not in SELECT_DR, we must be in RUN TEST/IDLE, so step the state
   to SELECT_DR. One more step takes us to SELECT_IR. Set the or1k_select_dr
   flag to false. */
/*---------------------------------------------------------------------------*/

static void
jp1_ll_prepare_control()
{
  if (!or1k_select_dr)
    {
      jp1_ll_write_jp1 (1, 0);	/* SELECT_DR SCAN */
    }

  jp1_ll_write_jp1 (1, 0);	/* SELECT_IR SCAN */
  or1k_select_dr = 0;

}	/* jp1_ll_prepare_control() */


/*----------------------------------------------------------------------------*/
/*!Force JTAG SELECT_DR state

   If we are not in SELECT_DR, we must be in RUN TEST/IDLE, so step the state
   to SELECT_DR. */
/*---------------------------------------------------------------------------*/

static void
jp1_ll_prepare_data()
{
  if (!or1k_select_dr)
    {
      jp1_ll_write_jp1 (0, 1);	/* SELECT_DR SCAN */
    }

  or1k_select_dr = 1;

}	/* jp1_ll_prepare_data() */



/* These are the low level OpenRISC 1000 JTAG Protocol functions for use with
   a remote JTAG server. All these functions are static and have the prefix
   jtr_ll_ */

/*----------------------------------------------------------------------------*/
/*!Connect to a remote OpenRISC 1000 JTAG server.

   Added by CZ 24/05/01

   Makes the connection as a socket and returns the file descriptor for the
   socket.

   @param[in]  hostname         The host to connect to
   @param[in]  port_or_service  Port or service to connect to

   @return  A file descriptor for the new socket or 0 on error. */
/*---------------------------------------------------------------------------*/

static int
jtr_ll_connect (char *hostname,
		char *port_or_service)
{
  struct hostent     *host;
  struct sockaddr_in  sin;
  struct servent     *service;
  struct protoent    *protocol;

  int       sock;
  long int  flags;

  char *endptr;

  const char *proto_name = "tcp";

  int   port   = 0;
  int   on_off = 0; /* Turn off Nagel's algorithm on the socket */

  protocol = getprotobyname (proto_name);
  if (NULL == protocol)
    {
      error ("jtr_ll_connect: Protocol \"%s\" not available.\n",
	     proto_name);
      return  0;
    }

  /* Convert port_or_service to an integer only if it is a well formatted
     decimal number (i.e. the end pointer is the null char at the end of the
     string.  Otherwise, assume that it is a service name. */

  port = strtol (port_or_service, &endptr, 10);
  if ('\0' != endptr[0])
    {
      port = 0;

      /* Not a port, see if its a service */
      service  = getservbyname (port_or_service, protocol->p_name);
      if (NULL == service)
	{
	  error ("jtr_ll_connect: Unknown service \"%s\".\n",
		 port_or_service);
	  return 0;
	}
      else
	{
	  port = ntohs (service->s_port);
	}
    }

  /* Try to find the host */
  host = gethostbyname (hostname);
  if (NULL == host)
    {
      error ("jtr_ll_connect: Unknown host \"%s\"\n", hostname);
      return 0;
    }

  /* Open a socket using IP4 stream (i.e. TCP/IP) connection and make it
     blocking. */
  sock = socket (PF_INET, SOCK_STREAM, 0);
  if (sock < 0)
    {
      error ("jtr_ll_connect: can't create socket, errno = %d (%s)\n",
	     errno ,strerror (errno) );
      return 0;
    }

  flags = fcntl (sock, F_GETFL, 0);
  if (flags < 0)
    {
      error ("jtr_ll_connect: can't get flags, errno = %d (%s)\n",
	     errno, strerror (errno) );
      close (sock);
      return 0;
    }
 
  flags &= ~O_NONBLOCK;
  if (fcntl (sock, F_SETFL, flags)  < 0)
    {
      error ("jtr_ll_connect: can't set flags %lx, errno = %d (%s)\n",
	     flags, errno, strerror (errno) );
      close (sock);
      return 0;
    }

  /* Try to open a connection to the remote end */
  memset (&sin, 0, sizeof (sin) );
  sin.sin_family = host->h_addrtype;
  sin.sin_port   = htons (port);
  memcpy (&sin.sin_addr, host->h_addr_list[0], host->h_length);

  /* Old version of this allowed EINPROGRESS, but did nothing about following
     it up and in any case, that should not be possible with a blocking
     connection. */
  if (connect (sock, (struct sockaddr *)&sin, sizeof (sin) ) < 0)
    {
      error ("jtr_ll_connect: connect failed  errno = %d (%s)\n",
	     errno, strerror (errno) );
      close (sock);
      return 0;
    }

  /* Turn the socket back to non-blocking */
  flags |= O_NONBLOCK;
  if (fcntl (sock, F_SETFL, flags)  < 0)
    {
      error ("jtr_ll_connect: can't set flags %lx, errno = %d (%s)\n",
	     flags, errno, strerror (errno) );
      close (sock);
      return 0;
    }

  /* Turn off Nagle's algorithm, i.e. send data immediately, don't bottle it
     up into useful sized chunks. */
  if (setsockopt (sock, protocol->p_proto, TCP_NODELAY, &on_off,
		  sizeof(int)) < 0)
    {
      error ("jtr_ll_connect: "
	     "can't disable Nagel's algorithm, errno %d (%s)\n", errno,
	     strerror (errno) );
      close(sock);
      return 0;
    }

  return sock;

}	/* jtr_ll_connect() */


/*----------------------------------------------------------------------------*/
/*!Close down an OpenRISC 1000 JTAG remote connection

   Closes the connection without waiting for any queued messages on the
   socket.

   In this version the remote socket FD is always used and the result is
   discarded, since it is never used by the calling function. */
/*---------------------------------------------------------------------------*/

static void
jtr_ll_close()
{
  int            flags;
  struct linger  linger;

  /* First, make sure we're non blocking */
  flags = fcntl (or1k_jtag_connection.device.fd, F_GETFL, 0);
  if (flags < 0)
    {
      error ("jtr_ll_close: can't get flags errno %d (%s)\n",
	     errno, strerror (errno) );
    }

  flags &= ~O_NONBLOCK;
  if (fcntl (or1k_jtag_connection.device.fd, F_SETFL, flags)  < 0)
    {
      error ("jtr_ll_close: can't set flags to 0x%x, errno %d (%s)\n",
	     flags, errno, strerror (errno) );
    }
 
  /* Now, make sure we don't linger around */
  linger.l_onoff  = 0;
  linger.l_linger = 0;

  if (setsockopt (or1k_jtag_connection.device.fd, SOL_SOCKET, SO_LINGER,
		  &linger, sizeof (linger) ) < 0)
    {
      error ("jtr_ll_close: can't disable SO_LINGER, errno %d (%s)",
	     errno, strerror (errno) );
    }

  (void)close (or1k_jtag_connection.device.fd);

  or1k_jtag_connection.device.fd = 0;
  or1k_jtag_connection.location  = OR1K_JTAG_NOT_CONNECTED;

}	/* jtr_ll_close() */


/*----------------------------------------------------------------------------*/
/*!Close down a remote connection if the server has terminated

   Utility function which closes the remote connection if the error result
   OR1K_JTAG_ERR_SERVER_TERMINATED has been received.

   @param[in]  result  Result code to check

   @return  The result passed as argument (for convenience) */
/*---------------------------------------------------------------------------*/

static enum or1k_jtag_errors
jtr_ll_check (enum or1k_jtag_errors  result)
{
  if (OR1K_JTAG_ERR_SERVER_TERMINATED == result)
    {
      jtr_ll_close ();
    }

  return result;
}	/* jtr_ll_check() */


/*----------------------------------------------------------------------------*/
/*!Read from a remote OpenRISC 1000 JTAG connection

   Added by CZ 24/05/01. Modified by Jeremy Bennett.

   The read should always be blocking, so if we get a result indicating a
   non-blocking FD, we simulate the blocking behavior using poll(). May take
   several reads to get the whole buffer in.

   @param[in]  fd   File descriptor for remote connection
   @param[out] buf  Buffer for data that is read
   @param[in]  len  Number of bytes to read

   @return  system error (positive) or OR1K JTAG error code */
/*---------------------------------------------------------------------------*/

static enum or1k_jtag_errors
jtr_ll_read (int   fd,
	     void *buf,
	     int   len)
{
  char* r_buf = (char*)buf;

  while (len > 0)
    {
      int  n = read (fd, r_buf, len);

      if (n < 0)
	{
	  struct pollfd block;

	  switch(errno)
	    {
	    case EWOULDBLOCK: /* or EAGAIN */
	      /* We've been called on a descriptor marked for nonblocking
		 I/O. We'd better simulate blocking behavior. */
	      block.fd = fd;
	      block.events = POLLIN;
	      block.revents = 0;

	      poll(&block,1,-1);
	      continue;

	    case EINTR: continue;
	    default:    return  errno;
	    }
	}
      else if (0 == n)
	{
	  /* Zero indicates EOF. */
	  return OR1K_JTAG_ERR_SERVER_TERMINATED;
	}
      else
	{
	  len   -= n;
	  r_buf += n;
	}
    }

  return  OR1K_JTAG_ERR_NONE;

}	/* jtr_ll_read() */


/*----------------------------------------------------------------------------*/
/*!Write to a remote OpenRISC 1000 JTAG connection

   Added by CZ 24/05/01. Modified by Jeremy Bennett.

   The write should always be blocking, so if we get a result indicating a
   non-blocking FD, we simulate the blocking behavior using poll(). May take
   several writes to get the whole buffer out.

   @param[in] fd   File descriptor for remote connection
   @param[in] buf  Buffer of data to be written
   @param[in] len  Number of bytes to write

   @return  system error (positive) or OR1K JTAG error code */
/*---------------------------------------------------------------------------*/

static enum or1k_jtag_errors
jtr_ll_write (int   fd,
	   void *buf,
	   int   len)
{
  char *w_buf = (char*)buf;	/* Pointer to buffer left to write */

  while (len > 0)
    {
      int            n = write (fd, w_buf, len);

      if (n < 0)
	{
	  struct pollfd  block;

	  switch (errno)
	    {
	    case EWOULDBLOCK: /* or EAGAIN */
	      /* We've been called on a descriptor marked for nonblocking
		 I/O. We'd better simulate blocking behavior. */
	      block.fd = fd;
	      block.events = POLLOUT;
	      block.revents = 0;

	      poll (&block, 1, -1);		/* Wait forever for output */
	      continue;

	    case EINTR: continue;
	    case EPIPE: return  OR1K_JTAG_ERR_SERVER_TERMINATED;
	    default:    return  errno;
	    }
	}
      else
	{
	  len   -= n;
	  w_buf += n;
	}
    }

  return  OR1K_JTAG_ERR_NONE;

}	/* jtr_ll_write() */


/*----------------------------------------------------------------------------*/
/*!Fetch a remote OpenRISC 1000 JTAG response

   All OR1K reponse structs start with a uint32_t status field. If a response
   is an error response this is all that is set.

   If the response is not an error, then there may be further bytes to
   get. The total number to get is gives by len.

   All data in the response is in raw network format. A bug has been fixed
   which converted the status to host format in the buffer (that should be
   done by the caller).

   All errors are not logged using the GDB logging function, not just plain
   printf.

   @param[in]  fd   File descriptor for remote connection
   @param[out] resp_buf  Buffer for the response
   @param[in]  len       Size of the response buffer in bytes if this is a
                         successful response.

			 @return  system error (positive) or OR1K JTAG error code */
/*---------------------------------------------------------------------------*/

static enum or1k_jtag_errors
jtr_ll_response (int   fd,
	      void *resp_buf,
	      int   len)
{
  long int         status;
  enum or1k_jtag_errors  result;

  /* Get the status and blow out for any failure */
  result = jtr_ll_read (fd, resp_buf, sizeof (status) );
  if (OR1K_JTAG_ERR_NONE != result)
    {
      return  jtr_ll_check (result);
    }

  status = ((struct jtr_failure_response *)resp_buf)->status;
  status = ntohl (status);

  if (OR1K_JTAG_ERR_NONE != status)
    {
      return  jtr_ll_check (status);
    }

  /* Get the rest of a good result. Note this works even if there is no more
     to get (i.e. len=4) */
  result = jtr_ll_read (fd, &(((char *)resp_buf)[sizeof (status) ]),
				 len - sizeof (status) );
  return  jtr_ll_check (result);

}	/* jtr_ll_response() */



/* These are the OpenRISC 1000 JTAG Protocol routines for use with a local JP1
   connection. They are built on tol of the remote protocol low level helper
   functions. All these functions are static and have the prefix jp1_ */


/*----------------------------------------------------------------------------*/
/*!Read a JTAG register from a local JTAG connection.

   What this does depends on which chain is selected. Drive the JTAG state
   machine to read a value from a register

   @param[in]  regnum  The JTAG register from which to read
   @param[out] data    Pointer for the returned value.

   @return  system error (positive) or OR1K JTAG error code */
/*---------------------------------------------------------------------------*/

static enum or1k_jtag_errors
jp1_read_jtag_reg (unsigned int  regnum,
		   ULONGEST     *data)
{
  int  retry;

  jp1_ll_prepare_data ();		/* SELECT_DR SCAN */

  for (retry = 0; retry < JP1_NUM_RETRIES; retry++)
    {     
      int  crc_read        = 0;
      int  crc_write       = 0;
      int  crc_actual_read = 0;
      int  crc_ok          = 0;

      jp1_ll_write_jp1 (0, 0);	/* CAPTURE_DR */
      jp1_ll_write_jp1 (0, 0);	/* SHIFT_DR */
      jp1_crc_w = 0;

      /* write regnum */
      jp1_ll_write_stream((ULONGEST)regnum,
			  jp1_chain_addr_size[or1k_jtag_current_chain], 0);
 
      /* read (R/W=0) */
      if (jp1_chain_has_rw[or1k_jtag_current_chain])
	{
	  jp1_ll_write_jp1 (0, 0);
	}

      if (jp1_chain_has_crc[or1k_jtag_current_chain])
	{
	  jp1_crc_r = 0;

	  /* *data = 0 */
	  *data = jp1_ll_read_stream (0,
				      jp1_chain_data_size[or1k_jtag_current_chain],
				      0);
	  crc_write       = jp1_crc_w;
	  crc_actual_read = crc_read;

	  /* Send my crc, EXIT1_DR */
	  crc_read = (int)jp1_ll_read_stream (crc_write, JP1_CRC_SIZE, 1);
	}

      jp1_ll_write_jp1 (1, 0);	/* UPDATE_DR */

      /* Did JTAG receive packet correctly? */
      if (jp1_chain_has_crc[or1k_jtag_current_chain])
	{
	  crc_ok = jp1_ll_read_jp1 ();
	}

      jp1_ll_write_jp1 (1, 0);	/* SELECT_DR */
      if (jp1_chain_has_crc[or1k_jtag_current_chain])
	{
	  if ((crc_read == crc_actual_read) && crc_ok)
	    {
	      return  OR1K_JTAG_ERR_CRC;  /* CZ */
	    }
	  JP1_RETRY_WAIT ();
	}
      else
	{
	  return  OR1K_JTAG_ERR_CRC;  /* CZ */
	}
    }

  return  OR1K_JTAG_ERR_NONE;

}	/* jp1_read_jtag_reg() */


/*----------------------------------------------------------------------------*/
/*!Write a JTAG register on a local JTAG connection.

   What this does depends on which chain is selected. Drive the JTAG state
   machine to read a value from a register

   @param[in]  regnum  The register to write to
   @param[in]  data    Data to write

   @return  system error (positive) or OR1K JTAG error code */
/*---------------------------------------------------------------------------*/

static enum or1k_jtag_errors
jp1_write_jtag_reg (unsigned int  regnum,
		    ULONGEST      data)
{
  int       retry;

  jp1_ll_prepare_data ();		/* SELECT_DR SCAN */

  /* If we don't have rw bit, we assume chain is read only. */
  if (!jp1_chain_has_rw[or1k_jtag_current_chain])
    {
      error ("or1k_jtag_write_reg: Chain not writable");
    }

  for (retry = 0; retry < JP1_NUM_RETRIES; retry++)
    {
      int  crc_read  = 0;
      int  crc_write = 0;
      int  crc_ok    = 0;

      jp1_ll_write_jp1 (0, 0);	/* CAPTURE_DR */
      jp1_ll_write_jp1 (0, 0);	/* SHIFT_DR */
      jp1_crc_w = 0;

      /* write regnum */
      jp1_ll_write_stream((ULONGEST)regnum,
			  jp1_chain_addr_size[or1k_jtag_current_chain], 0);

      /* write (R/W=1) - we tested that previously. */
      jp1_ll_write_jp1 (0, 1);	/* TDI=1 */

      /* write data */
      jp1_ll_write_stream (data, jp1_chain_data_size[or1k_jtag_current_chain],
			 0);
      if (jp1_chain_has_crc[or1k_jtag_current_chain])
	{
	  crc_write = jp1_crc_w;

	  /* write CRC, EXIT1_DR */
	  crc_read = (int)jp1_ll_read_stream  (crc_write, JP1_CRC_SIZE, 1);
	}

      jp1_ll_write_jp1 (1, 0);	/* UPDATE_DR */

      /* Did JTAG receive packet correctly? */
      if (jp1_chain_has_crc[or1k_jtag_current_chain])
	{
	  crc_ok = jp1_ll_read_jp1 ();
	}

      jp1_ll_write_jp1 (1, 0);	/* SELECT_DR */
      if (jp1_chain_has_crc[or1k_jtag_current_chain])
	{
	  if ((crc_read == crc_write) && crc_ok)
	    {
	      return  OR1K_JTAG_ERR_NONE;
	    }
	  JP1_RETRY_WAIT ();
	}
      else
	{
	  return  OR1K_JTAG_ERR_NONE;
	}
    }
  return  OR1K_JTAG_ERR_CRC;

}	/* jp1_write_jtag_reg() */



/*----------------------------------------------------------------------------*/
/*!Process a local OpenRISC 1000 JTAG block read command

   Process the JTAG state machine to read values from a block of
   registers/memory. Built on top of the local routine to read a single
   register.

   If any of the individual reads fail, give up and return the error code of
   that failure. Otherwise return success.

   @param[in]  regnum  The first register for the block from which to read
   @param[out] bdata   Pointer to block for the returned values
   @param[out] count   How many registers to read.

   @return  system error (positive) or OR1K JTAG error code */
/*---------------------------------------------------------------------------*/

static enum or1k_jtag_errors
jp1_read_jtag_block (unsigned int  regnum,
		     uint32_t     *bdata,
		     int           count)
{
  ULONGEST               data;
  enum or1k_jtag_errors  result;
  int                    i;

  for (i=0; i < count; i++)
    {
      result = jp1_read_jtag_reg (regnum + i, &data);
      if (OR1K_JTAG_ERR_NONE != result)
	{
	  return result;
	}

      bdata[i] = (uint32_t)data;
    }

  return  OR1K_JTAG_ERR_NONE;

}	/* jp1_read_jtag_block() */


/*----------------------------------------------------------------------------*/
/*!Process a local OpenRISC 1000 JTAG block write command

   Process the JTAG state machine to write values to a block of
   registers/memory. Built on top of the local routine to write a single
   register.

   If any of the individual writes fail, give up and return the error code of
   that failure. Otherwise return success.

   @param[in]  regnum  The first register for the block to which to write
   @param[out] bdata   Pointer to block for the values to write
   @param[out] count   How many registers to write.

   @return  system error (positive) or OR1K JTAG error code */
/*---------------------------------------------------------------------------*/

static enum or1k_jtag_errors
jp1_write_jtag_block (unsigned int    regnum,
		      const uint32_t *bdata,
		      int             count)
{
  enum or1k_jtag_errors  result;
  int                    i;

  for (i=0; i < count; i++)
    {
      result = jp1_write_jtag_reg (regnum + i, (ULONGEST)(bdata[i]));

      if (OR1K_JTAG_ERR_NONE != result)
	{
	  return result;
	}
    }

  return  OR1K_JTAG_ERR_NONE;

}	/* jp1_write_jtag_block() */


/*----------------------------------------------------------------------------*/
/*!Process a local OpenRISC 1000 JTAG set chain command

   Process the JTAG state machine to select the specified chain, unless it is
   already selected.

   @param[in] chain  The scan chain to use

   @return  system error (positive) or OR1K JTAG error code */
/*---------------------------------------------------------------------------*/

static enum or1k_jtag_errors
jp1_select_chain (int  chain)
{
  enum or1k_jtag_errors  result;

  /* Nothing to do if we already have the chain we want */
  if (or1k_jtag_current_chain == chain)
    {
      return  OR1K_JTAG_ERR_NONE;
    }

  /* Is it a chain we are allowed to set */
  if (!jp1_chain_is_valid[chain])
    {
      return OR1K_JTAG_ERR_INVALID_CHAIN;
    }

  jp1_ll_prepare_control ();	/* Select IR scan*/

  jp1_ll_write_jp1 (0, 0);		/* CAPTURE_IR */
  jp1_ll_write_jp1 (0, 0);		/* SHIFT_IR */

  /* write data, EXIT1_IR */
  jp1_ll_write_stream((ULONGEST)OR1K_JI_CHAIN_SELECT, OR1K_JI_SIZE, 1);
           
  jp1_ll_write_jp1 (1, 0);		/* UPDATE_IR */
  jp1_ll_write_jp1 (1, 0);		/* SELECT_DR */
   
  jp1_ll_write_jp1 (0, 0);		/* CAPTURE_DR */
  jp1_ll_write_jp1 (0, 0);		/* SHIFT_DR */

  /* write data, EXIT1_DR */
  jp1_ll_write_stream ((ULONGEST)chain, OR1K_SC_SIZE, 1);
     
  jp1_ll_write_jp1 (1, 0);		/* UPDATE_DR */
  jp1_ll_write_jp1 (1, 0);		/* SELECT_DR */

  /* Now we have to go out of SELECT_CHAIN mode.  */
  jp1_ll_write_jp1 (1, 0);		/* SELECT_IR */
  jp1_ll_write_jp1 (0, 0);		/* CAPTURE_IR */
  jp1_ll_write_jp1 (0, 0);		/* SHIFT_IR */

  /* write data, EXIT1_IR */
  jp1_ll_write_stream ((ULONGEST)OR1K_JI_DEBUG, OR1K_JI_SIZE, 1);
           
  jp1_ll_write_jp1 (1, 0);		/* UPDATE_IR */
  jp1_ll_write_jp1 (1, 0);		/* SELECT_DR */

  or1k_select_dr     = 1;
  or1k_jtag_current_chain = chain;

  return OR1K_JTAG_ERR_NONE;

}	/* jp1_select_chain() */



/* These are the OpenRISC 1000 JTAG Protocol routines for use with a remote
   server. They are built on tol of the remote protocol low level helper
   functions. All these functions are static and have the prefix jtr_ */

/*----------------------------------------------------------------------------*/
/*!Read a JTAG register from a remote OpenRISC 1000 JTAG server

   What this does depends on which chain is selected.

   Build the command from the args and send it. If successful get the
   response.

   All args and the results are in host format. All transmitted data is in
   network format. This routine converts between the two

   @param[in]  regnum  The JTAG register to read
   @param[out] data    Pointer for the returned value.

   @return  system error (positive) or OR1K JTAG error code */
/*---------------------------------------------------------------------------*/

static enum or1k_jtag_errors
jtr_read_jtag_reg (unsigned int  regnum,
		   ULONGEST     *data)
{
  enum or1k_jtag_errors  result;
  int                    fd = or1k_jtag_connection.device.fd;
  unsigned long int      len;
  unsigned long int      data_h;
  unsigned long int      data_l;

  struct jtr_read_message   tx_buf;
  struct jtr_read_response  rx_buf;

  /* Build the command in network format */
  len = sizeof (tx_buf)  - sizeof (tx_buf.command)  - sizeof (tx_buf.length);

  tx_buf.command = htonl (OR1K_JTAG_COMMAND_READ);
  tx_buf.length  = htonl (len);
  tx_buf.address = htonl (regnum);

  /* Try to send the command */
  result = jtr_ll_write (fd, &tx_buf, sizeof (tx_buf) );
  if (OR1K_JTAG_ERR_NONE != result)
    {
      return  jtr_ll_check (result);
    }

  /* Try to get the result */
  result = jtr_ll_response (fd, &rx_buf, sizeof (rx_buf) );
  if (OR1K_JTAG_ERR_NONE != result)
    {
      return  jtr_ll_check (result);
    }

  /* Break out the data */
  data_h  = ntohl (rx_buf.data_h);
  data_l  = ntohl (rx_buf.data_l);

  *data = (((ULONGEST)data_h) << 32) | (ULONGEST)data_l;
  return jtr_ll_check (ntohl (rx_buf.status) );

}	/* jtr_read_jtag_reg() */


/*----------------------------------------------------------------------------*/
/*!Write a JTAG register on a remote OpenRISC 1000 JTAG server

   What this does depends on which chain is selected.

   Build the command from the args and send it. If successful get the
   response.

   All args and the results are in host format. All transmitted data is in
   network format. This routine converts between the two

   @param[in]  regnum  The JTAG register to write
   @param[out] data    The data to write

   @return  system error (positive) or OR1K JTAG error code */
/*---------------------------------------------------------------------------*/

static enum or1k_jtag_errors
jtr_write_jtag_reg (unsigned long int  regnum,
		    ULONGEST           data)
{
  enum or1k_jtag_errors  result;
  int                    fd = or1k_jtag_connection.device.fd;
  unsigned long int      len;

  struct jtr_write_message   tx_buf;
  struct jtr_write_response  rx_buf;

  /* Build the command in network format */
  len = sizeof (tx_buf)  - sizeof (tx_buf.command)  - sizeof (tx_buf.length);

  tx_buf.command = htonl (OR1K_JTAG_COMMAND_WRITE);
  tx_buf.length  = htonl (len);
  tx_buf.address = htonl (regnum);
  tx_buf.data_h  = htonl((uint32_t)(data >> 32));
  tx_buf.data_l  = htonl((uint32_t)(data & 0xffffffff));

  /* Try to send the command */
  result = jtr_ll_write (fd, &tx_buf, sizeof (tx_buf) );
  if (OR1K_JTAG_ERR_NONE != result)
    {
      return  jtr_ll_check (result);
    }

  /* Try to get the result */
  result = jtr_ll_response (fd, &rx_buf, sizeof (rx_buf) );
  if (OR1K_JTAG_ERR_NONE != result)
    {
      return  jtr_ll_check (result);
    }

  /* Break out the response */
  return  jtr_ll_check (ntohl (rx_buf.status) );

}	/* jtr_write_jtag_reg() */


/*----------------------------------------------------------------------------*/
/*!Read a block of JTAG registers from a remote OpenRISC 1000 JTAG server

   What this does depends on which chain is selected.

   Construct the command from the args and send it. If successful get the
   response.

   All args and the results are in host format. All transmitted data is in
   network format. This routine converts between the two

   @param[in]  regnum  The first register to read
   @param[out] bdata   Pointer to block of memory for the returned values.
   @param[out] count   Number of registers to read.

   @return  system error (positive) or OR1K JTAG error code */
/*---------------------------------------------------------------------------*/

static enum or1k_jtag_errors
jtr_read_jtag_block (CORE_ADDR  regnum,
		     uint32_t  *bdata,
		     int        count)
{
  enum or1k_jtag_errors  result;
  int                    fd = or1k_jtag_connection.device.fd;
  unsigned long int      count_rv;
  unsigned long int      len;
  int                    r;

  struct jtr_read_block_message   tx_buf;
  struct jtr_read_block_response  rx_buf;

  /* Build the command in network format */
  len = sizeof (tx_buf)  - sizeof (tx_buf.command)  - sizeof (tx_buf.length);

  tx_buf.command  = htonl (OR1K_JTAG_COMMAND_READ_BLOCK);
  tx_buf.length   = htonl (len);
  tx_buf.address  = htonl (regnum);
  tx_buf.num_regs = htonl (count);

  /* Try to send the command */
  result = jtr_ll_write (fd, &tx_buf, sizeof (tx_buf) );
  if (OR1K_JTAG_ERR_NONE != result)
    {
      return  jtr_ll_check (result);
    }

  /* Try to get the result. This will have just the status and number of regs */
  result = jtr_ll_response (fd, &rx_buf, sizeof (rx_buf) );
  if (OR1K_JTAG_ERR_NONE != result)
    {
      return  jtr_ll_check (result);
    }

  /* Break out the num regs received */
  count_rv = ntohl (rx_buf.num_regs);

  /* Must get the same number of regs back */
  if (count_rv != count)
    {
      /* Read the offered registers back to clear the protocol. We don't want
	 them going in to the result buffer and their could be an arbitary
	 number, so get them one at a time. */
      for (r =  0; r < count_rv; r++)
	{
	  uint32_t  dummy_data;

	  result = jtr_ll_read (fd, &dummy_data, sizeof (dummy_data) );
	  if (OR1K_JTAG_ERR_NONE != result)
	    {
	      return  jtr_ll_check (result);
	    }
	}

      return OR1K_JTAG_ERR_PROTOCOL_ERROR;
    }

  /* Get the regs back and then convert them to host representation */
  result = jtr_ll_read (fd, bdata, count * sizeof (uint32_t) );
  if (OR1K_JTAG_ERR_NONE != result)
    {
      return  jtr_ll_check (result);
    }

  for (r = 0; r < count; r++)
    {
      bdata[r] = ntohl (bdata[r]);
    }

  /* Return the result */
  return  jtr_ll_check (ntohl (rx_buf.status) );

}	/* jtr_read_jtag_block() */


/*----------------------------------------------------------------------------*/
/*!Write a block of JTAG registers on a remote OpenRISC 1000 JTAG server

   What this does depends on which chain is selected.

   Construct the command from the args and send it. If successful get the
   response.

   All args and the results are in host format. All transmitted data is in
   network format. This routine converts between the two

   @param[in]  regnum  The start address address for the block to be read
   @param[out] bdata   Pointer to values to write to the block of memory.
   @param[out] count   Number of values to read.

   @return  system error (positive) or OR1K JTAG error code */
/*---------------------------------------------------------------------------*/

static enum or1k_jtag_errors
jtr_write_jtag_block (CORE_ADDR       regnum,
		      const uint32_t *bdata,
		      int             count)
{
  enum or1k_jtag_errors  result;
  int                    fd = or1k_jtag_connection.device.fd;
  unsigned long int      len;
  unsigned long int      full_len;
  int                    r;

  struct jtr_write_block_message  *tx_buf;	/* Allocate dynamically */
  struct jtr_write_block_response  rx_buf;

  /* Allocate a buffer large enough to send, then build the command */
  full_len = sizeof (*tx_buf)  +  (count - 1)  * sizeof (uint32_t);
  len      = full_len - sizeof (tx_buf->command)  - sizeof (tx_buf->length);
  tx_buf   = (struct jtr_write_block_message *)(xmalloc (full_len) );

  tx_buf->command  = htonl (OR1K_JTAG_COMMAND_WRITE_BLOCK);
  tx_buf->length   = htonl (len);
  tx_buf->address  = htonl (regnum);
  tx_buf->num_regs = htonl (count);

  for (r = 0; r < count; r++)
    {
      tx_buf->data[r] = htonl (bdata[r]);
    }

  /* Try to send the command, after which we can free the buffer */
  result = jtr_ll_write (fd, tx_buf, full_len);
  xfree (tx_buf);

  if (OR1K_JTAG_ERR_NONE != result)
    {
      return  jtr_ll_check (result);
    }

  /* Try to get the result */
  result = jtr_ll_response (fd, &rx_buf, sizeof (rx_buf) );
  if (OR1K_JTAG_ERR_NONE != result)
    {
      return  jtr_ll_check (result);
    }

  return jtr_ll_check (ntohl (rx_buf.status) );

}	/* jtr_write_jtag_block() */


/*----------------------------------------------------------------------------*/
/*!Select a remote OpenRISC 1000 JTAG chain command

   Construct the command from the args and send it. If successful get the
   response.

   All args and the results are in host format. All transmitted data is in
   network format. This routine converts between the two

   @param[in] chain  The scan chain to use

   @return  system error (positive) or OR1K JTAG error code */
/*---------------------------------------------------------------------------*/

static enum or1k_jtag_errors
jtr_select_chain (int  chain)
{
  enum or1k_jtag_errors  result;
  int                    fd = or1k_jtag_connection.device.fd;
  unsigned long int      len;

  struct jtr_chain_message   tx_buf;
  struct jtr_chain_response  rx_buf;

  /* Nothing to do if we already have the chain we want */
  if (or1k_jtag_current_chain == chain)
    {
      return  OR1K_JTAG_ERR_NONE;
    }

  /* Build the command in network format */
  len = sizeof (tx_buf)  - sizeof (tx_buf.command)  - sizeof (tx_buf.length);

  tx_buf.command = htonl (OR1K_JTAG_COMMAND_CHAIN);
  tx_buf.length  = htonl (len);
  tx_buf.chain   = htonl (chain);

  /* Try to send the command */
  result = jtr_ll_write (fd, &tx_buf, sizeof (tx_buf) );
  if (OR1K_JTAG_ERR_NONE != result)
    {
      or1k_jtag_current_chain = OR1K_SC_UNDEF;
      return  jtr_ll_check (result);
    }
  
  /* Try to get the result */
  result = jtr_ll_response (fd, &rx_buf, sizeof (rx_buf) );
  if (OR1K_JTAG_ERR_NONE != result)
    {
      or1k_jtag_current_chain = OR1K_SC_UNDEF;
      return  jtr_ll_check (result);
    }

  result = jtr_ll_check (ntohl (rx_buf.status) );

  if (OR1K_JTAG_ERR_NONE != result)
    {
      or1k_jtag_current_chain = OR1K_SC_UNDEF;
    }
  else
    {
      or1k_jtag_current_chain = chain;
    }

  return  result;

}	/* jtr_select_chain() */



/* These are the high level functions based on the JTAG Remote protocol,
   independent of whether they are local or remote. They just split the calls
   down to their local or remote variants. All errors are dealt with by this
   stage, so all functions have void returns. All functions are static and
   begin or1k_jtag_ */

/*----------------------------------------------------------------------------*/
/*!Function to read a JTAG register

   Call either the local or remote version of the corresponding JTAG function
   as appropriate.

   @param[in]  regnum  The JTAG register to read
   @param[out] data    Where to put the written value

   @return  system error (positive) or OR1K JTAG error code */
/*---------------------------------------------------------------------------*/

static void
or1k_jtag_read_jtag_reg (unsigned int  regnum,
			 ULONGEST     *data)
{
  enum or1k_jtag_errors  result;

  switch (or1k_jtag_connection.location)
    {
    case OR1K_JTAG_LOCAL:
      result = jp1_read_jtag_reg (regnum, data);
      break;

    case OR1K_JTAG_REMOTE:
      result = jtr_read_jtag_reg (regnum, data);
      break;

    default:
      result = OR1K_JTAG_ERR_NO_CONNECTION;
      break;
    }
 
  if (OR1K_JTAG_ERR_NONE != result)
    {
      error ("or1k_jtag_read_jtag_reg: regnum %u, data 0x%p, result %s\n",
	     regnum, data, or1k_jtag_err_name (result) );
    }
}	/* or1k_jtag_read_jtag_reg() */


/*----------------------------------------------------------------------------*/
/*!Function to write a JTAG register

   Call either the local or remote version of the corresponding JTAG function
   as appropriate.

   @param[in] regnum  The JTAG register to write
   @param[in] data    The value to write

   @return  system error (positive) or OR1K JTAG error code */
/*---------------------------------------------------------------------------*/

static void
or1k_jtag_write_jtag_reg (unsigned int  regnum,
			  ULONGEST      data)
{
  enum or1k_jtag_errors  result;

  switch (or1k_jtag_connection.location)
    {
    case OR1K_JTAG_LOCAL:
      result = jp1_write_jtag_reg((unsigned long int)regnum, data);
      break;

    case OR1K_JTAG_REMOTE:
      result = jtr_write_jtag_reg((unsigned long int)regnum, data);
      break;

    default:
      result = OR1K_JTAG_ERR_NO_CONNECTION;
      break;
    }
 
  if (OR1K_JTAG_ERR_NONE != result)
    {
      error ("or1k_jtag_write_jtag_reg: regnum %u, data 0x%16llx, result %s\n",
	     regnum, (long long unsigned int)data, or1k_jtag_err_name (result) );
    }
}	/* or1k_jtag_write_jtag_reg() */


/*----------------------------------------------------------------------------*/
/*!Function to read a block of JTAG registers via OpenRISC 1000 JTAG

   Call either the local or remote version of the corresponding JTAG function
   as appropriate.

   @param[in]   regnum The starting register
   @param[out]  bdata  Pointer to a buffer for the results
   @param[in]   count  Number of registers to be read

   @return  system error (positive) or OR1K JTAG error code */
/*---------------------------------------------------------------------------*/

static void
or1k_jtag_read_jtag_block (unsigned int  regnum,
			   uint32_t     *bdata,
			   int           count)
{
  enum or1k_jtag_errors  result;

  switch (or1k_jtag_connection.location)
    {
    case OR1K_JTAG_LOCAL:
      result = jp1_read_jtag_block (regnum, bdata, count);
      break;

    case OR1K_JTAG_REMOTE:
      result = jtr_read_jtag_block (regnum, bdata, count);
      break;

    default:
      result = OR1K_JTAG_ERR_NO_CONNECTION;
      break;
    }

  if (OR1K_JTAG_ERR_NONE != result)
    {
      error ("or1k_jtag_read_jtag_block: "
	     "regnum %u, bdata 0x%p, count %d, result %s\n",
	     regnum, bdata, count, or1k_jtag_err_name (result) );
    }
}	/* or1k_jtag_read_jtag_block() */


/*----------------------------------------------------------------------------*/
/*!Function to write a block of JTAG registers via OpenRISC 1000 JTAG

   Call either the local or remote version of the corresponding JTAG function
   as appropriate.

   @param[in]  regnum  The starting register to write
   @param[out] bdata   Pointer to a buffer with the values to write
   @param[in]  count  Number of values to write

   @return  system error (positive) or OR1K JTAG error code */
/*---------------------------------------------------------------------------*/

static void
or1k_jtag_write_jtag_block (unsigned int    regnum,
			    const uint32_t *bdata,
			    int             count)
{
  enum or1k_jtag_errors  result;

  switch (or1k_jtag_connection.location)
    {
    case OR1K_JTAG_LOCAL:
      result = jp1_write_jtag_block (regnum, bdata, count);
      break;

    case OR1K_JTAG_REMOTE:
      result = jtr_write_jtag_block (regnum, bdata, count);
      break;

    default:
      result = OR1K_JTAG_ERR_NO_CONNECTION;
      break;
  }

  if (OR1K_JTAG_ERR_NONE != result)
    {
      error ("or1k_jtag_write_jtag_block: "
	     "regnum %u, bdata 0x%p, count %d, result %s\n",
	     regnum, bdata, count, or1k_jtag_err_name (result) );
    }
}	/* or1k_jtag_write_jtag_block() */


/*----------------------------------------------------------------------------*/
/*!Function to select a scan chain via OpenRISC 1000 JTAG

   Call either the local or remote version of the corresponding JTAG function
   as appropriate.

   @param[in] chain  The scan chain to use

   @return  system error (positive) or OR1K JTAG error code */
/*---------------------------------------------------------------------------*/

static void
or1k_jtag_select_chain (int  chain)
{
  enum or1k_jtag_errors  result;

  switch (or1k_jtag_connection.location)
    {
    case OR1K_JTAG_LOCAL:
      result = jp1_select_chain (chain);
      break;

    case OR1K_JTAG_REMOTE:
      result = jtr_select_chain (chain);
      break;

    default:
      result = OR1K_JTAG_ERR_NO_CONNECTION;
      break;
    }

  if (OR1K_JTAG_ERR_NONE != result)
    {
      error ("or1k_jtag_select_chain: chain %d, result %s\n", chain,
	     or1k_jtag_err_name (result) );
    }
}	/* or1k_jtag_select_chain() */


/*----------------------------------------------------------------------------*/
/*!Function to reset the CPU.

   This takes the processor into reset and then releases the reset.

   The code is drawn from the GDB 5.3 code, pending clarification of how the
   reset works on real hardware. However there is no attempt to clear HW trace
   (not available with Igor Moder's version of the JTAG hardware), nor to set
   the stall flag (reset is defined to clear stall).

   Use the underlying JTAG protocol calls to select the relevant scan chain
   and change the relevant registers. Always succeeds.
*/
/*---------------------------------------------------------------------------*/

static void
or1k_jtag_reset()
{
  ULONGEST               data;

  /* Select the correct chain for controlling the processor */
  or1k_jtag_select_chain (OR1K_SC_REGISTER);
 
  /* Read the JTAG register, set the and reset bit and write it back */
  or1k_jtag_read_jtag_reg (OR1K_JTAG_RISCOP, &data);
  data |= OR1K_JTAG_RISCOP_RESET;
  or1k_jtag_write_jtag_reg (OR1K_JTAG_RISCOP, data);

  /* Wait 1000 microseconds. The old code used to loop reading the register
     100 times before this, but we've left that out. */
  usleep (1000);

  /* Clear the reset bit and write it back. The old code used to read the
     register back (and ignore the result), but we don't bother. */
  data &= ~OR1K_JTAG_RISCOP_RESET;
  or1k_jtag_write_jtag_reg (OR1K_JTAG_RISCOP, data);

}	/* or1k_jtag_reset() */


/*----------------------------------------------------------------------------*/
/*!Utility method to turn an OpenRISC 1000 JTAG error into a string

   Positive error values are system errors, negative values OpenRISC 100 JTAG
   errors.

   @param[in] e  The error number */
/*---------------------------------------------------------------------------*/

static const char *
or1k_jtag_err_name (enum or1k_jtag_errors  e)
{
  const char *jtag_err[] =
    {
      "None",
      "JTAG CRC error",
      "JTAG memory error",
      "Invalid JTAG command"
      "Remote JTAG server terminated",
      "No JTAG server connection",
      "JTAG protocol error",
      "JTAG command not implemented",
      "Invalid JTAG chain",
      "Invalid JTAG address",
      "JTAG access exception",
      "Invalid JTAG length",
      "JTAG out of memory"
    };

  if (e > 0)
    {
      return strerror (e);
    }
  else
    {
      return jtag_err[-e];
    }
}	/* or1k_jtag_err_name() */



/* These are the high level public functions. They call the underlying
   functions based on the OR1K JTAG protocol. */

/*----------------------------------------------------------------------------*/
/*!Initialize a new OpenRISC 1000 JTAG connection.

   Initialize a new connection to the or1k board, and make sure we are really
   connected.

   @param[in] args  The entity to connect to followed by an optional NO_RESET
   flag */
/*--------------------------------------------------------------------------*/

void
or1k_jtag_init (char *args)
{
  char  *port_name;
  char  *reset_arg;
  char **argv;
  int    remote_offset;

  if (args == 0)
    { /* CZ */
      error ("or1k_jtag_init: To open a or1k remote debugging connection,\n"
	     "you need to specify a parallel port connected to the target\n"
	     "board, or else a remote server which will proxy these services\n"
	     "for you.\n"
	     "Example: /dev/lp0 or jtag://debughost.mydomain.com:8100.\n");
      return;
    }

  /* If we currently have an open connection, shut it down. This is due to a
     temporary bug in gdb. */
  switch(or1k_jtag_connection.location)
    {
    case OR1K_JTAG_REMOTE:
      if (or1k_jtag_connection.device.fd > 0)
	{
	  jtr_ll_close ();
	}
      break;
    case OR1K_JTAG_LOCAL:
      if (or1k_jtag_connection.device.lp > 0)
	{
	  close (or1k_jtag_connection.device.lp);
	}
      break;
    }

  /* Parse the port name. Use libiberty to get rid of quote marks etc. */
  argv = buildargv (args);
  if (NULL == argv)
    {
      nomem (0);		/* Memory exhaustion is only cause of failure */
      return;
    }
  port_name = xstrdup (argv[0]);

  if (NULL != argv[1])
    {
      reset_arg = xstrdup (argv[1]);
    }
  else
    {
      reset_arg = NULL;
    }

  make_cleanup_freeargv (argv);
 
  /* CZ 24/05/01 - Check to see if we have specified a remote jtag interface
     or a local one. It is remote if it follows one of the URL naming
     conventions:

     jtag://<hostname>:<port_or_service> (default debug interface)
     jtag_orpsoc://<hostname>:<port_or_service> (orpsoc debug interface)
     jtag_mohor://<hostname>:<port_or_service> (Igor Mohor's debug interface)

     Work out the offset to the end of the :// in remote_offset, then use this
     as an indicator of whether a remote or local interface was requested.
  */
  if (0 == strncmp (port_name, "jtag://", 7))
    {
      remote_offset       = 7;
      or1k_dbg_if_version = OR1K_DBG_IF_ORPSOC;
    }
  else if (0 == strncmp (port_name, "jtag_orpsoc://", 14))
    {
      remote_offset       = 14;
      or1k_dbg_if_version = OR1K_DBG_IF_ORPSOC;
    }
  else if (0 == strncmp (port_name, "jtag_mohor://", 13))
    {
      remote_offset       = 13;
      or1k_dbg_if_version = OR1K_DBG_IF_MOHOR;
    }
  else
    {
      remote_offset = 0;
    }

  if (remote_offset > 0)
    {
      /* Interface is remote */
      /* Connect to the remote proxy server */
      char *port_or_service = strchr (&(port_name[remote_offset]), ':');
      char  hostname[256];
      int   len;

      if (NULL == port_or_service)
	{
	  error ("or1k_jtag_init: must specify remote port or service\n");
	  free (port_name);
	  return;
	}

      len = port_or_service - port_name - remote_offset;
      strncpy (hostname, &port_name[remote_offset], len);
      hostname[len] = '\0';
      port_or_service++;
   
      or1k_jtag_connection.device.fd = jtr_ll_connect (hostname,
						     port_or_service);
      free (port_name);
      if (0 == or1k_jtag_connection.device.fd)
	{
	  error ("or1k_jtag_init: can't access JTAG Proxy Server at \"%s\"",
		 port_name);
	  return;
	}

      or1k_jtag_connection.location = OR1K_JTAG_REMOTE;
    }
  else
    {
      /* Interface is local */
      /* Open and initialize the parallel port.  */
      or1k_jtag_connection.device.lp = open (port_name, O_WRONLY);
      free (port_name);

      if (or1k_jtag_connection.device.lp < 0)
	{
	  error ("Cannot open device.");
	  return;
	}

      jp1_ll_reset_jp1 ();
      or1k_jtag_connection.location = OR1K_JTAG_LOCAL; /* CZ */
    }

  or1k_jtag_current_chain = OR1K_SC_UNDEF;

  /* See if the user requested a reset */

  if (NULL != reset_arg)
    {
      if (0 == strncasecmp( reset_arg, "RESET", strlen ("RESET")))
	{
	  or1k_jtag_reset ();
	}
      else
	{
	  warning ("or1k_jtag_init: unrecognized argument, assume no reset\n");
	}
    }

}	/* or1k_jtag_init() */


/*----------------------------------------------------------------------------*/
/*!Close down an OpenRISC 1000 JTAG connection.

   Fixed an apparent problem - jtr_ll_close() should not be used with the local
   connection, since it isn't a socket.
*/
/*---------------------------------------------------------------------------*/

void
or1k_jtag_close()  /* CZ */
{
  switch (or1k_jtag_connection.location)
    {
    case OR1K_JTAG_LOCAL:
      (void)close (or1k_jtag_connection.device.lp);
      or1k_jtag_connection.device.lp = 0;
      break;

    case OR1K_JTAG_REMOTE:
      jtr_ll_close ();
      or1k_jtag_connection.device.fd = 0;
      break;
    }

  or1k_jtag_connection.location = OR1K_JTAG_NOT_CONNECTED;

}	/* or1k_jtag_close() */


/*----------------------------------------------------------------------------*/
/*!Global function to read a special purpose register via OpenRISC 1000 JTAG

   This can be used to read GPRs, since GPRs are mapped onto SPRs.

   Use the underlying JTAG protocol calls to select the relevant scan chain
   and get the SPR value. Always succeeds.

   @param[in]  sprnum  The address (register) to read

   @return  The value read */
/*---------------------------------------------------------------------------*/

ULONGEST
or1k_jtag_read_spr (unsigned int  sprnum)
{
  ULONGEST  data;

  /* Select the correct chain for SPR read/write, then read */
  or1k_jtag_select_chain (OR1K_SC_RISC_DEBUG);
  or1k_jtag_read_jtag_reg (sprnum, &data);

  return  data;

}	/* or1k_jtag_read_spr() */


/*----------------------------------------------------------------------------*/
/*!Global function to write a special purpose register via OpenRISC 1000 JTAG

   This can be used to write GPRs, since GPRs are mapped onto SPRs.

   Use the underlying JTAG protocol calls to select the relevant scan chain
   and write the SPR value. Always succeeds.

   @param[in]  sprnum  The SPR to write
   @param[in]  data    The value to write */
/*---------------------------------------------------------------------------*/

void
or1k_jtag_write_spr (unsigned int  sprnum,
		     ULONGEST      data)
{
  /* Select the correct chain for SPR read/write, then write */
  or1k_jtag_select_chain (OR1K_SC_RISC_DEBUG);
  or1k_jtag_write_jtag_reg (sprnum, data);

}	/* or1k_jtag_write_spr() */


/*----------------------------------------------------------------------------*/
/*!Global function to read a block of memory via OpenRISC 1000 JTAG

   Use the underlying JTAG protocol calls to select the relevant scan chain
   and read the memory. Always succeeds.

   The underlying protocol only reads whole uint32_t words, so we must fix
   this if the address and/or len are not on a word boundary with partial
   reads.

   This needs to work even for very small blocks of data (where the start and
   end may be in the same word).

   This also runs into endianism issues, since this is a read of a stream of
   bytes, but the underlying JTAG call is a stream of uint32_t. We need to
   handle this appropriately.

   @param[in]   addr   The starting address for the read
   @param[out]  bdata  Pointer to a buffer for the results
   @param[in]   len    Number of values to be read

   @return  The number of bytes read, or zero if there is an error */
/*---------------------------------------------------------------------------*/

int
or1k_jtag_read_mem (CORE_ADDR  addr,
		    gdb_byte  *bdata,
		    int        len)
{
  const int  bpw         = sizeof (uint32_t);
  CORE_ADDR  offset_mask = (CORE_ADDR)(bpw - 1);
  CORE_ADDR  word_mask   = ~offset_mask;		    

  int        start_off	 =  addr        & offset_mask;	    
  int        end_off	 = (addr + len) & offset_mask;	    
  int        start_bytes = (bpw - start_off) % bpw;
  int        end_bytes   = end_off;
  CORE_ADDR  aligned	 = (CORE_ADDR)(addr + bpw - 1) & word_mask;
  int        word_count;

  gdb_byte   buf[bpw];
  int        w;

  /* Scan chain for memory access */
  or1k_jtag_select_chain (OR1K_SC_WISHBONE);

  /* Get any odd start bytes */
  if (0 != start_bytes)
    {
      ULONGEST  data;
      int       i;
      
      start_bytes = start_bytes < len ? start_bytes : len;

      /* Read word (target endianism) and unpack allowing for the
	 endianism. Buffer will now have the character that was at the lowest
	 address at the lowest address in the buffer, independent of the
	 endianism. */
      or1k_jtag_read_jtag_reg (aligned - bpw, &data);
      store_unsigned_integer (buf, bpw, data);

      /* Copy to the main buffer */
      for (i = 0; i < start_bytes; i++)
	{
	  bdata[i] = buf[start_off + i];
	}

      /* If this was the lot, stop here */
      if( len == start_bytes )
	{
	  return  len;
	}
    }

  /* Get the main block (if any). This will be a string of words (target
     endianism), which we have to unpack to characters, so that the character
     at the lowest address in the word ends up in the lowest address in the
     buffer, independent of the endianism. */

  word_count  = (len - start_bytes - end_bytes) / bpw;

  if (word_count > 0)
    {
      or1k_jtag_read_jtag_block (aligned, (uint32_t *)(bdata + start_bytes),
				 word_count);

      for (w = 0; w < word_count; w++)
	{
	  gdb_byte *offset = bdata + start_bytes + w * bpw;
     
	  store_unsigned_integer (offset, bpw, *((uint32_t *)offset));
	}
    }

  /* Get any odd end bytes */
  if (0 != end_bytes)
    {
      ULONGEST  data;
      int       i;
      
      /* Read word and convert to endianism independent buffer */
      or1k_jtag_read_jtag_reg (aligned + word_count * bpw, &data);
      store_unsigned_integer (buf, bpw, data);

      /* Copy the relevant bytes to the main buffer */
      for (i = 0; i < end_bytes; i++)
	{
	  bdata[start_bytes + word_count * bpw + i] = buf[i];
	}
    }

  return  len;

}	/* or1k_jtag_read_mem() */


/*----------------------------------------------------------------------------*/
/*!Global function to write a block of memory via OpenRISC 1000 JTAG

   Use the underlying JTAG protocol calls to select the relevant scan chain
   and write the memory. Always succeeds.

   The underlying protocol only writes whole words, so we must fix this if the
   address and/or len are not on a word boundary with partial writes.

   This also runs into endianism issues, since this is a read of a stream of
   bytes, but the underlying JTAG call is a stream of uint32_t. We need to
   handle this appropriately.

   @param[in]   addr   The starting address for the read
   @param[out]  bdata  Pointer to a buffer for the results
   @param[in]   len    Number of values to be read

   @return  The number of bytes read, or zero if there is an error */
/*---------------------------------------------------------------------------*/

int
or1k_jtag_write_mem (CORE_ADDR       addr,
		     const gdb_byte *bdata,
		     int             len)
{
  const int  bpw         = sizeof (uint32_t);
  CORE_ADDR  offset_mask = (CORE_ADDR)(bpw - 1);
  CORE_ADDR  word_mask   = ~offset_mask;		    

  int        start_off	 = addr         & offset_mask;	    
  int        end_off	 = (addr + len) & offset_mask;	    
  int        start_bytes = (bpw - start_off) % bpw;
  int        end_bytes   = end_off;
  CORE_ADDR  aligned	 = (CORE_ADDR)(addr + bpw - 1) & word_mask;
  int        word_count;

  gdb_byte   buf[bpw];
  int        w;
  CORE_ADDR  c;

  /* Scan chain for memory access */
  or1k_jtag_select_chain (OR1K_SC_WISHBONE);

  /* Write any odd start bytes */
  if (0 != start_bytes)
    {
      ULONGEST  data;
      int       i;

      start_bytes = start_bytes < len ? start_bytes : len;

      /* Read word (target endianism) and unpack allowing for
	 endianism. Buffer will now have the character that was at the lowest
	 address at the lowest address in the buffer, independent of the
	 endianism. */
      or1k_jtag_read_jtag_reg (aligned - bpw, &data);
      store_unsigned_integer (buf, bpw, data);

      /* Patch bytes, which is now endianism independent. */
      for (i = 0; i < start_bytes; i++)
	{
	  buf[start_off + i] = bdata[i];
	}

      /* Convert back to a value represented with target endianism and write
	 back. */
      data = extract_unsigned_integer (buf, bpw);
      or1k_jtag_write_jtag_reg (aligned - bpw, data);

      /* If this was the lot, stop here */
      if( len == start_bytes )
	{
	  return  len;
	}
    }

  /* We need to patch the main data (if any) before sending, but this is
     immutable, so we need a copy. Since the originating buffer could be
     arbitrarily large we have to do this in chunks of up to
     OR1K_MAX_JTAG_WRITE words. */

  word_count  = (len - start_bytes - end_bytes) / bpw;

  for (c  = 0; c * OR1K_MAX_JTAG_WRITE < word_count; c++)
    {
      uint32_t      new_bdata[OR1K_MAX_JTAG_WRITE * bpw];
      unsigned int  c_offset = c * OR1K_MAX_JTAG_WRITE;
      unsigned int  todo     = word_count - c_offset;

      todo = todo > OR1K_MAX_JTAG_WRITE ? OR1K_MAX_JTAG_WRITE : todo;

      for (w = 0; w < todo; w++)
	{
	  const gdb_byte *byte_off = bdata + start_bytes + (c_offset + w) * bpw;
	  new_bdata[w] = extract_unsigned_integer (byte_off, bpw);
	}

      /* Write the patched block */
      or1k_jtag_write_jtag_block (aligned + c_offset * bpw,
				  (const uint32_t *)new_bdata,  todo);
    }
 
  /* Get any odd end bytes */
  if (0 != end_bytes)
    {
      ULONGEST  data;
      int       i;

      /* Read word and convert to endianism independent buffer */
      or1k_jtag_read_jtag_reg (aligned + word_count * bpw, &data);
      store_unsigned_integer (buf, bpw, data);

      /* Patch bytes */
      for (i = 0; i < end_bytes; i++)
	{
	  buf[i] = bdata[start_bytes + word_count * bpw + i];
	}

      /* Write back */
      data = extract_unsigned_integer (buf, bpw);
      or1k_jtag_write_jtag_reg (aligned + word_count * bpw, data);
    }

  return  len;

}	/* or1k_jtag_write_mem() */


/*----------------------------------------------------------------------------*/
/*!Global function to stall the CPU

   Use the underlying JTAG protocol calls to select the relevant scan chain
   and change the relevant registers. Always succeeds.

   @todo The old code always read back at least twice after setting the stall
         bit. We've assumed that wasn't necessary (is any read back
         necessary?).

	 @todo The old code also disabled HW trace. Was that really necessary? */
/*---------------------------------------------------------------------------*/

void
or1k_jtag_stall()
{
  ULONGEST               data;

  /* Select the correct chain for controlling the processor */
  or1k_jtag_select_chain (OR1K_SC_REGISTER);
 
  /* Read the JTAG register, set the stall bit and write it back. */
  or1k_jtag_read_jtag_reg (OR1K_JTAG_RISCOP, &data);
  data  |= OR1K_JTAG_RISCOP_STALL;
  or1k_jtag_write_jtag_reg (OR1K_JTAG_RISCOP, data);

  /* Loop until we read back that the register is set. */
  do
    {
      or1k_jtag_read_jtag_reg (OR1K_JTAG_RISCOP, &data);
    }
  while (OR1K_JTAG_RISCOP_STALL != (data & OR1K_JTAG_RISCOP_STALL));

}	/* or1k_jtag_stall() */


/*----------------------------------------------------------------------------*/
/*!Global function to unstall the CPU

   Use the underlying JTAG protocol calls to select the relevant scan chain
   and change the relevant registers. Always succeeds.

   @note At one stage the code used to loop to check the stall flag had been
         cleared. However that could allow time for the target to execute and
         then stall again, so this function would hang. */
/*---------------------------------------------------------------------------*/

void
or1k_jtag_unstall()
{
  ULONGEST               data;

  /* Select the correct chain for controlling the processor */
  or1k_jtag_select_chain (OR1K_SC_REGISTER);
 
  /* Read the JTAG register, clear the stall bit and write it back */
  or1k_jtag_read_jtag_reg (OR1K_JTAG_RISCOP, &data);
  data &= ~OR1K_JTAG_RISCOP_STALL;
  or1k_jtag_write_jtag_reg (OR1K_JTAG_RISCOP, data);

}	/* or1k_jtag_unstall() */


/*----------------------------------------------------------------------------*/
/*!Global function to wait for the CPU to stall

   Use the underlying JTAG protocol calls to wait for the processor to
   stall. Always succeeds.

   The wait time should be small if we are single stepping, but can be longer
   otherwise. This is controlled by the argument.

   @param[in] fast  1 (true) if the wait should be quick. */
/*---------------------------------------------------------------------------*/

void
or1k_jtag_wait (int  fast)
{
  ULONGEST  data;

  /* Select the correct chain for controlling the processor */
  or1k_jtag_select_chain (OR1K_SC_REGISTER);
 
  /* Read the JTAG register every 10us until the stall flag is set. */
  do
    {
      if (fast)
	{
	  usleep (OR1K_JTAG_FAST_WAIT);
	}
      else
	{
	  usleep (OR1K_JTAG_SLOW_WAIT);
	}

      or1k_jtag_read_jtag_reg (OR1K_JTAG_RISCOP, &data);
    }
  while (OR1K_JTAG_RISCOP_STALL != (data & OR1K_JTAG_RISCOP_STALL));

}	/* or1k_jtag_wait() */


/* EOF */
