
SHELL       = /bin/sh

TOP         = ../../../..
INCDIR      = $(TOP)/ee/include
LIBDIR      = $(TOP)/ee/lib
ILBDIR      = $(TOP)/ee/ilb

TARGET      = ipumpeg

OBJS        = modname.o
LIBS        = $(LIBDIR)/libipu.a \
              $(LIBDIR)/libmpeg.a

ILIBS       = $(ILBDIR)/libkernl.ilb \
              $(ILBDIR)/libc_stdio.ilb \
              $(ILBDIR)/libc_string.ilb \
              $(ILBDIR)/libgcc_common.ilb


PREFIX      = ee
AS          = $(PREFIX)-gcc
CC          = $(PREFIX)-gcc
LD          = erx-gcc
AR          = $(PREFIX)-ar
RANLIB      = $(PREFIX)-ranlib
DVPASM      = $(PREFIX)-dvp-as
OBJDUMP     = $(PREFIX)-objdump
STRIP       = $(PREFIX)-strip
READELF     = $(PREFIX)-readelf
BIN2ELF     = bin2elf
RUN         = dsedb -r run
CP          = /bin/cp
RM          = /bin/rm -f

LIBDUMP     = $(PREFIX)libdump
LIBGEN      = $(PREFIX)libgen
LIBLD       = $(PREFIX)libld
FIXUP       = $(PREFIX)fixup

CFLAGS      = -O4 -G 0 -g \
              -ansi -std=iso9899:1999 -pedantic -Wall -W \
              -Wtraditional -Wshadow -Wpointer-arith -Wbad-function-cast -Wcast-qual -Wcast-align -Wwrite-strings -Wconversion -Wsign-compare -Waggregate-return \
              -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -Wmissing-noreturn -Wredundant-decls -Wnested-externs -Winline \
              -DR5900

ASFLAGS     = -c -xassembler-with-cpp
DVPASMFLAGS = -g
LDFLAGS     = -Wl,-Map,$(TARGET).map,--check-sections,--stats,--warn-common,--warn-constructors,--warn-multiple-gp,--warn-section-align,--cref,--whole-archive -mno-crt0


all: $(TARGET).erx
	

clean:
	$(RM) *.o *.map *.lst *.dis *.erx *.sym *.a
	$(RM) stub.s
	$(RM) *.bak


stub.s: $(OBJS) $(LIBS)
	$(LD) $(LDFLAGS) -makestub -o $@ $(OBJS) $(LIBS) $(ILIBS:%=-ilb=%)

$(TARGET).erx: stub.s $(OBJS) $(LIBS)
	$(LD) $(LDFLAGS) -o $(@:.erx=.sym) stub.s $(OBJS) $(LIBS) $(ILIBS:%=-ilb=%)
	$(FIXUP) -o $@ $(@:.erx=.sym)
	$(LIBDUMP) $@

.s.o:
	$(AS) $(ASFLAGS) $(TMPLAGS) -Wa,-al -I$(INCDIR) -o $@ $< > $*.lst

.c.o:
	$(CC) $(CFLAGS)  $(TMPLAGS) -Wa,-al -I$(INCDIR) -c $< -o $*.o > $*.lst


