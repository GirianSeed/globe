#
#  IOP crt0.o Makefile
#
#  $Id: Makefile,v 1.1 1999/08/15 12:11:13 tei Exp $
#

ASFLAGS = -I../include

ifeq ($(wildcard PathDefs),)
PathDefs:
	sh ./SetupPath.sh > PathDefs || (rm -f PathDefs ; exit 1)
	$(MAKE) all
endif

include PathDefs

all: crt0.o

crt0.o: crt0.s
	$(CC) -xassembler-with-cpp $(ASFLAGS) $(CPPFLAGS) $(TARGET_MACH) -c $^
