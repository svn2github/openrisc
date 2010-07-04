##
## $Id: Makefile.am,v 1.2 2001-09-27 11:59:29 chris Exp $
##

AUTOMAKE_OPTIONS = foreign 1.4
ACLOCAL_AMFLAGS = -I $(RTEMS_TOPdir)/aclocal

if PPC_NEW_EXCEPTION_PROCESSING
CPUDIR = new_exception_processing
else
CPUDIR = old_exception_processing
endif

SUBDIRS = shared $(CPUDIR)

include $(top_srcdir)/../../../../../../automake/subdirs.am
include $(top_srcdir)/../../../../../../automake/local.am
