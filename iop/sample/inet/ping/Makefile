#
# IOP ƒTƒ“ƒvƒ‹—p Makefile template
#

#----------- customize section --------------
INCDIR   = -I/usr/local/sce/common/include -I.
CFLAGS   =  -Wall -g $(INCDIR) -G0
ASFLAGS  =
LOADLIBES= -ilb=inet.ilb -ilb=cdvdman.ilb
PROGNAME = ping.irx
OBJS	 = $(PROGNAME:%.irx=%.o)

all:	$(PROGNAME)

clean:
	rm -f *.o $(PROGNAME) *.map PathDefs

run:	$(PROGNAME)
	dsreset 0 2 ; dsistart $(PROGNAME)

$(PROGNAME:%.irx=%.o) : $(PROGNAME:%.irx=%.c) # add your local include file

$(PROGNAME): $(OBJS)
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@

#----------- rules --------------
%.irx: %.o
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@

-include PathDefs
PathDefs:
	iop-path-setup > PathDefs || (rm -f PathDefs ; exit 1)

