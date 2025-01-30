#
#  Netcnf Interface Library's Makefile
#

TOP     = ../../..
MODDIR  = $(TOP)/modules
LIBDIR  = $(TOP)/lib

CFLAGS  = -W -Wall -Werror -G0 -s -O2 \
          -I$(TOP)/../common/include -I$(TOP)/include -I$(TOP)/../ee/include
LDLIBS  = -ilb=netcnf.ilb
LIBNAME = netcnfif
IRX     = $(LIBNAME).irx
ILB     = $(LIBNAME).ilb
OBJS    = $(LIBNAME:%=%.o) $(LIBNAME:%=%ent.o) \
          interface.o netcnfif_data.o netcnf_env.o read_env.o write_env.o \
          dma.o callback.o mem.o

all:	$(IRX)

clean:
	rm -f *.o $(IRX) *~ PathDefs

install: $(IRX)
	cp -p $(IRX) $(MODDIR)

$(IRX): $(OBJS)
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -s -o $@ # no symbols
#	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@

$(LIBNAME:%=%ent.o): $(LIBNAME).tbl
	$(ILBGEN) -e $(@:%.o=%.s) $^ 
	$(AS) -o $@ $(@:%.o=%.s)
	rm -f $(@:%.o=%.s)

$(ILB): $(LIBNAME).tbl
	$(ILBGEN) -d $@ $^

-include PathDefs
PathDefs:
	iop-path-setup > PathDefs || (rm -f PathDefs ; exit 1)
