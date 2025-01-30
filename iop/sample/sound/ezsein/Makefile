INCDIR  = -I/usr/local/sce/common/include

CFLAGS  = $(INCDIR) -Wall -G0 -g -O2
ASFLAGS  = $(INCDIR) -G0

COMPILE.s = $(CC) -xassembler-with-cpp $(ASFLAGS) $(CPPFLAGS) $(TARGET_MACH) -c

#----------- customize section --------------
PROGNAME=	ezsein

OBJS=		entry.o rpc.o command.o
ILIBS=		-ilb=libsd.ilb -ilb=modhsyn.ilb

#----------- rules --------------
all:	$(PROGNAME).irx

clean:
	$(RM) *.o $(PROGNAME).irx *.obj *.map PathDefs

$(PROGNAME).irx: $(OBJS)
	$(LINK.o)  -o $@ $(OBJS) -L../../../lib -L. $(ILIBS)


clear:
	$(RM) *~

ifeq ($(wildcard PathDefs),)
PathDefs:
	iop-path-setup > PathDefs || (rm -f PathDefs ; exit 1)
endif
include PathDefs
