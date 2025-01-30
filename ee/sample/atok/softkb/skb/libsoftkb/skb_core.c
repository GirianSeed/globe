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
#include <string.h>

#include <eekernel.h>
#include <eetypes.h>
#include <sifdev.h>
#include <libpkt.h>
#include <libpad.h>
#include <libusbkb.h>

#include "libsoftkb.h"

// プロトタイプ宣言
extern gImport_t		*g_pSkbImport;			// メイン側関数群のポインタ
extern protectedSkb_t	g_protectSkb;			// 



static void deleteResource(skey_t *pKey);
static void makePacketsCursor(sceVif1Packet *pk, skey_t *pKey, textureInfo_t *pTexCur);

static void ctrlPad(skey_t *pKey, u_short down, u_short up, u_short press, u_short repeat);
static void padUp(skey_t *pKey, u_short up);
static void padDown(skey_t *pKey, u_short down, u_short press, u_short repeat);
static void pushKey(skey_t *pKey);
static void releaseKey(skey_t *pKey);

static void atokCursor(skey_t *pKey, int dx, int dy);

static void moveMouseCursor(skey_t *pKey, iPoint_t *pPoint);
static void leftButtonDown(skey_t *pKey, iPoint_t *pPoint);
static void leftButtonUp(skey_t *pKey, iPoint_t *pPoint);

static u_int searchFocus(skey_t *pKeyNext, sprite_t *pNowSp);
static void moveKeybd(skey_t *pKey, int x, int y);
static void changeAlpha(skey_t *pKey, int index, u_char alpha);

void initKeyTop(skey_t *pKey);
static void InitSpecialKey(skey_t *pKey, u_char flag);

static void open(skey_t *pKey);
static void close(skey_t *pKey);


