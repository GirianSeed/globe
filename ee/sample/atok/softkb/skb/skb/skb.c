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
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <stdarg.h>

#include <eekernel.h>
#include <eetypes.h>
#include <sifdev.h>
#include <libpkt.h>
#include <libpad.h>

#include "env.h"
#include "shareddef.h"

#include "tim2wrap.h"
#include "libsoftkb.h"
#include "libfep.h"
#include "fontwrap.h"
#include "textField.h"
#include "fepbar.h"
#include "sysenv.h"

#include "skb.h"


//============================================================================
#define SOUND_ON    // 音あり

#define Printf   (0)? 0 : printf
#define PRINTF   (1)? 0 : printf
//============================================================================


typedef iRect_t RECT;

extern void SetClipOffset(int x,int y);
extern void HelpOffset( int x, int y );

//--------------------------------------------------------------------------------------
// libsoftkb.a fep.a textField のための、インポート関数
//--------------------------------------------------------------------------------------
extern void SetRectFill(sceVif1Packet *pk, const IRECT *pRc, const RGBA32 c, int z);
extern void SetClipArea(sceVif1Packet *pVif1Pk, const iRect_t *pRc);
extern void SetLine(sceVif1Packet *pk, int x, int y, int cx, int cy, const RGBA32 c, int z);
extern void SetRectFillEx(sceVif1Packet *pk, const IRECT *pRc, const RGBA32 *c, int z);

static void initColor(void);
static u_int getFrameField(void);
static void dprintf(const char *msg, ...);
static void se(int soundNo);
static u_int getNtscPal(void);
static int thinkYHn(int v);
static int thinkYH(int v16);
static int thinkXW(int v16);
static int thinkYH16(int v);
static int thinkXW16(int v);
static void ucsToUtf8(const u_int *ucs4, u_char *utf8);

static void setChars(const u_int *pUcs4, u_char Flag, u_short ctrlCode);
static void skb_exitOK(void);
static void showHelp(u_char flag);
static int  getNowInputMode(void);
static void openSoftKB(char flag);
static void somewhereFocus(sceVif1Packet *pk, sprite_t *sp, u_int curPos, u_int col);
static void changeInputMode(u_char uiMode);
static void keytopChange(char keyN);
static void exchangeABCtoQWERTY(void);
static void setSkbListFontEnv(int x, int y);
static void setSkbListDrawString(sceVif1Packet *pk, const char *pUtf8, int x, int y);
static void setSkbListDrawStringFocus(sceVif1Packet *pk, const char *pUtf8, int x, int y);
static int getStringWidthUtf8(const char *pUtf8);
u_char showHideSoftKB(void);
void closeSoftKB(void);
int opening(void);
int closing(void);


static u_int *getStrings(void);
static void drawStrings(sceVif1Packet *pk, const u_int *pUCS4, const iRect_t *pClip, const iRect_t *pRc);
static void setCharsNormalColor(void);
static void setSubCandListColor(void);
static void setUnderlineColor(void);
static void setNormalBGColor(void);
static void setBeforeChangBGColor(void);
static void setChangingBGColor(void);
static int drawStringBeforeChange(sceVif1Packet *pk, const u_int *pUCS4, const iRect_t *pClip, const iRect_t *pRc);
static int getStringWidthSJIS(const char *psjis, float ratio);
static int drawStringChanging(sceVif1Packet *pk, const u_int *pUCS4, const iRect_t *pClip, const iRect_t *pRc,
                              int *scrollLX16, int *scrollRX16, int frameW, int frameH, textField_t *pTf,
                              void drawCandListFrame(sceVif1Packet *pk, textField_t *pTf, iRect_t *rc16, int nCurrentCand));
static int getStringWidth(const u_int *pUCS4);

static int powerLock(void);
static int powerUnlock(void);
static void atokON(void);
static void atokOFF(sceAtokContext *pAtok);
static void exitOK();
static void cancel(void);
static void changeMode(int mode);
static int changeText(int changetype, const u_int *szChar, u_short ctrlCode);
int getToCaretPos(void);


//============================================================================

// スレッド
#define ATOK_THREAD_PRIORITY  (60)   // 非同期ATOKエンジンのスレッドプライオリティ

// パケットバッファ
#define PACKET_MAX  (1024*3*MAX_CHAR_BUFF_COUNT) // 文字列用パケットサイズ

// 文字の大きさ
#define APP_FONT_BIG				(1.0f)
#define APP_FONT_DEF				(0.9f)
#define APP_FONT_MDL				(0.8f)
#define APP_FONT_SML				(0.7f)

#define FONT_W						(22)
#define FONT_H						(23)


//============================================================================
// 可変変数群
//============================================================================
tSkb_Ctl     g_Sample;     // ソフトウェアキーボード動作用領域の実体
textField_t  textField;    // テキストフィールド

#define INHIBIT_CHAR_MAX    (500)                // 入力禁止文字列数
#define INHIBIT_SUB_CHAR_MAX (16)                // キー入力可の入力禁止文字
// 判定用バッファ
inhibitChar_t inhibitChar;                               // 入力禁止文字
u_int         inhibitUCS4[INHIBIT_CHAR_MAX][10];         // 入力禁止文字用バッファ
inhibitChar_t inhibitSubChar;                            // キー入力可の入力禁止文字
u_int         inhibitSubUCS4[INHIBIT_SUB_CHAR_MAX][10];  // キー入力可の入力禁止文字用バッファ

// ワークバッファ
inhibitChar_t EnableChar;                                // 入力許可文字禁止文字へ変換前のワーク
u_int         EnableUCS4[INHIBIT_CHAR_MAX][10];          // 入力許可文字バッファ
inhibitChar_t inhibitMaskChar;                           // キー入力可の入力禁止文字 データベース
u_int         inhibitMaskUCS4[INHIBIT_SUB_CHAR_MAX][10]; // キー入力可の入力禁止文字用バッファ

//------------------ インターフェース --------------------------------------------------
gImport_t g_import;     // インポート用関数ポインタ用
gImport_t g_importFep;  // インポート用関数ポインタ用(FEP専用)


//------------------ 描画パケット     --------------------------------------------------
static sceVif1Packet g_pkVif1[2];    // パケットの管理情報
static u_int         g_pkVif1_ID=0;  // パケットの管理情報
static u_char g_pkVif1Buff[2][PACKET_MAX] __attribute__((aligned(128)));	// パケットの管理情報

//------------------ USB-KEYBOARD InterFace    --------------------------------------------------
int         KbStat;     // USB-KEYBOARD制御用
USBKBDATA_t KbData;     // USB-KEYBOARD制御用
int         KbNumLock;  // USB-KEYBOARD制御用



// 記号一覧
static charCodeBlock_t g_ListItems[] =
{
    // 半角文字
//    { 0x0020, 0x007f },
//    { 0x00a0, 0x00df },

    // 全角文字
    { 0x8140, 0x81ff },
    { 0x8240, 0x82ff },
    { 0x8340, 0x83df },
    { 0x8440, 0x84bf },

    { 0x8740, 0x879f },

    { 0x8890, 0x88ff },
    { 0x8940, 0x89ff },
    { 0x8a40, 0x8aff },
    { 0x8b40, 0x8bff },
    { 0x8c40, 0x8cff },
    { 0x8d40, 0x8dff },
    { 0x8e40, 0x8eff },
    { 0x8f40, 0x8fff },

    { 0x9040, 0x90ff },
    { 0x9140, 0x91ff },
    { 0x9240, 0x92ff },
    { 0x9340, 0x93ff },
    { 0x9440, 0x94ff },
    { 0x9540, 0x95ff },
    { 0x9640, 0x96ff },
    { 0x9740, 0x97ff },
    { 0x9840, 0x98ff },
    { 0x9940, 0x99ff },
    { 0x9a40, 0x9aff },
    { 0x9b40, 0x9bff },
    { 0x9c40, 0x9cff },
    { 0x9d40, 0x9dff },
    { 0x9e40, 0x9eff },
    { 0x9f40, 0x9fff },

    { 0xe040, 0xe0ff },
    { 0xe140, 0xe1ff },
    { 0xe240, 0xe2ff },
    { 0xe340, 0xe3ff },
    { 0xe440, 0xe4ff },
    { 0xe540, 0xe5ff },
    { 0xe640, 0xe6ff },
    { 0xe740, 0xe7ff },
    { 0xe840, 0xe8ff },
    { 0xe940, 0xe9ff },
    { 0xea40, 0xeaaf },

//    フォントに入っていない
//    { 0xed40, 0xedff },
//    { 0xee40, 0xeeff },
//
//    { 0xfa40, 0xfaff },
//    { 0xfb40, 0xfbff },
//    { 0xfc40, 0xfc4f },

	{ 0x0000, 0x0000 }		// END CODE
};


//======================================================================================
//
//     tSkb_Ctl g_Sample;  制御関数
//
//======================================================================================

//--------- 色設定 -------------------
u_char g_FontColSrc[][4] =        /* initColor() で、g_Sampleに、コピーされる */
{
	{0x80,	0x80,	0x80,	0x80},/* FONT_COL_NORMAL      */ // 通常 文字(白)
	{0x28,	0x28,	0x28,	0x80},/* FONT_COL_CANDLIST_SU */ // 候補一覧の漢字意外の文字
	{0x70,	0x70,	0xff,	0xff},/* FONT_COL_UNDERLINE   */ // アンダーライン色設定
	{0x50,	0x50,	0xc8,	0x80},/* FONT_COL_FOCUS       */ // ソフトウェアキーボードリスト時のカーソルがあるときの色
	{0x60,	0x60,	0x60,	0x80},/* FONT_COL_LIST        */ // ソフトウェアキーボードリスト時の通常色
};

u_char g_FontBGColSrc[][16] =
{
	{	// 通常時 (透明)
		0x80,	0x80,	0x80,	0x00,	0x80,	0x80,	0x80,	0x00,
		0x80,	0x80,	0x80,	0x00,	0x80,	0x80,	0x80,	0x00,
	},
	{	// 変換前
		0x1f,	0x1f,	0x1f,	0x80,	0x1f,	0x1f,	0x1f,	0x80,
		0x50,	0x50,	0x78,	0x80,	0x50,	0x50,	0x78,	0x80,
	},
	{	// 変換中背景
		0x1f,	0x1f,	0x1f,	0x80,	0x1f,	0x1f,	0x1f,	0x80,
		0x6c,	0xe8,	0x88,	0x80,	0x6c,	0xe8,	0x88,	0x80,
	},
	{	// 候補一覧中注目
		0x1f,	0x1f,	0x1f,	0x80,	0x1f,	0x1f,	0x1f,	0x80,
		0x6c,	0xe8,	0x88,	0x80,	0x6c,	0xe8,	0x88,	0x80,
	},
};

// 色情報初期化
static void initColor(void)
{
	int i;
	for(i=0; i<FONT_COL_MAX; i++)
	{
		memcpy(&g_Sample.aucFontCol[i][0],&g_FontColSrc[i][0], sizeof(u_char)*4);
	}

	for(i=0; i<FONT_BGCOL_MAX; i++)
	{
		memcpy(&g_Sample.aucFontBGCol[i][0],&g_FontBGColSrc[i][0], sizeof(u_char)*16);
	}
}

// 文字 通常色 設定
static void setCharsNormalColor(void)
{
	Font_SetColor(g_Sample.aucFontCol[FONT_COL_NORMAL][0], g_Sample.aucFontCol[FONT_COL_NORMAL][1], g_Sample.aucFontCol[FONT_COL_NORMAL][2], g_Sample.aucFontCol[FONT_COL_NORMAL][3]);
}

// 候補一覧中の漢字意外の文字色設定
static void setSubCandListColor(void)
{
	Font_SetColor(g_Sample.aucFontCol[FONT_COL_CANDLIST_SUB][0], g_Sample.aucFontCol[FONT_COL_CANDLIST_SUB][1], g_Sample.aucFontCol[FONT_COL_CANDLIST_SUB][2], g_Sample.aucFontCol[FONT_COL_CANDLIST_SUB][3]);
}

// 下線色設定
static void setUnderlineColor(void)
{
	Font_SetULColor(g_Sample.aucFontCol[FONT_COL_UNDERLINE][0], g_Sample.aucFontCol[FONT_COL_UNDERLINE][1], g_Sample.aucFontCol[FONT_COL_UNDERLINE][2], g_Sample.aucFontCol[FONT_COL_UNDERLINE][3]);
}


// 文字背景色 設定
static void setNormalBGColor(void)
{
	Font_SetBGColorEx(0, g_Sample.aucFontBGCol[FONT_BGCOL_NORMAL][0], g_Sample.aucFontBGCol[FONT_BGCOL_NORMAL][1], g_Sample.aucFontBGCol[FONT_BGCOL_NORMAL][2], g_Sample.aucFontBGCol[FONT_BGCOL_NORMAL][3]);
	Font_SetBGColorEx(1, g_Sample.aucFontBGCol[FONT_BGCOL_NORMAL][4], g_Sample.aucFontBGCol[FONT_BGCOL_NORMAL][5], g_Sample.aucFontBGCol[FONT_BGCOL_NORMAL][6], g_Sample.aucFontBGCol[FONT_BGCOL_NORMAL][7]);
	Font_SetBGColorEx(2, g_Sample.aucFontBGCol[FONT_BGCOL_NORMAL][8], g_Sample.aucFontBGCol[FONT_BGCOL_NORMAL][9], g_Sample.aucFontBGCol[FONT_BGCOL_NORMAL][10], g_Sample.aucFontBGCol[FONT_BGCOL_NORMAL][11]);
	Font_SetBGColorEx(3, g_Sample.aucFontBGCol[FONT_BGCOL_NORMAL][12], g_Sample.aucFontBGCol[FONT_BGCOL_NORMAL][13], g_Sample.aucFontBGCol[FONT_BGCOL_NORMAL][14], g_Sample.aucFontBGCol[FONT_BGCOL_NORMAL][15]);
}

