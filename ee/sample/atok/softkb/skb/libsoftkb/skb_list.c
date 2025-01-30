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

//	kigo.bin の[Capslock],[半/全]のボタンは常に無効なのでデータによって無効にされています。

#include <sys/types.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include <eekernel.h>
#include <eetypes.h>
#include <sifdev.h>
#include <libpkt.h>
#include <libccc.h>
#include <libpad.h>

#include "libsoftkb.h"
#include "sharedskb.h"

// 描画
#define CHAR_STARTDRAW_POSX		(22)		// 文字描画エリアからの文字描画開始X位置
#define CHAR_STARTDRAW_POSY		(2)			// 文字描画エリアからの文字描画開始Y位置
#define CHAR_TOPLEFT_POSX		(10)		// ソフトウェアキーボードの左上からのX位置
#define CHAR_TOPLEFT_POSY		(7)			// ソフトウェアキーボードの左上からのY位置
#define TEXTPOS_ADDY			(8)			// 文字間

// ボタン
#define BUTTON_CTRL_GLOBALPOSX	(16)		// コントロール部分のボタン
#define BUTTON_CTRL_GLOBALNUMV	(5)			// コントロール部分のアイテム数
#define BUTTON_HELP_GLOBALPOSX	(17)		// ヘルプ部分のボタン
#define BUTTON_HELP_GLOBALNUMV	(1)			// ヘルプ列のアイテム数
#define DEFAULT_FOCUS_X			(0)			// デフォルト フォーカスX値
#define DEFAULT_FOCUS_Y			(0)			// デフォルト フォーカスY値
#define FOCUS_MARGINX			(0)			// フォーカスマージンX値
#define FOCUS_MARGINY			(1)			// フォーカスマージンY値

// SCROLL
#define REPEAT_START_TIME		(20)		// リピート開始時間
#define LIST_SCROLL_SPEED4		(4)			// スクロールスピード 4
#define LIST_SCROLL_SPEED2		(2)			//                    2
#define LIST_SCROLL_SPEED1		(1)			//                    1
#define LIST_SCROLL_START		(30)		// スクロールするまでの時間(ウェイト)
#define LIST_ACCEL_MAX			(200)		// アクセル最大値
#define LIST_ACCEL_50			(50)		// アクセル  50
#define LIST_ACCEL_100			(100)		// アクセル 100
#define LIST_ACCEL_150			(150)		// アクセル 150
#define LIST_ACCEL_200			(200)		// アクセル 200

// プロトタイプ宣言
extern gImport_t		*g_pSkbImport;		// メイン側関数群のポインタ
extern protectedSkb_t	g_protectSkb;		// 


// トグル
#define TOGGLE_SP_OFFX	(10)
#define TOGGLE_SP_OFFY	(-4)
#define TOGGLE_SP_W		(13)
#define TOGGLE_SP_H		(26)
uv_t		g_uvToggleSrc = {STG(169),	STG(208),	STG(13),	STG(26)};
sprite_t	g_spToggleSrc = {TOGGLE_SP_OFFX, TOGGLE_SP_OFFY, 0, TOGGLE_SP_W, TOGGLE_SP_H};


void setFocus(skey_t *pKey, int x, int y);
static void pushKey(skey_t *pKey);

static int readResource(skey_t *pKey, const char *filename, u_int keyTopNo, inhibitChar_t *inhibitChar, void *pCharCodeBlock);
static void makePackets_listBody(sceVif1Packet *pk, skey_t *pKey,
				 textureInfo_t *pTexBtn, textureInfo_t *pTexChar, textureInfo_t *pTexBg);
static void makePackets_cursor(sceVif1Packet *pk, skey_t *pKey, textureInfo_t *pTexCur);
static void makeChars(sceVif1Packet *pk, skey_t *pKey);
int setSpriteCtrl(skey_t *pKey, u_short ctrlCode, sprite_t *pSp);
static void makeToggle(sceVif1Packet *pk, skey_t *pKey, textureInfo_t *pTex);


static void ctrlPad(skey_t *pKey, u_short down, u_short up, u_short press, u_short repeat);
static void padDown(skey_t *pKey, u_short down, u_short press, u_short repeat);
static void padUp(skey_t *pKey, u_short up);
static void scrollCheck(skey_t *pKey);
static int  scrollChars(skey_t *pKey, u_short press);
static void moveCursorList(skey_t *pKey, int dx, int dy);
static int  addChar(skey_t *pKey);

static void initKeyTop(skey_t *pKey);

static void loadUtf8Cache(skey_t *pKey, int startLine, int lineNum);
static void shiftUtf8Cache(skey_t *pKey, int shift);
static UTF8Char_t getUtf8CharFromPos(skey_t *pKey, int x, int y);
static void getSJISCharFromPos(skey_t *pKey, int x, int y, char *pBufSjis);
static inline int checkIgnoreSJISCode(u_int code);

static void moveMouseCursor(skey_t *pKey, iPoint_t *pPoint);
static int searchListByPoint(skey_t *pKey, iPoint_t *pPt);
static void leftButtonDown_list(skey_t *pKey, iPoint_t *pPoint);
static void leftButtonUp_list(skey_t *pKey, iPoint_t *pPoint);
int searchDummyKeyByIndex(skey_t *pKey, int index);
static u_int searchFocus(skey_t *pKeyNext, sprite_t *pNowSp);

static void open(skey_t *pKey);
static void close(skey_t *pKey);

//============================================================================
// PUBLIC FUNCTIONS
//============================================================================
//============================================================================
//  SkbList_Init											ref:  skb_core.h
//
//  通常ソフトウェアキーボード初期化
//  自分の提供できる関数を登録
//
//  exportSkb_t *pExport : 提供関数を登録する為のポインタ
//
//  戻り値 ： void
void SkbList_Init(exportSkb_t *pExport)
{
	SKB_ASSERT(pExport);

	// 使われる関数ポインタを設定
	pExport->readResource       = readResource;
	pExport->makePacketsSkbBody = makePackets_listBody;
	pExport->makePacketsCursor  = makePackets_cursor;

	pExport->setPad   = ctrlPad;
	pExport->mouseMove        = moveMouseCursor;
	pExport->mouseLButtonDown = leftButtonDown_list;
	pExport->mouseLButtonUp   = leftButtonUp_list;

	pExport->initKeyTop      = initKeyTop;

	pExport->searchFocus = searchFocus;


	pExport->open  = open;
	pExport->close = close;
}


