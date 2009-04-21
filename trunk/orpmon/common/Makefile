
# CFLAGS += -DET_DEBUG -DDEBUG

LIB	= common_o.o

OBJS	= common.o support.o cprintf.o screen.o font.o ctype.o string.o
SOBJS   = or32.o

all:	$(LIB)

$(LIB):	$(OBJS) $(SOBJS)
	$(LD) -r -o $@ $(OBJS) $(SOBJS)

#########################################################################

.depend:	Makefile $(OBJS:.o=.c) $(SOBJS:.o=.S)
		$(CC) -M $(CFLAGS) $(OBJS:.o=.c) > $@
		$(CC) -M $(CFLAGS) $(SOBJS:.o=.S) > $@

sinclude .depend

#########################################################################
