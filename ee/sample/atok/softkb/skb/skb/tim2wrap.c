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
// tim2.cのラップ関数群

#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <eekernel.h>
#include <eetypes.h>
#include <sifdev.h>
#include <libgraph.h>
#include <libpkt.h>


#include "tim2.h"
#include "tim2wrap.h"

static int   setTexInfo(textureInfo_t *pTex);
static u_int tim2LoadClut(sceVif1Packet *pk, TIM2_PICTUREHEADER *ph, u_int cbp);
//static int getLog2(u_long32 n);

static struct
{
	u_int uiCreateNum; // 作成した回数

	u_int sw, sh;      // スクリーン幅、高さ
	u_int tbp;         // テクスチャベースポインタ
	u_int cbp;         // クラットベースポインタ
	u_int offx, offy;  // 

	void* (*malign)(size_t align, size_t size); // メモリ確保コールバック
	void  (*mfree)(void *p);                    // メモリ開放コールバック
} g_Tim2;


//============================================================================
// PUBLIC FUNCTIONS
//============================================================================
void Tim2_Init( u_int screenWidth, u_int screenHeight,
                u_int offX, u_int offY,
                u_int texBasePtr, u_int clutBasePtr,
                void* (*malign)(size_t align, size_t size),
                void  (*mfree)(void *p))
{
	memset(&g_Tim2, 0x0, sizeof(g_Tim2));

	g_Tim2.sw = screenWidth;  // スクリーン幅
	g_Tim2.sh = screenHeight; // 高さ
	g_Tim2.tbp = texBasePtr;  // テクスチャベースポインタ
	g_Tim2.cbp = clutBasePtr; // クラットベースポインタ
	g_Tim2.offx = offX;       //
	g_Tim2.offy = offY;       //
	g_Tim2.malign = malign;
	g_Tim2.mfree  = mfree;
}






//============================================================================
//  Tim2_ReadTim2                                            ref: tim2wrap.h
//
//  Tim2ファイル読込み
//
//  textureInfo_t *pTex  : テクスチャ構造体
//  const char *filename : TIM2ファイル名
//
//  戻り値 ： int 0:成功/1:失敗
int Tim2_ReadTim2(textureInfo_t *pTex, const char *filename)
{
	#if 0//SKB_EX Binlink
	// ターゲットからTIM2ファイル読込み
	int fd, ret, size;
	SKB_ASSERT(pTex);

	// ファイル読込み
	printf("MSG : reading - %s\n", filename);
	fd = sceOpen(filename, SCE_RDONLY);
	if(fd < 0)
	{
		printf("ERROR : readTIM2 - open(%s)\n", filename);
		return 1;
	}
	size = sceLseek(fd, 0, SCE_SEEK_END);
	sceLseek(fd, 0, SCE_SEEK_SET);
	if(size <= 0)
	{
		printf("ERROR : readTIM2 - size(%d)\n", size);
		return 1;
	}
	pTex->pTim2Buffer = (char*)g_Tim2.malign(64, size);
	if(pTex->pTim2Buffer == NULL)
	{
		printf("ERROR : readTIM2 - memory allocate failed(%d)\n", size);
		return 1;
	}
	ret = sceRead(fd, (char*)pTex->pTim2Buffer, size);
	if(ret < 0)
	{
		printf("ERROR : readTIM2 - failed to read(%d)\n", ret);
		g_Tim2.mfree(pTex->pTim2Buffer);
		pTex->pTim2Buffer = NULL;
		return 1;
	}
	sceClose(fd);
	#else
	int ret;
	pTex->pTim2Buffer = (char*)filename; 
	                   //名前はfilename ですが、tim2 のaddressとして使用。
	#endif

	// データ設定
	ret = setTexInfo(pTex);
	if(!ret)
	{
		// データ設定成功
		g_Tim2.uiCreateNum++;
		return 0;
	}

	// データ設定失敗
	return 1;
}


