##
## $Id: Makefile.am,v 1.2 2001-09-27 12:02:07 chris Exp $
##

AUTOMAKE_OPTIONS = foreign 1.4

rtems_makedir = $(prefix)/make
rtems_make_customdir = $(rtems_makedir)/custom
rtems_make_custom_DATA = @CUSTOM_CFG_FILES@

noinst_DATA = default.cfg

include $(top_srcdir)/../../../automake/local.am
