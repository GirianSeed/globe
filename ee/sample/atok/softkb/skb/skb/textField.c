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

#include <sys/types.h>
#include <eekernel.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <libpkt.h>

#include <shareddef.h>
#include "textField.h"



#define TEXF_ASSERT(_a_)  assert((int)_a_)
#define TF_MAX_NUM			(1024)

#define CARET_W16			(2<<4)


static gImport_t		*g_pTxtFImport;


static struct
{
	textureInfo_t		tex;	// テクスチャ

} g_TF;



// テキストフィールド枠部分のサイズ
#define TF_MARGIN			(16)			// 枠部分のマージン
#define TF_MARGIN16			(TF_MARGIN<<4)	// 枠部分のマージンx16
#define TF_CLIP_W			(13)			// クリップ枠描画位置からの相対位置
#define TF_CLIP_H			(3)				// クリップ枠描画位置からの相対位置
//#define TF_CLIP_SIZE		(13)


// テキストフィールド スプライト
const static sprite_t	g_spSrc[] =
{
	{0,		0,		0,		16,		16},
	{0,		16,		0,		16,		16},
	{0,		32,		0,		16,		16},
	{16,	0,		0,		16,		16},
	{16,	16,		0,		16,		16},
	{16,	32,		0,		16,		16},
	{32,	0,		0,		16,		16},
	{32,	16,		0,		16,		16},
	{32,	32,		0,		16,		16},
};
// テキストフィールド 板情報
const static paneChip_t	g_pnSrc[] =
{
	{0,		0,		0x80808080},
	{1,		1,		0x80808080},
	{2,		2,		0x80808080},
	{3,		3,		0x80808080},
	{4,		4,		0x80808080},
	{5,		5,		0x80808080},
	{6,		6,		0x80808080},
	{7,		7,		0x80808080},
	{8,		8,		0x80808080},
};

// テキストフィールド UV座標
const static uv_t		g_uvTFSrc[] =
{
	{0,		0,		16,		16},
	{0,		16,		16,		16},
	{0,		32,		16,		16},
	{16,	0,		16,		16},
	{16,	16,		16,		16},
	{16,	32,		16,		16},
	{32,	0,		16,		16},
	{32,	16,		16,		16},
	{32,	32,		16,		16},
};
// 候補一覧 UV座標
const static uv_t		g_uvCLSrc[] =
{
	{48,	0,		16,		16},
	{48,	16,		16,		16},
	{48,	32,		16,		16},
	{64,	0,		16,		16},
	{64,	16,		16,		16},
	{64,	32,		16,		16},
	{80,	0,		16,		16},
	{80,	16,		16,		16},
	{80,	32,		16,		16},
};

#ifdef SKBRES_LINK  //Makefile define
//--------------------------------------------------------
#include <skbres.h>
#define TF_TEXTURE_NAME		tField_tm2
//--------------------------------------------------------
#else
//--------------------------------------------------------
#if CDBOOT
#define TF_TEXTURE_NAME		TF_TEXTURE_PATH"\\TFIELD.TM2;1"
#else
#define TF_TEXTURE_NAME		TF_TEXTURE_PATH"/tField.tm2"
#endif
//--------------------------------------------------------
#endif


static void drawText(sceVif1Packet *pk, textField_t *pTf);
static void drawCaret(sceVif1Packet *pk, textField_t *pTf, int x16, int y16);
static void moveTF(textField_t *pTf, int x16, int y16);
static void sizeTF(textField_t *pTf, int w16, int h16);
static void stretchRect(sprite_t *sp0, int w, int h);
static void setResource(textField_t *pTf);
#ifndef ATOK_NO_LINK
static void drawCandListFrame(sceVif1Packet *pk, textField_t *pTf, iRect_t *rc16, int nCurrentCand);
#endif

