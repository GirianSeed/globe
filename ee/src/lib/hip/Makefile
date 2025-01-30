#! gmake
#
# Makefile for project programmer
#
#		start 2000.09.04
#

# directory
TOP		:= ../../..

BINDIR		:= $(TOP)/bin
LIBDIR		:= $(TOP)/lib
INCDIR		:= $(TOP)/include
ILBDIR		:= $(TOP)/ilb

# HIGDIR is directory-path where did install 'HiG'
HIGDIR		:= $(TOP)
#HIGDIR		:= $(TOP)/src/lib/hig

HIGLIBDIR	:= $(LIBDIR)
PLUGINDIR	:= $(LIBDIR)

#HIGLIBDIR	:= $(HIGDIR)/hig
#PLUGINIR	:= $(HIGDIR)/hip

HIGINCDIR	:= $(HIGDIR)/include

LIB_INST_DIR	:= $(HIGLIBDIR)
INC_INST_DIR	:= $(HIGINCDIR)

ERX_INST_DIR	:= $(TOP)/modules
ILB_INST_DIR	:= $(TOP)/ilb

#*******************************#
# CHANGE BELLOW FOR Ur PROJECT	#
#*******************************#
TARGET		:= libhip
TAGSUFFIX	:= a
CSRCS		:= micro.c tex2d.c shape.c hrchy.c anime.c share.c tim2.c \
			clutbump.c shadowmap.c shadowbox.c lightmap.c fisheye.c reflect.c refract.c \
			access.c clip.c skin.c camera.c matrix.c manime.c
LIBSRCS		:= erxexport.c
ERXSRCS		:= modname.c
CCSRCS		:=					# suffix .cc
ASRCS		:= $(TARGET)_entry.s			# suffix .s
DVPSRCS		:= vu1micro.dsm				# suffix .dsm

LIBS		:=

ILIBS		:= $(ILBDIR)/libhig.ilb \
		   $(ILBDIR)/libvu0.ilb \
		   $(ILBDIR)/libgraph.ilb \
		   $(ILBDIR)/libdma.ilb \
		   $(ILBDIR)/libkernl.ilb \
		   $(ILBDIR)/libc_string.ilb \
		   $(ILBDIR)/libm_float.ilb

LCFILE		:= $(LIBDIR)/app.cmd

# alias
MAKEFILE	:= Makefile
SHELL		:= /bin/sh
RM		:= /bin/rm -f
ECHO		:= /bin/echo
CP		:= /bin/cp
INSTALL		:= /usr/bin/install

# command for PS2
PREFIX		:= ee
CC		:= $(PREFIX)-gcc
CPP		:= $(CC)
LD		:= erx-gcc
LIBGEN		:= $(PREFIX)libgen
LIBLD		:= $(PREFIX)libld
FIXUP		:= $(PREFIX)fixup
AR		:= $(PREFIX)-ar
AS		:= $(PREFIX)-gcc
DVPASM		:= $(PREFIX)-dvp-as
OBJDUMP		:= $(PREFIX)-objdump

# FLAGS
INCLUDES	:= -I. -I$(HIGINCDIR) -I$(INCDIR)
CPPFLAGS	:= $(INCLUDES)
#CFLAGS		:= $(INCLUDES) -G0 -O2 -Wall -Wa,-al -fno-common
#CXXFLAGS	:= $(INCLUDES) -G0 -O2 -Wall -Wa,-al -fno-exceptions
#ASFLAGS		:= $(INCLUDES) -G0 -c -xassembler-with-cpp -Wa,-al
CFLAGS		:= $(INCLUDES) -G0 -O2 -Wall -Wa,-al -fno-common
CXXFLAGS	:= $(INCLUDES) -G0 -O2 -Wall -Wa,-al -fno-exceptions -fno-common
ASFLAGS		:= $(INCLUDES) -G0 -c -xassembler-with-cpp -Wa,-al
DVPASMFLAGS	:= $(INCLUDES)
ARFLAGS		:= rc
LDFLAGS		:= -Wl,-Map,$(TARGET).map --check-sections,--stats,--warn-common,--warn-constructors,--warn-multiple-gp,--warn-section-align,--cref -mno-crt0

