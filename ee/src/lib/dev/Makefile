### Makefile

SHELL=/bin/sh

TOP = ../../..
INCDIR = $(TOP)/include
LIB_INST_DIR = $(TOP)/lib
HDR_INST_DIR = $(TOP)/include
SRC_INST_DIR = /usr/local/sce/ee/src/lib/dev
TARGET = libdev
OBJS = devvu0.o devvu1.o devgif.o devvif0.o devvif1.o devfont.o

PREFIX=ee
AS = $(PREFIX)-gcc
CC = $(PREFIX)-gcc
AR = $(PREFIX)-ar
RANLIB = $(PREFIX)-ranlib
DVPASM = $(PREFIX)-dvp-as
RM=/bin/rm -f
INSTALL = install

CFLAGS = -O2 -Wall -Wpointer-arith -Wno-unused -Wa,-al -fno-exceptions -fno-common -G0 
ASFLAGS = -c -xassembler-with-cpp -Wa,-al
DVPASMFLAGS = -g 

.SUFFIXES: .c .s .cc .dsm

all: $(TARGET).a

$(TARGET).a: $(OBJS)
	$(RM) $@
	$(AR) r $@ $(OBJS)
	$(RANLIB) $@

.s.o:
	$(AS) $(ASFLAGS) -I$(INCDIR) -o $@ $< > $*.lst

.c.o:
	$(CC) $(CFLAGS) -I$(INCDIR) -c $< -o $*.o > $*.lst

.dsm.o:
	$(DVPASM) $(DVPASMFLAGS) -I$(INCDIR) -o $@ $< > $*.lst

clean:
	/bin/rm -f *.o *.lst core *.elf *.a *~

install: all
	$(INSTALL) -d $(LIB_INST_DIR)
	cp $(TARGET).a $(LIB_INST_DIR)
	$(INSTALL) -d $(SRC_INST_DIR)
	cp Makefile *.c devfont.inc $(SRC_INST_DIR)
