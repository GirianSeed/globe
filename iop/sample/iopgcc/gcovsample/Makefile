#
#  IOP coverage testing sample Makefile
#
#  $Id: Makefile,v 1.2 2003/02/13 07:48:12 tei Exp $
#

#----------- customize section --------------
DBGFLAG  = -g
OPTFLAG  = -O0 -fprofile-arcs -ftest-coverage
#J プログラムコンパイル時に -fprofile-arcs をつけなければ
#J カバレッジテスト用コードは生成されず、libiopgcov.a 中のコードも
#J リンクされません。
#J したがって、 OPTFLAGの定義を
#J    OPTFLAG  = -O2 
#J と置き換えれば、テスト対象のプログラムソースを変更すること無く
#J リリース版のオブジェクトを作成することができます。
#J
CFLAGS   = -Wall -I../gcovlib $(DBGFLAG) $(OPTFLAG)
ASFLAGS  = $(DBGFLAG)
LDFLAGS  = $(DBGFLAG) -L../gcovlib
LOADLIBES= -liopgcov

PROG1 = noresident.irx
PROG2 = resident.irx
PROG3 = removable.irx

all:	$(PROG1) $(PROG2) $(PROG3)

$(PROG1): $(PROG1:.irx=.o) ../gcovlib/libiopgcov.a
	$(CC) $(LDFLAGS) -o $@ $(@:.irx=.o) $(LOADLIBES) 

$(PROG2): $(PROG2:.irx=.o) ../gcovlib/libiopgcov.a
	$(CC) $(LDFLAGS) -o $@ $(@:.irx=.o) $(LOADLIBES) 

$(PROG3): $(PROG3:.irx=.o) ../gcovlib/libiopgcov.a
	$(CC) $(LDFLAGS) -o $@ $(@:.irx=.o) $(LOADLIBES) 

../gcovlib/libiopgcov.a:
	(cd ../gcovlib; $(MAKE))

clean:
	rm -f *.o $(PROG1) $(PROG2) $(PROG3) *.map *.c.gcov *.da *.bb *.bbg

#----------- rules --------------
%.irx: %.o
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@

-include PathDefs
PathDefs:
	iop-path-setup > PathDefs || (rm -f PathDefs ; exit 1)
