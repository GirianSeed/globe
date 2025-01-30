#!gmake
#
# Makefile for HiG Library Core Archive
#
#		start 2000.09.04
#
TARGETS		:= block ellipsoid cblock cellipsoid 

HIGDIR		:= ../../../..

#--------------------------------------------------

TOP		:= /usr/local/sce/ee
BINDIR		:= $(TOP)/bin
INCDIR		:= $(TOP)/include
HIGINCDIR	:= $(HIGDIR)/include
GCCDIR		:= $(TOP)/gcc
GCCBIN		:= $(GCCDIR)/bin

BFILES		:= $(addsuffix .bin,$(TARGETS))
AFILES		:= $(BFILES:.bin=.s)
OFILES		:= $(BFILES:.bin=.o)

MAKEFILE	:= Makefile
SHELL		:= /bin/sh
RM		:= /bin/rm -f
ECHO		:= /bin/echo
CP		:= /bin/cp
ES2HIG		:= $(BINDIR)/es2hig
AS		:= $(GCCBIN)/ee-gcc
OBJCOPY		:= $(GCCBIN)/ee-objcopy

INCDIRS		:= $(INCDIR) $(HIGINCDIR)
INCLUDES	:= $(addprefix -I,$(INCDIRS))
ASFLAGS		:= $(INCLUDES) -c -xassembler-with-cpp -Wa,-al
OBJCOPYFLAGS	:= -O binary -S -g --only-section=.data
ES2HIGFLAGS	:= 

default all: $(BFILES)

clean:
	$(RM) $(OFILES) $(BFILES) *.lst *~

%.bin : %.o
	$(OBJCOPY) $(OBJCOPYFLAGS) $< $@

%.o : %.s
	$(AS) $(ASFLAGS) -o $@ $< > $*.lst
