
MAKEFILE := Makefile

PROJECT := main
TARGET	= $(PROJECT)

REGION=JP
#REGION=US
#REGION=EU

#ATOK=TRUE

#------------------------------------------------------------------------
# ƒRƒ“ƒpƒCƒ‰ LANG ŠÂ‹«•Ï”‘Î‰ž  ee-gcc ver 2.96-ee-001003-1
# 
#LANG=C-SJIS
#LANG=ja_JP.eucJP

ifeq ($(LANG),C-SJIS)
GCC_DEF = -DLANG_CSJIS
endif
#--------------------------------------------------------------------

#--------------------------------------------------------------------
# makefile defines:
#
#*****************************************
# option : version   cdrom / host  
#

ifndef $(CDBOOT)
CDBOOT = FALSE
endif

ifndef $(ATOK)
ATOK=FALSE
endif
ATOK=TRUE

ifeq ($(CDBOOT),TRUE)

$(PROJECT)_DEFINES = -DBOOT_FROM_CD 

endif


ifeq ($(REGION),JP)
ifeq ($(ATOK),TRUE)
LOCALE_DEFINES = -DSAMPLE_REGION_J -DATOK_LINK_USE
SKB_MAKE_OPT = LINK_RESOURCE=REGION_J_ONLY LINK_ATOK=ATOK_LINK_USE
else
LOCALE_DEFINES = -DSAMPLE_REGION_J -DATOK_NO_LINK
SKB_MAKE_OPT = LINK_RESOURCE=REGION_J_ONLY LINK_ATOK=ATOK_NO_LINK
endif
endif

ifeq ($(REGION),US)
LOCALE_DEFINES = -DSAMPLE_REGION_A -DATOK_NO_LINK 
SKB_MAKE_OPT = LINK_RESOURCE=REGION_J_OTHER_ONLY LINK_ATOK=ATOK_NO_LINK
endif

ifeq ($(REGION),EU)
LOCALE_DEFINES = -DSAMPLE_REGION_E -DATOK_NO_LINK 
SKB_MAKE_OPT = LINK_RESOURCE=REGION_J_OTHER_ONLY LINK_ATOK=ATOK_NO_LINK
endif


#--------------------------------------------------------------------
#
#

SHELL       = /bin/sh
XTERM       = xterm
KTERM       = kterm
DEBUGDIR = .


TOP         = /usr/local/sce

SCE_LIB_DIR = $(TOP)/ee/lib

ifeq ($(ATOK),TRUE)
ATOK_LIB    =  $(SCE_LIB_DIR)/libatok.a 
endif

CCC_DIR = utf8/libccc
CCC_INC_DIR = $(CCC_DIR)/include
CCC_LIB_DIR = $(CCC_DIR)/lib
CCC_LIB     = $(CCC_LIB_DIR)/libccc.a
JIS2UCS     = $(CCC_LIB_DIR)/jis2ucs.bin
UCS2JIS     = $(CCC_LIB_DIR)/ucs2jis.bin

PFONT_DIR  = libpfont
PFONT_INC_DIR = $(PFONT_DIR)/include
PFONT_LIB     = $(PFONT_DIR)/lib/libpfont.a


SKB_DIR      = ./skb
SKB_INC_DIR  = $(SKB_DIR)/include
SKB_LIB_DIR  = $(SKB_DIR)/lib


SKB_LIB      = $(SKB_LIB_DIR)/skb.a
LIBFEP_LIB   = $(SKB_LIB_DIR)/libfep.a
LIBSOFTKB_LIB= $(SKB_LIB_DIR)/libsoftkb.a
SKBRES_LIB   = $(SKB_LIB_DIR)/skbres.a
FONTWRAP_LIB = $(SKB_LIB_DIR)/fontwrap.a


SOFTKB_LIBS = \
       $(SKB_LIB)       \
       $(LIBFEP_LIB)    \
       $(LIBSOFTKB_LIB) \
       $(SKBRES_LIB)    \
       $(FONTWRAP_LIB)  \


#----------------------------------------

LIBS   = \
        $(CCC_LIB)    \
        $(PFONT_LIB)  \
        $(ATOK_LIB)   \
        $(SCE_LIB_DIR)/ntgui2.a  \
        $(SCE_LIB_DIR)/libcdvd.a  \
        $(SCE_LIB_DIR)/libgraph.a \
        $(SCE_LIB_DIR)/libpad.a   \
        $(SCE_LIB_DIR)/libmc.a    \
        $(SCE_LIB_DIR)/libscf.a   \
        $(SCE_LIB_DIR)/libusbkb.a \
        $(SCE_LIB_DIR)/libvu0.a   \
        $(SCE_LIB_DIR)/libpkt.a   \
        $(SCE_LIB_DIR)/libdma.a   \
        $(SCE_LIB_DIR)/libmrpc.a  \
        $(SCE_LIB_DIR)/libinsck.a \
        $(SCE_LIB_DIR)/libnet.a   \
        $(SCE_LIB_DIR)/libsdr.a   \
        $(SCE_LIB_DIR)/libmsin.a  \
        $(SCE_LIB_DIR)/libkernl.a \

INCDIR = . \
        -I$(CCC_INC_DIR)   \
        -I$(PFONT_INC_DIR) \
        -I$(SKB_INC_DIR)   \
        -I$(TOP)/ee/include     \
        -I$(TOP)/common/include \


LCFILE    = $(SCE_LIB_DIR)/app.cmd

#*****************************************

