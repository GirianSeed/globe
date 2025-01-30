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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <eekernel.h>
#include <eetypes.h>
#include <sifdev.h>
#include <libpkt.h>
#include <libpad.h>

#include "libfep.h"
#include "fepbar.inc"

#ifdef SKBRES_LINK  //Makefile define
//----------------------------------------------------------------
#include <skbres.h>
#define FEPBAR_TEXNAME_FILE		fepbar_tm2
//----------------------------------------------------------------
#else
//----------------------------------------------------------------
#if CDBOOT
#define FEPBAR_TEXNAME_FILE		LOCALPATH"\\TEX\\SKB\\FEPBAR.TM2;1"
#else
#define FEPBAR_TEXNAME_FILE		LOCALPATH"/tex/skb/fepbar.tm2"
#endif
//----------------------------------------------------------------
#endif

gImport_t		*g_pFepbarImport;


static void ctrlPad(u_short up, u_short down, u_short press, u_short repeat);
static void padDown(u_short up, u_short down, u_short press, u_short repeat);
static void padUp(u_short up);
static void make_packets(sceVif1Packet *pk, textureInfo_t *pTex);
static void make_packets_F(sceVif1Packet *pk, textureInfo_t *pTex);

static struct
{
	u_int uiFepMode[FEPMODE_MAX];	// 入力方法
	u_int uiFepmodeNum;				// 入力方法の数
	u_int uiFepIndex;				// 現在選択中の入力方法

	// pad
	u_short			up;
	u_short			down;
	u_short			press;
	u_short			repeat;


	textureInfo_t	tex;			// モードテクスチャ
	int				modebarPos16[2];// モードバー位置

} g_Fepbar;


//============================================================================
// PUBLIC FUNCTIONS
//============================================================================
//============================================================================
//  Fepbar_Init													ref: fepbar.h
//
//  FEPBARの初期化
//
//  戻り値 : void
int Fepbar_Init( gImport_t *pImport )
{
	if(!pImport)	return -1;
	g_pFepbarImport = pImport;

	memset(&g_Fepbar, 0x0, sizeof(g_Fepbar));

	// テクスチャ読み込み
	if(g_pFepbarImport->readTex(&g_Fepbar.tex, FEPBAR_TEXNAME_FILE))
	{
		return 1;
	}
	return  0;
}


//============================================================================
//  Fepbar_Destroy												ref: fepbar.h
//
//  FEPBARの削除
//
//  戻り値 : void
void Fepbar_Destroy(void)
{
	g_pFepbarImport->deleteTex(&g_Fepbar.tex);	// テクスチャ破棄
}


//============================================================================
//  Fepbar_Run													ref: fepbar.h
//
//  毎フレーム処理
//
//  sceVif1Packet *pk : パケットバッファ
//
//  戻り値 : void
void Fepbar_Run(sceVif1Packet *pk)
{
	if(g_pFepbarImport->iRegion == REGION_J)
	{
		// 日本
		make_packets(pk, &g_Fepbar.tex);
	}
	else
	{
		// 海外
		make_packets_F(pk, &g_Fepbar.tex);
	}

}


//============================================================================
//  Fepbar_Move													ref: fepbar.h
//
//  移動
//
//  int x        : 目的位置X
//  int y        : 目的位置Y
//
//  戻り値 : void
void Fepbar_Move(int x, int y)
{
	g_Fepbar.modebarPos16[0] = x << 4;
	g_Fepbar.modebarPos16[1] = VRESOLUTION_ADJUST(y << 4);
}


//============================================================================
//  Fepbar_SetPadData											ref: fepbar.h
//
//  パッド情報設定
//
//  u_long paddata : パッド情報
//
//  戻り値 : void
void Fepbar_SetPadData(u_long paddata)
{
	u_short down, press, repeat, up;

	up     = (paddata>>48)	& 0xFFFFU;
	down   = (paddata>>32)	& 0xFFFFU;
	press  = (paddata>>16)	& 0xFFFFU;
	repeat = (paddata)		& 0xFFFFU;

	if(g_Fepbar.up == up && g_Fepbar.down == down && g_Fepbar.press == press && g_Fepbar.repeat == repeat)
	{
		// 変化が無かった時は終了
		return;
	}

	// パッド情報設定
	ctrlPad(up, down, press, repeat);

	g_Fepbar.up     = up;
	g_Fepbar.down   = down;
	g_Fepbar.press  = press;
	g_Fepbar.repeat = repeat;
}


