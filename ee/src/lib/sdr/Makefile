# SCE CONFIDENTIAL
# $PSLibId$
# $Id: Makefile,v 1.7.6.1 2003/10/09 05:26:09 xokano Exp $
#
# Emotion Engine Library
#
# Copyright (C) 2003 Sony Computer Entertainment Inc.
# All Rights Reserved.
#
# --- Makefile

SHELL=		/bin/sh

TOP=		../../..
INCDIR=		$(TOP)/include
INCDIR_COMMON=	$(TOP)/../common/include
ILBDIR=		$(TOP)/ilb
INCOPT=		-I$(INCDIR_COMMON) -I$(INCDIR)

LIB_INST_DIR=	$(TOP)/lib
HDR_INST_DIR=	$(TOP)/include
ILB_INST_DIR=	$(TOP)/ilb
ERX_INST_DIR=	$(TOP)/modules

TARGET=		libsdr
OBJS=		sdr_main.o sdr_cb.o
LIB_OBJS=	$(OBJS) $(TARGET)_entry.o erxexport.o
ERX_OBJS=	$(OBJS) $(TARGET)_entry.o modname.o
ILBS=		libkernl.ilb libc_stdio.ilb
ILBS_DEPEND=	$(ILBS:%=$(ILBDIR)/%)

INIT_OBJ=	sdr_main.o

PREFIX=ee
AS=		$(PREFIX)-gcc
CC=		$(PREFIX)-gcc
AR=		$(PREFIX)-ar
RANLIB=		$(PREFIX)-ranlib
DVPASM=		$(PREFIX)-dvp-as
LD=		erx-gcc
LIBGEN=		$(PREFIX)libgen
LIBLD=		$(PREFIX)libld
FIXUP=		$(PREFIX)fixup
RM=		/bin/rm -f
INSTALL=	install

CFLAGS=		-c -Wa,-al -fno-common -O2 -G0 $(INCOPT)
ASFLAGS=	-c -Wa,-al -xassembler-with-cpp $(INCOPT)
DVPASMFLAGS=	-g 
LDFLAGS=	-mno-crt0 -Wl,-Map,$(TARGET).map,--check-sections,--stats,--warn-common,--warn-constructors,--warn-multiple-gp,--warn-section-align,--cref
LDLIBS=		-L$(ILBDIR) $(ILBS:%=-ilb=%)

# -ansi -std=iso9899:1999 
CPPFLAGS=	-Wall -W -pedantic \
		-Wshadow -Wpointer-arith -Wbad-function-cast -Wcast-qual \
		-Wcast-align -Wwrite-strings -Wconversion -Wsign-compare \
		-Waggregate-return -Wstrict-prototypes -Wmissing-prototypes \
		-Wmissing-declarations -Wmissing-noreturn -Wredundant-decls \
		-Wnested-externs -Winline

FILES_TO_CLEAN=	*.o *.lst *.map *.sym core

.SUFFIXES: .c .s .cc .dsm

all: $(TARGET).a $(TARGET).erx $(TARGET).ilb

$(TARGET).a: $(LIB_OBJS)
	$(RM) $@
	$(AR) r $@ $(LIB_OBJS)
	$(RANLIB) $@

$(TARGET)_entry.s: $(TARGET).tbl
	$(LIBGEN) -e $@ $<

$(TARGET).ilb: $(TARGET).tbl
	$(LIBGEN) -d $@ $<

stub.s: $(ERX_OBJS) $(ILBS_DEPEND)
	$(LD) $(LDFLAGS) -makestub -o $@ $(ERX_OBJS) $(LDLIBS)

$(TARGET).erx: stub.s $(ERX_OBJS)
	$(LD) $(LDFLAGS) -o $(@:.erx=.sym) stub.s $(ERX_OBJS) $(LDLIBS)
	$(FIXUP) -o $(TARGET).erx $(@:.erx=.sym)

.s.o:
	$(AS) $(ASFLAGS) -o $@ $< > $*.lst

.c.o:
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $*.o > $*.lst

.dsm.o:
	$(DVPASM) $(DVPASMFLAGS) -I$(INCDIR) -o $@ $< > $*.lst

clean:
	$(RM) $(TARGET).a $(TARGET).erx $(TARGET).ilb
	$(RM) $(TARGET)_entry.s stub.s
	$(RM) $(FILES_TO_CLEAN)

clear:
	$(RM) *~

install: all
	$(INSTALL) -d $(LIB_INST_DIR)
	$(INSTALL) -d $(ILB_INST_DIR)
	$(INSTALL) -d $(ERX_INST_DIR)
	cp $(TARGET).a $(LIB_INST_DIR)
	cp $(TARGET).ilb $(ILB_INST_DIR)
	cp $(TARGET).erx $(ERX_INST_DIR)
	cp $(TARGET).sym $(ERX_INST_DIR)

