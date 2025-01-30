#!gmake
#
# Makefile for User Application Program using HiG Library
#
#		start 2000.09.04
#

# directory
TOP             := /usr/local/sce/ee
BINDIR          := $(TOP)/bin
LIBDIR          := $(TOP)/lib
INCDIR          := $(TOP)/include
 
# HIGDIR is directory-path where did install 'HiG'
HIGDIR          := $(TOP)
 
HIGLIBDIR       := $(LIBDIR)
HIGINCDIR       := $(INCDIR)
PLUGINDIR       := $(LIBDIR)
                               
#*******************************#
# CHANGE BELLOW FOR Ur PROJECT	#
#*******************************#
TARGET		:= main
TAGSUFFIX	:= elf
CSRCS		:= main.c camera.c light.c util.c collision.c\
                   sample.c
CCSRCS		:=				# suffix .cc
ASRCS		:= 				# suffix .s
DVPSRCS		:=  vu0Code.dsm	# suffix .dsm
RUNTIME		:= crt0.o
LIBS		:= $(PLUGINDIR)/libhip.a	\
		   $(HIGLIBDIR)/libhig.a	\
		   $(LIBDIR)/libgraph.a		\
		   $(LIBDIR)/libdev.a		\
		   $(LIBDIR)/libdma.a		\
		   $(LIBDIR)/libpad.a		\
		   $(LIBDIR)/libpkt.a		\
		   $(LIBDIR)/libvu0.a
VSMDIR		:= ./micro
LCFILE		:= app.cmd

# alias
MAKEFILE	:= Makefile
SHELL		:= /bin/sh
RM		:= /bin/rm -f
ECHO		:= /bin/echo
CP		:= /bin/cp

# command for PS2
GCCDIR		:= $(TOP)/gcc
GCCBIN		:= $(GCCDIR)/bin
CC		:= $(GCCBIN)/ee-gcc
CPP		:= $(CC)
LD		:= $(GCCBIN)/ee-gcc
AR		:= $(GCCBIN)/ee-ar
AS		:= $(GCCBIN)/ee-gcc
DVPASM		:= $(GCCBIN)/ee-dvp-as
OBJDUMP		:= $(GCCBIN)/ee-objdump
RUN		:= dsedb -r run

# FLAGS
INCLUDES	:= -I. -I$(HIGINCDIR) -I$(INCDIR)
CPPFLAGS	:= $(INCLUDES)
CFLAGS		:= $(INCLUDES) -G0 -O2  -Wall -Wa,-al -fno-common
#CFLAGS		:= $(INCLUDES) -G0 -O2  -Wall -Wa,-al -fno-common -D _DOUBLE_OBJ
CXXFLAGS	:= $(INCLUDES) -G0 -O2  -Wall -Wa,-al -fno-exceptions -fno-common
ASFLAGS		:= $(INCLUDES) -G0 -c -xassembler-with-cpp -Wa,-al
DVPASMFLAGS	:= $(INCLUDES) -I$(VSMDIR)
ARFLAGS		:= rc
LDFLAGS		:= -Wl,-Map,$(TARGET).map -mno-crt0 -L$(LIBDIR) -lm

OBJS		:= $(RUNTIME) $(CSRCS:.c=.o) $(CCSRCS:.cc=.o) $(ASRCS:.s=.o) $(DVPSRCS:.dsm=.o)

.SUFFIXES: .c .s .cc .dsm .a .elf

all : $(TARGET).$(TAGSUFFIX)

run : $(TARGET).elf
	$(RUN) $(TARGET).elf

clean :
#	cd $(VSMDIR); make clean
	$(RM) core *.o *.map *.lst *.dis *.d $(TARGET).elf $(TARGET).a *~

link :
	$(RM) $(TARGET).elf
	gmake

$(TARGET).a : $(OBJS)
	$(AR) $(ARFLAGS) $(TARGET).a $(OBJS)

$(TARGET).elf : $(OBJS) $(LIBS)
	$(LD) -o $@ -T $(LCFILE) $(OBJS) $(LIBS) $(LDFLAGS)

$(RUNTIME) : $(LIBDIR)/crt0.s
	@$(AS) $(ASFLAGS) -o $@ $< > $*.lst

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

#micro.dsm : submake

submake :
	cd $(VSMDIR); make

include $(CSRCS:.c=.d) $(CCSRCS:.cc=.d) $(ASRCS:.s=.d)

