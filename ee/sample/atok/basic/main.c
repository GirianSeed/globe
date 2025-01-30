/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/*
 *                      ATOK Library Sample
 *
 *                         Version 0.10
 *                           Shift-JIS
 *
 *      Copyright (C) 2002 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *      0.10
 */
#include <eekernel.h>
#include <stdio.h>
#include <stdlib.h>
#include <sifrpc.h>
#include <sifdev.h>
#include <libgraph.h>
#include <libpkt.h>
#include <malloc.h>
#include <libatok.h>
#include <libdma.h>
#include <libccc.h>
#include <string.h>

#include "app.h"                // application設定ヘッダ
#include "pad.h"                // pad.c用ヘッダ
#include "font.h"               // font.c用ヘッダ
#include "ccc.h"                // ccc.c用ヘッダ
#include "info.h"               // information表示用ヘッダ
/*
 * libatok.aの基本的操作を知る為のサンプル
 * 電源管理等は一切行っていないので、実際の使用における
 * 注意事項等はsoftkbのサンプルを参照
 *
 * ATOKライブラリ関係のコメントはCコメントで、
 * その他のコメントはC++コメントにしている
 */

// GS Double Buffer
static sceGsDBuff db;
/* Atok Library Context: alignされている必要がある */
sceAtokContext atokctx __attribute__ ((aligned(64)));
/* dynamic library place: atok engineの場所。こちらもalignが必要 */
static char dlib[512 * 1024] __attribute__ ((aligned(64)));
/* 辞書拡張の為のwork buffer: alignされて16KB以上であることが推奨される */
#define WORK_BUFFER_SIZE        (2 * 1024 * 1024)
static char work_buffer[WORK_BUFFER_SIZE] __attribute__((aligned(64)));


// PAD入力用
static char alphabet[] =
{ 'a', 'b', 'c', 'd', 'e',
  'f', 'g', 'h', 'i', 'j',
  'k', 'l', 'm', 'n', 'o',
  'p', 'q', 'r', 's', 't',
  'u', 'v', 'w', 'x', 'y', 'z' };
static const u_int num_alpha = sizeof(alphabet) / sizeof(char);


static void init_modules(void);
static void init_ps2(void);
static void mount_pfs(void);
static void display_sync(sceGsDBuff *dbp, int side);

static void init_atok(void);
static void setup_atok(void);

static void test_mode_func(int);
static void reg_mode_func(int);
static void del_mode_func(int);

static void input_func(int, int);
static char *convert_func(int, int, int);

int main(int argc, char **argv)
{
    int frame = 0;

    // モジュールの初期化
    init_modules();

    // 通常のPS2初期化
    init_ps2();

    // PFSのマウント
    mount_pfs();

    // libcccの初期化
    ccc_init();

    // libatokの初期化
    init_atok();		/* Atokエンジンの初期化 */
    setup_atok();		/* Atokエンジンの設定 */

    // 表示Fontの初期化: このサンプルではlibpfontを使っている
    font_init();                // in font.c

    for (;;) {
        sceSamplePad_t *pad;            // in pad.h
        static mode_t mode = TEST_MODE;
        static int init = 0;

        // pad情報の更新＆取得
        pad = pad_update();             // in pad.c

        // mode info
        info_disp_mode(mode, 0, 1);	// in info.c

        // file info
        info_disp_file_stat(3, 0);	// in info.c

        // Atok Configure info
        info_disp_common(3, 7);		// in info.c
        switch (mode) {
          case TEST_MODE:	// 変換テストモード
            test_mode_func(init);
            init = 1;
            break;
          case REG_MODE:	// 辞書単語登録モード
            reg_mode_func(init);
            init = 1;
            break;
          case DEL_MODE:	// 辞書単語削除モード
            del_mode_func(init);
            init = 1;
            break;
        }

        init = 1;

        // モード切替はselect
        if (pad->SelectSw) {
            ++mode;
            if (mode > DEL_MODE)
                mode = TEST_MODE;
            init = 0;
        }
        display_sync(&db, frame & 1);
        frame++;
    }
    
    return 0;
}

