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

// fepbar.inc
#ifndef __FEPBAR_INC__
#define __FEPBAR_INC__

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
extern "C" {
#endif

#include "env.h"
#include "shareddef.h"

#define FEPBAR_UV_W			(25)
#define FEPBAR_UV_H			(25)
#define FEPBAR_SKB_UV_W		(37)
#define FEPBAR_SP_W16		(25<<4)
#define FEPBAR_SP_H16		(25<<4)
#define FEPBAR_OFFSETX16	(4<<4)							// 最前面のボタンを描画する際のFEPBARの土台からの距離
#define FEPBAR_SKB_SP_W16	(37<<4)
//============================================================================
// FEP ON時のモード表示
// 通常テクスチャ
const static uv_t		g_uvFepbarNormal[] =
{
	{STG(FEPBAR_UV_W*0), STG(FEPBAR_UV_H*1), STG(FEPBAR_UV_W), STG(FEPBAR_UV_H)},	// あ
	{STG(FEPBAR_UV_W*4), STG(FEPBAR_UV_H*1), STG(FEPBAR_UV_W), STG(FEPBAR_UV_H)},	// ｱ
	{STG(FEPBAR_UV_W*3), STG(FEPBAR_UV_H*1), STG(FEPBAR_UV_W), STG(FEPBAR_UV_H)},	// ア
	{STG(FEPBAR_UV_W*2), STG(FEPBAR_UV_H*1), STG(FEPBAR_UV_W), STG(FEPBAR_UV_H)},	// A
	{STG(FEPBAR_UV_W*1), STG(FEPBAR_UV_H*1), STG(FEPBAR_UV_W), STG(FEPBAR_UV_H)},	// Ａ

	{STG(FEPBAR_UV_W*3), STG(FEPBAR_UV_H*4), STG(FEPBAR_UV_W), STG(FEPBAR_UV_H)}	// A	FEP OFF
};
// 押下テクスチャ
const static uv_t		g_uvFepbarPress[] =
{
	{STG(FEPBAR_UV_W*0), STG(FEPBAR_UV_H*0), STG(FEPBAR_UV_W), STG(FEPBAR_UV_H)},	// あ	FEP ON
	{STG(FEPBAR_UV_W*4), STG(FEPBAR_UV_H*0), STG(FEPBAR_UV_W), STG(FEPBAR_UV_H)},	// ｱ
	{STG(FEPBAR_UV_W*3), STG(FEPBAR_UV_H*0), STG(FEPBAR_UV_W), STG(FEPBAR_UV_H)},	// ア
	{STG(FEPBAR_UV_W*2), STG(FEPBAR_UV_H*0), STG(FEPBAR_UV_W), STG(FEPBAR_UV_H)},	// A
	{STG(FEPBAR_UV_W*1), STG(FEPBAR_UV_H*0), STG(FEPBAR_UV_W), STG(FEPBAR_UV_H)},	// Ａ

	{STG(FEPBAR_UV_W*3), STG(FEPBAR_UV_H*4), STG(FEPBAR_UV_W), STG(FEPBAR_UV_H)}	// A	FEP OFF
};
// 解除テクスチャ
const static uv_t		g_uvFepbarRelease[] =
{
	{STG(FEPBAR_UV_W*0), STG(FEPBAR_UV_H*2), STG(FEPBAR_UV_W), STG(FEPBAR_UV_H)},	// あ
	{STG(FEPBAR_UV_W*4), STG(FEPBAR_UV_H*2), STG(FEPBAR_UV_W), STG(FEPBAR_UV_H)},	// ｱ
	{STG(FEPBAR_UV_W*3), STG(FEPBAR_UV_H*2), STG(FEPBAR_UV_W), STG(FEPBAR_UV_H)},	// ア
	{STG(FEPBAR_UV_W*2), STG(FEPBAR_UV_H*2), STG(FEPBAR_UV_W), STG(FEPBAR_UV_H)},	// A
	{STG(FEPBAR_UV_W*1), STG(FEPBAR_UV_H*2), STG(FEPBAR_UV_W), STG(FEPBAR_UV_H)},	// Ａ

	{STG(FEPBAR_UV_W*3), STG(FEPBAR_UV_H*4), STG(FEPBAR_UV_W), STG(FEPBAR_UV_H)}	// A	FEP OFF
};


//============================================================================
// ソフトウェアキーボード表示ボタン
// 通常
const static uv_t		g_uvFepbarSkbNormal =
{
	STG(FEPBAR_SKB_UV_W*0), STG(FEPBAR_UV_H*3), STG(FEPBAR_SKB_UV_W), STG(FEPBAR_UV_H)
};
// 押下中
const static uv_t		g_uvFepbarSkbPress =
{
	STG(FEPBAR_SKB_UV_W*0), STG(FEPBAR_UV_H*3), STG(FEPBAR_SKB_UV_W), STG(FEPBAR_UV_H)
};
// 解除
const static uv_t		g_uvFepbarSkbRelease =
{
	STG(FEPBAR_SKB_UV_W*0), STG(FEPBAR_UV_H*3), STG(FEPBAR_SKB_UV_W), STG(FEPBAR_UV_H)
};


//============================================================================
// 土台テクスチャ
// 日本版
const static uv_t		g_uvFepbarBase_J[] =
{
	{STG(FEPBAR_SKB_UV_W*0),	STG(FEPBAR_UV_H*4),	STG(5),	STG(FEPBAR_UV_H)},
	{STG(5),					STG(FEPBAR_UV_H*4),	STG(65),STG(FEPBAR_UV_H)},
	{STG(70),					STG(FEPBAR_UV_H*4),	STG(5),	STG(FEPBAR_UV_H)},
};
// 海外版
const static uv_t		g_uvFepbarBase_F[] =
{
	{STG(FEPBAR_SKB_UV_W*0),	STG(FEPBAR_UV_H*4),	STG(5),		STG(FEPBAR_UV_H)},
	{STG(5),					STG(FEPBAR_UV_H*4),	STG(37),	STG(FEPBAR_UV_H)},
	{STG(70),					STG(FEPBAR_UV_H*4),	STG(5),		STG(FEPBAR_UV_H)},
};


//============================================================================
// 日本版 モード表示
const static sprite_t g_spInputMode16 =
{
	FEPBAR_OFFSETX16+(FEPBAR_SP_W16*0), FEPBAR_SP_H16*0, 0, FEPBAR_SP_W16, FEPBAR_SP_H16
};


//============================================================================
// 土台スプライト
// 日本版
const static sprite_t g_spFepbarBase_J16[] =
{
	{FEPBAR_SP_W16*0,	FEPBAR_SP_H16*0,	0,	5<<4,	FEPBAR_SP_H16},
	{5<<4,				FEPBAR_SP_H16*0,	0,	65<<4,	FEPBAR_SP_H16},
	{70<<4,				FEPBAR_SP_H16*0,	0,	5<<4,	FEPBAR_SP_H16},
};
// 海外版
const static sprite_t g_spFepbarBase_F16[] =
{
	{FEPBAR_SP_W16*0,	FEPBAR_SP_H16*0,	0,	5<<4,	FEPBAR_SP_H16},
	{5<<4,				FEPBAR_SP_H16*0,	0,	37<<4,	FEPBAR_SP_H16},
	{(37+5)<<4,			FEPBAR_SP_H16*0,	0,	5<<4,	FEPBAR_SP_H16},
};


//============================================================================
// ソフトウェアキーボード表示用スプライト
// 日本版
const static sprite_t g_spFepbarSkbVisible_J16 =
{
	(FEPBAR_OFFSETX16*2)+(FEPBAR_SP_W16*1), FEPBAR_SP_H16*0, 0, FEPBAR_SKB_SP_W16, FEPBAR_SP_H16
};
// 海外版
const static sprite_t g_spFepbarSkbVisible_F16 =
{
	(FEPBAR_OFFSETX16+((1)<<4)), FEPBAR_SP_H16*0, 0, FEPBAR_SKB_SP_W16, FEPBAR_SP_H16
};













#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
}
#endif
#endif	// !__FEPBAR_H__
