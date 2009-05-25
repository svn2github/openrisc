##
## $Id: Makefile.am,v 1.2 2001-09-27 12:02:50 chris Exp $
##

AUTOMAKE_OPTIONS = foreign 1.4

rtems_makedir = $(prefix)/make

rtems_make_DATA = main.cfg leaf.cfg

EXTRA_DIST = README directory.cfg host.cfg.in leaf.cfg lib.cfg

SUBDIRS = Templates custom compilers

## -------------------------------------------------------
include $(top_srcdir)/automake/subdirs.am
include $(top_srcdir)/automake/host.am
