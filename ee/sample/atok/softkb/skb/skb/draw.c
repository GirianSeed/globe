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

// プリミティブの生成
#include <sys/types.h>
#include <eetypes.h>
#include <eeregs.h>
#include <eestruct.h>
#include <libpkt.h>
#include <libgraph.h>

#include "env.h"
#include "shareddef.h"


void SetVif1GsReg(sceVif1Packet *pk, u_int GsAdr, u_long GsData);

#if 1//SKB_EX ClipOffset 
static int clipOffsetX=0,clipOffsetY=0;
void SetClipOffset(int x,int y)
{
	clipOffsetX = x;
	clipOffsetY = y;
}
#endif
// 描画エリアを指定RECTでクリッピングする
// RECTは全画面の左上を(0,0)とした座標系
// NULLを渡したとき、デフォルトに戻す
void SetClipArea(sceVif1Packet *pVif1Pk, const iRect_t *pRc)
{
	// シザリングの設定
	if(pRc)
	{
		#if 1//SKB_EX ClipOffset 
		SetVif1GsReg(pVif1Pk, SCE_GS_SCISSOR_1, 
							  SCE_GS_SET_SCISSOR_1( clipOffsetX+pRc->x, (clipOffsetX + pRc->x + pRc->w),
							                        clipOffsetY+pRc->y, (clipOffsetY + pRc->y + pRc->h) )
					 );
		#else
		SetVif1GsReg(pVif1Pk, SCE_GS_SCISSOR_1, 
								SCE_GS_SET_SCISSOR_1(pRc->x, (pRc->x + pRc->w), pRc->y, (pRc->y + pRc->h)));
		#endif
	}
	else
	{
		// NULLが渡されたとき、デフォルトのシザリング領域に戻す
		SetVif1GsReg(pVif1Pk, SCE_GS_SCISSOR_1, 
								SCE_GS_SET_SCISSOR_1(0, SCREEN_WIDTH-1, 0, SCREEN_HEIGHT-1));
	}
}

// 汎用GSレジスタ設定をVIF1パケットに追加
void SetVif1GsReg(sceVif1Packet *pk, u_int GsAdr, u_long GsData)
{
	union128 giftag;

	sceVif1PkCnt(pk, 0);						// DMAcntでパケットを開く
	sceVif1PkOpenDirectCode(pk, 0);				// DIRECTコードを開く

	// GIFタグを開く
	// NLOOP=0, EOP=1, PRE=0, PRIM=0, PACKED, NREGS=1
	giftag.aul[0] = SCE_GIF_SET_TAG(0, SCE_GS_TRUE, SCE_GS_FALSE, 0, SCE_GIF_PACKED, 1);
	giftag.aul[1] = 0x000000000000000EUL;
	sceVif1PkOpenGifTag(pk, giftag.qw);

	sceVif1PkAddGsAD(pk, GsAdr, GsData);		// A+Dデータを追加

	sceVif1PkCloseGifTag(pk);					// GIFタグを閉じる
	sceVif1PkCloseDirectCode(pk);				// DIRECTコードを閉じる
	sceVif1PkTerminate(pk);						// VIF1 DMAcntパケットを閉じる
	return;
}




void SetRectFillEx(sceVif1Packet *pk, const IRECT *pRc, const RGBA32 *c, int z)
{
	int	x, y;
	int	w, h;
	union128 giftag;

	sceVif1PkCnt(pk, 0);						// DMAcntパケットで追加
	sceVif1PkOpenDirectCode(pk, 0);				// DIRECTコードを開く

	// 矩形タイル用GIFタグを開く
	// NLOOP=0, EOP=1, PRE=0, PRIM=0, REGLIST, NREGS=4
	giftag.aul[0] = SCE_GIF_SET_TAG(0, SCE_GS_TRUE, SCE_GS_FALSE, 0, SCE_GIF_REGLIST, 9);
	giftag.aul[1] = 0x0000000414141410UL;		// レジスタ記述子
	sceVif1PkOpenGifTag(pk, giftag.qw);

	// PRIMレジスタの値を設定(0x0)
	// sprite,グーロ,テクスチャなし,フォグOFF,アルファON
	// アンチエイリアスOFF,UV指定,コンテキスト1,フラグメント値制御
	sceVif1PkAddGsData(pk, SCE_GS_SET_PRIM(SCE_GS_PRIM_TRISTRIP, SCE_GS_TRUE, SCE_GS_FALSE, SCE_GS_FALSE, SCE_GS_TRUE,
											SCE_GS_FALSE, 0, 0, SCE_GS_FALSE));

	
	// RGBAQレジスタの値を設定(0x1)
	sceVif1PkAddGsData(pk, (u_long)*c);
	// XY値をそれぞれ設定(0x4)
	x = pRc->x;
	y = pRc->y;
	w = pRc->w;
	h = pRc->h;
	sceVif1PkAddGsData(pk, SCE_GS_SET_XYZ(OFFX+x,   OFFY+y,   z));		// (0x4)
	sceVif1PkAddGsData(pk, *(c+1));
	sceVif1PkAddGsData(pk, SCE_GS_SET_XYZ(OFFX+x+w, OFFY+y, z));		// (0x4)
	sceVif1PkAddGsData(pk, (u_long)*(c+2));
	sceVif1PkAddGsData(pk, SCE_GS_SET_XYZ(OFFX+x,   OFFY+y+h,   z));	// (0x4)
	sceVif1PkAddGsData(pk, (u_long)*(c+3));
	sceVif1PkAddGsData(pk, SCE_GS_SET_XYZ(OFFX+x+w, OFFY+y+h, z));		// (0x4)

	sceVif1PkCloseGifTag(pk);					// GIFタグを閉じる
	sceVif1PkCloseDirectCode(pk);				// DIRECTコードパケットを閉じる
	sceVif1PkTerminate(pk);						// DMAcntパケットを閉じる
	return;
}

