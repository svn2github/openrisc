#########################################################################

CONFIG_SHELL	:= $(shell if [ -x "$$BASH" ]; then echo $$BASH; \
		    else if [ -x /bin/bash ]; then echo /bin/bash; \
		    else echo sh; fi ; fi)

HOSTCC		= cc
HOSTCFLAGS	= -Wall -Wstrict-prototypes -fno-omit-frame-pointer


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

XCFLAGS += -I$(TOPDIR)/include -DOR1K -Wall -Wstrict-prototypes
XCFLAGS += -Werror-implicit-function-declaration 
#XCFLAGS += -fno-omit-frame-pointer
XCFLAGS += -fomit-frame-pointer
#XCFLAGS += -O0
#XCFLAGS += -O2
XCFLAGS += -O3
#XCFLAGS += -Os
XCFLAGS += -fno-strength-reduce -pipe -fno-builtin
# Use all software flags, so is compatible with minimal implementation
XCFLAGS += -mhard-mul -mhard-div -msoft-float 
#XCFLAGS += -mhard-mul -msoft-div -msoft-float 
#XCFLAGS += -msoft-mul -msoft-div -msoft-float 
# We require this as we we won't rely on any existing C library code
XCFLAGS += -nostdlib
#XCFLAGS += -DDEBUG

# For CoreMark:
FLAGS_STR ="$(XCFLAGS)"
# Add back to CFLAGS
CFLAGS += $(XCFLAGS) -DFLAGS_STR=\"$(FLAGS_STR)\"

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
