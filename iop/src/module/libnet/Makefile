#
#  IOP Libnet Makefile
#

ifeq ($(wildcard PathDefs),)
PathDefs:
	iop-path-setup > PathDefs || (rm -f PathDefs ; exit 1)
	$(MAKE) all
endif
include PathDefs

TOP       = ../../../..
SCETOP    = /usr/local/sce
#TOP      = $(SCETOP)
IOPTOP    = $(TOP)/iop
EETOP     = $(TOP)/ee
SCEIOPTOP = $(SCETOP)/iop
SCEEETOP  = $(SCETOP)/ee
RM        = /bin/rm -f

#----------- customize section --------------
XCFLAGS   = -I$(IOPTOP)/include -I$(TOP)/common/include -I$(EETOP)/include
ASFLAGS   =
LDFLAGS   =
CFLAGS    = -Wall -g -O2 $(XCFLAGS)

MODNAME   = libnet
ENTRY     = start
IRX       = $(MODNAME).irx
TBL       = $(MODNAME).tbl
ILB       = $(MODNAME).ilb
ENT       = $(MODNAME)ent
ENTS      = $(ENT).s
ENTO      = $(ENT).o

OBJS      = $(MODNAME).o

LIBS      = -ilb=msifrpc.ilb \
            -ilb=inet.ilb \
            -ilb=inetctl.ilb \
            -ilb=netcnf.ilb


all: $(IRX)

$(IRX): $(OBJS) $(TBL)
	$(ILBGEN) $(TBL) -e $(ENTS)
	$(AS) -o $(ENTO) $(ENTS)
	$(RM) $(ENTS)
	$(LINK.o) -e $(ENTRY) $(OBJS) $(ENTO) -s -o $@ $(LIBS)

$(ILB): $(TBL)
	$(ILBGEN) $(TBL) -d $(ILB)

clean:
	$(RM) *.o *.irx *.map *.ilb PathDefs

clear:
	$(RM) *~

install: all
	cp $(IRX) $(IOPTOP)/modules
	cp $(IRX) $(SCEIOPTOP)/modules
#	cp $(ILB) $(IOPTOP)/lib

