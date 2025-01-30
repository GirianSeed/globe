#
#  IOP thread sample program Makefile
#
#  $Id: Makefile,v 1.5 1999/08/18 07:06:21 tei Exp $
#
TOP = $(dir $(word 1, $(wildcard include ../include ../../include \
		../../../include ../../../../include ) ))

include $(TOP)lib/Makedefs

CFLAGS  = -I$(TOP)include -Wall
ASFLAGS  = -I$(TOP)include

#----------- customize section --------------
PROG1 = createth.irx

PROG2 = sleepth.irx

PROG3 = eventth.irx

PROG4 = startthargs.irx

#----------- rules --------------
%.irx: %.o
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@ \
		-ilb=$(TOP)lib/iop.ilb -L$(TOP)lib

all:	$(PROG1) $(PROG2) $(PROG3) $(PROG4)

clean:
	rm -f *.o $(PROG1) $(PROG2) $(PROG3) $(PROG4) *.map

run1:
	dsreset 0 2 ; dsistart $(PROG1)

run2:
	dsreset 0 2 ; dsistart $(PROG2)

run3:
	dsreset 0 2 ; dsistart $(PROG3)

run4:
	dsreset 0 2 ; dsistart $(PROG3)
