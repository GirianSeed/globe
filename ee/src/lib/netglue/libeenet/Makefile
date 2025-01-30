#			Copyright (C) 2001 Sony Computer Entertainment
#			All Right Reserved
#
#

TOPDIR	= ./
include $(TOPDIR)Makedefs

CFLAGS	+= \
	-I. \
	-I$(TOP)/../common/include \
	-I$(TOP)/include \
	-I$(TOP)/include/libeenet \
	-D__ELF__ \

TARGET	= netglue_eenet.a

HDRS	=
SRCS	= netglue_eenet.c
OBJS	= $(SRCS:.c=.o)

.SUFFIXES: .c .s

.c.o:
	$(CC) $(CFLAGS) $(TMPFLAGS) -c $< -o $*.o > $*.lst

all:	$(TARGET)

$(TARGET): $(OBJS)
	$(AR) -cru $@ $(OBJS)
	$(RANLIB) $@

clean:
	$(RM) *.o *.lst core $(TARGET) test *~
