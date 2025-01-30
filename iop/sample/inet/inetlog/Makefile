#
#  IOP hello program Makefile
#
#  $Id: Makefile,v 1.4 2002/12/12 00:29:07 ksh Exp $
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
MODVER   = ( ( 1 << 8 ) | 89 )
CFLAGS   =  -Wall -g -DMODVER="$(MODVER)"
ASFLAGS  =
LOADLIBES=
LDLIBS   = -ilb=inet.ilb
PROGNAME = inetlog.irx
OBJS	 = $(PROGNAME:%.irx=%.o) # add here other objects

all:	$(PROGNAME)

clean:
	rm -f *.o $(PROGNAME) *.map

$(PROGNAME:%.irx=%.o) : $(PROGNAME:%.irx=%.c)

$(PROGNAME): $(OBJS)
	$(LINK.o) $(LOADLIBES) $^ -o $@ $(LDLIBS)
