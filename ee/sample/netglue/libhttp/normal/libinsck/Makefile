#			Copyright (C) 2002 Sony Computer Entertainment
#			All Right Reserved
#
#

SHELL       = /bin/sh

TOP         = /usr/local/sce/ee
LIBDIR      = $(TOP)/lib
INCDIR      = $(TOP)/include -I$(TOP)/../common/include -I..

TARGET      = http_test.elf
OBJS        = crt0.o main.o ../$(TARGET:.elf=.o) ../poweroff.o

LCFILE      = $(LIBDIR)/app.cmd
LIBS        = $(LIBDIR)/libdev.a \
              $(LIBDIR)/libdma.a \
              $(LIBDIR)/libpkt.a \
              $(LIBDIR)/libpad.a \
              $(LIBDIR)/libcdvd.a \
              $(LIBDIR)/libvu0.a \
              $(LIBDIR)/libnet.a \
              $(LIBDIR)/libnetif.a \
              $(LIBDIR)/libmrpc.a \
              $(LIBDIR)/libhttp.a \
              $(LIBDIR)/base64.a \
              $(LIBDIR)/libqp.a \
              $(LIBDIR)/netglue_insck.a \
              $(LIBDIR)/libinsck.a \
              $(LIBDIR)/netcnfif.a \

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

all:  $(TARGET)

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

clean:
	$(RM) {.,..}/*.{o,map,lst,dis,elf,sym} stub.s core *~ index.html
