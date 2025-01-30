
SHELL		= /bin/sh

TOP		= /usr/local/sce/ee
INCDIR		= $(TOP)/include -I../include

LIB_INST_DIR	= ../lib
HDR_INST_DIR	= ../include
ILB_INST_DIR	= ../ilb
ERX_INST_DIR	= ../modules

TARGET		= libccc

OBJS		= libccc.o

LCFILE		= $(TOP)/lib/app.cmd
LIBS		=

LIB_OBJS	= $(OBJS) $(TARGET)_entry.o erxexport.o
ERX_OBJS	= $(OBJS) $(TARGET)_entry.o modname.o

PREFIX		= ee
AS		= $(PREFIX)-gcc
CC		= $(PREFIX)-gcc
LD		= $(PREFIX)-gcc
RANLIB		= $(PREFIX)-ranlib
DVPASM		= $(PREFIX)-dvp-as
OBJDUMP		= $(PREFIX)-objdump -mmips:5900
LD		= erx-gcc
LIBGEN		= $(PREFIX)libgen
LIBLD		= $(PREFIX)libld
FIXUP		= $(PREFIX)fixup
RM		= /bin/rm -f
TOUCH		= /bin/touch
INSTALL		= install


CFLAGS		= -G0 -O4 -Wa,-al -fno-common -fno-edge-lcm -ansi \
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
		-DR5900
#		-pedantic
#		-Wconversion
ASFLAGS		= -c -xassembler-with-cpp -Wa,-al
DVPASMFLAGS	=
DEPEND		= depend.txt
LDFLAGS     = -Wl,-Map,$(TARGET).map,--check-sections,--stats,--warn-common,--warn-constructors,--warn-multiple-gp,--warn-section-align,--cref -mno-crt0

.SUFFIXES: .c .s .cc .dsm

all: depend $(TARGET).a $(TARGET).erx $(TARGET).ilb

depend: $(DEPEND)

$(DEPEND): *.c
	@echo "make depend [ $@ ]"
	@makedepend -f- -I$(INCDIR) *.dsm *.c |\
	sed '/ \//s/ \/[^ ]*//g' |\
	sed '/\.o:$$/d' > $@

include $(DEPEND)

$(TARGET).ilb: $(TARGET).tbl
	$(LIBGEN) -d $@ $<

$(TARGET).a: $(LIB_OBJS)
	@echo "Archive     [ $@ ]"
	@$(RM) $@
	@$(AR) r $@ $(LIB_OBJS)
	@$(RANLIB) $@

$(TARGET)_entry.s: $(TARGET).tbl
	$(LIBGEN) -e $@ $<

stub.s: $(ERX_OBJS) $(ILBS)
	$(LD) $(LDFLAGS) -makestub -o $@ $(ERX_OBJS) $(ILIBS:%=-ilb=%)

$(TARGET).erx: stub.s $(ERX_OBJS)
	$(LD) $(LDFLAGS) -o $(@:.erx=.sym) stub.s $(ERX_OBJS) $(ILIBS:%=-ilb=%)
	$(FIXUP) -o $(TARGET).erx $(@:.erx=.sym)

.s.o:
	@echo "Assemble    [ $< ]"
	@$(AS) $(ASFLAGS) -I$(INCDIR) -o $@ $< > $*.lst

.dsm.o:
	@echo "DSM Compile [ $< ]"
	@$(DVPASM) $(DVPASMFLAGS) -I$(INCDIR) -o $@ $< > $*.lst

.c.o:
	@echo "C Compile   [ $< ]"
	@$(CC) $(CFLAGS) -I$(INCDIR) -c $< -o $*.o > $*.lst

.cc.o:
	@echo "C++ Compile [ $< ]"
	@$(CC) $(CXXFLAGS) -I$(INCDIR) -c $< -o $*.o > $*.lst

clean:
	$(RM) *.o *.map *.lst core *.dis *~ *.a
	$(RM) $(TARGET)_entry.s stub.s $(TARGET).ilb *.erx *.sym

install: all
	$(INSTALL) -d $(LIB_INST_DIR)
	$(INSTALL) -d $(ILB_INST_DIR)
	$(INSTALL) -d $(ERX_INST_DIR)
	cp $(TARGET).a $(LIB_INST_DIR)
	cp $(TARGET).ilb $(ILB_INST_DIR)
	cp $(TARGET).erx $(ERX_INST_DIR)
	cp $(TARGET).sym $(ERX_INST_DIR)

# DO NOT DELETE
