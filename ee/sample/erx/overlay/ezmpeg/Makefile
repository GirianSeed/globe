
SHELL       = /bin/sh

TOP         = ../../../../..
LIBDIR      = $(TOP)/ee/lib
INCDIR      = $(TOP)/ee/include -I$(TOP)/common/include
ILBDIR      = $(TOP)/ee/ilb

TARGET      = ezmpeg
OBJS        = display.o \
	      ezmpeg.o \
	      ldimage.o \
              $(TARGET)_entry.o

ILIBS       = $(ILBDIR)/libkernl.ilb \
              $(ILBDIR)/libgraph.ilb \
              $(ILBDIR)/libdma.ilb \
              $(ILBDIR)/libpad.ilb \
	      $(ILBDIR)/libmpeg.ilb \
	      $(ILBDIR)/libipu.ilb \
	      $(ILBDIR)/libc_stdio.ilb \
	      $(ILBDIR)/libc_stdlib.ilb \
	      $(ILBDIR)/libc_malloc.ilb \
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

CFLAGS      = -O2 -Wall -Wa,-al -DUSE_ERX=1
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
	$(DVPASM) $(DVPASMFLAGS) -I$(INCDIR) -o $@ $< > $*.lst

.c.o:
	$(CC) $(CFLAGS) $(TMPFLAGS) -I$(INCDIR) -c $< -o $*.o > $*.lst

clean:
	$(RM) *.o *.map *.lst core *.dis *.erx *.sym
	$(RM) $(TARGET)_entry.s stub.s

