##
## $Id: Makefile.am,v 1.2 2001-09-27 12:00:20 chris Exp $
##

AUTOMAKE_OPTIONS = foreign 1.4

VPATH = @srcdir@:@srcdir@/../../shared:@srcdir@/../../../shared

PGM = $(ARCH)/startup.rel

C_FILES = bspclean.c bsplibc.c bsppost.c bspstart.c bootcard.c \
    m68kpretaskinghook.c main.c page_table.c sbrk.c setvec.c \
    gnatinstallhandler.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.o)

OBJS = $(C_O_FILES)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

# USE_INIT_FINI tells main.c what C++ help we need.
AM_CPPFLAGS += -DUSE_INIT_FINI

$(PGM): $(OBJS)
	$(make-rel)

$(PROJECT_RELEASE)/lib/linkcmds: linkcmds
	$(INSTALL_DATA) $< $@

TMPINSTALL_FILES += $(PROJECT_RELEASE)/lib/linkcmds

all-local: $(ARCH) $(OBJS) $(PGM) $(TMPINSTALL_FILES)

.PRECIOUS: $(PGM)

EXTRA_DIST = bspclean.c bspstart.c linkcmds page_table.c

include $(top_srcdir)/../../../../../../automake/local.am
