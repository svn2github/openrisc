##
## $Id: Makefile.am,v 1.2 2001-09-27 12:00:14 chris Exp $
##

AUTOMAKE_OPTIONS = foreign 1.4

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../../automake/lib.am

if HAS_MP
GENERIC_MP_REL_PIECES = shmdr
endif
GENERIC_PIECES += $(GENERIC_MP_REL_PIECES)

if HAS_MP
BSP_MP_O_PIECES = shmsupp
endif
BSP_PIECES = startup clock console timer $(BSP_MP_O_PIECES)

# bummer; have to use $foreach since % pattern subst rules only replace 1x
OBJS = $(foreach piece, $(BSP_PIECES), $(wildcard ../$(piece)/$(ARCH)/*.o)) \
    $(foreach piece, $(GENERIC_PIECES), ../../../$(piece)/$(ARCH)/$(piece).rel)
LIB = $(ARCH)/libbsp.a

#
# (OPTIONAL) Add local stuff here using +=
#

$(LIB): $(OBJS)
	$(make-library)

$(PROJECT_RELEASE)/lib/libbsp$(LIB_VARIANT).a: $(LIB)
	$(INSTALL_DATA) $< $@

TMPINSTALL_FILES += $(PROJECT_RELEASE)/lib/libbsp$(LIB_VARIANT).a

all-local: $(ARCH) $(OBJS) $(LIB) $(TMPINSTALL_FILES)

.PRECIOUS: $(LIB)

include $(top_srcdir)/../../../../../../automake/local.am
