#
#  IOP module sample program Makefile
#
#  $Id: Makefile,v 1.2 1999/08/18 07:06:06 tei Exp $
#
TOP = $(dir $(word 1, $(wildcard include ../include ../../include \
		../../../include ../../../../include ) ))

include $(TOP)lib/Makedefs

CFLAGS  = -I$(TOP)include -Wall
ASFLAGS  = -I$(TOP)include

all: mylib.irx client.irx

# 常駐ライブラリのコンパイル ===============================================
mylib.irx : mylib.o mylib.tbl
#	  ライブラリエントリテーブルの作成
	$(ILBGEN) mylib.tbl -e mylibent.s
	$(AS) -o mylibent.o mylibent.s
	rm -f mylibent.s
#	   常駐ライブラリの本体のコンパイルとエントリテーブルとのリンク
# 	   -e オプションで初期化エントリを指定してください
	$(LINK.o) -e MyLibInit mylib.o mylibent.o  \
		$(LOADLIBES) $(LDLIBS) -o $@ \
		-L$(TOP)lib -ilb=$(TOP)lib/iop.ilb

# ライブラリを利用するプログラムのリンク時に必要な mylib.ilb ファイルの作成
mylib.ilb: mylib.tbl
	$(ILBGEN) mylib.tbl -d mylib.ilb

# ライブラリを利用するプログラムのコンパイル ===============================
client.irx : client.o mylib.ilb
	$(LINK.o) $(TOP)lib/crt0.o client.o $(LOADLIBES) $(LDLIBS) -o $@ \
		-L$(TOP)lib -ilb=$(TOP)lib/iop.ilb  -ilb=mylib.ilb
#	  プログラムがどの常駐ライブラリを呼び出しているかは
#	  ioplibdumpユーティリティで確認できます。
	$(ILBDUMP) client.irx 
#	  さらに、どの関数を呼び出しているかも ilb ファイルを
#	  指定することで確認できます
	$(ILBDUMP) client.irx : $(TOP)lib/iop.ilb  mylib.ilb

clean:
	rm -f *.o *.irx *.ilb *.map

run:
	dsreset 0 2; dsistart mylib.irx; dsistart client.irx
