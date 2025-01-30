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

/* モードinfo用str : app.hのmode_tに合せる事 */
static const char *modestr[] = {
    "入力変換テスト",
    "ユーザ単語登録",
    "ユーザ単語削除"
};

static sceCccUTF8 dst[256];             // ccc用のバッファ

// 現在のモードを表示
void info_disp_mode(int mode, int x, int y)
{

    sceCccSJIStoUTF8(dst, 256, "モード:");
    font_put(x, y, dst);
    sceCccSJIStoUTF8(dst, 256, modestr[mode]);
    font_put(x+4, y, dst);
}

// 共通の入力方法(mode切替)に対しての表示
void info_disp_common(int x, int y)
{
    sceCccSJIStoUTF8(dst, 256, "SELECT   : モード切替");
    font_put(x, y, dst);    
}

// 入力に関する操作方法を表示
void info_disp_input_info(int x, int y)
{
    static char *string[] = 
        { "←→     : 入力アルファベット選択",
          "○       : 入力アルファベット確定",
          "□       : アルファベット１文字削除",
          "△/START : 入力を確定する" };
    static const int num_of_str = sizeof(string)/sizeof(char *);
    int i;

    for (i = 0; i < num_of_str; i++) {
        sceCccSJIStoUTF8(dst, 256, string[i]);
        font_put(x, y++, dst);
    }
}

// 変換に関する操作方法を表示
// can_extendがtrueなら文節伸ばし／縮めるのinfoも出す
void info_disp_conv_info(int x, int y, int can_extend)
{
    static char *string1[] =
        { "→       : 注目文節確定",
          "△       : 注目文節変換",
          "START    : 変換を確定する"
        };
    static char *string2[] =
        { "○       : 注目文節を伸ばす",
          "□       : 注目文節を縮める" };
    static const int num_of_str1 = sizeof(string1)/sizeof(char *);
    static const int num_of_str2 = sizeof(string2)/sizeof(char *);
    int i;

    for (i = 0; i < num_of_str1; i++) {
        sceCccSJIStoUTF8(dst, 256, string1[i]);
        font_put(x, y++, dst);
    }
    if (!can_extend)    return;
    for (i = 0; i < num_of_str2; i++) {
        sceCccSJIStoUTF8(dst, 256, string2[i]);
        font_put(x, y++, dst);
    }
}

// 登録／削除終了時のcontinueメッセージ
void info_disp_continue(int x, int y)
{
    sceCccSJIStoUTF8(dst, 256, "START    : 戻る");
    font_put(x, y, dst);
}

// 現在のファイル情報を表示
void info_disp_file_stat(int x, int y)
{
    struct sce_stat buf;
    int ret;

    sceSync(ATOK_DEVICE ATOK_DIR USERDIC, 0);
    if ((ret = sceGetstat(ATOK_DEVICE ATOK_DIR USERDIC, &buf)) != 0)
        scePrintf("ERROR sceGetstat [%d]\n", ret);
    else {
        sceCccSJIStoUTF8(dst, 256, "ユーザー辞書サイズ :");
        font_put(x, y, dst);
        sprintf(dst, "%d", buf.st_size);
        font_put(x + 10, y, dst);
    }
}