//============================================================================
// STATIC FUNCTIONS
//============================================================================
//============================================================================
//  readResourceFrame
//
//  キー配列定義リソース読込み
//
//	skey_t *pKey : ソフトウェアキーボード構造体へのポインタ
//	const char *filename : キー配列定義リソースファイル名
//	u_int keyTopNo : キートップの番号
//	inhibitChar_t *inhibitChar : 入力禁止文字列
//	void *pTmp : 表示文字列
//
//	戻り値 : int : 0:成功/1:エラー
static int readResource(skey_t *pKey, const char *filename, u_int keyTopNo, inhibitChar_t *inhibitChar, void *pCharCodeBlock)
{

	SKB_ASSERT(pKey);
	SKB_ASSERT(filename);
	SKB_ASSERT(inhibitChar);
	SKB_ASSERT(pCharCodeBlock);

	memset(&pKey->List, 0, sizeof(skblist_t));

	pKey->List.pListItem = (charCodeBlock_t*)pCharCodeBlock;
	pKey->List.iUtf8CacheTopLine = -1;

	pKey->uiCursorPos = 0;

	readResourceFrame(pKey, filename, keyTopNo, inhibitChar, pKey->List.pListItem);

	// 行NO->文字コードに変更
	{
		int i=0, j=0;
		int line = 0;
		while(pKey->List.pListItem[i].start != 0x0000 ||
			  pKey->List.pListItem[i].end != 0x0000)
		{
			j = 0;
			for(j=pKey->List.pListItem[i].start; j<pKey->List.pListItem[i].end; j+=VISIBLE_CHARNUM_H)
			{
				pKey->List.auiCodeTable[line] = j;
				line++;
			}

			i++;
		}
		pKey->List.iLineNum = line;
	}


	setFocus(pKey, 0, 0);

	return 0;
}


//============================================================================
//	makePackets_listBody
//
//  ソフトウェアキーボードリスト描画パケット作成
//
//	sceVif1Packet *pk : VIFパケット格納用バッファへのポインタ
//	skey_t *pKey : ソフトウェアキーボード構造体へのポインタ
//	textureInfo_t *pTexBtn : ボタンテクスチャへのポインタ
//	textureInfo_t *pTexChar: 文字テクスチャへのポインタ
//	textureInfo_t *pTexBg  : 背景テクスチャへのポインタ
static void makePackets_listBody(sceVif1Packet *pk, skey_t *pKey,
				 textureInfo_t *pTexBtn, textureInfo_t *pTexChar, textureInfo_t *pTexBg)
{
	SKB_ASSERT(pk);
	SKB_ASSERT(pKey);
	SKB_ASSERT(pTexBtn);
	SKB_ASSERT(pTexChar);
//	SKB_ASSERT(pTexBg);			// BGが要らない場合もある


	makePacketsBody(pk, pKey, pTexBtn, pTexChar, pTexBg);	// ボディ部分描画
	makeToggle(pk, pKey, pTexBtn);
	makeChars(pk, pKey);									// 文字描画
}


//============================================================================
//	makeToggle
//
//	トグルボタン描画パケット作成
//
//	sceVif1Packet *pk : VIFパケット格納用バッファへのポインタ
//	skey_t *pKey : ソフトウェアキーボード構造体へのポインタ
//	textureInfo_t *pTexBtn : トグルテクスチャへのポインタ
static void makeToggle(sceVif1Packet *pk, skey_t *pKey, textureInfo_t *pTex)
{
	int col;
	if(pKey->List.iGlobalCurPosX != BUTTON_CTRL_GLOBALPOSX &&
	   pKey->List.iGlobalCurPosX != BUTTON_HELP_GLOBALPOSX)
	{
		//トグル移動
		int scrlY  = pKey->List.iScrollPosY;
		int charH  = g_pSkbImport->skb.uiListFontH;
		int posY16 = (((g_pSkbImport->thinkYH16(charH)*VISIBLE_CHARNUM_V)*scrlY)/(pKey->List.iLineNum-VISIBLE_CHARNUM_V));

		pKey->List.iOldPosY16 = posY16;

		pKey->List.spToggle16.x = g_pSkbImport->thinkXW16(g_spToggleSrc.x) + pKey->uiKbdPosX;
		pKey->List.spToggle16.y = g_pSkbImport->thinkYH16(g_spToggleSrc.y) + pKey->List.iOldPosY16 + pKey->uiKbdPosY;
		pKey->List.spToggle16.z = 0;
		pKey->List.spToggle16.w = g_pSkbImport->thinkXW16(g_spToggleSrc.w);
		pKey->List.spToggle16.h = g_pSkbImport->thinkYH16(g_spToggleSrc.h);

		pKey->List.iOldPosY16 = posY16;
		g_protectSkb.uiTogglePos16 = pKey->List.spToggle16.y;		// 一旦トグルの位置を退避
	}
	else
	{
		pKey->List.spToggle16.x = g_pSkbImport->thinkXW16(g_spToggleSrc.x) + pKey->uiKbdPosX;
		pKey->List.spToggle16.y = g_pSkbImport->thinkYH16(g_spToggleSrc.y) + pKey->List.iOldPosY16 + pKey->uiKbdPosY;
		pKey->List.spToggle16.z = 0;
		pKey->List.spToggle16.w = g_pSkbImport->thinkXW16(g_spToggleSrc.w);
		pKey->List.spToggle16.h = g_pSkbImport->thinkYH16(g_spToggleSrc.h);
	}

	col = affectAlpha(0x80808080, 0x80);
	g_pSkbImport->loadTex(pk, pTex);

	g_pSkbImport->drawTex(pk, col, &pKey->List.spToggle16, &g_uvToggleSrc, pTex);
}


//============================================================================
//	refreshUtf8Cache
//
//	UTF8に変換された文字コードキャッシュを更新
//
//	skey_t *pKey : ソフトウェアキーボード構造体へのポインタ
static void refreshUtf8Cache(skey_t *pKey)
{
	int iTopLine = pKey->List.iScrollPosY - pKey->List.iUtf8CacheTopLine;

	if(pKey->List.iUtf8CacheTopLine == -1)
	{
		pKey->List.iUtf8CacheTopLine = pKey->List.iScrollPosY;
		loadUtf8Cache(pKey, pKey->List.iUtf8CacheTopLine, VISIBLE_CHARNUM_V);
	}
	else if(iTopLine > 0)
	{
		shiftUtf8Cache(pKey, iTopLine);
		pKey->List.iUtf8CacheTopLine = pKey->List.iScrollPosY;
		loadUtf8Cache(pKey, pKey->List.iUtf8CacheTopLine+VISIBLE_CHARNUM_V-iTopLine, iTopLine);
	}
	else if(iTopLine < 0)
	{
		shiftUtf8Cache(pKey, iTopLine);
		pKey->List.iUtf8CacheTopLine = pKey->List.iScrollPosY;
		loadUtf8Cache(pKey, pKey->List.iUtf8CacheTopLine, -iTopLine);
	}
}