//============================================================================
//  Tim2_DeleteTim2                                          ref: tim2wrap.h
//
//  TIM2ファイルデータをテクスチャバッファに転送する為のパケット作成
//
//  textureInfo_t *pTex : テクスチャ構造体
//
//  戻り値 ： void
void Tim2_DeleteTim2(textureInfo_t *pTex)
{
	assert(pTex);

	if(pTex->pTim2Buffer)
	{
		#if 0//SKB_EX BinLink
		// Tim2バッファ削除
		g_Tim2.mfree(pTex->pTim2Buffer);
		#endif
		pTex->pTim2Buffer = NULL;
		--g_Tim2.uiCreateNum;
	}
}


//============================================================================
//  Tim2_SendTim2                                            ref: tim2wrap.h
//
//  TIM2ファイルデータをテクスチャバッファに転送する為のパケット作成
//
//  sceVif1Packet *pk   : パケットバッファのポインタ
//  textureInfo_t *pTex : テクスチャ構造体
//
//  戻り値 ： int 0:成功/1:失敗
void Tim2_SendTim2(sceVif1Packet *pk, textureInfo_t *pTex)
{
//	assert(pTex);
	{
		TIM2_PICTUREHEADER *ph;
		ph = Tim2GetPictureHeader(pTex->pTim2Buffer, 0);
		if( !ph )
		{
			printf("ERROR : setTexInfo - broken tim2.\n");
			return;
		}

		// ピクチャデータをVRAMに読み込み
		{
			static const int psmtbl[] =
			{
				SCE_GS_PSMCT16,
				SCE_GS_PSMCT24,
				SCE_GS_PSMCT32,
				SCE_GS_PSMT4,
				SCE_GS_PSMT8
			};
			int psm;
			int w, h;
			u_long128 *pImage;
			psm = psmtbl[(ph->ImageType&0x3f) - 1];		// ピクセルフォーマットを得る

			// イメージデータの先頭アドレスを計算
			pImage = (u_long128 *)((char *)ph + ph->HeaderSize);
			((sceGsTex0 *)&ph->GsTex0)->PSM  = psm;

			w = ph->ImageWidth;   // イメージXサイズ
			h = ph->ImageHeight;  // イメージYサイズ

			// ピクチャデータパケット用意
			sceVif1PkRefLoadImage( pk, (u_short)g_Tim2.tbp, (u_char)psm,
			                      (u_short)((ph->ImageWidth+63)/64),
			                      pImage,
			                      (u_int)(ph->ImageSize/16),
			                      (u_int)0, (u_int)0,
			                      (u_int)w, (u_int)h);

			if( ph->ClutColors )
			{
				// クラットパケット用意
				tim2LoadClut(pk, ph, g_Tim2.cbp);
			}
		}

		{
			const u_long64 giftagAD[2] =
			{
				SCE_GIF_SET_TAG(0, 1, 0, 0, SCE_GIF_PACKED, 1), // A+D用GIFタグ
				0x000000000000000EL
			};
			// テクスチャフラッシュ
			sceVif1PkCnt(pk, 0);
			sceVif1PkOpenDirectHLCode( pk, 0 );
			sceVif1PkOpenGifTag(pk, *(u_long128*)&giftagAD); // GIFタグを設定
			sceVif1PkAddGsData(pk, 0x00000000);       // D
			sceVif1PkAddGsData(pk, SCE_GS_TEXFLUSH);  // A(TEXFLUSH)
			sceVif1PkCloseGifTag(pk);                 // GIFタグを閉じる
			sceVif1PkCloseDirectHLCode( pk );
		}
	}
}


