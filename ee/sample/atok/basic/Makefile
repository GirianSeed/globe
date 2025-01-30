#!gmake
#
# Makefile for User Application Program using HiG Library
#
#		start 2000.09.04
#

# libpfont directory
PFONT_PATH	:= libpfont
CCC_PATH	:= utf8

# directory
TOP		:= /usr/local/sce/ee
BINDIR		:= $(TOP)/bin
LIBDIR		:= $(TOP)/lib
INCDIR		:= $(TOP)/include

#*******************************#
# CHANGE BELLOW FOR Ur PROJECT	#
#*******************************#
TARGET		:= main
TAGSUFFIX	:= elf
CSRCS		:= main.c pad.c font.c io.c ccc.c info.c
CCSRCS		:=				# suffix .cpp
ASRCS		:= 				# suffix .s
DVPSRCS		:=
RUNTIME		:= crt0.o
LIBS		:= $(PFONT_PATH)/lib/libpfont.a \
		   $(CCC_PATH)/libccc/lib/libccc.a \
		   $(LIBDIR)/libatok.a		\
		   $(LIBDIR)/libgraph.a		\
		   $(LIBDIR)/libdev.a		\
		   $(LIBDIR)/libdma.a		\
		   $(LIBDIR)/libpad.a		\
		   $(LIBDIR)/libpkt.a		\
		   $(LIBDIR)/libvu0.a
VSMDIR		:=
LCFILE		:= $(LIBDIR)/app.cmd

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
INCLUDES	:= -I. -I../../../include -I$(INCDIR) -I$(PFONT_PATH)/include -I$(CCC_PATH)/libccc/include
CPPFLAGS	:= $(INCLUDES)
CFLAGS		:= $(INCLUDES) -G0 -g -Wall -Wa,-al -fno-common -fno-strict-aliasing -DPFONT_PATH=\"$(PFONT_PATH)\" -DCCC_PATH=\"$(CCC_PATH)\"
C++FLAGS	:= $(INCLUDES) -G0 -g -Wall -Wa,-al -fno-common -fno-strict-aliasing -fexceptions -x c++ -frepo
ASFLAGS		:= $(INCLUDES) -G0 -c -xassembler-with-cpp -Wa,-al
DVPASMFLAGS	:= $(INCLUDES) -I$(VSMDIR)
ARFLAGS		:= rc
LDFLAGS		:= -Wl,-Map,$(TARGET).map -mno-crt0 -L$(LIBDIR) -lm

OBJS		:= $(RUNTIME) $(CSRCS:.c=.o) $(CCSRCS:.cpp=.o) $(ASRCS:.s=.o) $(DVPSRCS:.dsm=.o)

.SUFFIXES: .c .s .cpp .dsm .a .elf

all : $(TARGET).$(TAGSUFFIX)

run : $(TARGET).elf
	$(RUN) $(TARGET).elf

clean :
	$(RM) core *.o *.map *.lst *.dis *.d *.rpo $(TARGET).elf $(TARGET).a *~

link :
	$(RM) $(TARGET).elf
	gmake

$(TARGET).a : $(OBJS)
	$(AR) $(ARFLAGS) $(TARGET).a $(OBJS)

$(TARGET).elf : $(OBJS) $(LIBS)
	$(LD) -o $@ -T $(LCFILE) $(OBJS) $(LIBS) $(LDFLAGS)

$(RUNTIME) : $(LIBDIR)/crt0.s
	@$(AS) $(ASFLAGS) -o $@ $< > $*.lst

.c.o: app.h
	$(CC) -c $(CFLAGS) $< -o $@ > $*.lst

.cpp.o:
	$(CC) -c $(C++FLAGS) $< -o $@ > $*.lst

.s.o:
	$(AS) $(ASFLAGS) -o $@ $< > $*.lst

.dsm.o:
	$(DVPASM) $(DVPASMFLAGS) -o $@ $< --MD $*.d