//============================================================================
//  Fepbar_CreateModeType										ref: fepbar.h
//
//  モードタイプの設定
//
//  const u_int *pTypes : タイプ配列へのポインタ
//  int typeNum         : タイプ数
//
//  戻り値 : void
void Fepbar_CreateModeType(const u_int *pTypes, int typeNum)
{
	int i;
	g_Fepbar.uiFepIndex = 0;
	if(typeNum > FEPMODE_MAX)
	{
		g_pFepbarImport->dprintf("ERROR : Fepbar_CreateModeType - over %d\n", typeNum);
		return;
	}

	for(i=0; i<typeNum; i++)
	{
		if(pTypes[i] >= FEPMODE_MAX)
		{
			g_pFepbarImport->dprintf("ERROR : Fepbar_CreateModeType - unKnown type\n");
		}
		g_Fepbar.uiFepMode[i] = pTypes[i];		// 入力モード設定
	}

	g_Fepbar.uiFepmodeNum = typeNum;			// 入力モード数設定
}


//============================================================================
//  Fepbar_SetModeType											ref: fepbar.h
//
//  モードタイプの設定
//
//  fepAtokInputMode_m type : モードタイプ
//
//  戻り値 : void
void Fepbar_SetModeType(int type)
{
	int ttype = FEPOFF;
	int i;
	// かなロック状態かどうか表示していません。

	switch(type)
	{
	#if 1//SKB_EX FixMode
	case EX_ATOK_FIXHIRAGANA:		// 固定入力ひらがな
		ttype = FEPON_HIRA_FIX;
		break;
	case EX_ATOK_FIXZENKANA:		// 固定入力全角カタカナ
		ttype = FEPON_ZEN_KATAKANA_FIX;
		break;
	case EX_ATOK_FIXZENRAW:			// 固定入力全角無変換
		ttype = FEPON_ZEN_ALPHABET_FIX;
		break;
	case EX_ATOK_FIXHANKANA:		// 固定入力半角カタカナ
		ttype = FEPON_HAN_KATAKANA_FIX;
		break;
	case EX_ATOK_FIXHANRAW:			// 固定入力半角無変換
		ttype = FEPON_HAN_ALPHABET_FIX;
		break;
	#else
	case EX_ATOK_FIXHIRAGANA:		// 固定入力ひらがな
		ttype = FEPON_HIRA;
		break;
	case EX_ATOK_FIXZENKANA:		// 固定入力全角カタカナ
		ttype = FEPON_ZEN_KATAKANA;
		break;
	case EX_ATOK_FIXZENRAW:			// 固定入力全角無変換
		ttype = FEPON_ZEN_ALPHABET;
		break;
	case EX_ATOK_FIXHANKANA:		// 固定入力半角カタカナ
		ttype = FEPON_HAN_KATAKANA;
		break;
	case EX_ATOK_FIXHANRAW:			// 固定入力半角無変換
		ttype = FEPON_HAN_ALPHABET;
		break;
	#endif
	
	case EX_ATOK_FIXOFF:			// 固定入力モードOFF
		ttype = FEPOFF;
		break;

	case EX_ATOK_DSPHIRAGANA:		// ひらがな入力
		ttype = FEPON_HIRA;
		break;
	case EX_ATOK_DSPZENKANA:		// 全角カタカナ入力
		ttype = FEPON_ZEN_KATAKANA;
		break;
	case EX_ATOK_DSPZENRAW:			// 全角無変換入力
		ttype = FEPON_ZEN_ALPHABET;
		break;
	case EX_ATOK_DSPHANKANA:		// 半角カタカナ入力
		ttype = FEPON_HAN_KATAKANA;
		break;
	case EX_ATOK_DSPHANRAW:			// 半角無変換入力
		ttype = FEPON_HAN_ALPHABET;
		break;

	case EX_ATOK_EDIT_KANJIOFF:
		ttype = FEPOFF;
		break;
	default:
		g_pFepbarImport->dprintf("ERROR : Fepbar_SetModeType - Unknown types\n");
		break;
	}

	for(i=0; i<g_Fepbar.uiFepmodeNum; i++)
	{
		if(ttype == g_Fepbar.uiFepMode[i])
		{
			// 既に登録されているモードに合致するインデックスを設定する
			g_Fepbar.uiFepIndex = i;
			break;
		}
	}
}