//============================================================================
// PUBLIC FUNCTIONS
//============================================================================
//============================================================================
//  TextF_Init												ref:  textField.h
//
//  テキストフィールド初期化処理
//
//  gImport_t *pImport    : 外部コールバック関数群ポインタ
//  iRect_t *rc           : テキストフィールドの位置、サイズ
void TextF_Init(gImport_t *pImport)
{
	TEXF_ASSERT(pImport);
	g_pTxtFImport = pImport;

	// テクスチャ読み込み
	g_pTxtFImport->readTex(&g_TF.tex, TF_TEXTURE_NAME);
}


//============================================================================
//  TextF_Destroy											ref:  textField.h
//
//  テキストフィールド削除処理
//
void TextF_Destroy(void)
{
	g_pTxtFImport->deleteTex(&g_TF.tex);

}

//============================================================================
//  TexF_Create												ref:  textField.h
//
//  テキストフィールド作成
//
//  textField_t *pTf : テキストフィールド構造体へのポインタ
//  iRect_t *rc      : テキストフィールドの位置、サイズ
void TextF_Create(textField_t *pTf, iRect_t *rc)
{
	TEXF_ASSERT(pTf);
	TEXF_ASSERT(rc);
	memset(pTf, 0x0, sizeof(textField_t));

	// リソース設定
	setResource(pTf);

	// rcの位置、サイズは画面上に写る範囲と保証されている
	// 位置、サイズ設定
	TextF_Move(pTf, rc);
}


//============================================================================
//  TextF_Delete											ref:  textField.h
void TextF_Delete(textField_t *pTf)
{
}


extern void SetLine(sceVif1Packet *pk, int x, int y, int cx, int cy, const RGBA32 c, int z);
//============================================================================
//  TextF_Run												ref:  textField.h
//
//  通常処理
//
//  sceVif1Packet *pk : パケットへのポインタ
//  textField_t *pTf  : テキストフィールド構造体へのポインタ
void TextF_Run(sceVif1Packet *pk, textField_t *pTf)
{
	int i;
	TEXF_ASSERT(pk);
	TEXF_ASSERT(pTf);

	if(pTf->iHide)
	{
		// 描画しない
		return;
	}

	// テキストフィールド描画
	{
		sprite_t   *pSp = pTf->textField.aSP;
		uv_t       *pUv = pTf->textField.aUV;
		paneChip_t *pPn = pTf->textField.aPN;
		int spIndex, uvIndex;
		u_int col;

		g_pTxtFImport->loadTex(pk, &g_TF.tex);											// テクスチャロード
		for(i=0; i<TF_FRAME_ELEMENT; i++, pPn++)
		{
			spIndex = pPn->spIndex;
			uvIndex = pPn->uvIndex;
			col = affectAlpha(pPn->rgba, pTf->alphaValue[TF_ALPHA_EDITORBG]);

			g_pTxtFImport->drawTex(pk, col, (pSp+spIndex), (pUv+uvIndex), &g_TF.tex);	// テクスチャ描画
		}
	}

	drawText(pk, pTf);
}


//============================================================================
//  TextF_Move												ref:  textField.h
//
//  テキストフィールド移動
//
//  textField_t *pTf : テキストフィールド構造体へのポインタ
//  iRect_t *rc      : 矩形のポインタ
//					   FRAME/FIELD モードを考慮しない値でありNTSCを基準に作成。
void TextF_Move(textField_t *pTf, iRect_t *rc)
{
	int x16, y16, w16, h16;
	TEXF_ASSERT(pTf);
	TEXF_ASSERT(rc);

	// 限界値を設定
	if(rc->x < 0)
	{
		rc->x = 0;
	}
	if(rc->y < 0)
	{
		rc->y = 0;
	}
	if(rc->w < 0)
	{
		rc->w = TF_MARGIN*2;
	}
	if(rc->h < 0)
	{
		rc->h = 0;//(TF_MARGIN_H*2)>>4;
	}

	x16 = g_pTxtFImport->thinkXW16(rc->x);
	y16 = g_pTxtFImport->thinkYH16(rc->y);
	w16 = g_pTxtFImport->thinkXW16(rc->w);
	h16 = g_pTxtFImport->thinkYH16(rc->h);

	if((x16 == pTf->rcEdit16.x) && (y16 == pTf->rcEdit16.y) &&
	   (w16 == pTf->rcEdit16.w) && (h16 == pTf->rcEdit16.h))
	{
		return;
	}

	moveTF(pTf, x16, y16);
	sizeTF(pTf, w16, h16);
}


