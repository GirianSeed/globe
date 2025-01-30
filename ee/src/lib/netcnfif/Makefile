#
#  Netcnf Interface Library's Makefile
#

SHELL = /bin/sh

TOP    = ../../..
INCDIR = -I$(TOP)/include
LIBDIR = $(TOP)/lib

TARGET = netcnfif
OBJS   = netcnfif.o netcnfif_data.o dma.o

PREFIX = ee
AS     = $(PREFIX)-gcc
CC     = $(PREFIX)-gcc
AR     = $(PREFIX)-ar
RANLIB = $(PREFIX)-ranlib
DVPASM = $(PREFIX)-dvp-as
RM     = /bin/rm -f

CFLAGS      = -Wa,-al -fno-common -O2 -G0 
ASFLAGS     = -xassembler-with-cpp -Wa,-al 
DVPASMFLAGS = -g 

.SUFFIXES: .c .s .cc .dsm

all: $(TARGET).a

$(TARGET).a: $(OBJS)
	$(RM) $@
	$(AR) r $@ $(OBJS)
	$(RANLIB) $@ 

.s.o:
	$(AS) $(ASFLAGS) $(INCDIR) $< > $*.lst

.c.o:
	$(CC) $(CFLAGS) $(INCDIR) -c $< > $*.lst

.dsm.o:
	$(DVPASM) $(DVPASMFLAGS) $(INCDIR) -o $@ $< > $*.lst

clean:
	$(RM) *.o *.lst core *.elf *.a *~

install: $(TARGET).a
	cp -p $(TARGET).a $(LIBDIR)