static void init_modules(void)
{
#define SIO2MAN         "host0:/usr/local/sce/iop/modules/sio2man.irx"
#define PADMAN          "host0:/usr/local/sce/iop/modules/padman.irx"
#define DEV9            "host0:/usr/local/sce/iop/modules/dev9.irx"
#define ATAD            "host0:/usr/local/sce/iop/modules/atad.irx"
#define HDD             "host0:/usr/local/sce/iop/modules/hdd.irx"
#define PFS             "host0:/usr/local/sce/iop/modules/pfs.irx"
    sceSifInitRpc(0);
    while (sceSifLoadModule(SIO2MAN, 0, NULL) < 0)
        scePrintf("Can't load module %s\n", SIO2MAN);
    while (sceSifLoadModule(PADMAN, 0, NULL) < 0)
        scePrintf("Can't load module %s\n", PADMAN);
    while (sceSifLoadModule(DEV9, 0, NULL) < 0)
        scePrintf("Can't load module %s\n", DEV9);
    while (sceSifLoadModule(ATAD, 0, NULL) < 0)
        scePrintf("Can't load module %s\n", ATAD);
    while (sceSifLoadModule(HDD, 0, NULL) < 0)
        scePrintf("Can't load module %s\n", HDD);
    while (sceSifLoadModule(PFS, 0, NULL) < 0)
        scePrintf("Can't load module %s\n", PFS);
#undef PFS
#undef HDD
#undef ATAD
#undef DEV9
#undef PADMAN
#undef SIO2MAN
}

static void init_ps2(void)
{
    // DMAの初期化
    sceDmaReset(1);

    // GSの初期化
    sceGsResetPath();
    sceGsResetGraph(0, SCE_GS_INTERLACE, SCE_GS_NTSC, SCE_GS_FRAME);
    sceGsSetDefDBuff(&db, FBUF_FORMAT, SCREEN_WIDTH, SCREEN_HEIGHT,
                     SCE_GS_ZGREATER, ZBUF_FORMAT, SCE_GS_CLEAR);
    sceGsSyncPath(0, 0);
    sceGsSyncV(0);

    *(u_long *)&db.clear0.rgbaq = SCE_GS_SET_RGBAQ(0x00, 0x20, 0x40, 0x00, 0);
    *(u_long *)&db.clear1.rgbaq = SCE_GS_SET_RGBAQ(0x00, 0x20, 0x40, 0x00, 0);

    // Padの初期化
    pad_init();         // pad.c
}

static void mount_pfs(void)
{
    int res;

    // drive情報を得る
    res = sceDevctl(ATOK_DRIVE, HDIOC_STATUS, NULL, 0, NULL, 0);
    switch (res) {
      case 0:   scePrintf("HDD ok\n");                          break;
      case 1:   scePrintf("HDD drive unformat\n");              break;
      case 2:   scePrintf("HDD reserved\n");                    break;
      case 3:   scePrintf("HDD not connected\n");               break;
      case -19: scePrintf("%s device not found\n", ATOK_DRIVE); break;
      case -24: scePrintf("cannot open\n");                     break;
      default:  scePrintf("Unknown error\n");                   break;
    }

    // マウントを行う
    if (res == 0) {
        res = sceMount(ATOK_DEVICE, ATOK_DRIVE "__sysconf", 0, NULL, 0);
    }

    if (res < 0) {
        scePrintf("cannot mount device. errno = %d\n", res);
        exit (-1);
    }
}

static void display_sync(sceGsDBuff *dbp, int side)
{
    sceGsDrawEnv1 *draw;        // next draw target
    sceGsGParam *gp;
    int field;

    FlushCache(0);

    draw = side ? &dbp->draw1 : &dbp->draw0;

    gp = sceGsGetGParam();
    field = !sceGsSyncV(0);
    if ((gp->sceGsInterMode == SCE_GS_INTERLACE) &&
        (gp->sceGsFFMode = SCE_GS_FRAME)) {
        sceGsSetHalfOffset(draw, SCREEN_CENTERX, SCREEN_CENTERY, field);
    }
    sceGsSwapDBuff(dbp, side);
}

/*--------------------------------------------------
 * libatokを使った関数群
 *--------------------------------------------------*/
