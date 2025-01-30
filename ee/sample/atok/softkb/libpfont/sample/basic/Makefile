
SHELL	= /bin/sh

TOP		= /usr/local/sce/ee
LIBDIR	= -L$(TOP)/lib -L../../lib -L.
INCDIR	= -I$(TOP)/include -I../../include

TARGET	= main
OPTION	=

ifdef _DEBUG
OPTION	+= -g -D _DEBUG=1
endif

OBJS	= crt0.o	\
		sample.o	\
		$(TARGET).o

LCFILE	= $(TOP)/lib/app.cmd

LIBS	= -lm -ldma -lgraph -lvu0 -lpkt -lpfont -ldev

PREFIX	= ee-
SUFIX	= gcc
#SUFIX	= g++
AS		= $(PREFIX)$(SUFIX)
CC		= $(PREFIX)$(SUFIX)
LD		= $(PREFIX)$(SUFIX)
STRIP	= $(PREFIX)strip
DVPASM	= $(PREFIX)dvp-as
OBJDUMP	= $(PREFIX)objdump -mmips:5900
RUN		= dsedb -noexit -r run
RESET	= dsedb reset
RM		= /bin/rm -f


CFLAGS	= -O3 -G0 -g -fno-common -DR5900 -Wall -W -Wa,-al \
		-ansi\
		-Wshadow\
		-Wpointer-arith\
		-Wbad-function-cast\
		-Wcast-qual\
		-Wcast-align\
		-Wwrite-strings\
		-Wsign-compare\
		-Waggregate-return\
		-Wstrict-prototypes\
		-Wmissing-prototypes\
		-Wmissing-declarations\
		-Wmissing-noreturn\
		-Wredundant-decls\
		-Wnested-externs\
		-Winline\
		-std=c9x

#		-Wconversion
#		-pedantic
#		-Wtraditional

ASFLAGS		= -c -xassembler-with-cpp -Wa,-al
DVPASMFLAGS	=
LDFLAGS		= -Wl,-Map,$(TARGET).map -mno-crt0
DEPEND		= depend.txt

.SUFFIXES: .c .s .dsm

all: depend $(TARGET).elf

depend: $(DEPEND)

$(DEPEND): *.c
	@echo "make depend [ $@ ]"
	@makedepend -f- $(INCDIR) *.c |\
	sed '/ \//s/ \/[^ ]*//g' |\
	sed '/\.o:$$/d' > $@

include $(DEPEND)

$(TARGET).elf: $(OBJS)
	@echo "Linking     [ $@ ]"
	@$(LD) -o $@ -T $(LCFILE) $(OBJS) $(LIBS) $(LDFLAGS) $(LIBDIR)
	@$(STRIP) $@

crt0.o: $(TOP)/lib/crt0.s
	@echo "Assemble    [ $< ]"
	@$(AS) $(ASFLAGS) -o $@ $< > $*.lst

.s.o:
	@echo "Assemble    [ $< ]"
	@$(AS) $(ASFLAGS) $(INCDIR) -o $@ $< > $*.lst

.dsm.o:
	@echo "DSM Compile [ $< ]"
	@$(DVPASM) $(DVPASMFLAGS) $(INCDIR) -o $@ $< > $*.lst

.c.o:
	@echo "C Compile   [ $< ]"
	@$(CC) $(OPTION) $(CFLAGS) $(INCDIR) -c $< -o $*.o > $*.lst

_run:
	$(RUN) $(TARGET).elf

run:	$(TARGET).elf
	$(RUN) $(TARGET).elf

clean:
	$(RM) *.o *.map *.lst core *~ depend.txt *.elf