//============================================================================
//	makeChars
//
//	文字を描画
//
//	sceVif1Packet *pk : VIFパケット格納用バッファへのポインタ
//	skey_t *pKey : ソフトウェアキーボード構造体へのポインタ
static void makeChars(sceVif1Packet *pk, skey_t *pKey)
{
	// UTF8キャッシュを更新
	refreshUtf8Cache(pKey);

	// 文字パケット作成
	{
		int x, y;
		int globalX, globalY;
		int charPacketLocalPosX, charPacketLocalPosY;
		int centeringOffsetX;
		int charW = g_pSkbImport->skb.uiListFontW;
		int charH = g_pSkbImport->skb.uiListFontH;
		int dx, dy;
		int cx, cy;
		UTF8Char_t utf8Char;

		cx = g_pSkbImport->thinkXW(pKey->uiKbdPosX) + CHAR_STARTDRAW_POSX + CHAR_TOPLEFT_POSX;							// 文字書き始めOFFSET X
		cy = g_pSkbImport->thinkYH(pKey->uiKbdPosY) + g_pSkbImport->thinkYH(CHAR_STARTDRAW_POSY + CHAR_TOPLEFT_POSY);	// 文字書き始めOFFSET Y
		g_pSkbImport->skb.setListFontEnv(cx, cy);		// 文字の初期設定をする

		for(y=0; y<VISIBLE_CHARNUM_V; y++)
		{
			for(x=0; x<VISIBLE_CHARNUM_H; x++)
			{
				globalX = x;
				globalY = y + pKey->List.iScrollPosY;
				charPacketLocalPosX = (x * charW) + cx;
				charPacketLocalPosY = (y * charH) + cy;

				utf8Char = pKey->List.utf8Cache[(y * VISIBLE_CHARNUM_H) + x].utf8Char;
				charPacketLocalPosY = g_pSkbImport->thinkYHn(charPacketLocalPosY);

				centeringOffsetX = (charW - pKey->List.utf8Cache[(y * VISIBLE_CHARNUM_H) + x].iFontWidth) >> 1;
				dx = charPacketLocalPosX + centeringOffsetX;
				dy = charPacketLocalPosY + g_pSkbImport->thinkYHn(TEXTPOS_ADDY);

				if(globalX == pKey->List.iGlobalCurPosX &&
				   globalY == pKey->List.iGlobalCurPosY)
				{
					sprite_t sp16;
					g_pSkbImport->skb.setListDrawStringF(pk, utf8Char.buf, dx, dy);

					// フォーカススプライト設定
					if(!strlen(utf8Char.buf))
					{
						// 文字が無い部分はXの位置が文字幅の半分の位置になってしまうので
						// 文字幅の半分差し引いた値にする
						sp16.x = g_pSkbImport->thinkXW16(dx+FOCUS_MARGINX) - (g_pSkbImport->thinkXW16(charW)/2);
					}
					else
					{
						// これは、普通に書く為、NTSC/PALやFRAME/FIELDを考えなければならない。
						sp16.x = g_pSkbImport->thinkXW16(dx+FOCUS_MARGINX);
					}
					sp16.y = (dy<<4) + g_pSkbImport->thinkYH16(FOCUS_MARGINY);
					sp16.z = 0;
					sp16.w = g_pSkbImport->thinkXW16(charW);
					sp16.h = g_pSkbImport->thinkYH16(charH);
					makeCursorSprite(&sp16, pKey->spCursor, pKey->iCursorPos);
				}
				else
				{
					g_pSkbImport->skb.setListDrawString(pk, utf8Char.buf, dx, dy);
				}
			}
		}
	}
}


//============================================================================
//	loadUtf8Cache
//
//	UTF8に変換された文字列をキャッシュにロード
//
//	skey_t *pKey : ソフトウェアキーボード構造体へのポインタ
//	int startLine : ロードし始めの行番号
//	int lineNum  : ロードする行数
static void loadUtf8Cache(skey_t *pKey, int startLine, int lineNum)
{
	int count = 0;
	int line, i;
	int topLine = pKey->List.iUtf8CacheTopLine;
	char *pBuf = NULL;

	for(line=startLine; line<(startLine+lineNum); line++)
	{
		for(i=0; i<VISIBLE_CHARNUM_H; i++)
		{
			pKey->List.utf8Cache[(line - topLine) * VISIBLE_CHARNUM_H + i].utf8Char = getUtf8CharFromPos(pKey, i, line);
			pBuf = pKey->List.utf8Cache[(line - topLine) * VISIBLE_CHARNUM_H + i].utf8Char.buf;
			pKey->List.utf8Cache[(line - topLine) * VISIBLE_CHARNUM_H + i].iFontWidth = g_pSkbImport->skb.getStringWidth(pBuf);
			count++;
		}
	}
}


//============================================================================
//	shiftUtf8Cache
//
//	UTF8に変換された文字キャッシュを移動
//
//	skey_t *pKey : ソフトウェアキーボード構造体へのポインタ
//	int shift : 移動分
static void shiftUtf8Cache(skey_t *pKey, int shift)
{
	if(shift > 0)
	{
		int i, j;
		for(i=0; i<VISIBLE_CHARNUM_V-shift; i++)
		{
			for(j=0; j<VISIBLE_CHARNUM_H; j++)
			{
				pKey->List.utf8Cache[i * VISIBLE_CHARNUM_H + j] = pKey->List.utf8Cache[(i+shift) * VISIBLE_CHARNUM_H + j];
			}
		}
	}
	else if(shift < 0)
	{
		int i, j;
		for(i=VISIBLE_CHARNUM_V-1; i>=(-shift); i--)
		{
			for(j=0; j<VISIBLE_CHARNUM_H; j++)
			{
				pKey->List.utf8Cache[i * VISIBLE_CHARNUM_H + j] = pKey->List.utf8Cache[(i+shift) * VISIBLE_CHARNUM_H + j];
			}
		}
	}
}


//============================================================================
//	getUtf8CharFromPos
//
//	画面上のX,Y値からUTF8の文字コードを取得
//
//	skey_t *pKey : ソフトウェアキーボード構造体へのポインタ
//	int x : X値
//	int y : Y値
//
//	戻り値 : UTF8Char_t : 文字情報
static UTF8Char_t getUtf8CharFromPos(skey_t *pKey, int x, int y)
{
	UTF8Char_t ret;
	char acBufSjis[4];
	u_short code = pKey->List.auiCodeTable[y] + x;

	if(checkIgnoreSJISCode(code))
	{
		acBufSjis[0] = '\0';
	}
	else
	{
		if(code < 0x00ff)
		{
			// 半角文字
			acBufSjis[0] = (char)code;
			acBufSjis[1] = '\0';
		}
		else
		{
			// 全角文字
			acBufSjis[0] = ((char*)&code)[1];
			acBufSjis[1] = ((char*)&code)[0];
			acBufSjis[2] = '\0';
		}
	}

	sceCccSJIStoUTF8(ret.buf, sizeof(ret.buf), acBufSjis);
	return ret;
}


//============================================================================
//	getSJISCharFromPos
//
//	位置情報から、SJIS文字コードを取得
//
//	skey_t *pKey : ソフトウェアキーボード構造体へのポインタ
//	int x : X値
//	int y : Y値
//	char *pBufSjis : SJIS文字コード格納用バッファ
static void getSJISCharFromPos(skey_t *pKey, int x, int y, char *pBufSjis)
{
	u_short code = pKey->List.auiCodeTable[y] + x;
	if(checkIgnoreSJISCode(code))
	{
		pBufSjis[0] = '\0';
	}
	else
	{
		if(code < 0x00ff)
		{
			// 半角文字
			pBufSjis[0] = (char)code;
			pBufSjis[1] = '\0';
		}
		else
		{
			// 全角文字
			pBufSjis[0] = ((char*)&code)[1];
			pBufSjis[1] = ((char*)&code)[0];
			pBufSjis[2] = '\0';
		}
	}
}


