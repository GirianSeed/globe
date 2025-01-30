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

#include <libccc.h>
#include "app.h"
#include "io.h"

// Makefile内でCCC_PATHが設定されている事に注意
// ファイルデバイスはio.cで定義
#define JIS2UCS_PATH    CCC_PATH "/libccc/lib/jis2ucs.bin"
#define UCS2JIS_PATH    CCC_PATH "/libccc/lib/ucs2jis.bin"

// 変換バッファのサイズ
#define BUFSZ           (256)

static sceCccJISCS      *_jis2ucs;
static sceCccUCS2       *_ucs2jis;
static sceCccUTF8	dst[BUFSZ];
static int		len;		// 最後の変換による文字数

/*
 * libcccを使うには変換テーブルを初期化してあげなくてはならない
 */
void ccc_init(void)
{
    // 変換テーブルファイルを読み込む
    _jis2ucs = (sceCccJISCS *)file_read(JIS2UCS_PATH);
    _ucs2jis  = (sceCccUCS2 *)file_read(UCS2JIS_PATH);

    // 変換テーブルを設定する
    sceCccSetTable(_jis2ucs, _ucs2jis);
}

/*
 * SJIS->UTF8変換を行いdstのアドレスを返す
 * 変換結果の文字数はget_len_ccc()を使うこと
 * staticなバッファを使っているのでthread safeではない
 * BUFSZ byte以上の場合は完全変換されない事に注意
 */
const sceCccUTF8 *ccc_conv_sjis2utf8(const char *sjis)
{
    len = sceCccSJIStoUTF8(dst, BUFSZ, sjis);
    return dst;
}

/*
 * 最後の変換の文字数を返す
 */
int ccc_get_conv_length(void)
{
    return len;
}
