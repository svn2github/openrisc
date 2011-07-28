include ../Makefile.inc

CFLAGS	+= -DIC=0 -DDC=0 

all: support.o int.o reset.o interrupts.o

%.o: %.c
	@echo "    CC $<"
	@$(CC) -c $(CFLAGS) $(INCDIRS) -o $@ $<

%.o: %.S
	@echo "    CC $<"
	@$(CC) -c $(CFLAGS) $(INCDIRS) -o $@ $<

clean:
	@rm -f *.a *.o
