#			Copyright (C) 2000 Sony Computer Entertainment
#			All Right Reserved
#
# $Id: Makefile,v 1.1 2002/02/01 12:14:17 ksh Exp $
#

VERSION = `cat ../VERSION`

DEFS	= # -DDEBUG
CFLAGS	= -Wall -g $(DEFS) -I../common -I.

OBJS	= gamed.o physics.o ntohf.o
TARGET  = gamed

LD = gcc

INSDIR	= /usr/local/bin

.PHONY:	all install taz clean

all:		$(TARGET)

$(TARGET):		$(OBJS)
	$(LD) -o $(TARGET) $(OBJS) -lm -lpthread

#stamp.o:        ../VERSION
#	$(RM) stamp.c
#	echo "char *stamp_str = \"Version $(VERSION)" \
#	"`(LANG=C; export LANG; date)`\";"      > stamp.c
#	$(CC) -c stamp.c
#	$(RM) stamp.c  

clean:
	$(RM) $(TARGET) *.o *~