// 文字背景 変換前 設定
static void setBeforeChangBGColor(void)
{
	Font_SetBGColorEx(0, g_Sample.aucFontBGCol[FONT_BGCOL_BEFORECHANGE][0], g_Sample.aucFontBGCol[FONT_BGCOL_BEFORECHANGE][1], g_Sample.aucFontBGCol[FONT_BGCOL_BEFORECHANGE][2], g_Sample.aucFontBGCol[FONT_BGCOL_BEFORECHANGE][3]);
	Font_SetBGColorEx(1, g_Sample.aucFontBGCol[FONT_BGCOL_BEFORECHANGE][4], g_Sample.aucFontBGCol[FONT_BGCOL_BEFORECHANGE][5], g_Sample.aucFontBGCol[FONT_BGCOL_BEFORECHANGE][6], g_Sample.aucFontBGCol[FONT_BGCOL_BEFORECHANGE][7]);
	Font_SetBGColorEx(2, g_Sample.aucFontBGCol[FONT_BGCOL_BEFORECHANGE][8], g_Sample.aucFontBGCol[FONT_BGCOL_BEFORECHANGE][9], g_Sample.aucFontBGCol[FONT_BGCOL_BEFORECHANGE][10], g_Sample.aucFontBGCol[FONT_BGCOL_BEFORECHANGE][11]);
	Font_SetBGColorEx(3, g_Sample.aucFontBGCol[FONT_BGCOL_BEFORECHANGE][12], g_Sample.aucFontBGCol[FONT_BGCOL_BEFORECHANGE][13], g_Sample.aucFontBGCol[FONT_BGCOL_BEFORECHANGE][14], g_Sample.aucFontBGCol[FONT_BGCOL_BEFORECHANGE][15]);
}

// 文字背景 変換中 設定
static void setChangingBGColor(void)
{
	Font_SetBGColorEx(0, g_Sample.aucFontBGCol[FONT_BGCOL_CHANGING][0], g_Sample.aucFontBGCol[FONT_BGCOL_CHANGING][1], g_Sample.aucFontBGCol[FONT_BGCOL_CHANGING][2], g_Sample.aucFontBGCol[FONT_BGCOL_CHANGING][3]);
	Font_SetBGColorEx(1, g_Sample.aucFontBGCol[FONT_BGCOL_CHANGING][4], g_Sample.aucFontBGCol[FONT_BGCOL_CHANGING][5], g_Sample.aucFontBGCol[FONT_BGCOL_CHANGING][6], g_Sample.aucFontBGCol[FONT_BGCOL_CHANGING][7]);
	Font_SetBGColorEx(2, g_Sample.aucFontBGCol[FONT_BGCOL_CHANGING][8], g_Sample.aucFontBGCol[FONT_BGCOL_CHANGING][9], g_Sample.aucFontBGCol[FONT_BGCOL_CHANGING][10], g_Sample.aucFontBGCol[FONT_BGCOL_CHANGING][11]);
	Font_SetBGColorEx(3, g_Sample.aucFontBGCol[FONT_BGCOL_CHANGING][12], g_Sample.aucFontBGCol[FONT_BGCOL_CHANGING][13], g_Sample.aucFontBGCol[FONT_BGCOL_CHANGING][14], g_Sample.aucFontBGCol[FONT_BGCOL_CHANGING][15]);
}

//--------- キーボード ABC配列_QWERTY配列 入れ替え -------------------
static int swap_ABC_QWERTY( int notFoundSetType )
{
	int n;

	Printf("swap checking...\n");
	if( g_import.iRegion != REGION_J )	return 0;	// 日本版のみ動作します。

	for( n=0; n<SKBTYPE_MAX; n++ ){
		if(g_Sample.auiNextKeyTop[n] == SKBTYPE_QWERTY ||
		   g_Sample.auiNextKeyTop[n] == SKBTYPE_ABC      )	break;
	}
	if( n == SKBTYPE_MAX )	return 0;               // 見つからなかったので入れ替え無し

	if( g_Sample.modeBarNum[SKBTYPE_QWERTY] < 0 )	return 0; // 見つからなかったので入れ替え無し
	if( g_Sample.modeBarNum[SKBTYPE_ABC   ] < 0 )	return 0; // 見つからなかったので入れ替え無し

	if( g_Sample.uiNowKeyType == SKBTYPE_ABC ){
		g_Sample.uiNowKeyType = SKBTYPE_QWERTY;    // 現在のキーボードが入れ替え対象だったら、アクティブキーボード変更
	}
	else if( g_Sample.uiNowKeyType == SKBTYPE_QWERTY ){
		g_Sample.uiNowKeyType = SKBTYPE_ABC;      // 現在のキーボードが入れ替え対象だったら、アクティブキーボード変更 
	}
	else{ // 現在のキーボードがABC QWERTY どちらでもなかったら 引数に従う。
		if( notFoundSetType >= 0 && notFoundSetType < SKBTYPE_MAX ){
			if( g_Sample.modeBarNum[notFoundSetType] < 0 )	return 0;	//引数が不正だったら無し

			g_Sample.uiNowKeyType = notFoundSetType;
			// 引数がABC QWERTY どちらでもなかったら、
			if( g_Sample.uiNowKeyType != SKBTYPE_ABC && g_Sample.uiNowKeyType != SKBTYPE_QWERTY ){
				return -1; //入れ替えは無かったが、他のキーボードが変更された。
			}
		}
	}

	Printf("swap SKBTYPE_QWERTY<->SKBTYPE_ABC \n");

	for( n=0; n<SKBTYPE_MAX; n++ ){
		if( g_Sample.auiNextKeyTop[n] == SKBTYPE_QWERTY ){
			g_Sample.auiNextKeyTop[n] = SKBTYPE_ABC;
		}
		else if( g_Sample.auiNextKeyTop[n] == SKBTYPE_ABC ){
			g_Sample.auiNextKeyTop[n] = SKBTYPE_QWERTY;
		}
	}

	return 1; //キーボードを入れ替えた。
}

//-------- 入力禁止文字の設定 Open時に有効化-------------------
static void inhibitChar_SetAllKeyborad()
{
	int i;
	skey_t *pKey;

	for(i=0; i < SKBTYPE_MAX; i++){
		pKey = &(g_Sample.KBD[i]);
		pKey->pInhibitChar = &inhibitChar;
		#if 0 // オープン時に設定。
		setEnableChars(pKey);                  // 全てのキーを入力可に戻す。
		setInhibitChars( pKey, &inhibitChar ); // 禁止文字列をキーに設定
		#endif
	}
}

// usbkeyborad 時用、フェプ入力モード変更処理
static void changeInputMode_usbkb()
{
	int mode   = SoftKB_GetInputMode( &g_Sample.KBD[g_Sample.uiNowKeyType] );
	int modekb = g_Sample.inputMode[ g_Sample.uiNowKeyType ];

	switch( mode ){
	case SKBMODE_HIRAGANA:
	                        if( modekb==FEPOFF ) mode = FEPON_HIRA_FIX;
	                        else                 mode = FEPON_HIRA;
	                        break;
	case SKBMODE_HAN_KATAKANA:
	                        if( modekb==FEPOFF ) mode = FEPON_HAN_KATAKANA_FIX;
	                        else                 mode = FEPON_HAN_KATAKANA;
	                        break;
	case SKBMODE_ZEN_KATAKANA:
	                        if( modekb==FEPOFF ) mode = FEPON_ZEN_KATAKANA_FIX;
	                        else                 mode = FEPON_ZEN_KATAKANA;
	                        break;
	case SKBMODE_HAN_ALPHABET:         // ALPHABET_FIXモードは、変換時カナがでるので使用しない
	                        if( modekb==FEPOFF ) mode = FEPOFF;//mode = FEPON_HAN_ALPHABET_FIX;
	                        else                 mode = FEPON_HAN_ALPHABET;
	                        break;
	case SKBMODE_ZEN_ALPHABET:
	                        if( modekb==FEPOFF ) mode = FEPOFF;//mode = FEPON_ZEN_ALPHABET_FIX;
	                        else                 mode = FEPON_ZEN_ALPHABET;
	                        break;
	}

	Fep_SetInputMode(mode);
	Printf("changeInputMode_usbkb %x\n",mode);
}


//============================================================================
// テキスト
//============================================================================
// 文字コードを変更(UCS2->UTF8)
static void ucsToUtf8(const u_int *ucs4, u_char *utf8)
{
	sceCccUTF8** dst = (sceCccUTF8**)&utf8;
	int i=0;

	SKBSMPL_ASSERT(ucs4);
	SKBSMPL_ASSERT(utf8);

	while(*(ucs4+i))
	{
		sceCccEncodeUTF8(dst, *(ucs4+i));
		i++;
	}
	*(*dst) = 0;
}



//============================================================================
// g_import 共通部 登録関数
//============================================================================
// デバッグ用出力
static void dprintf(const char *msg, ...)
{
	#ifdef DEBUG
	va_list argptr;
	char    text[1024];

	va_start(argptr, msg);
	vsprintf(text, msg, argptr);
	va_end(argptr);

	printf("%s", text);
	#endif
}

// フレームモード/フィールドモード取得
static u_int getFrameField(void)
{
	#if FIELD_MODE
	return 0;
	#else
	return 1;
	#endif
}

// NTSC/PAL取得
static u_int getNtscPal(void)
{
	// NTSC
	return 0;
}

// 効果音
static void se(int soundNo)
{
	#ifdef SOUND_ON
	if( g_Sample.Message ){
		g_Sample.Message( SKBMES_SE, (void*)soundNo );
	}
	#endif
}

//g_import.readTex   = Tim2_ReadTim2;      tim2wrap.c
//g_import.deleteTex = Tim2_DeleteTim2;    tim2wrap.c
//g_import.loadTex   = Tim2_SendTim2;      tim2wrap.c
//g_import.drawTex   = Tim2_DrawSprite;    tim2wrap.c
//g_import.drawPane  = SetRectFill;        draw.c
//g_import.drawLine  = SetLine;            draw.c

// Y方向の座標計算
static int thinkYHn(int v)
{
	return (int)(((float)v*VRESOLUTION_RATIO)*TV_RATIO);
}

// Y方向の座標計算
static int thinkYH(int v16)
{
	return (((int)(((float)v16/VRESOLUTION_RATIO)/TV_RATIO)) >> 4);
}

// X方向の座標計算
static int thinkXW(int v16)
{
	return v16 >> 4;
}

// Y方向の座標計算
static int thinkYH16(int v)
{
#if PAL_MODE
	return (((int)(((float)(v << 4))*(VRESOLUTION_RATIO*TV_RATIO))));
#else
	// NTSC
	#if FIELD_MODE
		return (v << 4);
	#else
		return (v << 3);
	#endif
#endif
}

// X方向の座標計算
static int thinkXW16(int v)
{
	return (v << 4);
}

// SOFTKB フォーカス設定
static void somewhereFocus(sceVif1Packet *pk, sprite_t *sp, u_int curPos, u_int col)
{
	SoftKB_DrawFocus(pk, sp, curPos, col);
}

//g_import.isConv    = Fep_GetNowConv;    libFep.a



//============================================================================
// g_import.skb ソフトウェアキーボード部 登録関数
//============================================================================

// SOFTKBのモード設定からFEP側のモード設定に変更する為の関数
static void changeInputMode(u_char uiMode)
{
	fepAtokInputMode_m mode = FEPOFF;

	mode = g_Sample.inputMode[g_Sample.uiNowKeyType];
	if( mode != FEPOFF ){
		switch(uiMode)
		{
		case SKBMODE_HIRAGANA:
			mode = FEPON_HIRA;
			break;
		case SKBMODE_HAN_KATAKANA:
			mode = FEPON_HAN_KATAKANA;
			break;
		case SKBMODE_ZEN_KATAKANA:
			mode = FEPON_ZEN_KATAKANA;
			break;
		case SKBMODE_HAN_ALPHABET:
			mode = FEPOFF;
			break;
		case SKBMODE_ZEN_ALPHABET:
			mode = FEPOFF;
		break;
		}
	}
	if( g_Sample.Message){
		if( mode != FEPOFF ){
			g_Sample.Message( SKBMES_FEP_ON , (void*)0 );
		}
		else{
			g_Sample.Message( SKBMES_FEP_OFF, (void*)0 );
		}
	}

	// 入力モード設定
	if(Fep_SetInputMode(mode))
	{
		printf("ERROR : changeInputMode - non support mode.");
	}
	Printf("MSG : changeInputMode uiMode[%d],mode[%d]\n",uiMode,mode);
}


