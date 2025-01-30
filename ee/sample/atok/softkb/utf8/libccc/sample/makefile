
SHELL		= /bin/sh

TOP		= /usr/local/sce/ee
LIBDIR		= -L$(TOP)/lib -L../lib
INCDIR		= -I$(TOP)/include -I../include

TARGET		= main

OBJS		= crt0.o\
		$(TARGET).o

LCFILE		= $(TOP)/lib/app.cmd
LIBS		= -lccc

PREFIX		= ee-
AS		= $(PREFIX)gcc
CC		= $(PREFIX)gcc
LD		= $(PREFIX)gcc
STRIP		= $(PREFIX)strip
DVPASM		= $(PREFIX)dvp-as
OBJDUMP		= $(PREFIX)objdump -mmips:5900
RUN		= dsedb -noexit -r run
RM		= /bin/rm -f
TOUCH		= /bin/touch

CFLAGS		= -G0 -O4 -fno-common -ansi -Wa,-al \
		-Wall \
		-W \
		-Wtraditional \
		-Wshadow \
		-Wpointer-arith \
		-Wbad-function-cast \
		-Wcast-qual \
		-Wcast-align \
		-Wwrite-strings \
		-Wsign-compare \
		-Waggregate-return \
		-Wstrict-prototypes \
		-Wmissing-prototypes \
		-Wmissing-declarations \
		-Wmissing-noreturn \
		-Wredundant-decls \
		-Wnested-externs \
		-Winline \
		-DR5900 \
		-std=c9x

#		-pedantic
#		-Wconversion

ASFLAGS		= -c -xassembler-with-cpp -Wa,-al
DVPASMFLAGS	=
LDFLAGS		= -Wl,-Map,$(TARGET).map -mno-crt0
TMPFLAGS	=
DEPEND		= depend.txt

.SUFFIXES: .c .s .cc .dsm

all: depend $(TARGET).elf

depend: $(DEPEND)

$(DEPEND): *.c
	@echo "make depend [ $@ ]"
	@makedepend -f- $(INCDIR) *.dsm *.c |\
	sed '/ \//s/ \/[^ ]*//g' |\
	sed '/\.o:$$/d' > $@

include $(DEPEND)

$(TARGET).elf: $(OBJS)
	@echo "Linking     [ $@ ]"
	@$(LD) -o $@ -T $(LCFILE) $(OBJS) $(LIBS) $(LDFLAGS) $(LIBDIR)
	@$(STRIP) $@

crt0.o: $(TOP)/lib/crt0.s
	@echo "Assemble    [ $< ]"
	@$(AS) $(ASFLAGS) $(TMPFLAGS) -o $@ $< > $*.lst

.s.o:
	@echo "Assemble    [ $< ]"
	@$(AS) $(ASFLAGS) $(TMPFLAGS) $(INCDIR) -o $@ $< > $*.lst

.dsm.o:
	@echo "DSM Compile [ $< ]"
	@$(DVPASM) $(DVPASMFLAGS) $(INCDIR) -o $@ $< > $*.lst

.c.o:
	@echo "C Compile   [ $< ]"
	@$(CC) $(CFLAGS) $(TMPFLAGS) $(INCDIR) -c $< -o $*.o > $*.lst

.cc.o:
	@echo "C++ Compile [ $< ]"
	@$(CC) $(CXXFLAGS) $(TMPFLAGS) $(INCDIR) -c $< -o $*.o > $*.lst

run: $(TARGET).elf
	$(RUN) $(TARGET).elf

clean:
	$(RM) *.o *.map *.lst core *.dis *~ *.elf test.utf8.txt test.sjis.txt
# DO NOT DELETE