//============================================================================
// PUBLIC FUNCTIONS
//============================================================================
//============================================================================
//  SkbCore_Init											ref:  skb_core.h
//
//  ソフトウェアキーボード初期化
//
//  exportSkb_t *pExport : 外部とのやりとり用ポインタ
//
//  戻り値 ： void
void SkbCore_Init(exportSkb_t *pExport)
{
	SKB_ASSERT(pExport);

	// 使われる関数ポインタを設定
	pExport->readResource   = readResourceFrame;
	pExport->deleteResource = deleteResource;

	pExport->makePacketsSkbBody = makePacketsBody;
	pExport->makePacketsCursor  = makePacketsCursor;
	pExport->searchFocus    = searchFocus;


	pExport->setPad         = ctrlPad;
	pExport->mouseMove        = moveMouseCursor;
	pExport->mouseLButtonDown = leftButtonDown;
	pExport->mouseLButtonUp   = leftButtonUp;

	pExport->trans           = moveKeybd;
	pExport->changeAlpha     = changeAlpha;
	pExport->initKeyTop      = initKeyTop;

	pExport->searchFocus     = searchFocus;
	pExport->initSpecialKey  = InitSpecialKey;

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
//	void *pTmp : 各ソフトウェアキーボードタイプで自由に使えるポインタ(このソフトウェアキーボードタイプでは、使われていない)
//
//	戻り値 : int : 0:成功/1:エラー
int readResourceFrame(skey_t *pKey, const char *filename, u_int keyTopNo, inhibitChar_t *inhibitChar, void *pTmp)
{
	#ifndef SKBRES_LINK  //Makefile define
	int fd, ret, size, i;
	SKB_ASSERT(pKey);
	SKB_ASSERT(filename);
	SKB_ASSERT(inhibitChar);

	// 既にリソース用メモリが確保されていた時には開放する
	if(pKey->pKeyBuff)
	{
		g_pSkbImport->mfree(pKey->pKeyBuff);
		pKey->pKeyBuff = NULL;
	}

	// ファイル読込み
	fd = sceOpen(filename, SCE_RDONLY);
	if(fd < 0)
	{
		g_pSkbImport->dprintf("ERROR %s, %d: readResourceFrame - open(%s)\n", __FILE__, __LINE__, filename);
		return 1;
	}
	size = sceLseek(fd, 0, SCE_SEEK_END);
	sceLseek(fd, 0, SCE_SEEK_SET);
	if(size <= 0)
	{
		g_pSkbImport->dprintf("ERROR %s, %d: readResourceFrame - size(%d)\n", __FILE__, __LINE__, size);
		return 1;
	}
	pKey->pKeyBuff = (char*)g_pSkbImport->malign(128, size);
	if(pKey->pKeyBuff == NULL)
	{
		g_pSkbImport->dprintf("ERROR %s, %d: readResourceFrame - malloc failed(%d)\n", __FILE__, __LINE__, size);
		return 1;
	}
	ret = sceRead(fd, (char*)pKey->pKeyBuff, size);
	sceClose(fd);
	#else
	int i;
	SKB_ASSERT(pKey);
	SKB_ASSERT(inhibitChar);
	pKey->pKeyBuff=(char*)filename;	//skbres.a をリンクした場合は、アドレスを渡すだけ。
	#endif

	// データ設定
	{
		lump_t	*pLump = (lump_t*)pKey->pKeyBuff;

		if(pLump->length != 0)
		{
			// スプライト情報設定
			pKey->pSps    = (sprite_t*)(pKey->pKeyBuff + pLump->offset);
			pKey->uiSpNum = pLump->length / sizeof(sprite_t);
			g_pSkbImport->dprintf("MSG : readResourceFrame\n");
			g_pSkbImport->dprintf("    : 		sprite_num = %d, %d, %d\n", pKey->uiSpNum, pLump->offset, pLump->length);
		}

		pLump++;
		if(pLump->length != 0)
		{
			// UV情報設定
			pKey->pUvs    = (uv_t*)(pKey->pKeyBuff + pLump->offset);
			pKey->uiUvNum = pLump->length / sizeof(uv_t);
			g_pSkbImport->dprintf("    : 		uv_num = %d, %d, %d\n", pKey->uiUvNum, pLump->offset, pLump->length);
		}

		pLump++;
		if(pLump->length != 0)
		{
			// キーボード情報設定
			pKey->pBtns    = (button_t*)(pKey->pKeyBuff + pLump->offset);
			pKey->uiBtnNum = pLump->length / sizeof(button_t);
			g_pSkbImport->dprintf("    : 		button_num = %d, %d, %d\n", pKey->uiBtnNum, pLump->offset, pLump->length);
		}

		pLump++;
		if(pLump->length != 0)
		{
			// 背景チップ情報設定
			pKey->pChips    = (paneChip_t*)(pKey->pKeyBuff + pLump->offset);
			pKey->uiChipNum = pLump->length / sizeof(paneChip_t);
			g_pSkbImport->dprintf("    : 		paneChip_num = %d, %d, %d\n", pKey->uiChipNum, pLump->offset, pLump->length);
		}

		//
		pLump++;
		if(pLump->length != 0)
		{
			// ソフトウェアキーボード情報設定
			pKey->pSkbInfo = (skbInfo_t*)(pKey->pKeyBuff + pLump->offset);
			g_pSkbImport->dprintf("    : 		skbInfo = %d, %d\n", pLump->offset, pLump->length);
		}
	}

	{
		sprite_t *pSp = pKey->pSps;
		uv_t     *pUv = pKey->pUvs;

		// スプライトを16倍する
		for(i=0; i<pKey->uiSpNum; i++)
		{
			pSp[i].x = g_pSkbImport->thinkXW16(pSp[i].x);
			pSp[i].y = g_pSkbImport->thinkYH16(pSp[i].y);
			pSp[i].z = g_pSkbImport->thinkXW16(pSp[i].z);
			pSp[i].w = g_pSkbImport->thinkXW16(pSp[i].w);
			pSp[i].h = g_pSkbImport->thinkYH16(pSp[i].h);
		}
		// UVを16倍する
		for(i=0; i<pKey->uiUvNum; i++)
		{
			pUv[i].u  = STG(pUv[i].u);
			pUv[i].v  = STG(pUv[i].v);
			pUv[i].tw = STG(pUv[i].tw-1);	// UV使用の為：幅が256だとしたら、
											// テクセルの幅は0～255なので16倍する前に-1して、
											// 16倍した後に半ピクセルずらす為に+8する
			pUv[i].th = STG(pUv[i].th-1);	// UV使用の為：高さが256だとしたら
											// テクセルの高さは0～255なので16倍する前に-1して、
											// 16倍した後に半ピクセルずらす為に+8する
		}
	}

	// 入力禁止文字を指定
	pKey->pInhibitChar = inhibitChar;
	setInhibitChars(pKey, pKey->pInhibitChar);


	// キーボード初期設定
	pKey->uiKeyTopNo = keyTopNo;
	setLayout(pKey);

	return 0;
}



//============================================================================
//  readResourceFrame
//
//  キー定義リソース破棄
//
//	skey_t *pKey : ソフトウェアキーボード構造体へのポインタ
static void deleteResource(skey_t *pKey)
{
	SKB_ASSERT(pKey);

	if(pKey->pKeyBuff)
	{
		#ifndef SKBRES_LINK  //Makefile define
		g_pSkbImport->mfree(pKey->pKeyBuff);
		#else
		moveKeybd( pKey, 0, 0 );
		{
			int i;
			sprite_t *pSp = pKey->pSps;
			uv_t     *pUv = pKey->pUvs;

			// スプライト座標を元に戻す。
			for(i=0; i<pKey->uiSpNum; i++)
			{
				pSp[i].x = pSp[i].x/g_pSkbImport->thinkXW16(1);
				pSp[i].y = pSp[i].y/g_pSkbImport->thinkYH16(1);
				pSp[i].z = pSp[i].z/g_pSkbImport->thinkXW16(1);
				pSp[i].w = pSp[i].w/g_pSkbImport->thinkXW16(1);
				pSp[i].h = pSp[i].h/g_pSkbImport->thinkYH16(1);
			}
			// UVを元に戻す。
			for(i=0; i<pKey->uiUvNum; i++)
			{
				pUv[i].u  = (pUv[i].u -8)/16  ;
				pUv[i].v  = (pUv[i].v   )/16  ;
				pUv[i].tw = (pUv[i].tw-8)/16+1;
				pUv[i].th = (pUv[i].th-8)/16+1;
			}
		}
		#endif
		pKey->pKeyBuff = NULL;
	}
}


//============================================================================
//	makePacketsBody
//
//  ソフトウェアキーボード描画パケット作成
//
//	sceVif1Packet *pk : VIFパケット格納用バッファへのポインタ
//	skey_t *pKey : ソフトウェアキーボード構造体へのポインタ
//	textureInfo_t *pTexBtn : ボタンテクスチャへのポインタ
//	textureInfo_t *pTexChar: 文字テクスチャへのポインタ
//	textureInfo_t *pTexBg  : 背景テクスチャへのポインタ
void makePacketsBody(sceVif1Packet *pk, skey_t *pKey,
						 textureInfo_t *pTexBtn, textureInfo_t *pTexChar, textureInfo_t *pTexBg)
{
	int i;
	u_char alpha;
	u_int col=0x80808080;
	button_t		*pBtn   = NULL;
	buttonState_t	*pState = NULL;
	paneChip_t		*pPane  = NULL;
	u_char			s       = SKBMASK_LAYOUT(pKey->ucShift);
	u_int			spIndex, uvIndex;

	SKB_ASSERT(pk);
	SKB_ASSERT(pKey);
	SKB_ASSERT(pTexBtn);
	SKB_ASSERT(pTexChar);
//	SKB_ASSERT(pTexBg);		// BGが要らない場合もある

	if(pTexBg != NULL)
	{
		// 背景描画
		g_pSkbImport->loadTex(pk, pTexBg);
		for(i=0; i<pKey->uiChipNum; i++)
		{
			pPane = (pKey->pChips+i);

			spIndex = pPane->spIndex;
			uvIndex = pPane->uvIndex;

			// 色設定
			col = affectAlpha(pPane->rgba, pKey->ucAlpha[SKBALPHA_BG]);

			// パケット作成
			g_pSkbImport->drawTex(pk, col, (pKey->pSps+spIndex), (pKey->pUvs+uvIndex), pTexBg);
		}
	}

	// キーベース描画
	g_pSkbImport->loadTex(pk, pTexBtn);
	{
		for(i=0; i<pKey->uiBtnNum; i++)
		{
			pBtn = (pKey->pBtns+i);
			if(pBtn->animFlag & KEY_INVISIBLE || pBtn->animFlag & KEY_DUMMY)
			{
				// ダミーボタンは描画しない
				continue;
			}

			// スプライトインデックス取得
			spIndex = pBtn->spBaseIndex;

			alpha = pKey->ucAlpha[SKBALPHA_BASE];
			// 色設定
			if(g_pSkbImport->skb.getAtokMode() && (g_protectSkb.ucUseMouse == 0))
			{
				// 変換中の時は、アルファ値を設定
				alpha = SKBDEF_DISABLE_ALPHA;
			}

			if(pBtn->animFlag & KEY_DISABLE)
			{
				// 入力禁止文字のキーだった時
				uvIndex = pBtn->uvBaseIndex[0];		// 通常テクスチャ
				// 色設定
				col = affectAlpha(DEFAULT_RGBA, alpha);
			}
			else
			{
				// キーベースのUVインデックスを取得
				uvIndex = pBtn->uvBaseIndex[0];		// 通常テクスチャ
				if(((pKey->uiPush == 1) && (pKey->uiCursorPos == i)) || (pBtn->animFlag & KEY_PUSH))
				{
					uvIndex = pBtn->uvBaseIndex[1];		// 押下状態のテクスチャ
				}

				if(((pBtn->state[s].ctrlCode == SKBCTRL_SHIFT_L) ||
				   (pBtn->state[s].ctrlCode == SKBCTRL_SHIFT_R) ||
					(pBtn->state[s].ctrlCode == SKBCTRL_CAPS) ||
					(pBtn->state[s].ctrlCode == SKBCTRL_KANJI)))
				{
					uvIndex = pBtn->uvBaseIndex[0];		// 通常テクスチャ
					if(pBtn->animFlag & KEY_PUSH)
					{
						uvIndex = pBtn->uvBaseIndex[1];		// 押下状態のテクスチャ
					}
				}

				col = affectAlpha(pBtn->rgba, alpha);
			}

			// パケット作成
			g_pSkbImport->drawTex(pk, col, (pKey->pSps+spIndex), (pKey->pUvs+uvIndex), pTexBtn);
		}
	}

	// キートップ描画
	g_pSkbImport->loadTex(pk, pTexChar);
	{
		for(i=0; i<pKey->uiBtnNum; i++)
		{
			pBtn   = (pKey->pBtns+i);
			pState = &pBtn->state[s];
			if(pBtn->animFlag & KEY_INVISIBLE || pBtn->animFlag & KEY_DUMMY)
			{
				// ダミーボタンは描画しない
				continue;
			}

			// 色設定
			if(pBtn->animFlag & KEY_DISABLE)
			{
				col = DEFAULT_DISABLE_RGBA;
			}
			else
			{
				col = pBtn->rgba;
			}

			// アルファ設定
			alpha = pKey->ucAlpha[SKBALPHA_BASE];
			if(g_pSkbImport->skb.getAtokMode() && (g_protectSkb.ucUseMouse == 0))
			{
				// 変換中の時は、アルファ値を設定
				alpha = SKBDEF_DISABLE_ALPHA;
			}

			col = affectAlpha(col, alpha);

			spIndex = pBtn->spCharIndex;
			uvIndex = pState->uvCharIndex[0];

			// SHIFTが押されていたら[Backspace]は[Delete]になる
			if((pState->ctrlCode == SKBCTRL_BACKSPACE) && (s & LAYOUT_SHIFT))
			{
				uvIndex = pState->uvCharIndex[1];
			}
			// 全角だったら[半/全]ボタンを切替
			if((pState->ctrlCode == SKBCTRL_KANJI) && (s & LAYOUT_ZENKAKU))
			{
				uvIndex = pState->uvCharIndex[1];
			}

			// フォーカス色設定
			//  カーソル番号と一致、DISABLEキーじゃない、ATOKの状態が変換中でない
			if((pKey->uiCursorPos == i) && !(pBtn->animFlag & KEY_DISABLE) &&
			   !g_pSkbImport->skb.getAtokMode() && (g_protectSkb.ucUseMouse == 0))
			{
				col = SKBDEF_ATTENTION_COL | SKBDEF_ATTENTION_ALPHA(pKey->ucAlpha[SKBALPHA_CURSOR]);
			}

			// パケット作成
			g_pSkbImport->drawTex(pk, col, (pKey->pSps+spIndex), (pKey->pUvs+uvIndex), pTexChar);
		}
	}
}


//============================================================================
//  makePacketsCursor
//
//	ソフトウェアキーボードカーソルパケット作成
//
//	sceVif1Packet *pk : VIFパケット格納用バッファへのポインタ
//	skey_t *pKey : ソフトウェアキーボード構造体へのポインタ
//	textureInfo_t *pTexCur : カーソルテクスチャへのポインタ
static void makePacketsCursor(sceVif1Packet *pk, skey_t *pKey, textureInfo_t *pTexCur)
{
	SKB_ASSERT(pk);
	SKB_ASSERT(pKey);
	SKB_ASSERT(pTexCur);

	if(g_pSkbImport->skb.getAtokMode())
	{
		// ATOKがONの時は、表示しない
		return;
	}

	{
		int i;
		int col = affectAlpha(0x80808080, pKey->ucAlpha[SKBALPHA_CURSOR]);
		static u_int cur = 500000;
		static u_int posX = 0, posY = 0;
		u_int  nowCur  = pKey->uiCursorPos;
		u_int  spIndex = pKey->pBtns[nowCur].spBaseIndex;

		// カーソルスプライト再計算
		makeCursorSprite(&pKey->pSps[spIndex], pKey->spCursor, pKey->iCursorPos);	// スプライト作成

		cur  = pKey->uiCursorPos;
		posX = pKey->uiKbdPosX;
		posY = pKey->uiKbdPosY;

		// 描画
		g_pSkbImport->loadTex(pk, pTexCur);
		for(i=0; i<4; i++)
		{
			g_pSkbImport->drawTex(pk, col, &pKey->spCursor[i], (uv_t*)&g_uvCursors[i], pTexCur);
		}
	}
}


//============================================================================
//  makeCursorSprite
//
//	ソフトウェアキーボードカーソルスプライト作成
//
//	sprite_t *pSpSrc : スプライト元データへのポインタ
//	sprite_t *pSpDst : スプライト保存先のポインタ
//	u_int floating : アニメーションの度合い
void makeCursorSprite(sprite_t *pSpSrc, sprite_t *pSpDst, u_int floating)
{
	int    w, h;

	w = g_pSkbImport->thinkXW16(SKBCURSOR_SP_W);
	h = g_pSkbImport->thinkYH16(SKBCURSOR_SP_H);

	// 左上
	pSpDst[0].x = pSpSrc->x - floating;
	pSpDst[0].y = pSpSrc->y - floating;
	pSpDst[0].z = pSpSrc->z;
	pSpDst[0].w = w;
	pSpDst[0].h = h;

	// 右上
	pSpDst[1].x = pSpSrc->x + pSpSrc->w - w + floating;
	pSpDst[1].y = pSpSrc->y - floating;
	pSpDst[1].z = pSpSrc->z;
	pSpDst[1].w = w;
	pSpDst[1].h = h;

	// 左下
	pSpDst[2].x = pSpSrc->x - floating;
	pSpDst[2].y = pSpSrc->y + pSpSrc->h - h + floating;
	pSpDst[2].z = pSpSrc->z;
	pSpDst[2].w = w;
	pSpDst[2].h = h;

	// 右下
	pSpDst[3].x = pSpSrc->x + pSpSrc->w - w + floating;
	pSpDst[3].y = pSpSrc->y + pSpSrc->h - h + floating;
	pSpDst[3].z = pSpSrc->z;
	pSpDst[3].w = w;
	pSpDst[3].h = h;

}


//============================================================================
//  moveKeybd
//
//	ソフトウェアキーボード移動
//
//	skey_t *pKey : ソフトウェアキーボード構造体へのポインタ
//	int x : X位置
//	int y : Y位置
static void moveKeybd(skey_t *pKey, int x, int y)
{
	int i;
	int _x, _y;
	sprite_t *pSp = pKey->pSps;
	SKB_ASSERT(pKey);

	x = g_pSkbImport->thinkXW16(x);
	y = g_pSkbImport->thinkYH16(y);

	// 前回と同じ場所なら移動させない
	if(x == pKey->uiKbdPosX && y == pKey->uiKbdPosY)
	{
		return;
	}

	_x = x - pKey->uiKbdPosX;
	_y = y - pKey->uiKbdPosY;

	for( i=0; i<pKey->uiSpNum; i++ )
	{
		(pSp+i)->x += _x;
		(pSp+i)->y += _y;
	}

	pKey->uiKbdPosX = x;
	pKey->uiKbdPosY = y;
}


//============================================================================
//  changeAlpha
//
//	アルファ値設定
//
//	skey_t *pKey : ソフトウェアキーボード構造体へのポインタ
//	int index : ボタンのインデックス
//	u_char alpha : 反映させたいアルファ値
static void changeAlpha(skey_t *pKey, int index, u_char alpha)
{
	if(index >= SKBALPHA_MAX)
	{
		return;
	}

	pKey->ucAlpha[index] = alpha;
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
	static u_short _down   = 0;
	static u_short _up     = 0;
	static u_short _press  = 0;
	static u_short _repeat = 0;

	SKB_ASSERT(pKey);
	if(down == _down && up == _up && press == _press && repeat == _repeat )
	{
		return;
	}

	padDown(pKey, down, press, repeat);
	padUp(pKey, up);

	_up     = up;
	_down   = down;
	_press  = press;
	_repeat = repeat;
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
	int dx=0, dy=0;
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
		#if 1//SKB_EX Sound
		SoftKB_se_play(SKBSE_CHANGEKEYTOP);
		#endif
		return;
	}

	// 押された瞬間のキーが優先される
	// 押された時
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
		#if 1//SKB_EX Sound
		if(down & SCE_PADR1) SoftKB_se_play(SKBSE_SHIFT);
		#endif
	}


	if((down & SCE_PADR2) || (press & SCE_PADR2))
	{
		onStateCtrlCode(pKey, SKBCTRL_RIGHT, KEY_PUSH);
	}
	if((down & SCE_PADRup) || (press & SCE_PADRup))
	{
		int index = 0;
		u_int uiTmp[][2] =
		{
			{0x00000020, 0x00000000},
			{0x00003000, 0x00000000},
		};
		if(pKey->ucShift & LAYOUT_ZENKAKU)
		{
			index = 1;
		}

		onStateKey(pKey, &uiTmp[index][0], KEY_PUSH);
	}
	if((down & SCE_PADRdown) || (press & SCE_PADRdown))
	{
									// ESCボタンは、ソフトウェアキーボード上にない
	}
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
			// 変換中なら全文確定を設定
			g_pSkbImport->skb.kakuteiAll();
			#if 1//SKB_EX Sound
			SoftKB_se_play(SKBSE_DETECTEXIT);
			#endif
		}
		else
		{
			pushEnter(pKey);								// 確定
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
		if(g_pSkbImport->skb.getAtokMode())
		{
			g_pSkbImport->skb.kakuteiPart();
			#if 1//SKB_EX Sound
			SoftKB_se_play(SKBSE_CANDDETECT);
			#endif
		}
		else
		{
			pushKey(pKey);			// 文字返し
			pKey->uiPush = 1;		// カーソル押下中フラグをON
		}
	}
	if(repeat & SCE_PADselect){}
	if(repeat & SCE_PADstart){}

	// カーソル移動
	{
		if(repeat & SCE_PADLup)
		{
			dy = -1;	// [↑]
		}
		else if(repeat & SCE_PADLdown)
		{
			dy = +1;	// [↓]
		}
	
		if(repeat & SCE_PADLleft)
		{
			dx = -1;	// [←]
		}
		else if(repeat & SCE_PADLright)
		{
			dx = 1;		// [→]
		}
		if(g_pSkbImport->skb.getAtokMode())
		{
			// ATOK/ON時 && 変換中
			atokCursor(pKey, dx, dy);
		}
		else
		{
			// ATOK/OFF時
			moveCursor(pKey, dx, dy);
		}
	}
}