//============================================================================
//  Tim2_DrawSprite                                          ref: tim2wrap.h
//
//  スプライト描画
//
//  sceVif1Packet *pk : パケットバッファへのポインタ
//  u_int col         : スプライト色
//  sprite_t *pSp     : スプライト構造体のポインタ
//  uv_t *pUv         : UV構造体のポインタ
//  textureInfo_t *pTex : テクスチャ構造体のポインタ
//
//  戻り値 ： void
void Tim2_DrawSprite(sceVif1Packet *pk, u_int col, sprite_t *pSp, uv_t *pUv, textureInfo_t *pTex)
{
	int	x=0, y=0, w=0, h=0, z=0;
	int u=0, v=0, tw=0, th=0;
	u_int offx = g_Tim2.offx;
	u_int offy = g_Tim2.offy;

	// スプライト用GIFタグ
	const u_long64 giftagSPR[2] =
	{
		SCE_GIF_SET_TAG(0, 1, 0, 0, SCE_GIF_REGLIST, 8),
		0x00000000F4343810L   // PRIM,RGBAQ,CLAMP_1,UV,XYZF2,UV,XYZF2
	};
	const u_long64 giftagAD[2] =
	{
		SCE_GIF_SET_TAG(0, 1, 0, 0, SCE_GIF_PACKED, 1), // A+D用GIFタグ
		0x000000000000000EL
	};

	// GSのレジスタ設定
	sceVif1PkCnt(pk, 0);
	sceVif1PkOpenDirectHLCode( pk, 0 );
	sceVif1PkOpenGifTag(pk, *((u_long128*)&giftagAD));  // GIFタグを設定

	// TEX1レジスタの設定
	sceVif1PkAddGsData(pk,
			SCE_GS_SET_TEX1(1, pTex->bMipmapLv, 0, pTex->nFilter, 0, 0, pTex->nLod));	// D(TEX1_1)
	sceVif1PkAddGsData(pk, SCE_GS_TEX1_1);		// A(TEX1_1)

	// TEX0レジスタの値を設定
	if(pTex->GsTex0.PSM==SCE_GS_PSMT4)
	{
		// 16色テクスチャのとき
		sceGsTex0 Load;     // TEX0(CLUT読み込み用)
		sceGsTex0 Change;   // TEX0(CSA変更用)

		// GSのTEX0レジスタの設定をコピーして、CLUT読み込み用とCSA変更用に値を修正
		Load        = pTex->GsTex0;      // オリジナルのTEX0_1をコピー
		Load.PSM    = SCE_GS_PSMT8;      // 8bitテクスチャだとフェイク
		Load.CBP   += pTex->nClutNo/16;  // CBPのオフセットをずらす
		Load.TCC    = 1;                 //
		Load.TFX    = 0;                 //
		Load.CSA    = 0;                 // CSAは先頭を使う
		Load.CLD    = 1;                 // CLUTバッファのロードをおこなう

		Change      = pTex->GsTex0;          // オリジナルのTEX0_1をコピー
		Change.CBP += pTex->nClutNo/16;      // CBPのオフセットをずらす
		Change.TCC    = 1;                   //
		Change.TFX    = 0;                   //
		Change.CSA  = (pTex->nClutNo & 0x0F);// CSAは先頭を使う
		Change.CLD  = 0;                     // CLUTバッファのロードをおこなわない

		// いったん256色CLUTとだましてCLUTバッファにロード
		sceVif1PkAddGsData(pk, *(u_long64 *)&Load);  // D(TEX0_1)
		sceVif1PkAddGsData(pk, SCE_GS_TEX0_1);       // A(TEX0_1)

		// もういちどTEX0レジスタの値を設定してCSAだけ変更
		sceVif1PkAddGsData(pk, *(u_long64 *)&Change);// D(TEX0_1)
		sceVif1PkAddGsData(pk, SCE_GS_TEX0_1);       // A(TEX0_1)
	}
	else
	{
		// 16色以外のとき
		sceGsTex0 t0;   // TEX0

		t0 = pTex->GsTex0;         // オリジナルのTEX0_1を
		t0.CBP += pTex->nClutNo*4; // CBPのオフセットをずらす
		t0.TCC = 1;                //
		t0.TFX = 0;                //

		sceVif1PkAddGsData(pk, *(u_long64 *)&t0); // D(TEX0_1)
		sceVif1PkAddGsData(pk, SCE_GS_TEX0_1);    // A(TEX0_1)
	}

	// MIPTBP1レジスタの設定
	sceVif1PkAddGsData(pk, pTex->GsMiptbp1);  // D(MIPTBP1_1)
	sceVif1PkAddGsData(pk, SCE_GS_MIPTBP1_1); // A(MIPTBP1_1)

	// MIPTBP2レジスタの設定
	sceVif1PkAddGsData(pk, pTex->GsMiptbp2);  // D(MIPTBP2_1)
	sceVif1PkAddGsData(pk, SCE_GS_MIPTBP2_1); // A(MIPTBP2_1)

	// ALPHA_1レジスタの設定
	sceVif1PkAddGsAD(pk, SCE_GS_ALPHA_1, SCE_GS_SET_ALPHA(SCE_GS_ALPHA_CS, SCE_GS_ALPHA_CD, SCE_GS_ALPHA_AS, SCE_GS_ALPHA_CD, 0x00));

	sceVif1PkCloseGifTag(pk); // GIFタグを閉じる
	sceVif1PkCloseDirectHLCode( pk );

	// スプライト描画
	sceVif1PkOpenDirectHLCode( pk, 0 );
	sceVif1PkOpenGifTag(pk, *((u_long128*)&giftagSPR)); // GIFタグを設定

	// PRIMレジスタの値を設定(0x0)
	// Sprite,フラット,テクスチャ,フォグOFF,アルファON
	// アンチエイリアスON,UV指定,コンテキスト1,フラグメント値制御
	// αチャネル使用しない
	sceVif1PkAddGsData(pk, SCE_GS_SET_PRIM(SCE_GS_PRIM_SPRITE, 0, 1, 0, 1, 1, 1, 0, 0));

	// RGBAQレジスタの値を設定(0x1)
	sceVif1PkAddGsData(pk, col);

	// CLAMPレジスタの値を設定(0x8)
	sceVif1PkAddGsData(pk, SCE_GS_SET_CLAMP(0, 0, 0, 0, 0, 0));

	// UV,XY値をそれぞれ設定(左上,右下)
	x = pSp->x;
	y = pSp->y;
	z = pSp->z;
	w = pSp->w;
	h = pSp->h;
	u = pUv->u;
	v = pUv->v;
	tw = pUv->tw;
	th = pUv->th;
	sceVif1PkAddGsData(pk, SCE_GS_SET_UV (u, v));                  // (0x3)
	sceVif1PkAddGsData(pk, SCE_GS_SET_XYZ(offx+x,   offy+y,   z)); // (0x4)
	sceVif1PkAddGsData(pk, SCE_GS_SET_UV (u+tw, v+th));            // (0x3)
	sceVif1PkAddGsData(pk, SCE_GS_SET_XYZ(offx+x+w, offy+y+h, z)); // (0x4)
	sceVif1PkAddGsData(pk, 0);       // 0xF(アラインメント調整)
	sceVif1PkCloseGifTag(pk);        // GIFタグを閉じる
	sceVif1PkCloseDirectHLCode( pk );
}


