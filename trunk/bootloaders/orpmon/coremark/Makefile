
LIB	= coremark.o
OBJS	= core_list_join.o  core_matrix.o  core_state.o  cvt.o fmod.o \
	core_main.o core_portme.o core_util.o ee_printf.o

all:	$(LIB)

$(LIB):	$(OBJS)
	$(LD) -r -o $@ $(OBJS)

.depend:	Makefile $(OBJS:.o=.c)
		$(CC) -M $(CFLAGS) $(OBJS:.o=.c) > $@
sinclude .depend
