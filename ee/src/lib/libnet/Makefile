#
#	EE Libnet Makefile
#

SHELL        = /bin/sh

TOP          = ../../../..
SCETOP       = /usr/local/sce
#TOP         = $(SCETOP)
EETOP        = $(TOP)/ee
SCEEETOP     = $(SCETOP)/ee

INCDIR       = -I$(EETOP)/include -I$(TOP)/common/include
PUB_INCDIR   = -I$(EETOP)/include

TARGET       = libnet
OBJS         = $(TARGET).o \
               compatible.o

PREFIX       = ee
AS           = $(PREFIX)-gcc
CC           = $(PREFIX)-gcc
AR           = $(PREFIX)-ar
RANLIB       = $(PREFIX)-ranlib
DVPASM       = $(PREFIX)-dvp-as
RM           = /bin/rm -f

CFLAGS       = -Wa,-al -fno-common -O2 -G0 
ASFLAGS      = -xassembler-with-cpp -Wa,-al 
DVPASMFLAGS  = -g 


.SUFFIXES: .c .s .cc .dsm

all: $(TARGET).a

$(TARGET).a: $(OBJS)
	$(RM) $@
	$(AR) r $@ $(OBJS)
	$(RANLIB) $@

.s.o:
	$(AS) $(ASFLAGS) $(INCDIR) $(PUB_INCDIR) $< > $*.lst

.c.o:
	$(CC) $(CFLAGS) $(INCDIR) $(PUB_INCDIR) -c $< > $*.lst

.dsm.o:
	$(DVPASM) $(DVPASMFLAGS) $(INCDIR) $(PUB_INCDIR) -o $@ $< > $*.lst

clean:
	$(RM) *.o *.lst core *.elf *.a

clear:
	$(RM) *~

install: all
	cp $(TARGET).a $(EETOP)/lib
	cp $(TARGET).a $(SCEEETOP)/lib