//============================================================================
//	padUp
//
//	パッド解除処理
//
//	skey_t *pKey : ソフトウェアキーボード構造体へのポインタ
//	u_short up : ボタン解除情報
static void padUp(skey_t *pKey, u_short up)
{
	SKB_ASSERT(pKey);
	if(up & SCE_PADL1){}
	if(up & SCE_PADR1)
	{
		releaseShift(pKey, 0);
	}
	if(up & SCE_PADL2)
	{
		offStateCtrlCode(pKey, SKBCTRL_LEFT, KEY_PUSH);
	}
	if(up & SCE_PADR2)
	{
		offStateCtrlCode(pKey, SKBCTRL_RIGHT, KEY_PUSH);
	}
	if(up & SCE_PADRup)
	{
		int index = 0;
		u_int uiTmp[][2] =
		{
			{0x00000020, 0x00000000},
			{0x00003000, 0x00000000},
		};
		if(pKey->ucShift & LAYOUT_ZENKAKU)
		{
			index = 1;
		}

		offStateKey(pKey, &uiTmp[index][0], KEY_PUSH);
	}
	if(up & SCE_PADRdown)
	{
	}
	if(up & SCE_PADRleft)
	{
		offStateCtrlCode(pKey, SKBCTRL_BACKSPACE, KEY_PUSH);
	}
	if(up & SCE_PADRright)
	{
		releaseKey(pKey);
		pKey->uiPush = 0;		// カーソル押下中フラグをOFF
	}
	if(up & SCE_PADselect){}
	if(up & SCE_PADstart)
	{
		offStateCtrlCode(pKey, SKBCTRL_ENTER, KEY_PUSH);
	}
}


