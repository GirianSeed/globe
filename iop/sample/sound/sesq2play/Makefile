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

INCDIR  = -I../../../include -I../../../../common/include \
	  -I/usr/local/sce/common/include

#----------- customize section --------------
CFLAGS   = $(INCDIR) -Wall -G 0 -g -O2
ASFLAGS  = $(INCDIR)
LDLIBS   = 
PROGNAME = setest
OBJS     = main.o


#----------- rules --------------
$(PROGNAME).irx: $(OBJS)
	$(LINK.o)  -o $@ $(OBJS) -L/home/tokiwa/source/g/iop/lib -L../../../lib -L./ $(LOADLIBES) $(LDLIBS) \
	-ilb=libsd.ilb -ilb=modhsyn.ilb -ilb=modsesq2.ilb

all: $(PROGNAME).irx

clean:
	rm -f *.o $(PROGNAME).irx *.map

run: all
	dsifilesv -c "\
	dsreset 0 2 ;\
	dsidb bload ../../../../data/sound/seq/ambulance.sq 101000;\
	dsidb bload ../../../../data/sound/wave/ambulance.hd 120000;\
	dsidb bload ../../../../data/sound/wave/ambulance.bd 150000;\
	dsistart ../../../modules/libsd.irx ;\
	dsistart ../../../modules/modhsyn.irx ;\
	dsistart ../../../modules/modsesq2.irx ;\
	dsistart $(PROGNAME).irx ;\
	"




