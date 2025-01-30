#
# IOP sample Makefile template
#

INCDIR   = -I/usr/local/sce/common/include -I.
CFLAGS   =  -Wall -g $(INCDIR)
ASFLAGS  =
LOADLIBES= -ilb=inet.ilb -ilb=cdvdman.ilb
PROGNAME = ftp.irx
OBJS	 = $(PROGNAME:%.irx=%.o) \
           hddpart.o # add here other objects

all:	$(PROGNAME)

clean:
	rm -f *.o $(PROGNAME) PathDefs

run:	$(PROGNAME)
	dsreset 0 2 ; dsistart $(PROGNAME)

$(PROGNAME:%.irx=%.o) : $(PROGNAME:%.irx=%.c) # add your local include file

$(PROGNAME): $(OBJS)
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@

%.irx: %.o
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@

-include PathDefs
PathDefs:
	iop-path-setup > PathDefs || (rm -f PathDefs ; exit 1)

