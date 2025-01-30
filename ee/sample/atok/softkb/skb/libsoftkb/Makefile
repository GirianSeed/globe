#------------------------------------------------------------------------
SKBDBG_MODE	= 1			# ƒfƒoƒbƒO•¶Žš	0:o—Í‚µ‚È‚¢		/1:o—Í‚·‚é

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
LIBDIR      = $(TOP)/ee/lib

CCC_INC_DIR   = ../../utf8/libccc/include
CCC_LIB_DIR   = ../../utf8/libccc/lib

INCDIR      = ../include \
              -I$(TOP)/ee/include \
              -I$(TOP)/common/include \
              -I$(CCC_INC_DIR)   \


TARGET      = libsoftkb
INST_DIR = ..

OBJS        = \
              skb_core.o \
              skb_list.o \
              $(TARGET).o

LCFILE = $(LIBDIR)/app.cmd
LIBS   = \
         $(LIBDIR)/libgraph.a \
         $(LIBDIR)/libdev.a \
         $(LIBDIR)/libdma.a \
         $(LIBDIR)/libpad.a \
         $(LIBDIR)/libpkt.a \
         $(LIBDIR)/libvu0.a \
         $(LIBDIR)/libusbkb.a

PREFIX      = ee
AS          = $(PREFIX)-gcc
AR          = $(PREFIX)-ar
CC          = $(PREFIX)-gcc
DVPASM      = $(PREFIX)-dvpasm
OBJDUMP     = $(PREFIX)-objdump
CP          = /bin/cp -p -u
RM          = /bin/rm -f

# Ý’èFSKBDBG_MODE[ƒfƒoƒbƒOƒ‚[ƒh]
CFLAGS      = -Os -Werror -Wall -Wa,-al -fno-common -fno-strict-aliasing $(COMMON_DEF) $(GCC_DEF) -DSKBDBG_MODE=$(SKBDBG_MODE)
CXXFLAGS    = -O2 -Wall -Wa,-al -fno-exceptions -fno-common -fno-strict-aliasing $(GCC_DEF)
ASFLAGS     = -c -xassembler-with-cpp -Wa,-al
DVPASMFLAGS = 
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
	@$(CP) sharedskb.h $(INST_DIR)/include
	@$(CP) $(TARGET).h $(INST_DIR)/include
	@$(CP) $(TARGET).a $(INST_DIR)/lib

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
	@$(CC) $(CXXFLAGS) $(TMPFLAGS) -I$(INCDIR) -c $< -o $*.o > $*.lst

.cpp.o:
	@echo C++ Compile [[36m $< [m]
	@$(CC) $(CXXFLAGS) $(TMPFLAGS) -I$(INCDIR) -c $< -o $*.o > $*.lst

clean:
	$(RM) *.o *.map *.lst core *.dis *.bin *.elf *.txt *.a
	$(RM) $(INST_DIR)/include/sharedskb.h 
	$(RM) $(INST_DIR)/include/$(TARGET).h 
	$(RM) $(INST_DIR)/lib/$(TARGET).a 

