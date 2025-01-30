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

#ifndef __LIBSOFTKB_H__
#define __LIBSOFTKB_H__

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
extern "C" {
#endif

#include <shareddef.h>
#include "sharedskb.h"

// 入力モード
#define SKBMODE_ZENKAKU			(0x80)
#define SKBMODE_HIRAGANA		(SKBMODE_ZENKAKU | 0x00)
#define SKBMODE_HAN_KATAKANA	(0x01)
#define SKBMODE_ZEN_KATAKANA	(SKBMODE_ZENKAKU | 0x01)
#define SKBMODE_HAN_ALPHABET	(0x02)
#define SKBMODE_ZEN_ALPHABET	(SKBMODE_ZENKAKU | 0x02)

#define KEYTOP_NUM_MAX		(10)
#define SKBMODE_NOHIT		(KEYTOP_NUM_MAX + 5)

// フェーズ
enum
{
	SKBPHASE_OPENING,	// オープン中
	SKBPHASE_OPENED,	// オープンし終わった
	SKBPHASE_CLOSING,	// クローズ中
	SKBPHASE_CLOSED,	// クローズし終わった

	SKBPHASE_MAX,
};

// アルファ値配列のインデックス
enum
{
	SKBALPHA_BG,
	SKBALPHA_BASE,
	SKBALPHA_CHAR,
	SKBALPHA_CURSOR,

	SKBALPHA_MAX
};

// キートップ
enum
{
	// 日本版
	KEYTOP_HIRAGANA = 0,	// 50音配列ひらがな
	KEYTOP_KATAKANA,		// 50音配列カタカナ
	KEYTOP_ABC,				// ABC配列
	KEYTOP_SIGN,			// 記号(特殊ボタンだけ)
	KEYTOP_NUMJP,			// 数字(日本版：全角半角ボタンあり)
	KEYTOP_PC_JAPAN,		// 日本
	// 海外版
	KEYTOP_PC_US,			// アメリカ
	KEYTOP_PC_FRANCE,		// フランス
	KEYTOP_PC_SPAIN,		// スペイン
	KEYTOP_PC_GERMAN,		// ドイツ
	KEYTOP_PC_ITALIA,		// イタリア
	KEYTOP_PC_DUTCH,		// オランダ
	KEYTOP_PC_PORTUGRESS,	// ポルトガル
	KEYTOP_PC_UK,			// イギリス
	KEYTOP_PC_CANADAF,		// カナディアンフレンチ
	KEYTOP_NUMF,			// 数字(海外版：全角半角ボタン無し)
	KEYTOP_SIGNF,			// 記号(通貨etc)
	KEYTOP_IDIOMCHAR,		// 発音記号付き文字

	KEYTOP_MAX
};

// モードバー
enum
{
	// 日本版
	MODEBAR_CHAR_HIRAGANA_J = 0,	// ひらがな
	MODEBAR_CHAR_KATAKANA_J,		// カタカナ
	MODEBAR_CHAR_ALPHABET_J,		// アルファベット
	MODEBAR_CHAR_SIGN_J,			// 記号
	// 海外版
	MODEBAR_CHAR_ALPHABET_F,		// アルファベット
	MODEBAR_CHAR_UMLAUT_F,			// 発音記号つきアルファベット
	MODEBAR_CHAR_SIGN_F,			// 記号
	// 共通
	MODEBAR_CHAR_NUM,				// 数字

	MODEBAR_CHAR_MAX
};

// 入力禁止文字 構造体
struct inhibitChar_s
{
	u_int	*pCtrlKey;		// 入力禁止コントロールキーへのポインタ
	u_int	uiCKeyNum;		// 入力禁止コントロールキーの数
	u_int	(*pChar)[10];	// 入力禁止文字列へのポインタ(UCS4)
	u_int	uiCharNum;		// 入力禁止文字列の数
};



struct skey_s
{
	// このキーボードが使う関数
	exportSkb_t		export;

	int				created;		// 0:まだ作成されていない/1:作成済み
	u_int			uiKeyTopNo;		// キートップ番号
	u_int			uiCursorPos;	// 現在のカーソル位置
	u_int			uiPush;			// ○ボタンが押されているかフラグ

	// 状態
	u_char			ucInputMode;	// 入力モード
									// 0: ひらがな
									// 1: カタカナ
									// 2: アルファベット

	u_char			ucModebarType;	// モードバータイプ
									// 0: ひらがな
									// 1: カタカナ
									// 2: アルファベット
									// 3: 記号
									// 4: アルファベット
									// 5: 発音記号つきアルファベット
									// 6: 記号
									// 7: 数字

	u_char			ucShift;		// シフト状態
									// bit 1: SHIFT    ON
									// bit 2: CAPSLOCK ON
									// bit 4: ZENKAKU  ON/ALTGR ON
									// bit 8: カーソルでSHIFTが押された時 ON

	inhibitChar_t	*pInhibitChar;	// 禁止文字へのポインタ

	// 描画用
	char			*pKeyBuff;		// キーリソースバッファ
	u_int			uiSpNum;		// スプライト数
	u_int			uiUvNum;		// UV数
	u_int			uiChipNum;		// 背景用チップ数
	u_int			uiBtnNum;		// キー数
	sprite_t		*pSps;			// スプライト情報
	uv_t			*pUvs;			// UV情報
	paneChip_t		*pChips;		// 背景用チップ
	button_t		*pBtns;			// キー情報
	skbInfo_t		*pSkbInfo;		// ソフトウェアキーボード情報

	u_char			ucAlpha[SKBALPHA_MAX];		// アルファ値
												// 0:BG
												// 1:キーボタン
												// 2:文字
												// 3:カーソル

	int		uiKbdPosX, uiKbdPosY;	// キーボードの位置

	sprite_t		spCursor[5];	// カーソルのスプライト
	int				iCursorPos;		// カーソルのフワフワ

	int				iPhase;			// フェーズ

	skblist_t		List;			// リスト
};


int  SoftKB_Init(gImport_t *pImport);
void SoftKB_Destroy(void);

void SoftKB_Run(sceVif1Packet *pk, skey_t *pKey);
int  SoftKB_CheckShown(void);
void SoftKB_UseMouse(u_char flag);
int  SoftKB_Create(skey_t *pKey, iPoint_t *skbPoint, u_int keyTopNo, inhibitChar_t *inhibitChar, void *pTmp);
void SoftKB_Delete(skey_t *pKey);
void SoftKB_Open(skey_t *pKey);
void SoftKB_Close(skey_t *pKey);

void SoftKB_SetPadData(skey_t *pKey, u_long paddata);
void SoftKB_SetMouseData(skey_t *pKey, u_long128 *data);

void SoftKB_Move(skey_t *pKey, int x, int y);
void SoftKB_ChangeAlpha(skey_t *pKey, int index, u_char alpha);
u_char SoftKB_GetInputMode(skey_t *pKey);
void SoftKB_GetSize(skey_t *pKey, int *w, int *h);
void SoftKB_MoveModebar(int x, int y);
void SoftKB_SetModebarType(const u_int *pTypes, int maxNum);
void SoftKB_ClearModebarType(void);
void SoftKB_DrawFocus(sceVif1Packet *pk, sprite_t *sp, u_int curPos, u_int col);
u_int SoftKB_SearchFocus(skey_t *pKeyNow, skey_t *pKeyNext);
void SoftKB_SetStartCurPos(skey_t *pKey, u_int keyTopType);


const iPoint_t* SoftKB_GetMousePoint(void);
//const u_int SoftKB_GetMouseBtn(void);     // 未実装
//const u_int SoftKB_GetMouseWheel(void);   // 未実装

#if 1//SKB_EX Sound
void SoftKB_se_play(int sound);
#endif

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
}
#endif

#endif	// !__LIBSOFTKB_H__
