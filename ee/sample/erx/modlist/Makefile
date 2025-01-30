
SHELL       = /bin/sh

TOP         = ../../../..
INCDIR      = $(TOP)/ee/include -I../../include
ILBDIR      = $(TOP)/ee/ilb

TARGET      = modlist
OBJS        = modlist.o
ILIBS       = \
              $(ILBDIR)/liberx.ilb \
              $(ILBDIR)/libkernl.ilb \
              $(ILBDIR)/libc_stdio.ilb

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
RUN         = dsestart
RM          = /bin/rm -f

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
LDFLAGS     = -Wl,-Map,$(TARGET).map,--check-sections,--stats,--warn-common,--warn-constructors,--warn-multiple-gp,--warn-section-align,--cref -mno-crt0


all: $(TARGET).erx
	

clean:
	$(RM) *.o *.map *.lst *.dis *.elf *.a
	$(RM) $(TARGET).erx $(TARGET).sym
	$(RM) stub.s
	$(RM) *.bak

run: $(TARGET).erx
	-$(RUN) $(TARGET).erx

.s.o:
	$(AS) $(ASFLAGS) $(TMPLAGS) -Wa,-al -I$(INCDIR) -o $@ $< > $*.lst

.c.o:
	$(CC) $(CFLAGS)   $(TMPLAGS) -Wa,-al -I$(INCDIR) -c $< -o $*.o > $*.lst

stub.s: $(OBJS)
	$(LD) $(LDFLAGS) -makestub -o $@ $(OBJS) $(ILIBS:%=-ilb=%)

$(TARGET).erx: stub.s $(OBJS)
	$(LD) $(LDFLAGS) -o $(@:.erx=.sym) stub.s $(OBJS) $(ILIBS:%=-ilb=%)
	$(FIXUP) -o $(TARGET).erx $(@:.erx=.sym)

modlist.o:	modlist.c