//============================================================================
//	pushEnter
//
//	Enterセット
//
//	skey_t *pKey : ソフトウェアキーボード構造体へのポインタ
void pushEnter(skey_t *pKey)
{
	g_pSkbImport->skb.setChars(NULL, ((pKey->ucShift&LAYOUT_SHIFT) ? 1:0), USBKEYC_ENTER|USBKB_RAWDAT);
}


//============================================================================
//	pushLeftArrow
//
//	←文字セット
//
//	skey_t *pKey : ソフトウェアキーボード構造体へのポインタ
void pushLeftArrow(skey_t *pKey)
{
	g_pSkbImport->skb.setChars(NULL, ((pKey->ucShift&LAYOUT_SHIFT) ? 1:0), USBKEYC_LEFT_ARROW|USBKB_RAWDAT);
}


//============================================================================
//	pushRightArrow
//
//	→文字セット
//
//	skey_t *pKey : ソフトウェアキーボード構造体へのポインタ
void pushRightArrow(skey_t *pKey)
{
	g_pSkbImport->skb.setChars(NULL, ((pKey->ucShift&LAYOUT_SHIFT) ? 1:0), USBKEYC_RIGHT_ARROW|USBKB_RAWDAT);
}


//============================================================================
//	pushUpArrow
//
//	↑文字セット
//
//	skey_t *pKey : ソフトウェアキーボード構造体へのポインタ
void pushUpArrow(skey_t *pKey)
{
	if(pKey->ucShift & LAYOUT_FOCUSSHIFT)
	{
		// フォーカスあわせてシフト押した時は、シフト状態を無視。
		g_pSkbImport->skb.setChars(NULL, 0, USBKEYC_UP_ARROW|USBKB_RAWDAT);
		return;
	}

	g_pSkbImport->skb.setChars(NULL, ((pKey->ucShift&LAYOUT_SHIFT) ? 1:0), USBKEYC_UP_ARROW|USBKB_RAWDAT);
}


//============================================================================
//	pushDownArrow
//
//	↓文字セット
//
//	skey_t *pKey : ソフトウェアキーボード構造体へのポインタ
void pushDownArrow(skey_t *pKey)
{
	// @
	if(pKey->ucShift & LAYOUT_FOCUSSHIFT)
	{
		// フォーカスあわせてシフト押した時は、シフト状態を無視。
		g_pSkbImport->skb.setChars(NULL, 0, USBKEYC_DOWN_ARROW|USBKB_RAWDAT);
		return;
	}

	g_pSkbImport->skb.setChars(NULL, ((pKey->ucShift&LAYOUT_SHIFT) ? 1:0), USBKEYC_DOWN_ARROW|USBKB_RAWDAT);
}


//============================================================================
//	pushSpace
//
//	スペース文字セット
//
//	skey_t *pKey : ソフトウェアキーボード構造体へのポインタ
void pushSpace(skey_t *pKey)
{
	int index = 0;
	u_int uiTmp[][2] =
	{
		{0x00000020, 0x00000000},	// 半角
		{0x00003000, 0x00000000},	// 全角
	};

	if(pKey->ucShift & LAYOUT_ZENKAKU)
	{
		index = 1;
	}

	g_pSkbImport->skb.setChars(&uiTmp[index][0], ((pKey->ucShift&LAYOUT_SHIFT) ? 1:0), ' ');
}


//============================================================================
//	setLayout
//
//	シフトをキートップによって設定
//
//	skey_t *pKey : ソフトウェアキーボード構造体へのポインタ
void setLayout(skey_t *pKey)
{
	SKB_ASSERT(pKey);

	// キートップによって固有の設定を反映させる
	switch(pKey->uiKeyTopNo)
	{
	case KEYTOP_HIRAGANA:
		// ひらがな
		pKey->ucShift |= LAYOUT_ZENKAKU | LAYOUT_CAPS;		// 全角を設定
		onStateCtrlCode(pKey, SKBCTRL_CAPS, KEY_DISABLE);	// CapsLockを無効にする
		break;

	case KEYTOP_KATAKANA:
		// カタカナ
		pKey->ucShift |= LAYOUT_ZENKAKU;					// 全角を設定
		onStateCtrlCode(pKey, SKBCTRL_KANJI, KEY_PUSH);		// 半角/全角/漢字 を押下状態にする
		break;

	default:
		pKey->ucShift = 0;
	}

	{
		int ucInputMode = 0;
		ucInputMode = SoftKB_GetInputMode(pKey);		// 全角/半角モードか取得
		g_pSkbImport->skb.changeInputMode(ucInputMode);	// 入力モード変更
	}
}


//============================================================================
//	pushShift
//
//	SHIFT 押下、ショートカットで指定
//
//	skey_t *pKey : ソフトウェアキーボード構造体へのポインタ
//	u_char flag : 0:ショートカット（R1）/1:カーソルを合わせて押された
void pushShift(skey_t *pKey, u_char flag)
{
	if(flag == 0)
	{
		// ショートカットでおされた
		pKey->ucShift |= LAYOUT_SHIFT;
		pKey->ucShift &= ~LAYOUT_FOCUSSHIFT;
	}
	else
	{
		// SHIFTボタンがおされた
		pKey->ucShift ^= (LAYOUT_SHIFT|LAYOUT_FOCUSSHIFT);
	}

	setEnableChars(pKey);						// 全てのキーを初期化
	setInhibitChars(pKey, pKey->pInhibitChar);	// 入力禁止キーを再設定

	if(pKey->ucShift & LAYOUT_SHIFT)
	{
		onStateCtrlCode(pKey, SKBCTRL_SHIFT_L, KEY_PUSH);
		onStateCtrlCode(pKey, SKBCTRL_SHIFT_R, KEY_PUSH);
	}
	else
	{
		offStateCtrlCode(pKey, SKBCTRL_SHIFT_L, KEY_PUSH);
		offStateCtrlCode(pKey, SKBCTRL_SHIFT_R, KEY_PUSH);
	}
}


