### Makefile

SHELL=/bin/sh

TOP = ../../..
INCDIR = $(TOP)/include

LIB_INST_DIR = $(TOP)/lib
HDR_INST_DIR = $(TOP)/include
SRC_INST_DIR = /usr/local/sce/ee/src/lib/dma
TARGET = libdma
OBJS = libdma.o defence.o 
#INST_HDRS = libdma.h 

PREFIX=ee
AS = $(PREFIX)-gcc
CC = $(PREFIX)-gcc
AR = $(PREFIX)-ar
RANLIB = $(PREFIX)-ranlib
DVPASM = $(PREFIX)-dvp-as
RM=/bin/rm -f
INSTALL = install


CFLAGS = -c -Wa,-al -fno-common -O2 -G0 
ASFLAGS = -c -xassembler-with-cpp -Wa,-al 
DVPASMFLAGS = -g 

INIT_OBJ = libdma.o defence.o

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
	/bin/rm -f *.o *.lst core *.elf *.a

install: all
	$(INSTALL) -d $(LIB_INST_DIR)
	cp $(TARGET).a $(LIB_INST_DIR)
	$(INSTALL) -d $(SRC_INST_DIR)


