/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library libccc Version 1.1
 */
/*
 *      Copyright (C) 2003 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                     <libccc - libccc.h>
 */

#ifndef __libccc__
#define __libccc__

#include <sys/types.h>
#if defined(__cplusplus)
extern "C"{
#endif

/* ------------------------------------------------------------------------- 
   文字コード変換ライブラリ 
  
   ・マルチスレッドは考慮していません。
   ・現在の変換テーブルはJIStoUCS2、UCS2toJISそれぞれに65536個、計256kバイト必要です。
   ・SJISの0x20~0x7fの範囲についてはASCII 0x80~0xffの範囲は"JIS X 0201"相当です。
   ・JISは"JIS X 0213"を元にSCEI独自の文字セットを想定しています。
   ・unicodeは"ver1.1"を元にSCEI独自の文字セットを想定しています。
   ------------------------------------------------------------------------- */

/* 文字コード */
typedef unsigned short	sceCccJISCS;
typedef unsigned short	sceCccSJISCS;
typedef unsigned short	sceCccUCS2;
typedef unsigned int	sceCccUCS4;

/* 符合方式 */
typedef unsigned char	sceCccJISTF;
typedef unsigned char	sceCccSJISTF;
typedef unsigned char	sceCccUTF8;
typedef unsigned short	sceCccUTF16;

/* 符合化による１文字あたりの最大バイト数 */
#define __MAXCHRSIZE_SJIS__		(2)
#define __MAXCHRSIZE_UTF8__		(4)
#define __MAXCHRSIZE_UTF16__		(4)

/* ------------------------------------------------------------------------- 
   文字コード変換テーブル設定
   変換時は必ず設定する必要があります
   テーブルサイズは65536個。UCS2を越える範囲はサポートしない。
   jis2ucs : jis から ucs2 への対応表(jis並び)
   ucs2jis : ucs2 から jis への対応表(ucs2並び)
   ------------------------------------------------------------------------- */
void sceCccSetTable(sceCccUCS2 const* jis2ucs, sceCccJISCS const* ucs2jis);


/* ------------------------------------------------------------------------- 
   文字数カウント
   str : 文字列
   return : 文字数
   ------------------------------------------------------------------------- */
/* UTF16文字列の字数を数える */
int sceCccStrlenUTF16(sceCccUTF16 const* str);

/* UTF8文字列の字数を数える */
int sceCccStrlenUTF8(sceCccUTF8 const* str);

/* SJIS文字列の字数を数える */
int sceCccStrlenSJIS(sceCccSJISTF const* str);


/* ------------------------------------------------------------------------- 
   文字コードの有効性検証
   code : 文字コード
   return : 1=有効 0:無効
   注）現在は厳密なチェックを行なっていません。
   ------------------------------------------------------------------------- */
/* UCS4のコードの有効性検証 */
int sceCccIsValidUCS4(sceCccUCS4 code);

/* UCS2のコードの有効性検証 */
int sceCccIsValidUCS2(sceCccUCS4 code);

/* JISのコードの有効性検証 */
int sceCccIsValidJIS(sceCccJISCS code);

/* UTF8のコードの有効性検証 */
int sceCccIsValidUTF8(sceCccUCS4 code);

/* UTF16のコードの有効性検証 */
int sceCccIsValidUTF16(sceCccUCS4 code);

/* SJISのコードの有効性検証 */
int sceCccIsValidSJIS(sceCccJISCS code);


/* ------------------------------------------------------------------------- 
   １文字復号
   str : 取得元アドレスへのポインタ。取得後は一文字分進む。
   return : 文字コード
   ------------------------------------------------------------------------- */
/* UTF16文字列から１文字取得 */
sceCccUCS4 sceCccDecodeUTF16(sceCccUTF16 const** str);

/* UTF8文字列から１文字取得 */
sceCccUCS4 sceCccDecodeUTF8(sceCccUTF8 const** str);

/* SJIS文字列から１文字取得 */
sceCccJISCS sceCccDecodeSJIS(sceCccSJISTF const** str);


/* ------------------------------------------------------------------------- 
   １文字符号
   dst : 出力先アドレスへのポインタ。出力後は一文字分進む。
   code : 文字コード
   ------------------------------------------------------------------------- */
/* UTF16文字列として１文字出力 */
void sceCccEncodeUTF16(sceCccUTF16** dst, sceCccUCS4 code);

/* UTF8文字列として１文字出力 */
void sceCccEncodeUTF8(sceCccUTF8** dst, sceCccUCS4 code);

/* SJIS文字列として１文字出力 */
void sceCccEncodeSJIS(sceCccSJISTF** dst, sceCccJISCS code);


/* ------------------------------------------------------------------------- 
   文字コード変換
   code : 文字コード
   chr : 変換に失敗したときに出力されるキャラクタ(default:0)
   return : 変換後のコード
   ------------------------------------------------------------------------- */
/* UCSからJISへ１文字変換 */
sceCccJISCS sceCccUCStoJIS(sceCccUCS4 code, sceCccJISCS chr);

/* JISからUCSへ１文字変換 */
sceCccUCS4 sceCccJIStoUCS(sceCccJISCS code, sceCccUCS4 chr);


/* ------------------------------------------------------------------------- 
   文字列符合方式変換
   dst : 出力文字列
   count : 出力バッファサイズ
   str : 入力文字列
   return : 変換後の文字数
   ------------------------------------------------------------------------- */
/* UTF8からUTF16へ変換 */
int sceCccUTF8toUTF16(sceCccUTF16* dst, size_t count, sceCccUTF8 const* str);

/* UTF8からSJISへ変換 */
int sceCccUTF8toSJIS(sceCccSJISTF* dst, size_t count, sceCccUTF8 const* str);

/* SJISからUTF8へ変換 */
int sceCccSJIStoUTF8(sceCccUTF8* dst, size_t count, sceCccSJISTF const* str);

/* SJISからUTF16へ変換 */
int sceCccSJIStoUTF16(sceCccUTF16* dst, size_t count, sceCccSJISTF const* str);

/* UTF16からSJISへ変換 */
int sceCccUTF16toSJIS(sceCccSJISTF* dst, size_t count, sceCccUTF16 const* str);

/* UTF16からUTF8へ変換 */
int sceCccUTF16toUTF8(sceCccUTF8* dst, size_t count, sceCccUTF16 const* str);


/* ------------------------------------------------------------------------- 
   文字列変換時の無効データ置き換え用キャラクタ設定。
   chr : キャラクタ
   return : 以前に設定されていたコード
   ------------------------------------------------------------------------- */
sceCccJISCS sceCccSetErrorCharSJIS(sceCccJISCS chr);
sceCccUCS4 sceCccSetErrorCharUTF8(sceCccUCS4 chr);
sceCccUCS4 sceCccSetErrorCharUTF16(sceCccUCS4 chr);


/* ------------------------------------------------------------------------- 
   libcccライブラリのエントリの取得。
   return : libcccのエントリライブラリ
   ------------------------------------------------------------------------- */
void *sceCccGetErxEntries(void);


#if defined(__cplusplus)
}
#endif


#endif /* __libccc__ */
