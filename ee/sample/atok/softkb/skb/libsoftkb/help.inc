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

#ifndef __FEPSKB_INC__
#define __FEPSKB_INC__

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
extern "C" {
#endif

#include "shareddef.h"

enum
{
	HELP_JAPANESE,
	HELP_ENGLISH,
	HELP_FRENCH,
	HELP_SPANISH,
	HELP_GERMAN,
	HELP_DUTCH,
	HELP_ITALIAN,
	HELP_PORTUGUESE,

	HELP_J_ENGLISH,
	HELP_MAX,
};
//============================================================================
// UV DEFINE
//============================================================================
//============================================================================
// HELP 基本操作画面(文言)
#define HELP_TITLE_UV_X			STG(208)
#define HELP_TITLE_UV_Y			(0)
#define HELP_TITLE_UV_W			STG(208)
#define HELP_TITLE_UV_H			STG(24)


uv_t	g_uvTitleSrc[] =
{
	{HELP_TITLE_UV_X+(HELP_TITLE_UV_W*0),	HELP_TITLE_UV_Y+(HELP_TITLE_UV_H*4),		HELP_TITLE_UV_W,	HELP_TITLE_UV_H},	// 日本語
	{HELP_TITLE_UV_X+(HELP_TITLE_UV_W*0),	HELP_TITLE_UV_Y+(HELP_TITLE_UV_H*0),		HELP_TITLE_UV_W,	HELP_TITLE_UV_H},	// 英語
	{HELP_TITLE_UV_X+(HELP_TITLE_UV_W*0),	HELP_TITLE_UV_Y+(HELP_TITLE_UV_H*1),		HELP_TITLE_UV_W,	HELP_TITLE_UV_H},	// フランス語
	{HELP_TITLE_UV_X+(HELP_TITLE_UV_W*1),	HELP_TITLE_UV_Y+(HELP_TITLE_UV_H*2),		HELP_TITLE_UV_W,	HELP_TITLE_UV_H},	// スペイン語
	{HELP_TITLE_UV_X+(HELP_TITLE_UV_W*1),	HELP_TITLE_UV_Y+(HELP_TITLE_UV_H*1),		HELP_TITLE_UV_W,	HELP_TITLE_UV_H},	// ドイツ語
	{HELP_TITLE_UV_X+(HELP_TITLE_UV_W*1),	HELP_TITLE_UV_Y+(HELP_TITLE_UV_H*0),		HELP_TITLE_UV_W,	HELP_TITLE_UV_H},	// オランダ語
	{HELP_TITLE_UV_X+(HELP_TITLE_UV_W*0),	HELP_TITLE_UV_Y+(HELP_TITLE_UV_H*3),		STG(470-208),		HELP_TITLE_UV_H},	// イタリア語
	{HELP_TITLE_UV_X+(HELP_TITLE_UV_W*0),	HELP_TITLE_UV_Y+(HELP_TITLE_UV_H*2),		HELP_TITLE_UV_W,	HELP_TITLE_UV_H},	// ポルトガル語

	{HELP_TITLE_UV_X+(HELP_TITLE_UV_W*0),	HELP_TITLE_UV_Y+(HELP_TITLE_UV_H*0),		HELP_TITLE_UV_W,	HELP_TITLE_UV_H},	// 日本リージョン 英語
};





//============================================================================
// PADアイテム / ボタン / コントローラ本体
#define BTNITEM_UV_X1		STG(256)	// ボタンテクスチャX座標 1
#define BTNITEM_UV_X2		STG(352)	// ボタンテクスチャX座標 2
#define BTNITEM_UV_Y		STG(128)	// ボタンテクスチャY座標
#define BTNITEM_UV_W		STG(88)		// ボタンテクスチャ幅
#define BTNITEM_UV_H		STG(21)		// ボタンテクスチャ高さ

#define PADBODY_UV_X		(0)			// コントローラ本体テクスチャX座標
#define PADBODY_UV_Y		(0)			// コントローラ本体テクスチャY座標
#define PADBODY_UV_W		STG(204)	// コントローラ本体テクスチャW座標
#define PADBODY_UV_H		STG(230)	// コントローラ本体テクスチャH座標

// コントローラUV
uv_t	g_uvPadItemSrc[] =
{
	{PADBODY_UV_X,						PADBODY_UV_Y,						PADBODY_UV_W,		PADBODY_UV_H},	// コントローラ
	{BTNITEM_UV_X1+(BTNITEM_UV_W*0),	BTNITEM_UV_Y+(BTNITEM_UV_H*4),		BTNITEM_UV_W,		BTNITEM_UV_H},	// L2/R2ボタン
	{BTNITEM_UV_X1+(BTNITEM_UV_W*0),	BTNITEM_UV_Y+(BTNITEM_UV_H*2),		BTNITEM_UV_W,		BTNITEM_UV_H},	// 方向キー
	{BTNITEM_UV_X1+(BTNITEM_UV_W*0),	BTNITEM_UV_Y+(BTNITEM_UV_H*0),		BTNITEM_UV_W,		BTNITEM_UV_H},	// セレクトボタン
	{BTNITEM_UV_X1+(BTNITEM_UV_W*0),	BTNITEM_UV_Y+(BTNITEM_UV_H*1),		BTNITEM_UV_W,		BTNITEM_UV_H},	// スタートボタン
	{BTNITEM_UV_X1+(BTNITEM_UV_W*0),	BTNITEM_UV_Y+(BTNITEM_UV_H*3),		BTNITEM_UV_W,		BTNITEM_UV_H},	// R1ボタン
	{BTNITEM_UV_X2+(BTNITEM_UV_W*0),	BTNITEM_UV_Y+(BTNITEM_UV_H*1),		BTNITEM_UV_W,		BTNITEM_UV_H},	// △
	{BTNITEM_UV_X2+(BTNITEM_UV_W*0),	BTNITEM_UV_Y+(BTNITEM_UV_H*2),		BTNITEM_UV_W,		BTNITEM_UV_H},	// ○
	{BTNITEM_UV_X2+(BTNITEM_UV_W*0),	BTNITEM_UV_Y+(BTNITEM_UV_H*3),		BTNITEM_UV_W,		BTNITEM_UV_H},	// ×
	{BTNITEM_UV_X2+(BTNITEM_UV_W*0),	BTNITEM_UV_Y+(BTNITEM_UV_H*0),		BTNITEM_UV_W,		BTNITEM_UV_H},	// □

};


//============================================================================
// 文言
#define CHARS_UV_X		(0)
#define CHARS_UV_Y		(16)
#define CHARS_UV_W		STG(256)
#define CHARS_UV_H		STG(16)


uv_t	g_uvCharsSrc[] =
{
	{CHARS_UV_X, STG(CHARS_UV_Y*8), CHARS_UV_W, CHARS_UV_H},	// 挿入位置移動
	{CHARS_UV_X, STG(CHARS_UV_Y*0), CHARS_UV_W, CHARS_UV_H},	// キー、項目の選択
	{CHARS_UV_X, STG(CHARS_UV_Y*6), CHARS_UV_W, CHARS_UV_H},	// キートップ切替
	{CHARS_UV_X, STG(CHARS_UV_Y*5), CHARS_UV_W, CHARS_UV_H},	// 確定
	{CHARS_UV_X, STG(CHARS_UV_Y*7), CHARS_UV_W, CHARS_UV_H},	// 大/小文字切替
	{CHARS_UV_X, STG(CHARS_UV_Y*3), CHARS_UV_W, CHARS_UV_H},	// 空白
	{CHARS_UV_X, STG(CHARS_UV_Y*1), CHARS_UV_W, CHARS_UV_H},	// キー、項目の決定
	{CHARS_UV_X, STG(CHARS_UV_Y*2), CHARS_UV_W, CHARS_UV_H},	// キャンセル
	{CHARS_UV_X, STG(CHARS_UV_Y*4), CHARS_UV_W, CHARS_UV_H},	// 削除
};

uv_t	g_uvCharsSrc_JE[] =
{
	{CHARS_UV_X, STG(CHARS_UV_Y*8), CHARS_UV_W, CHARS_UV_H},	// 挿入位置移動
	{CHARS_UV_X, STG(CHARS_UV_Y*0), CHARS_UV_W, CHARS_UV_H},	// キー、項目の選択
	{CHARS_UV_X, STG(CHARS_UV_Y*6), CHARS_UV_W, CHARS_UV_H},	// キートップ切替
	{CHARS_UV_X, STG(CHARS_UV_Y*5), CHARS_UV_W, CHARS_UV_H},	// 確定
	{CHARS_UV_X, STG(CHARS_UV_Y*7), CHARS_UV_W, CHARS_UV_H},	// 大/小文字切替
	{CHARS_UV_X, STG(CHARS_UV_Y*9), CHARS_UV_W, CHARS_UV_H},	// 変換/空白
	{CHARS_UV_X, STG(CHARS_UV_Y*1), CHARS_UV_W, CHARS_UV_H},	// キー、項目の決定
	{CHARS_UV_X, STG(CHARS_UV_Y*10),CHARS_UV_W, CHARS_UV_H},	// キャンセル
	{CHARS_UV_X, STG(CHARS_UV_Y*4), CHARS_UV_W, CHARS_UV_H},	// 削除
};



//============================================================================
// 文言を囲うフレーム
#define FRAME_UV_X		STG(208)
#define FRAME_UV_Y		STG(144)
#define FRAME_UV_W		STG(16)
#define FRAME_UV_H		STG(16)

#define FRAME_SP_W		(16)
#define FRAME_SP_H		(16)

uv_t	g_uvFrameSrc[] =
{
	{FRAME_UV_X+(FRAME_UV_W*0),	FRAME_UV_Y+(FRAME_UV_H*0),	FRAME_UV_W,	FRAME_UV_H},	// L2/R2
	{FRAME_UV_X+(FRAME_UV_W*0),	FRAME_UV_Y+(FRAME_UV_H*1),	FRAME_UV_W,	FRAME_UV_H},	// 方向キー
	{FRAME_UV_X+(FRAME_UV_W*0),	FRAME_UV_Y+(FRAME_UV_H*2),	FRAME_UV_W,	FRAME_UV_H},	// SELECT
	{FRAME_UV_X+(FRAME_UV_W*1),	FRAME_UV_Y+(FRAME_UV_H*0),	FRAME_UV_W,	FRAME_UV_H},	// START
	{FRAME_UV_X+(FRAME_UV_W*1),	FRAME_UV_Y+(FRAME_UV_H*1),	FRAME_UV_W,	FRAME_UV_H},	// R1
	{FRAME_UV_X+(FRAME_UV_W*1),	FRAME_UV_Y+(FRAME_UV_H*2),	FRAME_UV_W,	FRAME_UV_H},	// △
	{FRAME_UV_X+(FRAME_UV_W*2),	FRAME_UV_Y+(FRAME_UV_H*0),	FRAME_UV_W,	FRAME_UV_H},	// ○
	{FRAME_UV_X+(FRAME_UV_W*2),	FRAME_UV_Y+(FRAME_UV_H*1),	FRAME_UV_W,	FRAME_UV_H},	// ×
	{FRAME_UV_X+(FRAME_UV_W*2),	FRAME_UV_Y+(FRAME_UV_H*2),	FRAME_UV_W,	FRAME_UV_H},	// □
};


//============================================================================
// ATOK バージョン

uv_t	g_uvAtokVerSrc[] =
{
	{0,		STG(270),	STG(425),	STG(24)},	// 日本語
	{0,		STG(240),	STG(500),	STG(24)},	// 英語
};

uv_t	g_uvAtokLogoSrc =
{
	STG(516),	STG(196),	STG(40),	STG(40),
};



//============================================================================
// SPRITE DEFINE
//============================================================================
// NTSC
//============================================================================
// タイトル
#define TITLE_SP_W		(208)
#define TITLE_SP_H		(24)


sprite_t	g_spTitleSrc[] =
{
	{271,	46,		0,	TITLE_SP_W, TITLE_SP_H},	// 日本リージョン 日本語
	{229,	60,		0,	TITLE_SP_W, TITLE_SP_H},	// 英語
	{218,	62,		0,	TITLE_SP_W, TITLE_SP_H},	// フランス語
	{220,	62,		0,	TITLE_SP_W, TITLE_SP_H},	// スペイン語
	{230,	62,		0,	TITLE_SP_W, TITLE_SP_H},	// ドイツ語
	{227,	60,		0,	TITLE_SP_W, TITLE_SP_H},	// イタリア語
	{196,	62,		0,	470-208,    TITLE_SP_H},	// オランダ語
	{227,	60,		0,	TITLE_SP_W, TITLE_SP_H},	// ポルトガル語

	{232,	42,		0,	TITLE_SP_W, TITLE_SP_H},	// 日本リージョン 英語
};


//============================================================================
// PADアイテム配置情報
#define PADBODY_SP_W		(200)
#define PADBODY_SP_H		(232)

#define PADITEM_SP_X		(0)
#define PADITEM_SP_Y		(408)
#define PADITEM_SP_W		(88)
#define PADITEM_SP_H		(21)

sprite_t	g_spPadItemSrc_NJ[] =	// 日本リージョン 日本語
{
	{233,		86,		0,		PADBODY_SP_W,		PADBODY_SP_H},	// コントローラ

	{176,		82,		0,		PADITEM_SP_W,		PADITEM_SP_H},	// L2/R2
	{154,		136,	0,		PADITEM_SP_W,		PADITEM_SP_H},	// 方向キー
	{202,		242,	0,		PADITEM_SP_W,		PADITEM_SP_H},	// SELECT
	{232,		298,	0,		PADITEM_SP_W,		PADITEM_SP_H},	// START
	{420,		76,		0,		PADITEM_SP_W,		PADITEM_SP_H},	// R1
	{437,		124,	0,		PADITEM_SP_W,		PADITEM_SP_H},	// △
	{438,		174,	0,		PADITEM_SP_W,		PADITEM_SP_H},	// ○
	{435,		224,	0,		PADITEM_SP_W,		PADITEM_SP_H},	// ×
	{395,		276,	0,		PADITEM_SP_W,		PADITEM_SP_H},	// □
};

sprite_t	g_spPadItemSrc_NJE[] =	// 日本リージョン 英語
{
	{233,		86,		0,		PADBODY_SP_W,		PADBODY_SP_H},	// コントローラ

	{176,		82,		0,		PADITEM_SP_W,		PADITEM_SP_H},	// L2/R2
	{154,		136,	0,		PADITEM_SP_W,		PADITEM_SP_H},	// 方向キー
	{202,		242,	0,		PADITEM_SP_W,		PADITEM_SP_H},	// SELECT
	{232,		298,	0,		PADITEM_SP_W,		PADITEM_SP_H},	// START
	{420,		76,		0,		PADITEM_SP_W,		PADITEM_SP_H},	// R1
	{437,		124,	0,		PADITEM_SP_W,		PADITEM_SP_H},	// △
	{438,		174,	0,		PADITEM_SP_W,		PADITEM_SP_H},	// ○
	{435,		224,	0,		PADITEM_SP_W,		PADITEM_SP_H},	// ×
	{395,		276,	0,		PADITEM_SP_W,		PADITEM_SP_H},	// □
};


sprite_t	g_spPadItemSrc_NE[] =	// 英語
{
	{232,		108,	0,		PADBODY_SP_W,		PADBODY_SP_H},	// コントローラ

	{175,		104,	0,		PADITEM_SP_W,		PADITEM_SP_H},	// L2/R2
	{153,		160,	0,		PADITEM_SP_W,		PADITEM_SP_H},	// 方向キー
	{201,		262,	0,		PADITEM_SP_W,		PADITEM_SP_H},	// SELECT
	{231,		320,	0,		PADITEM_SP_W,		PADITEM_SP_H},	// START
	{418,		96,		0,		PADITEM_SP_W,		PADITEM_SP_H},	// R1
	{436,		144,	0,		PADITEM_SP_W,		PADITEM_SP_H},	// △
	{439,		194,	0,		PADITEM_SP_W,		PADITEM_SP_H},	// ○
	{434,		244,	0,		PADITEM_SP_W,		PADITEM_SP_H},	// ×
	{393,		294,	0,		PADITEM_SP_W,		PADITEM_SP_H},	// □
};


sprite_t	g_spPadItemSrc_NF[] =	// フランス語
{
	{232,		108,	0,		PADBODY_SP_W,		PADBODY_SP_H},	// コントローラ

	{175,		106,	0,		PADITEM_SP_W,		PADITEM_SP_H},	// L2/R2
	{153,		160,	0,		PADITEM_SP_W,		PADITEM_SP_H},	// 方向キー
	{201,		264,	0,		PADITEM_SP_W,		PADITEM_SP_H},	// SELECT
	{231,		320,	0,		PADITEM_SP_W,		PADITEM_SP_H},	// START
	{418,		96,		0,		PADITEM_SP_W,		PADITEM_SP_H},	// R1
	{434,		146,	0,		PADITEM_SP_W,		PADITEM_SP_H},	// △
	{438,		196,	0,		PADITEM_SP_W,		PADITEM_SP_H},	// ○
	{434,		246,	0,		PADITEM_SP_W,		PADITEM_SP_H},	// ×
	{393,		296,	0,		PADITEM_SP_W,		PADITEM_SP_H},	// □
};


sprite_t	g_spPadItemSrc_NS[] =	// スペイン語
{
	{232,		108,	0,		PADBODY_SP_W,		PADBODY_SP_H},	// コントローラ

	{175,		106,	0,		PADITEM_SP_W,		PADITEM_SP_H},	// L2/R2
	{153,		162,	0,		PADITEM_SP_W,		PADITEM_SP_H},	// 方向キー
	{200,		264,	0,		PADITEM_SP_W,		PADITEM_SP_H},	// SELECT
	{230,		322,	0,		PADITEM_SP_W,		PADITEM_SP_H},	// START
	{419,		96,		0,		PADITEM_SP_W,		PADITEM_SP_H},	// R1
	{436,		146,	0,		PADITEM_SP_W,		PADITEM_SP_H},	// △
	{440,		196,	0,		PADITEM_SP_W,		PADITEM_SP_H},	// ○
	{435,		246,	0,		PADITEM_SP_W,		PADITEM_SP_H},	// ×
	{395,		296,	0,		PADITEM_SP_W,		PADITEM_SP_H},	// □
                                                                        

};


sprite_t	g_spPadItemSrc_NG[] =	// ドイツ語
{
	{232,		54*2,	0,		PADBODY_SP_W,		PADBODY_SP_H},	// コントローラ

	{174,		53*2,	0,		PADITEM_SP_W,		PADITEM_SP_H},	// L2/R2
	{152,		81*2,	0,		PADITEM_SP_W,		PADITEM_SP_H},	// 方向キー
	{200,		131*2,	0,		PADITEM_SP_W,		PADITEM_SP_H},	// SELECT
	{230,		161*2,	0,		PADITEM_SP_W,		PADITEM_SP_H},	// START
	{417,		49*2,	0,		PADITEM_SP_W,		PADITEM_SP_H},	// R1
	{435,		73*2,	0,		PADITEM_SP_W,		PADITEM_SP_H},	// △
	{437,		98*2,	0,		PADITEM_SP_W,		PADITEM_SP_H},	// ○
	{433,		123*2,	0,		PADITEM_SP_W,		PADITEM_SP_H},	// ×
	{392,		148*2,	0,		PADITEM_SP_W,		PADITEM_SP_H},	// □
};


sprite_t	g_spPadItemSrc_NI[] =	// イタリア語
{
	{232,		54*2,	0,		PADBODY_SP_W,		PADBODY_SP_H},	// コントローラ

	{175,		53*2,	0,		PADITEM_SP_W,		PADITEM_SP_H},	// L2/R2
	{152,		81*2,	0,		PADITEM_SP_W,		PADITEM_SP_H},	// 方向キー
	{202,		133*2,	0,		PADITEM_SP_W,		PADITEM_SP_H},	// SELECT
	{231,		161*2,	0,		PADITEM_SP_W,		PADITEM_SP_H},	// START
	{419,		48*2,	0,		PADITEM_SP_W,		PADITEM_SP_H},	// R1
	{436,		73*2,	0,		PADITEM_SP_W,		PADITEM_SP_H},	// △
	{438,		98*2,	0,		PADITEM_SP_W,		PADITEM_SP_H},	// ○
	{435,		123*2,	0,		PADITEM_SP_W,		PADITEM_SP_H},	// ×
	{395,		148*2,	0,		PADITEM_SP_W,		PADITEM_SP_H},	// □
};


sprite_t	g_spPadItemSrc_ND[] =	// オランダ語
{
	{232,		54*2,	0,		PADBODY_SP_W,		PADBODY_SP_H},	// コントローラ

	{175,		53*2,	0,		PADITEM_SP_W,		PADITEM_SP_H},	// L2/R2
	{153,		81*2,	0,		PADITEM_SP_W,		PADITEM_SP_H},	// 方向キー
	{201,		132*2,	0,		PADITEM_SP_W,		PADITEM_SP_H},	// SELECT
	{230,		161*2,	0,		PADITEM_SP_W,		PADITEM_SP_H},	// START
	{418,		48*2,	0,		PADITEM_SP_W,		PADITEM_SP_H},	// R1
	{435,		73*2,	0,		PADITEM_SP_W,		PADITEM_SP_H},	// △
	{439,		98*2,	0,		PADITEM_SP_W,		PADITEM_SP_H},	// ○
	{434,		123*2,	0,		PADITEM_SP_W,		PADITEM_SP_H},	// ×
	{393,		148*2,	0,		PADITEM_SP_W,		PADITEM_SP_H},	// □
};


sprite_t	g_spPadItemSrc_NP[] =	// ポルトガル語
{
	{232,		110,	0,		PADBODY_SP_W,		PADBODY_SP_H},	// コントローラ

	{174,		106,	0,		PADITEM_SP_W,		PADITEM_SP_H},	// L2/R2
	{152,		160,	0,		PADITEM_SP_W,		PADITEM_SP_H},	// 方向キー
	{200,		266,	0,		PADITEM_SP_W,		PADITEM_SP_H},	// SELECT
	{230,		318,	0,		PADITEM_SP_W,		PADITEM_SP_H},	// START
	{417,		96,		0,		PADITEM_SP_W,		PADITEM_SP_H},	// R1
	{435,		144,	0,		PADITEM_SP_W,		PADITEM_SP_H},	// △
	{437,		194,	0,		PADITEM_SP_W,		PADITEM_SP_H},	// ○
	{433,		246,	0,		PADITEM_SP_W,		PADITEM_SP_H},	// ×
	{392,		294,	0,		PADITEM_SP_W,		PADITEM_SP_H},	// □
};


//============================================================================
// 文言配置情報
#define CHARS_SP_W		(256)
#define CHARS_SP_H		(16)

sprite_t	g_spCharsSrc_NJ[] =		// 日本リージョン 日本語
{
	{164,	96,		0, CHARS_SP_W, CHARS_SP_H},	// 挿入位置移動
	{112,	152,	0, CHARS_SP_W, CHARS_SP_H},	// キー、項目の選択
	{147,	260,	0, CHARS_SP_W, CHARS_SP_H},	// キートップ切替
	{281,	314,	0, CHARS_SP_W, CHARS_SP_H},	// 確定
	{420,	90,		0, CHARS_SP_W, CHARS_SP_H},	// 大/小文字切替
	{438,	140,	0, CHARS_SP_W, CHARS_SP_H},	// 空白
	{440,	192,	0, CHARS_SP_W, CHARS_SP_H},	// キー、項目の決定
	{435,	240,	0, CHARS_SP_W, CHARS_SP_H},	// キャンセル
	{395,	294,	0, CHARS_SP_W, CHARS_SP_H},	// 削除
};


sprite_t	g_spCharsSrc_NJE[] =	// 日本リージョン 英語
{
	{163,	96,  0, CHARS_SP_W, CHARS_SP_H},	// 挿入位置移動
	{119,	152, 0, CHARS_SP_W, CHARS_SP_H},	// キー、項目の選択
	{156,	260, 0, CHARS_SP_W, CHARS_SP_H},	// キートップ切替
	{258,	314, 0, CHARS_SP_W, CHARS_SP_H},	// キー、項目の決定
	{420,	90,  0, CHARS_SP_W, CHARS_SP_H},	// 大/小文字切替
	{438,	140, 0, CHARS_SP_W, CHARS_SP_H},	// 空白
	{440,	192, 0, CHARS_SP_W, CHARS_SP_H},	// 確定
	{436,	240, 0, CHARS_SP_W, CHARS_SP_H},	// キャンセル
	{395,	294, 0, CHARS_SP_W, CHARS_SP_H},	// 削除
};


sprite_t	g_spCharsSrc_NE[] =		// 英語
{
	{160,	120, 0, CHARS_SP_W, CHARS_SP_H},	// 挿入位置移動
	{117,	176, 0, CHARS_SP_W, CHARS_SP_H},	// キー、項目の選択
	{155,	280, 0, CHARS_SP_W, CHARS_SP_H},	// キートップ切替
	{435,	262, 0, CHARS_SP_W, CHARS_SP_H},	// キー、項目の決定
	{419,	112, 0, CHARS_SP_W, CHARS_SP_H},	// 大/小文字切替
	{437,	162, 0, CHARS_SP_W, CHARS_SP_H},	// 空白
	{257,	336, 0, CHARS_SP_W, CHARS_SP_H},	// 確定
	{439,	212, 0, CHARS_SP_W, CHARS_SP_H},	// キャンセル
	{395,	314, 0, CHARS_SP_W, CHARS_SP_H},	// 削除
};


sprite_t	g_spCharsSrc_NF[] =		// フランス語
{
	{127,	60*2,	0, CHARS_SP_W, CHARS_SP_H},	// 挿入位置移動
	{30, 	88*2,	0, CHARS_SP_W, CHARS_SP_H},	// キー、項目の選択
	{163,	141*2,	0, CHARS_SP_W, CHARS_SP_H},	// キートップ切替
	{435,	132*2,	0, CHARS_SP_W, CHARS_SP_H},	// キー、項目の決定
	{419,	56*2,	0, CHARS_SP_W, CHARS_SP_H},	// 大/小文字切替
	{435,	82*2,	0, CHARS_SP_W, CHARS_SP_H},	// 空白
	{244,	169*2,	0, CHARS_SP_W, CHARS_SP_H},	// 確定
	{440,	107*2,	0, CHARS_SP_W, CHARS_SP_H},	// キャンセル
	{395,	157*2,	0, CHARS_SP_W, CHARS_SP_H},	// 削除
};


sprite_t	g_spCharsSrc_NS[] =		// スペイン語
{
	{157,	60*2,	0, CHARS_SP_W, CHARS_SP_H},	// 挿入位置移動
	{30, 	89*2,	0, CHARS_SP_W, CHARS_SP_H},	// キー、項目の選択
	{160,	141*2,	0, CHARS_SP_W, CHARS_SP_H},	// キートップ切替
	{436,	132*2,	0, CHARS_SP_W, CHARS_SP_H},	// キー、項目の決定
	{419,	56*2,	0, CHARS_SP_W, CHARS_SP_H},	// 大/小文字切替
	{436,	82*2,	0, CHARS_SP_W, CHARS_SP_H},	// 空白
	{252,	169*2,	0, CHARS_SP_W, CHARS_SP_H},	// 確定
	{441,	107*2,	0, CHARS_SP_W, CHARS_SP_H},	// キャンセル
	{395,	157*2,	0, CHARS_SP_W, CHARS_SP_H},	// 削除
};


sprite_t	g_spCharsSrc_NG[] =		// ドイツ語
{
	{108,	59*2,	0, CHARS_SP_W, CHARS_SP_H},	// 挿入位置移動
	{53, 	89*2,	0, CHARS_SP_W, CHARS_SP_H},	// キー、項目の選択
	{64, 	140*2,	0, CHARS_SP_W, CHARS_SP_H},	// キートップ切替
	{434,	132*2,	0, CHARS_SP_W, CHARS_SP_H},	// キー、項目の決定
	{417,	57*2,	0, CHARS_SP_W, CHARS_SP_H},	// 大/小文字切替
	{435,	82*2,	0, CHARS_SP_W, CHARS_SP_H},	// 空白
	{228,	168*2,	0, CHARS_SP_W, CHARS_SP_H},	// 確定
	{438,	107*2,	0, CHARS_SP_W, CHARS_SP_H},	// キャンセル
	{393,	157*2,	0, CHARS_SP_W, CHARS_SP_H},	// 削除
};


sprite_t	g_spCharsSrc_NI[] =		// イタリア語
{
	{147,	60*2,	0, CHARS_SP_W, CHARS_SP_H},	// 挿入位置移動
	{77, 	88*2,	0, CHARS_SP_W, CHARS_SP_H},	// キー、項目の選択
	{137,	142*2,	0, CHARS_SP_W, CHARS_SP_H},	// キートップ切替
	{436,	132*2,	0, CHARS_SP_W, CHARS_SP_H},	// キー、項目の決定
	{419,	56*2,	0, CHARS_SP_W, CHARS_SP_H},	// 大/小文字切替
	{438,	82*2,	0, CHARS_SP_W, CHARS_SP_H},	// 空白
	{239,	169*2,	0, CHARS_SP_W, CHARS_SP_H},	// 確定
	{439,	107*2,	0, CHARS_SP_W, CHARS_SP_H},	// キャンセル
	{396,	157*2,	0, CHARS_SP_W, CHARS_SP_H},	// 削除
};


sprite_t	g_spCharsSrc_ND[] =		// オランダ語
{
	{113,	60*2,	0, CHARS_SP_W, CHARS_SP_H},	// 挿入位置移動
	{74, 	88*2,	0, CHARS_SP_W, CHARS_SP_H},	// キー、項目の選択
	{122,	140*2,	0, CHARS_SP_W, CHARS_SP_H},	// キートップ切替
	{435,	132*2,	0, CHARS_SP_W, CHARS_SP_H},	// キー、項目の決定
	{418,	56*2,	0, CHARS_SP_W, CHARS_SP_H},	// 大/小文字切替
	{436,	82*2,	0, CHARS_SP_W, CHARS_SP_H},	// 空白
	{221,	168*2,	0, CHARS_SP_W, CHARS_SP_H},	// 確定
	{440,	107*2,	0, CHARS_SP_W, CHARS_SP_H},	// キャンセル
	{394,	157*2,	0, CHARS_SP_W, CHARS_SP_H},	// 削除
};


sprite_t	g_spCharsSrc_NP[] =		// ポルトガル語
{
	{154,	120,	0, CHARS_SP_W, CHARS_SP_H},	// 挿入位置移動
	{66,	176,	0, CHARS_SP_W, CHARS_SP_H},	// キー、項目の選択
	{173,	282,	0, CHARS_SP_W, CHARS_SP_H},	// キートップ切替
	{434,	262,	0, CHARS_SP_W, CHARS_SP_H},	// キー、項目の決定
	{416,	112,	0, CHARS_SP_W, CHARS_SP_H},	// 大/小文字切替
	{435,	162,	0, CHARS_SP_W, CHARS_SP_H},	// 空白
	{255,	334,	0, CHARS_SP_W, CHARS_SP_H},	// 確定
	{438,	212,	0, CHARS_SP_W, CHARS_SP_H},	// キャンセル
	{393,	314,	0, CHARS_SP_W, CHARS_SP_H},	// 削除
};



//============================================================================
// フレーム配置情報
iRect_t		g_spFrameSrc_NJ[] =		// 日本リージョン 日本語
{
	{112,	70,		131,	22},	// L2/R2
	{76,	126,	145,	22},	// 方向キー
	{119,	233,	151,	22},	// SELECT
	{169,	287,	130,	22},	// □
	{404,	64,		140,	22},	// R1
	{422,	113,	103,	22},	// △
	{425,	164,	125,	22},	// ○
	{422,	213,	96,		22},	// ×
	{381,	266,	100,	22},	// □
};

iRect_t		g_spFrameSrc_NJE[] =		// 日本リージョン 英語
{
	{112,	70,		131,	22},	// L2/R2
	{76,	126,	145,	22},	// 方向キー
	{119,	233,	151,	22},	// SELECT
	{168,	287,	130,	22},	// □
	{403,	64,		140,	22},	// R1
	{421,	113,	115,	22},	// △
	{425,	164,	101,	22},	// ○
	{421,	213,	96,		22},	// ×
	{381,	266,	100,	22},	// □

};

iRect_t		g_spFrameSrc_NE[] =		// 英語
{
	{110,	92,		131,	22},	// L2/R2
	{60,	148,	159,	22},	// 方向キー
	{108,	254,	158,	22},	// SELECT
	{194,	308,	102,	22},	// □
	{403,	84,		100,	22},	// R1
	{421,	134,	103,	22},	// △
	{425,	184,	101,	22},	// ○
	{421,	234,	96,		22},	// ×
	{380,	286,	100,	22},	// □
};


iRect_t		g_spFrameSrc_NF[] =		// フランス語
{
	{85,	47*2,	158,	22},	// L2/R2
	{19,	75*2,	199,	22},	// 方向キー
	{119,	127*2,	149,	22},	// SELECT
	{201,	155*2,	95, 	22},	// □
	{403,	43*2,	100,	22},	// R1
	{420,	68*2,	103,	22},	// △
	{425,	93*2,	101,	22},	// ○
	{421,	118*2,	96,		22},	// ×
	{380,	143*2,	100,	22},	// □
};


iRect_t		g_spFrameSrc_NS[] =		// スペイン語
{
	{120,	47*2,	121,	22},	// L2/R2
	{17,	75*2,	201,	22},	// 方向キー
	{123,	127*2,	144,	22},	// SELECT
	{207,	155*2,	90, 	22},	// □
	{403,	43*2,	100,	22},	// R1
	{421,	68*2,	103,	22},	// △
	{425,	93*2,	101,	22},	// ○
	{420,	118*2,	144,	22},	// ×
	{381,	143*2,	100,	22},	// □
};


iRect_t		g_spFrameSrc_NG[] =		// ドイツ語
{
	{76,	46*2,	166,	22},	// L2/R2
	{19,	75*2,	199,	22},	// 方向キー
	{31,	127*2,	236,	22},	// SELECT
	{194,	155*2,	103,	22},	// □
	{402,	43*2,	100,	22},	// R1
	{420,	68*2,	103,	22},	// △
	{424,	93*2,	101,	22},	// ○
	{420,	118*2,	96, 	22},	// ×
	{379,	143*2,	100,	22},	// □
};

iRect_t		g_spFrameSrc_NI[] =		// イタリア語
{
	{118,	47*2,	125,	22},	// L2/R2
	{50,	75*2,	169,	22},	// 方向キー
	{109,	128*2,	159,	22},	// SELECT
	{210,	155*2,	87,		22},	// □
	{403,	43*2,	100,	22},	// R1
	{421,	68*2,	103,	22},	// △
	{424,	93*2,	101,	22},	// ○
	{421,	118*2,	96,		22},	// ×
	{380,	143*2,	100,	22},	// □
};

iRect_t		g_spFrameSrc_ND[] =		// オランダ語
{
	{81,	47*2,	163,	22},	// L2/R2
	{37,	75*2,	184,	22},	// 方向キー
	{85,	127*2,	184,	22},	// SELECT
	{185,	155*2,	113,	22},	// □
	{403,	43*2,	100,	22},	// R1
	{421,	68*2,	103,	22},	// △
	{425,	93*2,	101,	22},	// ○
	{420,	118*2,	96,		22},	// ×
	{380,	143*2,	100,	22},	// □
};


iRect_t		g_spFrameSrc_NP[] =		// ポルトガル語
{
	{113,	92,		130,	22},	// L2/R2
	{20,	148,	199,	22},	// 方向キー
	{130,	255,	139,	22},	// SELECT
	{194,	306,	103,	22},	// □
	{402,	84,		100,	22},	// R1
	{420,	134,	103,	22},	// △
	{424,	184,	101,	22},	// ○
	{420,	234,	96,		22},	// ×
	{379,	286,	100,	22},	// □
};


//============================================================================
// ATOK バージョン
//
// CAUTION!! : このサンプルで使われているATOKロゴや商標は
//				改変して使ってはいけません。

sprite_t	g_spAtokVerSrc[] =			// バージョン
{
	{90,	350,	0,		425,	24},	// 日本語
	{45,	350,	0,		500,	24},	// 英語
};

sprite_t	g_spAtokLogoSrc[] =			// ロゴ
{
	{550,	334,	0,		40,		40},	// 日本語
	{550,	334,	0,		40,		40},	// 英語
};




//============================================================================
//============================================================================
typedef struct
{
	sprite_t	*spChars;		// 文字
	sprite_t	*spItems;		// コントローラなどの絵
	sprite_t	*spTitle;		// "基本操作画面"文字
	IRECT		*rcFrames;		// フレームスプライト

	uv_t		*uvChars;		// 文字
	uv_t		*uvItems;		// コントローラなどの絵
	uv_t		*uvTitle;		// "基本操作画面"文字
	uv_t		*uvFrames;		// フレームUV

	int			charNum;		// 文字の数
	int			itemNum;		// コントローラなどの絵の数
} helpMaps_t;

helpMaps_t		g_HelpMapsNTSC [] =
{
	// 日本リージョン 日本語
	{
		g_spCharsSrc_NJ,	g_spPadItemSrc_NJ,		&g_spTitleSrc[HELP_JAPANESE],	g_spFrameSrc_NJ,
		g_uvCharsSrc,		g_uvPadItemSrc,			&g_uvTitleSrc[HELP_JAPANESE],	g_uvFrameSrc,
		sizeof(g_spCharsSrc_NJ)/sizeof(sprite_t),	sizeof(g_spPadItemSrc_NJ)/sizeof(sprite_t)
	},
	// 英語
	{
		g_spCharsSrc_NE,	g_spPadItemSrc_NE,		&g_spTitleSrc[HELP_ENGLISH],	g_spFrameSrc_NE,
		g_uvCharsSrc,		g_uvPadItemSrc,			&g_uvTitleSrc[HELP_ENGLISH],	g_uvFrameSrc,
		sizeof(g_spCharsSrc_NE)/sizeof(sprite_t),	sizeof(g_spPadItemSrc_NE)/sizeof(sprite_t)
	},
	// フランス語
	{
		g_spCharsSrc_NF,	g_spPadItemSrc_NF,		&g_spTitleSrc[HELP_FRENCH],	g_spFrameSrc_NF,
		g_uvCharsSrc,		g_uvPadItemSrc,			&g_uvTitleSrc[HELP_FRENCH],	g_uvFrameSrc,
		sizeof(g_spCharsSrc_NF)/sizeof(sprite_t),	sizeof(g_spPadItemSrc_NF)/sizeof(sprite_t)
	},
	// スペイン語
	{
		g_spCharsSrc_NS,	g_spPadItemSrc_NS,		&g_spTitleSrc[HELP_SPANISH],	g_spFrameSrc_NS,
		g_uvCharsSrc,		g_uvPadItemSrc,			&g_uvTitleSrc[HELP_SPANISH],	g_uvFrameSrc,
		sizeof(g_spCharsSrc_NS)/sizeof(sprite_t),	sizeof(g_spPadItemSrc_NS)/sizeof(sprite_t)
	},
	// ドイツ語
	{
		g_spCharsSrc_NG,	g_spPadItemSrc_NG,		&g_spTitleSrc[HELP_GERMAN],	g_spFrameSrc_NG,
		g_uvCharsSrc,		g_uvPadItemSrc,			&g_uvTitleSrc[HELP_GERMAN],	g_uvFrameSrc,
		sizeof(g_spCharsSrc_NG)/sizeof(sprite_t),	sizeof(g_spPadItemSrc_NG)/sizeof(sprite_t)
	},
	// イタリア語
	{
		g_spCharsSrc_NI,	g_spPadItemSrc_NI,		&g_spTitleSrc[HELP_ITALIAN],	g_spFrameSrc_NI,
		g_uvCharsSrc,		g_uvPadItemSrc,			&g_uvTitleSrc[HELP_ITALIAN],	g_uvFrameSrc,
		sizeof(g_spCharsSrc_NI)/sizeof(sprite_t),	sizeof(g_spPadItemSrc_NI)/sizeof(sprite_t)
	},
	// オランダ語
	{
		g_spCharsSrc_ND,	g_spPadItemSrc_ND,		&g_spTitleSrc[HELP_DUTCH],	g_spFrameSrc_ND,
		g_uvCharsSrc,		g_uvPadItemSrc,			&g_uvTitleSrc[HELP_DUTCH],	g_uvFrameSrc,
		sizeof(g_spCharsSrc_ND)/sizeof(sprite_t),	sizeof(g_spPadItemSrc_ND)/sizeof(sprite_t)
	},
	// ポルトガル語
	{
		g_spCharsSrc_NP,	g_spPadItemSrc_NP,		&g_spTitleSrc[HELP_PORTUGUESE],	g_spFrameSrc_NP,
		g_uvCharsSrc,		g_uvPadItemSrc,			&g_uvTitleSrc[HELP_PORTUGUESE],	g_uvFrameSrc,
		sizeof(g_spCharsSrc_NP)/sizeof(sprite_t),	sizeof(g_spPadItemSrc_NP)/sizeof(sprite_t)
	},

	// 日本リージョン 英語
	{
		g_spCharsSrc_NJE,	g_spPadItemSrc_NJE,		&g_spTitleSrc[HELP_J_ENGLISH],	g_spFrameSrc_NJE,
		g_uvCharsSrc_JE,	g_uvPadItemSrc,			&g_uvTitleSrc[HELP_J_ENGLISH],	g_uvFrameSrc,
		sizeof(g_spCharsSrc_NJE)/sizeof(sprite_t),	sizeof(g_spPadItemSrc_NJE)/sizeof(sprite_t)
	},
};

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
}
#endif

#endif	// !__FEPSKB_INC__