/*
 * Atokの初期化。app.hで定義されているファイルを使って
 * atokctxをコンテキストとして初期化する
 * dynamic libraryをloadする場所dlib、atokのコンテキストatokctxは
 * alignを守らなくてはならない事に注意
 */
void init_atok(void)
{
    int ret;

    ret = sceAtokInit(&atokctx,                         /* context pointer */
                      ATOK_DEVICE ATOK_DIR SYSDIC,      /* system dic path */
                      ATOK_DEVICE ATOK_DIR USERDIC,     /* user dic path   */
                      ATOK_DEVICE,                      /* sysconf device  */
                      (void *)dlib);                    /* dlib buffer     */
    if (ret != 0) {
        scePrintf("ERROR: sceAtokInit [%d]\n", ret);
        exit(-1);
    }
    ret = sceAtokKanjiOn(&atokctx);
    if (ret != 0) {
        scePrintf("ERROR: sceAtokKanjiOn [%d]\n", ret);
        exit(-1);
    }
}

/*
 * Atokの初期の設定をする
 * このサンプルにおいては英数片仮名を全角変換するように変更する
 */
void setup_atok(void)
{
    sceAtokConfig       atokcfg;
    int ret;

    /* atokcfgにはsizeof(sceAtokConfig)を設定しなくてはならない
     * (Getを使うにあたっても)					*/
    atokcfg.Size = sizeof(sceAtokConfig);

    /* 現在の設定を取得し設定変更を行う
     * ここでは参考として全取得->変更->全設定を行う		*/
    ret = sceAtokGetConfig(&atokctx, &atokcfg, SCE_ATOK_CONFIG_ALL);
    if (ret != 0) {
        scePrintf("ERROR: sceAtokGetConfig [%d]\n", ret);
        exit(-1);
    }
    atokcfg.HZCnvDetail = 0x38; /* bit 4,5,6 : 数字、英字、片仮名->全角
                                 * see. libatok.h			*/
    atokcfg.ConvMode = SCE_ATOK_CONVMODE_AUTO;  /* 単／連文節の自動判別 */
    ret = sceAtokGetConfig(&atokctx, &atokcfg, SCE_ATOK_CONFIG_ALL);
    if (ret != 0) {
        scePrintf("ERROR: sceAtokSetConfig [%d]\n", ret);
        exit(-1);
    }
}

// 変換テスト用のモード
static void test_mode_func(int init)
{
    sceSamplePad_t *pad = pad_get();		// in pad.c
    static enum {
        INPUT_SUB,		// 読み入力
        CONVERT_SUB		// 変換
    } smode = INPUT_SUB;

    if (init == 0) {
        smode = INPUT_SUB;
        input_func(-1, -1);
        convert_func(-1, -1, FALSE);
    }

    switch (smode) {
      case INPUT_SUB:
        input_func(2, 5);
        if (pad->RupSw || pad->StartSw) {
            smode = CONVERT_SUB;
            // 通常のFEP(IME)では入力確定と共に最初の変換候補が現れるので
            // SUB MODE移行前に初期変換を行っておく
            sceAtokEditConv(&atokctx, SCE_ATOK_EDIT_CONVFORWARD, 0);
        }
        break;
      case CONVERT_SUB:
        if (pad->StartSw) {
            // INPUT_SUBに戻る時には古い変換情報は消す
            convert_func(-1, -1, FALSE);
            smode = INPUT_SUB;
        }
        convert_func(2, 5, TRUE);	// TRUEは文節伸縮可能である事をしめす
        break;
    }
}

