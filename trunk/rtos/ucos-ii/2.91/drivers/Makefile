
LIB	= drivers.o
OBJS	= uart.o console.o

all:	$(LIB)

$(LIB):	$(OBJS)
	$(LD) -r -o $@ $(OBJS)

.depend:	Makefile $(OBJS:.o=.c)
		$(CC) -M $(CFLAGS) $(OBJS:.o=.c) > $@
sinclude .depend
