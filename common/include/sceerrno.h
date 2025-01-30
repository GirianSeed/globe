/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.7
 $Id: sceerrno.h,v 1.12.2.1 2002/10/24 06:26:58 kaol Exp $
 */
/* 
 * Emotion Engine Library / I/O Processor Library
 *
 * Copyright (C) 2002 Sony Computer Entertainment Inc.
 * All Rights Reserved.
 *
 * sceerrno.h - common error number definition and utilities
 *
 *      Date            Design      Log
 *  ----------------------------------------------------
 *      2002-09-11      style       initial draft
 *      2002-09-13      style       revised
 *      2002-09-17      style       official first version
 *      2002-10-15      style       solve EE/IOP diffs
 *      2002-10-18      style       lib. prefix BNNETCNF added
 *      2002-10-22      style       `SCE_' prefixed standard errno in sceerr.h
 */

#ifndef _SCE_ERRNO_H
#define _SCE_ERRNO_H

#include <errno.h>
#ifdef R3000
#include <kerror.h>		/* IOP kernel original error no. */
#endif

/*
 * 未定義エラー番号の定義:
 *     errno.h の EE/IOP における差異を吸収
 */
#ifndef EFORMAT
#define EFORMAT		47
#endif
#ifndef EUNSUP
#define EUNSUP		48
#endif
#ifndef ENOSHARE
#define ENOSHARE	136
#endif

/*
 * `SCE_' プレフィックス付き標準エラー番号
 */
#include <sceerr.h>

/*
 * SCE 独自定義エラー番号:
 * errno.h/kerror.h 未定義エラー番号 (下位 16 ビット: SCE_EBASE..0xffff)
 *     errno.h:         0-1xx
 *     kerror.h(IOP): 100-4xx
 */

#define SCE_EBASE		0x8000		/* 32768 */

/* 対象が汎用なエラー */
#define SCE_EINIT		0x8001		/* 初期化処理が行われていない	*/
#define SCE_EID			0x8002		/* ID が存在しない		*/
#define SCE_ESEMAPHORE		0x8003		/* セマフォ処理が行えない	*/
#define SCE_ESEMA		SCE_ESEMAPHORE
#define SCE_ETHREAD		0x8004		/* スレッド処理が行えない	*/
#define SCE_ETIMER		0x8005		/* タイマー/アラーム処理が行えない */

/* ライブラリ依存なエラー */
#define SCE_EBASE_LIB		0x9000		/* 36864 */

#define SCE_EDEVICE_BROKEN	0x9001		/* デバイス破損の可能性		*/
#define SCE_EFILE_BROKEN	0x9002		/* ファイルまたはディレクトリ破損の可能性 */
#define SCE_ENEW_DEVICE		0x9003		/* 新規デバイス検出		*/
#define SCE_EMDEPTH		0x9004		/* ディレクトリが深過ぎる	*/

#define SCE_ENO_PROGRAM		0x9005		/* プログラムチャンクが無い	*/
#define SCE_ENO_SAMPLESET	0x9006		/* サンプルセットチャンクが無い	*/
#define SCE_ENO_SAMPLE		0x9007		/* サンプルチャンクが無い	*/
#define SCE_ENO_VAGINFO		0x9008		/* VAGInfoチャンクが無い	*/
#define SCE_ENO_SBADDR		0x9009		/* スプリットブロックのアドレス情報が無い */
#define SCE_EBAD_PNUM		0x9010		/* プログラムナンバーは範囲外	*/
#define SCE_ENO_PNUM		0x9011		/* プログラムナンバーは未使用	*/
#define SCE_EBAD_SSNUM		0x9012  	/* サンプルセットナンバーは範囲外 */
#define SCE_ENO_SSNUM		0x9013  	/* サンプルセットナンバーは未使用 */
#define SCE_EBAD_SPNUM		0x9014  	/* サンプルナンバーは範囲外	*/
#define SCE_ENO_SPNUM		0x9015  	/* サンプルナンバーは未使用	*/
#define SCE_EBAD_VAGNUM		0x9016  	/* VAGInfoナンバーは範囲外	*/
#define SCE_ENO_VAGNUM		0x9017  	/* VAGInfoナンバーは未使用	*/
#define SCE_EBAD_SBNUM		0x9018  	/* スプリットブロックナンバーは範囲外 */
#define SCE_EVAGINFO_NOISE	0x9019  	/* VAGInfoが指し示すVAGはノイズである */
#define SCE_ENO_SPLITNUM	0x9020  	/* スプリットナンバーは未使用	*/

/*
 * 汎用正常終了マクロ
 */
#define SCE_OK	0

/*
 * ライブラリ識別プレフィックス
 */

/* エラー識別ビット */
#define SCE_ERROR_PREFIX_ERROR		0x80000000

/* プレフィックス・シフト量 */
#define SCE_ERROR_PREFIX_SHIFT		16

/* エラー・ビットマスク */
#define SCE_ERROR_MASK_LIBRARY_PREFIX	0x7fff0000
#define SCE_ERROR_MASK_ERRNO		0x0000ffff

/* ライブラリ識別プレフィックス値 */
#define SCE_ERROR_PREFIX_MC2	  0x01010000 /* メモリーカード        libmc2  */
#define SCE_ERROR_PREFIX_BNNETCNF 0x01020000 /* PS BB Navi. Net conf. bnnetcf */
#define SCE_ERROR_PREFIX_SDHD	  0x01030000 /* Sound Data .HD        libsdhd */

/*
 * ユーティリティ
 */ 

/* for library author */
#define SCE_ERROR_ENCODE(prefix,err) (SCE_ERROR_PREFIX_ERROR | (prefix) | (err))

/* for user */
#define SCE_ERROR_ERRNO(err)      ((err) & SCE_ERROR_MASK_ERRNO)
#define SCE_ERROR_LIB_PREFIX(err) ((err) & SCE_ERROR_MASK_LIBRARY_PREFIX)

#endif /* _SCE_ERRNO_H */