// 辞書登録モード
static void reg_mode_func(int init)
{
#define BUFSZ   (256)
    sceSamplePad_t *pad = pad_get();		// in pad.c
    static enum {
        YOMI_INPUT_SUB,			// 読み入力
        REG_INPUT_SUB,			// 登録文字列の入力
        REG_CONVERT_SUB,		// 登録文字列の変換
        REGIST_SUB,			// 登録
        REGIST_CONT_SUB			// 登録完了画面
    } smode = YOMI_INPUT_SUB;
    static char yomi[BUFSZ];
    static char *registword;
    int ret;

    if (init == 0) {
        smode = YOMI_INPUT_SUB;
        input_func(-1, -1);
        convert_func(-1, -1, FALSE);
    }

    switch (smode) {
      case YOMI_INPUT_SUB:
        input_func(2, 5);
        if (pad->RupSw || pad->StartSw) {
            /* alphabet入力によってatokエンジンに入れられている
             * 入力文字列をyomiバッファに入れておく		*/
            sceAtokGetConvertingStr(&atokctx, yomi, BUFSZ);
            smode = REG_INPUT_SUB;
            input_func(-1,-1);
        }
        break;
      case REG_INPUT_SUB:
        // よみは常に表示しておく
        font_put(2, 5, ccc_conv_sjis2utf8(yomi));		// in font.c
        // 登録文字の入力
        input_func(2, 6);
        if (pad->RupSw || pad->RupSw) {
            smode = REG_CONVERT_SUB;
            /* Atokエンジンに入力された文字列の初期変換を行っておく */
            sceAtokEditConv(&atokctx, SCE_ATOK_EDIT_CONVFORWARD, 0);
        }
        break;
      case REG_CONVERT_SUB:
        // よみは常に表示
        font_put(2, 5, ccc_conv_sjis2utf8(yomi));		// in font.c

        if (pad->StartSw) {
            smode = REGIST_SUB;
        }
        registword = convert_func(2, 6, TRUE);
        break;
      case REGIST_SUB:
	/* 実登録を行う
	 * 第２引数は品詞番号を示し、以下の番号を渡す
	 * アプリケーションに品詞概念が無い場合は
	 * 6:名詞サ変での登録が好ましい(このサンプルでも使用)
	 *
	 * 1  : 一般名詞        11 : 連体詞     21 : ナ行五段
	 * 2  : 固有人名        12 : 接続詞     22 : ハ行五段
	 * 3  : 固有地名        13 : 感動詞     23 : バ行五段
	 * 4  : 固有組織        14 : 接頭語     24 : マ行五段
	 * 5  : 固有一般        15 : 接尾語     25 : ラ行五段
	 * 6  : 名詞サ変        16 : 数詞       26 : ワ行五段
	 * 7  : 名詞ザ変        17 : カ行五段   27 : 一般動詞
	 * 8  : 名詞形動        18 : ガ行五段   28 : カ変動詞
	 * 9  : 独立語          19 : サ行五段   29 : サ変動詞
	 * 10 : 単漢字          20 : タ行五段   30 : ザ変動詞
	 *                                      31 : 形容詞
	 *                                      32 : 形容動詞
	 *                                      33 : 副詞
	 */
        ret = sceAtokDicRegWord(&atokctx, 6, registword, yomi);
	/* retが10の場合は辞書拡張を行わなくてはならない
	 * この動作は現在の所場合によっては非常に長い時間がかかるため
	 * 実アプリケーションにおいてはWaiting状態を表示する事		*/
        if (ret == 10) {
            scePrintf("Block Full!!! Expand Dictionary Now!!!\n");
            ret = sceAtokDicExpand(&atokctx, (void *)work_buffer, (size_t) WORK_BUFFER_SIZE);
            if (ret == 0) {
                ret = sceAtokDicRegWord(&atokctx, 6, registword, yomi);
            }
        }
        if (ret != 0)
            scePrintf("ERROR: sceAtokDicRegWord/Expand. [%d]\n", ret);
        smode = REGIST_CONT_SUB;
        // fall throgh
      case REGIST_CONT_SUB:
        font_put(2, 5, ccc_conv_sjis2utf8("登録完了"));
        info_disp_continue(3, 8);
        if (pad->StartSw) {
            smode = YOMI_INPUT_SUB;
            input_func(-1, -1);
            convert_func(-1, -1, FALSE);
        }
        break;
    }
}

