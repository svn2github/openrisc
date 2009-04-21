
LIB	= cmds.o
OBJS	= dhry.o eth.o cpu.o camera.o load.o memory.o global.o touch.o atabug.o hdbug.o

all:	$(LIB)

$(LIB):	$(OBJS)
	$(LD) -r -o $@ $(OBJS)

.depend:	Makefile $(OBJS:.o=.c)
		$(CC) -M $(CFLAGS) $(OBJS:.o=.c) > $@
sinclude .depend
