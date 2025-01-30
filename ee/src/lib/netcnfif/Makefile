
SHELL       = /bin/sh

TOP         = ../../../..
INCDIR      = -I$(TOP)/ee/include -I$(TOP)/common/include
LIBDIR      = $(TOP)/ee/lib

TARGET      = netcnfif
OBJS        = $(TARGET).o \
              callback.o \
              convauth.o \
              dma.o \
              netcnfif_data.o \
              rpc.o \
              userdef.o

LIB_OBJS    = $(OBJS)

PREFIX      = ee
AS          = $(PREFIX)-gcc
CC          = $(PREFIX)-gcc
AR          = $(PREFIX)-ar
RANLIB      = $(PREFIX)-ranlib
DVPASM      = $(PREFIX)-dvp-as
RM          = /bin/rm -f

CFLAGS      = -c -Wa,-al -fno-common -O2 -G0 
LDFLAGS     = -Wl,-Map,$(TARGET).map,--check-sections,--stats,--warn-common,--warn-constructors,--warn-multiple-gp,--warn-section-align,--cref -mno-crt0

ASFLAGS     = -c -xassembler-with-cpp -Wa,-al 
DVPASMFLAGS = -g 

.SUFFIXES: .c .s .cc .dsm

all: $(TARGET).a

$(TARGET).a: $(LIB_OBJS)
	$(RM) $@
	$(AR) r $@ $(LIB_OBJS)
	$(RANLIB) $@

.s.o:
	$(AS) $(ASFLAGS) $(INCDIR) -o $@ $< > $*.lst

.c.o:
	$(CC) $(CFLAGS) $(INCDIR) -c $< -o $*.o > $*.lst

.dsm.o:
	$(DVPASM) $(DVPASMFLAGS) $(INCDIR) -o $@ $< > $*.lst

clean:
	$(RM) *.o *.lst core *.elf *.a *.sym *.ilb *.map

install: all
	cp $(TARGET).a $(LIBDIR)

