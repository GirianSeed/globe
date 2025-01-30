SHELL       = /bin/sh

TOP         = /usr/local/sce
EE_TOP      = $(TOP)/ee
LIBDIR      = $(EE_TOP)/lib
INCDIR      = $(EE_TOP)/include -I$(EE_TOP)/include/libeenet -I$(TOP)/common/include

TARGET      = main
OBJS        = crt0.o \
            $(TARGET).o \
            poweroff.o \

LCFILE      = $(LIBDIR)/app.cmd
LIBS        = \
            ../ent_cnf/ent_cnf.a \
            ../util/ifinfo/libifinfo.a \
            $(LIBDIR)/ent_dump.a \
            $(LIBDIR)/ent_smap.a \
            $(LIBDIR)/ent_eth.a \
            $(LIBDIR)/ent_ppp.a \
            $(LIBDIR)/eenetctl.a \
            $(LIBDIR)/libeenet.a \
            $(LIBDIR)/libcdvd.a \
            $(LIBDIR)/libscf.a \
            $(LIBDIR)/libgraph.a \

PREFIX      = ee
AS          = $(PREFIX)-gcc
CC          = $(PREFIX)-gcc
LD          = $(PREFIX)-gcc
DVPASM      = $(PREFIX)-dvp-as
OBJDUMP     = $(PREFIX)-objdump
RUN         = dsedb -r run
RM          = /bin/rm -f

CFLAGS      = -O2 -Wall -Wa,-al -fno-common -fno-strict-aliasing
CXXFLAGS    = -O0 -Wall -Wa,-al -fno-exceptions -fno-common -fno-strict-aliasing
ASFLAGS     = -c -xassembler-with-cpp -Wa,-al
DVPASMFLAGS =
LDFLAGS     = -Wl,-Map,$(TARGET).map -mno-crt0 -L$(LIBDIR) -lm

.SUFFIXES: .c .s .cc .dsm

all: $(TARGET).elf

$(TARGET).elf: $(OBJS) $(LIBS)
	$(LD) -o $@ -T $(LCFILE) $(OBJS) $(LIBS) $(LDFLAGS)

crt0.o: $(LIBDIR)/crt0.s
	$(AS) $(ASFLAGS) $(TMPFLAGS) -o $@ $< > $*.lst

.s.o:
	$(AS) $(ASFLAGS) $(TMPFLAGS) -I$(INCDIR) -o $@ $< > $*.lst

.dsm.o:
	$(DVPASM) $(DVPASMFLAGS) -I$(INCDIR) -o $@ $< > $*.lst

.c.o:
	$(CC) $(CFLAGS) $(TMPFLAGS) -I$(INCDIR) -c $< -o $*.o > $*.lst

.cc.o:
	$(CC) $(CXXFLAGS) $(TMPFLAGS) -I$(INCDIR) -c $< -o $*.o > $*.lst

$(EENET_TOP)/sample/ent_cnf/ent_cnf.a:
	cd $(EENET_TOP)/sample/ent_cnf; make

run: $(TARGET).elf
	$(RUN) $(TARGET).elf

clean:
	$(RM) *.o *.map *.lst core *.elf ,* *~ "#"*
	$(RM) tcpdump.log