//============================================================================
//	checkIgnoreSJISCode
//
//	一般的なフォントに入ってる/いないチェック
//
//	u_int code : チェック用文字コード
//
//	戻り値 : int : 0:入っていない/1:入っている
static inline int checkIgnoreSJISCode(u_int code)
{
	if( code >= 0x81ad  &&  code <= 0x81b7 ){ return 1; }
	if( code >= 0x81c0  &&  code <= 0x81c7 ){ return 1; }
	if( code >= 0x81cf  &&  code <= 0x81d9 ){ return 1; }
	if( code >= 0x81e9  &&  code <= 0x81ef ){ return 1; }
	if( code >= 0x81f8  &&  code <= 0x81fb ){ return 1; }
	if( code >= 0x81fd  &&  code <= 0x81ff ){ return 1; }

	if( code >= 0x8240  &&  code <= 0x824e ){ return 1; }
	if( code >= 0x8259  &&  code <= 0x825f ){ return 1; }
	if( code >= 0x827a  &&  code <= 0x8280 ){ return 1; }
	if( code >= 0x829b  &&  code <= 0x829e ){ return 1; }
	if( code >= 0x82f2  &&  code <= 0x82ff ){ return 1; }

	if( code >= 0x8397  &&  code <= 0x839e ){ return 1; }
	if( code >= 0x83b7  &&  code <= 0x83be ){ return 1; }
	if( code >= 0x83d7  &&  code <= 0x83ff ){ return 1; }

	if( code >= 0x8461  &&  code <= 0x846f ){ return 1; }
	if( code >= 0x8492  &&  code <= 0x849e ){ return 1; }
	if( code >= 0x84bf  &&  code <= 0x86ff ){ return 1; }

	if( code == 0x875e ){ return 1; }
	if( code >= 0x8776  &&  code <= 0x877d ){ return 1; }
	if( code >= 0x8780  &&  code <= 0x8781 ){ return 1; }
	if( code >= 0x8790  &&  code <= 0x8792 ){ return 1; }
	if( code >= 0x8794  &&  code <= 0x879f ){ return 1; }

	if( code >= 0x8890  &&  code <= 0x889e ){ return 1; }

	return 0;
}



//============================================================================
//  ctrlPad
//
//	パッド処理
//
//	skey_t *pKey : ソフトウェアキーボード構造体へのポインタ
//	u_short down : ボタン押し始め情報
//	u_short up   : ボタン解除情報
//	u_short press : ボタンの押下情報
//	u_short repeat: ボタンのリピート情報
static void ctrlPad(skey_t *pKey, u_short down, u_short up, u_short press, u_short repeat)
{
	SKB_ASSERT(pKey);

	padDown(pKey, down, press, repeat);
	padUp(pKey, up);
}


//============================================================================
//	padDown
//
//	パッド押下処理
//
//	skey_t *pKey : ソフトウェアキーボード構造体へのポインタ
//	u_short down : ボタン押し始め情報
//	u_short press : ボタンの押下情報
//	u_short repeat: ボタンのリピート情報
static void padDown(skey_t *pKey, u_short down, u_short press, u_short repeat)
{
	SKB_ASSERT(pKey);

	if(g_pSkbImport->isConv())
	{
		// 変換中の時は、操作できない
		return;
	}

	if(down & SCE_PADselect)
	{
		pKey->uiPush = 0;								// ボタン押下フラグをRELEASEにする
		g_pSkbImport->skb.keytopChange(SKBMODE_NOHIT);	// キートップ変更用コールバック呼び出し
		return;
	}

	// ボタン押し始め情報
	if(down & SCE_PADL1)
	{
	}
	if((down & SCE_PADL2) || (press & SCE_PADL2))
	{
		onStateCtrlCode(pKey, SKBCTRL_LEFT, KEY_PUSH);
	}

	if((down & SCE_PADR1) || (press & SCE_PADR1))
	{
		pushShift(pKey, 0);
	}

	if((down & SCE_PADR2) || (press & SCE_PADL2))
	{
		onStateCtrlCode(pKey, SKBCTRL_RIGHT, KEY_PUSH);
	}
	if((down & SCE_PADRup) || (press & SCE_PADRup))
	{
		u_int uiTmp[] = {0x00003000, 0x00000000};
		onStateKey(pKey, uiTmp, KEY_PUSH);
	}
	if((down & SCE_PADRdown) || (press & SCE_PADRdown)){}
	if((down & SCE_PADRleft) || (press & SCE_PADRleft))
	{
		onStateCtrlCode(pKey, SKBCTRL_BACKSPACE, KEY_PUSH);
	}
	if(down & SCE_PADstart)
	{
		#if 0//SKB_EX
		if(g_pSkbImport->skb.getAtokInput())
		#else
		if( g_pSkbImport->isConv() )
		#endif
		{
			// ATOKで入力があるなら全文確定を設定
			g_pSkbImport->skb.kakuteiAll();
			#if 1//SKB_EX Sound
			SoftKB_se_play(SKBSE_DETECTEXIT);
			#endif
		}
		else
		{
			pushEnter(pKey);
		}
		onStateCtrlCode(pKey, SKBCTRL_ENTER, KEY_PUSH);
		g_pSkbImport->skb.exitOK();						// EXIT
		return;
	}
	// ボタンのリピート情報
	if(repeat & SCE_PADL1){}
	if(repeat & SCE_PADL2)
	{
		pushLeftArrow(pKey);	// ←
	}
	if(repeat & SCE_PADR1){}
	if(repeat & SCE_PADR2)
	{
		pushRightArrow(pKey);	// →
	}
	if(repeat & SCE_PADRup)
	{
		pushSpace(pKey);		// SPACE
	}
	if(repeat & SCE_PADRdown)
	{
		pushEsc(pKey);			// ESC
	}
	if(repeat & SCE_PADRleft)
	{
		pushBackSpace(pKey);	// BACKSPACE
	}
	if(repeat & SCE_PADRright)
	{
		pushKey(pKey);
		pKey->uiPush = 1;		// カーソル押下中フラグをON
	}
	if(repeat & SCE_PADselect){}
	if(repeat & SCE_PADstart){}

	// カーソル移動
	{
		int dx=0, dy=0;
		u_short updown=0;

		if(repeat & SCE_PADLup)
		{
			dy -= 1;
		}
		if(repeat & SCE_PADLdown)
		{
			dy += 1;
		}
		updown = scrollChars(pKey, press);

		// 長く押しつづけた時はさらに早くなる
		// 2回カーソル動かす
		if(pKey->List.iRepeatAccel >= LIST_ACCEL_150 &&
		   (pKey->List.iGlobalCurPosX != BUTTON_CTRL_GLOBALPOSX) &&
		   (pKey->List.iGlobalCurPosX != BUTTON_HELP_GLOBALPOSX))
		{
			if(updown & SCE_PADLup)
			{
				dy -= 1;
			}
			if(updown & SCE_PADLdown)
			{
				dy += 1;
			}
		}

		if(repeat & SCE_PADLleft)
		{
			dx -= 1;
		}
		if(repeat & SCE_PADLright)
		{
			dx += 1;
		}

		moveCursorList(pKey, dx, dy);
	}
}


