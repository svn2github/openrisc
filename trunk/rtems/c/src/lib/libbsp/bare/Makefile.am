##
## $Id: Makefile.am,v 1.2 2001-09-27 11:59:44 chris Exp $
##

AUTOMAKE_OPTIONS = foreign 1.4
ACLOCAL_AMFLAGS = -I $(RTEMS_TOPdir)/aclocal

include $(top_srcdir)/../bsp.am

SUBDIRS = . include

EXTRA_DIST = bsp_specs build-tools do-bare-bsp-build

include $(top_srcdir)/../../../../../automake/subdirs.am
include $(top_srcdir)/../../../../../automake/local.am