// ABC配列とQWERTY配列を入れ替える
static void exchangeABCtoQWERTY(void)
{
	u_char ucInputMode;
	skey_t *pCloseKey = &g_Sample.KBD[g_Sample.uiNowKeyType]; //exchange する前に保存。
	skey_t *pKey = NULL;

	if(! swap_ABC_QWERTY(SKBTYPE_QWERTY) )	return;

	// 入れ替えたら、ABCかQWERTYかのモードを環境設定 
	g_Sample.Env.alphabetKeyType ^= 1;

	SoftKB_Close( pCloseKey );// 現在のキーボードClose

	pKey = &g_Sample.KBD[g_Sample.uiNowKeyType];
	ucInputMode = SoftKB_GetInputMode(pKey);        // 
	changeInputMode(ucInputMode);                   // 入力モード変更

	SoftKB_Open(&g_Sample.KBD[g_Sample.uiNowKeyType]); // 新しいキーボードOpen
}


// このコールバックは、キーボード種類をあらわすモードバーが
// 何番目（クリエイト順）になっているか返すようにする。
static int getNowInputMode(void)
{
	return g_Sample.modeBarNum[ g_Sample.uiNowKeyType ];
}


// g_import.skb.getAtokInput= Fep_GetNowAtokInput;  libfep.a
// g_import.skb.getAtokMode = Fep_Changing;         libfep.a
// g_import.skb.kakuteiPart = Fep_SetKakuteiPart;   libfep.a
// g_import.skb.kakuteiAll  = Fep_SetKakuteiAll;    libfep.a


// 文字コード(UCS4)をSOFTKB->FEPに引渡し。
static void setChars(const u_int *pUcs4, u_char Flag, u_short ctrlCode)
{
	static u_int uiTmp[10];

	if(pUcs4 != NULL)
	{
		memcpy(uiTmp, pUcs4, sizeof(u_int)*10);
		Fep_PostSoftKey(uiTmp, Flag, ctrlCode); // FEPに文字登録
	}
	else if(ctrlCode != 0xFFFFFFFF)
	{
		memset(uiTmp, 0x0, sizeof(u_int)*10);
		Fep_PostSoftKey(uiTmp, Flag, ctrlCode); // FEPに文字登録
	}
}


// g_import.skb.exitOK =
static void skb_exitOK()
{
	Printf("MSG : skb_exitOK\n");
}


// ヘルプ表示
// u_char flag : 0 : 非表示
//               1 : 表示
static void showHelp(u_char flag)
{
	g_Sample.iCoverView = flag;
	g_import.skb.ucShowHelp = flag;
	if( g_Sample.Message ){
		if(flag)	g_Sample.Message( SKBMES_HELP_SHOW,(void*)0 );
		else    	g_Sample.Message( SKBMES_HELP_HIDE,(void*)0 );
	}
	Printf("MSG : showHelp - %d\n", flag);
}


// キートップの変更
// char keyNo : -1 : 次のキートップ
//            : その他 : 直接キートップを設定
static void keytopChange(char keyNo)
{
	u_char ucInputMode;
	skey_t *pKey = NULL,*pOldKey;

	if( g_Sample.auiNextKeyTop[g_Sample.uiNowKeyType] == g_Sample.uiNowKeyType ){
		return;
	}

	SoftKB_Close(&g_Sample.KBD[g_Sample.uiNowKeyType]);// 現在のキーボードClose
	pOldKey = &g_Sample.KBD[g_Sample.uiNowKeyType];

	if(keyNo == SKBMODE_NOHIT)
	{
		// 次のキートップ
		g_Sample.uiNowKeyType = g_Sample.auiNextKeyTop[g_Sample.uiNowKeyType];
		PRINTF("%d, %d\n", g_Sample.uiNowKeyType, g_Sample.auiNextKeyTop[g_Sample.uiNowKeyType]);
	}
	else
	{
		int i;
		if(keyNo < 0)
		{
			keyNo += (SKBTYPE_MAX-1); // 左方向に移動
		}

		for(i=0; i<keyNo; i++)
		{
			g_Sample.uiNowKeyType = g_Sample.auiNextKeyTop[g_Sample.uiNowKeyType];	// 右方向に移動
		}
	}

	pKey = &g_Sample.KBD[g_Sample.uiNowKeyType];
	ucInputMode = SoftKB_GetInputMode(pKey);
	changeInputMode(ucInputMode);                // 入力モード変更
	//pKey->uiCursorPos = SoftKB_SearchFocus(pOldKey, pKey);

	SoftKB_Open(&g_Sample.KBD[g_Sample.uiNowKeyType]); // 新しいキーボードOpen
}

// オープン中の呼ばれる関数。アニメーション挿入などに使用。
// g_import.skb.opening
int opening(void)
{
	Printf("skb.opening\n");
	return 1;
}

// クローズ中に呼ばれる関数。アニメーション挿入などに使用。
// g_import.skb.close
int closing(void)
{
	Printf("skb.closing\n");
	return 1;
}

// ソフトウェアキーボードリスト
// フォント環境設定
static void setSkbListFontEnv(int x, int y)
{
	Font_SetRatio(APP_FONT_MDL);    // フォントの大きさ設定
	Font_SetLocate(x, y);           // 位置設定
	Font_SetEffect(FONT_EFFECT_BG); // 効果設定
	setCharsNormalColor();          // 通常文字色設定
	setNormalBGColor();             // 通常背景色設定
}

// 通常文字描画
static void setSkbListDrawString(sceVif1Packet *pk, const char *pUtf8, int x, int y)
{
	Font_SetColor(g_Sample.aucFontCol[FONT_COL_LIST][0], g_Sample.aucFontCol[FONT_COL_LIST][1], g_Sample.aucFontCol[FONT_COL_LIST][2], g_Sample.aucFontCol[FONT_COL_LIST][3]);
	Font_SetLocate(x, y);                   // 位置設定
	Font_PutsPacket(pUtf8, pk, PACKET_MAX); // パケット作成
}
// カーソルがあたっている時の文字描画
static void setSkbListDrawStringFocus(sceVif1Packet *pk, const char *pUtf8, int x, int y)
{
	Font_SetColor(g_Sample.aucFontCol[FONT_COL_FOCUS][0], g_Sample.aucFontCol[FONT_COL_FOCUS][1], g_Sample.aucFontCol[FONT_COL_FOCUS][2], g_Sample.aucFontCol[FONT_COL_FOCUS][3]);
	Font_SetLocate(x, y);                   // 位置設定
	Font_PutsPacket(pUtf8, pk, PACKET_MAX); // パケット作成
}

// 文字列の幅を返す
static int getStringWidthUtf8(const char *pUtf8)
{
	Font_SetRatio(APP_FONT_DEF);
	return Font_GetStrLength(pUtf8);
}



//============================================================================
// g_import Ｆｅｐ部 登録関数
//============================================================================
// ATOKがONになった(漢字入力モードON) 全角半角キーで
static void atokON(void)
{
	if( g_Sample.fepModeOnState!=-1 ){
		Fep_SetInputMode( g_Sample.fepModeOnState );
		g_Sample.fepModeOnState=-1;
	}
	else Fep_SetInputMode( FEPOFF );

	Printf("MSG : atokON - fepModeOnState %d\n",g_Sample.fepModeOnState);
}

// ATOKがOFFになった(漢字入力モードOFF)
static void atokOFF(sceAtokContext *pAtok)
{
	// fepbarを編集
	g_Sample.fepModeOnState = Fepbar_GetNowMode();

	Printf("MSG : atokOFF\n");
}

// モードが変更した
static void changeMode(int mode)
{
	if(mode == -1){
		Fepbar_NextMode();        // AtokModeChange
	}else{
		Fepbar_SetModeType(mode);
	}
	Printf("MSG : changeModex[0x%x]\n",mode);
}


static int changeText(int changetype, const u_int *szChar, u_short ctrlCode)
{
	int i=0, j=0;
	u_int num  = inhibitChar.uiCharNum; // 指定されている禁止文字数
	u_int numc = inhibitChar.uiCKeyNum; // 指定されている禁止コントロール数

	if(szChar == NULL && ctrlCode == 0)
	{
		// 入力無かった時
		return FEP_EDIT_NG;
	}

	if(szChar != NULL)
	{
		// 入力禁止文字チェック
		// 禁止文字が単一文字
		for(i=0; i<num; i++)
		{
			j=0;
			#if 1//SKB_EX inhibitChar
			if( inhibitChar.pChar[i][1] && inhibitChar.pChar[i][1]!=0 )	continue;
			while(*(szChar+j))
			{
				// 複数文字サポート
				if(inhibitChar.pChar[i][0] == *(szChar+j))
				{
					return FEP_EDIT_NG; // 入力禁止 エラー
				}
				j++;
			}
			#else
			while(*(szChar+j))
			{
				// 複数文字サポート
				if(inhibitChar.pChar[i][0] == *(szChar+j))
				{
					return FEP_EDIT_NG; // 入力禁止 エラー
				}
				j++;
			}
			// 文字列内の１文字をチェック
			#endif
		}
		#if 1//SKB_EX inhibitSub
		for(i=0; i<inhibitSubChar.uiCharNum; i++){
			j=0;
			while(*(szChar+j))
			{
				if(inhibitSubChar.pChar[i][0] == *(szChar+j))
				{
					return FEP_EDIT_NG; // 入力禁止 エラー
				}
				j++;
			}
		}
		#endif
	}
	else
	{
		// 入力禁止コードチェック
		for(i=0; i<numc; i++)
		{
			// コントロールコードでチェック
			if(ctrlCode == inhibitChar.pCtrlKey[i])
			{
				return FEP_EDIT_NG; // 入力禁止 エラー
			}
		}
	}

	// 限界文字数チェック
	{
		char utf8Buf[1024];

		ucsToUtf8(g_Sample.uiChars, utf8Buf); // 確定済みの文字コード変換 UCS4 -> UTF8
		i = strlen(utf8Buf);
		if(changetype == FEP_EDIT_ADD)
		{
			// 挿入処理
			ucsToUtf8(szChar, utf8Buf);       // 挿入文字コード変換 UCS4 -> UTF8
			i += strlen(utf8Buf);

			if(g_import.fep.uiMaxTextLen < i)
			{
				return FEP_EDIT_OVER; // バッファオーバーエラー
			}
		}
		else if(changetype == FEP_EDIT_INSERT)
		{
			// 上書き処理
			char utf8BufOverWrite[1024];
			const u_int *pw = NULL;

			ucsToUtf8(szChar, utf8Buf);       // 上書き文字コード変換 UCS4 -> UTF8
			i += strlen(utf8Buf);             // 今回上書きしたい文字数取得

			pw = Fep_GetInsertChars();        // 上書きされる文字列へのポインタ取得
			ucsToUtf8(pw, utf8BufOverWrite);  // 上書き文字コード変換 UCS4 -> UTF8
			i -= strlen(utf8BufOverWrite);

			if(g_import.fep.uiMaxTextLen < i)
			{
				return FEP_EDIT_OVER; // バッファオーバーエラー
			}
		}
	}

	return FEP_EDIT_OK;
}


// 電源ロック
static int powerLock(void)
{
	Printf("Power Lock\n");
	return 1;
}

// 電源アンロック
static int powerUnlock(void)
{
	Printf("Power UnLock\n");
	return 0;
}

// 終了処理
// pFep->ok
static void exitOK()
{
	Printf("MSG : exitOK\n");

	if( g_Sample.iOpenFlag > 0 ){  // 0:未Open 1:Open中
		g_Sample.iOpenFlag = -1;   // 0:未Open 1:Open中
		// 編集結果をコールバック関数で伝える。
		if( g_Sample.Message )	g_Sample.Message( SKBMES_RESULT, (void*)g_Sample.uiChars );
		return;
	}
}

// キャンセル
void cancel(void)
{
	Printf("MSG : cancel\n");

	if( g_Sample.iOpenFlag > 0 ){  // 0:未Open 1:Open中
		g_Sample.iOpenFlag = -1;   // 0:未Open 1:Open中
		// 編集結果をコールバック関数で伝える。(cancelは、テキストはＮＵＬＬ)
		if( g_Sample.Message )	g_Sample.Message( SKBMES_RESULT, (void*)0 );
	}
}

// USBキーボードのＮｕｍＬｏｃｋ取得。
int getNumlockState()
{
	return KbNumLock;
}

// ソフトウェアキーボードの表示/非表示を取得
u_char showHideSoftKB(void)
{
	return g_import.skb.ucShowSKbd;
}

// ソフトウェアキーボードの非表示
void closeSoftKB(void)
{
	Printf("closeSoftKB\n");

	g_import.skb.ucShowSKbd = 0;	// ソフトウェアキーボード非表示
	g_import.skb.ucShowHelp = 0;	// ヘルプ非表示
}

// ソフトウェアキーボードを表示
// char flag : 0 : 単にオープン
//             1 : ソフトウェアキーボードの遷移サイクルを初期化
static void openSoftKB(char flag)
{
	Printf("openSoftKB %d\n",flag);
	g_import.skb.ucShowSKbd = 1;

	#if 1//SKB_EX Insert 
	Fep_SetInsertMode(FEP_EDIT_INSERT); // ソフトウェアキーボードは挿入モード
	#endif

	if(flag)
	{
		skey_t *pKey, *pOldKey;
		u_char ucInputMode;

		pOldKey = &g_Sample.KBD[g_Sample.uiNowKeyType];
		pKey = &g_Sample.KBD[g_Sample.uiNowKeyType];

		ucInputMode = SoftKB_GetInputMode(pKey); // 全角/半角モードか取得
		SoftKB_Open(pKey);                       // 新しいキーボードOpen
		changeInputMode(ucInputMode);            // 入力モード変更
		pKey->uiCursorPos = SoftKB_SearchFocus(pOldKey, pKey);
	}

	{
		fepAtokInputMode_m mode = g_Sample.inputMode[g_Sample.uiNowKeyType];

		//Ｆｅｐの入力モード指定（キーボードによって入力途中の文字列があったら終了される）
		Fep_SetInputMode(mode);	
		if( g_Sample.Message ){
			if( mode != FEPOFF )	g_Sample.Message( SKBMES_FEP_ON , (void*)0 );
			else                	g_Sample.Message( SKBMES_FEP_OFF, (void*)0 );
		}
	}

	g_Sample.iCoverView = 0; // text box 表示
}