//============================================================================
//	addChar
//
//	文字が選択されたので文字コードを送る
//
//	skey_t *pKey : ソフトウェアキーボード構造体へのポインタ
//
//	戻り値: int : 0:成功/1:エラー
static int addChar(skey_t *pKey)
{
	char bufSjis[4];
	getSJISCharFromPos(pKey, pKey->List.iGlobalCurPosX, pKey->List.iGlobalCurPosY, bufSjis);

	{
		// SJIS -> UTF8
		UTF8Char_t utf8Char = getUtf8CharFromPos(pKey, pKey->List.iGlobalCurPosX, pKey->List.iGlobalCurPosY);
		if(g_pSkbImport->skb.getStringWidth(utf8Char.buf) <= 0)
		{
			g_pSkbImport->dprintf("ERROR : addChar - invalid char\n");
			return 1;
		}
	}


	if(bufSjis[0] == 0x00)
	{
		// 文字がない
		return 1;
	}

	// SJIS -> UCS4
	{
		sceCccJISCS jis;
		sceCccUCS4 chr=0;
		sceCccSJISTF const *pSjis = (sceCccSJISTF *)bufSjis;
		u_int iBuf[4];
		int cnt=0, i=0;

		while(*pSjis)
		{
			// SJIS -> UCS4
			jis = sceCccDecodeSJIS(&pSjis);
			iBuf[i] = sceCccJIStoUCS(jis, chr);
			cnt++;
		}

		g_pSkbImport->skb.setChars(iBuf, ((pKey->ucShift&LAYOUT_SHIFT) ? 1:0), 0);
	}

	return 0;
}


//============================================================================
//	makePackets_cursor
//
//	カーソル描画パケット作成
//
//	sceVif1Packet *pk : VIFパケット格納用ポインタ
//	skey_t *pKey : ソフトウェアキーボード構造体へのポインタ
//	textureInfo_t *pTexCur : カーソルテクスチャへのポインタ
static void makePackets_cursor(sceVif1Packet *pk, skey_t *pKey, textureInfo_t *pTexCur)
{
	sprite_t sp16;
	pKey->uiCursorPos = -1;
	if(pKey->List.iGlobalCurPosX == BUTTON_HELP_GLOBALPOSX)
	{
		setSpriteCtrl(pKey, SKBCTRL_HELP, &sp16);
		makeCursorSprite(&sp16, pKey->spCursor, pKey->iCursorPos);
		pKey->uiCursorPos = 1;
	}
	else if(pKey->List.iGlobalCurPosX == BUTTON_CTRL_GLOBALPOSX)
	{
		int iY = pKey->List.iGlobalCurPosY - pKey->List.iScrollPosY;
		switch(iY)
		{
		case 0:
			setSpriteCtrl(pKey, SKBCTRL_BACKSPACE, &sp16);
			pKey->uiCursorPos = 0;
			break;
		case 1:
			setSpriteCtrl(pKey, SKBCTRL_ENTER, &sp16);
			pKey->uiCursorPos = 2;
			break;
		case 3:
			if(setSpriteCtrl(pKey, SKBCTRL_SHIFT_L, &sp16))
			{
				setSpriteCtrl(pKey, SKBCTRL_SHIFT_R, &sp16);
			}
			pKey->uiCursorPos = 3;
			break;
		}
		makeCursorSprite(&sp16, pKey->spCursor, pKey->iCursorPos);
	}

	{
		int i;
		int col = affectAlpha(0x80808080, pKey->ucAlpha[SKBALPHA_CURSOR]);

		// 描画
		g_pSkbImport->loadTex(pk, pTexCur);
		for(i=0; i<4; i++)
		{
			g_pSkbImport->drawTex(pk, col, &pKey->spCursor[i], (uv_t*)&g_uvCursors[i], pTexCur);
		}
	}

}


//============================================================================
//	setSpriteCtrl
//
//	コントロールコード用のスプライトを設置
//
//	skey_t *pKey : ソフトウェアキーボード構造体へのポインタ
//	u_short ctrlCode : コントロールコード
//	sprite_t *pSp : スプライト格納用ポインタ
//
//	戻り値 : int : 0:成功/1:該当するコントロールコードが無かった
int setSpriteCtrl(skey_t *pKey, u_short ctrlCode, sprite_t *pSp)
{
	int i, index = 0;
	u_char ucShift = SKBMASK_LAYOUT(pKey->ucShift);
	SKB_ASSERT(pKey);

	for(i=0; i<pKey->uiBtnNum; i++)
	{
		if(ctrlCode == pKey->pBtns[i].state[ucShift].ctrlCode)
		{
			index = pKey->pBtns[i].spBaseIndex;
			memcpy(pSp, &pKey->pSps[index], sizeof(sprite_t));
			return 0;
		}
	}

	return 1;
}


//============================================================================
//	pushKey
//
//	キーが押下されたときの処理
//
//	skey_t *pKey : ソフトウェアキーボード構造体へのポインタ
static void pushKey(skey_t *pKey)
{
	if(pKey->List.iGlobalCurPosX == BUTTON_HELP_GLOBALPOSX)
	{
		g_pSkbImport->skb.showHelp(1);
		g_pSkbImport->skb.ucShowHelp = 1;
		pKey->uiCursorPos = 1;
		onStateCtrlCode(pKey, SKBCTRL_HELP, KEY_PUSH);
		releaseShift(pKey, 1);
	}
	else if(pKey->List.iGlobalCurPosX == BUTTON_CTRL_GLOBALPOSX)
	{
		int iY = pKey->List.iGlobalCurPosY - pKey->List.iScrollPosY;
		switch(iY)
		{
		case 0:
			pushBackSpace(pKey);
			pKey->uiCursorPos = 0;
			break;
		case 1:
			pushEnter(pKey);
			pKey->uiCursorPos = 2;
			break;
		case 3:
			pushShift(pKey, 1);
			pKey->uiCursorPos = 3;
			break;
		}
	}
	else
	{
		// 文字描画領域 内
		// 文字入れ
		addChar(pKey);
		releaseShift(pKey, 1);
	}
}


//============================================================================
//	releaseKey
//
//	キーが解除されたときの処理
//
//	skey_t *pKey : ソフトウェアキーボード構造体へのポインタ
static void releaseKey(skey_t *pKey)
{
	if(pKey->List.iGlobalCurPosX == BUTTON_HELP_GLOBALPOSX)
	{
		offStateCtrlCode(pKey, SKBCTRL_HELP, KEY_PUSH);
	}
	else if(pKey->List.iGlobalCurPosX == BUTTON_CTRL_GLOBALPOSX)
	{
		int iY = pKey->List.iGlobalCurPosY - pKey->List.iScrollPosY;
		switch(iY)
		{
		case 0:
			break;
		case 1:
			break;
		case 3:
			releaseShift(pKey, 0);
			break;
		}
	}

	pKey->uiCursorPos = -1;	// カーソルを無効にする
	pKey->uiPush = 0;		// カーソル押下中フラグをOFF
}


//============================================================================
//	setFocus
//
//	フォーカス(カーソル)位置設定
//
//	skey_t *pKey : ソフトウェアキーボード構造体へのポインタ
//
//	int x : X方向の値
//	int y : Y方向の値
void setFocus(skey_t *pKey, int x, int y)
{
	pKey->List.iGlobalCurPosY = y;
	pKey->List.iGlobalCurPosX = x;

	if(pKey->List.iGlobalCurPosX > (VISIBLE_CHARNUM_H-1))
	{
		pKey->List.iGlobalCurPosX = (VISIBLE_CHARNUM_H-1);
	}
	if(pKey->List.iGlobalCurPosY > (VISIBLE_CHARNUM_V-1))
	{
		pKey->List.iGlobalCurPosY = (VISIBLE_CHARNUM_V-1);
	}
}


