#
#  Inet Event Sample Program's Makefile
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
CFLAGS   =  -Wall -G0
ASFLAGS  =
LDLIBS   = -ilb=inet.ilb -ilb=inetctl.ilb -ilb=netcnf.ilb \
	-ilb=usbmload.ilb
PROGNAME = event.irx
OBJS	 = $(PROGNAME:%.irx=%.o) get_event.o up_down.o if_id.o dump.o # add here other objects

all:	$(PROGNAME)

clean:
	rm -f *.o $(PROGNAME) *.map *~ *.log PathDefs

$(PROGNAME:%.irx=%.o) : $(PROGNAME:%.irx=%.c) # add your local include file

$(PROGNAME): $(OBJS)
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@