//============================================================================
//  TextF_ChangeAlpha										ref:  textField.h
//
//  テキストフィールドアルファ値変更
//
//  textField_t *pTf : テキストフィールド構造体へのポインタ
//  int index        : アルファ配列へのインデックス
//  u_char alpha     : 設定したいアルファ値
void TextF_ChangeAlpha(textField_t *pTf, int index, u_char alpha)
{
	TEXF_ASSERT(pTf);

	pTf->alphaValue[index] = alpha;
}

//============================================================================
//  TextF_SetDispMode										ref:  textField.h
//
//  テキストフィールドの表示モード設定
//
//  textField_t *pTf : テキストフィールド構造体へのポインタ
//  int mode         : モード
void TextF_SetDispMode( textField_t *pTf, int mode )
{
	TEXF_ASSERT(pTf);

	pTf->dispMode = mode;
}

//============================================================================
// STATIC FUNCTIONS
//============================================================================
//============================================================================
//	drawText
//
//	文字列描画
//
//	sceVif1Packet *pk : VIF1パケット格納用バッファへのポインタ
//	textField_t *pTf : テキストフィールド構造体へのポインタ
static void drawText(sceVif1Packet *pk, textField_t *pTf)
{
	int strWidth16 = 0;
	int x16 = pTf->rcEdit16.x + g_pTxtFImport->thinkXW16(TF_MARGIN);	// テキストフィールドの枠の幅  分右側に描画
	int y16 = pTf->rcEdit16.y + g_pTxtFImport->thinkYH16(TF_MARGIN);	// テキストフィールドの枠の高さ分下側に描画
	int w16 = pTf->rcEdit16.w - g_pTxtFImport->thinkXW16(TF_MARGIN<<1);	// テキストフィールドの枠の幅  分小さくする
	int h16 = pTf->rcEdit16.h - g_pTxtFImport->thinkYH16(TF_MARGIN<<1);	// テキストフィールドの枠の高さ分小さくする
	int caretPos = g_pTxtFImport->txt.getCaretPos();					// キャレット位置取得
	int cx=0 ,cy=0, cw=0, ch=0;
	int clipX = x16>>4;
	int clipY = y16>>4;
	int clipW = w16>>4;
	int clipH = (h16>>4) + 10;
	u_int *pUCS4 = g_pTxtFImport->txt.getStrings();						// 確定文字列取得
	u_int *pDispText;
	iRect_t rc   = {x16, y16, w16, h16};								// 枠
	iRect_t clip = {clipX, clipY, clipW, clipH};						// クリップ領域
	u_int uiBuf[TF_MAX_NUM];
	u_int dispText[TF_MAX_NUM];

	int scrx16, scry16;

	cx = g_pTxtFImport->thinkXW(x16);
	cy = g_pTxtFImport->thinkYH(y16);
	cw = g_pTxtFImport->thinkXW(w16);
	ch = g_pTxtFImport->thinkYH(h16);

	// スクロール
	if(pTf->iScrollLX16 < (x16+(TF_MARGIN16*2)))
	{
		// 左方向にはみ出した
		pTf->iScrollPosX16 -= 2000;
		if(pTf->iScrollPosX16 < 0)
		{
			pTf->iScrollPosX16 = 0;
		}
	}
	else if(pTf->iScrollRX16 > (x16+w16-(TF_MARGIN16*2)))
	{
		// 右方向にはみ出した
		if(!((pTf->iScrollLX16-2000) < x16))
		{
			pTf->iScrollPosX16 += 2000;
		}
	}

	// 確定文字列 描画
	rc.x = cx - g_pTxtFImport->thinkXW(pTf->iScrollPosX16);
	rc.y = cy;
	rc.w = cw;
	rc.h = ch;
	if( pTf->dispMode ){ // password 用
		int n;

		for(n=0;;n++){
			if(*(pUCS4+n)==0)	break;
			dispText[n]='*';
		}
		dispText[n]=0;
		g_pTxtFImport->txt.drawStrings(pk, dispText, &clip, &rc);	// 文字描画
		pDispText=dispText;
	}
	else{
		g_pTxtFImport->txt.drawStrings(pk, pUCS4, &clip, &rc);	// 文字描画
		pDispText=pUCS4;
	}

	// キャレット
	memcpy((char*)uiBuf, (char*)pDispText, caretPos*sizeof(u_int));
	uiBuf[caretPos] = '\0';
	strWidth16 = g_pTxtFImport->txt.getStringWidth(uiBuf);	// 文字列の先頭からキャレット位置までの幅を求める

	// キャレットが存在しているバイト位置の文字が描画されたGSローカル座標を得る
	scrx16 = x16 + strWidth16;
	scry16 = y16;

	#ifndef ATOK_NO_LINK
	if(g_pTxtFImport->ucUseAtok)
	{
		// ATOK 使用
		int atokState = g_pTxtFImport->txt.getAtokState();	// 入力ステートを取得

		if(atokState <= SCE_ATOK_ISTATE_BEFOREINPUT)
		{
			// 文字入力前の時
			drawCaret(pk, pTf, scrx16 - pTf->iScrollPosX16, scry16);
			pTf->iScrollLX16 = scrx16 - pTf->iScrollPosX16;
			pTf->iScrollRX16 = scrx16 - pTf->iScrollPosX16;
		}
		else if(atokState == SCE_ATOK_ISTATE_BEFORECONVERT)
		{
			// 変換前
			rc.x = g_pTxtFImport->thinkXW(scrx16 - pTf->iScrollPosX16);
			g_pTxtFImport->txt.drawStringBeforeChange(pk, pUCS4, &clip, &rc);		// 変換前文字列描画

			strWidth16 = g_pTxtFImport->txt.getToCaretPos();
			scrx16 += strWidth16;	// キャレット用

			drawCaret(pk, pTf, scrx16 - pTf->iScrollPosX16, scry16);
			pTf->iScrollLX16 = scrx16 - pTf->iScrollPosX16;
			pTf->iScrollRX16 = scrx16 - pTf->iScrollPosX16;
		}
		else
		{
			// 変換中
			rc.x = g_pTxtFImport->thinkXW(scrx16 - pTf->iScrollPosX16);
			g_pTxtFImport->txt.drawStringChanging(pk, pUCS4, &clip, &rc,
												   &pTf->iScrollLX16, &pTf->iScrollRX16,
												   TF_MARGIN, TF_MARGIN, pTf, drawCandListFrame);		// 変換前文字列描画
			strWidth16 = g_pTxtFImport->txt.getToCaretPos();
			scrx16 += strWidth16;	// キャレット用
			pTf->iScrollLX16 = scrx16 - pTf->iScrollPosX16;
			pTf->iScrollRX16 = scrx16 - pTf->iScrollPosX16;
		}
	}
	else
	#endif
	{
		// ATOK 未使用
		drawCaret(pk, pTf, scrx16 - pTf->iScrollPosX16, scry16);
		pTf->iScrollLX16 = scrx16 - pTf->iScrollPosX16;
		pTf->iScrollRX16 = scrx16 - pTf->iScrollPosX16;
	}
}


