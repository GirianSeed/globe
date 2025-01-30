SHELL       = /bin/sh

TOP         = /usr/local/sce
EE_TOP      = $(TOP)/ee
IOP_TOP     = $(TOP)/iop
LIBDIR      = $(EE_TOP)/lib
INCDIR      = $(EE_TOP)/include -I$(EE_TOP)/include/libeenet -I$(TOP)/common/include -I../ent_cnf -I.

EE_ENT_CNF_DIR  = ../ent_cnf
IOP_ENT_CNF_DIR = $(IOP_TOP)/sample/libeenet/ent_cnf

TARGET      = http
OBJS        = crt0.o \
              $(TARGET).o \
              poweroff.o \
              
LCFILE      = $(LIBDIR)/app.cmd
LIBS        = $(EE_ENT_CNF_DIR)/ent_cnf.a \
              $(LIBDIR)/ent_smap.a \
              $(LIBDIR)/ent_eth.a \
              $(LIBDIR)/ent_ppp.a \
              $(LIBDIR)/eenetctl.a \
              $(LIBDIR)/libeenet.a \
              $(LIBDIR)/libcdvd.a \
              $(LIBDIR)/libscf.a \

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

all: $(TARGET).elf iop-irx

iop-irx: $(IOP_ENT_CNF_DIR)/ent_cnf.irx

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

$(EE_ENT_CNF_DIR)/ent_cnf.a:
	cd $(EE_ENT_CNF_DIR); make

$(IOP_ENT_CNF_DIR)/ent_cnf.irx:
	cd $(IOP_ENT_CNF_DIR); make

run: $(TARGET).elf
	$(RUN) $(TARGET).elf

clean:
	$(RM) *.o *.map *.lst core *.elf ,* *~ "#"*