//============================================================================
//	scrollCheck
//
//	スクロール最大/最小チェック
//
//	skey_t *pKey : ソフトウェアキーボード構造体へのポインタ
static void scrollCheck(skey_t *pKey)
{
	int iLY = pKey->List.iGlobalCurPosY - pKey->List.iScrollPosY;

	if(iLY < 0)
	{
		pKey->List.iScrollPosY -= 0 - iLY;
	}
	if(iLY > (VISIBLE_CHARNUM_V-1))
	{
		pKey->List.iScrollPosY += iLY - (VISIBLE_CHARNUM_V-1);
	}
}


//============================================================================
//	moveCursorList
//
//	カーソル移動
//
//	skey_t *pKey : ソフトウェアキーボード構造体へのポインタ
//	int dx : 移動量X値
//	int dy : 移動量Y値
static void moveCursorList(skey_t *pKey, int dx, int dy)
{
	int iY = 0;
	u_int maxX[] = {17, 16, 16, 16, 15, 15, 15, 15};	// maxX[4],maxX[5]は常にDISABLE状態なのでカウントしない

	if(dy)
	{
		// Y方向の値を変更
		if(dy > 0)
		{
			// ↓方向の移動
			if(pKey->List.iGlobalCurPosX == BUTTON_HELP_GLOBALPOSX)
			{
				// 変化無し
			}
			else if(pKey->List.iGlobalCurPosX == BUTTON_CTRL_GLOBALPOSX)
			{
				// コントロールボタン
				// ここは、アクセルとか関係ない
				pKey->List.iGlobalCurPosY += 1;
				iY = pKey->List.iGlobalCurPosY - pKey->List.iScrollPosY;
				if(iY == 2)
				{
					pKey->List.iGlobalCurPosY = pKey->List.iScrollPosY + 3;
				}
				else if(iY == 4)
				{
					pKey->List.iGlobalCurPosY = pKey->List.iScrollPosY;
				}
			}
			else
			{
				pKey->List.iGlobalCurPosY += dy;
			}
		}
		else
		{
			// ↑方向の移動
			if(pKey->List.iGlobalCurPosX == BUTTON_HELP_GLOBALPOSX)
			{
				// 変化無し
			}
			else if(pKey->List.iGlobalCurPosX == BUTTON_CTRL_GLOBALPOSX)
			{
				// コントロールボタン
				pKey->List.iGlobalCurPosY -= 1;
				iY = pKey->List.iGlobalCurPosY - pKey->List.iScrollPosY;
				if(iY == 2)
				{
					pKey->List.iGlobalCurPosY = pKey->List.iScrollPosY + 1;
				}
				else if(iY == -1)
				{
					pKey->List.iGlobalCurPosY = pKey->List.iScrollPosY + 3;
				}
			}
			else
			{
				pKey->List.iGlobalCurPosY += dy;
			}
		}

		if(pKey->List.iGlobalCurPosY < 0)
		{
			pKey->List.iGlobalCurPosY = 0;
			pKey->List.iScrollPosY = 0;
			pKey->List.iRepeatAccel = 0;
		}
		if(pKey->List.iGlobalCurPosY > (pKey->List.iLineNum-1))
		{
			pKey->List.iGlobalCurPosY = (pKey->List.iLineNum-1);
			pKey->List.iRepeatAccel = 0;
		}
		scrollCheck(pKey);
	}


	if(dx)
	{
		iY = pKey->List.iGlobalCurPosY - pKey->List.iScrollPosY;
		// X方向の値を変更
		if(dx > 0)
		{
			pKey->List.iGlobalCurPosX += 1;
			if(maxX[iY] < pKey->List.iGlobalCurPosX)
			{
				pKey->List.iGlobalCurPosX = 0;
			}
		}
		else
		{
			pKey->List.iGlobalCurPosX -= 1;
			if(pKey->List.iGlobalCurPosX < 0)
			{
				pKey->List.iGlobalCurPosX = maxX[iY];
			}
		}
	}

	iY = pKey->List.iGlobalCurPosY - pKey->List.iScrollPosY;
	if(iY == 2 && pKey->List.iGlobalCurPosX == BUTTON_CTRL_GLOBALPOSX)
	{
		pKey->List.iGlobalCurPosY = pKey->List.iScrollPosY + 1;
	}
}


//============================================================================
//	scrollChars
//
//	文字一覧スクロール処理
//
//	skey_t *pKey : ソフトウェアキーボード構造体へのポインタ
//	u_short press : 押下情報
//
//	戻り値 : int : 押下情報
static int scrollChars(skey_t *pKey, u_short press)
{
	static u_short usOldPress = 0;
	static int iRepeatOn = 0;
	static int iCountDown = LIST_SCROLL_START;
	int iv=0;

	if(press && press  == usOldPress)
	{
		if(iCountDown > 0)
		{
			iCountDown--;
		}
		if(iCountDown <= 0)
		{
			if(iRepeatOn)
			{
				pKey->List.iRepeatAccel++;
				if(pKey->List.iRepeatAccel > LIST_ACCEL_MAX)
				{
					pKey->List.iRepeatAccel = LIST_ACCEL_MAX;
				}
			}
			else
			{
				iRepeatOn = 1;
			}
		}
	}
	else
	{
		iRepeatOn = 0;
		iCountDown = LIST_SCROLL_START;
		pKey->List.iRepeatAccel = 0;
	}

	if(iRepeatOn)
	{
		static int iRepeatCycle = LIST_SCROLL_SPEED4;
		if(--iRepeatCycle <= 0)
		{
			if(pKey->List.iRepeatAccel < LIST_ACCEL_50)		 { iRepeatCycle = LIST_SCROLL_SPEED4; }
			else if(pKey->List.iRepeatAccel < LIST_ACCEL_100){ iRepeatCycle = LIST_SCROLL_SPEED2; }
			else if(pKey->List.iRepeatAccel < LIST_ACCEL_150){ iRepeatCycle = LIST_SCROLL_SPEED1; }
			else if(pKey->List.iRepeatAccel < LIST_ACCEL_200){ iRepeatCycle = LIST_SCROLL_SPEED1; }
			iv |= press;
		}
	}

	usOldPress = press;
	return 	iv;
}


//============================================================================
//	padUp
//
//	パッド解除
//
//	skey_t *pKey : ソフトウェアキーボード構造体へのポインタ
//	u_short up : 解除情報
static void padUp(skey_t *pKey, u_short up)
{
	if(up & SCE_PADL1){}
	if(up & SCE_PADL2)
	{
		offStateCtrlCode(pKey, SKBCTRL_LEFT, KEY_PUSH);
	}
	if(up & SCE_PADR1)
	{
		releaseShift(pKey, 0);
	}
	if(up & SCE_PADR2)
	{
		offStateCtrlCode(pKey, SKBCTRL_RIGHT, KEY_PUSH);
	}
	if(up & SCE_PADRup)
	{
		u_int uiTmp[] = {0x00003000, 0x00000000};
		offStateKey(pKey, uiTmp, KEY_PUSH);
	}
	if(up & SCE_PADRdown){}
	if(up & SCE_PADRleft)
	{
		offStateCtrlCode(pKey, SKBCTRL_BACKSPACE, KEY_PUSH);
	}
	if(up & SCE_PADRright)
	{
		releaseKey(pKey);
	}
	if(up & SCE_PADselect){}
	if(up & SCE_PADstart)
	{
		offStateCtrlCode(pKey, SKBCTRL_ENTER, KEY_PUSH);
	}
}


