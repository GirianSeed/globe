/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5.5
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

// modebar.inc
#ifndef __MODEBAR_INC__
#define __MODEBAR_INC__

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
extern "C" {
#endif

#include "shareddef.h"
#include "sharedskb.h"


#define MODEBAR_BASE_UV_W		(26)		// 土台のUV値
#define MODEBAR_BASE_UV_H		(26)
#define MODEBAR_CHAR_UV_W		(16)		// 文字のUV値
#define MODEBAR_CHAR_UV_H		(16)

#define MODEBAR_BASE_SP_W		(26)		// 土台のスプライト値
#define MODEBAR_BASE_SP_H		(26)
#define MODEBAR_BASE_SP_W16		(26<<4)		// 土台のスプライト値(16倍)
#define MODEBAR_BASE_SP_H16		(26<<4)
#define MODEBAR_CHAR_SP_W		(16)		// 文字のスプライト値
#define MODEBAR_CHAR_SP_H		(16)
#define MODEBAR_CHAR_SP_W16		(16<<4)		// 文字のスプライト値(16倍)
#define MODEBAR_CHAR_SP_H16		(16<<4)

#define MODEBAR_CHAR_SP_UP		(-2)		// テクスチャの文字は、ベースラインが異なるので調整
#define MODEBAR_CHAR_SP_DOWN	(2)


// ソフトウェアキーボード描画時の右下のモード データ定義
#define MODEBAR_LEFT	(0)		// 左
#define MODEBAR_CENTER	(1)		// 間
#define MODEBAR_RIGHT	(2)		// 右
#define MODEBAR_1		(3)		// モードが1パターン

// 土台UV値 設定
static const uv_t g_uvModebarBase[] =
{
	{STG(MODEBAR_BASE_UV_W*0),	STG(MODEBAR_BASE_UV_H*4),	STG(MODEBAR_BASE_UV_W),	STG(MODEBAR_BASE_UV_H)},		// 左
	{STG(MODEBAR_BASE_UV_W*0),	STG(MODEBAR_BASE_UV_H*5),	STG(MODEBAR_BASE_UV_W),	STG(MODEBAR_BASE_UV_H)},		// 間
	{STG(MODEBAR_BASE_UV_W*0),	STG(MODEBAR_BASE_UV_H*8),	STG(MODEBAR_BASE_UV_W),	STG(MODEBAR_BASE_UV_H)},		// 右
};

// 入力モードカーソル
static const uv_t g_uvModebarCursor[] =
{
	{STG(MODEBAR_BASE_UV_W*1),	STG(MODEBAR_BASE_UV_H*4),	STG(MODEBAR_BASE_UV_W),	STG(MODEBAR_BASE_UV_H)},		// 左
	{STG(MODEBAR_BASE_UV_W*1),	STG(MODEBAR_BASE_UV_H*5),	STG(MODEBAR_BASE_UV_W),	STG(MODEBAR_BASE_UV_H)},		// 間
	{STG(MODEBAR_BASE_UV_W*1),	STG(MODEBAR_BASE_UV_H*8),	STG(MODEBAR_BASE_UV_W),	STG(MODEBAR_BASE_UV_H)},		// 右
	{STG(MODEBAR_BASE_UV_W*1),	STG(MODEBAR_BASE_UV_H*7),	STG(MODEBAR_BASE_UV_W),	STG(MODEBAR_BASE_UV_H)},		// 一個
};

// 文字
static const uv_t g_uvModebarChar[] =
{
	// 日本版
	{STG(MODEBAR_CHAR_UV_W*13),	STG(MODEBAR_CHAR_UV_H*31),	STG(MODEBAR_CHAR_UV_W),		STG(MODEBAR_CHAR_UV_H)},		// あ
	{STG(MODEBAR_CHAR_UV_W*15),	STG(MODEBAR_CHAR_UV_H*21),	STG(MODEBAR_CHAR_UV_W),		STG(MODEBAR_CHAR_UV_H)},		// ア
	{STG(MODEBAR_CHAR_UV_W*0),	STG(MODEBAR_CHAR_UV_H*2),	STG(MODEBAR_CHAR_UV_W),		STG(MODEBAR_CHAR_UV_H)},		// A
	{STG(MODEBAR_CHAR_UV_W*13),	STG(MODEBAR_CHAR_UV_H*57),	STG(MODEBAR_CHAR_UV_W),		STG(MODEBAR_CHAR_UV_H)},		// 記号
	// 海外版
	{STG(MODEBAR_CHAR_UV_W*0),		STG(MODEBAR_CHAR_UV_H*2),	STG(MODEBAR_CHAR_UV_W),		STG(MODEBAR_CHAR_UV_H)},	// A
	{STG((MODEBAR_CHAR_UV_W*15)),	STG(MODEBAR_CHAR_UV_H*56),	STG(MODEBAR_CHAR_UV_W),		STG(MODEBAR_CHAR_UV_H)},	// 発音記号付き特殊文字
	{STG((MODEBAR_CHAR_UV_W*14)),	STG(MODEBAR_CHAR_UV_H*57),	STG(MODEBAR_CHAR_UV_W),		STG(MODEBAR_CHAR_UV_H)},	// 特殊記号
	// 共通
	{STG((MODEBAR_CHAR_UV_W*4)),	STG(MODEBAR_CHAR_UV_H*59),	STG(MODEBAR_CHAR_UV_W),		STG(MODEBAR_CHAR_UV_H)},	// 123
};


#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
}
#endif
#endif	// !__MODEBAR_H__
