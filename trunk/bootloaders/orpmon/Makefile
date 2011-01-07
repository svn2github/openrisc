ifndef CROSS_COMPILE
CROSS_COMPILE = or32-elf-
endif

export	CROSS_COMPILE


#########################################################################

TOPDIR	:= $(shell if [ "$$PWD" != "" ]; then echo $$PWD; else pwd; fi)
export	TOPDIR

include $(TOPDIR)/config.mk

# order is important here:
SUBDIRS	= drivers common cmds coremark services doc

LIBS  =	common/common_o.o cmds/cmds.o coremark/coremark.o services/services.o \
	 drivers/drivers.o

#########################################################################


all: build.h orpmon.or32 orpmon-flash.or32

build.h:
	echo "#define BUILD_VERSION \"`date`\"" > $(TOPDIR)/include/build.h

docs: Makefile
	$(MAKE) -C doc docs || exit 1

reset.o: reset.S Makefile
	$(CC) -c -o $@ $< $(CFLAGS) -DIN_FLASH=0

reset-flash.o: flash_reset.S Makefile
	$(CC) -c -o $@ $< $(CFLAGS) -DIN_FLASH=1

orpmon.or32: depend subdirs reset.o $(LIBS) Makefile gencrc
	$(LD) -Tram.ld -o $@ reset.o -e 256 $(LIBS) $(LDFLAGS) 
	$(OBJCOPY) -O binary $@ $@.bin
#-S -j .vectors -j .text -j .data -j .rodata
#	./gencrc $@.tmp $@

orpmon-flash.or32: depend subdirs reset-flash.o $(LIBS) Makefile gencrc
	$(LD) -Tflash.ld -o $@ reset-flash.o $(LIBS) $(LDFLAGS) 
	$(OBJCOPY) -O binary $@ $@.tmp

#	../utils/bin2flimg 4 $@.tmp > $@.mem

gencrc: gencrc.c
	$(HOSTCC) -o gencrc -g gencrc.c

System.map: orpmon.or32
	@$(NM) $< | \
		grep -v '\(compiled\)\|\(\.o$$\)\|\( [aUw] \)\|\(\.\.ng$$\)\|\(LASH[RL]DI\)' | \
		sort > System.map

#########################################################################

depend dep:
	@for dir in $(SUBDIRS) ; do $(MAKE) -C $$dir .depend ; done

subdirs:
	@for dir in $(SUBDIRS) ; do $(MAKE) -C $$dir || exit 1 ; done

clean:
	find . -type f \
		\( -name 'core' -o -name '*.bak' -o -name '*~' \
		-o -name '*.o'  -o -name '*.tmp' \
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
	rm -f gencrc
#########################################################################