//============================================================================
//	releaseShift
//
//	SHIFT 解除
//
//	skey_t *pKey : ソフトウェアキーボード構造体へのポインタ
//	u_char flag : 0:ショートカット（R1）/1:カーソルを合わせて押された
void releaseShift(skey_t *pKey, u_char flag)
{
	if(flag == 0)
	{
		// SHIFTボタンがリリースされた
		if(pKey->ucShift & LAYOUT_FOCUSSHIFT)
		{
		}
		else
		{
			pKey->ucShift &= ~(LAYOUT_SHIFT|LAYOUT_FOCUSSHIFT);
		}
	}
	else
	{
		// その他の文字に対してボタンリリースされた
		if(pKey->ucShift & LAYOUT_FOCUSSHIFT)
		{
			pKey->ucShift &= ~(LAYOUT_SHIFT|LAYOUT_FOCUSSHIFT);
		}
	}


	if(!(pKey->ucShift & LAYOUT_SHIFT))
	{
		offStateCtrlCode(pKey, SKBCTRL_SHIFT_L, KEY_PUSH);
		offStateCtrlCode(pKey, SKBCTRL_SHIFT_R, KEY_PUSH);
	}

	setEnableChars(pKey);							// 全てのキーを初期化
	setInhibitChars(pKey, pKey->pInhibitChar);		// 入力禁止キーを再設定
}


//============================================================================
//	pushCapsLock
//
//	CAPSLOCKが押下されたときの処理
//
//	skey_t *pKey : ソフトウェアキーボード構造体へのポインタ
void pushCapsLock(skey_t *pKey)
{
	pKey->ucShift ^= LAYOUT_CAPS;
	setEnableChars(pKey);							// 全てのキーを初期化
	setInhibitChars(pKey, pKey->pInhibitChar);	// 入力禁止キーを再設定
	if(pKey->ucShift & LAYOUT_CAPS)
	{
		onStateCtrlCode(pKey, SKBCTRL_CAPS, KEY_PUSH);
	}
	else
	{
		offStateCtrlCode(pKey, SKBCTRL_CAPS, KEY_PUSH);
	}
	#if 1//SKB_EX Sound
	SoftKB_se_play(SKBSE_ONCAPSLOCK);
	#endif
}


//============================================================================
//	pushTab
//
//	TAB文字セット
//
//	skey_t *pKey : ソフトウェアキーボード構造体へのポインタ
void pushTab(skey_t *pKey)
{
	// SHIFTが押されていると
	u_char ucShift = SKBMASK_LAYOUT(pKey->ucShift);
	u_int  uiUCS4[2];
	if(ucShift & LAYOUT_SHIFT)
	{
		// TODO : TAB文字があったら←方向にキャレット移動
		//		  その行のみ
	}
	else
	{
		uiUCS4[0] = 0x00000009;
		uiUCS4[1] = 0x0;
		g_pSkbImport->skb.setChars(uiUCS4, 0, 0);
		#if 1//SKB_EX Sound
		SoftKB_se_play(SKBSE_ONKEY);
		#endif
	}
}


//============================================================================
//	pushKanji
//
//	半角/全角/漢字
//
//	skey_t *pKey : ソフトウェアキーボード構造体へのポインタ
void pushKanji(skey_t *pKey)
{
	u_char mode;
	// キートップによっては、半角をサポートしていない物がある
	if(pKey->uiKeyTopNo == KEYTOP_HIRAGANA)
	{
		// 「ひらがな」の時は全角のみ
		//  DISABLEなら絶対入ってこない。
		return;
	}

	pKey->ucShift ^= LAYOUT_ZENKAKU;
	if(pKey->ucShift & LAYOUT_ZENKAKU)
	{
		// 全角
		onStateCtrlCode(pKey, SKBCTRL_KANJI, KEY_PUSH);
	}
	else
	{
		// 半角
		offStateCtrlCode(pKey, SKBCTRL_KANJI, KEY_PUSH);
	}

	// 入力モード変更
	mode = SoftKB_GetInputMode(pKey);
	g_pSkbImport->skb.changeInputMode(mode);

	setEnableChars(pKey);						// 全てのキーを初期化
	setInhibitChars(pKey, pKey->pInhibitChar);	// 入力禁止キーを再設定

	#if 1//SKB_EX Sound
	SoftKB_se_play(SKBSE_CHANGEZENHAN);
	#endif
}


//============================================================================
//	pushEsc
//
//	ESC文字セット
//
//	skey_t *pKey : ソフトウェアキーボード構造体へのポインタ
void pushEsc(skey_t *pKey)
{
	// SHIFTに対して
	g_pSkbImport->skb.setChars(NULL, ((pKey->ucShift&LAYOUT_SHIFT) ? 1:0), USBKEYC_ESCAPE|USBKB_RAWDAT);
}


//============================================================================
//	pushBackSpace
//
//	BACKSPACE文字セット
//
//	skey_t *pKey : ソフトウェアキーボード構造体へのポインタ
void pushBackSpace(skey_t *pKey)
{
	u_char ucShift = SKBMASK_LAYOUT(pKey->ucShift);
	if(ucShift & LAYOUT_SHIFT)
	{
		u_short usFlag = USBKEYC_DELETE|USBKB_RAWDAT;
		if(g_pSkbImport->ucUseAtok &&
		   #if 1//SKB_EX BS<->DEL
		   g_pSkbImport->skb.getAtokInput() )
		   #else
		   (pKey->ucModebarType == MODEBAR_CHAR_HIRAGANA_J || pKey->ucModebarType == MODEBAR_CHAR_KATAKANA_J))
		   #endif
		{
			usFlag = USBKEYC_BS|USBKB_RAWDAT;
		}
		// SHIFTが押されている時にはDELETEに置き換わる
		g_pSkbImport->skb.setChars(NULL, ((pKey->ucShift&LAYOUT_SHIFT) ? 1:0), usFlag);
	}
	else
	{
		g_pSkbImport->skb.setChars(NULL, ((pKey->ucShift&LAYOUT_SHIFT) ? 1:0), USBKEYC_BS|USBKB_RAWDAT);
	}
}


//============================================================================
//	onStateCtrlCode
//
//	コントロールキーを指定された状態にする
//	これは、パッドからの直接入力で使います。
//
//	skey_t *pKey : ソフトウェアキーボード構造体へのポインタ
//	u_int ctrlCode : コントロールキー
//	u_char state : 状態(KEY_PUSH/KEY_DISABLE/KEY_INVISIBLE等)
//
//	戻り値 : int : 0:成功/1:指定されたコントロールキーが見つからなかった
int onStateCtrlCode(skey_t *pKey, u_int ctrlCode, u_char state)
{
	int i;
	u_char ucShift = SKBMASK_LAYOUT(pKey->ucShift);
	SKB_ASSERT(pKey);

	for(i=0; i<pKey->uiBtnNum; i++)
	{
		if(ctrlCode == pKey->pBtns[i].state[ucShift].ctrlCode)
		{
			pKey->pBtns[i].animFlag |= state;
			return 0;
		}
	}

	return 1;
}


//============================================================================
//	offStateCtrlCode
//
//	コントロールキーを指定された状態のみOFFにする。
//	これは、パッドからの直接入力で使います。
//
//	skey_t *pKey : ソフトウェアキーボード構造体へのポインタ
//	u_int ctrlCode : コントロールキー
//	u_char state : 状態(KEY_PUSH/KEY_DISABLE/KEY_INVISIBLE等)
//
//	戻り値 : int : 0:成功/1:指定されたコントロールキーが見つからなかった
int offStateCtrlCode(skey_t *pKey, u_int ctrlCode, u_char state)
{
	int i;
	u_char ucShift = SKBMASK_LAYOUT(pKey->ucShift);
	SKB_ASSERT(pKey);

	for(i=0; i<pKey->uiBtnNum; i++)
	{
		if(ctrlCode == pKey->pBtns[i].state[ucShift].ctrlCode)
		{
			pKey->pBtns[i].animFlag &= ~state;
			return 0;
		}
	}

	return 1;
}


