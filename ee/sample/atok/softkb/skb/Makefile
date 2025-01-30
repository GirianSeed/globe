#--------------------------------------------------------------------
# ソフトウェアキーボード リソース
RESOURCE_SOURCE = SKBRES_LINK         # ライブラリとしてリンク
#RESOURCE_SOURCE = SKBRES_LOAD        # ファイルロード

# リソースロードパス切り替え。 SKBRES_LINK 時無効。
CDBOOT = 0                            # 1: cdrom    

# リソースのストリップ
ifndef $(LINK_RESOURCE)
#LINK_RESOURCE = REGION_LINK_ALL      # 全て使用
LINK_RESOURCE = REGION_J_ONLY         # 日本版のリソースのみ有効
#LINK_RESOURCE = REGION_J_OTHER_ONLY  # 海外版のリソースのみ有効
endif

ifndef $(LINK_ATOK)
LINK_ATOK = ATOK_LINK_USE             # ATOKライブラリを使用します。
#LINK_ATOK  = ATOK_NO_LINK            # ATOKライブラリを使用しません。
endif

# 以上を変更したら make define をやって、common.def を生成する。
#--------------------------------------------------------------------
DEFINES = -D$(RESOURCE_SOURCE) -D$(LINK_RESOURCE) -DCDBOOT=$(CDBOOT) -D$(LINK_ATOK)

#--------------------------------------------------------------------
# コンパイラ LANG 環境変数対応  ee-gcc ver 2.96-ee-001003-1
# 
#LANG=C-SJIS
#LANG=ja_JP.eucJP
#--------------------------------------------------------------------

SUBDIRS = fontwrap resource libsoftkb fep skb 

COMMON_DEF_FILE = common.def

all: lib $(COMMON_DEF_FILE)
	@echo ++++ $(COMMON_DEF_FILE) +++++++++++++++++++++++++++++++++++++++++++++++++++
	@more common.def
	@echo +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@for i in $(SUBDIRS); do \
		if [ -d $$i ]; then \
			(echo "[ make < $$i > ]"; cd $$i; make ) || break; \
		fi; \
	done

define: clean
	@echo ++++ make $(COMMON_DEF_FILE) ++++++++++++++++++++++++++++++++++++++++++++++
	echo COMMON_DEF=$(DEFINES) > $(COMMON_DEF_FILE)
	@echo +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

$(COMMON_DEF_FILE):
	@echo ++++ make $(COMMON_DEF_FILE) ++++++++++++++++++++++++++++++++++++++++++++++
	echo COMMON_DEF=$(DEFINES) > $(COMMON_DEF_FILE)

lib:
	mkdir lib

clean: $(COMMON_DEF_FILE)
	@for i in $(SUBDIRS); do \
		if [ -d $$i ]; then \
			(cd $$i; make clean) || break; \
		fi; \
	done
	rm -f $(COMMON_DEF_FILE)
