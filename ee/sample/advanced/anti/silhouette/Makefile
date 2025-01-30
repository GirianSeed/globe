
SHELL       = /bin/sh

TOP         = ../../../..
LIBDIR      = $(TOP)/lib
INCDIR      = $(TOP)/include

TARGET      = main
OBJS        = crt0.o $(TARGET).o \
	      packet.o \
              flower.o \
              mathfunc.o aa.o util.o

LCFILE      = $(LIBDIR)/app.cmd
LIBS        = $(LIBDIR)/libgraph.a \
              $(LIBDIR)/libgp.a \
              $(LIBDIR)/libdev.a \
              $(LIBDIR)/libpkt.a \
              $(LIBDIR)/libdma.a \
              $(LIBDIR)/libpad.a \
	      $(LIBDIR)/libvu0.a

INCLUDES =  -I. -I$(INCDIR)

PREFIX      = ee
AS          = $(PREFIX)-gcc
CC          = $(PREFIX)-gcc
LD          = $(PREFIX)-gcc
DVPASM      = $(PREFIX)-dvp-as
OBJDUMP     = $(PREFIX)-objdump
RUN         = dsedb -r run
RM          = /bin/rm -f

COMMONFLAGS = -g  -Wall -fno-strict-aliasing
CFLAGS      = $(COMMONFLAGS)
CXXFLAGS    = $(COMMONFLAGS) -fno-exceptions -fno-common
ASFLAGS     = -c -xassembler-with-cpp -Wa,-al
DVPASMFLAGS = 
LDFLAGS     = -Wl,-Map,$(TARGET).map -mno-crt0 -L$(LIBDIR) -lm
TMPFLAGS    =

.SUFFIXES: .c .s .cc .dsm

all: $(TARGET).elf

$(TARGET).elf: $(OBJS) $(LIBS)
	$(LD) -o $@ -T $(LCFILE) $(OBJS) $(LIBS) $(LDFLAGS)

crt0.o: $(LIBDIR)/crt0.s
	$(AS) $(ASFLAGS) $(TMPFLAGS) -o $@ $< > $*.lst

.s.o:
	$(AS) $(ASFLAGS) $(TMPFLAGS) $(INCLUDES) -o $@ $< > $*.lst

.dsm.o:
	$(DVPASM) $(DVPASMFLAGS) $(INCLUDES) -o $@ $< > $*.lst

.c.o:
	$(CC) $(CFLAGS) $(TMPFLAGS) $(INCLUDES) -c $< -o $*.o > $*.lst

.cc.o:
	$(CC) $(CXXFLAGS) $(TMPFLAGS) $(INCLUDES) -c $< -o $*.o > $*.lst

run: $(TARGET).elf
	$(RUN) $(TARGET).elf

clean:
	$(RM) *.o *.map *.lst core *.dis *.elf

aa.o: aa.h util.h
main.o: aa.h mathfunc.h util.h
mathfunc.o:  mathfunc.h
util.o: util.h