//============================================================================
//	onStateKey
//
//	普通のキーを指定された状態のみONにする。
//
//	skey_t *pKey : ソフトウェアキーボード構造体へのポインタ
//	u_int *key : 文字列へのポインタ
//	u_char state : 状態(KEY_PUSH/KEY_DISABLE/KEY_INVISIBLE等)
//
//	戻り値 : int : 0:成功/1:指定されたコントロールキーが見つからなかった
int onStateKey(skey_t *pKey, u_int *key, u_char state)
{
	int i;
	u_char ucShift = SKBMASK_LAYOUT(pKey->ucShift);
	SKB_ASSERT(pKey);

	for(i=0; i<pKey->uiBtnNum; i++)
	{
		if((strcmp((char*)key, (char*)pKey->pBtns[i].state[ucShift].key) == 0) &&
		   !(pKey->pBtns[i].animFlag & (KEY_DUMMY|KEY_INVISIBLE|KEY_DISABLE)))
		{
			pKey->pBtns[i].animFlag |= state;
			return 0;
		}
	}

	return 1;
}


//============================================================================
//	offStateKey
//
//	通常キーを指定された状態のみOFFにする。
//
//	skey_t *pKey : ソフトウェアキーボード構造体へのポインタ
//	u_int *key : 文字列へのポインタ
//	u_char state : 状態(KEY_PUSH/KEY_DISABLE/KEY_INVISIBLE等)
//
//	戻り値 : int : 0:成功/1:指定されたコントロールキーが見つからなかった
int offStateKey(skey_t *pKey, u_int *key, u_char state)
{
	int i;
	u_char ucShift = SKBMASK_LAYOUT(pKey->ucShift);
	SKB_ASSERT(pKey);

	for(i=0; i<pKey->uiBtnNum; i++)
	{
		if((strcmp((char*)key, (char*)pKey->pBtns[i].state[ucShift].key) == 0) &&
		   !(pKey->pBtns[i].animFlag & (KEY_DUMMY|KEY_INVISIBLE|KEY_DISABLE)))
		{
			pKey->pBtns[i].animFlag &= ~state;
			return 0;
		}
	}

	return 1;
}


//============================================================================
//	pushKey
//
//	文字設定 キーに割り当てられた文字をコールバックで返す
//
//	skey_t *pKey : ソフトウェアキーボード構造体へのポインタ
static void pushKey(skey_t *pKey)
{
	u_int  uiNowCur = pKey->uiCursorPos;
	u_char ucShift = SKBMASK_LAYOUT(pKey->ucShift);
	SKB_ASSERT(pKey);

	// binファイルには、コントロールコードが入っていながら、キーコードも入っている場合がある。
	if((pKey->pBtns[uiNowCur].state[ucShift].ctrlCode != 0))
	{
		u_int code = pKey->pBtns[uiNowCur].state[ucShift].ctrlCode;
		switch(code)
		{
		case SKBCTRL_CAPS:		pushCapsLock(pKey);		return;
		case SKBCTRL_SHIFT_L:
		case SKBCTRL_SHIFT_R:	pushShift(pKey, 1);
								#if 1//SKB_EX Sound
								SoftKB_se_play(SKBSE_SHIFT);
								#endif
								return;
		case SKBCTRL_BACKSPACE:	pushBackSpace(pKey);	break;
		case SKBCTRL_ENTER:		pushEnter(pKey);		break;
		case SKBCTRL_UP:		pushUpArrow(pKey);		break;
		case SKBCTRL_DOWN:		pushDownArrow(pKey);	break;
		case SKBCTRL_LEFT:		pushLeftArrow(pKey);	break;
		case SKBCTRL_RIGHT:		pushRightArrow(pKey);	break;
		case SKBCTRL_HELP:
			if(g_pSkbImport->isConv())
			{
				// 変換中の時は、キートップの変更ができない
				return;
			}

			g_pSkbImport->skb.showHelp(1);
			g_pSkbImport->skb.ucShowHelp = 1;
			releaseShift(pKey, 1);
			#if 1//SKB_EX Sound
			SoftKB_se_play(SKBSE_ONHELP);
			#endif
			break;
		case SKBCTRL_KANJI:		pushKanji(pKey);		return;
		case SKBCTRL_TAB:		pushTab(pKey);			break;
		case SKBCTRL_ABCQWERTY:
			g_pSkbImport->skb.abcToQwerty();
			g_pSkbImport->dprintf("MSG : push abc <--> qwerty\n");
			#if 1//SKB_EX Sound
			SoftKB_se_play(SKBSE_QWERTYABC);
			#endif
			break;
		default:
			g_pSkbImport->dprintf("ERROR : pushKey - non supported CTRL(%x)\n", code);
		}
	}
	else
	{
		if(g_protectSkb.ucUseMouse ||
		   pKey->pBtns[uiNowCur].state[ucShift].key[0] == 0x00000020 ||
		   pKey->pBtns[uiNowCur].state[ucShift].key[0] == 0x00003000)
		{
			// マウスを使う場合は、スペースキーはコントロールにする
			g_pSkbImport->skb.setChars(pKey->pBtns[uiNowCur].state[ucShift].key, ((pKey->ucShift&LAYOUT_SHIFT) ? 1:0), ' ');
		}
		else
		{
			g_pSkbImport->skb.setChars(pKey->pBtns[uiNowCur].state[ucShift].key, 0,
									   pKey->pBtns[uiNowCur].state[ucShift].ctrlCode);
			releaseShift(pKey, 1);
		}
	}
}


//============================================================================
//	releaseKey
//
//	ボタン解除時のアニメーション等の後処理
//
//	skey_t *pKey : ソフトウェアキーボード構造体へのポインタ
static void releaseKey(skey_t *pKey)
{
	u_int  uiNowCur = pKey->uiCursorPos;
	u_char ucShift = SKBMASK_LAYOUT(pKey->ucShift);
	SKB_ASSERT(pKey);

	if(pKey->pBtns[uiNowCur].state[ucShift].ctrlCode != 0)
	{
		u_int code = pKey->pBtns[uiNowCur].state[ucShift].ctrlCode;
		switch(code)
		{
		case SKBCTRL_CAPS:		//offStateCtrlCode(pKey, SKBCTRL_CAPS, KEY_PUSH);	break
			pKey->uiPush = 0;
			return;
		case SKBCTRL_SHIFT_L:
		case SKBCTRL_SHIFT_R:
			releaseShift(pKey, 0);
			pKey->uiPush = 0;
			return;
		case SKBCTRL_BACKSPACE:	offStateCtrlCode(pKey, SKBCTRL_BACKSPACE, KEY_PUSH);break;
		case SKBCTRL_ENTER:		offStateCtrlCode(pKey, SKBCTRL_ENTER, KEY_PUSH);break;
		case SKBCTRL_UP:		offStateCtrlCode(pKey, SKBCTRL_UP, KEY_PUSH);break;
		case SKBCTRL_DOWN:		offStateCtrlCode(pKey, SKBCTRL_DOWN, KEY_PUSH);break;
		case SKBCTRL_LEFT:		offStateCtrlCode(pKey, SKBCTRL_LEFT, KEY_PUSH);break;
		case SKBCTRL_RIGHT:		offStateCtrlCode(pKey, SKBCTRL_RIGHT, KEY_PUSH);break;
		case SKBCTRL_HELP:		offStateCtrlCode(pKey, SKBCTRL_HELP, KEY_PUSH);break;
		case SKBCTRL_KANJI:
			pKey->uiPush = 0;
			return;
		case SKBCTRL_TAB:		offStateCtrlCode(pKey, SKBCTRL_TAB, KEY_PUSH);	break;
		case SKBCTRL_ABCQWERTY:
			g_pSkbImport->dprintf("MSG : push abc <--> qwerty\n");
			break;
		}
	}
	else
	{
		releaseShift(pKey, 1);
	}
}


