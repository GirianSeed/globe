### Makefile

SHELL=/bin/sh

TOP = ../../..
INCDIR = $(TOP)/include
ILBDIR = $(TOP)/ilb

LIB_INST_DIR = $(TOP)/lib
HDR_INST_DIR = $(TOP)/include

ERX_INST_DIR = $(TOP)/modules
ILB_INST_DIR = $(TOP)/ilb

TARGET = libvu0
OBJS = libvu0.o
LIB_OBJS = $(OBJS) $(TARGET)_entry.o erxexport.o
ERX_OBJS = $(OBJS) $(TARGET)_entry.o modname.o
ILIBS = $(ILBDIR)/libkernl.ilb
INST_HDRS = libvu0.h

PREFIX=ee
AS = $(PREFIX)-gcc
CC = $(PREFIX)-gcc
AR = $(PREFIX)-ar
RANLIB = $(PREFIX)-ranlib
DVPASM = $(PREFIX)-dvp-as
LD	= erx-gcc
LIBGEN 	= $(PREFIX)libgen
LIBLD 	= $(PREFIX)libld
FIXUP	= $(PREFIX)fixup
RM=/bin/rm -f
INSTALL = install

CFLAGS = -c -Wall -Wa,-al -fno-common -O2 -G0 
ASFLAGS = -c -xassembler-with-cpp -Wall -Wa,-al
DVPASMFLAGS = -g 
LDFLAGS	= -Wl,-Map,$(TARGET).map,--check-sections,--stats,--warn-common,--warn-constructors,--warn-multiple-gp,--warn-section-align,--cref -mno-crt0

.SUFFIXES: .c .s .cc .dsm

default all: safe

safe: OPT = -DSCE_VU0_SAFE_CONTEXT
safe: erase $(TARGET).a $(TARGET).erx $(TARGET).ilb
	@echo "******** Context-Safe libvu0 created"

unsafe: erase $(TARGET).a $(TARGET).erx $(TARGET).ilb
	@echo "******** Context-Unsafe libvu0 created"

erase:
	$(RM) $(OBJS)

$(TARGET).a: $(LIB_OBJS)
	$(RM) $@
	$(AR) r $@ $(LIB_OBJS)
	$(RANLIB) $@

$(TARGET)_entry.s: $(TARGET).tbl
	$(LIBGEN) -e $@ $<

$(TARGET).ilb: $(TARGET).tbl
	$(LIBGEN) -d $@ $<

stub.s: $(ERX_OBJS) $(ILBS)
	$(LD) $(LDFLAGS) -makestub -o $@ $(ERX_OBJS) $(ILIBS:%=-ilb=%)

$(TARGET).erx: stub.s $(ERX_OBJS)
	$(LD) $(LDFLAGS) -o $(@:.erx=.sym) stub.s $(ERX_OBJS) $(ILIBS:%=-ilb=%)
	$(FIXUP) -o $(TARGET).erx $(@:.erx=.sym)

.s.o:
	$(AS) $(ASFLAGS) -I$(INCDIR) -o $@ $< > $*.lst

.c.o:
	$(CC) $(CFLAGS) $(OPT) -I$(INCDIR) -c $< -o $*.o > $*.lst

.dsm.o:
	$(DVPASM) $(DVPASMFLAGS) -I$(INCDIR) -o $@ $< > $*.lst

clean:
	/bin/rm -f *.o *.lst core *.elf *.a $(TARGET)_entry.s $(TARGET).ilb stub.s $(TARGET).erx $(TARGET).sym $(TARGET).map

install:
	$(INSTALL) -d $(LIB_INST_DIR)
	$(INSTALL) -d $(ILB_INST_DIR)
	$(INSTALL) -d $(ERX_INST_DIR)
	cp $(TARGET).a $(LIB_INST_DIR)
	cp $(TARGET).ilb $(ILB_INST_DIR)
	cp $(TARGET).erx $(ERX_INST_DIR)
	cp $(TARGET).sym $(ERX_INST_DIR)
