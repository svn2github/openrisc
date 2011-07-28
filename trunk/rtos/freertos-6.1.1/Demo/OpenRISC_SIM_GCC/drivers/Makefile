include ../Makefile.inc

INCDIR 	= -I../arch
CFLAGS	+= $(INCDIR)

all: libdrivers.a

libdrivers.a: uart.o
	@$(AR) cru $@ $<
	@$(RANLIB) $@
	@echo Complete $@

%.o: %.c
	@echo "    CC $<"
	@$(CC) -c $(CFLAGS) $(INCDIRS) -o $@ $<

%.o: %.S
	@echo "    CC $<"
	@$(CC) -c $(CFLAGS) $(INCDIRS) -o $@ $<

clean:
	@rm -f *.a *.o
