##
## $Id: Makefile.am,v 1.2 2001-09-27 12:01:06 chris Exp $
##

AUTOMAKE_OPTIONS = foreign 1.4

C_FILES = em86.c lib.c misc.c mm.c pci.c zlib.c

H_FILES = bootldr.h zlib.h pci.h

S_FILES = em86real.S exception.S head.S

noinst_DATA = ppcboot.lds

EXTRA_DIST = README bootldr.h em86.c em86real.S exception.S head.S lib.c \
    misc.c mm.c pci.c pci.h ppcboot.lds zlib.c zlib.h

include $(top_srcdir)/../../../../../automake/local.am