//============================================================================
// STATIC FUNCTIONS
//============================================================================
// テクスチャ構造体に値設定
static int setTexInfo(textureInfo_t *pTex)
{
	assert(pTex);

	if(!Tim2CheckFileHeaer(pTex->pTim2Buffer))
	{
		// TIM2ファイルが壊れている
		printf("ERROR : setTexInfo - broken tim2.\n");
		return 1;
	}
	else
	{
		// ピクチャヘッダを得る
		TIM2_PICTUREHEADER *ph;
		ph = Tim2GetPictureHeader(pTex->pTim2Buffer, 0);
		if(!ph)
		{
			printf("ERROR : setTexInfo - failed to get TIM2_PICTUREHEADER.\n");
			return 1;
		}
		else
		{
			// ピクチャヘッダをVRAMに読込み
			Tim2LoadPicture(ph, g_Tim2.tbp, g_Tim2.cbp);
			// ピクチャヘッダからいくつかのデータを得る
			pTex->GsTex0 = *(sceGsTex0 *)&ph->GsTex0;  // ピクチャヘッダからTEX0レジスタへの設定値を得る
			pTex->nTexSizeW = ph->ImageWidth;          // ピクチャサイズX
			pTex->nTexSizeH = ph->ImageHeight;         // ピクチャサイズY
			// ミップマップレベルを得る
			pTex->bMipmapLv = ph->MipMapTextures - 1;  // ミップマップレベル
			if(pTex->bMipmapLv) {
				// MIPMAPを使用するとき
				TIM2_MIPMAPHEADER *pm;
				pm = (TIM2_MIPMAPHEADER *)(ph + 1);  // ピクチャヘッダの直後にMIPMAPヘッダ
				pTex->GsMiptbp1 = pm->GsMiptbp1;
				pTex->GsMiptbp2 = pm->GsMiptbp2;
			} else {
				// MIPMAPを使用しないとき
				pTex->GsMiptbp1 = 0;
				pTex->GsMiptbp2 = 0;
			}
		}
	}

	return 0;
}


