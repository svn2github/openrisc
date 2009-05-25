##
## $Id: Makefile.am,v 1.2 2001-09-27 11:59:11 chris Exp $
##

AUTOMAKE_OPTIONS = foreign 1.4
ACLOCAL_AMFLAGS = -I $(RTEMS_TOPdir)/aclocal

rtems_makedir = $(prefix)/make

GENERIC_FILES = target.cfg

rtems_make_DATA = $(GENERIC_FILES)

EXTRA_DIST = target.cfg.in

include $(top_srcdir)/../../automake/host.am
