##
## $Id: Makefile.am,v 1.2 2001-09-27 12:01:25 chris Exp $
##

AUTOMAKE_OPTIONS = foreign 1.4

# PROC_SPECIFIC_O_FILES = exceptions mmu clock
PROC_SPECIFIC_O_FILES = exceptions mmu
GENERIC_FILES =

# bummer; have to use $foreach since % pattern subst rules only replace 1x
OBJS = \
    $(foreach piece, $(PROC_SPECIFIC_O_FILES), ../../mpc6xx/$(piece)/$(ARCH)/*.o) \
    $(foreach piece, $(GENERIC_FILES), ../../mpc6xx/$(piece)/$(ARCH)/*.o)
LIB = $(ARCH)/libcpuspec.a

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

$(LIB): ${OBJS}
	echo $(OBJ)
	$(make-library)
	cp $(LIB) ..

all-local: ${ARCH} $(LIB)

include $(top_srcdir)/../../../../../automake/local.am
