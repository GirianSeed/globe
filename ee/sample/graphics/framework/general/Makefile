#! gmake

# clear include path
C_INCLUDE_PATH	:=

# directory
TOP		:= /usr/local/sce/ee
LIBDIR		:= $(TOP)/lib
INCDIR		:= $(TOP)/include
CURDIR		:= $(shell pwd)

# change bellow for Ur project
TARGET		:= main
TAGSUFFIX	:= elf
CSRCS		:= main.c gmain.c camera.c packbuf.c \
		   vumem1.c pad.c mode.c \
		   info.c micro.c shape.c tex2d.c hrchy.c object.c anime.c \
		   share.c light.c model.c user.c
CCSRCS		:=
ASRCS		:=
DVPSRCS		:= vu1micro.dsm
RUNTIME		:= crt0.o
LCFILE		:= $(LIBDIR)/app.cmd
LIBS		:= $(LIBDIR)/libgraph.a		\
		   $(LIBDIR)/libdev.a		\
		   $(LIBDIR)/libdma.a		\
		   $(LIBDIR)/libpad.a		\
		   $(LIBDIR)/libpkt.a		\
		   $(LIBDIR)/libvu0.a		\

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
CPP		:= $(shell $(CC) -print-prog-name=cpp)
LD		:= $(GCCBIN)/ee-gcc
AS		:= $(GCCBIN)/ee-gcc
DVPASM		:= $(GCCBIN)/ee-dvp-as
OBJDUMP		:= $(GCCBIN)/ee-objdump
RUN		:= dsedb -r run

# FLAGS
INCLUDES	:= -I. -I$(INCDIR)
CPPFLAGS	:= $(INCLUDES) -D__IEEE_LITTLE_ENDIAN
CFLAGS		:= $(INCLUDES) -O2 -Wall -Wa,-al -fno-common -fno-strict-aliasing -D__IEEE_LITTLE_ENDIAN
CXXFLAGS	:= $(INCLUDES) -O2 -Wall -Wa,-al -fno-exceptions -fno-common -fno-strict-aliasing -D__IEEE_LITTLE_ENDIAN
ASFLAGS		:= $(INCLUDES) -c -xassembler-with-cpp -Wa,-al
DVPASMFLAGS	:= $(INCLUDES)
LDFLAGS		:= -Wl,-Map,$(TARGET).map -mno-crt0 -L$(LIBDIR) -lm
TMPFLAGS	:=

# OBJECT
OBJS		:= $(RUNTIME)		\
		   $(CSRCS:.c=.o)	\
		   $(CCSRCS:.cc=.o)	\
		   $(ASRCS:.s=.o)	\
		   $(DVPSRCS:.dsm=.o)

# PATH and Suffix

.SUFFIXES: .c .s .cc .dsm

vpath	%.c	$(CURDIR)
vpath	%.cc	$(CURDIR)
vpath	%.h	$(CURDIR) $(INCDIR)
vpath	%.dsm	$(CURDIR)
vpath	%.s	$(CURDIR)

#
# RULE
#

default all: $(TARGET).$(TAGSUFFIX)

run: $(TARGET).elf
	$(RUN) $(TARGET).elf

clean:
	$(RM) core *.o *.map *.lst *.dis $(TARGET).$(TAGSUFFIX) *.d *~

$(TARGET).elf: $(OBJS) $(LIBS)
	$(LD) -o $@ -T $(LCFILE) $(OBJS) $(LIBS) $(LDFLAGS)

$(RUNTIME) : $(LIBDIR)/crt0.s
	$(AS) $(ASFLAGS) -o $@ $< > $*.lst

.c.o:
	$(CC) -c $(CFLAGS) $< -o $@ > $*.lst

.cc.o:
	$(CC) -c $(CXXFLAGS) $< -o $@ > $*.lst

.s.o:
	$(AS) $(ASFLAGS) -o $@ $< > $*.lst

.dsm.o:
	$(DVPASM) $(DVPASMFLAGS) -o $@ $<

$(CSRCS:.c=.d): %.d: %.c
	set -e
	$(CPP) -MM -MG $(CPPFLAGS) $< |			\
	sed 's/\($*\)\.o[ :]*/\1.o $@ :/g' > $@;	\
	[ -s $@ ] || rm -f $@

$(CCSRCS:.cc=.d): %d: %.cc
	set -e
	$(CPP) -MM -MG $(CPPFLAGS) $< |			\
	sed 's/\($*\)\.o[ :]*/\1.o $@ :/g' > $@;	\
	[ -s $@ ] || rm -f $@

$(DVPSRCS:.dsm=.d): %.d: %.dsm
	set -e
	$(CPP) -MM -MG $(CPPFLAGS) $< |			\
	sed 's/\($*\)\.o[ :]*/\1.o $@ :/g' > $@;	\
	[ -s $@ ] || rm -f $@

$(ASRCS:.s=.d): %.d :%.s
	set -e
	$(CPP) -MM -MG $(CPPFLAGS) $< |			\
	sed 's/\($*\)\.o[ :]*/\1.o $@ :/g' > $@		\
	[ -s $@ ] || rm -f $@

include $(CSRCS:.c=.d) $(CCSRCS:.cc=.d) $(DVPSRCS:.dsm=.d) $(ASRCS.s=.d)
