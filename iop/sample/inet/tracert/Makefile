# SCE CONFIDENTIAL
# 
# $Id: Makefile,v 1.1 2002/07/10 08:32:16 tetsu Exp $
# 
#      tracert
# 
#                          Version 1.1
# 
#      Copyright (C) 2002 Sony Computer Entertainment Inc.
# 	                 All Rights Reserved.
# 
#                          Makefile
# 
#        Version       Date            Design     Log
#  --------------------------------------------------------------------
#        1.1           2002.07.09      tetsu      first version

ifeq ($(wildcard PathDefs),)
PathDefs:
	iop-path-setup > PathDefs || (rm -f PathDefs ; exit 1)
	$(MAKE) all
else
include PathDefs
endif

#----------- rules --------------
%.irx: %.o
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@

#----------- customize section --------------
CFLAGS   =  -Wall -G0
ASFLAGS  =
LDLIBS   = -ilb=inet.ilb -ilb=cdvdman.ilb
PROGNAME = tracert.irx
OBJS	 = $(PROGNAME:%.irx=%.o) # add here other objects

all:	$(PROGNAME)

clean:
	rm -f *.o $(PROGNAME) *.map *~ *.log PathDefs

$(PROGNAME:%.irx=%.o) : $(PROGNAME:%.irx=%.c) # add your local include file

$(PROGNAME): $(OBJS)
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@
