SHELL       = /bin/sh

TOP         = /usr/local/sce
EETOP       = $(TOP)/ee
INCDIR      = $(EETOP)/include -I$(EETOP)/include/libeenet
ILBDIR      = $(EETOP)/ilb

ILIBS       = $(ILBDIR)/libkernl.ilb \
              $(ILBDIR)/libc_stdio.ilb \
              $(ILBDIR)/libc_string.ilb \
              $(ILBDIR)/libeenet.ilb \

TARGET      = routeinfo
OBJS        = $(TARGET).o
ERX_OBJS    = $(OBJS) erx_start.o

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
RUN         = dsestart

CFLAGS      = -c -Wa,-al -fno-common -O2 -G0
ASFLAGS     = -c -xassembler-with-cpp -Wa,-al 
DVPASMFLAGS = 
LDFLAGS     = -Wl,-Map,$(TARGET).map,--check-sections,--stats,--warn-common,--warn-constructors,--warn-multiple-gp,--warn-section-align,--cref -mno-crt0


.SUFFIXES: .c .s .cc .dsm

all: lib$(TARGET).a $(TARGET).erx

stub.s: $(ERX_OBJS) $(ILIBS)
	$(LD) $(LDFLAGS) -makestub -o $@ $(ERX_OBJS) $(ILIBS:%=-ilb=%)

$(TARGET).erx: stub.s $(ERX_OBJS)
	$(RM) $@
	$(LD) $(LDFLAGS) -o $(@:.erx=.sym) stub.s $(ERX_OBJS) $(ILIBS:%=-ilb=%)
	$(FIXUP) -o $(TARGET).erx $(@:.erx=.sym)

lib$(TARGET).a: $(OBJS)
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
	$(RM) *.o *.lst core *.elf *.a *.map *~
	$(RM) *.erx *.sym stub.s

run: $(TARGET).erx
	-$(RUN) $(TARGET).erx

