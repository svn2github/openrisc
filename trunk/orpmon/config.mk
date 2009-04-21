#########################################################################

CONFIG_SHELL	:= $(shell if [ -x "$$BASH" ]; then echo $$BASH; \
		    else if [ -x /bin/bash ]; then echo /bin/bash; \
		    else echo sh; fi ; fi)

HOSTCC		= cc
HOSTCFLAGS	= -Wall -Wstrict-prototypes -O2 -fomit-frame-pointer

#
# Specify the path to the tool chain
#
TOOL_PREFIX = /tools/or32-uclinux

#########################################################################

#
# Include the make variables (CC, etc...)
#
AS	= $(CROSS_COMPILE)as
LD	= $(CROSS_COMPILE)ld
CC	= $(CROSS_COMPILE)gcc
AR	= $(CROSS_COMPILE)ar
NM	= $(CROSS_COMPILE)nm
STRIP	= $(CROSS_COMPILE)strip
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump
RANLIB	= $(CROSS_COMPILE)ranlib

CFLAGS += -I$(TOPDIR)/include -DOR1K -Wall -Wstrict-prototypes
CFLAGS += -Werror-implicit-function-declaration -fomit-frame-pointer
CFLAGS += -fno-strength-reduce -O2 -g -pipe -fno-builtin
#CFLAGS += -msoft-mul -msoft-div -nostdlib
CFLAGS += -nostdlib

LIBGCC := $(shell $(CC) $(CFLAGS) -print-libgcc-file-name)
LDFLAGS+= $(LIBGCC)

#########################################################################

export	CONFIG_SHELL HOSTCC HOSTCFLAGS CROSS_COMPILE \
	AS LD CC AR NM STRIP OBJCOPY OBJDUMP \
	MAKE CFLAGS ASFLAGS

#########################################################################

%.o:	%.S
	$(CC) $(CFLAGS) -c -o $@ $(CURDIR)/$<
%.o:	%.c
	$(CC) $(CFLAGS) -c -o $@ $<

%.bin: %.or32
	or32-rtems-objcopy -O binary $< $@

%.img: %.bin
	utils/bin2flimg 1 $< > $@

%.srec: %.bin
	utils/bin2srec $< > $@

#########################################################################
