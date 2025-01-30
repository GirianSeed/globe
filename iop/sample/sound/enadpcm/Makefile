#
#  IOP test program Makefile
#
#  $Id: Makefile,v 1.3 2003/06/04 11:10:43 kaol Exp $
#

ifeq ($(wildcard PathDefs),)
PathDefs:
	iop-path-setup > PathDefs || (rm -f PathDefs ; exit 1)
endif
include PathDefs

RM = /bin/rm -f

TOP = ../../..

INCOPT  = -I$(TOP)/../common/include -I$(TOP)/include \
	-I/usr/local/sce/common/include
LDOPT =
LIBS = -ilb=spucodec.ilb -ilb=libsd.ilb 
#----------- rules --------------
%.irx: %.o
	$(LINK.o) $^ -o $@ $(LIBS)

#----------- customize section --------------
CFLAGS   = $(INCOPT) -Wall -G 0 -O2
LDFLAGS = -L$(TOP)/lib -G 0 -s $(LDOPT)

PROGNAME = main.irx

all: $(PROGNAME)

clean:
	rm -f *.o $(PROGNAME) *.map

run: all
	dsifilesv -c "\
		dsreset 0 2;\
		dsistart ../../../modules/libsd.irx;\
		dsistart ../../../modules/spucodec.irx;\
		dsistart $(PROGNAME);"
