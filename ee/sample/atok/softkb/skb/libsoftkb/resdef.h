/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
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

#ifndef __RESDEF_H__
#define __RESDEF_H__

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
extern "C" {
#endif

// Makefile define
//   SKBRES_LINK
//   REGION_J_ONLY
//   REGION_J_OTHER_ONLY
//   CDBOOT

typedef struct
{
	#ifdef SKBRES_LINK  
	char*  caKeyTopFilename;		// ファイル名
	#else
	char   caKeyTopFilename[LOCALPATH_MAX];		// ファイル名
	#endif
	u_char fepType;								// FEPタイプ
												// 		ひらがな、カタカナ、無変換
	u_char modeType;							// モードタイプ:
												// 		ひらがな、カタカナ、アルファベット、数字、記号
} rsc_t;

#ifdef SKBRES_LINK
//--------------------------------------------------------
#include <skbres.h>
//---------------------------------------
// skbres.a用
//---------------------------------------
static rsc_t g_RSC[] =
{
	// 日本版
	{	// 50音配列ひらがな
		0,
		0,
		MODEBAR_CHAR_HIRAGANA_J,
	},
	{	// 50音配列カタカナ
		0,
		1,
		MODEBAR_CHAR_KATAKANA_J,
	},
	{	// ABC配列
		0,
		2,
		MODEBAR_CHAR_ALPHABET_J,
	},
	{	// 記号(特殊ボタンだけ)
		0,
		2,
		MODEBAR_CHAR_SIGN_J,
	},
	{	// 数字(日本版：全角半角ボタン付き)
		0,
		2,
		MODEBAR_CHAR_NUM,
	},
	{	// 日本106キーボード
		0,
		2,
		MODEBAR_CHAR_ALPHABET_J,
	},
	// 海外版
	// QWERTY & AZERTY
	{	// アメリカ
		0,
		2,
		MODEBAR_CHAR_ALPHABET_F,
	},
	{	// フランス
		0,
		2,
		MODEBAR_CHAR_ALPHABET_F,
	},
	{	// スペイン
		0,
		2,
		MODEBAR_CHAR_ALPHABET_F,
	},
	{	// ドイツ
		0,
		2,
		MODEBAR_CHAR_ALPHABET_F,
	},
	{	// イタリア
		0,
		2,
		MODEBAR_CHAR_ALPHABET_F,
	},
	{	// オランダ
		0,
		2,
		MODEBAR_CHAR_ALPHABET_F,
	},
	{	// ポルトガル
		0,
		2,
		MODEBAR_CHAR_ALPHABET_F,
	},
	{	// イギリス
		0,
		2,
		MODEBAR_CHAR_ALPHABET_F,
	},
	{	// カナディアンフレンチ
		0,
		2,
		MODEBAR_CHAR_ALPHABET_F,
	},
	{	// 数字
		0,
		2,
		MODEBAR_CHAR_NUM,
	},
	{	// 記号(通貨etc)
		0,
		2,
		MODEBAR_CHAR_SIGN_F,
	},
	{	// 発音記号付き文字
		0,
		2,
		MODEBAR_CHAR_UMLAUT_F,
	},
};

// テクスチャ定義
static const char *caTexFilename[] =
{
	// ソフトウェアキーボード
	skbBG_tm2,		// 背景
	skbKey_tm2,		// キートップ文字
	skbBtn_tm2,		// キー(ボタン)
	skbMdbar_tm2,		// 入力モード
	// ヘルプ
	skbHItem_tm2,	// コントローラ,ボタンの絵
};

static const char *caTexLangFilename[] =
{
	// ヘルプ文字
	#ifdef REGION_J_OTHER_ONLY
	0,
	#else
	skbHJp_tm2,	// 日本
	#endif
	skbHEng_tm2,	// 英語(日本向け英語バージョンも含む)
	#ifdef REGION_J_ONLY
	0,
	0,
	0,
	0,
	0,
	0,
	#else
	skbHFch_tm2,	// フランス語
	skbHSp_tm2,	// スペイン語
	skbHGmn_tm2,	// ドイツ語
	skbHIta_tm2,	// イタリア語
	skbHDch_tm2,	// オランダ語
	skbHPtg_tm2,	// ポルトガル語
	#endif
};
//--------------------------------------------------------
#else

#if CDBOOT
//---------------------------------------
// CDBOOT用
//---------------------------------------
static const rsc_t g_RSC[] =
{
	// 日本版
	{
		#ifdef REGION_J_OTHER_ONLY
		0,
		#else
		SKB_JPKEY_PATH"\\AIUHIRA.BIN;1",		// 50音配列ひらがな
		#endif
		0,
		MODEBAR_CHAR_HIRAGANA_J,
	},
	{
		#ifdef REGION_J_OTHER_ONLY
		0,
		#else
		SKB_JPKEY_PATH"\\AIUKANA.BIN;1",		// 50音配列カタカナ
		#endif
		1,
		MODEBAR_CHAR_KATAKANA_J,
	},
	{
		#ifdef REGION_J_OTHER_ONLY
		0,
		#else
		SKB_JPKEY_PATH"\\ABC.BIN;1",			// ABC配列
		#endif
		2,
		MODEBAR_CHAR_ALPHABET_J,
	},
	{
		#ifdef REGION_J_OTHER_ONLY
		0,
		#else
		SKB_JPKEY_PATH"\\KIGO.BIN;1",		// 記号(特殊ボタンだけ)
		#endif
		2,
		MODEBAR_CHAR_SIGN_J,
	},
	{
		#ifdef REGION_J_OTHER_ONLY
		0,
		#else
		SKB_JPKEY_PATH"\\NUM.BIN;1",			// 数字(日本版：全角半角ボタン付き)
		#endif
		2,
		MODEBAR_CHAR_NUM,
	},
	{
		#ifdef REGION_J_OTHER_ONLY
		0,
		#else
		SKB_JPKEY_PATH"\\QWERTY.BIN;1",		// 日本
		#endif
		2,
		MODEBAR_CHAR_ALPHABET_J,
	},
	// 海外版
	// QWERTY & AZERTY
	{
		#ifdef REGION_J_ONLY
		0,
		#else
		SKB_OTHERKEY_PATH"\\USQ.BIN;1",			// アメリカ
		#endif
		2,
		MODEBAR_CHAR_ALPHABET_F,
	},
	{
		#ifdef REGION_J_ONLY
		0,
		#else
		SKB_OTHERKEY_PATH"\\FRENCHQ.BIN;1",		// フランス
		#endif
		2,
		MODEBAR_CHAR_ALPHABET_F,
	},
	{
		#ifdef REGION_J_ONLY
		0,
		#else
		SKB_OTHERKEY_PATH"\\SPANISHQ.BIN;1",	// スペイン
		#endif
		2,
		MODEBAR_CHAR_ALPHABET_F,
	},
	{
		#ifdef REGION_J_ONLY
		0,
		#else
		SKB_OTHERKEY_PATH"\\GERMANQ.BIN;1",		// ドイツ
		#endif
		2,
		MODEBAR_CHAR_ALPHABET_F,
	},
	{
		#ifdef REGION_J_ONLY
		0,
		#else
		SKB_OTHERKEY_PATH"\\ITALIANQ.BIN;1",	// イタリア
		#endif
		2,
		MODEBAR_CHAR_ALPHABET_F,
	},
	{
		#ifdef REGION_J_ONLY
		0,
		#else
		SKB_OTHERKEY_PATH"\\DUTCHQ.BIN;1",		// オランダ
		#endif
		2,
		MODEBAR_CHAR_ALPHABET_F,
	},
	{
		#ifdef REGION_J_ONLY
		0,
		#else
		SKB_OTHERKEY_PATH"\\PORTUGQ.BIN;1",		// ポルトガル
		#endif
		2,
		MODEBAR_CHAR_ALPHABET_F,
	},
	{
		#ifdef REGION_J_ONLY
		0,
		#else
		SKB_OTHERKEY_PATH"\\UKQ.BIN;1",			// イギリス
		#endif
		2,
		MODEBAR_CHAR_ALPHABET_F,
	},
	{
		#ifdef REGION_J_ONLY
		0,
		#else
		SKB_OTHERKEY_PATH"\\CFRENCHQ.BIN;1",	// カナディアンフレンチ
		#endif
		2,
		MODEBAR_CHAR_ALPHABET_F,
	},
	{
		#ifdef REGION_J_ONLY
		0,
		#else
		SKB_OTHERKEY_PATH"\\NUMF.BIN;1",		// 数字
		#endif
		2,
		MODEBAR_CHAR_NUM,
	},
	{
		#ifdef REGION_J_ONLY
		0,
		#else
		SKB_OTHERKEY_PATH"\\SPSIGN.BIN;1",		// 記号(通貨etc)
		#endif
		2,
		MODEBAR_CHAR_SIGN_F,
	},
	{
		#ifdef REGION_J_ONLY
		0,
		#else
		SKB_OTHERKEY_PATH"\\IDIOMCHR.BIN;1",	// 発音記号付き文字
		#endif
		2,
		MODEBAR_CHAR_UMLAUT_F,
	},
};

// テクスチャ定義
static const char *caTexFilename[] =
{
	// ソフトウェアキーボード
	SKB_SKBTEX_PATH"\\SKBBG.TM2;1",		// 背景
	SKB_SKBTEX_PATH"\\SKBKEY.TM2;1",	// キートップ文字
	SKB_SKBTEX_PATH"\\SKBBTN.TM2;1",	// キー(ボタン)
	SKB_SKBTEX_PATH"\\SKBMDBAR.TM2;1",	// 入力モード
	// ヘルプ
	SKB_HELPTEX_PATH"\\SKBHITEM.TM2;1",	// コントローラ,ボタンの絵
};

static const char *caTexLangFilename[] =
{
	// ヘルプ文字
	#ifdef REGION_J_OTHER_ONLY
	0,
	#else
	SKB_HELPTEX_PATH"\\SKBHJP.TM2;1",	// 日本
	#endif

	SKB_HELPTEX_PATH"\\SKBHENG.TM2;1",	// 英語(日本向け英語バージョンも含む)

	#ifdef REGION_J_ONLY
	0,
	0,
	0,
	0,
	0,
	0,
	#else
	SKB_HELPTEX_PATH"\\SKBHFCH.TM2;1",	// フランス語
	SKB_HELPTEX_PATH"\\SKBHSP.TM2;1",	// スペイン語
	SKB_HELPTEX_PATH"\\SKBHGMN.TM2;1",	// ドイツ語
	SKB_HELPTEX_PATH"\\SKBHITA.TM2;1",	// イタリア語
	SKB_HELPTEX_PATH"\\SKBHDCH.TM2;1",	// オランダ語
	SKB_HELPTEX_PATH"\\SKBHPTG.TM2;1",	// ポルトガル語
	#endif
};
//--------------------------------------------------------
#else //CDBOOT

//---------------------------------------
// HOST用
//---------------------------------------
static const rsc_t g_RSC[] =
{
	// 日本版
	{
		#ifdef REGION_J_OTHER_ONLY
		0,
		#else
		SKB_JPKEY_PATH"/aiuHira.bin",		// 50音配列ひらがな
		#endif
		0,
		MODEBAR_CHAR_HIRAGANA_J,
	},
	{
		#ifdef REGION_J_OTHER_ONLY
		0,
		#else
		SKB_JPKEY_PATH"/aiuKana.bin",		// 50音配列カタカナ
		#endif
		1,
		MODEBAR_CHAR_KATAKANA_J,
	},
	{
		#ifdef REGION_J_OTHER_ONLY
		0,
		#else
		SKB_JPKEY_PATH"/abc.bin",			// ABC配列
		#endif
		2,
		MODEBAR_CHAR_ALPHABET_J,
	},
	{
		#ifdef REGION_J_OTHER_ONLY
		0,
		#else
		SKB_JPKEY_PATH"/kigo.bin",			// 記号(特殊ボタンだけ)
		#endif
		2,
		MODEBAR_CHAR_SIGN_J,
	},
	{
		#ifdef REGION_J_OTHER_ONLY
		0,
		#else
		SKB_JPKEY_PATH"/num.bin",			// 数字(日本版：全角半角ボタン付き)
		#endif
		2,
		MODEBAR_CHAR_NUM,
	},
	{
		#ifdef REGION_J_OTHER_ONLY
		0,
		#else
		SKB_JPKEY_PATH"/qwerty.bin",		// 日本
		#endif
		2,
		MODEBAR_CHAR_ALPHABET_J,
	},
	// 海外版
	// QWERTY & AZERTY
	{
		#ifdef REGION_J_ONLY
		0,
		#else
		SKB_OTHERKEY_PATH"/usQ.bin",			// アメリカ
		#endif
		2,
		MODEBAR_CHAR_ALPHABET_F,
	},
	{
		#ifdef REGION_J_ONLY
		0,
		#else
		SKB_OTHERKEY_PATH"/frenchQ.bin",		// フランス
		#endif
		2,
		MODEBAR_CHAR_ALPHABET_F,
	},
	{
		#ifdef REGION_J_ONLY
		0,
		#else
		SKB_OTHERKEY_PATH"/spanishQ.bin",		// スペイン
		#endif
		2,
		MODEBAR_CHAR_ALPHABET_F,
	},
	{
		#ifdef REGION_J_ONLY
		0,
		#else
		SKB_OTHERKEY_PATH"/germanQ.bin",		// ドイツ
		#endif
		2,
		MODEBAR_CHAR_ALPHABET_F,
	},
	{
		#ifdef REGION_J_ONLY
		0,
		#else
		SKB_OTHERKEY_PATH"/italianQ.bin",		// イタリア
		#endif
		2,
		MODEBAR_CHAR_ALPHABET_F,
	},
	{
		#ifdef REGION_J_ONLY
		0,
		#else
		SKB_OTHERKEY_PATH"/dutchQ.bin",			// オランダ
		#endif
		2,
		MODEBAR_CHAR_ALPHABET_F,
	},
	{
		#ifdef REGION_J_ONLY
		0,
		#else
		SKB_OTHERKEY_PATH"/portugQ.bin",		// ポルトガル
		#endif
		2,
		MODEBAR_CHAR_ALPHABET_F,
	},
	{
		#ifdef REGION_J_ONLY
		0,
		#else
		SKB_OTHERKEY_PATH"/ukQ.bin",			// イギリス
		#endif
		2,
		MODEBAR_CHAR_ALPHABET_F,
	},
	{
		#ifdef REGION_J_ONLY
		0,
		#else
		SKB_OTHERKEY_PATH"/cfrenchQ.bin",		// カナディアンフレンチ
		#endif
		2,
		MODEBAR_CHAR_ALPHABET_F,
	},
	{
		#ifdef REGION_J_ONLY
		0,
		#else
		SKB_OTHERKEY_PATH"/numF.bin",			// 数字
		#endif
		2,
		MODEBAR_CHAR_NUM,
	},
	{
		#ifdef REGION_J_ONLY
		0,
		#else
		SKB_OTHERKEY_PATH"/spSign.bin",			// 記号(通貨etc)
		#endif
		2,
		MODEBAR_CHAR_SIGN_F,
	},
	{
		#ifdef REGION_J_ONLY
		0,
		#else
		SKB_OTHERKEY_PATH"/idiomChr.bin",		// 発音記号付き文字
		#endif
		2,
		MODEBAR_CHAR_UMLAUT_F,
	},
};

// テクスチャ定義
static const char *caTexFilename[] =
{
	// ソフトウェアキーボード
	SKB_SKBTEX_PATH"/skbBG.tm2",		// 背景
	SKB_SKBTEX_PATH"/skbKey.tm2",		// キートップ文字
	SKB_SKBTEX_PATH"/skbBtn.tm2",		// キー(ボタン)
	SKB_SKBTEX_PATH"/skbMdbar.tm2",		// 入力モード
	// ヘルプ
	SKB_HELPTEX_PATH"/skbHItem.tm2",	// コントローラ,ボタンの絵
};

static const char *caTexLangFilename[] =
{
	// ヘルプ文字
	#ifdef REGION_J_OTHER_ONLY
	0,
	#else
	SKB_HELPTEX_PATH"/skbHJp.tm2",	// 日本
	#endif

	SKB_HELPTEX_PATH"/skbHEng.tm2",	// 英語(日本向け英語バージョンも含む)

	#ifdef REGION_J_ONLY
	0,
	0,
	0,
	0,
	0,
	0,
	#else
	SKB_HELPTEX_PATH"/skbHFch.tm2",	// フランス語
	SKB_HELPTEX_PATH"/skbHSp.tm2",	// スペイン語
	SKB_HELPTEX_PATH"/skbHGmn.tm2",	// ドイツ語
	SKB_HELPTEX_PATH"/skbHIta.tm2",	// イタリア語
	SKB_HELPTEX_PATH"/skbHDch.tm2",	// オランダ語
	SKB_HELPTEX_PATH"/skbHPtg.tm2",	// ポルトガル語
	#endif
};
#endif //CDBOOT
#endif //SKBRES_LINK


#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
}
#endif

#endif	// !__RESDEF_H__
