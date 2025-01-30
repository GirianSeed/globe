#
#  IOP gcov support library Makefile
#
#  $Id: Makefile,v 1.1 2003/02/04 06:55:00 tei Exp $
#

#----------- customize section --------------
DBGFLAG  = -g
OPTFLAG  = -O2 -G0
CFLAGS   = -Wall $(DBGFLAG) $(OPTFLAG)
ASFLAGS  = $(DBGFLAG)
LDFLAGS  = $(DBGFLAG)
LOADLIBES=

PROG1 = iopgcovout.irx
PROG2 = iopgcovctl.irx
LIB   = libiopgcov.a
HEADER= iopgcov.h

all:	$(PROG1) $(PROG2) $(LIB)

clean:
	rm -f *.o $(PROG1) $(PROG2) $(LIB) *.map

install:
	install -m 644 $(LIB) /usr/local/sce/iop/install/lib
	install -m 644 $(HEADER) /usr/local/sce/iop/install/include
	install -m 644 $(PROG1) $(PROG2) /usr/local/sce/iop/modules
	@if [ -r /usr/local/sce/iop/install/Install.sh ]; then \
	    sh /usr/local/sce/iop/install/Install.sh; \
	fi

$(LIB): iop_bb.o
	$(AR) rcu $@ iop_bb.o

iopgcovout.o iopgcovctl.o iop_bb.o: iopgcovout.h
iopgcovout.o: gcov_io.h

#----------- rules --------------
%.irx: %.o
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@

-include PathDefs
PathDefs:
	iop-path-setup > PathDefs || (rm -f PathDefs ; exit 1)
