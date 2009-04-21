
LIB	= drivers.o
OBJS	= int.o eth.o uart.o tick.o flash.o keyboard.o spi.o ata.o smc91111.o

all:	$(LIB)

$(LIB):	$(OBJS)
	$(LD) -r -o $@ $(OBJS)

.depend:	Makefile $(OBJS:.o=.c)
		$(CC) -M $(CFLAGS) $(OBJS:.o=.c) > $@
sinclude .depend
