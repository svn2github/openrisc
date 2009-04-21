# (C) Marko Mlinar, based on ppcboot
#
# (C) Copyright 2000
# Wolfgang Denk, DENX Software Engineering, wd@denx.de.
#
# See file CREDITS for list of people who contributed to this
# project.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 2 of
# the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston,
# MA 02111-1307 USA
#

# CFLAGS += -DET_DEBUG -DDEBUG

LIB	= services.o

OBJS	= net.o tftp.o bootp.o rarp.o arp.o dos.o modem.o

all:	$(LIB)

$(LIB):	$(START) $(OBJS) $(AOBJS)
	$(LD) -r -o $@ $(OBJS) $(AOBJS)

#########################################################################

.depend:	Makefile $(OBJS:.o=.c)
		$(CC) -M $(CFLAGS) $(OBJS:.o=.c) > $@

sinclude .depend

#########################################################################