//============================================================================
//	drawText
//
//	キャレット描画
//
//	sceVif1Packet *pk : VIF1パケット格納用バッファへのポインタ
//	textField_t *pTf : テキストフィールド構造体へのポインタ
//	int x16 : キャレット描画X位置
//	int y16 : キャレット描画Y位置
static void drawCaret(sceVif1Packet *pk, textField_t *pTf, int x16, int y16)
{
	int ex16 = pTf->rcEdit16.x + g_pTxtFImport->thinkXW16(TF_MARGIN);
	int ew16 = pTf->rcEdit16.w - g_pTxtFImport->thinkXW16(TF_MARGIN<<1);
	iRect_t rc = {x16, y16, CARET_W16, 0};
	u_int col = affectAlpha(pTf->uiCaretColor, pTf->alphaValue[TF_ALPHA_CARET]);

	if(rc.x < ex16)
	{
		rc.x = ex16;
	}
	if(rc.x > (ex16+ew16))
	{
		rc.x = ex16+ew16;
	}
	rc.h = g_pTxtFImport->txt.uiCharHeight<<4;
	#if 1 //SKB_EX Insert
	if( ( !g_pTxtFImport->txt.isInsertMode() ) 
	  #ifndef ATOK_NO_LINK
	  && g_pTxtFImport->txt.getAtokState() != SCE_ATOK_ISTATE_BEFORECONVERT
	  #endif
	){
		//上書き動作時は、キャレットを太らせている。
		rc.w +=  (1<<4);
	}
	#endif
	g_pTxtFImport->drawPane(pk, &rc, col, 0);
}


