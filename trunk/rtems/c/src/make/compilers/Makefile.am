##
##  $Id: Makefile.am,v 1.2 2001-09-27 12:02:06 chris Exp $
##

AUTOMAKE_OPTIONS = foreign 1.4

rtems_makedir = $(prefix)/make
rtems_make_compilersdir = $(rtems_makedir)/compilers

## At the moment all actively supported configurations
## use gcc-target-default.cfg only.
rtems_make_compilers_DATA = gcc-target-default.cfg

include $(top_srcdir)/../../../automake/local.am