// 文字列の幅を取得
// SJISコードの文字列をUTF8に変換して
static int getStringWidthSJIS(const char *psjis, float ratio)
{
	char buf[2048];

	Font_SetRatio(ratio);
	sceCccSJIStoUTF8((sceCccUTF8*)buf, HENKAN_SIZE, (sceCccSJISTF*)psjis);

	return g_import.thinkXW16(Font_GetStrLength(buf));
}



//============================================================================
// g_import.txt テキストフィールド部、登録関数
//============================================================================

//g_import.txt.uiCharHeight = Font_GetHeight();  // fontwrap.a

// 文字列取得(UCS4)
static u_int *getStrings(void)
{
	return g_Sample.uiChars;
}

// 文字列描画
static void drawStrings(sceVif1Packet *pk, const u_int *pUCS4, const iRect_t *pClip, const iRect_t *pRc)
{
	char buf[2048];

	if(pUCS4 == NULL)
	{
		return;
	}

	ucsToUtf8(pUCS4, buf);       // UCS4->UTF8に変換

	Font_SetRatio(APP_FONT_DEF);                       // フォントの大きさ設定
	Font_SetLocate(pRc->x, g_import.thinkYHn(pRc->y)); // 位置設定
	Font_SetEffect(FONT_EFFECT_BG);                    // 効果設定
	setCharsNormalColor();       // 通常文字色設定
	setNormalBGColor();          // 通常背景色設定

	Font_SetClipOn();
	SetClipArea(pk, pClip);                // クリップ設定
	Font_PutsPacket(buf, pk, PACKET_MAX);  // パケット作成
	SetClipArea(pk, NULL);                 // クリップ解除
	Font_SetClipOff();
}

// 変換文字列表示
static int drawStringBeforeChange(sceVif1Packet *pk, const u_int *pUCS4, const iRect_t *pClip, const iRect_t *pRc)
{
	char sjis[HENKAN_SIZE];
	char buf[2048];
	int  strWidth16 = 0;

	// アンダーラインをつける,背景色設定
	Font_SetEffect(FONT_EFFECT_BG | FONT_EFFECT_ULD);
	Font_SetRatio(APP_FONT_DEF);  // フォントの大きさ設定
	setCharsNormalColor();        // 文字色を通常の色にする
	setUnderlineColor();          // 下線色を設定
	setBeforeChangBGColor();      // 背景色を設定

	// 変換前文字列をATOKから取得(SJIS)
	Fep_GetConvertChars(sjis);

	// 描画用にSJIS->UTF8に変換
	sceCccSJIStoUTF8((sceCccUTF8*)buf, HENKAN_SIZE, (sceCccSJISTF*)sjis);
	strWidth16 = g_import.thinkXW16(Font_GetStrLength(buf)); // ついでに文字列幅を求める

	// 描画
	Font_SetLocate(pRc->x, g_import.thinkYHn(pRc->y));       // 位置設定

	Font_SetClipOn();
	SetClipArea(pk, pClip);               // クリップ設定
	Font_PutsPacket(buf, pk, PACKET_MAX); // パケット作成
	SetClipArea(pk, NULL);                // クリップ解除
	Font_SetClipOff();

	return strWidth16;
}


// 変換中文字列表示
// 文節ごとに描画
// 候補リストのサイズを指定
static int drawStringChanging(sceVif1Packet *pk, const u_int *pUCS4, const iRect_t *pClip, const iRect_t *pRc,
                              int *scrollLX16, int *scrollRX16, int frameW, int frameH, textField_t *pTf,
                              void drawCandListFrame(sceVif1Packet *pk, textField_t *pTf, iRect_t *rc16, int nCurrentCand) )
{
	// 文字列を描画する際のY、Hの値は、フォントライブラリの方で勝手に計算されるので
	// フィールドモードの際のY、Hの値でOK。
	int  i;
	int  maxClust, nowClust;
	char buf[2048];
	char *psjis = NULL;
	int  strWidth16 = 0;
	int  iCharStartPosX16 = 0;

	maxClust = Fep_GetClusterNum();
	nowClust = Fep_GetNowCluster();

	setCharsNormalColor();
	setUnderlineColor();

	Font_SetRatio(APP_FONT_DEF);                       // フォントの大きさ設定
	Font_SetLocate(pRc->x, g_import.thinkYHn(pRc->y)); // 位置設定
	Font_SetClipOn();
	SetClipArea(pk, pClip); // クリップ設定
	for(i=0; i<maxClust; i++)
	{
		psjis = Fep_GetClusterChars(i);
		sceCccSJIStoUTF8((sceCccUTF8*)buf, HENKAN_SIZE, (sceCccSJISTF*)psjis);
		strWidth16 += g_import.thinkXW16(Font_GetStrLength(buf));

		if(i == nowClust)
		{
			// 注目文節 ---
			// 注目文節が納まるようにスクロール
			iCharStartPosX16 = *scrollLX16 = Font_GetLocateX();

			// 背景色設定
			Font_SetEffect(FONT_EFFECT_BG);
			setChangingBGColor();

			Font_PutsPacket(buf, pk, PACKET_MAX);	// パケット作成

			// 注目文節が納まるようにスクロール
			*scrollRX16 = Font_GetLocateX();
		}
		else
		{
			// 注目文節 以外---
			Font_SetEffect(FONT_EFFECT_BG | FONT_EFFECT_UL);
			setUnderlineColor();
			setBeforeChangBGColor();

			Font_PutsPacket(buf, pk, PACKET_MAX);	// パケット作成
		}
	}
	SetClipArea(pk, NULL);  // クリップ解除
	Font_SetClipOff();


	//--------------------------------------------------------
	if(!Fep_ShowCandList())
	{
		// 候補一覧必要ない
		return strWidth16;
	}

	// 候補一覧を表示
	{
		char *pCandStart;                        // 候補リストデータの先頭
		int iPixels;                             // 候補リストの横幅サイズ(GS座標系)
		char buf[16];
		char sjis[16];
		char itemMain[HENKAN_SIZE];              // 候補一覧データ
		char itemSub[HENKAN_SIZE];               // 候補一覧データ 添え字
		char itemTmp[HENKAN_SIZE*2];             // 

		int i;
		int nCands   = Fep_CandListNum();        // 候補一覧データ数
		int nOffCand = Fep_GetCandListOffset();  // 候補一覧オフセット
		int nCurCand = Fep_GetCurrentCand();     // 候補一覧現在選択中のNO
		int nCandMax = Fep_GetCandListMax();     // 候補一覧最大値
		int gx = 0, gy = 0;
		char *cp;
		iRect_t clip;                            // クリッピング領域
		iRect_t rc;                              // 

		const int listHm = g_import.thinkYHn(2); //
		const int listHm16 = listHm << 4;

		int charH = 0, charH16 = 0;              // 文字の高さ

		pCandStart = Fep_GetCandStart();

		iPixels = Fep_GetCandListMaxWidth(APP_FONT_MDL, APP_FONT_MDL); // 候補一覧で表示される文字列の最大幅を得る
		iPixels += getStringWidthSJIS("0.", APP_FONT_MDL);             // 候補一覧インデックス
		charH   = Font_GetHeight();     // 文字の高さ取得
		charH16 = charH << 4;
		Font_SetRatio(APP_FONT_MDL);    // フォントの大きさ設定

		// 選択中NO/全体の数が候補の文字幅より大きかった場合
		{
			int pix = 0;
			int currentCand = Fep_GetCurrentCand();
			int maxCand     = Fep_GetCandListMax();
			char cNowIndex[16];
			sprintf(cNowIndex, "%d/%d", currentCand, maxCand); // 選択中NO/全体の数

			pix = getStringWidthSJIS(cNowIndex, APP_FONT_MDL);
			if(pix > iPixels)
			{
				iPixels = pix;
			}
		}

		// 候補一覧
		rc.x = iCharStartPosX16;
		rc.y = g_import.thinkYH16(pRc->y) + charH16;
		rc.w = iPixels + g_import.thinkXW16(frameW<<1);
		rc.h = ((charH16+listHm16)*(nCands+1)) + g_import.thinkYH16(frameH<<1); // ((候補＋候補間の空間) * 候補数) + 上下のフレームの高さ

		// 枠が画面に納まりきらない場合、表示位置を修正
		{
			int w = (rc.x+rc.w) - ((SCREEN_WIDTH<<4) - (SAFESCREEN_X16<<1));
			if(w > 0)
			{
				rc.x -= w;
				if(rc.x < SAFESCREEN_X16)
				{
					rc.x = SAFESCREEN_X16; // 候補一覧描画位置は安全フレーム内におさめる
				}
			}

			iCharStartPosX16 = rc.x;
		}
		drawCandListFrame(pk, pTf, &rc, (nCurCand-nOffCand)); // 枠描画

		// クリップ領域設定
		clip.x = rc.x>>4;
		clip.y = rc.y>>4;
		clip.w = rc.w>>4;
		clip.h = rc.h>>4;
		Font_SetClipOn();
		SetClipArea(pk, &clip); // クリップ設定


		// 選択候補カーサを描画
		{
			iRect_t curRc;

			curRc.x = iCharStartPosX16 + g_import.thinkXW16(frameW);
			curRc.y = (g_import.thinkYH16(pRc->y) + (charH16) + g_import.thinkYH16(frameH)) + ((charH16+listHm16)*(nCurCand-nOffCand));
			curRc.w = iPixels;
			curRc.h = charH16 + listHm16;

			SetRectFillEx(pk, &curRc, (RGBA32*)&g_Sample.aucFontBGCol[FONT_BGCOL_CHANGING][0], 0);
		}

		// 文字列を描画 --------
		Font_SetEffect(FONT_EFFECT_BG);
		// sjis[0] = '1'   // インデックス
		sjis[1] = '.';
		sjis[2] = 0x00;
		cp = pCandStart;

		gx = g_import.thinkXW(iCharStartPosX16) + frameW;
		gy = g_import.thinkYHn(pRc->y + frameH) + charH;
		for(i=0; i<nCands; i++)
		{
			Font_SetRatio(APP_FONT_MDL); // フォントの大きさ設定
			Font_SetLocate(gx, gy);

			// 候補番号を描画
			if(i == 9)
			{
				sjis[0] = '0';
			}
			else
			{
				sjis[0] = '1' + i;
			}
			sceCccSJIStoUTF8((sceCccUTF8*)buf, HENKAN_SIZE, (sceCccSJISTF*)sjis);
			setCharsNormalColor();
			setNormalBGColor();
			Font_PutsPacket(buf, pk, PACKET_MAX);

			// 候補文字列を描画
			memset(itemMain, 0x00, HENKAN_SIZE);
			memset(itemSub,  0x00, HENKAN_SIZE);
			cp = Fep_GetCandListItem(cp, itemMain, itemSub); // 文字取得(漢字:itemMain, ひらがな:itemSub)
			sceCccSJIStoUTF8((sceCccUTF8*)itemTmp, HENKAN_SIZE, (sceCccSJISTF*)itemMain);
			Font_PutsPacket(itemTmp, pk, PACKET_MAX); // パケット作成

			// 付属語の描画
			setSubCandListColor();
			sceCccSJIStoUTF8((sceCccUTF8*)itemTmp, HENKAN_SIZE, (sceCccSJISTF*)itemSub);
			Font_PutsPacket(itemTmp, pk, PACKET_MAX); // パケット作成

			gy += (charH) + listHm;  // 次の候補の為のY値作成
		}

		// 候補の数描画
		Font_SetRatio(APP_FONT_SML); // フォントの大きさ設定
		setCharsNormalColor();
		setNormalBGColor();
		Font_SetLocate(gx, gy+2);
		sprintf(sjis, "%d/%d", nCurCand+1, nCandMax);
		sceCccSJIStoUTF8((sceCccUTF8*)buf, HENKAN_SIZE, (sceCccSJISTF*)sjis);
		Font_PutsPacket(buf, pk, PACKET_MAX);

		SetClipArea(pk, NULL);  // クリップ解除
		Font_SetClipOff();
	}

	return strWidth16;
}

// 文字列(UCS2)の幅(ピクセル)取得
static int getStringWidth(const u_int *pUCS4)
{
	char buf[2048];
	// UCS4->UTF8に変換
	ucsToUtf8(pUCS4, buf);

	Font_SetRatio(APP_FONT_DEF);
	return Font_GetStrLength(buf) << 4;
}

//g_import.txt.getCaretPos    = Fep_GetCaretPos;
//g_import.txt.getAtokState   = Fep_AtokGetInputState;

#if 1 //SKB_EX Insert
int isInsertMode(void)  //上書きモード時にキャレットのグラフィックを変化させるために追加。
{
	if( Fep_GetInsertMode() == FEP_EDIT_ADD )   return 0;
	else                                        return 1;
}
#endif

