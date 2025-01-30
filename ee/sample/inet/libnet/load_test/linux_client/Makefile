#			Copyright (C) 2000 Sony Computer Entertainment
#			All Right Reserved
#
# $Id: Makefile,v 1.1 2002/02/01 11:05:56 ksh Exp $
#

VERSION = `cat ../VERSION`

DEFS	= # -DDEBUG
CFLAGS	= -Wall -g $(DEFS) -I../common -I.

PROGS	= load_test

INSDIR	= /usr/local/bin

.PHONY:	all install taz clean

all:		$(PROGS)

load_test:	load_test.o

load_test.o:		load_test.c

#stamp.o:        ../VERSION
#	$(RM) stamp.c
#	echo "char *stamp_str = \"Version $(VERSION)" \
#	"`(LANG=C; export LANG; date)`\";"      > stamp.c
#	$(CC) -c stamp.c
#	$(RM) stamp.c  

clean:
	$(RM) $(PROGS) *.o stamp.c