//============================================================================
//	initKeyTop
//
//	キートップ初期化(空)
//
//	skey_t *pKey : ソフトウェアキーボード構造体へのポインタ
static void initKeyTop(skey_t *pKey)
{
}


//============================================================================
//	moveMouseCursor
//
//	マウスカーソル移動
//
//	skey_t *pKey : ソフトウェアキーボード構造体へのポインタ
//	iPoint_t *pPoint : カーソル位置へのポインタ
static void moveMouseCursor(skey_t *pKey, iPoint_t *pPoint)
{
	int keyNo = SKBMOUSE_NULL;
	SKB_ASSERT(pKey);

	if((keyNo=searchKeyByPoint(pKey, pPoint)) != SKBMOUSE_NULL)
	{
		// ボタンの方で見つかったのでフォーカス移動
		pKey->uiCursorPos = keyNo;
	}
	else
	{
		int curPos = SKBMOUSE_NULL;
		// ボタンの方で見つからなかったので一覧部分を検索
		keyNo = searchListByPoint(pKey, pPoint);
		if(keyNo == SKBMOUSE_NULL)
		{
			return;		// 触れてない
		}

		// 一覧の方で見つかった
		// 一覧側のカーソル移動
		pKey->List.iGlobalCurPosX = keyNo % VISIBLE_CHARNUM_H;
		pKey->List.iGlobalCurPosY = (keyNo / VISIBLE_CHARNUM_H) + pKey->List.iScrollPosY;

		// ソフトウェアキーボード内のカーソル移動
		curPos = searchDummyKeyByIndex(pKey, keyNo);
		if(curPos != SKBMOUSE_NULL)
		{
			pKey->uiCursorPos = curPos;
		}
	}
}


//============================================================================
//	searchDummyKeyByIndex
//
//	一覧のインデックスからダミーキーを捜す
//
//	skey_t *pKey : ソフトウェアキーボード構造体へのポインタ
//	int index : 文字一覧のローカルインデックス
//
//	戻り値 : int : カーソルPos
int searchDummyKeyByIndex(skey_t *pKey, int index)
{
	int i, d;

	// 次に選択するキーボードのフォーカス位置を求める
	for(i=0, d=0; i<pKey->uiBtnNum; i++)
	{
		if((pKey->pBtns[i].animFlag & KEY_DUMMY))
		{
			if(index == d)
			{
				return i;
			}
			d++;
		}
	}

	return SKBMOUSE_NULL;	// 見つからなかった
}


//============================================================================
//	searchDummyKeyByIndex
//
//	位置情報から文字一覧のどのキーを選択しているか検索
//
//	skey_t *pKey : ソフトウェアキーボード構造体へのポインタ
//	iPoint_t *pPt : 位置情報のポインタ
//
//	戻り値 : int : 文字一覧のインデックス/SKBMOUSE_NULL
static int searchListByPoint(skey_t *pKey, iPoint_t *pPt)
{
	// 文字パケット作成
	int x, y;
	int globalX, globalY;
	int charPacketLocalPosX, charPacketLocalPosY;
	int centeringOffsetX;
	int charW = g_pSkbImport->skb.uiListFontW;
	int charH = g_pSkbImport->skb.uiListFontH;
	int charW16 = g_pSkbImport->thinkXW16(g_pSkbImport->skb.uiListFontW);
	int charH16 = g_pSkbImport->thinkYH16(g_pSkbImport->skb.uiListFontH);		// このY値は、フォントライブラリに渡す値なので、NTSC/PALやFRAME/FIELDを考えなくて良い。
	int dx, dy;
	int cx, cy;
	int px, py;
	int tx, ty;
	UTF8Char_t utf8Char;

	cx = g_pSkbImport->thinkXW(pKey->uiKbdPosX) + CHAR_STARTDRAW_POSX + CHAR_TOPLEFT_POSX;							// 文字書き始めOFFSET X
	cy = g_pSkbImport->thinkYH(pKey->uiKbdPosY) + g_pSkbImport->thinkYH(CHAR_STARTDRAW_POSY + CHAR_TOPLEFT_POSY);	// 文字書き始めOFFSET Y
	px = g_pSkbImport->thinkXW16(pPt->x);
	py = g_pSkbImport->thinkYH16(pPt->y);

	for(y=0; y<VISIBLE_CHARNUM_V; y++)
	{
		for(x=0; x<VISIBLE_CHARNUM_H; x++)
		{
			globalX = x;
			globalY = y + pKey->List.iScrollPosY;
			charPacketLocalPosX = (x * charW) + cx;			// ローカル位置を取得
			charPacketLocalPosY = (y * charH) + cy;

			utf8Char = pKey->List.utf8Cache[(y * VISIBLE_CHARNUM_H) + x].utf8Char;
			charPacketLocalPosY = g_pSkbImport->thinkYHn(charPacketLocalPosY);

			centeringOffsetX = (charW - pKey->List.utf8Cache[(y * VISIBLE_CHARNUM_H) + x].iFontWidth) >> 1;
			dx = charPacketLocalPosX + centeringOffsetX;
			dy = charPacketLocalPosY + g_pSkbImport->thinkYHn(TEXTPOS_ADDY);

			tx = g_pSkbImport->thinkXW16(dx+FOCUS_MARGINX);
			ty = (dy<<4) + g_pSkbImport->thinkYH16(FOCUS_MARGINY);

			if((tx <= px) && (ty <= py) &&
			   ((tx + charW16) > px) && ((ty + charH16) > py))
			{
				return ((y*VISIBLE_CHARNUM_H)+x);		// リストのインデックスを返す
			}
		}
	}

	return SKBMOUSE_NULL;
}