// クラットロード
static u_int tim2LoadClut(sceVif1Packet *pk, TIM2_PICTUREHEADER *ph, u_int cbp)
{
	int i;
	int	cpsm;
	int colByte=0;
	u_long128 *pClut;

	// CLUTピクセルフォーマットを得る
	if(ph->ClutType==TIM2_NONE)
	{
		// CLUTデータが存在しないとき
		return(1);
	}
	else if((ph->ClutType & 0x3F)==TIM2_RGB16)
	{
		cpsm = SCE_GS_PSMCT16;
		colByte = 2;
	}
	else if((ph->ClutType & 0x3F)==TIM2_RGB24)
	{
		cpsm = SCE_GS_PSMCT24;
		colByte = 3;
	}
	else
	{
		cpsm = SCE_GS_PSMCT32;
		colByte = 4;
	}
	((sceGsTex0 *)&ph->GsTex0)->CPSM = cpsm; // CLUT部ピクセルフォーマット設定
	((sceGsTex0 *)&ph->GsTex0)->CSM  = 0;    // CLUT格納モード(常にCSM1)
	((sceGsTex0 *)&ph->GsTex0)->CSA  = 0;    // CLUTエントリオフセット(常に0)
	((sceGsTex0 *)&ph->GsTex0)->CLD  = 1;    // CLUTバッファのロード制御(常にロード)

	if(cbp==-1)
	{
		// cbpの指定がないとき、ピクチャヘッダのGsTex0メンバから値を取得
		cbp = ((sceGsTex0 *)&ph->GsTex0)->CBP;
	}
	else
	{
		// cbpが指定されたとき、ピクチャヘッダのGsTex0メンバの値をオーバーライド
		((sceGsTex0 *)&ph->GsTex0)->CBP = cbp;
	}

	// CLUTデータの先頭アドレスを計算
	pClut = (u_long128 *)((char *)ph + ph->HeaderSize + ph->ImageSize);

	// CLUTデータをGSローカルメモリに送信
	// CLUT形式とテクスチャ形式によってCLUTデータのフォーマットなどが決まる
	switch((ph->ClutType<<8) | ph->ImageType)
	{
	case (((TIM2_RGB16 | 0x40)<<8) | TIM2_IDTEX4): // 16色,CSM1,16ビット,並び替えずみ
	case (((TIM2_RGB24 | 0x40)<<8) | TIM2_IDTEX4): // 16色,CSM1,24ビット,並び替えずみ
	case (((TIM2_RGB32 | 0x40)<<8) | TIM2_IDTEX4): // 16色,CSM1,32ビット,並び替えずみ
	case (( TIM2_RGB16        <<8) | TIM2_IDTEX8): // 256色,CSM1,16ビット,並び替えずみ
	case (( TIM2_RGB24        <<8) | TIM2_IDTEX8): // 256色,CSM1,24ビット,並び替えずみ
	case (( TIM2_RGB32        <<8) | TIM2_IDTEX8): // 256色,CSM1,32ビット,並び替えずみ
		// 256色CLUTかつ、CLUT格納モードがCSM1のとき
		// 16色CLUTかつ、CLUT格納モードがCSM1で入れ替え済みフラグがONのとき
		// すでにピクセルが入れ替えられて配置されているのでそのまま転送可能だー
		sceVif1PkRefLoadImage( pk, cbp, cpsm,
							  1,
							  pClut,
							  (u_int)((colByte*ph->ClutColors)/16),
							  (u_int)0, (u_int)0,
							  (u_int)16, (u_int)(ph->ClutColors/16));
		break;
	case (( TIM2_RGB16        <<8) | TIM2_IDTEX4): // 16色,CSM1,16ビット,リニア配置
	case (( TIM2_RGB24        <<8) | TIM2_IDTEX4): // 16色,CSM1,24ビット,リニア配置
	case (( TIM2_RGB32        <<8) | TIM2_IDTEX4): // 16色,CSM1,32ビット,リニア配置
	case (((TIM2_RGB16 | 0x80)<<8) | TIM2_IDTEX4): // 16色,CSM2,16ビット,リニア配置
	case (((TIM2_RGB24 | 0x80)<<8) | TIM2_IDTEX4): // 16色,CSM2,24ビット,リニア配置
	case (((TIM2_RGB32 | 0x80)<<8) | TIM2_IDTEX4): // 16色,CSM2,32ビット,リニア配置
	case (((TIM2_RGB16 | 0x80)<<8) | TIM2_IDTEX8): // 256色,CSM2,16ビット,リニア配置
	case (((TIM2_RGB24 | 0x80)<<8) | TIM2_IDTEX8): // 256色,CSM2,24ビット,リニア配置
	case (((TIM2_RGB32 | 0x80)<<8) | TIM2_IDTEX8): // 256色,CSM2,32ビット,リニア配置
		// 16色CLUTかつ、CLUT格納モードがCSM1で入れ替え済みフラグがOFFのとき
		// 16色CLUTかつ、CLUT格納モードがCSM2のとき
		// 256色CLUTかつ、CLUT格納モードがCSM2のとき
		// CSM2はパフォーマンスが悪いので、CSM1として入れ替えながら転送
		{
			for(i=0; i<(ph->ClutColors/16); i++)
			{
				sceVif1PkRefLoadImage( pk, cbp, cpsm,
									  1,
									  pClut,
									  (u_int)((colByte*ph->ClutColors)/16),
									  (u_int)(i & 1)*8, (u_int)(i>>1)*2,
									  (u_int)8, (u_int)2);
				// 次の16色へ、アドレス更新
				if((ph->ClutType & 0x3F)==TIM2_RGB16)
				{
					pClut = (u_long128 *)((char *)pClut + 2*16); // 16bit色のとき
				}
				else if((ph->ClutType & 0x3F)==TIM2_RGB24)
				{
					pClut = (u_long128 *)((char *)pClut + 3*16); // 24bit色のとき
				}
				else
				{
					pClut = (u_long128 *)((char *)pClut + 4*16); // 32bit色のとき
				}
			}
			break;
		}
	default:
		printf("ERROR : tim2LoadClut - Illegal clut and texture combination. ($%02X,$%02X)\n", ph->ClutType, ph->ImageType);
		return 1;
	}

	return 0;
}

// ビット幅を得る
//static int getLog2(u_long32 n)
//{
//	u_long32 i;
//	for(i=31; i>0; i--) {
//		if(n & (1<<i)) {
//			break;
//		}
//	}
//	if(n>(1<<i)) {
//		i++;
//	}
//	return(i);
//}
