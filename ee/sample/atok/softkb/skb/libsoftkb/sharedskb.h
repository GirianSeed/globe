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

#ifndef __SHAREDSKB_H__
#define __SHAREDSKB_H__

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
extern "C" {
#endif

#include "shareddef.h"

// キーボードの状態
#define LAYOUT_SHIFT		(0x01)		// SHIFT     キーON
#define LAYOUT_CAPS			(0x02)		// CAPSLOCK  キーON
#define LAYOUT_ZENKAKU		(0x04)		// 半角/全角 キーON
#define LAYOUT_ALTGR		(0x04)		// AltGr     キーON(このキーがあるのはヨーロッパだけ)
#define LAYOUT_FOCUSSHIFT	(0x80)		// カーソルを合わせてSHIFTがONになったとき
// キーの状態
#define KEY_PUSH			(1<<1)		// 押下中       キー
#define KEY_DISABLE			(1<<2)		// 選択できない キー
#define KEY_FOCUSON			(1<<3)		// カーソル位置 キー
#define KEY_INVISIBLE		(1<<4)		// 見えない     キー
#define KEY_DUMMY			(1<<5)		// ダミー       キー
// マスク
#define SKBMASK_LAYOUT(_l_)		(_l_ & ~(1<<7))		// カーソルSHIFTのマスク
// マウス
#define SKBMOUSE_NULL		(-1)		// NULL フラグ
#define SKBMOUSE_LBTNDOWN	(1<<0)		// 左ボタン押下
#define SKBMOUSE_RBTNDOWN	(1<<1)		// 右ボタン押下
#define SKBMOUSE_WBTNDOWN	(1<<2)		// ホイール押下
#define SKBMOUSE_DOWN		(1)			// ボタン押下
#define SKBMOUSE_UP			(0)			// ボタン解除


// 色
#define SKBDEF_DISABLE_ALPHA	(0x40)

#define SKBDEF_ATTENTION_ALPHA(_a_)	((((u_int)(0xff*_a_))>>7) << (6*4))
#define SKBDEF_ATTENTION_COL	(0x00ff7070)

enum
{
	SKBCTRL_NORMAL = 0,	// ノーマルキー
	SKBCTRL_CAPS,		// CAPSLOCK
	SKBCTRL_SHIFT_L,	// SHIFT左
	SKBCTRL_SHIFT_R,	// SHIFT右
	SKBCTRL_BACKSPACE,	// バックスペース
	SKBCTRL_ENTER,		// 改行
	SKBCTRL_UP,			// ↑
	SKBCTRL_DOWN,		// ↓
	SKBCTRL_LEFT,		// ←
	SKBCTRL_RIGHT,		// →
	SKBCTRL_HELP,		// ヘルプ
	SKBCTRL_KANJI,		// 漢字
	SKBCTRL_TAB,		// タブ
	SKBCTRL_ABCQWERTY,	// ABC<->QWERTY

	SKBCTRL_MAX,
};



#define MODEBAR_HIRAGANA	(0)


typedef struct skey_s         skey_t;
typedef struct inhibitChar_s  inhibitChar_t;

typedef struct
{
	u_char ucUseAtok;
	int  (*readResource)  (skey_t *pKey, const char *filename, u_int keyTopNo, inhibitChar_t *inhibitChar, void *pTmp);
	void (*deleteResource)(skey_t *pKey);
	void (*makePacketsSkbBody)(sceVif1Packet *pk, skey_t *pKey,
								textureInfo_t *pTexBtn, textureInfo_t *pTexChar, textureInfo_t *pTexBg);
	void (*makePacketsCursor)(sceVif1Packet *pk, skey_t *pKey, textureInfo_t *pTexBtn);
	void (*setPad)        (skey_t *pKey, u_short down, u_short up, u_short press, u_short repeat);

	void (*open) (skey_t *pKey);
	void (*close)(skey_t *pKey);

	void (*mouseMove)			(skey_t *pKey, iPoint_t *pPoint);
	void (*mouseLButtonDown)	(skey_t *pKey, iPoint_t *pPoint);
	void (*mouseLButtonUp)		(skey_t *pKey, iPoint_t *pPoint);

	u_int (*searchFocus)  (skey_t *pKeyNext, sprite_t *pNowSp);


	// 操作系
	void (*trans)         (skey_t *pKey, int x, int y);
	void (*changeAlpha)   (skey_t *pKey, int index, u_char alpha);
	void (*initKeyTop)    (skey_t *pKey);

	void (*initSpecialKey)(skey_t *pKey, u_char flag);

} exportSkb_t;


// SOFTKB関数群からも参照できる構造体
typedef struct
{
	u_char			ucUseMouse;			// マウス使用
										// 0 : 使用しない
										// 1 : 使用する

	u_int			uiDragState;		// ドラッグ中か
										// 1bit : 左ボタン
										// 2bit : 右ボタン
										// 4bit : ホイール

	u_int			uiOldCurPos[3];		// ドラッグしたまま他のボタンに移動した時の検出用ワーク
										// [0] : 左ボタン
										// [1] : 右ボタン
										// [2] : Wheelボタン

	u_int			uiOldCurPosList[2];	// 一覧X,Y値

	u_int			uiTogglePos16;		// トグルPOS[絶対座標]
	u_int			uiDragToggle;
} protectedSkb_t;


// リスト関連
#define VISIBLE_CHARNUM_H		(16)		// 水平方向の見える文字数
#define VISIBLE_CHARNUM_V		(8)			// 垂直方向の見える文字数

typedef struct
{
	u_char buf[4];

} UTF8Char_t;

typedef struct
{
	UTF8Char_t		utf8Char;
	int				iFontWidth;

} UTF8CharDrawCache_t;

typedef struct
{
	int		iScrollPosY;		// スクロールY値
	int		iGlobalCurPosX;		
	int		iGlobalCurPosY;
	int		iRepeatAccel;
	int		iOldPosY16;			// トグル
	sprite_t spToggle16;

	u_int	auiCodeTable[600];
	int		iLineNum;

	int		iUtf8CacheTopLine;

	UTF8CharDrawCache_t	utf8Cache[VISIBLE_CHARNUM_H * VISIBLE_CHARNUM_V];

	// 固有
	charCodeBlock_t *pListItem;
} skblist_t;




//============================================================================
// キーリソース構造体
typedef struct
{
	u_short	uvCharIndex[2];		// 文字	UV情報へのオフセット
	u_short	ctrlCode;			// コントロールコード(ESC、CAPSLOCK、SHIFT、ENTER、↑↓←→、TAB)
	u_int	key[10];			// 文字コード
} buttonState_t;


typedef struct
{
	buttonState_t	state[8];
	u_short			spBaseIndex;	// キーベース スプライト情報へのオフセット
	u_short			spCharIndex;	// キートップ スプライト情報へのオフセット

	u_short			uvBaseIndex[2];	// キーベース UV情報へのオフセット
									// 0: 通常
									// 1: 押された時

	u_int			rgba;			// スプライト色
	u_char			animFlag;		// アニメーション用フラグ
									// 0x01 : 通常
									// 0x02 : 押された
									// 0x04 : 選択できない
									// 0x10 : 透明(ダミー)

	u_short			direction[4];	// 方向キーによるカーソルの行き先定義
									// [0] : ↑
									// [1] : ↓
									// [2] : ←
									// [3] : →

} button_t;


// ソフトウェアキーボードの情報
typedef struct
{
	unsigned int w, h;				// ソフトウェアキーボードの幅、高さ
} skbInfo_t;


// キー配列各情報へのインデックス
typedef struct
{
	int length;						// データの長さ(バイト単位)
	int offset;						// データオフセット(ファイルの先頭から)
} lump_t;


// キーリソースファイル中の情報
enum
{
	LUMP_SPRITE,	// スプライト情報の塊
	LUMP_UV,		// UV情報の塊
	LUMP_KBD,		// キー情報の塊
	LUMP_PANE,		// 背景チップの塊
	LUMP_SKB,		// ソフトウェアキーボードの情報

	LUMP_MAX
};





//============================================================================
// カーソル定義
#define SKBCURSOR_UV_X	STG(208)
#define SKBCURSOR_UV_Y	STG(156)
#define SKBCURSOR_UV_W	STG(26/2)
#define SKBCURSOR_UV_H	STG(26/2)
#define SKBCURSOR_SP_W	(26/2)
#define SKBCURSOR_SP_H	(26/2)

const static uv_t g_uvCursors[] =
{
	{	SKBCURSOR_UV_X,					SKBCURSOR_UV_Y,					SKBCURSOR_UV_W,		SKBCURSOR_UV_H	},	// 左上"┌"テクスチャ
	{	SKBCURSOR_UV_X+SKBCURSOR_UV_W,	SKBCURSOR_UV_Y,					SKBCURSOR_UV_W,		SKBCURSOR_UV_H	},	// 右上"┐"テクスチャ
	{	SKBCURSOR_UV_X,					SKBCURSOR_UV_Y+SKBCURSOR_UV_H,	SKBCURSOR_UV_W,		SKBCURSOR_UV_H	},	// 左下"└"テクスチャ
	{	SKBCURSOR_UV_X+SKBCURSOR_UV_W,	SKBCURSOR_UV_Y+SKBCURSOR_UV_H,	SKBCURSOR_UV_W,		SKBCURSOR_UV_H	},	// 右下"┘"テクスチャ
	{	0,								0,								0,					0				},	// ダミー
};




//============================================================================
// 各SOFTKBでの共通関数

int  readResourceFrame(skey_t *pKey, const char *filename, u_int keyTopNo, inhibitChar_t *inhibitChar, void *pTmp);
void makePacketsBody(sceVif1Packet *pk, skey_t *pKey,
						 textureInfo_t *pTexBtn, textureInfo_t *pTexChar, textureInfo_t *pTexBg);

void setLayout(skey_t *pKey);
void setInhibitChars(skey_t *pKey, inhibitChar_t *inhibitChar);
void setEnableChars(skey_t *pKey);
void setReleseChars(skey_t *pKey);

void pushEnter(skey_t *pKey);
void pushLeftArrow(skey_t *pKey);
void pushRightArrow(skey_t *pKey);
void pushUpArrow(skey_t *pKey);
void pushDownArrow(skey_t *pKey);
void pushSpace(skey_t *pKey);
void pushShift(skey_t *pKey, u_char flag);
void releaseShift(skey_t *pKey, u_char flag);
void pushCapsLock(skey_t *pKey);
void pushEsc(skey_t *pKey);
void pushBackSpace(skey_t *pKey);
void pushTab(skey_t *pKey);
void pushKanji(skey_t *pKey);

void moveCursor(skey_t *pKey, int dx, int dy);
void makeCursorSprite(sprite_t *pSpSrc, sprite_t *pSpDst, u_int floating);


int onStateCtrlCode(skey_t *pKey, u_int ctrlCode, u_char state);
int offStateCtrlCode(skey_t *pKey, u_int ctrlCode, u_char state);
int onStateKey(skey_t *pKey, u_int *key, u_char state);
int offStateKey(skey_t *pKey, u_int *key, u_char state);

int searchKeyByPoint(skey_t *pKey, iPoint_t *pPt);


#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
}
#endif

#endif	// ! __SHAREDSKB_H__
