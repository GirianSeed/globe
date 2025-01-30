#!gmake
#
# Makefile for User Application Program using HiG Library
#
#		start 2000.09.04
#

# directory
TOP		:= /usr/local/sce/ee
BINDIR		:= $(TOP)/bin
LIBDIR		:= $(TOP)/lib
INCDIR		:= $(TOP)/include

# HIGDIR is directory-path where did install 'HiG'
HIGDIR		:= $(TOP)

HIGLIBDIR	:= $(LIBDIR)
HIGINCDIR	:= $(INCDIR)
HIPLIBDIR	:= $(LIBDIR)

#*******************************#
# CHANGE BELLOW FOR Ur PROJECT	#
#*******************************#
TARGET		:= main
TAGSUFFIX	:= elf
CSRCS		:= main.c camera.c light.c util.c mtxdisp.c\
		   sample0.c sample1.c sample2.c sample3.c sample4.c\
		   sample5.c sample6.c sample7.c sample8.c sample9.c\
		   sample10.c sample11.c sample12.c sample13.c sample14.c\
		   sample15.c sample16.c sample17.c sample18.c sample19.c\
		   sample20.c sample21.c sample22.c sample23.c sample24.c\
		   sample25.c sample26.c sample27.c sample28.c sample29.c\
		   sample30.c sample31.c sample32.c
CCSRCS		:=				# suffix .cpp
ASRCS		:= 				# suffix .s
DVPSRCS		:= micro.dsm			# suffix .dsm
RUNTIME		:= crt0.o
LIBS		:= $(HIPLIBDIR)/libhip.a	\
		   $(HIGLIBDIR)/libhig.a	\
		   $(LIBDIR)/libgp.a	\
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
CFLAGS		:= $(INCLUDES) -G0 -O2 -g -Wall -Wa,-al -fno-common -fno-strict-aliasing
C++FLAGS	:= $(INCLUDES) -G0 -O2 -g -Wall -Wa,-al -fno-common -fno-strict-aliasing -fexceptions -x c++ -frepo
ASFLAGS		:= $(INCLUDES) -G0 -c -xassembler-with-cpp -Wa,-al
DVPASMFLAGS	:= $(INCLUDES) -I$(VSMDIR)
ARFLAGS		:= rc
LDFLAGS		:= -Wl,-Map,$(TARGET).map -mno-crt0 -L$(LIBDIR) -lm

OBJS		:= $(RUNTIME) $(CSRCS:.c=.o) $(CCSRCS:.cpp=.o) $(ASRCS:.s=.o) $(DVPSRCS:.dsm=.o)

.SUFFIXES: .c .s .cpp .dsm .a .elf
vpath %.h	.:$(HIGINCDIR):$(INCDIR)
vpath %.vsm	$(VSMDIR)


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

.c.o:
	$(CC) -c $(CFLAGS) $< -o $@ > $*.lst

.cpp.o:
	$(CC) -c $(C++FLAGS) $< -o $@ > $*.lst

.s.o:
	$(AS) $(ASFLAGS) -o $@ $< > $*.lst

.dsm.o:
	$(DVPASM) $(DVPASMFLAGS) -o $@ $< --MD $*.d

$(CSRCS:.c=.d) :%.d : %.c
	set -e
	$(CPP) -M $(CPPFLAGS) $< | \
	sed 's/\($*\)\.o[ :]*/\1.o $@ :/g' > $@; \
	[ -s $@ ] || rm -f $@

$(CCSRCS:.cpp=.d) : %.d : %.cpp
	set -e
	$(CPP) -M $(CPPFLAGS) $< | \
	sed 's/\($*\)\.o[ :]*/\1.o $@ :/g' > $@; \
	[ -s $@ ] || rm -f $@

$(ASRCS:.s=.d) : %.d : %.s
	set -e
	$(CPP) -M $(CPPFLAGS) $< | \
	sed 's/\($*\)\.o[ :]*/\1.o $@ :/g' > $@; \
	[ -s $@ ] || rm -f $@

-include $(CSRCS:.c=.d) $(CCSRCS:.cpp=.d) $(ASRCS:.s=.d) $(DVPSRCS:.dsm=.d)


