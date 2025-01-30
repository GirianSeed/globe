
SHELL       = /bin/sh

TOP         = ../../../../..
INCDIR      = -I$(TOP)/ee/include -I$(TOP)/common/include -I$(TOP)/ee/include/libeenet
LIBDIR      = $(TOP)/ee/lib
ILBDIR      = $(TOP)/ee/ilb
ERXDIR      = $(TOP)/ee/modules

TARGET      = netglue_eenet
TARGET_ERX  = ntg_ent
TARGET_ILB  = netglue
OBJS        = $(TARGET).o

LIB_OBJS    = $(OBJS) $(TARGET)_entry.o erxexport.o
ERX_OBJS    = $(OBJS) $(TARGET)_entry.o modname.o
TBL         = ../tbl/netglue.tbl

ILIBS       = $(ILBDIR)/libkernl.ilb \
              $(ILBDIR)/libeenet.ilb

PREFIX      = ee
AS          = $(PREFIX)-gcc
CC          = $(PREFIX)-gcc
AR          = $(PREFIX)-ar
RANLIB      = $(PREFIX)-ranlib
DVPASM      = $(PREFIX)-dvp-as
RM          = /bin/rm -f

LD          = erx-gcc
LIBGEN      = $(PREFIX)libgen
LIBLD       = $(PREFIX)libld
FIXUP       = $(PREFIX)fixup

CFLAGS      = -c -W -Wall -Werror -Wa,-al -fno-common -O2 -G0 -D__ELF__ 
LDFLAGS     = -Wl,-Map,$(TARGET).map,--check-sections,--stats,--warn-common,--warn-constructors,--warn-multiple-gp,--warn-section-align,--cref -mno-crt0

ASFLAGS     = -c -xassembler-with-cpp -Wa,-al 
DVPASMFLAGS = -g 

.SUFFIXES: .c .s .cc .dsm

all: $(TARGET).a $(TARGET).erx $(TARGET).ilb
	mv $(TARGET).ilb $(TARGET_ILB).ilb
	mv $(TARGET).erx $(TARGET_ERX).erx
	mv $(TARGET).sym $(TARGET_ERX).sym

$(TARGET).a: $(LIB_OBJS)
	$(RM) $@
	$(AR) r $@ $(LIB_OBJS)
	$(RANLIB) $@

$(TARGET)_entry.s: $(TBL)
	$(LIBGEN) -e $@ $<

$(TARGET).ilb: $(TBL)
	$(LIBGEN) -d $@ $<

stub.s: $(ERX_OBJS) $(ILBS)
	$(LD) $(LDFLAGS) -makestub -o $@ $(ERX_OBJS) $(ILIBS:%=-ilb=%)

$(TARGET).erx: stub.s $(ERX_OBJS)
	$(LD) $(LDFLAGS) -o $(@:.erx=.sym) stub.s $(ERX_OBJS) $(ILIBS:%=-ilb=%)
	$(FIXUP) -o $(TARGET).erx $(@:.erx=.sym)

.s.o:
	$(AS) $(ASFLAGS) $(INCDIR) -o $@ $< > $*.lst

.c.o:
	$(CC) $(CFLAGS) $(INCDIR) -c $< -o $*.o > $*.lst

.dsm.o:
	$(DVPASM) $(DVPASMFLAGS) $(INCDIR) -o $@ $< > $*.lst

clean:
	$(RM) *.o *.lst core *.elf *.a *.erx *.sym *.ilb *.map
	$(RM) $(TARGET)_entry.s stub.s

install: all
	cp $(TARGET).a $(LIBDIR)
	cp $(TARGET_ILB).ilb $(ILBDIR)
	cp $(TARGET_ERX).erx $(ERXDIR)
	cp $(TARGET_ERX).sym $(ERXDIR)

