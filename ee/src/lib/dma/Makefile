### Makefile

SHELL=/bin/sh

TOP = ../../..
INCDIR = $(TOP)/include

LIB_INST_DIR = $(TOP)/lib
HDR_INST_DIR = $(TOP)/include
ILB_INST_DIR = $(TOP)/ilb
ERX_INST_DIR = $(TOP)/modules
SRC_INST_DIR = /usr/local/sce/ee/src/lib/dma
TARGET = libdma
OBJS = libdma.o defence.o 
#INST_HDRS = libdma.h 

LIB_OBJS    = $(OBJS) $(TARGET)_entry.o erxexport.o
ERX_OBJS    = $(OBJS) $(TARGET)_entry.o modname.o
ILIBS       = $(ILB_INST_DIR)/libkernl.ilb

PREFIX=ee
AS = $(PREFIX)-gcc
CC = $(PREFIX)-gcc
AR = $(PREFIX)-ar
RANLIB = $(PREFIX)-ranlib
DVPASM = $(PREFIX)-dvp-as
LD          = erx-gcc
LIBGEN      = $(PREFIX)libgen
LIBLD       = $(PREFIX)libld
FIXUP       = $(PREFIX)fixup
RM=/bin/rm -f
INSTALL = install


CFLAGS = -c -Wa,-al -fno-common -O2 -G0 
ASFLAGS = -c -xassembler-with-cpp -Wa,-al 
DVPASMFLAGS = -g 
LDFLAGS     = -Wl,-Map,$(TARGET).map,--check-sections,--stats,--warn-common,--warn-constructors,--warn-multiple-gp,--warn-section-align,--cref -mno-crt0

INIT_OBJ = libdma.o defence.o

.SUFFIXES: .c .s .cc .dsm

all: $(TARGET).a $(TARGET).erx $(TARGET).ilb

$(TARGET).ilb: $(TARGET).tbl
	$(LIBGEN) -d $@ $<

$(TARGET).a: $(LIB_OBJS)
	$(RM) $@
	$(AR) r $@ $(LIB_OBJS)
	$(RANLIB) $@

$(TARGET)_entry.s: $(TARGET).tbl
	$(LIBGEN) -e $@ $<

stub.s: $(ERX_OBJS) $(ILBS)
	$(LD) $(LDFLAGS) -makestub -o $@ $(ERX_OBJS) $(ILIBS:%=-ilb=%)

$(TARGET).erx: stub.s $(ERX_OBJS)
	$(LD) $(LDFLAGS) -o $(@:.erx=.sym) stub.s $(ERX_OBJS) $(ILIBS:%=-ilb=%)
	$(FIXUP) -o $(TARGET).erx $(@:.erx=.sym)

.s.o:
	$(AS) $(ASFLAGS) -I$(INCDIR) -o $@ $< > $*.lst

.c.o:
	$(CC) $(CFLAGS) -I$(INCDIR) -c $< -o $*.o > $*.lst

.dsm.o:
	$(DVPASM) $(DVPASMFLAGS) -I$(INCDIR) -o $@ $< > $*.lst

clean:
	/bin/rm -f *.o *.lst *.map core *.elf *.a
	$(RM) $(TARGET)_entry.s stub.s $(TARGET).ilb *.erx *.sym

install: all
	$(INSTALL) -d $(LIB_INST_DIR)
	$(INSTALL) -d $(ILB_INST_DIR)
	$(INSTALL) -d $(ERX_INST_DIR)
	cp $(TARGET).a $(LIB_INST_DIR)
	cp $(TARGET).ilb $(ILB_INST_DIR)
	cp $(TARGET).erx $(ERX_INST_DIR)
	cp $(TARGET).sym $(ERX_INST_DIR)
	$(INSTALL) -d $(SRC_INST_DIR)