// 辞書登録単語の削除用モード
static void del_mode_func(int init)
{
#define BUFSZ   (256)
    sceSamplePad_t *pad = pad_get();
    static enum {
        YOMI_INPUT_SUB,			// 読み入力
        CONVERT_SELECT_SUB,		// 変換文字選択
        DELETE_SUB,			// 削除
        DELETE_CONT_SUB			// 削除完了画面
    } smode = YOMI_INPUT_SUB;
    static char yomi[BUFSZ];
    static char *delword;

    if (init == 0) {
        smode = YOMI_INPUT_SUB;
        input_func(-1, -1);
        convert_func(-1, -1, FALSE);
    }
    switch (smode) {
      case YOMI_INPUT_SUB:
        input_func(2, 5);
        if (pad->StartSw || pad->RupSw) {
	    /* 読み文字列が確定されたら
	     * yomiバッファに現在の文字を取得しておく	*/
            sceAtokGetConvertingStr(&atokctx, yomi, BUFSZ);
            smode = CONVERT_SELECT_SUB;
            /* 初期変換を行う */
            sceAtokEditConv(&atokctx, SCE_ATOK_EDIT_CONVFORWARD, 0);
            /* このサンプルにおいては、削除モードで読みに対して複数の文節の
	     * 存在は意味がないので、文節のカウントが１以上なら文節を伸ばし続ける
	     */
            while (sceAtokGetConvertingClauseCount(&atokctx) > 1)
		   sceAtokEditConv(&atokctx, SCE_ATOK_EDIT_FOCUSRIGHT, 0);
        }
        break;
      case CONVERT_SELECT_SUB:
        // よみは常に表示しておく
        font_put(2, 5, ccc_conv_sjis2utf8(yomi));
        if (pad->StartSw) {
            smode = DELETE_SUB;
            /* 初期変換を行っておく */
            sceAtokEditConv(&atokctx, SCE_ATOK_EDIT_CONVFORWARD, 0);
        }
        delword = convert_func(2, 6, FALSE);
        break;
      case DELETE_SUB:
        {
            int ret;

            /* 実消去を行う
             * 第２引数の16-8ビットは単語の種類を示し
	     *          1       : ユーザー登録単語
             *          2       : 自動登録単語
	     *          4       : 全て
             * を意味する。このサンプルでは4の全てを指定している
             * また第３引数は候補インデックスによる削除の場合に使う引数で
             * 次候補変換中に、候補インデックスを指定して削除する場合に使う
             * その場合は第２引数がいらない
             * このサンプルでは0xffff(インデックス指定でない)にしている
	     */
            ret = sceAtokDicDelWord(&atokctx, (4 << 8) | 6, 0xffff, delword, yomi, 0);
            if (ret != 0) {
                scePrintf("ERROR sceAtokDicDelWord [%d]\n", ret);
            }
            smode = DELETE_CONT_SUB;
        }
        /* fall throught */
      case DELETE_CONT_SUB:
        font_put(2, 5, ccc_conv_sjis2utf8("削除完了"));
        info_disp_continue(3, 8);
        if (pad->StartSw) {
            smode = YOMI_INPUT_SUB;
            input_func(-1, -1);
            convert_func(-1, -1, FALSE);
        }
        break;
    }
#undef BUFSZ
}

//
// 文字入力用関数
// Args:
//      x, y    : 入力を行う場所(2byte font単位座標)
//                x, y共に負数の場合、保持情報をclearする
//      pad     : pad情報
// 操作:
//      パッド左←→で入力alphabet選択
//      パッド右→でalphabet確定
//      パッド右←でalphabet 1文字削除
//
static void input_func(int x, int y)
{
#define BUFSZ   (256)
    sceSamplePad_t *pad = pad_get();
    static char buf[BUFSZ];
    static int alpha = 0;
    int len;

    // x, yが共に負数の場合初期化とみなす
    if (x < 0 && y < 0) {
        alpha = 0;
        buf[0] = '\0';
        /* atokエンジンの変換状態をclearしておく
	 * 明示的にclearしないと最後の状態が残る
	 */
        sceAtokEditConv(&atokctx, SCE_ATOK_EDIT_DELETEALL, 0);
        return;
    } else
        // 操作方法表示
        info_disp_input_info(3, 8);

    if (pad->LrightSw) ++alpha;
    if (pad->LleftSw) --alpha;
    /* 入力文字は文字コードをsceAtokEditConvに渡す */
    if (pad->RrightSw) sceAtokEditConv(&atokctx, alphabet[alpha], 0);
    /* 入力解除はSCE_ATOK_EDIT_BACKSPACEで行う */
    if (pad->RleftSw) sceAtokEditConv(&atokctx, SCE_ATOK_EDIT_BACKSPACE, 0);

    if (alpha < 0)              alpha = num_alpha - 1;
    if (alpha >= num_alpha)     alpha = 0;

    /* 入力された文字を表示
     * lenには該当文字のbyte数が返る。文字数ではない
     * また、入力文字は適時変換が行われる
     * このサンプルではローマ字入力の場合平仮名に変換されている
     * see. setup_atok()
     */
    if ((len = sceAtokGetConvertingStr(&atokctx, buf, BUFSZ)) > 0) {
        font_put(x, y, ccc_conv_sjis2utf8(buf));
    }

    // 選択中のalphabetを表示
    buf[0] = alphabet[alpha];
    buf[1] = '\0';
    font_put(x + len/2, y, ccc_conv_sjis2utf8(buf));

    // カーソルを表示
    font_put(x + len/2, y, ccc_conv_sjis2utf8("_"));

#undef BUFSZ
}

