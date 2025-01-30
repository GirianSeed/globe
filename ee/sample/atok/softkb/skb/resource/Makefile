
TARGET   = skbres.a
TARGET_H = skbres.h

TOP = /usr/local/sce

PREFIX      = ee-
AS          = $(PREFIX)gcc
CC          = $(PREFIX)gcc
LD          = $(PREFIX)gcc
AR          = $(PREFIX)ar
RANLIB      = $(PREFIX)ranlib
BIN2ELF     = $(TOP)/bin/bin2elf

SKBRES_OBJS= \
               abc_bin.o        \
               aiuHira_bin.o    \
               aiuKana_bin.o    \
               kigo_bin.o       \
               kigo_dum_bin.o   \
               num_bin.o        \
               qwerty_bin.o     \
               cfrenchQ_bin.o  \
               dutchQ_bin.o    \
               frenchQ_bin.o   \
               germanQ_bin.o   \
               idiomChr_bin.o  \
               italianQ_bin.o  \
               numF_bin.o      \
               portugQ_bin.o   \
               spSign_bin.o    \
               spanishQ_bin.o  \
               ukQ_bin.o       \
               usQ_bin.o       \
               skbHDch_tm2.o  \
               skbHEng_tm2.o  \
               skbHFch_tm2.o  \
               skbHGmn_tm2.o  \
               skbHIta_tm2.o  \
               skbHItem_tm2.o \
               skbHJp_tm2.o   \
               skbHPtg_tm2.o  \
               skbHSp_tm2.o   \
               fepbar_tm2.o     \
               skbBG_tm2.o      \
               skbBtn_tm2.o     \
               skbKey_tm2.o     \
               skbMdbar_tm2.o   \
               tField_tm2.o     \


all: $(TARGET) ../lib/$(TARGET) ../include/$(TARGET_H)

../lib/$(TARGET):$(TARGET)
	cp $(TARGET) ../lib

../include/$(TARGET_H):
	cp $(TARGET_H) ../include

$(TARGET): $(SKBRES_OBJS)
	$(RM) $@
	$(AR) r $@ $(SKBRES_OBJS)
	$(RANLIB) $@

clean:
	rm -f *.o *.a
	$(RM) ../include/$(TARGET_H)
	$(RM) ../lib/$(TARGET)

abc_bin.o :  key/jp/abc.bin 
	$(BIN2ELF) --EE -a 80 -s $(*F) -e $(*F)_end $< $@
aiuHira_bin.o :  key/jp/aiuHira.bin
	$(BIN2ELF) --EE -a 80 -s $(*F) -e $(*F)_end $< $@
aiuKana_bin.o :  key/jp/aiuKana.bin
	$(BIN2ELF) --EE -a 80 -s $(*F) -e $(*F)_end $< $@
kigo_bin.o :  key/jp/kigo.bin
	$(BIN2ELF) --EE -a 80 -s $(*F) -e $(*F)_end $< $@
kigo_dum_bin.o :  key/jp/kigo_dum.bin
	$(BIN2ELF) --EE -a 80 -s $(*F) -e $(*F)_end $< $@
num_bin.o :  key/jp/num.bin
	$(BIN2ELF) --EE -a 80 -s $(*F) -e $(*F)_end $< $@
qwerty_bin.o :  key/jp/qwerty.bin
	$(BIN2ELF) --EE -a 80 -s $(*F) -e $(*F)_end $< $@

cfrenchQ_bin.o :  key/other/cfrenchQ.bin
	$(BIN2ELF) --EE -a 80 -s $(*F) -e $(*F)_end $< $@
dutchQ_bin.o :  key/other/dutchQ.bin
	$(BIN2ELF) --EE -a 80 -s $(*F) -e $(*F)_end $< $@
frenchQ_bin.o :  key/other/frenchQ.bin
	$(BIN2ELF) --EE -a 80 -s $(*F) -e $(*F)_end $< $@