// キャレット位置を取得
int getToCaretPos(void)
{
	char aSjis[1024];

	Fep_ConvStrings(aSjis);
	return getStringWidthSJIS(aSjis, APP_FONT_DEF);
}



//======================================================================================
//  キーボードセット、情報取得処理
//======================================================================================
int KeyBoardSet_Setup( KEYBOARD_SET* kbSet, tSkb_Ctl* skbCtl, int region  )
{
	int n;
	int alphabetKeyType=-1;

	int*next    = &skbCtl->auiNextKeyTop[0];
	int*modeBar = &skbCtl->modeBarNum[0];
	fepAtokInputMode_m* fepMode = &skbCtl->inputMode[0];

	// キーボードの順番と、使用フェプモードを、キーボードセット情報からコピー
	for(n=0;n<SKBTYPE_MAX;n++){
		next[n]    = kbSet->Keyboard[n].next;
		fepMode[n] = kbSet->Keyboard[n].fepMode;
		modeBar[n] = -1;
	}

	// モードバー設定
	{
		int c = 0;
		#ifndef REGION_J_OTHER_ONLY
		static const u_int MODEBAR_SKBTYPE_GRAPH_J[SKBTYPE_MAX]={	//SKBTYPE_ 順 モードバーグラッフィック
			MODEBAR_CHAR_HIRAGANA_J,  // あ  ひらがな 
			MODEBAR_CHAR_KATAKANA_J,  // ア  カタカナ 
			MODEBAR_CHAR_ALPHABET_J,  // Ａ  アルファベット
			MODEBAR_CHAR_ALPHABET_J,  // Ａ  アルファベット
			MODEBAR_CHAR_NUM,         // 123 数字
			MODEBAR_CHAR_SIGN_J,      // ☆  ＪＩＳ
		};
		#endif
		#ifndef REGION_J_ONLY
		static const u_int MODEBAR_SKBTYPE_GRAPH_AE[SKBTYPE_MAX]={	//SKBTYPE_ 順
			-1,
			-1,
			MODEBAR_CHAR_UMLAUT_F,    // Ａ  発音記号つきアルファベット 
			MODEBAR_CHAR_ALPHABET_F,  // Ａ  アルファベット  
			MODEBAR_CHAR_NUM,         // 123 数字            
			MODEBAR_CHAR_SIGN_F,      // ＠  記号            
		};
		#endif
		const u_int *modeBarGRAPH;

		#ifdef REGION_J_ONLY
		modeBarGRAPH = MODEBAR_SKBTYPE_GRAPH_J;  // 日本用
		#else
			#ifdef REGION_J_OTHER_ONLY
		modeBarGRAPH = MODEBAR_SKBTYPE_GRAPH_AE; // 海外用
			#else  //リソースが両方ある場合は、引数に従う。
			if( region == REGION_J )  modeBarGRAPH = MODEBAR_SKBTYPE_GRAPH_J;
			else                      modeBarGRAPH = MODEBAR_SKBTYPE_GRAPH_AE;
			#endif
		#endif

		// キーボードを順を、１順して、モードバー情報を作成。
		n = skbCtl->uiNowKeyType = kbSet->startKeyboardType;
		while( n < SKBTYPE_MAX ){
			if( n >= SKBTYPE_MAX ){
				Printf("KEYBOARD_SET LinkError\n");
				break;
			}
			if( alphabetKeyType == -1 ){	//キーボードセットが、どちらタイプを使用しているか判定。
				if( n == SKBTYPE_QWERTY )	alphabetKeyType=1;	// 代入値は、system.ini 環境準拠
				else if( n == SKBTYPE_ABC )	alphabetKeyType=0;	// 代入値は、system.ini 環境準拠
			}

			modeBar[n] = c;
			kbSet->Bar.ModeNum[n] = c;
			kbSet->Bar.SetMode[c] = modeBarGRAPH[n];

			Printf("setModeBar %d\n",modeBar[n]);
			c++;
			n = kbSet->Keyboard[n].next;
			if( n == kbSet->startKeyboardType )	break;
			if( c >= SKBTYPE_MAX )	printf("kbset error\n");
		}
		kbSet->Bar.setCount = c;
		if( modeBar[SKBTYPE_QWERTY] == -1 )	modeBar[SKBTYPE_QWERTY] = modeBar[SKBTYPE_ABC   ];
		if( modeBar[SKBTYPE_ABC   ] == -1 )	modeBar[SKBTYPE_ABC   ] = modeBar[SKBTYPE_QWERTY];

		// モードバー初期化
		Printf("CreateBar %d\n",c);
		SoftKB_ClearModebarType();
		SoftKB_SetModebarType( &kbSet->Bar.SetMode[0], kbSet->Bar.setCount );
	}

	// Ｆｅｐモード初期化 
	Fepbar_ClearModeType();
	Fepbar_CreateModeType( kbSet->Fep.Mode, kbSet->Fep.modeCount );

	return alphabetKeyType;
}

// キーボードセットの画面レイアウト座標位置を取得 (キーボード num 番目)
void KeyBoardSet_getKeybordLayout( KEYBOARD_SET* kbSet, int num, iPoint_t* point )
{
	*point = kbSet->Keyboard[num].Position;
}
// キーボードセットの画面レイアウト座標位置を取得 （テキストフィールド）
void KeyBoardSet_getTextBoxLayout( KEYBOARD_SET* kbSet, iRect_t* rect )
{
	*rect = kbSet->Text.Rect;
}
// キーボードセットの画面レイアウト座標位置を取得 （Ｆｅｐバー）
void KeyBoardSet_getFepBarLayout( KEYBOARD_SET* kbSet, iPoint_t* point )
{
	*point = kbSet->Fep.Position;
}
// キーボードセットの画面レイアウト座標位置を取得 （モードバー）
void KeyBoardSet_getModeBarLayout( KEYBOARD_SET* kbSet, iPoint_t* point )
{
	*point = kbSet->Bar.Position;
}

// キーボード初期状態の設定
void KeyBoardSet_InitKeyboad( KEYBOARD_SET* kbSet, int n, tSkb_Ctl* skbCtl )
{
	skey_t* skey = &skbCtl->KBD[n];
	u_int ucShift;
	u_int *ctrl;

	// カーソル位置の初期化。
	SoftKB_SetStartCurPos(skey, skey->uiKeyTopNo);

	// キーロックの設定
	ucShift = kbSet->Keyboard[n].ucShift;
	          // 1: SHIFT    ON
	          // 2: CAPSLOCK ON
	          // 4: ZENKAKU  ON/ALTGR ON

	//全角/半角ロックデフォルト設定
	if( skey->uiKeyTopNo == KEYTOP_NUMJP    // 全角/半角のあるキーボードだけ
	 || skey->uiKeyTopNo == KEYTOP_PC_JAPAN 
	 || skey->uiKeyTopNo == KEYTOP_ABC      
	 || skey->uiKeyTopNo == KEYTOP_KATAKANA 
	 ){ 
		if( ucShift & LAYOUT_ZENKAKU ){
			skey->ucShift |= LAYOUT_ZENKAKU;                  // 全角を設定
			onStateCtrlCode( skey, SKBCTRL_KANJI, KEY_PUSH);  // 半角/全角/漢字 全角
		}
		else{
			skey->ucShift &= (~LAYOUT_ZENKAKU);               // 半角を設定
			offStateCtrlCode( skey, SKBCTRL_KANJI, KEY_PUSH); // 半角/全角/漢字 半角
		}
	}

	//Ｃａｐｓロックデフォルト設定
	if( skey->uiKeyTopNo != KEYTOP_HIRAGANA   //キャプスの無いキーボードを除外 
	 && skey->uiKeyTopNo != KEYTOP_KATAKANA   
	 && skey->uiKeyTopNo != KEYTOP_IDIOMCHAR  
	 && skey->uiKeyTopNo != KEYTOP_NUMJP      
	 && skey->uiKeyTopNo != KEYTOP_NUMF       
	 && skey->uiKeyTopNo != KEYTOP_SIGNF      
	 && skey->uiKeyTopNo != KEYTOP_SIGN       
	 ){ 
		if( ucShift & LAYOUT_CAPS ){
			skey->ucShift |= LAYOUT_CAPS;                   
			onStateCtrlCode( skey, SKBCTRL_CAPS , KEY_PUSH);  // ＣＡＰＳロック
		}
		else{
			skey->ucShift &= (~LAYOUT_CAPS);                
			offStateCtrlCode( skey, SKBCTRL_CAPS , KEY_PUSH);  // ＣＡＰＳロックOFF
		}
	}

	// 全てのコントロールキーを、入力可に戻す。
	offStateCtrlCode( skey, SKBCTRL_CAPS     , KEY_DISABLE );
	offStateCtrlCode( skey, SKBCTRL_SHIFT_L  , KEY_DISABLE );
	offStateCtrlCode( skey, SKBCTRL_SHIFT_R  , KEY_DISABLE );
	offStateCtrlCode( skey, SKBCTRL_BACKSPACE, KEY_DISABLE );
	offStateCtrlCode( skey, SKBCTRL_ENTER    , KEY_DISABLE );
	offStateCtrlCode( skey, SKBCTRL_UP       , KEY_DISABLE );
	offStateCtrlCode( skey, SKBCTRL_DOWN     , KEY_DISABLE );
	offStateCtrlCode( skey, SKBCTRL_LEFT     , KEY_DISABLE );
	offStateCtrlCode( skey, SKBCTRL_RIGHT    , KEY_DISABLE );
	offStateCtrlCode( skey, SKBCTRL_HELP     , KEY_DISABLE );
	offStateCtrlCode( skey, SKBCTRL_KANJI    , KEY_DISABLE );
	offStateCtrlCode( skey, SKBCTRL_TAB      , KEY_DISABLE );
	offStateCtrlCode( skey, SKBCTRL_ABCQWERTY, KEY_DISABLE );

	// 入力禁止キーを設定する
	if( (ctrl = kbSet->Keyboard[n].disableCtrl) ){
		for(;;ctrl++){
			if( *ctrl == 0 )	break;
			onStateCtrlCode( skey, *ctrl, KEY_DISABLE );
		}
	}
}


//============================================================================
// ucs4 文字列関数
//============================================================================
static int ucs4cmp( u_int* ucs4str0, u_int* ucs4str1, int count )
{
	int n;

	for(n=0;n<count;n++){
		if( ucs4str0[n] != ucs4str1[n] ){
			return n+1;
		}
		if( ucs4str0[n] == 0 )	break;
	}
	return 0;
}
static int ucs4cpy( u_int* ucs4dst, u_int* ucs4src, int count )
{
	int n;

	for(n=0;n<count;n++){
		ucs4dst[n] = ucs4src[n];
		if( ucs4src[n] == 0 ){
			for(;n<count;n++)	ucs4dst[n]=0;
			return 0;
		}
	}
	n--;
	ucs4dst[n] = 0;

	return -n;
}
//--------------------------------------------------------------------------------------
// SKB 外部インターフェース
//--------------------------------------------------------------------------------------
int ThreadID_SKBWork=-1;            //重い処理用のスレッドのＩＤ格納変数
static char stack[16*1024] __attribute__((aligned(128))); //そのスタック

SKB_ARG SkbArg; //環境引数保存。

// SKB関数郡初期化。
// ソフトウェアキーボード使用前に環境設定を行う
int Init_SKBConfig( SKB_ARG* skbArg, char *fsName, short keyType, short repWait, short repSpeed )
{
	SkbArg = *skbArg;
	memset(&g_import, 0x0, sizeof(gImport_t));

	g_import.iRegion = SkbArg.region;  // 地域指定
	#ifdef REGION_J_ONLY
	g_import.iRegion = REGION_J;       // 日本版
	#endif
	#ifdef REGION_J_OTHER_ONLY
	if( g_import.iRegion == REGION_J ){
		g_import.iRegion = REGION_A;   // ＵＳ版 english only
		//g_import.iRegion = REGION_E; // ＥＵ版 no Support
	}
	#endif

	g_import.malign = SkbArg.memAlign;              // Sysenv_ReadSystemIni() が、g_importを、
	g_import.mfree  = SkbArg.memFree;               // 直接参照しているので、最初に設定

	g_import.usSystemIni[ENV_KEYBD_TYPE   ]=keyType;  // デフォルトのキーボードに。
	g_import.usSystemIni[ENV_KEYBD_REPEATW]=repWait;  // デフォルトのウェイトに
	g_import.usSystemIni[ENV_KEYBD_REPEATS]=repSpeed; // デフォルトのスピードに
	g_import.usSystemIni[ENV_SOFTKB_ONOFF ]=1;        // デフォルトを画面キーボード有り
	g_import.usSystemIni[ENV_SOFTKB_QWERTY]=0;        // デフォルトをアルファベットはＡＢＣ順

	return Sysenv_ReadSystemIni(g_import.usSystemIni, fsName); // system.ini読む
}