//
//  文字変換用関数
// Args:
//      x, y    : 変換を行う場所(2byte font単位座標)
//              x, y共に負数の場合、保持情報をclearする
// 操作:
//      パッド左→で注目文節確定
//      パッド右↑で注目文節変換
//      パッド右→で注目文節を延ばす
//      パッド右←で注目文節を縮める
// can_extendがFALSEの場合は文節の伸縮はできない
static char *convert_func(int x, int y, int can_extend)
{
#define BUFSZ   (256)
    sceSamplePad_t *pad = pad_get();
    static char buf[BUFSZ];
    static char det[BUFSZ];     // 確定文字列
    int i, len;

    if (x < 0 && y < 0) {
        det[0] = '\0';
        buf[0] = '\0';
        /* SCE_ATOK_EDIT_DELETEALLで変換エンジンの状態を空にしておく */
        sceAtokEditConv(&atokctx, SCE_ATOK_EDIT_DELETEALL, 0);
    } else
        // 操作方法表示
        info_disp_conv_info(3, 8, TRUE);


    /* 確定文字列が存在した場合detにcatする
     * 確定を行うとatok engineはその文字列の保持を止めてしまうので、
     * 確定文字の表示に必要となる
     */
    if (sceAtokGetConvertedStr(&atokctx, buf, BUFSZ) > 0) {
        strcat(det, buf);
        sceAtokFlushConverted(&atokctx); /* 一応２重登録しないようにFlush */
    }

    // 確定文字列を表示 : lenには文字数がかえる
    // ccc_get_conv_length()は最後のccc_conv_sjis2utf8()における変換によって
    // 生成された「文字数」を返す(sceCccSJIStoUTF8の仕様)ので
    // sceAtokGetConveringStr()とは違うことに注意
    font_put(x, y, ccc_conv_sjis2utf8(det));
    len = ccc_get_conv_length();

    // 未確定文字列がある場合bufに入れる。また操作ができるようにする
    if (sceAtokGetConvertingStr(&atokctx, buf, BUFSZ) > 0) {
        if (pad->RupSw)
            sceAtokEditConv(&atokctx, SCE_ATOK_EDIT_CONVFORWARD, 0);
        if (pad->LrightSw)
            sceAtokEditConv(&atokctx, SCE_ATOK_EDIT_KAKUTEIPART, 0);
        if (can_extend) {
            if (pad->RrightSw)
                sceAtokEditConv(&atokctx, SCE_ATOK_EDIT_FOCUSRIGHT, 0);
            if (pad->RleftSw)
                sceAtokEditConv(&atokctx, SCE_ATOK_EDIT_FOCUSLEFT, 0);
        }
    }

    // 未確定文字列を表示。確定文字列分表示をずらす必要がある
    font_put(x + len, y, ccc_conv_sjis2utf8(buf));

    // 注目文節にunderlineを引く
    /* sceAtokGetFocusClauseLen()は文字数ではなく「バイト数」を
     * 返すことに注意。						*/
    for (i = 0; i < sceAtokGetFocusClauseLen(&atokctx) / 2; i++) {
        font_put(x + len + i, y, ccc_conv_sjis2utf8("＿"));
    }
    return det;
#undef BUFSZ
}
