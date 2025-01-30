#
#  IOP hello program Makefile
#
#  $Id: Makefile,v 1.3 2003/04/30 09:06:34 ksh Exp $
#
ifeq ($(wildcard PathDefs),)
PathDefs:
	iop-path-setup > PathDefs || (rm -f PathDefs ; exit 1)
	$(MAKE) all
else
include PathDefs
endif

#----------- rules --------------
%.irx: %.o
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@

#----------- customize section --------------
CFLAGS   =  -Wall -g
ASFLAGS  =
LOADLIBES=
LDLIBS   = -ilb=inet.ilb
PROGNAME = netstat.irx
OBJS	 = $(PROGNAME:%.irx=%.o) # add here other objects

all:	$(PROGNAME)

clean:
	rm -f *.o $(PROGNAME) *.map PathDefs

$(PROGNAME:%.irx=%.o) : $(PROGNAME:%.irx=%.c)

$(PROGNAME): $(OBJS)
	$(LINK.o) $(LOADLIBES) $^ -o $@ $(LDLIBS)