// ソフトウェアキーボード関係の初期化
// 返り値 電源コントロール時に、待つべきセマフォＩＤを返すint(*)(void)関数ポインタ。
//
SKB_GETSEMA_FUNC SKB_Init()
{
	Printf("SKB_Init [%s]\n",!getFrameField() ? "FieldMode" : "Etc Mode");
	#if CDBOOT
	Printf("CDBOOT!!\n");
	#endif 

	ThreadID_SKBWork=-1 ;	//重い処理用のスレッドのＩＤ格納変数初期化
	//--------------------------------------------------
	// 制御構造体初期化 & その他
	//--------------------------------------------------
	memset(&g_Sample, 0x0, sizeof(g_Sample));
	{

		// フォント色定義 設定
		initColor();
		/*	以下関数で、使用される。
			setCharsNormalColor();  // フォント色           通常
			setSubCandListColor();  // 変換候補送りがなの色 
			setUnderlineColor();    // アンダーライン色     
			setNormalBGColor();     // フォント背景色設定   
			setChangingBGColor();   // フォント背景色（辞書変換中）
		*/

		//--------------------------------------------------
		// SoftWareKeyBoad描画領域確保
		//--------------------------------------------------
		sceVif1PkInit(&g_pkVif1[0], (u_long128*)g_pkVif1Buff[0]);		// sceVif1Packet構造体を初期化
		sceVif1PkInit(&g_pkVif1[1], (u_long128*)g_pkVif1Buff[1]);		// sceVif1Packet構造体を初期化
	}
	//--------------------------------------------------
	// 入力禁止文字の定義構造体の初期化
	//--------------------------------------------------
	{
		int n;
		
		inhibitChar.pCtrlKey  = NULL;
		inhibitChar.uiCKeyNum = 0;
		inhibitChar.pChar     = inhibitUCS4;
		inhibitChar.uiCharNum = 0;
		
		for(n=0;n<SKBTYPE_MAX;n++){
			g_Sample.KBD[n].pInhibitChar=&inhibitChar;
		}
		#if 1//SKB_EX Inhibit
		// 入力禁止文字であるが、画面キーボードのボタンは押せる文字リスト格納バッファ。
		inhibitSubChar.uiCKeyNum = 0;
		inhibitSubChar.pCtrlKey  = NULL;
		inhibitSubChar.uiCharNum = 0;
		inhibitSubChar.pChar     = inhibitSubUCS4;
		#endif
	}

	// システム設定を取得
	g_Sample.Env.inputDevice     = g_import.usSystemIni[ENV_SOFTKB_ONOFF];   // screen keyboard or usbkeyborad
	g_Sample.Env.alphabetKeyType = g_import.usSystemIni[ENV_SOFTKB_QWERTY];  // abc or pc key

	// 文字Y値を設定
	Font_SetTvFontRatio(VRESOLUTION_ADJUST_F(TV_RATIO));

	//--------------------------------------------------
	// g_Import 共通部設定
	//--------------------------------------------------
	{
		if(g_import.iRegion == REGION_J){
			#if ATOK_USE == 0
			g_import.ucUseAtok  = 0;  // ATOK使わない
			#else
			g_import.ucUseAtok  = 1;  // ATOK使う
			#endif
		}
		else{
			g_import.ucUseAtok  = 0;  // ATOK使わない
		}

		g_import.uiScreenW = SCREEN_WIDTH;
		g_import.uiScreenH = SCREEN_VHEIGHT;

		g_import.dprintf       = dprintf;
		g_import.getFrameField = getFrameField;
		g_import.getNtscPal    = getNtscPal;
		g_import.usLang        = Sysenv_GetLang();
		if(g_import.iRegion == REGION_J){
			if( g_import.usLang > 1  )	g_import.usLang = 1;
		}
		else{
			if( g_import.usLang == 0 )	g_import.usLang = 1;
		}

		g_import.se      = se;

		{
			u_int tbp,cbp;

			cbp = SkbArg.Gs_workAddress256;
			tbp = cbp + 4;

			Tim2_Init( SCREEN_WIDTH, SCREEN_HEIGHT, OFFX, OFFY, tbp, cbp,
			           g_import.malign,
			           g_import.mfree );

			g_import.readTex   = Tim2_ReadTim2;
			g_import.deleteTex = Tim2_DeleteTim2;
			g_import.loadTex   = Tim2_SendTim2;
			g_import.drawTex   = Tim2_DrawSprite;
			g_import.drawPane  = SetRectFill;
			g_import.drawLine  = SetLine;
		}

		g_import.thinkYHn  = thinkYHn;
		g_import.thinkYH   = thinkYH;
		g_import.thinkXW   = thinkXW;
		g_import.thinkYH16 = thinkYH16;
		g_import.thinkXW16 = thinkXW16;

		g_import.drawFocus = somewhereFocus;
		g_import.isConv    = Fep_GetNowConv;
	}

	//--------------------------------------------------
	// g_Import Ｆｅｐ初期化
	//--------------------------------------------------
	{
		// 各設定コピー
		int atokPrio;
		fepImport_t *pFep = &g_import.fep;

		pFep->puiUCS4      = g_Sample.uiChars;  // 文字列へのポインタ
		pFep->uiMaxTextLen = MAX_CHAR_COUNT;    // 限界文字数

		pFep->atokON  = atokON;
		pFep->atokOFF = atokOFF;
		pFep->changeMode = changeMode;
		pFep->changeText = changeText;
		pFep->powerLock   = powerLock;
		pFep->powerUnlock = powerUnlock;
		pFep->ok          = exitOK;
		pFep->cancel      = cancel;
		pFep->getNumlockState = getNumlockState;

		pFep->showHideSoftKB = showHideSoftKB;
		pFep->closeSoftKB    = closeSoftKB;
		pFep->openSoftKB     = openSoftKB;

		pFep->getStrWidthSJIS = getStringWidthSJIS;

		{
			struct ThreadParam t_info;

			ReferThreadStatus( GetThreadId(), &t_info );
			if( t_info.currentPriority < ATOK_THREAD_PRIORITY-2 ){
				atokPrio = ATOK_THREAD_PRIORITY;
			}
			else{
				atokPrio = t_info.currentPriority;
				if(atokPrio<127-2)      atokPrio+=2;
				else if(atokPrio<127-1) atokPrio+=1;
			}
			Printf("AtokPrio=%d\n",atokPrio);
		}
		Fep_Init(&g_import   , g_import.ucUseAtok, atokPrio, SkbArg.AtokPath.sysDic, SkbArg.AtokPath.usrDic, SkbArg.AtokPath.system );
	}
	//---------------------------------------------------------------------------
	// FEPBAR初期化（USB キーボード使用時に出る、Ｆｅｐ入力モード表示を行うバー）
	//---------------------------------------------------------------------------
	Fepbar_Init(&g_import);

	//--------------------------------------------------
	// g_Import ソフトウェアキーボード部設定
	//--------------------------------------------------
	{
		int j;
		skbImport_t *pSkb = &g_import.skb;

		pSkb->uiListFontW = FONT_W;
		pSkb->uiListFontH = FONT_H;
		pSkb->ucShowSKbd = g_import.usSystemIni[ENV_SOFTKB_ONOFF];
		pSkb->changeInputMode = changeInputMode;
		pSkb->abcToQwerty = exchangeABCtoQWERTY;
		pSkb->getNowInputMode = getNowInputMode;

		pSkb->getAtokInput= Fep_GetNowAtokInput;
		pSkb->getAtokMode = Fep_Changing;
		pSkb->kakuteiPart = Fep_SetKakuteiPart;
		pSkb->kakuteiAll  = Fep_SetKakuteiAll;

		pSkb->setChars = setChars;
		pSkb->exitOK   = skb_exitOK;
		pSkb->showHelp = showHelp;
		pSkb->keytopChange = keytopChange;
		pSkb->opening = opening;
		pSkb->closing = closing;

		pSkb->setListFontEnv     = setSkbListFontEnv;
		pSkb->setListDrawString  = setSkbListDrawString;
		pSkb->setListDrawStringF = setSkbListDrawStringFocus;
		pSkb->getStringWidth  = getStringWidthUtf8;


		SoftKB_Init(&g_import);


		//--------------------------------------------------
		// ソフトウェアキーボード 作成
		//--------------------------------------------------
		{
			int keyTop[SKBTYPE_MAX];

			#ifndef REGION_J_OTHER_ONLY
			if( g_import.iRegion == REGION_J ){
				keyTop[SKBTYPE_HIRA  ] = KEYTOP_HIRAGANA;
				keyTop[SKBTYPE_KANA  ] = KEYTOP_KATAKANA;
				keyTop[SKBTYPE_ABC   ] = KEYTOP_ABC;
				switch( g_import.usSystemIni[ENV_KEYBD_TYPE   ]){
				case USBKB_ARRANGEMENT_101       : keyTop[SKBTYPE_QWERTY] = KEYTOP_PC_US;    break; // US
				default:
				case USBKB_ARRANGEMENT_106       : keyTop[SKBTYPE_QWERTY] = KEYTOP_PC_JAPAN; break;
				case USBKB_ARRANGEMENT_106_KANA  : keyTop[SKBTYPE_QWERTY] = KEYTOP_PC_JAPAN; break;
				}
				keyTop[SKBTYPE_NUM   ] = KEYTOP_NUMJP;
				keyTop[SKBTYPE_SIGN  ] = KEYTOP_SIGN;
			}
			else
			#endif
			{
			#ifndef REGION_J_ONLY
				keyTop[SKBTYPE_HIRA  ] = -1;
				keyTop[SKBTYPE_KANA  ] = -1;
				keyTop[SKBTYPE_ABC   ] = KEYTOP_IDIOMCHAR;
				switch( g_import.usSystemIni[ENV_KEYBD_TYPE   ] ){
				default:
				case USBKB_ARRANGEMENT_101       : keyTop[SKBTYPE_QWERTY] = KEYTOP_PC_US;         break; // US
				}
				keyTop[SKBTYPE_NUM   ] = KEYTOP_NUMF;
				keyTop[SKBTYPE_SIGN  ] = KEYTOP_SIGNF;
			#endif 
			}

			g_Sample.Env.alphabetKeyType = g_import.usSystemIni[ENV_SOFTKB_QWERTY];

			for(j=0; j<SKBTYPE_MAX; j++)
			{
				iPoint_t skbPoint = {0, 0};
				void *listItem;

				if( keyTop[j] < 0 )	continue;
				if( keyTop[j] == KEYTOP_SIGN )	listItem = (void*)g_ListItems;
				else                          	listItem = NULL;
				
				if( SoftKB_Create( &g_Sample.KBD[j], &skbPoint, keyTop[j], &inhibitChar, listItem ) ){
					printf("ERROR %s, %d: SKB_Init - SoftKB_Create", __FILE__, __LINE__);
					return 0;
				}
				// キーボード表示の半透明レートの設定。
				g_Sample.KBD[j].ucAlpha[0] = 0x80;
				g_Sample.KBD[j].ucAlpha[1] = 0x80;
				g_Sample.KBD[j].ucAlpha[2] = 0x80;
				g_Sample.KBD[j].ucAlpha[3] = 0x80;
			}
		}
	}
	//-----------------------------------------------------------
	// TEXTFIELD 初期化（テキストボックスの作成）
	//-----------------------------------------------------------
	{
		// 各設定コピー
		iRect_t rc = {30, 128, 500, 25};
		txtFImport_t *pTxt = &g_import.txt;

		//テキストフィールドのフォントサイズにしてから、フォントの高さを取得する。
		Font_SetRatio(APP_FONT_DEF);	

		pTxt->uiCharHeight = Font_GetHeight();

		pTxt->getStrings     = getStrings;
		pTxt->drawStrings    = drawStrings;
		pTxt->drawStringBeforeChange = drawStringBeforeChange;
		pTxt->drawStringChanging = drawStringChanging;
		pTxt->getStringWidth = getStringWidth;
		pTxt->getCaretPos    = Fep_GetCaretPos;
		pTxt->getAtokState   = Fep_AtokGetInputState;
		#if 1//SKB_EX Insert  
		pTxt->isInsertMode  = isInsertMode; //上書きモード時にキャレットを変化させるために追加。
		#endif
		pTxt->getToCaretPos = getToCaretPos;

		TextF_Init(&g_import);
		TextF_Create(&textField,  &rc);
		TextF_ChangeAlpha(&textField, TF_ALPHA_EDITORBG  , 0x80);
		TextF_ChangeAlpha(&textField, TF_ALPHA_CARET     , 0x80);
		TextF_ChangeAlpha(&textField, TF_ALPHA_CANDLISTBG, 0x80);
		textField.uiCaretColor = 0x80ff7070;     // キャレット色設定
	}

	// 電源ＯＦＦ時に、
	// ＡＴＯＫ辞書ファイルの論理的整合性を守るため
	// ＡＴＯＫによるＨＤＤアクセス中は、電源ＯＦＦを待ってもらう必要がある。
	return Fep_GetAtokSemaID; //呼び出し側に渡す。
}

// クローズ時に編集結果文字列の受け取り、ＳＥなど、メッセージを受け取る関数を登録する関数。
void SKB_SetCallback( int(*func)(int,void*) )
{
	g_Sample.Message = func;
}


// 
// キーボードの、system.ini  情報の取得
// 
void SKB_GetKbdConfig( int* kbType, int *repeatWait, int *repeatSpeed,int *region  )
{
	*kbType      = g_import.usSystemIni[ENV_KEYBD_TYPE   ];
	*repeatWait  = g_import.usSystemIni[ENV_KEYBD_REPEATW];
	*repeatSpeed = g_import.usSystemIni[ENV_KEYBD_REPEATS];
}


//
// 返り値 ソフトキーボード動作状態 0:非動作 1:動作中
//
int SKB_GetStatus()
{
	return g_Sample.iControlFlag;
}