//============================================================================
//	moveTF
//
//	TextFieldの位置変更
//
//	textField_t *pTf : テキストフィールド構造体へのポインタ
//	int x16 : 移動X相対位置
//	int y16 : 移動Y相対位置
static void moveTF(textField_t *pTf, int x16, int y16)
{
	int i;
	pTf->rcEdit16.x += (x16 - pTf->rcEdit16.x);
	pTf->rcEdit16.y += (y16 - pTf->rcEdit16.y);

	memcpy(pTf->textField.aSP, g_spSrc, sizeof(sprite_t)*TF_FRAME_ELEMENT);

	// 座標を修正
	for(i=0; i<TF_FRAME_ELEMENT; i++)
	{
		pTf->textField.aSP[i].x = g_pTxtFImport->thinkXW16(pTf->textField.aSP[i].x) + x16;
		pTf->textField.aSP[i].y = g_pTxtFImport->thinkYH16(pTf->textField.aSP[i].y) + y16;
		pTf->textField.aSP[i].z = g_pTxtFImport->thinkXW16(pTf->textField.aSP[i].z);
		pTf->textField.aSP[i].w = g_pTxtFImport->thinkXW16(pTf->textField.aSP[i].w);
		pTf->textField.aSP[i].h = g_pTxtFImport->thinkYH16(pTf->textField.aSP[i].h);
	}
}


//============================================================================
//	sizeTF
//
//	TextFieldのサイズ変更
//
//	textField_t *pTf : テキストフィールド構造体へのポインタ
//	int w16 : 描画サイズ幅
//	int h16 : 描画サイズ高さ
static void sizeTF(textField_t *pTf, int w16, int h16)
{
	pTf->rcEdit16.w = w16;
	pTf->rcEdit16.h = h16;
	stretchRect(pTf->textField.aSP, w16, h16);
}


