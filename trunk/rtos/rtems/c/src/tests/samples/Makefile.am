##
## $Id: Makefile.am,v 1.2 2001-09-27 12:02:26 chris Exp $
##

AUTOMAKE_OPTIONS = foreign 1.4
ACLOCAL_AMFLAGS = -I $(RTEMS_TOPdir)/aclocal

CPLUSPLUS_TESTS = cdtest

## base_mp is a sample multiprocessing test
MP_TESTS = base_mp

FP_TESTS = paranoia

SUBDIRS = hello ticker base_sp unlimited minimum $(MP_TESTS) \
    $(CPLUSPLUS_TESTS) $(FP_TESTS)

AUTOMAKE_FILES = sample.am

EXTRA_DIST = $(AUTOMAKE_FILES)

include $(top_srcdir)/../../../../automake/subdirs.am
include $(top_srcdir)/../../../../automake/local.am