PREFIX      = ee-
AS          = $(PREFIX)gcc
CC          = $(PREFIX)gcc
LD          = $(PREFIX)gcc
GDB         = $(PREFIX)gdb
DVPASM      = $(PREFIX)dvp-as
OBJDUMP     = $(PREFIX)objdump -mmips:5900
OBJCOPY     = $(PREFIX)objcopy
BIN2ELF     = $(SDK)/bin/bin2elf
ERUN        = dsedb -noexit -r run

RM          = /bin/rm -f

COMMON_CFLAGS = -O2 -g -Wall -Wa,-al -fno-common -fno-strict-aliasing # -mvu0-use-vf0-vf15

CFLAGS      =  $(COMMON_CFLAGS) $($(PROJECT)_DEFINES) $(DEBUGGER_DEFINES) $(LOCALE_DEFINES) $(GCC_DEF)
CXXFLAGS    =  $(COMMON_CFLAGS) -Woverloaded-virtual  $($(PROJECT)_DEFINES) $(DEBUGGER_DEFINES) $(GCC_DEF)
ASFLAGS     = -c -xassembler-with-cpp -Wa,-al
DVPASMFLAGS =
#LDFLAGS     = -Wl,-Map,$(TARGET).map -nostartfiles -L $(SCE_LIB_DIR) -lm
LDFLAGS     = -Wl,-Map,$(TARGET).map -mno-crt0 -L $(SCE_LIB_DIR) -lm
TMPFLAGS    =

DEPEND	= makedepend

OBJS   = \
        main.o      \
        pad.o       \
        kbd.o       \

EXTRA_OBJS = crt0.o \


# end makefile defines
#--------------------------------------------------------------------


#--------------------------------------------------------------------
# makefile target templates

.SUFFIXES: .c .s .cc .dvpasm .dvp .vpu .vuasm


MAKED = @$(SHELL) -ec 'cat $(notdir $*).d | sed '\''s=\($(notdir $*)\)\.o[ :]*=$*.o \
		$*.md : =g'\'' > $*.md; \
		rm -f $(notdir $*).d'

MAKEDVPD = @$(SHELL) -ec 'cat $(notdir $*).d | sed '\''s=\($(notdir $*)\)\.dvp.o[ :]*=$*.o \
		$*.md : =g'\'' > $*.md; \
		rm -f $(notdir $*).d'

.s.o:
	@echo -n "$< : "
	$(AS) $(ASFLAGS) $(TMPFLAGS) -I $(INCDIR) -o $@ $< > $*.lst

%.o %.md: %.dvp
	@echo -n "$< : "
	$(CC) -MD $(CXXFLAGS) $(TMPFLAGS) -I $(INCDIR) -E -o $*.dvpasm -x assembler-with-cpp $< > $*.lst
	$(DVPASM) $(DVPASMFLAGS) -I $(INCDIR) -o $(basename $@).o $*.dvpasm > $*.lst
	@$(RM) $*.dvpasm
	$(MAKEDVPD)

%.o %.md: %.c
	@echo C Compile   [[33m $< [m]
	@$(CC) -MD $(CFLAGS) $(TMPFLAGS) -I $(INCDIR) -c $< -o $*.o > $*.lst
	$(MAKED)

%.md %.o: %.cc
	@echo -n "$< : "
ifdef GENERATE_ASM
	$(CC) $(CXXFLAGS) -fverbose-asm $(TMPFLAGS) -I $(INCDIR) -S $< -o $*.S > $*.lst
endif
	$(CC) -MD $(CXXFLAGS) $(TMPFLAGS) -I $(INCDIR) -c $< -o $*.o > $*.lst
	$(MAKED)



# end makefile target templates
#--------------------------------------------------------------------



#--------------------------------------------------------------------
# makefile targets:

all: make_skb make_elf

make_skb: 
	@echo ---- skb make ---------------------------------------------
	@echo $(MAKE) $(SKB_MAKE_OPT)
	@(cd $(SKB_DIR); $(MAKE) $(SKB_MAKE_OPT))
	@echo ---- skb make end -----------------------------------------

make_elf: $(TARGET) 


$(TARGET).elf: $(OBJS) $(EXTRA_OBJS) $(LIBS) $(SOFTKB_LIBS)
	@echo -n "$@ : "
	$(LD) -o $@ -T $(LCFILE) $(OBJS) $(EXTRA_OBJS) $(SKB_OBJS) $(SOFTKB_LIBS) $(LIBS) $(LDFLAGS)

$(PROJECT)-objs:	$(TARGET).elf


crt0.o: $(SCE_LIB_DIR)/crt0.s
	$(AS) $(ASFLAGS) $(TMPFLAGS) -o $@ $< > $*.lst


$(TARGET): $(TARGET).elf #make-objs $(LIBS)
	@echo "Compile finished."


run: all
	@echo "Executing " $(TARGET).elf "..."
	@(umask 2; $(ERUN) $(TARGET).elf)
	@echo "Finished."


clean:
	@echo ---- skb clean ---------------------------------------------
	@(cd $(SKB_DIR); $(MAKE) clean)
	@echo ---- skb clean end -----------------------------------------
	-$(RM) *~ *.bak "#*" *.o *.rpo *.S
	-$(RM) $(TARGET).elf
	-$(RM) tmp/*.*
	-$(RM) *.tmp
	-$(RM) *.map *.lst core *.dis
	-$(RM) *.vuasm *.dvpasm
	-$(RM) *.d *.md
	-$(RM) xerun.sh


# end makefile targets
#--------------------------------------------------------------------

ifneq "$(strip $($(PROJECT)OBJS))" ""

-include $($(PROJECT)OBJS:.o=.md)

endif