// タイルをVIF1パケットに追加
void SetRectFill(sceVif1Packet *pk, const IRECT *pRc, const RGBA32 c, int z)
{
	int	x, y;
	int	w, h;
	union128 giftag;

	sceVif1PkCnt(pk, 0);						// DMAcntパケットで追加
	sceVif1PkOpenDirectCode(pk, 0);				// DIRECTコードを開く

	// 矩形タイル用GIFタグを開く
	// NLOOP=0, EOP=1, PRE=0, PRIM=0, REGLIST, NREGS=4
	giftag.aul[0] = SCE_GIF_SET_TAG(0, SCE_GS_TRUE, SCE_GS_FALSE, 0, SCE_GIF_REGLIST, 4);
	giftag.aul[1] = 0x0000000000004410UL;		// レジスタ記述子
	sceVif1PkOpenGifTag(pk, giftag.qw);

	// PRIMレジスタの値を設定(0x0)
	// sprite,フラット,テクスチャなし,フォグOFF,アルファON
	// アンチエイリアスOFF,UV指定,コンテキスト1,フラグメント値制御
	sceVif1PkAddGsData(pk, SCE_GS_SET_PRIM(SCE_GS_PRIM_SPRITE, SCE_GS_FALSE, SCE_GS_FALSE, SCE_GS_FALSE, SCE_GS_TRUE,
											SCE_GS_FALSE, 0, 0, SCE_GS_FALSE));

	// RGBAQレジスタの値を設定(0x1)
	sceVif1PkAddGsData(pk, (u_long)c);

	// XY値をそれぞれ設定(0x4,0x4)
	x = pRc->x;
	y = pRc->y;
	w = pRc->w;
	h = pRc->h;
	sceVif1PkAddGsData(pk, SCE_GS_SET_XYZ(OFFX+x,   OFFY+y,   z));	// (0x4)
	sceVif1PkAddGsData(pk, SCE_GS_SET_XYZ(OFFX+x+w, OFFY+y+h, z));	// (0x4)

	sceVif1PkCloseGifTag(pk);					// GIFタグを閉じる
	sceVif1PkCloseDirectCode(pk);				// DIRECTコードパケットを閉じる
	sceVif1PkTerminate(pk);						// DMAcntパケットを閉じる
	return;
}


// ラインひくだけ
void SetLine(sceVif1Packet *pk, int x, int y, int cx, int cy, const RGBA32 c, int z)
{
	union128 giftag;

	sceVif1PkCnt(pk, 0);						// DMAcntパケットで追加
	sceVif1PkOpenDirectCode(pk, 0);				// DIRECTコードを開く

	// GIFタグを開く
	// NLOOP=0, EOP=1, PRE=0, PRIM=0, REGLIST, NREGS=8
	giftag.aul[0] = (u_long)SCE_GIF_SET_TAG(0, SCE_GS_TRUE, SCE_GS_FALSE, 0, SCE_GIF_REGLIST, 5);
	giftag.aul[1] = (u_long)0x00000000000F4410UL;
	sceVif1PkOpenGifTag(pk, giftag.qw);

	// PRIMレジスタの値を設定(0x0)
	// LineStrip,フラット,テクスチャなし,フォグOFF,アルファOFF
	// アンチエイリアスOFF,UV指定,コンテキスト1,フラグメント値制御
	sceVif1PkAddGsData(pk, SCE_GS_SET_PRIM(SCE_GS_PRIM_LINESTRIP, SCE_GS_FALSE, SCE_GS_FALSE, SCE_GS_FALSE, SCE_GS_FALSE,
											SCE_GS_FALSE, 0, 0, SCE_GS_FALSE));
	// RGBAQレジスタの値を設定(0x1)
	sceVif1PkAddGsData(pk, (u_long)c);

	// XY値をそれぞれ設定(0x4,0x4,0x4,0x4,0x4)
	sceVif1PkAddGsData(pk, SCE_GS_SET_XYZ(OFFX+x,   OFFY+y,     z));	// (0x4)
	sceVif1PkAddGsData(pk, SCE_GS_SET_XYZ(OFFX+cx,  OFFY+cy,   z));	// (0x4)

	// NOPを追加(0xF)
	sceVif1PkAddGsData(pk, 0x000000000000000UL);

	sceVif1PkCloseGifTag(pk);					// GIFタグを閉じる
	sceVif1PkCloseDirectCode(pk);				// DIRECTコードパケットを閉じる
	sceVif1PkTerminate(pk);						// DMAcntパケットを閉じる
}
