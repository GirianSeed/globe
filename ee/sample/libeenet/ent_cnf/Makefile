SHELL       = /bin/sh

TOP         = /usr/local/sce
EETOP       = $(TOP)/ee
IOPTOP      = $(TOP)/iop
INCDIR      = $(EETOP)/include -I$(EETOP)/include/libeenet -I$(TOP)/common/include
ILBDIR      = $(TOP)/ee/ilb

ILIBS       = $(ILBDIR)/libkernl.ilb \
              $(ILBDIR)/libc_stdio.ilb \
              $(ILBDIR)/libc_string.ilb


TARGET      = ent_cnf
OBJS        = ent_cnf.o
			  
LIB_OBJS    = $(OBJS) $(TARGET)_entry.o erxexport.o
ERX_OBJS    = $(OBJS) $(TARGET)_entry.o modname.o

IOPENTCNFDIR= $(IOPTOP)/sample/libeenet/ent_cnf
TARGETIOP   = $(IOPENTCNFDIR)/ent_cnf.irx

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

CFLAGS      = -c -Wa,-al -fno-common -Wall -O2 -G0
ASFLAGS     = -c -xassembler-with-cpp -Wa,-al 
DVPASMFLAGS = 
LDFLAGS     = -Wl,-Map,$(TARGET).map,--check-sections,--stats,--warn-common,--warn-constructors,--warn-multiple-gp,--warn-section-align,--cref -mno-crt0


.SUFFIXES: .c .s .cc .dsm

all: $(TARGET).a $(TARGET).erx $(TARGETIOP)

$(TARGET).a: $(LIB_OBJS)
	$(RM) $@
	$(AR) r $@ $(LIB_OBJS)
	$(RANLIB) $@

stub.s: $(ERX_OBJS) $(ILIBS)
	$(LD) $(LDFLAGS) -makestub -o $@ $(ERX_OBJS) $(ILIBS:%=-ilb=%)

$(TARGET).erx: $(ERX_OBJS) stub.s
	$(RM) $@
	$(LD) $(LDFLAGS) -o $(@:.erx=.sym) stub.s $(ERX_OBJS) $(ILIBS:%=-ilb=%)
	$(FIXUP) -o $(TARGET).erx $(@:.erx=.sym)

$(TARGET)_entry.s: $(TARGET).tbl
	$(LIBGEN) -d $(TARGET).ilb -e $@ $<				

.s.o:
	$(AS) $(ASFLAGS) -I$(INCDIR) -o $@ $< > $*.lst

.c.o:
	$(CC) $(CFLAGS) -I$(INCDIR) -c $< -o $*.o > $*.lst

.dsm.o:
	$(DVPASM) $(DVPASMFLAGS) -I$(INCDIR) -o $@ $< > $*.lst

$(TARGETIOP):
	cd $(IOPENTCNFDIR); make

clean:
	$(RM) *.o *.lst core *.elf *.a *.map
	$(RM) *.erx *.sym $(TARGET)_entry.* stub.s $(TARGET).ilb

