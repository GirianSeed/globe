#
#  IOP test program Makefile
#
#  $Id: Makefile,v 1.6 1999/08/18 07:07:00 tei Exp $
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
		$(LIBS) -lspu2 $(ILIBS)

#----------- rules --------------
clean:
	rm -f *.o $(PROGNAME) *.map

run:
	dsidb -r "bload knot_l.int 100000" ; dsreset 0 2 ; dsistart $(PROGNAME)