//============================================================================
//	atokCursor
//
//	ATOK使用時には方向キーにアサインされる
//
//	skey_t *pKey : ソフトウェアキーボード構造体へのポインタ
//	int dx : コントローラの左右矢印キー
//	int dy : コントローラの上下矢印キー
static void atokCursor(skey_t *pKey, int dx, int dy)
{
	if(dx == 0 && dy == 0)
	{
		return;
	}

	if(dy)
	{
		if(dy > 0)
		{
			pushDownArrow(pKey);	// ↓方向キー
		}
		else
		{
			pushUpArrow(pKey);		// ↑方向キー
		}
	}

	if(dx)
	{
		if(dx > 0)
		{
			pushRightArrow(pKey);	// →方向キー
		}
		else
		{
			pushLeftArrow(pKey);	// ←方向キー
		}
	}
}


//============================================================================
//	moveCursor
//
//	カーソル移動
//
//	skey_t *pKey : ソフトウェアキーボード構造体へのポインタ
//	int dx : 移動数(X値)
//	int dy : 移動数(Y値)
void moveCursor(skey_t *pKey, int dx, int dy)
{
	enum{ _up, _down, _left, _right };
	int d = _up;
	u_int nowCursor = pKey->uiCursorPos;
	#if 1//SKB_EX Cursor
	u_int backupCursor = pKey->uiCursorPos;
	u_int searchCount;
	#endif
	SKB_ASSERT(pKey);

	if(dx == 0 && dy == 0)
	{
		return;
	}

	if(dy)
	{
		int y;
		if(dy > 0)
		{
			for(y=0; y<dy; y++)
			{
				nowCursor = pKey->uiCursorPos;
				pKey->uiCursorPos = pKey->pBtns[nowCursor].direction[_down];
			}
			d = _down;
		}
		else
		{
			for(y=dy; y<0; y++)
			{
				nowCursor = pKey->uiCursorPos;
				pKey->uiCursorPos = pKey->pBtns[nowCursor].direction[_up];
			}
			d = _up;
		}

		// DISABLE, INVISIBLE を無視
		nowCursor = pKey->uiCursorPos;
		#if 1//SKB_EX Cursor
		searchCount = 32;
		#endif
		while(1)
		{
			if(!(pKey->pBtns[nowCursor].animFlag & KEY_INVISIBLE) &&
			   !(pKey->pBtns[nowCursor].animFlag & KEY_DISABLE))
			{
				pKey->uiCursorPos = nowCursor;
				break;
			}
			nowCursor = pKey->pBtns[nowCursor].direction[d];
			#if 1//SKB_EX Cursor
			searchCount--;
			if(searchCount==0){
				pKey->uiCursorPos=backupCursor;
				break;
			}
			#endif
		}
	}

	if(dx)
	{
		int x, d;
		if(dx > 0)
		{
			for(x=0; x<dx; x++)
			{
				nowCursor = pKey->uiCursorPos;
				pKey->uiCursorPos = pKey->pBtns[nowCursor].direction[_right];
			}
			d = _right;
		}
		else
		{
			for(x=dx; x<0; x++)
			{
				nowCursor = pKey->uiCursorPos;
				pKey->uiCursorPos = pKey->pBtns[nowCursor].direction[_left];
			}
			d = _left;
		}

		// DISABLE, INVISIBLE を無視
		nowCursor = pKey->uiCursorPos;
		#if 1//SKB_EX Cursor
		searchCount = 32;
		#endif
		while(1)
		{
			if(!(pKey->pBtns[nowCursor].animFlag & KEY_INVISIBLE) &&
			   !(pKey->pBtns[nowCursor].animFlag & KEY_DISABLE))
			{
				pKey->uiCursorPos = nowCursor;
				break;
			}
			nowCursor = pKey->pBtns[nowCursor].direction[d];
			#if 1//SKB_EX Cursor
			searchCount--;
			if(searchCount==0){
				pKey->uiCursorPos=backupCursor;
				break;
			}
			#endif
		}
	}

}


//============================================================================
//	searchFocus
//
//	現在のカーソル位置から次回選択されるソフトウェアキーボードのカーソルの場所を求める
//
//	skey_t *pKeyNext : 次回選択予定のソフトウェアキーボード構造体へのポインタ
//	sprite_t *pNowSp : 現在選択中のソフトウェアキーボード構造体へのポインタ
//
//	戻り値 : u_int : 0:初期値/0以外:次回選択予定のソフトウェアキーボードのキーインデックス
static u_int searchFocus(skey_t *pKeyNext, sprite_t *pNowSp)
{
	int cx =0, cy =0;
	int i;
	u_int spIndex;

	sprite_t *pSp = pNowSp;
	cx = pSp->x + (pSp->w>>1);
	cy = pSp->y + (pSp->h>>1);


	// 次に選択するキーボードのフォーカス位置を求める
	{
		button_t *pBtn=NULL;
		for(i=0; i<pKeyNext->uiBtnNum; i++)
		{
			pBtn = &pKeyNext->pBtns[i];
			spIndex = pBtn->spBaseIndex;
			pSp = &pKeyNext->pSps[spIndex];

			if((pSp->x <= cx) && (pSp->y <= cy) &&
			   ((pSp->x+pSp->w) > cx) && ((pSp->y+pSp->h) > cy) &&
			   !(pKeyNext->pBtns[i].animFlag & (KEY_INVISIBLE|KEY_DISABLE|KEY_DUMMY)))
			{
				return i;
			}
		}

		// 粘るのはやめる。
		return 0;
	}
}


//============================================================================
//	moveMouseCursor
//
//	マウスカーソル移動
//
//	skey_t *pKey : ソフトウェアキーボード構造体へのポインタ
//	iPoint_t *pPoint : マウスカーソルの位置
static void moveMouseCursor(skey_t *pKey, iPoint_t *pPoint)
{
	int keyNo = SKBMOUSE_NULL;
	SKB_ASSERT(pKey);

	if((keyNo=searchKeyByPoint(pKey, pPoint)) != -1)
	{
		// 見つかったのでフォーカス移動
		pKey->uiCursorPos = keyNo;
	}
}


//============================================================================
//	leftButtonDown
//
//	マウス左ボタン押下
//
//	skey_t *pKey : ソフトウェアキーボード構造体へのポインタ
//	iPoint_t *pPoint : マウスカーソルの位置
static void leftButtonDown(skey_t *pKey, iPoint_t *pPoint)
{
	int keyNo = SKBMOUSE_NULL;
	SKB_ASSERT(pKey);

	// キー位置特定
	keyNo = searchKeyByPoint(pKey, pPoint);
	if(keyNo == SKBMOUSE_NULL)
	{
		// HITしているキーが無かった
		if(g_protectSkb.uiOldCurPos[0] != SKBMOUSE_NULL)
		{
			pKey->uiPush = 0;				// カーソル押下中フラグをOFF
			g_protectSkb.uiOldCurPos[0] = SKBMOUSE_NULL;
		}
		return;
	}

	if(!(g_protectSkb.uiDragState & SKBMOUSE_LBTNDOWN))
	{
		// 左ボタン押下した
		pKey->uiCursorPos = keyNo;
		pKey->uiPush  = 1;					// カーソル押下中フラグをON

		g_protectSkb.uiOldCurPos[0] = pKey->uiCursorPos;	// ドラッグしたまま他のボタンに移動した時の検出フラグを保存
		g_protectSkb.uiDragState |= SKBMOUSE_LBTNDOWN;
	}
	else
	{
		if(g_protectSkb.uiOldCurPos[0] != keyNo)
		{
			// ドラッグしたまま他のボタンに移動した
			pKey->uiPush = 0;				// カーソル押下中フラグをOFF
			g_protectSkb.uiOldCurPos[0] = SKBMOUSE_NULL;
		}
	}
}


