SHELL       = /bin/sh

TOP         = ../../../..
EE_TOP      = $(TOP)/ee
INCDIR      = -I$(EE_TOP)/include -I$(EE_TOP)/include/libeenet -I.

TARGET      = ent_cnf
OBJS        = $(TARGET).o

PREFIX      = ee
AS          = $(PREFIX)-gcc
CC          = $(PREFIX)-gcc
AR          = $(PREFIX)-ar
RANLIB      = $(PREFIX)-ranlib
DVPASM      = $(PREFIX)-dvp-as
RM          = /bin/rm -f

CFLAGS      = -c -Wa,-al -fno-common -O2 -G0
ASFLAGS     = -c -xassembler-with-cpp -Wa,-al 
DVPASMFLAGS = -g 


.SUFFIXES: .c .s .cc .dsm

all: $(TARGET).a

$(TARGET).a: $(OBJS)
	$(RM) $@
	$(AR) r $@ $(OBJS)
	$(RANLIB) $@

.s.o:
	$(AS) $(ASFLAGS) $(INCDIR) -o $@ $< > $*.lst

.c.o:
	$(CC) $(CFLAGS) $(INCDIR) -c $< -o $*.o > $*.lst

.dsm.o:
	$(DVPASM) $(DVPASMFLAGS) $(INCDIR) -o $@ $< > $*.lst

clean:
	$(RM) *.o *.lst core *.elf *.a