//============================================================================
//  Fepbar_ClearModeType										ref: fepbar.h
//
//  モードタイプの初期化
//
//  戻り値 : void
void Fepbar_ClearModeType(void)
{
	g_Fepbar.uiFepIndex = 0;
	g_Fepbar.uiFepmodeNum = 0;
}


#if 1//SKB_EX Fepbar_GetNowMode
//============================================================================
//  Fepbar_GetNowMode											ref: fepbar.h
//
//  FEP入力モードの取得
//
//  戻り値 : Fep入力モード
//
int Fepbar_GetNowMode()
{
	return g_Fepbar.uiFepMode[ g_Fepbar.uiFepIndex ];
}
#endif


#if 1//SKB_EX Fepbar_NextMode
//============================================================================
//  Fepbar_NextMode													ref: None
//
//  FEP入力モードを次のFEP入力モードへシフトする。（順番はクリエイト順）
//
//  戻り値 : void
void Fepbar_NextMode(void)
{
	u_int nextMode = g_Fepbar.uiFepIndex+1;

	if(g_Fepbar.uiFepmodeNum == 1)
	{
		return;
	}
	if(nextMode >= g_Fepbar.uiFepmodeNum)
	{
		nextMode = 0;
	}
	printf("Fepbar_NextMode() Num[%d] nextMode[%d]\n",g_Fepbar.uiFepmodeNum,nextMode);
	
	Fep_SetInputMode(g_Fepbar.uiFepMode[nextMode]);	// 次の入力モード設定
	//g_Fepbar.uiFepIndex = nextMode;	libfepの指示に従うので設定しない。

	Fep_se_play(SKBSE_CHANGEKEYTOP); 
}
#endif


//============================================================================
// STATIC FUNCTIONS
//============================================================================
// パッドコントロール
static void ctrlPad(u_short up, u_short down, u_short press, u_short repeat)
{
	padDown(up, down, press, repeat);
	padUp  (up);
}

// パッド押下
static void padDown(u_short up, u_short down, u_short press, u_short repeat)
{
	if(!g_pFepbarImport->isConv())
	{
		// 変換していない時のみ以下の操作OK
		if(down & SCE_PADRright)
		{
			// ソフトウェアキーボード表示
			g_pFepbarImport->fep.openSoftKB(1);
		}
		if(down & SCE_PADselect)
		{
			#if 1//SKB_EX Fepbar_NextMode
			Fepbar_NextMode(); // AtokModeChange
			#else
			// モード切替
			u_int nextMode = g_Fepbar.uiFepIndex+1;
			if(nextMode >= g_Fepbar.uiFepmodeNum)
			{
				nextMode = 0;
			}
	
			Fep_SetInputMode(g_Fepbar.uiFepMode[nextMode]);	// 次の入力モード設定
			g_Fepbar.uiFepIndex = nextMode;					// 入力モードのインデックス指定
			#endif
		}
	}
	if(down & SCE_PADRdown)
	{
		// 文字消去 または、Cancel送信
		if(g_pFepbarImport->iRegion == REGION_J)
		{
			Fep_SetKanjiCancel();
		}
	}
}


// パッド解除
static void padUp(u_short up)
{
	if(up & SCE_PADL1){}
	if(up & SCE_PADL2){}
	if(up & SCE_PADR1){}
	if(up & SCE_PADR2){}
	if(up & SCE_PADRup){}
	if(up & SCE_PADRdown){}
	if(up & SCE_PADRleft){}
	if(up & SCE_PADRright){}
	if(up & SCE_PADselect){}
	if(up & SCE_PADstart){}
}


