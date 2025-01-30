#!/usr/bin/make -f
.PHONY: all clean

TARGET      = prver
SRCS        = main.c
OBJS        = $(SRCS:.c=.o)

CPPFLAGS    =
CFLAGS      = --std=gnu99 -O2 -Wall -Wa,-al
LDFLAGS     = -Wl,-Map,$@.map

all: $(TARGET)

$(TARGET): $(OBJS)
	$(LINK.c) -o $@ $^ $(LDLIBS)

.SUFFIXES: .c

.c.o:
	@echo compile $<
	@$(COMPILE.c) $< -o $*.o > $*.lst

clean:
	-$(RM) $(OBJS) $(OBJS:.o=.lst)
	-$(RM) $(TARGET)
	-$(RM) $(TARGET).map
