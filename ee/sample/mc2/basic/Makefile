
SHELL	    = /bin/sh

TOP	= /usr/local/sce/ee
#TOP	= ../..
LIBDIR	    = $(TOP)/lib
INCDIR	    = -I$(TOP)/include -I$(TOP)/../common/include

TARGET	    = main
OBJS	    = crt0.o $(TARGET).o

LCFILE	    = $(LIBDIR)/app.cmd
LIBS	    = $(LIBDIR)/libgraph.a \
	      $(LIBDIR)/libdma.a \
	      $(LIBDIR)/libdev.a \
	      $(LIBDIR)/libpkt.a \
	      $(LIBDIR)/libkernl.a \
	      $(LIBDIR)/libpad2.a \
	      $(LIBDIR)/libmc2.a \
	      $(LIBDIR)/libdbc.a \
	      $(LIBDIR)/libcdvd.a

PREFIX	    = ee
AS	    = $(PREFIX)-gcc
CC	    = $(PREFIX)-gcc
LD	    = $(PREFIX)-gcc
DVPASM	    = $(PREFIX)-dvp-as
OBJDUMP	    = $(PREFIX)-objdump
RUN	    = dsedb -r run
RM	    = rm -f

CFLAGS	    = -G0 -g -O2 -Wall -Wa,-al -fno-common
CXXFLAGS    = -O0 -Wall -Werror -Wa,-al -fno-exceptions -fno-common
ASFLAGS	    = -c -xassembler-with-cpp -Wa,-al
DVPASMFLAGS = -g 
#LDFLAGS   = -Wl,-Map,$(TARGET).map -nostartfiles -L$(LIBDIR) -lm
LDFLAGS	    = -Wl,-Map,$(TARGET).map -mno-crt0 -L$(LIBDIR) -lm
TMPFLAGS    =

.SUFFIXES: .c .s .cc .dsm

all: $(TARGET).elf

$(TARGET).elf: $(OBJS) $(LIBS)
	$(LD) -o $@ -T $(LCFILE) $(OBJS) $(LIBS) $(LDFLAGS)

crt0.o: $(LIBDIR)/crt0.s
	$(AS) $(ASFLAGS) $(TMPFLAGS) -o $@ $< > $*.lst

.s.o:
	$(AS) $(ASFLAGS) $(TMPFLAGS) $(INCDIR) -o $@ $< > $*.lst

.dsm.o:
	$(DVPASM) $(DVPASMFLAGS) $(INCDIR) -o $@ $< > $*.lst

.c.o:
	$(CC) $(CFLAGS) $(TMPFLAGS) $(INCDIR) -c $< -o $*.o > $*.lst

.cc.o:
	$(CC) $(CXXFLAGS) $(TMPFLAGS) $(INCDIR) -c $< -o $*.o > $*.lst

run: $(TARGET).elf
	$(RUN) $(TARGET).elf

clean:
	$(RM) *.o *.map *.lst core *.dis *.elf