// 海外版FEPモードバー作成
static void make_packets_F(sceVif1Packet *pk, textureInfo_t *pTex)
{
	int num=0, i=0;
	u_int col = DEFAULT_RGBA;
	uv_t *pUv = NULL;
	sprite_t sp;

	g_pFepbarImport->loadTex(pk, pTex);

	// 土台を描画
	num = sizeof(g_spFepbarBase_F16)/sizeof(sprite_t);
	for(i=0; i<num; i++)
	{
		sp.x = g_spFepbarBase_F16[i].x + g_Fepbar.modebarPos16[0];
		sp.y = g_pFepbarImport->thinkYHn(g_spFepbarBase_F16[i].y) + g_Fepbar.modebarPos16[1];
		sp.z = 0;
		sp.w = g_spFepbarBase_F16[i].w;
		sp.h = g_pFepbarImport->thinkYHn(g_spFepbarBase_F16[i].h);
		g_pFepbarImport->drawTex(pk, col, &sp, (uv_t*)&g_uvFepbarBase_F[i], pTex);
	}

	// ソフトウェアキーボード表示ボタン描画
	pUv = (uv_t*)&g_uvFepbarSkbNormal;
	sp.x = g_spFepbarSkbVisible_F16.x + g_Fepbar.modebarPos16[0];
	sp.y = g_pFepbarImport->thinkYHn(g_spFepbarSkbVisible_F16.y) + g_Fepbar.modebarPos16[1];
	sp.z = 0;
	sp.w = g_spFepbarSkbVisible_F16.w;
	sp.h = g_pFepbarImport->thinkYHn(g_spFepbarSkbVisible_F16.h);
	g_pFepbarImport->drawTex(pk, col, &sp, pUv, pTex);

	// フォーカス描画
	g_pFepbarImport->drawFocus(pk, &sp, 0, 0x80808080);
}


// 日本版FEPモードバー作成
static void make_packets(sceVif1Packet *pk, textureInfo_t *pTex)
{
	int num=0, i=0;
	u_int col = DEFAULT_RGBA;
	uv_t *pUv = NULL;
	sprite_t sp;
	int index = g_Fepbar.uiFepIndex;
	int mode  = g_Fepbar.uiFepMode[index];

	g_pFepbarImport->loadTex(pk, pTex);

	// 土台を描画
	num = sizeof(g_spFepbarBase_J16)/sizeof(sprite_t);
	for(i=0; i<num; i++)
	{
		sp.x = g_spFepbarBase_J16[i].x + g_Fepbar.modebarPos16[0];
		sp.y = g_pFepbarImport->thinkYHn(g_spFepbarBase_J16[i].y) + g_Fepbar.modebarPos16[1];
		sp.z = 0;
		sp.w = g_spFepbarBase_J16[i].w;
		sp.h = g_pFepbarImport->thinkYHn(g_spFepbarBase_J16[i].h);
		g_pFepbarImport->drawTex(pk, col, &sp, (uv_t*)&g_uvFepbarBase_J[i], pTex);
	}

	// 現在のモードを描画
	#if 1//SKB_EX FixMode
	if( mode >= FEPON_HIRA_FIX ){
		mode-=FEPON_HIRA_FIX;
		pUv = (uv_t*)g_uvFepbarPress;
	}
	else{
		pUv = (uv_t*)g_uvFepbarNormal;
	}
	#else
	pUv = (uv_t*)g_uvFepbarNormal;
	#endif
	sp.x = g_spInputMode16.x + g_Fepbar.modebarPos16[0];
	sp.y = g_pFepbarImport->thinkYHn(g_spInputMode16.y) + g_Fepbar.modebarPos16[1];
	sp.w = g_spInputMode16.w;
	sp.h = g_pFepbarImport->thinkYHn(g_spInputMode16.h);
	g_pFepbarImport->drawTex(pk, col, &sp, &pUv[mode], pTex);

	// ソフトウェアキーボード表示ボタン描画
	pUv = (uv_t*)&g_uvFepbarSkbNormal;
	sp.x = g_spFepbarSkbVisible_J16.x + g_Fepbar.modebarPos16[0];
	sp.y = g_pFepbarImport->thinkYHn(g_spFepbarSkbVisible_J16.y) + g_Fepbar.modebarPos16[1];
	sp.z = 0;
	sp.w = g_spFepbarSkbVisible_J16.w;
	sp.h = g_pFepbarImport->thinkYHn(g_spFepbarSkbVisible_J16.h);
	g_pFepbarImport->drawTex(pk, col, &sp, pUv, pTex);

	// フォーカス描画
	g_pFepbarImport->drawFocus(pk, &sp, 0, 0x80808080);
}