//============================================================================
//	leftButtonDown_list
//
//	マウス左ボタン押下
//
//	skey_t *pKey : ソフトウェアキーボード構造体へのポインタ
//	iPoint_t *pPt : 位置情報のポインタ
static void leftButtonDown_list(skey_t *pKey, iPoint_t *pPoint)
{
	int keyNo  = SKBMOUSE_NULL;
	int listNo = SKBMOUSE_NULL;
	int tgNo = SKBMOUSE_NULL;
	SKB_ASSERT(pKey);

	keyNo = searchKeyByPoint(pKey, pPoint);		// ボタン検索

	// 一覧部分を検索
	if(tgNo == SKBMOUSE_NULL)
	{
		listNo = searchListByPoint(pKey, pPoint);
	}

	if(keyNo  == SKBMOUSE_NULL &&
	   tgNo   == SKBMOUSE_NULL &&
	   listNo == SKBMOUSE_NULL)
	{
		pKey->uiPush = 0;				// カーソル押下中フラグをOFF
		return;
	}


	if(!(g_protectSkb.uiDragState & SKBMOUSE_LBTNDOWN))
	{
		// 左ボタン押下
		// キー
		if(keyNo != SKBMOUSE_NULL)
		{
			pKey->uiCursorPos = keyNo;
			pKey->uiPush  = 1;					// カーソル押下中フラグをON

			g_protectSkb.uiOldCurPos[0] = pKey->uiCursorPos;	// ドラッグしたまま他のボタンに移動した時の検出フラグを保存
			g_protectSkb.uiDragState |= SKBMOUSE_LBTNDOWN;
			return;
		}

		// 一覧
		if(listNo != SKBMOUSE_NULL)
		{
			g_protectSkb.uiOldCurPosList[0] = listNo % VISIBLE_CHARNUM_H;
			pKey->List.iGlobalCurPosX = listNo % VISIBLE_CHARNUM_H;
			g_protectSkb.uiOldCurPosList[1] = (listNo / VISIBLE_CHARNUM_H) + pKey->List.iScrollPosY;
			pKey->List.iGlobalCurPosY = (listNo / VISIBLE_CHARNUM_H) + pKey->List.iScrollPosY;
			pKey->uiPush  = 1;					// カーソル押下中フラグをON

			g_protectSkb.uiDragState |= SKBMOUSE_LBTNDOWN;
		}
	}
	else
	{
		// ドラッグ中
		// キー
		if(g_protectSkb.uiOldCurPos[0] != keyNo && keyNo != SKBMOUSE_NULL)
		{
			pKey->uiPush = 0;				// カーソル押下中フラグをOFF
			g_protectSkb.uiOldCurPos[0] = SKBMOUSE_NULL;
			return;
		}

		// 一覧
		if(g_protectSkb.uiOldCurPosList[0] != (listNo % VISIBLE_CHARNUM_H) ||
		   g_protectSkb.uiOldCurPosList[1] != ((listNo / VISIBLE_CHARNUM_H) + pKey->List.iScrollPosY))
		{
			// ドラッグしたまま他のボタンに移動した
			pKey->uiPush = 0;				// カーソル押下中フラグをOFF
			g_protectSkb.uiOldCurPosList[0] = SKBMOUSE_NULL;
			g_protectSkb.uiOldCurPosList[1] = SKBMOUSE_NULL;
		}
	}
}


//============================================================================
//	leftButtonUp_list
//
//	マウス左ボタン解除
//
//	skey_t *pKey : ソフトウェアキーボード構造体へのポインタ
//	iPoint_t *pPt : 位置情報のポインタ
static void leftButtonUp_list(skey_t *pKey, iPoint_t *pPoint)
{
	int keyNo  = SKBMOUSE_NULL;
	int listNo = SKBMOUSE_NULL;
	SKB_ASSERT(pKey);

	// マウスポインタ
	keyNo = searchKeyByPoint(pKey, pPoint);
	if(keyNo != SKBMOUSE_NULL)
	{
		// ボタンの方で見つかったのでフォーカス移動
		if(g_protectSkb.uiOldCurPos[0] == pKey->uiCursorPos)
		{
			pushKey(pKey);			// 文字を設定
		}
	}
	else
	{
		// ボタンの方で見つからなかったので一覧部分を検索
		listNo = searchListByPoint(pKey, pPoint);
		if(listNo == SKBMOUSE_NULL)
		{
			pKey->uiPush = 0;					// カーソル押下中フラグをOFF
			g_protectSkb.uiOldCurPosList[0] = SKBMOUSE_NULL;
			g_protectSkb.uiOldCurPosList[1] = SKBMOUSE_NULL;
			g_protectSkb.uiOldCurPos[0] = SKBMOUSE_NULL;
			g_protectSkb.uiDragState &= ~SKBMOUSE_LBTNDOWN;		// Dragフラグを解除
			return;		// 触れてない
		}

		// 文字入れ
		if(g_protectSkb.uiOldCurPosList[0] != SKBMOUSE_NULL &&
		   g_protectSkb.uiOldCurPosList[1] != SKBMOUSE_NULL)
		{
			addChar(pKey);
			releaseShift(pKey, 1);
		}
	}

	pKey->uiPush = 0;					// カーソル押下中フラグをOFF
	g_protectSkb.uiOldCurPosList[0] = SKBMOUSE_NULL;
	g_protectSkb.uiOldCurPosList[1] = SKBMOUSE_NULL;
	g_protectSkb.uiOldCurPos[0] = SKBMOUSE_NULL;
	g_protectSkb.uiDragToggle = 0;
	g_protectSkb.uiDragState &= ~SKBMOUSE_LBTNDOWN;		// Dragフラグを解除
}


//============================================================================
//	searchFocus
//
//	位置からフォーカスの場所を求める
//
//	skey_t *pKeyNext : 次回選択予定のソフトウェアキーボード構造体へのポインタ
//	sprite_t *pNowSp : 現在選択中のソフトウェアキーボード構造体へのポインタ
//
//	戻り値 : u_int : 0:初期値/0以外:次回選択予定のソフトウェアキーボードのキーインデックス
static u_int searchFocus(skey_t *pKeyNext, sprite_t *pNowSp)
{
	int keyNo=SKBMOUSE_NULL, listNo=SKBMOUSE_NULL;
	iPoint_t pt;
	sprite_t *pSp = pNowSp;

	// フォーカスのあたっているスプライトの中心位置を求める
	pt.x = g_pSkbImport->thinkXW(pSp->x + (pSp->w>>1));
	pt.y = g_pSkbImport->thinkYH(pSp->y + (pSp->h>>1));

	keyNo = searchKeyByPoint(pKeyNext, &pt);
	if(keyNo != SKBMOUSE_NULL)
	{
		// ボタン部分
		return keyNo;
	}

	// ボタン部分で見つからなかったので文字一覧内で探す
	listNo = searchListByPoint(pKeyNext, &pt);
	if(listNo != SKBMOUSE_NULL)
	{
		// 終了
		u_int RR[] = {2, 0, 1, 1, 1, 1, 0, 0};
		u_int cnt, i, pp;

		pp = 0;
		cnt = listNo / VISIBLE_CHARNUM_H;
		for(i=0; i<cnt; i++)
		{
			pp += RR[i];
		}

		return (pp + listNo);
	}

	pKeyNext->List.iGlobalCurPosX = 0;
	pKeyNext->List.iGlobalCurPosY = 0;
	return 0;
}


//============================================================================
//	open
//
//	ソフトウェアキーボードがOPENした時の処理
//
//	skey_t *pKey : ソフトウェアキーボード構造体へのポインタ
static void open(skey_t *pKey)
{
//	以下の設定を無視すると、
//	文字部分のカーソル位置とキー(ボタン)のカーソル位置が合わなくて
//	予期しない動作をします。
	u_int RR[] = {18, 16, 17, 17, 17, 17, 16, 16};
	u_int cntX, i;

	// リスト部分の初期化
	pKey->List.iScrollPosY = 0;
	pKey->List.iUtf8CacheTopLine = -1;

	cntX = 0;
	for(i=0; i<sizeof(RR)/sizeof(RR[0]); i++)
	{
		if((cntX + RR[i]) > (pKey->uiCursorPos))
		{
			break;
		}
		cntX += RR[i];
	}

	pKey->List.iGlobalCurPosX = (pKey->uiCursorPos) - cntX;
	pKey->List.iGlobalCurPosY = i;

	setFocus(pKey, pKey->List.iGlobalCurPosX, pKey->List.iGlobalCurPosY);
}


//============================================================================
//	close
//
//	ソフトウェアキーボードがCLOSEした時の処理（空）
//
//	skey_t *pKey : ソフトウェアキーボード構造体へのポインタ
static void close(skey_t *pKey)
{
}
