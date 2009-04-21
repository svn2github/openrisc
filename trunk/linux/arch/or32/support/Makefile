#
# Makefile for the linux kernel.
#

obj-y		:= initrd.o 
                   # 8051-mem.o

IMAGE		:= initrd-fb-03.ext2
#IMAGE_8051	:= 8051.mem 

arch/$(ARCH)/support/initrd.o: arch/$(ARCH)/support/$(IMAGE) \
				arch/$(ARCH)/support/tools/bintoc 
	arch/$(ARCH)/support/tools/bintoc .initrd < arch/$(ARCH)/support/$(IMAGE) |$(AS) -o arch/$(ARCH)/support/initrd.o

arch/$(ARCH)/support/initrd-comp.o: arch/$(ARCH)/support/initrd-bb-9-or32.ext2 \
				arch/$(ARCH)/support/tools/bintoc 
	gzip -c arch/$(ARCH)/support/initrd-bb-9-or32.ext2 | arch/$(ARCH)/support/tools/bintoc |$(AS) -o arch/$(ARCH)/support/initrd.o

arch/$(ARCH)/support/8051-mem.o: arch/$(ARCH)/support/$(IMAGE_8051) \
				arch/$(ARCH)/support/tools/bintoc 
	arch/$(ARCH)/support/tools/bintoc .init.mem.8051 < arch/$(ARCH)/support/$(IMAGE_8051) |$(AS) -o arch/$(ARCH)/support/8051-mem.o

initrd-comp.o: initrd-bb-9-or32.ext2 tools/bintoc 
	gzip -c initrd-bb-9-or32.ext2 | tools/bintoc | $(AS) -o initrd.o

arch/$(ARCH)/support/tools/bintoc: arch/$(ARCH)/support/tools/bintoc.c
	$(HOSTCC) $(HOSTCFLAGS) -o $@ $@.c
