ifndef CROSS_COMPILE
CROSS_COMPILE = or32-elf-
endif

export	CROSS_COMPILE

#########################################################################

TOPDIR	:= $(shell if [ "$$PWD" != "" ]; then echo $$PWD; else pwd; fi)
export	TOPDIR

include $(TOPDIR)/config.mk

# order is important here:
SUBDIRS	= drivers common ucos ucos-port tasks

LIBS  =	common/common.o drivers/drivers.o ucos/ucos.o ucos-port/ucos-port.o \
	tasks/tasks.o

#########################################################################

UCOSAPP = ucos-tasks$(TASKS)

all: $(UCOSAPP).or32 $(UCOSAPP).bin $(UCOSAPP).srec $(UCOSAPP).img

$(UCOSAPP).or32: depend subdirs $(LIBS) Makefile
	$(CC) -Tram.ld -o $@ $(LIBS) -nostdlib $(LDFLAGS)

%.map: %.or32
	@$(NM) $< | \
		grep -v '\(compiled\)\|\(\.o$$\)\|\( [aUw] \)\|\(\.\.ng$$\)\|\(LASH[RL]DI\)' | \
		sort > $@

#########################################################################

depend dep:
	@for dir in $(SUBDIRS) ; do $(MAKE) -C $$dir .depend ; done

subdirs:
	@for dir in $(SUBDIRS) ; do $(MAKE) -C $$dir || exit 1 ; done

clean:
	find . -type f \
		\( -name 'core' -o -name '*.bak' -o -name '*~' \
		-o -name '*.o'  -o -name '*.tmp' -o -name '*.hex' \
		-o -name '*.or32' -o -name '*.bin' -o -name '*.srec' \
		-o -name '*.mem' -o -name '*.img' -o -name '*.out' \
		-o -name '*.aux' -o -name '*.log' \) -print \
		| xargs rm -f
	rm -f System.map

distclean: clean
	find . -type f \
		\( -name .depend -o -name '*.srec' -o -name '*.bin' \
		-o -name '*.pdf' \) \
		-print | xargs rm -f
	rm -f $(OBJS) *.bak tags TAGS
	rm -fr *.*~

#########################################################################
