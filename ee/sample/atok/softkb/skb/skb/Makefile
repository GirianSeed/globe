#------------------------------------------------------------------------
FIELD_MODE	= 0			# ‰ð‘œ“xÝ’è	0:FRAME_MODE(H/2)	/1:FIELD_MODE(H)
PAL_MODE	= 0			# TVÝ’è        0:NTSC				/1:PAL

SKBDBG_MODE	= 0			# ƒfƒoƒbƒO•¶Žš	0:o—Í‚µ‚È‚¢		/1:o—Í‚·‚é
ATOK_USE	= 1			# ATOK			0:Žg‚í‚È‚¢			/1:Žg‚¤

COMMON_DEF_DIR  = ..
COMMON_DEF_FILE = $(COMMON_DEF_DIR)/common.def
include $(COMMON_DEF_FILE)                   # COMMON_DEF= -DƒIƒvƒVƒ‡ƒ“ƒY
#------------------------------------------------------------------------
# ƒRƒ“ƒpƒCƒ‰ LANG ŠÂ‹«•Ï”‘Î‰ž  ee-gcc ver 2.96-ee-001003-1
# 
#LANG=C-SJIS
#LANG=ja_JP.eucJP

ifeq ($(LANG),C-SJIS)
GCC_DEF = -DLANG_CSJIS
endif
#------------------------------------------------------------------------


SHELL       = /bin/sh

TOP         = /usr/local/sce

LIBDIR      = $(TOP)/lib
LIBLOCALDIR = ../lib
CCC_INC_DIR = ../../utf8/libccc/include
CCC_LIB_DIR = ../../utf8/libccc/lib

INCDIR      = ../include \
              -I$(TOP)/ee/include     \
              -I$(TOP)/common/include \
              -I$(CCC_INC_DIR)   \


TARGET   = skb
INST_DIR = ..

OBJS        = skb.o \
              sysenv.o \
              tim2.o \
              tim2wrap.o \
              draw.o \
              fepbar.o \
              textField.o \


LCFILE = $(LIBDIR)/app.cmd
LIBS   = \
         $(LIBDIR)/libcdvd.a \
         $(LIBDIR)/libscf.a \
         $(LIBDIR)/libgraph.a \
         $(LIBDIR)/libdev.a \
         $(LIBDIR)/libdma.a \
         $(LIBDIR)/libpad.a \
         $(LIBDIR)/libpkt.a \
         $(LIBDIR)/libvu0.a \
         $(LIBDIR)/libusbkb.a \
         $(LIBLOCALDIR)/libsoftkb.a \
         $(LIBLOCALDIR)/libfep.a \
         $(CCC_LIB_DIR)/libccc.a \

PREFIX      = ee
AS          = $(PREFIX)-gcc
AR          = $(PREFIX)-ar
CC          = $(PREFIX)-gcc
LD          = $(PREFIX)-gcc
DVPASM      = $(PREFIX)-dvpasm
OBJDUMP     = $(PREFIX)-objdump
RUN         = dsedb -r run
RM          = /bin/rm -f
CP          = /bin/cp -p -u

# Ý’èFCDBOOT[CDƒu[ƒg], FIELD_MODE[ƒtƒŒ[ƒ€ƒ‚[ƒh], SKBDBG_MODE[ƒfƒoƒbƒOƒ‚[ƒh], ATOK_USE[ATOKŽg—p]
APPFLAG     = -DFIELD_MODE=$(FIELD_MODE) -DSKBDBG_MODE=$(SKBDBG_MODE) \
              -DATOK_USE=$(ATOK_USE) -DPAL_MODE=$(PAL_MODE) -DUSBMOUSE_USE=$(USBMOUSE_USE)
#CFLAGS      = -O2 -G0 -Werror -Wall -fno-common -fno-strict-aliasing $(APPFLAG) $(COMMON_DEF) $(GCC_DEF)
CFLAGS      = -O2 -G0 -Wall -fno-common -fno-strict-aliasing $(APPFLAG) $(COMMON_DEF) $(GCC_DEF)


CXXFLAGS    = -O2 -Wall -Wa,-al -fno-exceptions -fno-common $(COMMON_DEF) $(GCC_DEF)
ASFLAGS     = -c -xassembler-with-cpp -Wa,-al
DVPASMFLAGS = 
#LDFLAGS     = -Wl,-Map,$(TARGET).map -mno-crt0 -L. -L$(LIBDIR) -lstdc++ -lgcc -lm
LDFLAGS     = -Wl,-Map,$(TARGET).map -nostartfiles -L. -L$(LIBDIR) -lstdc++ -lgcc -lm
TMPFLAGS    =
DEPEND      = depend.txt


.SUFFIXES: .c .s .cc .dsm

all: depend $(TARGET).a

depend: $(DEPEND)

$(DEPEND): *.c
	@echo "make depend [[35m $@ [m]"
	@makedepend -f- -I$(INCDIR) *.c |\
	sed '/ \//s/ \/[^ ]*//g' |\
	sed '/\.o:$$/d' > $@

include $(DEPEND)

$(TARGET).a: $(OBJS)
	@echo Linking and make [[35m $@ [m]
	@$(RM) $(TARGET).a
	@$(AR) qcs $(TARGET).a $(OBJS)
	@$(CP) $(TARGET).h $(INST_DIR)/include
	@$(CP) $(TARGET).a $(INST_DIR)/lib

crt0.o: $(LIBDIR)/crt0.s
	@echo Assemble    [[32m $< [m] 
	@$(AS) $(ASFLAGS) $(TMPFLAGS) -o $@ $< > $*.lst

.s.o:
	@echo Assemble    [[34m $< [m] 
	@$(AS) $(ASFLAGS) $(TMPFLAGS) -I$(INCDIR) -o $@ $< > $*.lst

.dsm.o:
	@echo DSM Compile [[33m $< [m]
	@$(DVPASM) $(DVPASMFLAGS) -I$(INCDIR) -o $@ $< > $*.lst

.c.o:
	@echo C Compile   [[36m $< [m]
	@$(CC) $(CFLAGS) $(TMPFLAGS) -I$(INCDIR) -c $< -o $*.o > $*.lst

.cc.o:
	@echo C++ Compile [[36m $< [m]
	$(CC) $(CXXFLAGS) $(TMPFLAGS) -I$(INCDIR) -c $< -o $*.o > $*.lst

.cpp.o:
	@echo C++ Compile [[36m $< [m]
	$(CC) $(CXXFLAGS) $(TMPFLAGS) -I$(INCDIR) -c $< -o $*.o > $*.lst

run: $(TARGET).elf
	$(RUN) $(TARGET).elf

clean:
	$(RM) *.o *.map *.lst core *.dis *.bin *.elf *.txt  *.a
	$(RM) ../include/$(TARGET).h ../lib/$(TARGET).a