//
// 返り値 全角入力か判定  0:全角入力 1:半角入力
//
int SKB_GetZenkakuMode()
{
	int mode = SoftKB_GetInputMode(&g_Sample.KBD[g_Sample.uiNowKeyType]);		// 全角/半角モードか取得
	int ret=-1;

	switch(mode)
	{
	case SKBMODE_HIRAGANA    :
	case SKBMODE_ZEN_KATAKANA:
	case SKBMODE_ZEN_ALPHABET: ret = 1; break;
	case SKBMODE_HAN_KATAKANA:
	case SKBMODE_HAN_ALPHABET: ret = 0; break;
	}
	return ret;
}

// 内部関数　ソフトウェアキーボード  オープン
void SKB_open();
//
// パッド入力の受け取りと、動作制御
//
void SKB_Control( u_long ulPad )
{
	int inputKbdFlag=0;

	if( g_Sample.iControlFlag && g_Sample.iOpenFlag==0 ){
		// SKB_Open() で、入力禁止情報作成待ちで、開かれていなかったら。
		if( ThreadID_SKBWork==-1 ){ // 生成後、ここでＯＰＥＮ
			SKB_open();             // 内部で、iOpenFlagが１になる。
		}
	}
	else if( g_Sample.iOpenFlag <= 0 )	return;

	if( KbStat < 0 ) inputKbdFlag = 0;
	else             inputKbdFlag = KbStat;

	if( g_import.iRegion != REGION_J ) //日本向けでなければ、○×情報を入れかえる。
	{
		u_long pad_Rright = (((u_long)SCE_PADRright<<48)|((u_long)SCE_PADRright<<32)|((u_long)SCE_PADRright<<16)|((u_long)SCE_PADRright));
		u_long pad_Rdown  = (((u_long)SCE_PADRdown <<48)|((u_long)SCE_PADRdown <<32)|((u_long)SCE_PADRdown <<16)|((u_long)SCE_PADRdown ));
		                     // release                  triger                      press                        repeat
		u_long pad_Mask   = ~(pad_Rright|pad_Rdown);

		#if SCE_PADRright > SCE_PADRdown
		pad_Rright = (ulPad&pad_Rright)/(SCE_PADRright/SCE_PADRdown);
		pad_Rdown  = (ulPad&pad_Rdown )*(SCE_PADRright/SCE_PADRdown);
		#else
		pad_Rright = (ulPad&pad_Rright)*(SCE_PADRdown/SCE_PADRright);
		pad_Rdown  = (ulPad&pad_Rdown )/(SCE_PADRdown/SCE_PADRright);
		#endif
		ulPad = (( ulPad & pad_Mask) | pad_Rright | pad_Rdown );
	}

	if(! SoftKB_CheckShown() )      // キートップが表示されているか？  USBキーボード入力時は、表示されていない。
	{
		// Ｆｅｐバー
		Fepbar_SetPadData(ulPad);   // キーボードの代わりに、Ｆｅｐ入力モード表示バーを処理
		if( g_Sample.iUsbKeyFlag==0 ){
			g_Sample.iUsbKeyFlag = 1;
			if(g_Sample.Message)	g_Sample.Message( SKBMES_USBKEY_ON,(void*)0 );
			//USBキーボードになると、デフォルトでＦｅｐ入力、ひらがなに設定されるので、再設定。
			changeInputMode_usbkb();
		}
	}
	else
	{
		if( g_Sample.iUsbKeyFlag ){
			g_Sample.iUsbKeyFlag = 0;
			if(g_Sample.Message)	g_Sample.Message( SKBMES_USBKEY_OFF,(void*)0 );
		}
		// キーボード
		SoftKB_SetPadData(&g_Sample.KBD[g_Sample.uiNowKeyType], ulPad); // パッドのデータ設定
	}

	// USB キーボード入力による制御
	if(!g_Sample.iCoverView)  // テキストボックスが表示マスクされていないか？  Help画面表示中は、マスクされている。 
	{
		//テキストボックス有効時
		if( inputKbdFlag )    // ＵＳＢキーボードによる入力が有効
		{
			Fep_PostKeys( &KbData );	// キーボード入力文字をPOST
		}
	}
}

//
// USBキーボード入力受け取り
//
void SKB_SetUsbKeyborad( int stat, USBKBDATA_t *usbKb, int numLock )
{
	KbStat = stat;
	if(stat>0){
		KbData = *usbKb;
		KbNumLock = numLock;
	}
}

//
// 内部関数　ソフトウェアキーボード  オープン
//
KEYBOARD_SET  KBSet;
u_int         KBText[256];
void SKB_open()
{
	KEYBOARD_SET* kbSet = &KBSet;
	u_int*         text = KBText;

	//-------- ソフトウェアキーボード OPEN ---
	openSoftKB(1);
	{
		skey_t *pKey;
		int n;
		//キーボードの設定初期化 ※openSoftKB()で、デフォルトが設定されるので、openSoftKB()より後に行う
		for(n=0;n<SKBTYPE_MAX;n++){
			KeyBoardSet_InitKeyboad( kbSet, n , &g_Sample );
		}
		// キーロックの状態を変更にともない、入力禁止状態を再設定。
		pKey = &g_Sample.KBD[g_Sample.uiNowKeyType];
		setEnableChars( pKey );                     // 全てのキーを有効化
		setInhibitChars( pKey, pKey->pInhibitChar );// 入力禁止キーを再設定
	}
	//初期テキストの設定
	if( text ){
		// 初期設定文字列を、ＦＥＰに送信
		Fep_SetText( text );
	}

	// デフォルト入力デバイスの設定（パッドまたは、ＵＳＢキーボード）  system.ini 環境準拠
	g_import.skb.ucShowSKbd = g_Sample.Env.inputDevice;
	// ＵＳＢキーボードでスタートだったら、ＵＳＢキーボード入力時のＦｅｐモードで設定。
	if( g_Sample.Env.inputDevice==0 ){
		changeInputMode_usbkb();
		g_Sample.Message( SKBMES_USBKEY_ON,(void*)0 );	//使用アプリ側に報告
	}
	// fep On/Off 時のＦｅｐモード記憶変数を、初期化。
	g_Sample.fepModeOnState = -1;

	//オープンした判定用に、フラグを立てておく。
	g_Sample.iOpenFlag = 1;
}


//
// ソフトウェアキーボード  オープン
//
void SKB_Open( KEYBOARD_SET* kbSet, int editType, u_int* text, int inputMax )
{
	int n;
	int atype;

	Printf("SkbOpen \n" );
	g_Sample.iControlFlag = 1;            // SoftWareKeyBoard表示開始

	//--------  テキスト編集バッファの初期化 ------------------------------------------------
	g_import.fep.uiMaxTextLen = MAX_CHAR_COUNT; //デフォルト最大文字数指定。バッファクリアにも使用される。
	g_Sample.uiChars[0] = 0;	// テキストバッファ初期化
	g_Sample.uiCharNum  = 0;
	{
		static int clearCode[2] = { 0x00000015, 0 };
		Fep_SetText( clearCode );	// 文字列送出(NULL文字まで送信される)
	}
	//--------  最大入力文字数の設定 =-------------------------------------------------------
	if( inputMax > 0 && inputMax <= MAX_CHAR_COUNT ){
		g_import.fep.uiMaxTextLen = inputMax;
	}
	//--------- 編集モード設定 (通常 / -1:PASSWORD)------------------------------------------
	if( editType == -1 ) TextF_SetDispMode( &textField, 1 );
	else                 TextF_SetDispMode( &textField, 0 );

	//======== キーボードセットの設定  ============================================================================
	atype = KeyBoardSet_Setup( kbSet, &g_Sample, g_import.iRegion );
	//  環境に ABC<->QWERTY 設定をあわせる
	if( atype >= 0 && atype != g_Sample.Env.alphabetKeyType ){
		swap_ABC_QWERTY(-1);	//
	}
	//--  画面上のレイアウトを設定  ------
	{
		iRect_t  rect;
		iPoint_t point;

		for(n=0;n<SKBTYPE_MAX;n++){
			if( g_Sample.modeBarNum[n] < 0 )	continue;
			KeyBoardSet_getKeybordLayout( kbSet, n, &point );	SoftKB_Move( &g_Sample.KBD[n], point.x, point.y );
			g_Sample.skbPos[n] = point;
		}
		KeyBoardSet_getTextBoxLayout( kbSet, &rect );    TextF_Move( &textField,  &rect );
		KeyBoardSet_getFepBarLayout( kbSet, &point );    Fepbar_Move( point.x, point.y );
		KeyBoardSet_getModeBarLayout( kbSet, &point );   SoftKB_MoveModebar( point.x, point.y );
	}

	KBSet=*kbSet;
	for(n=0;n<256;n++){
		KBText[n]=text[n];
		if( text[n]==0 )	break;
	}

	if( ThreadID_SKBWork==-1 ){	//入力禁止情報生成が終わっていたら即ＯＰＥＮ
		SKB_open();
	}
}


//
// ソフトウェアキーボードの描画パケット作成
//   引数の clipOffX clipOffY は、作成パケットの描画位置ではなく、
//   描画オフセット(GSによる描画先のオフセット)を変更した場合に、
//   シザリング矩形位置の補正用オフセット値で(シザリングはGSの描画オフセットの影響を受けないため)
//   描画オフセットに影響を受けさせない描画パケット(全画面表示のヘルプ画面などの)にも
//   逆算用オフセット値として、使用されています。
void SKB_makePacket(sceVif1Packet *pkVif1, int clipOffX, int clipOffY )
{
	if( g_Sample.iOpenFlag == 0 )	return;

	if( g_Sample.iCoverView ){	// textbox がマスクされていたら、ヘルプ表示中
		SetClipOffset( 0, 0 );	// ヘルプ表示中は、シザリング範囲をずらさない。
		SetClipArea( pkVif1, NULL );
		HelpOffset( -clipOffX, -clipOffY );
	}
	else{
		SetClipOffset( clipOffX, clipOffY ); 
	}

	if(! SoftKB_CheckShown() ) // 画面にキーボードを出していないなら
	{	// USBキーボードを使用中
		Fepbar_Run(pkVif1);  // USBキーボード時に、表示されるＦｅｐ入力モードバーの処理
	}
	else
	{	// 画面のキーボード使用中
		SoftKB_Run(pkVif1, &g_Sample.KBD[g_Sample.uiNowKeyType]); // 画面キーボードの処理
	}

	if(!g_Sample.iCoverView) // テキストボックスの描画マスクされていなければ
	{
		//編集操作を行う。
		Fep_Run();

		TextF_Run(pkVif1, &textField); // テキストボックス処理
	}
}

//
// ソフトウェアキーボードの描画パケット作成
//
void* SKB_MakeVif1CallPacket( int clipOffX, int clipOffY )
{
	sceVif1Packet *pkVif1 = &g_pkVif1[g_pkVif1_ID];

	g_pkVif1_ID^=1;    // ダブルバッファ入れ替え

	sceVif1PkReset(pkVif1);     // Reset

	// パケット生成 
	SKB_makePacket(pkVif1, clipOffX, clipOffY );

	sceVif1PkRet(pkVif1, 0);    // ※sceVif1PkEndではない
	sceVif1PkTerminate(pkVif1); // 

	return((void *)pkVif1->pBase); //作成したパケットのアドレスを返り値に
}

//
// ソフトウェアキーボードのクローズ
//
void SKB_Close( char* text )
{
	Printf("SKB_Close()\n");

	SoftKB_Close(&g_Sample.KBD[g_Sample.uiNowKeyType]); // キーボードClose

	g_Sample.iCoverView = 1;       // TextBox非表示
	g_Sample.iOpenFlag = 0;        // 0:Closed 1:Open中&Opened   -1:Close中
	g_Sample.iControlFlag = 0;     // 0:Closed 1:Open中&Opened&Close中

	g_Sample.Env.inputDevice = g_import.skb.ucShowSKbd; //クローズ時に情報を残す（pad/usb）。

	closeSoftKB();
}



// 禁止文字情報をコピーする内部関数
static int SKB_inhibitCopy( inhibitChar_t *src , inhibitChar_t *dst ,int max )
{
	int n;

	dst->uiCharNum = 0;

	for(n=0;n<src->uiCharNum;n++){
		if( n >= max )	return -n;
		ucs4cpy( &dst->pChar[dst->uiCharNum][0], &src->pChar[n][0], 10);
		dst->uiCharNum++;
	}

	return n;
}

