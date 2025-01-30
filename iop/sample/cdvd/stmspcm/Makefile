#
#  IOP test program Makefile
#
#  $Id: Makefile,v 1.4.16.1 2003/06/04 10:36:08 kashiwa Exp $
#
ifeq ($(wildcard PathDefs),)
PathDefs:
	iop-path-setup > PathDefs || (rm -f PathDefs ; exit 1)
	$(MAKE) all
else
include PathDefs
endif

INCDIR  = -I/usr/local/sce/common/include

#----------- rules --------------
%.irx: %.o
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@

#----------- customize section --------------
CFLAGS   = $(INCDIR) -Wall -G 0
ASFLAGS  = $(INCDIR)
LDLIBS   = -ilb=cdvdman.ilb -ilb=libsd.ilb
PROGNAME = main.irx

all: $(PROGNAME)

clean:
	rm -f *.o $(PROGNAME) *.map
run: all
	dsifilesv -c "\
	dsreset 0 2;\
	dsistart ../../../modules/libsd.irx;\
	dsistart ../../../modules/cdvdstm.irx;\
	dsistart $(PROGNAME)"

