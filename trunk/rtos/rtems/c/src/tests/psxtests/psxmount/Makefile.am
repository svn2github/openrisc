##
## $Id: Makefile.am,v 1.2 2001-09-27 12:02:25 chris Exp $
##

AUTOMAKE_OPTIONS = foreign 1.4

TEST = psxmount

MANAGERS = all

C_FILES = main.c test.c
C_O_FILES = $(C_FILES:%.c=${ARCH}/%.o)

DOCTYPES = scn
DOCS = $(DOCTYPES:%=$(TEST).%)

SRCS = $(DOCS) $(C_FILES) $(H_FILES)
OBJS = $(C_O_FILES)

PRINT_SRCS = $(DOCS)

PGM = ${ARCH}/$(TEST).exe

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(RTEMS_ROOT)/make/leaf.cfg
include $(top_srcdir)/psxtests.am

#
# (OPTIONAL) Add local stuff here using +=
#

${PGM}: $(OBJS) $(LINK_FILES)
	$(make-exe)

all-local: $(ARCH) $(TMPINSTALL_FILES)

EXTRA_DIST = $(C_FILES) $(DOCS)

include $(top_srcdir)/../../../../automake/local.am