//============================================================================
//	stretchRect
//
//	TextField矩形を伸ばす
//
//	textField_t *pTf : テキストフィールド構造体へのポインタ
//	int w : キャレット描画X位置
//	int h : キャレット描画Y位置
static void stretchRect(sprite_t *sp0, int w, int h)
{
//	 <---  W  ---->
//	┌─┬──┬─┐<---
//	│0 │ 3  │6 │   |
//	├─┼──┼─┤   |
//	│  │    │  │   |
//	│1 │ 4  │7 │   | H
//	│  │    │  │   |
//	├─┼──┼─┤   |
//	│2 │ 5  │8 │   |
//	└─┴──┴─┘<---
//
//  0,1,2,6,7,8 = 枠部分
//  4 = 可変部分
	sprite_t *pSp = sp0+1;
	sprite_t *pSpPrev;
	int rs = 0;

	// 1, 4, 7の高さ、4の幅を計算
	if(g_pTxtFImport->getFrameField())
	{
		// FRAME : Y値、高さを1/2にする
		rs = 1;
	}
	w -= g_pTxtFImport->thinkXW16(TF_MARGIN<<1);	// 可変部分の幅  (左右2個分)
	h -= g_pTxtFImport->thinkYH16(TF_MARGIN<<1);//(TF_MARGIN_H16<<1);	// 可変部分の高さ(上下2個分)
	if(w < 0)
	{
		w = 0;
	}

	if(h < 0)
	{
		h = 0;
	}

	pSp->h = h;							// Pane[1]の高さ
	pSpPrev = (sp0+1);
	pSp = (sp0+2);
	pSp->y = h + pSpPrev->y;			// Pane[2]のY値

	pSpPrev = (sp0+2);
	pSp = (sp0+3);
	pSp->w = w;							// Pane[3]の幅

	pSpPrev = (sp0+3);
	pSp = (sp0+4);
	pSp->h = h;							// Pane[4]の高さ
	pSp->w = w;							// Pane[4]の幅

	pSpPrev = (sp0+4);
	pSp = (sp0+5);
	pSp->w = w;							// Pane[5]の幅
	pSp->y = h + pSpPrev->y;			// Pane[5]のY値

	pSpPrev = (sp0+5);
	pSp = (sp0+6);
	pSp->x = pSpPrev->x + pSpPrev->w;	// Pane[6]のX値

	pSpPrev = (sp0+6);
	pSp = (sp0+7);
	pSp->x = pSpPrev->x;				// Pane[7]のX値
	pSp->h = h;							// Pane[7]の高さ

	pSpPrev = (sp0+7);
	pSp = (sp0+8);
	pSp->y = pSpPrev->h + pSpPrev->y;	// Pane[8]のY値
	pSp->x = pSpPrev->x;				// Pane[8]のX値
}


//============================================================================
//	setResource
//
//	矩形の元データを設定
//
//	textField_t *pTf : テキストフィールド構造体へのポインタ
static void setResource(textField_t *pTf)
{
	int i;
	// テキストフィールド
	memcpy(pTf->textField.aSP, g_spSrc,   sizeof(sprite_t)*TF_FRAME_ELEMENT);
	memcpy(pTf->textField.aUV, g_uvTFSrc, sizeof(uv_t)*TF_FRAME_ELEMENT);
	memcpy(pTf->textField.aPN, g_pnSrc,   sizeof(paneChip_t)*TF_FRAME_ELEMENT);

	// 候補一覧
	memcpy(pTf->candList.aSP, g_spSrc,   sizeof(sprite_t)*TF_FRAME_ELEMENT);
	memcpy(pTf->candList.aUV, g_uvCLSrc, sizeof(uv_t)*TF_FRAME_ELEMENT);
	memcpy(pTf->candList.aPN, g_pnSrc,   sizeof(paneChip_t)*TF_FRAME_ELEMENT);

	for(i=0; i<TF_FRAME_ELEMENT; i++)
	{
		pTf->textField.aSP[i].x  = g_pTxtFImport->thinkXW16(pTf->textField.aSP[i].x);
		pTf->textField.aSP[i].y  = g_pTxtFImport->thinkYH16(pTf->textField.aSP[i].y);
		pTf->textField.aSP[i].z  = g_pTxtFImport->thinkXW16(pTf->textField.aSP[i].z);
		pTf->textField.aSP[i].w  = g_pTxtFImport->thinkXW16(pTf->textField.aSP[i].w);
		pTf->textField.aSP[i].h  = g_pTxtFImport->thinkYH16(pTf->textField.aSP[i].h);

		pTf->textField.aUV[i].u  = STG(pTf->textField.aUV[i].u);
		pTf->textField.aUV[i].v  = STG(pTf->textField.aUV[i].v);
		pTf->textField.aUV[i].tw = STG(pTf->textField.aUV[i].tw);
		pTf->textField.aUV[i].th = STG(pTf->textField.aUV[i].th);

		pTf->candList.aSP[i].x  = g_pTxtFImport->thinkXW16(pTf->candList.aSP[i].x);
		pTf->candList.aSP[i].y  = g_pTxtFImport->thinkYH16(pTf->candList.aSP[i].y);
		pTf->candList.aSP[i].z  = g_pTxtFImport->thinkXW16(pTf->candList.aSP[i].z);
		pTf->candList.aSP[i].w  = g_pTxtFImport->thinkXW16(pTf->candList.aSP[i].w);
		pTf->candList.aSP[i].h  = g_pTxtFImport->thinkYH16(pTf->candList.aSP[i].h);

		pTf->candList.aUV[i].u  = STG(pTf->candList.aUV[i].u);
		pTf->candList.aUV[i].v  = STG(pTf->candList.aUV[i].v);
		pTf->candList.aUV[i].tw = STG(pTf->candList.aUV[i].tw);
		pTf->candList.aUV[i].th = STG(pTf->candList.aUV[i].th);
	}
}


