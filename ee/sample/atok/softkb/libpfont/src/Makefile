
SHELL		= /bin/sh

TOP		= /usr/local/sce/ee
INCDIR		= -I$(TOP)/include -I../include
ILBDIR		= $(TOP)/ilb

INST_LIB_DIR	= ../lib
INST_ILB_DIR	= ../ilb
INST_ERX_DIR	= ../modules

TARGET		= libpfont

OBJS		= libpfont.o
LIB_OBJS	= $(OBJS) $(TARGET)_entry.o erxexport.o
ERX_OBJS	= $(OBJS) $(TARGET)_entry.o modname.o
ILIBS		= $(ILBDIR)/libvu0.ilb \
		  $(ILBDIR)/libkernl.ilb \
		  $(ILBDIR)/libc_string.ilb

PREFIX		= ee
AS		= $(PREFIX)-gcc
CC		= $(PREFIX)-gcc
LD		= $(PREFIX)-gcc
AR		= $(PREFIX)-ar
RANLIB		= $(PREFIX)-ranlib
STRIP		= $(PREFIX)-strip
DVPASM		= $(PREFIX)-dvp-as
LD		= erx-gcc
LIBGEN		= $(PREFIX)libgen
LIBLD		= $(PREFIX)libld
FIXUP		= $(PREFIX)fixup
OBJDUMP		= $(PREFIX)-objdump -mmips:5900
RUN		= dsedb -noexit -d localhost -r run
RM		= /bin/rm -f
CP		= /bin/cp
INSTALL		= /usr/bin/install

CFLAGS		= -O3 -G0 -g -fno-common -DR5900 -Wall -W -Wa,-al \
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
DEPEND		= depend.txt
LDFLAGS	= -Wl,-Map,$(TARGET).map,--check-sections,--stats,--warn-common,--warn-constructors,--warn-multiple-gp,--warn-section-align,--cref -mno-crt0

.SUFFIXES: .c .s .dsm

all: depend $(TARGET).a $(TARGET).erx $(TARGET).ilb

depend: $(DEPEND)

$(DEPEND): *.c
	@echo "make depend [ $@ ]"
	@makedepend -f- $(INCDIR) *.c |\
	sed '/ \//s/ \/[^ ]*//g' |\
	sed '/\.o:$$/d' > $@

include $(DEPEND)

$(TARGET).a: $(LIB_OBJS)
	@echo "Archive     [ $@ ]"
	@$(RM) $@
	@$(AR) r $@ $(LIB_OBJS)
	@$(RANLIB) $@

$(TARGET)_entry.s : $(TARGET).tbl
	@echo "Create Entry[ $@ ]"
	@$(LIBGEN) -e $@ $<

$(TARGET).ilb: $(TARGET).tbl
	@echo "Create ilb  [ $@ ]"
	@$(LIBGEN) -d $@ $<

stub.s: $(ERX_OBJS) $(ILBS)
	@echo "Create stub [ $@ ]"
	@$(LD) $(LDFLAGS) -makestub -o $@ $(ERX_OBJS) $(ILIBS:%=-ilb=%)

$(TARGET).erx: stub.s $(ERX_OBJS)
	@echo "Create sym  [ $(@:.erx=.sym) ]"
	@$(LD) $(LDFLAGS) -o $(@:.erx=.sym) stub.s $(ERX_OBJS) $(ILIBS:%=-ilb=%)
	@echo "Create erx  [ $@ ]"
	@$(FIXUP) -o $(TARGET).erx $(@:.erx=.sym)

.s.o:
	@echo "Assemble    [ $< ]"
	@$(AS) $(ASFLAGS) $(INCDIR) -o $@ $< > $*.lst

.dsm.o:
	@echo "DSM Compile [ $< ]"
	@$(DVPASM) $(DVPASMFLAGS) $(INCDIR) -o $@ $< > $*.lst

.c.o:
	@echo "C Compile   [ $< ]"
	@$(CC) $(CFLAGS) $(INCDIR) -c $< -o $*.o > $*.lst

clean:
	$(RM) *.o *.lst core *.elf *.a $(TARGET)_entry.s $(TARGET).ilb stub.s $(TARGET).erx $(TARGET).sym $(DEPEND)

install:
	$(INSTALL) -d $(INST_LIB_DIR)
	$(INSTALL) -d $(INST_ILB_DIR)
	$(INSTALL) -d $(INST_ERX_DIR)
	$(CP) $(TARGET).a $(INST_LIB_DIR)
	$(CP) $(TARGET).ilb $(INST_ILB_DIR)
	$(CP) $(TARGET).erx $(INST_ERX_DIR)
	$(CP) $(TARGET).sym $(INST_ERX_DIR)
