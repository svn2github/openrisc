##
## $Id: Makefile.am,v 1.2 2001-09-27 11:59:35 chris Exp $
##

AUTOMAKE_OPTIONS = foreign 1.4

if HAS_POSIX
POSIX_DIRS = posix
endif

if HAS_ITRON
ITRON_DIRS = itron
endif

SUBDIRS = rtems $(POSIX_DIRS) $(ITRON_DIRS)

include $(top_srcdir)/../../../automake/subdirs.am
include $(top_srcdir)/../../../automake/local.am
