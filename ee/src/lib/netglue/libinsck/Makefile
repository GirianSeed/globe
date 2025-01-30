#			Copyright (C) 2001 Sony Computer Entertainment
#			All Right Reserved
#
#

TOPDIR	= ./
include $(TOPDIR)Makedefs

CFLAGS	+= -I. -I$(TOP)/../common/include -D__ELF__

TARGET	= netglue_insck.a

HDRS	=
SRCS	= netglue_insck.c
OBJS	= $(SRCS:.c=.o)

.SUFFIXES: .c .s

all:	$(TARGET)

$(TARGET): $(OBJS)
	$(AR) -cru $@ $(OBJS)
	$(RANLIB) $@

clean:
	$(RM) *.o core $(TARGET) test *~
