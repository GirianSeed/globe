
SHELL       = /bin/sh

TOP         = ../../../..
LIBDIR      = $(TOP)/ee/lib
INCDIR      = $(TOP)/ee/include -I$(TOP)/common/include
ILBDIR      = $(TOP)/ee/ilb

LIB_INST_DIR = $(TOP)/ee/lib
HDR_INST_DIR = $(TOP)/ee/include
ILB_INST_DIR = $(TOP)/ee/ilb
ERX_INST_DIR = $(TOP)/ee/modules
SRC_INST_DIR = /usr/local/sce/ee/src/lib/timer

TARGET      = libtimer

OBJS        = libtimer.o alarm.o delaythread.o
LIB_OBJS    = $(OBJS) $(TARGET)_entry.o erxexport.o
ERX_OBJS    = $(OBJS) $(TARGET)_entry.o modname.o

ILIBS       = $(ILBDIR)/libkernl.ilb \
              $(ILBDIR)/libc_string.ilb \
              $(ILBDIR)/libgcc_common.ilb


LCFILE      = $(LIBDIR)/app.cmd
LIBS        = \
              $(LIBDIR)/libkernl.a \
              $(LIBDIR)/libgraph.a \
              $(LIBDIR)/libpc.a \
              $(TARGET).a \

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
RUN         = dsedb  -r run
RM          = /bin/rm -f
INSTALL     = install


LIBGEN      = $(PREFIX)libgen
LIBLD       = $(PREFIX)libld
FIXUP       = $(PREFIX)fixup

ifeq "$(shell ee-gcc -dumpversion)" "2.9-ee-991111-01"
CFLAGS      = -O4 -G0 -g \
              -fverbose-asm \
              -ansi -Wall -W \
              -Waggregate-return \
              -Wbad-function-cast \
              -Wconversion \
              -Wcast-qual \
              -Wcast-align \
              -Wcomment \
              -Wchar-subscripts \
              -Wformat \
              -Wimplicit-function-declaration \
              -Wimplicit-int \
              -Wimport \
              -Winline \
              -Wmain \
              -Wmissing-braces \
              -Wmissing-declarations \
              -Wmissing-prototypes \
              -Wmissing-noreturn \
              -Wmultichar \
              -Wnested-externs \
              -Wparentheses \
              -Wpointer-arith \
              -Wredundant-decls \
              -Wshadow \
              -Wsign-compare \
              -Wstrict-prototypes \
              -Wunknown-pragmas \
              -Wredundant-decls \
              -Wwrite-strings \
              -DR5900
#             -Wtraditional -pedantic
endif

ifeq "$(shell ee-gcc -dumpversion)" "2.96-ee-001003-1"
CFLAGS      = -O4 -G0 -g \
              -fverbose-asm \
              -ansi -std=iso9899:1999 -Wall -W \
              -Waggregate-return \
              -Wbad-function-cast \
              -Wconversion \
              -Wcast-qual \
              -Wcast-align \
              -Wcomment \
              -Wchar-subscripts \
              -Wfloat-equal \
              -Wformat \
              -Wimplicit-function-declaration \
              -Wimplicit-int \
              -Wimport \
              -Winline \
              -Wlongjmp-clobbers \
              -Wmain \
              -Wmissing-braces \
              -Wmissing-declarations \
              -Wmissing-prototypes \
              -Wmissing-noreturn \
              -Wmultichar \
              -Wnested-externs \
              -Wnested-externs-else \
              -Wparentheses \
              -Wpakced \
              -Wpadded \
              -Wpointer-arith \
              -Wredundant-decls \
              -Wreturn-type \
              -Wshadow \
              -Wsign-compare \
              -Wstrict-prototypes \
              -Wswitch \
              -Wundef \
              -Wuninitialized \
              -Wunknown-pragmas \
              -Wunused-function \
              -Wunused-label \
              -Wunused-parameter \
              -Wunused-value \
              -Wunused-variable \
              -Wunreachable-code \
              -Wredundant-decls \
              -Wwrite-strings \
              -DR5900
#             -Wtraditional -pedantic
endif

ifeq "$(shell ee-gcc -dumpversion)" "3.2-ee-020826"
CFLAGS      = -O4 -G0 -g \
              -fverbose-asm \
              -ansi -std=iso9899:1999 -Wall -W \
              -Waggregate-return \
              -Wbad-function-cast \
              -Wconversion \
              -Wcast-qual \
              -Wcast-align \
              -Wcomment \
              -Wchar-subscripts \
              -Wdeprecated-declarations \
              -Wdisabled-optimization \
              -Wfloat-equal \
              -Wformat \
              -Wformat-extra-args \
              -Wformat-nonliteral \
              -Wformat-security \
              -Wformat-y2k \
              -Wimplicit-function-declaration \
              -Wimplicit-int \
              -Wimport \
              -Winline \
              -Wlongjmp-clobbers \
              -Wmain \
              -Wmissing-braces \
              -Wmissing-declarations \
              -Wmissing-format-attribute \
              -Wmissing-prototypes \
              -Wmissing-noreturn \
              -Wmultichar \
              -Wnested-externs \
              -Wpacked \
              -Wpadded \
              -Wparentheses \
              -Wpointer-arith \
              -Wredundant-decls \
              -Wreturn-type \
              -Wshadow \
              -Wsign-compare \
              -Wstrict-prototypes \
              -Wsystem-headers \
              -Wswitch \
              -Wundef \
              -Wuninitialized \
              -Wunreachable-code \
              -Wunused-function \
              -Wunused-label \
              -Wunused-parameter \
              -Wunused-value \
              -Wunused-variable \
              -Wunknown-pragmas \
              -Wredundant-decls \
              -Wwrite-strings \
              -DR5900
