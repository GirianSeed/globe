#
#  IOP thread sample program Makefile
#
#  $Id: Makefile,v 1.10 2002/05/19 12:42:14 tei Exp $
#

#----------- customize section --------------
DBGFLAG  = -g
#OPTFLAG  = -O2
CFLAGS   = -Wall $(DBGFLAG) $(OPTFLAG)
ASFLAGS  = $(DBGFLAG)
LDFLAGS  = $(DBGFLAG)
LOADLIBES=

PROG1 = createth.irx
PROG2 = sleepth.irx
PROG3 = eventth.irx
PROG4 = startthargs.irx
PROG9 = profiling.irx

all:	$(PROG1) $(PROG2) $(PROG3) $(PROG4) $(PROG9)

clean:
	rm -f *.o $(PROG1) $(PROG2) $(PROG3) $(PROG4) $(PROG9) *.map

#----------- rules --------------
%.irx: %.o
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@

-include PathDefs
PathDefs:
	iop-path-setup > PathDefs || (rm -f PathDefs ; exit 1)
