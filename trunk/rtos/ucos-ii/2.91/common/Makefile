
# CFLAGS += -DET_DEBUG -DDEBUG

LIB	= common.o

OBJS	= cprintf_r.o ctype.o string.o main.o

all:	$(LIB)

$(LIB):	$(OBJS) $(SOBJS)
	$(LD) -r -o $@ $(OBJS) $(SOBJS)

#########################################################################

.depend:	Makefile $(OBJS:.o=.c)
		$(CC) -M $(CFLAGS) $(OBJS:.o=.c) > $@

sinclude .depend

#########################################################################
