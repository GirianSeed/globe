#			Copyright (C) 2002 Sony Computer Entertainment
#			All Right Reserved
#
#

SHELL       = /bin/sh

TOP         = /usr/local/sce
EE_TOP      = $(TOP)/ee
IOP_TOP     = $(TOP)/iop
LIBDIR      = $(EE_TOP)/lib
EE_ENT_CNF_DIR  = $(EE_TOP)/sample/libeenet/ent_cnf
IOP_ENT_CNF_DIR = $(IOP_TOP)/sample/libeenet/ent_cnf
INCDIR      = $(EE_TOP)/include -I$(EE_TOP)/include/libeenet -I$(TOP)/common/include -I$(EE_ENT_CNF_DIR) -I.. -I.

TARGET      = http_test.elf
OBJS        = crt0.o main.o ../$(TARGET:.elf=.o) ../poweroff.o

LCFILE      = $(LIBDIR)/app.cmd
LIBS        = $(LIBDIR)/libdev.a \
              $(LIBDIR)/libdma.a \
              $(LIBDIR)/libpkt.a \
              $(LIBDIR)/libpad.a \
              $(LIBDIR)/libcdvd.a \
              $(LIBDIR)/libvu0.a \
              $(LIBDIR)/libhttp.a \
              $(LIBDIR)/base64.a \
              $(LIBDIR)/libqp.a \
              $(LIBDIR)/netglue_eenet.a \
              $(EE_ENT_CNF_DIR)/ent_cnf.a \
              $(LIBDIR)/ent_smap.a \
              $(LIBDIR)/ent_eth.a \
              $(LIBDIR)/ent_ppp.a \
              $(LIBDIR)/ent_dump.a \
              $(LIBDIR)/eenetctl.a \
              $(LIBDIR)/libeenet.a \
              $(LIBDIR)/libscf.a \

PREFIX      = ee
AS          = $(PREFIX)-gcc
CC          = $(PREFIX)-gcc
LD          = $(PREFIX)-gcc
DVPASM      = $(PREFIX)-dvp-as
OBJDUMP     = $(PREFIX)-objdump
RUN         = dsedb -r run
RM          = /bin/rm -f

CFLAGS      = -Wall -Wa,-al -fno-common -Wstrict-prototypes
CXXFLAGS    = -O0 -Wall -Wa,-al -fno-exceptions -fno-common
ASFLAGS     = -c -xassembler-with-cpp -Wa,-al
DVPASMFLAGS = -g 
LDFLAGS     = -mno-crt0 -L$(LIBDIR) -lcdvd -lscf -lm

.SUFFIXES: .c .s .cc .dsm

all:  $(TARGET) iop-irx

iop-irx: $(IOP_ENT_CNF_DIR)/ent_cnf.irx

$(TARGET): $(OBJS) $(LIBS)
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

clean:
	$(RM) {.,..}/*.{o,map,lst,dis,elf} core *~ index.html
