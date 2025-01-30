
SHELL       = /bin/sh

TOP         = ../../../../..
LIBDIR      = $(TOP)/ee/lib
INCDIR      = $(TOP)/ee/include
ILBDIR      = $(TOP)/ee/ilb

TARGET      = iga
OBJS        = iga.o \
              devinit.o \
              work.o \
              $(TARGET)_entry.o

ILIBS       = $(ILBDIR)/libkernl.ilb \
              $(ILBDIR)/libgraph.ilb \
              $(ILBDIR)/libdma.ilb \
              $(ILBDIR)/libdev.ilb \
              $(ILBDIR)/libpkt.ilb \
              $(ILBDIR)/libpad.ilb \
              $(ILBDIR)/libvu0.ilb \
              $(ILBDIR)/libc_stdio.ilb \
              $(ILBDIR)/libc_stdlib.ilb \
              $(ILBDIR)/libc_malloc.ilb \
              $(ILBDIR)/libm_float.ilb \
              ../baseelf.ilb

PREFIX      = ee
AS          = $(PREFIX)-gcc
CC          = $(PREFIX)-gcc
LD          = erx-gcc
DVPASM      = $(PREFIX)-dvp-as
OBJDUMP     = $(PREFIX)-objdump
RUN         = dsedb -r run
RM          = /bin/rm -f

LIBGEN      = $(PREFIX)libgen
LIBLD       = $(PREFIX)libld
FIXUP       = $(PREFIX)fixup

CFLAGS      = -O2 -Wall -W -Wa,-al -fno-common -DUSE_ERX=1
ASFLAGS     = -c -xassembler-with-cpp -Wa,-al
DVPASMFLAGS = 
LDFLAGS     = -Wl,-Map,$(TARGET).map -mno-crt0
TMPFLAGS    =

.SUFFIXES: .c .s .cc .dsm

all: $(TARGET).erx

$(TARGET)_entry.s: ../overlay.tbl
	$(LIBGEN) -e $@ $<

stub.s: $(OBJS) $(ILIBS)
	$(LIBLD) --ignore-undef -s stub.s $(OBJS) : $(ILIBS)

$(TARGET).erx: stub.s $(OBJS) $(LIBS)
	$(LD) -o $(@:.erx=.sym) stub.s $(OBJS) $(LIBS) $(LDFLAGS)
	$(FIXUP) -o $(TARGET).erx $(@:.erx=.sym)

.s.o:
	$(AS) $(ASFLAGS) $(TMPFLAGS) -I$(INCDIR) -o $@ $< > $*.lst

.dsm.o:
	$(CC) -I$(INCDIR) -x c -E $< > $*.tmp
	$(DVPASM) $(DVPASMFLAGS) -I$(INCDIR) -o $@ $*.tmp > $*.lst
	$(RM) $*.tmp

.c.o:
	$(CC) $(CFLAGS) $(TMPFLAGS) -I$(INCDIR) -c $< -o $*.o > $*.lst


run: $(TARGET).elf
	$(RUN) $(TARGET).elf

clean:
	$(RM) *.o *.map *.lst core *.dis *.erx *.sym
	$(RM) $(TARGET)_entry.s stub.s