//============================================================================
//	leftButtonUp
//
//	マウス左ボタン解除
//
//	skey_t *pKey : ソフトウェアキーボード構造体へのポインタ
//	iPoint_t *pPoint : マウスカーソルの位置
static void leftButtonUp(skey_t *pKey, iPoint_t *pPoint)
{
	int keyNo = SKBMOUSE_NULL;
	SKB_ASSERT(pKey);

	// マウスポインタ
	keyNo = searchKeyByPoint(pKey, pPoint);
	if(g_protectSkb.uiOldCurPos[0] == pKey->uiCursorPos)
	{
		pushKey(pKey);			// 文字を設定
	}

	pKey->uiPush = 0;			// カーソル押下中フラグをOFF
	g_protectSkb.uiOldCurPos[0] = SKBMOUSE_NULL;
	g_protectSkb.uiDragState &= ~SKBMOUSE_LBTNDOWN;		// Dragフラグを解除
}


//============================================================================
//	searchKeyByPoint
//
//	マウス座標位置からHITしているキー番号を探す
//
//	skey_t *pKey : ソフトウェアキーボード構造体へのポインタ
//	iPoint_t *pPoint : マウスカーソルの位置
int searchKeyByPoint(skey_t *pKey, iPoint_t *pPt)
{
	int spIndex;
	int cx = g_pSkbImport->thinkXW16(pPt->x);
	int cy = g_pSkbImport->thinkYH16(pPt->y);
	int i;
	sprite_t *pSp = NULL;

	// 次に選択するキーボードのフォーカス位置を求める
	button_t *pBtn=NULL;
	for(i=0; i<pKey->uiBtnNum; i++)
	{
		pBtn = &pKey->pBtns[i];
		spIndex = pBtn->spBaseIndex;
		pSp = &pKey->pSps[spIndex];

		if((pSp->x <= cx) && (pSp->y <= cy) &&
		   ((pSp->x+pSp->w) > cx) && ((pSp->y+pSp->h) > cy) &&
		   !(pKey->pBtns[i].animFlag & KEY_INVISIBLE) &&
		   !(pKey->pBtns[i].animFlag & KEY_DISABLE) &&
		   !(pKey->pBtns[i].animFlag & KEY_DUMMY))
		{
			return i;
		}
	}

	return -1;	// 見つからなかった
}


//============================================================================
//	setInhibitChars
//
//	入力禁止文字を反映
//
//	skey_t *pKey : ソフトウェアキーボード構造体へのポインタ
//	inhibitChar_t *inhibitChar : 入力禁止文字構造体へのポインタ
void setInhibitChars(skey_t *pKey, inhibitChar_t *inhibitChar)
{
	u_int i, s;
	u_int charNum = inhibitChar->uiCharNum;
	u_int ctrlNum = inhibitChar->uiCKeyNum;
	u_char ucShift = SKBMASK_LAYOUT(pKey->ucShift);

	SKB_ASSERT(pKey);
	SKB_ASSERT(inhibitChar);

	for(i=0; i<pKey->uiBtnNum; i++)
	{
		// コントロールコードは操作しない
		if(pKey->pBtns[i].state[ucShift].ctrlCode != 0)
		{
			continue;
		}

		for(s=0; s<charNum; s++)
		{
			if(memcmp((char*)pKey->pBtns[i].state[ucShift].key, (char*)inhibitChar->pChar[s], 10) == 0)
			{
				// 入力禁止文字フラグ立てる
				pKey->pBtns[i].animFlag |= KEY_DISABLE;
				goto NEXT;
			}
		}

		for(s=0; s<ctrlNum; s++)
		{
			if(pKey->pBtns[i].state[ucShift].ctrlCode == inhibitChar->pCtrlKey[s])
			{
				// 入力禁止コントロールキー
				pKey->pBtns[i].animFlag |= KEY_DISABLE;
				break;
			}
		}

	NEXT:;
	}
}


//============================================================================
//	setEnableChars
//
//	ダミー（KEY_INVISIBLE）キー以外のキー全てを
//	有効にする。
//
//	skey_t *pKey : ソフトウェアキーボード構造体へのポインタ
void setEnableChars(skey_t *pKey)
{
	u_int i;
	u_char ucShift = SKBMASK_LAYOUT(pKey->ucShift);
	SKB_ASSERT(pKey);

	for(i=0; i<pKey->uiBtnNum; i++)
	{
		// コントロールコードは操作しない
		if(pKey->pBtns[i].state[ucShift].ctrlCode != 0)
		{
			continue;
		}

		if(!(pKey->pBtns[i].animFlag & KEY_INVISIBLE))
		{
			pKey->pBtns[i].animFlag &= ~KEY_DISABLE;
		}
	}
}


//============================================================================
//	setReleseChars
//
//	全てのキーのKEY_PUSH状態を解除
//
//	skey_t *pKey : ソフトウェアキーボード構造体へのポインタ
void setReleseChars(skey_t *pKey)
{
	u_int i;
	SKB_ASSERT(pKey);

	for(i=0; i<pKey->uiBtnNum; i++)
	{
		pKey->pBtns[i].animFlag &= ~KEY_PUSH;
	}
}


//============================================================================
//	initKeyTop
//
//	ソフトウェアキーボードのキートップ初期化
//
//	skey_t *pKey : ソフトウェアキーボード構造体へのポインタ
void initKeyTop(skey_t *pKey)
{
	setReleseChars(pKey);						// 全てのキーのPUSH状態を解除
	setEnableChars(pKey);						// 全てのキーを有効化
	setInhibitChars(pKey, pKey->pInhibitChar);	// 入力禁止キーを再設定

	setLayout(pKey);							// モードによってキートップの状態を設定

}


//============================================================================
//	InitSpecialKey
//
//	特殊きー初期化
//
//	skey_t *pKey : ソフトウェアキーボード構造体へのポインタ
//	u_char flag : 0:LAYOUT_FOCUSSHIFTは無視/1:LAYOUT_FOCUSSHIFTを考慮
static void InitSpecialKey(skey_t *pKey, u_char flag)
{
	u_int i;
	u_char ucShift = SKBMASK_LAYOUT(pKey->ucShift);
	SKB_ASSERT(pKey);

	for(i=0; i<pKey->uiBtnNum; i++)
	{
		// 通常文字コードは操作しない
		if(pKey->pBtns[i].state[ucShift].ctrlCode == 0xFFFFFFFF)
		{
			continue;
		}

		if(pKey->pBtns[i].state[ucShift].ctrlCode == SKBCTRL_SHIFT_L ||
		   pKey->pBtns[i].state[ucShift].ctrlCode == SKBCTRL_SHIFT_R)
		{
			if(flag)
			{
				if(!(pKey->ucShift & LAYOUT_FOCUSSHIFT))
				{
					pKey->ucShift &= ~LAYOUT_SHIFT;
					pKey->pBtns[i].animFlag &= ~KEY_PUSH;
				}
			}
			else
			{
				pKey->ucShift &= ~(LAYOUT_SHIFT|LAYOUT_FOCUSSHIFT);
				pKey->pBtns[i].animFlag &= ~KEY_PUSH;
			}
			continue;
		}
		else if(pKey->pBtns[i].state[ucShift].ctrlCode != SKBCTRL_CAPS &&
		   pKey->pBtns[i].state[ucShift].ctrlCode != SKBCTRL_KANJI)
		{
			pKey->pBtns[i].animFlag &= ~KEY_PUSH;
		}
	}
	setEnableChars( pKey );                     // 全てのキーを有効化
	setInhibitChars( pKey, pKey->pInhibitChar );// 入力禁止キーを再設定
}


//============================================================================
//	open
//
//	ソフトウェアキーボードがOPENした時の処理（空）
//
//	skey_t *pKey : ソフトウェアキーボード構造体へのポインタ
static void open(skey_t *pKey)
{
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