#             -Wtraditional -pedantic
endif

ifeq "$(shell ee-gcc -dumpversion)" "3.2-beta2-ee-030210"
CFLAGS      = -O4 -G0 -g \
              -fverbose-asm \
              -ansi -std=iso9899:1999 -Wall -W \
              -Waggregate-return \
              -Wbad-function-cast \
              -Wconversion \
              -Wcast-qual \
              -Wcast-align \
              -Wcomment \
              -Wchar-subscripts \
              -Wdeprecated-declarations \
              -Wdisabled-optimization \
              -Wfloat-equal \
              -Wformat \
              -Wformat-extra-args \
              -Wformat-nonliteral \
              -Wformat-security \
              -Wformat-y2k \
              -Wimplicit-function-declaration \
              -Wimplicit-int \
              -Wimport \
              -Winline \
              -Wlongjmp-clobbers \
              -Wmain \
              -Wmissing-braces \
              -Wmissing-declarations \
              -Wmissing-format-attribute \
              -Wmissing-prototypes \
              -Wmissing-noreturn \
              -Wmultichar \
              -Wnested-externs \
              -Wpacked \
              -Wpadded \
              -Wparentheses \
              -Wpointer-arith \
              -Wredundant-decls \
              -Wreturn-type \
              -Wshadow \
              -Wsign-compare \
              -Wstrict-prototypes \
              -Wsystem-headers \
              -Wswitch \
              -Wundef \
              -Wuninitialized \
              -Wunreachable-code \
              -Wunused-function \
              -Wunused-label \
              -Wunused-parameter \
              -Wunused-value \
              -Wunused-variable \
              -Wunknown-pragmas \
              -Wredundant-decls \
              -Wwrite-strings \
              -DR5900
#             -Wtraditional -pedantic
endif

ASFLAGS     = -c -xassembler-with-cpp
DVPASMFLAGS = -g
LDFLAGS     = -Wl,-Map,$(TARGET).map,--warn-common,--warn-constructors,--warn-multiple-gp,--warn-section-align,--cref -mno-crt0 -L$(LIBDIR)

INIT_OBJ    = libtimer.o


all: $(TARGET).a $(TARGET).erx $(TARGET).ilb
	

clean:
	$(RM) *.o *.map *.lst *.dis *.elf *.a
	$(RM) $(TARGET).ilb $(TARGET)_entry.s $(TARGET).erx $(TARGET).sym
	$(RM) stub.s
	$(RM) *.bak

$(TARGET).a: $(LIB_OBJS)
	$(RM) $@
	$(AR) r $(TARGET).a $(LIB_OBJS)
	$(RANLIB) $(TARGET).a

$(TARGET).ilb: $(TARGET).tbl
	$(LIBGEN) -d $@ $<

$(TARGET)_entry.s: $(TARGET).tbl
	$(LIBGEN) -e $@ $<

stub.s: $(ERX_OBJS)
	$(LD) $(LDFLAGS) -makestub -o $@ $(ERX_OBJS) $(ILIBS:%=-ilb=%)

$(TARGET).erx: stub.s $(ERX_OBJS)
	$(LD) $(LDFLAGS) -o $(@:.erx=.sym) stub.s $(ERX_OBJS) $(ILIBS:%=-ilb=%)
	$(FIXUP) -o $(TARGET).erx $(@:.erx=.sym)


.s.o:
	$(AS) $(ASFLAGS) $(TMPLAGS) -Wa,-al -I$(INCDIR) -o $@ $< > $*.lst

.c.o:
	$(CC) $(CFLAGS) -I$(INCDIR) -c $< -o $*.o > $*.lst

libtimer.o:	libtimer.c

alarm.o:	alarm.c

delaythread.o:	delaythread.c


install: all
	$(INSTALL) -d $(LIB_INST_DIR)
	$(INSTALL) -d $(ILB_INST_DIR)
	$(INSTALL) -d $(ERX_INST_DIR)
	cp $(TARGET).a $(LIB_INST_DIR)
	cp $(TARGET).ilb $(ILB_INST_DIR)
	cp $(TARGET).erx $(ERX_INST_DIR)
	cp $(TARGET).sym $(ERX_INST_DIR)
	$(INSTALL) -d $(SRC_INST_DIR)
#	cp Makefile       $(SRC_INST_DIR)
	cp alarm.c        $(SRC_INST_DIR)
	cp delaythread.c  $(SRC_INST_DIR)
	cp erxexport.c    $(SRC_INST_DIR)
#	cp libtimer.c     $(SRC_INST_DIR)
	cp libtimer.tbl   $(SRC_INST_DIR)
	cp modname.c      $(SRC_INST_DIR)