#ifndef ATOK_NO_LINK
//============================================================================
//	drawCandListFrame
//
//	候補一覧のフレーム描画用パケット作成
//
//	sceVif1Packet *pk : VIF1パケット格納用バッファへのポインタ
//	textField_t *pTf : テキストフィールド構造体へのポインタ
//	iRect_t *rc16 : 矩形を構成するバッファへのポインタ
//	int nCurrentCand : 選択中候補番号
static void drawCandListFrame(sceVif1Packet *pk, textField_t *pTf, iRect_t *rc16, int nCurrentCand)
{
	int i;
	int x16 = rc16->x;
	int y16 = rc16->y;

	memcpy(pTf->candList.aSP, g_spSrc,  sizeof(sprite_t)*TF_FRAME_ELEMENT);

	// 座標を修正
	for(i=0; i<TF_FRAME_ELEMENT; i++)
	{
		pTf->candList.aSP[i].x = g_pTxtFImport->thinkXW16(pTf->candList.aSP[i].x) + x16;
		pTf->candList.aSP[i].y = g_pTxtFImport->thinkYH16(pTf->candList.aSP[i].y) + y16;
		pTf->candList.aSP[i].z = g_pTxtFImport->thinkXW16(pTf->candList.aSP[i].z);
		pTf->candList.aSP[i].w = g_pTxtFImport->thinkXW16(pTf->candList.aSP[i].w);
		pTf->candList.aSP[i].h = g_pTxtFImport->thinkYH16(pTf->candList.aSP[i].h);
	}

	// 枠の幅、高さ変更
	stretchRect(pTf->candList.aSP, rc16->w, rc16->h);

	// 枠描画
	{
		sprite_t   *pSp = pTf->candList.aSP;
		uv_t       *pUv = pTf->candList.aUV;
		paneChip_t *pPn = pTf->candList.aPN;
		int spIndex, uvIndex;
		u_int col;

		g_pTxtFImport->loadTex(pk, &g_TF.tex);											// テクスチャロード
		for(i=0; i<TF_FRAME_ELEMENT; i++, pPn++)
		{
			spIndex = pPn->spIndex;
			uvIndex = pPn->uvIndex;
			col = affectAlpha(pPn->rgba, pTf->alphaValue[TF_ALPHA_CANDLISTBG]);
			g_pTxtFImport->drawTex(pk, col, (pSp+spIndex), (pUv+uvIndex), &g_TF.tex);	// テクスチャ描画
		}
	}
}
#endif
