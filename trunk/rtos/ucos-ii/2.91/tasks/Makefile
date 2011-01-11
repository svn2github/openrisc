
# User should set TASKS to determine which set to run

ifeq ($(TASKS),)
$(error Must specify TASKS, eg. TASKS=1 when building to choose user task program)
endif


LIB	= tasks.o
OBJS	= tasks$(TASKS).o

all:	$(LIB)

$(LIB):	$(OBJS)
	$(LD) -r -o $@ $(OBJS)

# Depends too difficult to deal with here

.depend:	Makefile
		$(CC) -M $(CFLAGS) *.c > $@
sinclude .depend


#.depend:	Makefile $(OBJS:.o=.c)
#		$(CC) -M $(CFLAGS) $(OBJS:.o=.c) > $@
#sinclude .depend
