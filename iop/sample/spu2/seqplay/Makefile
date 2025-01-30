#
#  IOP test program Makefile
#
#  $Id: Makefile,v 1.4 1999/08/18 07:07:00 tei Exp $
#
TOP = $(dir $(word 1, $(wildcard include ../include ../../include \
		../../../include ../../../../include ) ))

include $(TOP)lib/Makedefs

CFLAGS  = -I$(TOP)include -Wall -G 0
ASFLAGS  = -I$(TOP)include
LIBS	= -L$(TOP)lib
ILIBS	= -ilb=$(TOP)lib/iop.ilb 

#----------- customize section --------------
PROGNAME = main.irx

OBJS	= $(PROGNAME:%.irx=%.o)

all: $(PROGNAME)

$(PROGNAME): $(OBJS)
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@ \
		$(LIBS) -lsnd2 -lspu2 $(ILIBS)

#----------- rules --------------
clean:
	rm -f *.o $(PROGNAME) *.map

run:
	dsidb -r "bload simple.vh 0x160000; bload simple.vb 0x170000;bload fuga.seq 0x150000" ; dsreset 0 2 ; dsistart $(PROGNAME)

