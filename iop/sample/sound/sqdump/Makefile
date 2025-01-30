#
#  IOP sdsq sample program Makefile
#
#  $Id: Makefile,v 1.4 2003/06/04 11:12:31 kaol Exp $
#
ifeq ($(wildcard PathDefs),)
PathDefs:
	iop-path-setup > PathDefs || (rm -f PathDefs ; exit 1)
	$(MAKE) all
else
include PathDefs
endif

TOPDIR = ../../../..
INCDIR = -I $(TOPDIR)/common/include
LIBDIR = $(TOPDIR)/iop/lib
MODDIR = $(TOPDIR)/iop/modules
 
#----------- customize section --------------
CFLAGS   = $(INCDIR) -Wall -Werror -G 0
ASFLAGS  = $(INCDIR)
LDLIBS   = -ilb=$(LIBDIR)/sdsq.ilb
PROGNAME = sqdump.irx
OBJS	 = main.o sqdump.o
SQDATA   = host1:$(TOPDIR)/data/sound/seq/overlo_s.sq
OUTFILE  = host1:sqdump_log.txt

ifeq ($(OPTION), DEBUG)
	CFLAGS	+= -D_DEBUG
endif

all: $(PROGNAME)

#----------- rules --------------
%.irx: $(OBJS)
	$(LINK.o) $^ $(LDLIBS) -o $@

clean:
	rm -f *.o $(PROGNAME) *.map

run: all
	dsifilesv -c "\
	dsreset 0 2;\
	dsistart $(MODDIR)/sdsq.irx;\
	dsistart $(PROGNAME) $(SQDUMPOP) -o $(OUTFILE) $(SQDATA)"

%.sq: all
	dsifilesv -c "\
	dsreset 0 2;\
	dsistart $(MODDIR)/sdsq.irx;\
	dsistart $(PROGNAME) $(SQDUMPOP) -o $(OUTFILE) $@"

usage: all
	dsifilesv -c "\
	dsreset 0 2;\
	dsistart $(MODDIR)/sdsq.irx;\
	dsistart $(PROGNAME) -u"
