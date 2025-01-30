#
#  IOP test program Makefile
#
#  $Id: Makefile,v 1.2 1999/08/19 08:44:39 tei Exp $
#
include ../../../lib/Makedefs

CFLAGS  = -I../../../include -I. -Wall -G0
ASFLAGS  = -I../../../include -G0

#----------- customize section --------------
PROGNAME = rspu2drv

OBJS     = rsd_main.o  rsd_com.o rsd_cb.o 

#----------- rules --------------
all:	$(PROGNAME).irx

clean:
	rm -f *.o $(PROGNAME).irx *.obj *.map

$(PROGNAME).irx: $(OBJS)
	$(LINK.o)  -o $@ \
		$(OBJS) -L../../../lib -L./  -lsnd2 -lspu2 -ilb=../../../lib/iop.ilb