germanQ_bin.o :  key/other/germanQ.bin
	$(BIN2ELF) --EE -a 80 -s $(*F) -e $(*F)_end $< $@
idiomChr_bin.o :  key/other/idiomChr.bin
	$(BIN2ELF) --EE -a 80 -s $(*F) -e $(*F)_end $< $@
italianQ_bin.o :  key/other/italianQ.bin
	$(BIN2ELF) --EE -a 80 -s $(*F) -e $(*F)_end $< $@
numF_bin.o :  key/other/numF.bin
	$(BIN2ELF) --EE -a 80 -s $(*F) -e $(*F)_end $< $@
portugQ_bin.o :  key/other/portugQ.bin
	$(BIN2ELF) --EE -a 80 -s $(*F) -e $(*F)_end $< $@
spSign_bin.o :  key/other/spSign.bin
	$(BIN2ELF) --EE -a 80 -s $(*F) -e $(*F)_end $< $@
spanishQ_bin.o :  key/other/spanishQ.bin
	$(BIN2ELF) --EE -a 80 -s $(*F) -e $(*F)_end $< $@
ukQ_bin.o :  key/other/ukQ.bin
	$(BIN2ELF) --EE -a 80 -s $(*F) -e $(*F)_end $< $@
usQ_bin.o :  key/other/usQ.bin
	$(BIN2ELF) --EE -a 80 -s $(*F) -e $(*F)_end $< $@

skbHDch_tm2.o :  tex/help/skbHDch.tm2
	$(BIN2ELF) --EE -a 80 -s $(*F) -e $(*F)_end $< $@
skbHEng_tm2.o :  tex/help/skbHEng.tm2
	$(BIN2ELF) --EE -a 80 -s $(*F) -e $(*F)_end $< $@
skbHFch_tm2.o :  tex/help/skbHFch.tm2
	$(BIN2ELF) --EE -a 80 -s $(*F) -e $(*F)_end $< $@
skbHGmn_tm2.o :  tex/help/skbHGmn.tm2
	$(BIN2ELF) --EE -a 80 -s $(*F) -e $(*F)_end $< $@
skbHIta_tm2.o :  tex/help/skbHIta.tm2
	$(BIN2ELF) --EE -a 80 -s $(*F) -e $(*F)_end $< $@
skbHItem_tm2.o :  tex/help/skbHItem.tm2
	$(BIN2ELF) --EE -a 80 -s $(*F) -e $(*F)_end $< $@
skbHJp_tm2.o :  tex/help/skbHJp.tm2
	$(BIN2ELF) --EE -a 80 -s $(*F) -e $(*F)_end $< $@
skbHPtg_tm2.o :  tex/help/skbHPtg.tm2
	$(BIN2ELF) --EE -a 80 -s $(*F) -e $(*F)_end $< $@
skbHSp_tm2.o :  tex/help/skbHSp.tm2
	$(BIN2ELF) --EE -a 80 -s $(*F) -e $(*F)_end $< $@

fepbar_tm2.o :  tex/skb/fepbar.tm2
	$(BIN2ELF) --EE -a 80 -s $(*F) -e $(*F)_end $< $@
skbBG_tm2.o :  tex/skb/skbBG.tm2
	$(BIN2ELF) --EE -a 80 -s $(*F) -e $(*F)_end $< $@
skbBtn_tm2.o :  tex/skb/skbBtn.tm2
	$(BIN2ELF) --EE -a 80 -s $(*F) -e $(*F)_end $< $@
skbKey_tm2.o :  tex/skb/skbKey.tm2
	$(BIN2ELF) --EE -a 80 -s $(*F) -e $(*F)_end $< $@
skbMdbar_tm2.o :  tex/skb/skbMdbar.tm2
	$(BIN2ELF) --EE -a 80 -s $(*F) -e $(*F)_end $< $@
tField_tm2.o :  tex/skb/tField.tm2
	$(BIN2ELF) --EE -a 80 -s $(*F) -e $(*F)_end $< $@




