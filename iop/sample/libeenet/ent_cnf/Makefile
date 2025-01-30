#
# IOP sample Makefile template
#

TOP      = ../../../../
EE_TOP   = $(TOP)/ee
IOP_TOP  = $(TOP)/iop

INCDIR   = -I$(IOP_TOP)/include -I$(TOP)/common/include -I. \
           -I$(EE_TOP)/sample/libeenet/ent_cnf
CFLAGS   = -Wall $(INCDIR) -G0
ASFLAGS  =
LOADLIBES= -ilb=eenetctl.ilb -ilb=netcnf.ilb
PROGNAME = ent_cnf.irx
OBJS	 = $(PROGNAME:%.irx=%.o) # add here other objects

all:	$(PROGNAME)

clean:
	rm -f *.o $(PROGNAME) PathDefs

$(PROGNAME:%.irx=%.o) : $(PROGNAME:%.irx=%.c) # add your local include file

$(PROGNAME): $(OBJS)
	$(ILBGEN) $(PROGNAME:%.irx=%.tbl) -e $(PROGNAME:%.irx=%ent.s)
	$(AS) -o $(PROGNAME:%.irx=%ent.o) $(PROGNAME:%.irx=%ent.s)
	$(RM) -f $(PROGNAME:%.irx=%ent.s)
	$(LINK.o) $(OBJS) $(PROGNAME:%.irx=%ent.o) $(LOADLIBES) $(LDLIBS) -o $@

%.irx: %.o
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@

-include PathDefs
PathDefs:
	iop-path-setup > PathDefs || (rm -f PathDefs ; exit 1)

