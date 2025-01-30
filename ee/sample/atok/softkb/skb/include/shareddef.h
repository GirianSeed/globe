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

#ifndef __SHAREDDEF_H__
#define __SHAREDDEF_H__

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
extern "C" {
#endif

#include <assert.h>
#include <libccc.h>
#include <libpkt.h>

#ifndef ATOK_NO_LINK
#include <libatok.h>
#else
#define DUMYSIZE (16)
typedef unsigned char sceAtokContext[DUMYSIZE] __attribute__((aligned(64)));
#endif

#include "extypes.h"

#define STG(_v_)			(((_v_) << 4) + 8)

#define SKB_ASSERT(_a_)     assert((int)_a_)
#define SKBSMPL_ASSERT(_a_) assert((int)_a_)

#define LOCALPATH_MAX		(256)
#if CDBOOT
#define LOCALPATH			"cdrom0:\\RESOURCE"
#else
#define LOCALPATH			"host0:skb/resource"
#endif

// ソフトウェアキーボードで使われるリソースへのパス
#if CDBOOT
#define SKB_JPKEY_PATH		LOCALPATH"\\KEY\\JP"		// 日本向けキートップのパス
#define SKB_OTHERKEY_PATH	LOCALPATH"\\KEY\\OTHER"		// 海外向けキートップのパス
#define SKB_SKBTEX_PATH		LOCALPATH"\\TEX\\SKB"		// テクスチャのパス
#define SKB_HELPTEX_PATH	LOCALPATH"\\TEX\\HELP"		// ヘルプテクスチャのパス
#else
#define SKB_JPKEY_PATH		LOCALPATH"/key/jp"		// 日本向けキートップのパス
#define SKB_OTHERKEY_PATH	LOCALPATH"/key/other"	// 海外向けキートップのパス
#define SKB_SKBTEX_PATH		LOCALPATH"/tex/skb"		// テクスチャのパス
#define SKB_HELPTEX_PATH	LOCALPATH"/tex/help"	// ヘルプテクスチャのパス
#endif

// 仕向け情報
#define REGION_J	(0)
#define REGION_A	(1)
#define REGION_E	(2)

typedef enum
{
	// キーボード
	ENV_KEYBD_TYPE,				// タイプ
	ENV_KEYBD_REPEATW,			// リピートウェイト
	ENV_KEYBD_REPEATS,			// リピート開始

	// マウス
	ENV_MOUSE_SPEED,			// スピード
	ENV_MOUSE_DBLCLK,			// ダブルクリック間隔
	ENV_MOUSE_LR,				// LEFTHAND RIGHTHAND

	// ATOK
	ENV_ATOK_MODE,				// ローマ字入力、仮名入力
	ENV_ATOK_BIND,				// バインドタイプ

	// ソフトウェアキーボード
	ENV_SOFTKB_ONOFF,			// ON/OFF
	ENV_SOFTKB_QWERTY,			// ABC配列orQWERTYか？

	ENV_MAX
} SYSenv_m;


// キーバインド
#define KEYBIND_TYPE1	(0)
#define KEYBIND_TYPE2	(1)

// 文字入力FEP
#define FEPMODE_HIRAGANA	(0)
#define FEPMODE_KATAKANA	(1)
#define FEPMODE_ALPHABET	(2)

typedef IRECT		iRect_t;
typedef IPOINT		iPoint_t;

typedef struct
{
	int x __attribute__((aligned(16)));
	int y, z;
	u_short w, h;
} sprite_t __attribute__((aligned(16)));

typedef struct
{
	unsigned short u __attribute__((aligned(8)));
	unsigned short v, tw, th;
} uv_t __attribute__((aligned(8)));

typedef struct
{
	unsigned short spIndex;
	unsigned short uvIndex;
	unsigned int   rgba;
} paneChip_t;


typedef struct
{
	char		*pTim2Buffer;			// TIM2バッファポインタ
	sceGsTex0	GsTex0;					// レジスタ設定
	u_int		nTexSizeW, nTexSizeH;	// テクスチャ幅、高さ

	u_char		bMipmapLv;				// TIM2の最大ミップマップレベル(0～6)
	u_long64	GsMiptbp1;				// TIM2のMIPTBP1レジスタ値
	u_long64	GsMiptbp2;				// TIM2のMIPTBP2レジスタ値
	u_int		nLod;					// MIPMAPのためのLOD値
	u_int		nFilter;				// MIPMAP時のフィルタモード
	u_int		nClutNo;				// CLUT番号
} textureInfo_t;


#define TF_FRAME_ELEMENT	(9)
enum
{
	// アルファ値
	TF_ALPHA_EDITORBG,		// テキスト入力部分の背景
	TF_ALPHA_FRONT,			// 文字
	TF_ALPHA_BACK,			// 文字背景
	TF_ALPHA_CARET,			// キャレット
	TF_ALPHA_CANDLISTBG,	// 候補一覧背景

	TF_ALPHA_MAX
};

typedef struct
{
	int					iHide;					// 0: 表示/ 1: 非表示
	iRect_t				rcEdit16;				// テキストフィールド位置、サイズ

	textureInfo_t		tex;					// テクスチャ情報
	struct
	{
		sprite_t		aSP[TF_FRAME_ELEMENT];	// スプライト
		uv_t			aUV[TF_FRAME_ELEMENT];	// UV
		paneChip_t		aPN[TF_FRAME_ELEMENT];	// PANE
	} textField, candList;						// ----テキストフィールド用
												// ----候補一覧用

	u_char		alphaValue[TF_ALPHA_MAX];

	// スクロール
	int			iScrollPosX16;		// X値
	int			iScrollLX16;
	int			iScrollRX16;

	// 色
	u_char	ucConvCol[4][4];			// 変換中      文字列
	u_char	ucMarkCol[4][4];			// 注目文節    文字列
	u_char	ucMarkBGCandListCol[4][4];	// 候補一覧注目文字列
	u_char	ucFontBGCol[4][4];			// 通常フォント文字列
	u_int	uiCaretColor;				// キャレット色

	int		iCaretAnimSpeed;			// キャレットアニメーションスピード

	// 文字関連
	int		fontH;						// フォント高さ
	int		dispMode;					// 文字表示モード

} textField_t;

typedef struct
{
	u_int start;
	u_int end;
} charCodeBlock_t;








// 色を示す32ビット型の定義と、それを扱うマクロ
#define DEFAULT_RGBA			(0x80808080)
#define DEFAULT_DISABLE_RGBA	(0x40404040)


//============================================================================
typedef struct
{
	u_int	uiCharHeight;		// フォントの高さ

	int  (*getToCaretPos)(void);	// キャレットの位置
	// 文字列描画
	u_int *(*getStrings)(void);
	void (*drawStrings)(sceVif1Packet *pk, const u_int *pUCS4, const iRect_t *pClip, const iRect_t *rc16);
	int  (*drawStringBeforeChange)(sceVif1Packet *pk, const u_int *pUCS4, const iRect_t *pClip, const iRect_t *rc16);
	int  (*drawStringChanging)(sceVif1Packet *pk, const u_int *pUCS4, const iRect_t *pClip, const iRect_t *rc16,
							  int *scrollLX16, int *scrollRX16,
							  int frameW16, int frameH16, textField_t *pTf,
							  void drawCandListFrame(sceVif1Packet *pk, textField_t *pTf, iRect_t *rc16, int nCurrentCand));
	int  (*getCaretPos)(void);
	int  (*getStringWidth)(const u_int *str);	// 入力文字列の幅を取得

	// 候補一覧
	int   (*getAtokState)(void);				// ATOKの入力状態を取得
	#if 1 //SKB_EX
	      //キャレットを上書きモード時に変化させるために追加
	int   (*isInsertMode)(void);				// 挿入モードか判定。
	#endif
} txtFImport_t;

typedef struct
{
	u_int	*puiUCS4;				// 文字列へのポインタ
	u_int	uiMaxTextLen;			// 最大文字数

	// アプリ操作
	void (*atokON)(void);						// ATOK ON
	void (*atokOFF)(sceAtokContext *pAtok);		// ATOK OFF
	void (*ok)(void);							// ok
	void (*cancel)(void);						// キャンセル
	void (*changeMode)(int mode);				// 入力モード変更
	int  (*changeText)(int changeType, const u_int *szChar, u_short ctrlCode);	// 文字チェック
	u_char (*showHideSoftKB)(void);				// ソフトウェアキーボード
												// 0: 非表示
												// 1: 表示
	void (*closeSoftKB)(void);					// ソフトウェアキーボード閉じる
	void (*openSoftKB)(char flag);				// ソフトウェアキーボード開く

	int (*getNumlockState)(void);				// 現在NumLock状態か？
												// 0: 違う
												// 1: NumLock状態

	// 電源管理
	int (*powerLock)(void);
	int (*powerUnlock)(void);

	int (*getStrWidthSJIS)(const char *sjis, float fontratio);		// 戻り値 : 16倍された値

} fepImport_t;

typedef struct
{
	u_char	ucShowSKbd;							// ソフトウェアキーボード
												// 0: 非表示
												// 1: 表示

	u_char	ucShowHelp;							// ヘルプ表示中
												// 0: 非表示
												// 1: 表示

	u_int	uiListFontW, uiListFontH;			// リストの文字幅、高さ

	void  (*changeInputMode)(u_char);			// 入力モード変更
	void  (*abcToQwerty)(void);					// ABC <--> QWERTY
	int   (*getNowInputMode)(void);				// 現在の入力モードを取得

	// アプリ
	void (*exitOK)();														// 決定終了
	void (*exitCancel)();													// キャンセル終了	これもsetCharsで補える？
	void (*setChars)(const u_int *pUcs4, u_char Flag, u_short ctrlCode);	// 文字設定
																			// スペースキー、方向キー、削除キー含む
	void (*showHelp)(u_char flag);		// ヘルプ表示
	void (*keytopChange)(char keyNo);	// キートップ変更
	int (*getAtokInput)(void);			// ATOKのモード取得(入力があるか)
	u_int (*getAtokMode)(void);			// ATOKのモード取得(変換中か)
	void (*kakuteiAll)(void);			// 全文確定
	void (*kakuteiPart)(void);			// 部分確定
	int  (*opening)(void);				// オープン中
	int  (*closing)(void);				// クローズ中

	// 文字描画
	void (*setListFontEnv)(int x, int y);		// リスト時のフォント設定
	void (*setListDrawString)(sceVif1Packet *pk,  const char *pUtf8, int x, int y);	// リスト時の文字描画
	void (*setListDrawStringF)(sceVif1Packet *pk, const char *pUtf8, int x, int y);	// リスト時の文字描画(フォーカスあり)
	int  (*getStringWidth)(const char *pUtf8);		// 文字列の幅を取得

} skbImport_t;

typedef struct
{
	skbImport_t		skb;	// SOFTKB
	fepImport_t		fep;	// FEP
	txtFImport_t	txt;	// TEXT FIELD

	//----------------------
	// 共通部分
	u_char	ucUseAtok;							// ATOK
												// 0: 使わない
												// 1: 使う

	u_int	iRegion;							// リージョン
												// 0: Japan
												// 1: Us
												// 2: Europe

	u_short	usLang;								// 言語設定

	u_short	usSystemIni[ENV_MAX];				// OSDシステム設定
	u_int	uiScreenW, uiScreenH;				// スクリーン大きさ

	int   (*thinkYHn)(int v);					// YHを設定(1/1、NTSC/PAL考慮、FRAME/FIELD考慮)
	int   (*thinkYH16)(int v);					// YHを設定(16倍、NTSC/PAL考慮、FRAME/FIELD考慮)
	int   (*thinkXW16)(int v);					// XWを設定(16倍)
	int   (*thinkYH)(int v16);					// YHを設定(1/16、NTSC/PAL考慮、FRAME/FIELD考慮)
	int   (*thinkXW)(int v16);					// XWを設定(1/16)
	void  (*dprintf)(const char *msg, ...);		// デバッグ用PRINTF
	u_int (*getNtscPal)(void);					// 0:NTSC/1:PAL
	u_int (*getFrameField)();					// 0:Field/1:Frame
	int  (*isConv)(void);						// 変換中か？
												// 0: 違う
												// 1: 変換中

	// メモリ管理
	void* (*malign)(size_t align, size_t size);	// = memalign
	void  (*mfree)(void *p);					// = free

	// 音
	void (*se)(int soundNo);

	// 描画関連
	int  (*readTex)(textureInfo_t *pTex, const char *filename);		// テクスチャ読み込み
	void (*deleteTex)(textureInfo_t *pTex);							// テクスチャ破棄
	void (*loadTex)(sceVif1Packet *pk, textureInfo_t *pTex);		// テクスチャロード
	void (*drawTex)(sceVif1Packet *pk, u_int col, sprite_t *pSp, uv_t *pUv, textureInfo_t *pTex);	// テクスチャ描画
	void (*drawPane)(sceVif1Packet *pk, const IRECT *pRc, const RGBA32 c, int z);					// 板描画
	void (*drawLine)(sceVif1Packet *pk, int x, int y, int cx, int cy, const RGBA32 c, int z);		// ライン描画
	void (*drawFocus)(sceVif1Packet *pk, sprite_t *sp, u_int curPos, u_int col);					// フォーカス描画

} gImport_t;


//============================================================================
extern __inline__ u_int affectAlpha(u_int src, u_char alpha)
{
	u_char *pcol;
	u_char _alpha;
	u_int	col=0x80808080;

	pcol = (u_char*)&src;
	_alpha = (alpha * pcol[0])>>7;
	col = (u_int)_alpha;
	col <<= (6*4);
	col |= src & 0x00ffffff;

	return col;
}

#define ORIGINAL_PAL_RATIO	(1.19047619f)


#if 1//SKB_EX_Sound
enum
{
	// キートップ部分
	SKBSE_ONKEY = 0,	// キートップのキー押下
	SKBSE_BACKSPACE,	// BackSpace （□ボタン押下時、キートップ上のBackspace押下時）
	SKBSE_ENTER,		// Enter         （キートップ上のReturn押下時）
	SKBSE_SHIFT,		// Shift          （R1押下時、キートップ上のShift押下時）
	SKBSE_CARETMOVE,	// キャレット移動（R1L1押下時）
	SKBSE_OK,		// 決定          （START押下時）
	SKBSE_CANCEL,		// 戻り、キャンセル（×押下時）
	SKBSE_CHANGEKEYTOP,	// キートップ切替（SELECT押下時）
	SKBSE_CHANGEZENHAN,	// 半角全角切替
	SKBSE_ONHELP,		// ヘルプキー押下時
	SKBSE_QWERTYABC,	// QWERTY<-->ABC配列変更ボタン
	SKBSE_ONCAPSLOCK,	// CapsLockキー押下時
	SKBSE_ONSPACE,		// Spaceキー押下時

	// 日本語変換部分
	SKBSE_STARTTRANS,	// 変換開始（△ボタン押下時）
	SKBSE_MOVECANDLIST,	// 変換候補一覧内移動（↑↓ボタン押下時 または、△ボタン押下時）
	SKBSE_CANDDETECT,	// 変換候補決定（○ボタン押下時）
	SKBSE_MOVECLS,		// 変換文節移動（←→ボタン押下時）
	SKBSE_STRETCHCLS,	// 変換文節伸縮（R1＋←→ボタン押下時）
	SKBSE_DETECTEXIT,	// 確定終了（STARTボタン押下時）
					
	// 以下、編集部分
	SKBSE_INHIBITINPUT,	// 入力禁止文字が入力された時

	// IPアドレス入力時
	SKBSE_ONKEY_IP,         // キートップのキー押下
	SKBSE_BACKSPACE_IP,	// BackSpace （□ボタン押下時、キートップ上のBackspace押下時）
	SKBSE_ENTER_IP,		// Enter         （キートップ上のReturn押下時）
	SKBSE_CARETMOVE_IP,	// キャレット移動（R2L2押下時）
	SKBSE_OK_IP,		// 決定          （START押下時）
	SKBSE_CANCEL_IP,	// 戻り、キャンセル（×押下時）
	SKBSE_ONSPACE_IP,	// Spaceキー押下時

	// IPアドレス入力警告時
	SKBSE_INHIBITINPUT_IP,	        // 入力禁止文字が入力された時
	SKBSE_ONKEY2_IP,		// 数字入力時に256以上の場合,0に0入力の場合
	SKBSE_ONKEY3_IP,		// DELETEキー押下,空にする場合

	SKBSE_MAX
};
#endif


#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
}
#endif

#endif	// !__SHAREDDEF_H__
