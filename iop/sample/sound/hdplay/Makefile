#
#  IOP test program Makefile
#
ifeq ($(wildcard PathDefs),)
PathDefs:
	iop-path-setup > PathDefs || (rm -f PathDefs ; exit 1)
	$(MAKE) all
else
include PathDefs
endif

INCDIR  = -I../../../../common/include -I../../../include \
	-I/usr/local/sce/common/include 

#----------- customize section --------------
CFLAGS   = $(INCDIR) -Wall -G 0 -g 
ASFLAGS  = $(INCDIR)
LDLIBS   = 
PROGNAME = main
OBJS     = main.o 

#----------- rules --------------
$(PROGNAME).irx: $(OBJS)
	$(LINK.o)  -o $@ $(OBJS) -L../../../lib -L./ $(LOADLIBES) $(LDLIBS) \
	-ilb=libsd.ilb -ilb=sdhd.ilb 

all: $(PROGNAME).irx

clean:
	rm -f *.o $(PROGNAME) *.map

run: main.irx
	dsifilesv -c " dsreset 0 2;\
	dsistart ../../../modules/libsd.irx;\
	dsistart ../../../modules/sdhd.irx;\
	dsistart main.irx;"