OBJS		:= $(CSRCS:.c=.o) $(CCSRCS:.cc=.o) $(ASRCS:.s=.o) $(DVPSRCS:.dsm=.o)

LIB_OBJS	:= $(OBJS) $(LIBSRCS:.c=.o)
ERX_OBJS	:= $(OBJS) $(ERXSRCS:.c=.o)

.SUFFIXES: .c .s .cc .dsm .a .elf

default all : $(TARGET).$(TAGSUFFIX) $(TARGET).erx $(TARGET).ilb

clean :
	$(RM) core *.o *.map *.lst *.dis $(TARGET).elf $(TARGET).a *.d *~ $(TARGET)_entry.s $(TARGET).ilb $(TARGET).erx stub.s $(TARGET).sym

install: all
	$(INSTALL) -d $(LIB_INST_DIR)
	$(INSTALL) -d $(ILB_INST_DIR)
	$(INSTALL) -d $(ERX_INST_DIR)
	$(CP) $(TARGET).a $(LIB_INST_DIR)
	$(CP) $(TARGET).ilb $(ILB_INST_DIR)
	$(CP) $(TARGET).erx $(ERX_INST_DIR)
	$(CP) $(TARGET).sym $(ERX_INST_DIR)

$(TARGET).a : $(LIB_OBJS)
	$(AR) $(ARFLAGS) $(TARGET).a $(LIB_OBJS)

$(TARGET).elf : $(OBJS)
	$(LD) -o $@ -T $(LCFILE) $(OBJS) $(LIBS) $(LDFLAGS)

$(TARGET)_entry.s : $(TARGET).tbl
	$(LIBGEN) -e $@ $<

$(TARGET).ilb : $(TARGET).tbl
	$(LIBGEN) -d $@ $<

stub.s: $(ERX_OBJS) $(ILBS)
	$(LD) $(LDFLAGS) -makestub -o $@ $(ERX_OBJS) $(ILIBS:%=-ilb=%)

$(TARGET).erx: stub.s $(ERX_OBJS)
	$(LD) $(LDFLAGS) -o $(@:.erx=.sym) stub.s $(ERX_OBJS) $(ILIBS:%=-ilb=%)
	$(FIXUP) -o $(TARGET).erx $(@:.erx=.sym)

.c.o:
	$(CC) -c $(CFLAGS) $< -o $@ > $*.lst

.cc.o:
	$(CC) -c $(CXXFLAGS) $< -o $@ > $*.lst

.s.o:
	$(AS) $(ASFLAGS) -o $@ $< > $*.lst

.dsm.o:
	$(DVPASM) $(DVPASMFLAGS) -o $@ $< > $*.lst

$(CSRCS:.c=.d) :%.d : %.c
	set -e
	$(CPP) -M $(CPPFLAGS) $< | \
	sed 's/\($*\)\.o[ :]*/\1.o $@ :/g' > $@; \
	[ -s $@ ] || rm -f $@

$(CCSRCS:.cc=.d) : %.d : %.cc
	set -e
	$(CPP) -M $(CPPFLAGS) $< | \
	sed 's/\($*\)\.o[ :]*/\1.o $@ :/g' > $@; \
	[ -s $@ ] || rm -f $@

$(ASRCS:.s=.d) : %.d : %.s
	set -e
	$(CPP) -M $(CPPFLAGS) $< | \
	sed 's/\($*\)\.o[ :]*/\1.o $@ :/g' > $@; \
	[ -s $@ ] || rm -f $@

include $(CSRCS:.c=.d) $(CCSRCS:.cc=.d) $(ASRCS:.s=.d)