// 禁止文字列から、定型文 http:// などの定型文内も禁止文字を使用しているか検査し、禁止キーに設定。
static void SKB_inhibitDisable_stringKey()
{
	int n,bn,sn,cn,dn;
	u_int *disableChar;
	u_int *keyChar;
	skey_t *pKey;
	inhibitChar_t *pInhibit;
	int inhibitMax;

	pInhibit   = &inhibitChar;      //入力禁止文字登録先
	inhibitMax = pInhibit->uiCharNum;
	for( n=0; n<SKBTYPE_MAX; n++ ){ 
		pKey = &g_Sample.KBD[n];
		if( pKey->uiKeyTopNo != KEYTOP_ABC )	continue;// ABC 配列以外は関係なし
		for( bn=0; bn<pKey->uiBtnNum ;bn++ ){ // ボタンキーを一つづつチェック
			for( sn=0; sn<8; sn++ ){          // Shift Ctrl Kanji などで８ケース分の情報がある
				if( pKey->pBtns[bn].state[sn].ctrlCode ){ //コントロールキーは無視
					continue;
				}
				keyChar = pKey->pBtns[bn].state[sn].key;

				if( keyChar[1]==0 )	continue; // 定型文のキー以外は無視

				for( cn=0; cn<10; cn++){
					if( keyChar[cn] == 0 ) break;

					for( dn=0; dn < inhibitMax ;dn++ ){
						disableChar = &pInhibit->pChar[dn][0];
						if( disableChar[1] )             continue;
						if( keyChar[cn] == disableChar[0] ) break;
					}
					if( dn != inhibitMax ){ //禁止文字が含まれていたら
						for( dn=0; dn< pInhibit->uiCharNum; dn++ ){ //禁止キャラクタの重複チェック
							if( ucs4cmp( keyChar, &pInhibit->pChar[dn][0], 10 )==0 )	break;
						}
						if( pInhibit->uiCharNum==0 || dn==pInhibit->uiCharNum ){// 禁止キャラクタが重複していない
							// 禁止キャラクタに追加
							if( pInhibit->uiCharNum < INHIBIT_CHAR_MAX ){//禁止文字列バッファの最大数を超えていなければ
								ucs4cpy( &pInhibit->pChar[pInhibit->uiCharNum][0], keyChar, 10 );
								pInhibit->uiCharNum ++; //禁止文字列登録。
							}
						}
						break;
					}
				}
			}
		}
	}
}

// 入力許可文字から、ソフトウェアキーボードと照らし合わせて、
// 入力禁止文字情報を、作成する内部関数。
// （ＵＳＢキーボードから入力できる文字を考慮していない）
// 非常に重い処理なので、低プライオリティのスレッドで実行している。
static void SKB_thread_InhibitEnableToDisable()
{
	int n,bn,sn,cn;
	u_int *enableChar;
	u_int *keyChar;
	skey_t *pKey;
	inhibitChar_t *pEnable = &EnableChar; 
	inhibitChar_t *pInhibit;
	int inhibitMax;

	inhibitChar.uiCKeyNum = 0;
	inhibitChar.pCtrlKey  = NULL;
	inhibitChar.uiCharNum = 0;
	inhibitChar.pChar     = inhibitUCS4;
	inhibitSubChar.uiCKeyNum = 0;
	inhibitSubChar.pCtrlKey  = NULL;
	inhibitSubChar.uiCharNum = 0;
	inhibitSubChar.pChar     = inhibitSubUCS4;

	Printf("enableChar count %d, %p \n",pEnable->uiCharNum,(void *)pEnable);

	for( n=0; n<SKBTYPE_MAX; n++ ){ //全てのキーボードを検索
		pKey = &g_Sample.KBD[n];
		if( pKey->uiKeyTopNo == KEYTOP_SIGN )	continue;// Jis表入力は非サポート
		for( bn=0; bn<pKey->uiBtnNum ;bn++ ){ // ボタンキーを一つづつチェック
			for( sn=0; sn<8; sn++ ){          // Shift Ctrl Kanji などで８ケース分の情報がある
				if( pKey->pBtns[bn].state[sn].ctrlCode ){ //コントロールキーは無視だが、
					if( pKey->pBtns[bn].state[sn].ctrlCode != SKBCTRL_TAB ){
						continue;
					}
					keyChar = (u_int *)"\t";              // タブだけは、入力制御に従う。
				}
				else keyChar = pKey->pBtns[bn].state[sn].key;

				for( cn=0; cn < pEnable->uiCharNum ;cn++ ){
					enableChar = &pEnable->pChar[cn][0];
					if( ucs4cmp( keyChar, enableChar, 10) == 0 ){//キーキャラクタは最大１０文字分
						break;
					}
				}
				if( cn == pEnable->uiCharNum ){ // キーキャラクタが許可文字列リストに含まれていなかったら
					pInhibit   = &inhibitChar;    //入力禁止文字登録先
					inhibitMax = INHIBIT_CHAR_MAX;
					if( inhibitMaskChar.uiCharNum ){ // 入力禁止だが、キー入力は可の文字列リストがある場合
						for(cn=0;cn<inhibitMaskChar.uiCharNum;cn++){ // （濁点などの付加文字用リスト）
							if( ucs4cmp( keyChar, &inhibitMaskChar.pChar[cn][0], 10)==0 ) break;
						}
						if( cn < inhibitMaskChar.uiCharNum ){ //該当した場合、登録先を
							pInhibit = &inhibitSubChar;       //キー入力可の入力禁止文字バッファに
							inhibitMax = INHIBIT_SUB_CHAR_MAX;//変更する。
						}
					}

					for( cn=0; cn< pInhibit->uiCharNum; cn++ ){ //禁止キャラクタの重複チェック
						if( ucs4cmp( keyChar, &pInhibit->pChar[cn][0], 10 )==0 )	break;
					}
					if( pInhibit->uiCharNum==0 || cn==pInhibit->uiCharNum ){// 禁止キャラクタが重複していない
						// 禁止キャラクタに追加
						if( pInhibit->uiCharNum < inhibitMax ){//禁止文字列バッファの最大数を超えていなければ
							ucs4cpy( &pInhibit->pChar[pInhibit->uiCharNum][0], keyChar, 10 );
							pInhibit->uiCharNum ++; //禁止文字列登録。
						}
					}
				}
			}
		}
	}

	// 作成した禁止文字列から、定型文 http:// などのキーも、禁止文字使用キーは禁止設定に。
	SKB_inhibitDisable_stringKey();

	Printf("inhibitChar count=%d *******************\n", inhibitChar.uiCharNum);

	n = ThreadID_SKBWork;
	ThreadID_SKBWork=-1;
	if( n!=-1 )	ExitDeleteThread(); //スレッド呼び出しの時は、スレッド削除。
}

// 禁止文字情報から入力禁止文字情報を作成する内部関数
static void SKB_inhibitSet_Disable( inhibitChar_t *pDisable )
{
	int n,cn;
	u_int *keyChar;
	inhibitChar_t *pInhibit;
	int inhibitMax;

	inhibitChar.uiCKeyNum = 0;
	inhibitChar.pCtrlKey  = NULL;
	inhibitChar.uiCharNum = 0;
	inhibitChar.pChar     = inhibitUCS4;
	inhibitSubChar.uiCKeyNum = 0;
	inhibitSubChar.pCtrlKey  = NULL;
	inhibitSubChar.uiCharNum = 0;
	inhibitSubChar.pChar     = inhibitSubUCS4;

	Printf("disableChar count %d, %p \n",pDisable->uiCharNum,(void *)pDisable);

	for(n=0;n<pDisable->uiCharNum;n++){
		keyChar = &pDisable->pChar[n][0];

		pInhibit   = &inhibitChar;    //入力禁止文字登録先
		inhibitMax = INHIBIT_CHAR_MAX;

		if( inhibitMaskChar.uiCharNum ){ // 入力禁止だが、キー入力は可の文字列リストがある場合
			for(cn=0;cn<inhibitMaskChar.uiCharNum;cn++){ // （濁点などの付加文字用リスト）
				if( ucs4cmp( keyChar, &inhibitMaskChar.pChar[cn][0], 10)==0 ) break;
			}
			if( cn < inhibitMaskChar.uiCharNum ){ //該当した場合、登録先を
				pInhibit = &inhibitSubChar;       //キー入力可の入力禁止文字バッファに
				inhibitMax = INHIBIT_SUB_CHAR_MAX;//変更する。
			}
		}

		for( cn=0; cn< pInhibit->uiCharNum; cn++ ){ //禁止キャラクタの重複チェック
			if( ucs4cmp( keyChar, &pInhibit->pChar[cn][0], 10 )==0 )	break;
		}
		if( pInhibit->uiCharNum==0 || cn==pInhibit->uiCharNum ){// 禁止キャラクタが重複していない
			// 禁止キャラクタに追加
			if( pInhibit->uiCharNum < inhibitMax ){//禁止文字列バッファの最大数を超えていなければ
				ucs4cpy( &pInhibit->pChar[pInhibit->uiCharNum][0], keyChar, 10 );
				pInhibit->uiCharNum ++; //禁止文字列登録。
			}
		}
	}
	// 作成した禁止文字列から、定型文 http:// などのキーも、禁止文字使用キーは禁止設定に。
	SKB_inhibitDisable_stringKey();

	Printf("inhibitChar count=%d *******************\n", inhibitChar.uiCharNum);
}
// 許可文字情報から入力禁止文字情報を作成する内部関数
static void SKB_inhibitSet_EnableToDisable( inhibitChar_t *pEnable )
{
	EnableChar.uiCKeyNum = 0;
	EnableChar.pCtrlKey  = NULL;
	EnableChar.uiCharNum = 0;
	EnableChar.pChar     = EnableUCS4;

	if( !pEnable )	return;
	SKB_inhibitCopy( pEnable, &EnableChar, INHIBIT_CHAR_MAX );

	{
		struct ThreadParam t_info;
		struct ThreadParam tp;
		int prio;

		ReferThreadStatus( GetThreadId(), &t_info );
		prio=t_info.currentPriority;
		if( prio < 127-1 )  prio+=1;

		tp.entry		  = SKB_thread_InhibitEnableToDisable; // スレッドのエントリ関数
		tp.stack		  = stack;                             // スタックエリアの先頭アドレス
		tp.stackSize	  = sizeof(stack);                     // スタックエリアのサイズ
		tp.initPriority	  = prio;                              // スレッドプライオリティ
		tp.gpReg		  = &_gp;                              // GPレジスタ値

		ThreadID_SKBWork = CreateThread(&tp);    // スレッドを生成
		if( ThreadID_SKBWork == -1 ){            // スレッド作成失敗
			SKB_thread_InhibitEnableToDisable(); // しかたないので、直呼び
			return;
		}
		StartThread( ThreadID_SKBWork, 0 );
	}
}


//  入力禁止文字ではあるが、画面キーボードから入力できる文字列を設定。（濁点、半濁点など付加文字用）
void SKB_SetInhibit_MaskChar( u_int (*ucs4)[10], int count )
{
	inhibitChar_t inhibitWork;

	inhibitWork.uiCKeyNum = 0;
	inhibitWork.pCtrlKey  = NULL;
	inhibitWork.uiCharNum = count;
	inhibitWork.pChar     = ucs4;

	inhibitMaskChar.uiCKeyNum = 0;
	inhibitMaskChar.pCtrlKey  = NULL;
	inhibitMaskChar.uiCharNum = 0;
	inhibitMaskChar.pChar     = inhibitMaskUCS4;

	SKB_inhibitCopy( &inhibitWork, &inhibitMaskChar, INHIBIT_SUB_CHAR_MAX );
}

//  禁止文字情報の設定
void SKB_SetInhibit_Disable( u_int (*ucs4)[10], int count )
{
	inhibitChar_t inhibitWork;

	inhibitWork.uiCKeyNum = 0;
	inhibitWork.pCtrlKey  = NULL;
	inhibitWork.uiCharNum = count;
	inhibitWork.pChar     = ucs4;

	// 許可文字情報を禁止情報バッファに作成。
	SKB_inhibitSet_Disable( &inhibitWork );
	inhibitChar_SetAllKeyborad();  // キーボード全部に禁止文字列情報を設定
}

//  禁止文字情報の設定（入力、許可文字情報版）
void SKB_SetInhibit_Enable( u_int (*ucs4)[10], int count )
{
	inhibitChar_t inhibitWork;

	inhibitWork.uiCKeyNum = 0;
	inhibitWork.pCtrlKey  = NULL;
	inhibitWork.uiCharNum = count;
	inhibitWork.pChar     = ucs4;

	// 許可文字情報から禁止文字情報を禁止情報バッファに作成。
	SKB_inhibitSet_EnableToDisable( &inhibitWork );
	inhibitChar_SetAllKeyborad();  // キーボード全部に禁止文字列情報を設定
}


// その文字コードが、禁止文字であるか、判定
//  返り値   0:許可文字   1:禁止文字
int SKB_IsDisableUcs4( u_int ucs4 )
{
	static int testCode[2];
	int n;

	testCode[0] = ucs4;
	for( n=0; n < inhibitChar.uiCharNum ;n++ ){
		if( ucs4cmp( testCode, inhibitChar.pChar[n] ,10 )==0 ){
			Printf("out %c\n", inhibitChar.pChar[n][0]);
			return 1;
		}
	}
	for( n=0; n < inhibitSubChar.uiCharNum ;n++ ){
		if( ucs4cmp( testCode, inhibitSubChar.pChar[n] ,10 )==0 ){
			Printf("out %c\n", inhibitSubChar.pChar[n][0]);
			return 1;
		}
	}

	return 0;
}

// 終了
void SKB_Exit(void)
{
	int i;

	if( ThreadID_SKBWork != -1 ){        // スレッドが残っていたら削除
		TerminateThread( ThreadID_SKBWork );
		DeleteThread( ThreadID_SKBWork );
		ThreadID_SKBWork=-1;
	}

	// ソフトウェアキーボード関係終了処理
	for(i=0; i<SKBTYPE_MAX; i++)
	{
		SoftKB_Close(&g_Sample.KBD[i]);  // キーボードClose
		SoftKB_Delete(&g_Sample.KBD[i]);
	}
	SoftKB_Destroy();

	TextF_Delete(&textField);
	TextF_Destroy();

	Fep_Exit();
	Fepbar_Destroy();
}

//--------------------------------------------------------------------------------------
