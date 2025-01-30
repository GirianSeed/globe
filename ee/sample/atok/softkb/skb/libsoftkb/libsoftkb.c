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
// ソフトウェアキーボード内部の文字コードはUCS2(int型でとってありますが)です。

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <eekernel.h>
#include <eetypes.h>
#include <sifdev.h>
#include <libscf.h>
#include <libpkt.h>
#include <libpad.h>

#include "libsoftkb.h"
#include "help.inc"
#include "modebar.inc"
#include "resdef.h"			// リソースファイル名を定義

// プロトタイプ宣言
gImport_t		*g_pSkbImport = NULL;		// メイン側関数群のポインタ
protectedSkb_t	g_protectSkb;				// SOFTKB関数群からも参照できる構造体

extern void SkbCore_Init(exportSkb_t *pExport);
extern void SkbList_Init(exportSkb_t *pExport);


// テクスチャバッファ内部定義
enum
{
	TEX_BG,
	TEX_KEYTOP,
	TEX_BTN,
	TEX_MODE,
	TEX_HELPITEMS,

	TEX_HELPCHARS,

	TEX_MAX
};



static struct
{
	u_int			uiModebarItemNum;	// モードバー要素数
	u_int			uiCreateNum;		// 作成回数

	// 画面周り
	textureInfo_t	tex[TEX_MAX];		// テクスチャ
	sprite_t		spCursor[4+1];		// カーソル スプライト
	uv_t			uvCursor[4+1];		// カーソル UV

	// モードバー
	u_int			auiModebarType[KEYTOP_NUM_MAX];	// モードバータイプ数
													// MAX : 10個

	int				modebarPos16[2];				// モードバー位置
	sprite_t		modebarBgSp[KEYTOP_NUM_MAX];	// モードバースプライト背景
	sprite_t		modebarCharSp[KEYTOP_NUM_MAX];	// モードバースプライト文字

	// pad
	u_short			up;
	u_short			down;
	u_short			press;
	u_short			repeat;
	u_int			padCnt[16];			// 同時に一個のボタンしか処理しない為のバッファ

	// ヘルプ
	sprite_t		SpHelpItems[10];	// ヘルプ ボタン等アイテム ワーク
	sprite_t		SpHelpChars[10];	// ヘルプ 文字 ワーク
	sprite_t		SpHelpTitle;		// "基本操作画面" ワーク
	sprite_t		SpAtokVer;			// ATOKバージョン
	sprite_t		SpAtokLogo;			// ATOKロゴ
	sprite_t		SpFrame[9][9];		// フレーム(9スプライト/1フレーム)
	iRect_t			rcFrame[9];

	// マウス
	u_int			uiBtn;				// ボタン情報
										// 1bit : 左ボタン
										// 2bit : 右ボタン
										// 4bit : ホイール
	iPoint_t		mousePos;			// マウス位置情報
	u_int			uiWheel;			// ホイール情報


} g_libskb;


static void initHelp(void);
static void setFrame(sprite_t *sp0, iRect_t *rc, int x, int y);
static void drawHelp(sceVif1Packet *pk, skey_t *pKey, textureInfo_t *pChars, textureInfo_t *pItems);
static void makeModebars(void);
static void makePackets_modebars(sceVif1Packet *pk, skey_t *pKey, textureInfo_t *pTexBtn, textureInfo_t *pTexChar);
static void openingAnim(skey_t *pKey);
static void closingAnim(skey_t *pKey);
static int searchModebarByPoint(skey_t *pKey, iPoint_t *pt);

//============================================================================
// PUBLIC FUNCTIONS
//============================================================================
//============================================================================
//  SoftKB_Init												ref:  libsoftkb.h
//
//  初期化
//
//  gImport_t *pImport : コールバック関数群ポインタ
//  戻り値 : int : 0:初期化成功/1:初期化失敗
int SoftKB_Init(gImport_t *pImport)
{
	int i, typeNum, lang;
	SKB_ASSERT(pImport);

	memset(&g_libskb, 0x0, sizeof(g_libskb));			// ワーク初期化
	memset(&g_protectSkb, 0x0, sizeof(protectedSkb_t));	// ワーク初期化
	g_pSkbImport = pImport;						// メイン側関数群のポインタ

	#if 1//SKB_EX BinLink
	//init_g_RSC();
		#ifndef REGION_J_OTHER_ONLY
	g_RSC[ 0].caKeyTopFilename = aiuHira_bin; // 50音配列ひらがな
	g_RSC[ 1].caKeyTopFilename = aiuKana_bin; // 50音配列カタカナ
	g_RSC[ 2].caKeyTopFilename = abc_bin;     // ABC配列
	g_RSC[ 3].caKeyTopFilename = kigo_bin;    // 記号(特殊ボタンだけ)
	g_RSC[ 4].caKeyTopFilename = num_bin;     // 数字(日本版：全角半角ボタン付き)
	g_RSC[ 5].caKeyTopFilename = qwerty_bin;  // 日本
		#endif
		#ifndef REGION_J_ONLY
	g_RSC[ 6].caKeyTopFilename = usQ_bin;			// アメリカ
	g_RSC[ 7].caKeyTopFilename = ukQ_bin;			// イギリス
	g_RSC[ 8].caKeyTopFilename = frenchQ_bin;		// フランス
	g_RSC[ 9].caKeyTopFilename = spanishQ_bin;		// スペイン
	g_RSC[10].caKeyTopFilename = germanQ_bin;		// ドイツ
	g_RSC[11].caKeyTopFilename = italianQ_bin;		// イタリア
	g_RSC[12].caKeyTopFilename = dutchQ_bin;		// オランダ
	g_RSC[13].caKeyTopFilename = portugQ_bin;		// ポルトガル
	g_RSC[14].caKeyTopFilename = cfrenchQ_bin;		// カナディアンフレンチ
	g_RSC[15].caKeyTopFilename = numF_bin;			// 数字
	g_RSC[16].caKeyTopFilename = spSign_bin;		// 記号(通貨etc)
	g_RSC[17].caKeyTopFilename = idiomChr_bin;		// 発音記号付き文字
		#endif
	#endif
	typeNum = (sizeof(g_RSC)/sizeof(rsc_t));
	if(typeNum != KEYTOP_MAX)
	{
		// キー配置リソースの数がDefineと一致していなかったらエラー
		g_pSkbImport->dprintf("ERROR %s, %d: SoftKB_Init - resource num wrong.\n", __FILE__, __LINE__);
		return 1;
	}

	// テクスチャを全部ロード(ヘルプ文字を除く)
	for(i=0; i<TEX_HELPCHARS; i++)
	{
		g_pSkbImport->readTex(&g_libskb.tex[i], caTexFilename[i]);
	}

	// テクスチャヘルプ文字のみロード
	lang = g_pSkbImport->usLang;
	g_pSkbImport->readTex(&g_libskb.tex[TEX_HELPCHARS], &caTexLangFilename[lang][0]);
	initHelp();

	return 0;
}



//============================================================================
//  SoftKB_Destroy											ref:  libsoftkb.h
//
//  終了処理
//
//  戻り値 : void
void SoftKB_Destroy(void)
{
	int i;

	// テクスチャを全部破棄
	for(i=0; i<TEX_MAX; i++)
	{
		g_pSkbImport->deleteTex(&g_libskb.tex[i]);
	}
}


//============================================================================
// SoftKB_UseMouse											ref:  libsoftkb.h
//
// ソフトウェアキーボードでマウスを使う
//
void SoftKB_UseMouse(u_char flag)
{
	if(flag)
	{
		flag = 1;	// 使う
	}

	g_protectSkb.ucUseMouse = flag;
}


//============================================================================
// SoftKB_CheckShown										ref:  libsoftkb.h
//
// ソフトウェアキーボードが出現中か？
//
//  戻り値 : int
int SoftKB_CheckShown(void)
{
	return g_pSkbImport->skb.ucShowSKbd;
}


//============================================================================
//  SoftKB_Create											ref:  libsoftkb.h
//
//  ソフトウェアキーボード作成
//
//  skey_t *pKey      : キー構造体へのポインタ
//  iPoint_t *skbPoint: ソフトウェアキーボードの位置へのポインタ
//  u_int keyTopNo    : キートップ番号
//  inhibitChar_t *inhibitChar : 入力禁止文字へのポインタ
//  void *pTmp        : 各キーボードで自由に使える
//
//  戻り値 : int 0(成功)/1(失敗)
int SoftKB_Create(skey_t *pKey, iPoint_t *skbPoint, u_int keyTopNo, inhibitChar_t *inhibitChar, void *pTmp)
{
	int ret;
	SKB_ASSERT(pKey);
	SKB_ASSERT(skbPoint);
	SKB_ASSERT(inhibitChar);

	if(keyTopNo > KEYTOP_IDIOMCHAR)
	{
		// キートップの番号が無効
		g_pSkbImport->dprintf("ERROR %s, %d: SoftKB_Create - non support keyNo.\n", __FILE__, __LINE__);
		return 1;
	}

	// ソフトウェアキーボード構造体初期化
	memset(pKey, 0x0, sizeof(skey_t));

	SkbCore_Init(&pKey->export);
	if(keyTopNo == KEYTOP_SIGN)
	{
		// 文字一覧の時は、
		// 通常キーボードと違う処理を設定
		SkbList_Init(&pKey->export);
	}

	// リソースファイル読込み
	ret = pKey->export.readResource(pKey, &g_RSC[keyTopNo].caKeyTopFilename[0], keyTopNo, inhibitChar, pTmp);
	if(ret)
	{
		g_pSkbImport->dprintf("ERROR %s, %d: SoftKB_Create - failed to read resource.\n", __FILE__, __LINE__);
		return 1;
	}
	pKey->uiKeyTopNo  = keyTopNo;
	pKey->ucInputMode = g_RSC[keyTopNo].fepType;	// リソースにより入力モードを設定
	pKey->ucModebarType = g_RSC[keyTopNo].modeType;	// モードタイプを設定

	g_libskb.uiCreateNum++;
	g_pSkbImport->dprintf("MSG : SoftKB_Create - created keyboard(%d).\n", g_libskb.uiCreateNum);
	return 0;
}


//============================================================================
//  SoftKB_Delete											ref:  libsoftkb.h
//
// ソフトウェアキーボード 削除
//
//  戻り値 : void
void SoftKB_Delete(skey_t *pKey)
{
	// リソースファイル削除
	pKey->export.deleteResource(pKey);

	--g_libskb.uiCreateNum;
}


//============================================================================
//  SoftKB_SetModebarType									ref:  libsoftkb.h
//
//  モードバー設定
//
//  const u_int *pTypes : タイプ情報の入ったポインタ
//  int typeNum         : タイプ情報の数
//
//  戻り値 : void
void SoftKB_SetModebarType(const u_int *pTypes, int typeNum)
{
	if(typeNum >= KEYTOP_NUM_MAX)
	{
		g_pSkbImport->dprintf("ERROR : SoftKB_SetModebarType - over %d\n", typeNum);
		return;
	}

	memcpy(g_libskb.auiModebarType, pTypes, typeNum*sizeof(int));	// モードバータイプ設定

	g_libskb.uiModebarItemNum = typeNum;							// モードバー要素数設定
	makeModebars();
}


//============================================================================
//  SoftKB_ClearModebarType									ref:  libsoftkb.h
//
//  モードバーを初期化
//
//  戻り値 : void
void SoftKB_ClearModebarType(void)
{
	g_libskb.uiModebarItemNum = 0;
}


//============================================================================
//  SoftKB_Open												ref:  libsoftkb.h
//
//  ソフトウェアキーボード オープン処理
//
//  skey_t *pKey      : キー構造体へのポインタ
//
//  戻り値 : void
void SoftKB_Open(skey_t *pKey)
{
	pKey->export.initKeyTop(pKey);		// キートップ変更

	pKey->iPhase = SKBPHASE_OPENING;	// フェーズ設定

	pKey->export.open(pKey);
	pKey->export.initSpecialKey(pKey, 0);

	g_pSkbImport->dprintf("MSG : SoftKB_Open\n");
}


//============================================================================
//  SoftKB_Close											ref:  libsoftkb.h
//
//  ソフトウェアキーボード クローズ処理
//
//  skey_t *pKey      : キー構造体へのポインタ
//
//  戻り値 : void
void SoftKB_Close(skey_t *pKey)
{
	pKey->iPhase = SKBPHASE_CLOSING;		// フェーズ設定

	pKey->export.close(pKey);

	g_pSkbImport->dprintf("MSG : SoftKB_Close\n");
}


//============================================================================
//  SoftKB_Run												ref:  libsoftkb.h
//
//  ソフトウェアキーボード 毎フレーム処理
//
//  sceVif1Packet *pk : パケットバッファへのポインタ
//  skey_t *pKey      : ソフトウェアキーボード構造体へのポインタ
//
//  戻り値 : void
void SoftKB_Run(sceVif1Packet *pk, skey_t *pKey)
{
	SKB_ASSERT(pk);
	SKB_ASSERT(pKey);

	if(!g_pSkbImport->skb.ucShowSKbd && !g_pSkbImport->skb.ucShowHelp)
	{
		// ソフトウェアキーボードが非表示 かつ ヘルプが表示されていない時
		return;
	}

	switch(pKey->iPhase)
	{
	case SKBPHASE_OPENING:
		openingAnim(pKey);
		break;

	case SKBPHASE_CLOSING:
		closingAnim(pKey);
		break;

	case SKBPHASE_CLOSED:
		break;

	case SKBPHASE_OPENED:
	default:
		// 通常処理
		if(g_pSkbImport->skb.ucShowHelp)
		{
			// ヘルプ描画
			drawHelp(pk, pKey, &g_libskb.tex[TEX_HELPCHARS], &g_libskb.tex[TEX_HELPITEMS]);
		}
		else
		{
			// パケット作成
			pKey->export.makePacketsSkbBody(pk, pKey, &g_libskb.tex[TEX_BTN], &g_libskb.tex[TEX_KEYTOP], &g_libskb.tex[TEX_BG]);
			pKey->export.makePacketsCursor (pk, pKey, &g_libskb.tex[TEX_BTN]);

			makePackets_modebars(pk, pKey, &g_libskb.tex[TEX_BTN], &g_libskb.tex[TEX_KEYTOP]);
		}
		break;
	}
}


//============================================================================
//  SoftKB_SetPadData										ref:  libsoftkb.h
//
//  パッド情報設定
//
//  u_long paddata : パッド情報
//
//  戻り値 : void
void SoftKB_SetPadData(skey_t *pKey, u_long paddata)
{
	u_short down, press, repeat, up;
	SKB_ASSERT(pKey);

	// オープン中、クローズ中、クローズ後
	if(pKey->iPhase == SKBPHASE_OPENING || pKey->iPhase == SKBPHASE_CLOSING || pKey->iPhase == SKBPHASE_CLOSED)
	{
		return;
	}

	// オープン後
	up     = (paddata>>48)	& 0xFFFFU;
	down   = (paddata>>32)	& 0xFFFFU;
	press  = (paddata>>16)	& 0xFFFFU;
	repeat = (paddata)		& 0xFFFFU;

	if(!g_pSkbImport->skb.ucShowSKbd && !g_pSkbImport->skb.ucShowHelp)
	{
		// ソフトウェアキーボードが非表示 かつ
		// ヘルプが表示されている場合は、PAD情報を渡さない
		return;
	}


	// △×□○ボタンが押されたら、方向キーは無視。
	if( press & ( SCE_PADRup | SCE_PADRdown | SCE_PADRleft | SCE_PADRright ) ){
		press &=  ~( SCE_PADLup | SCE_PADLdown | SCE_PADLleft | SCE_PADLright );
		repeat&=  ~( SCE_PADLup | SCE_PADLdown | SCE_PADLleft | SCE_PADLright );
		down  &=  ~( SCE_PADLup | SCE_PADLdown | SCE_PADLleft | SCE_PADLright );
	}


	if(g_pSkbImport->skb.ucShowHelp)
	{
		// ヘルプ表示中
		if(down & SCE_PADRdown)
		{
			// ヘルプ解除
			g_pSkbImport->skb.ucShowHelp = 0;
			g_pSkbImport->skb.showHelp(0);

			pKey->export.initSpecialKey(pKey, 1);
			pKey->uiPush = 0;
			#if 1//SKB_EX Sound
			SoftKB_se_play(SKBSE_CANCEL);
			#endif
		}
	}
	else
	{
		// パッド処理
		pKey->export.setPad(pKey, down, up, press, repeat);
	}
}


//============================================================================
//  SoftKB_SetMouseData										ref:  libsoftkb.h
//
//  マウス情報設定
//
//  u_long128 data : マウス情報
//
//  戻り値 : void
void SoftKB_SetMouseData(skey_t *pKey, u_long128 *pData)
{
	static int lbtnState = SKBMODE_NOHIT;
	static int dragState = 0;
	int hitNo = SKBMODE_NOHIT;
	int _x, _y, _w, _btn;
	char *cPtr = NULL;
	SKB_ASSERT(pKey);
	SKB_ASSERT(pData);

	if(!g_protectSkb.ucUseMouse)
	{
		// マウスは使わない
		return;
	}

	// オープン中、クローズ中、クローズ後
	if(pKey->iPhase == SKBPHASE_OPENING || pKey->iPhase == SKBPHASE_CLOSING || pKey->iPhase == SKBPHASE_CLOSED)
	{
		return;
	}
	if(!g_pSkbImport->skb.ucShowSKbd && !g_pSkbImport->skb.ucShowHelp)
	{
		// ソフトウェアキーボードが非表示 かつ ヘルプが表示されていない時
		return;
	}

	cPtr = (u_char *)pData;
	if(cPtr[0] == 0)
	{
		return;		// マウス情報の更新がない場合は、抜ける
	}

	_btn = cPtr[1];		// ボタン
	_x   = cPtr[2];		// X差分
	_y   = cPtr[3];		// Y差分
	_w   = cPtr[4];		// ホイール

	// マウスの移動範囲を制限
	g_libskb.mousePos.x += _x;
	g_libskb.mousePos.y += _y;
	if(g_libskb.mousePos.x < 0)
	{
		g_libskb.mousePos.x = 0;
	}
	if(g_libskb.mousePos.y < 0)
	{
		g_libskb.mousePos.y = 0;
	}
	if(g_libskb.mousePos.x > g_pSkbImport->uiScreenW)
	{
		g_libskb.mousePos.x = g_pSkbImport->uiScreenW;
	}
	if(g_libskb.mousePos.y > g_pSkbImport->uiScreenH)
	{
		g_libskb.mousePos.y = g_pSkbImport->uiScreenH;
	}

	if(g_pSkbImport->skb.ucShowHelp)
	{
		if(SKBMOUSE_LBTNDOWN & _btn)
		{
			// ヘルプ解除
			g_pSkbImport->skb.ucShowHelp = 0;
			g_pSkbImport->skb.showHelp(0);

			pKey->export.initSpecialKey(pKey, 1);
			return;
		}
	}

	// CAUTION : マウス情報は、更新された時のみ送信される
	// クリック ////////////
	// 左ボタン
	if(SKBMOUSE_LBTNDOWN & _btn)
	{
		// 押下
		hitNo = searchModebarByPoint(pKey, &g_libskb.mousePos);
		if(hitNo == SKBMODE_NOHIT)
		{
			// キー
			pKey->export.mouseLButtonDown(pKey, &g_libskb.mousePos);
		}
		else
		{
			// モードバー
			if(!dragState)
			{
				// 押下した
				lbtnState = hitNo;		// モードバーが押された
				dragState = 0;			// ドラッグフラグON
			}
			else
			{
				// ドラッグ
				if(lbtnState != hitNo)
				{
					lbtnState = SKBMODE_NOHIT;			// モードバーが押された
				}
			}
		}
		g_libskb.uiBtn |= SKBMOUSE_LBTNDOWN;
	}
	else if(!(SKBMOUSE_LBTNDOWN & _btn) && (SKBMOUSE_LBTNDOWN & g_libskb.uiBtn))
	{
		// 解除
		hitNo = searchModebarByPoint(pKey, &g_libskb.mousePos);
		if(hitNo == SKBMODE_NOHIT)
		{
			// 文字設定
			pKey->export.mouseLButtonUp(pKey, &g_libskb.mousePos);
		}
		else
		{
			// モードバー
			if(lbtnState == hitNo)
			{
				g_pSkbImport->skb.keytopChange(hitNo);	// キートップ変更用コールバック呼び出し
			}
			dragState = 0;								// ドラッグフラグOFF
		}
		g_libskb.uiBtn &= ~SKBMOUSE_LBTNDOWN;
	}

#if 0
	// 右ボタン
	if(SKBMOUSE_RBTNDOWN & _btn)
	{
		// 押下
	}
	else if(!(SKBMOUSE_LBTNDOWN & _btn) && (SKBMOUSE_LBTNDOWN & g_libskb.uiBtn))
	{
		// 解除
	}

	// ホイール
	if(SKBMOUSE_WBTNDOWN & _btn)
	{
		// 押下
	}
	else if(!(SKBMOUSE_WBTNDOWN & _btn) && (SKBMOUSE_WBTNDOWN & g_libskb.uiBtn))
	{
		// 解除
	}
#endif

	pKey->export.mouseMove(pKey, &g_libskb.mousePos);	// マウス移動
}


//============================================================================
//  SoftKB_Move												ref:  libsoftkb.h
//
//  ソフトウェアキーボード移動
//
//  skey_t *pKey : キーボード構造体へのポインタ
//  int x        : 目的位置X
//  int y        : 目的位置Y
//
//  戻り値 : void
void SoftKB_Move(skey_t *pKey, int x, int y)
{
	pKey->export.trans(pKey, x, y);	// 移動
}


//============================================================================
//  SoftKB_MoveModebar										ref:  libsoftkb.h
//
//  モードバー移動
//
//  int x        : 目的位置X
//  int y        : 目的位置Y
//
//  戻り値 : void
void SoftKB_MoveModebar(int x, int y)
{
	g_libskb.modebarPos16[0] = g_pSkbImport->thinkXW16(x);
	g_libskb.modebarPos16[1] = g_pSkbImport->thinkYH16(y);

	makeModebars();		// モードバー作成
}


//============================================================================
//  SoftKB_ChangeAlpha										ref:  libsoftkb.h
//
//  アルファ情報変更
//
//  skey_t *pKey : キーボード構造体へのポインタ
//  int index    : アルファ値配列のインデックス(SKBALPHA_BG etc)
//  u_char alpha : アルファ値
//
//  戻り値 : void
void SoftKB_ChangeAlpha(skey_t *pKey, int index, u_char alpha)
{
	if(index < 0 || index > SKBALPHA_MAX)
	{
		// 予期されないインデックス
		g_pSkbImport->dprintf("ERROR : SoftKB_ChangeAlpha - non support alpha(%d)\n", index);
		return;
	}

	pKey->export.changeAlpha(pKey, index, alpha);	// アルファ値変更
}


//============================================================================
//  SoftKB_GetInputMode										ref:  libsoftkb.h
//
//  入力モード取得
//
//  skey_t *pKey : キーボード構造体へのポインタ
//
//  戻り値 : u_char : 入力モード
u_char SoftKB_GetInputMode(skey_t *pKey)
{
//	pKey->ucShift
	u_char flag = 0;

	if(pKey->ucShift & LAYOUT_ZENKAKU)
	{
		flag = SKBMODE_ZENKAKU;
	}

	return (pKey->ucInputMode | flag);
}


//============================================================================
//  SoftKB_GetSize											ref:  libsoftkb.h
//
//  入力モード取得
//
//  skey_t *pKey : キーボード構造体へのポインタ
//  int *w : 幅
//  int *h : 高さ
//
//  戻り値 : void
void SoftKB_GetSize(skey_t *pKey, int *w, int *h)
{
	*w = pKey->pSkbInfo->w;
	*h = pKey->pSkbInfo->h;
}


//============================================================================
//  SoftKB_SearchFocus										ref:  libsoftkb.h
//
//  ソフトウェアキーボードのフォーカスを取得
//
//	skey_t *pKeyNow  : 現在選択されているソフトウェアキーボード構造体へのポインタ
//	skey_t *pKeyNext : 次回選択されるソフトウェアキーボード構造体へのポインタ
u_int SoftKB_SearchFocus(skey_t *pKeyNow, skey_t *pKeyNext)
{
	// 現在のフォーカス位置(x,y,w,h)を取得
	sprite_t *pSp;
	int cur   = pKeyNow->uiCursorPos;
	int index = pKeyNow->pBtns[cur].spBaseIndex;
	pSp = &pKeyNow->pSps[index];

	// 次に選択したキートップ上で(x,y,w,h)に最も近い場所を探してリターン
	return pKeyNext->export.searchFocus(pKeyNext, pSp);
}


//============================================================================
//  SoftKB_DrawFocus										ref:  libsoftkb.h
//
//  フォーカス設定＆描画
//  ソフトウェアキーボード以外で
//  ソフトウェアキーボードと同じフォーカス使いたい場合
//
//  sceVif1Packet *pk : VIFパケットバッファへのポインタ
//  sprite_t *sp : フォーカスをあてたい矩形
//  sprite_t *focus : フォーカスのスプライト4個分
//  u_int curPos    : カーソルのフワフワ度
//  u_int col    : 色
//
void SoftKB_DrawFocus(sceVif1Packet *pk, sprite_t *sp, u_int curPos, u_int col)
{
	int i;
	sprite_t aSpFocus[4];
	makeCursorSprite(sp, aSpFocus, curPos);

	// 描画
	g_pSkbImport->loadTex(pk, &g_libskb.tex[TEX_BTN]);
	for(i=0; i<4; i++)
	{
		g_pSkbImport->drawTex(pk, col, &aSpFocus[i], (uv_t*)&g_uvCursors[i], &g_libskb.tex[TEX_BTN]);
	}
}


//============================================================================
//  SoftKB_GetMouseBtn										ref:  libsoftkb.h
//
//  ボタン情報取得
//
//  戻り値 : const u_int : ボタン情報
const u_int SoftKB_GetMouseBtn(void)
{
	return g_libskb.uiBtn;
}


//============================================================================
//  SoftKB_GetMousePoint									ref:  libsoftkb.h
//
//  ポインタ情報取得
//
//  戻り値 : const iPoint_t* : 位置情報へのポインタ
const iPoint_t* SoftKB_GetMousePoint(void)
{
	return &g_libskb.mousePos;
}


//============================================================================
//  SoftKB_GetMouseWheel									ref:  libsoftkb.h
//
//  ホイール情報取得
//
//  戻り値 : const u_int : ホイール情報へのポインタ
const u_int SoftKB_GetMouseWheel(void)
{
	return g_libskb.uiWheel;
}


//============================================================================
//  SoftKB_SetStartCurPos									ref:  libsoftkb.h
//
//  ソフトウェアキーボード出現時の
//  キートップの種類に合わせた初期カーソル位置を設定
//
//  戻り値 : const u_int : ホイール情報へのポインタ
void SoftKB_SetStartCurPos(skey_t *pKey, u_int keyTopType)
{
	switch(keyTopType)
	{
	case KEYTOP_HIRAGANA:		// 50音配列ひらがな
	case KEYTOP_KATAKANA:		// 50音配列カタカナ
		pKey->uiCursorPos = 21;
		break;
	case KEYTOP_ABC:			// ABC配列
		pKey->uiCursorPos = 12;
		break;
	case KEYTOP_SIGN:			// 記号(特殊ボタンだけ)
	case KEYTOP_SIGNF:			// 記号(通貨etc)
	case KEYTOP_IDIOMCHAR:		// 発音記号付き文字
		pKey->uiCursorPos = 0;
		break;
	case KEYTOP_NUMJP:			// 数字(日本版：全角半角ボタンあり)
	case KEYTOP_NUMF:			// 数字(海外版：全角半角ボタン無し)
		pKey->uiCursorPos = 7;
		break;
	case KEYTOP_PC_JAPAN:		// 日本
		pKey->uiCursorPos = 30;
	case KEYTOP_PC_US:			// アメリカ
		pKey->uiCursorPos = 30;
		break;
	case KEYTOP_PC_FRANCE:		// フランス
		pKey->uiCursorPos = 16;
		break;
	case KEYTOP_PC_SPAIN:		// スペイン
	case KEYTOP_PC_GERMAN:		// ドイツ
	case KEYTOP_PC_DUTCH:		// オランダ
	case KEYTOP_PC_ITALIA:		// イタリア
	case KEYTOP_PC_PORTUGRESS:	// ポルトガル
	case KEYTOP_PC_UK:			// イギリス
	case KEYTOP_PC_CANADAF:		// カナディアンフレンチ
		pKey->uiCursorPos = 29;
		break;

	default:
		g_pSkbImport->dprintf("ERROR : SoftKB_SetStartCurPos - Unknown keytopType\n");
	};
}


//============================================================================
// STATIC FUNCTIONS
//============================================================================
//============================================================================
//	openingAnim
//
//	アニメーション開始用コールバック
//
//	skey_t *pKey : ソフトウェアキーボード構造体へのポインタ
static void openingAnim(skey_t *pKey)
{
	if(g_pSkbImport->skb.opening())
	{
		pKey->iPhase = SKBPHASE_OPENED;
	}
}


//============================================================================
//	closingAnim
//
//	アニメーション終了
//
//	skey_t *pKey : ソフトウェアキーボード構造体へのポインタ
static void closingAnim(skey_t *pKey)
{
	if(g_pSkbImport->skb.closing())
	{
		pKey->iPhase = SKBPHASE_CLOSED;
	}
}


//============================================================================
//	makeModebars
//
//	モードバー作成
static void makeModebars(void)
{
	int i=0;
	int uvindex = 0;
	int keyNum = g_libskb.uiModebarItemNum;
	int mrw, mrh;
	int base = 0;

	if(keyNum == 0)
	{
		return;
	}

	memset(g_libskb.modebarBgSp,   0x0, sizeof(sprite_t)*KEYTOP_NUM_MAX);	// モードバースプライト背景初期化
	memset(g_libskb.modebarCharSp, 0x0, sizeof(sprite_t)*KEYTOP_NUM_MAX);	// モードバースプライト文字初期化

	// 左側のモードを描画
	if(keyNum == 1)
	{
		// モードが一個しかない場合
		uvindex = MODEBAR_1;

		g_libskb.modebarBgSp[0].x = g_libskb.modebarPos16[0] + (i*MODEBAR_BASE_SP_W16);
		g_libskb.modebarBgSp[0].y = g_libskb.modebarPos16[1];
		g_libskb.modebarBgSp[0].z = 0;
		g_libskb.modebarBgSp[0].w = MODEBAR_BASE_SP_W16;
		g_libskb.modebarBgSp[0].h = g_pSkbImport->thinkYHn(MODEBAR_BASE_SP_H16);
	}
	else
	{
		for(i=0; i<keyNum; i++)
		{
			// spriteを作成
			g_libskb.modebarBgSp[i].x = g_libskb.modebarPos16[0] + (i*MODEBAR_BASE_SP_W16);
			g_libskb.modebarBgSp[i].y = g_libskb.modebarPos16[1];
			g_libskb.modebarBgSp[i].z = 0;
			g_libskb.modebarBgSp[i].w = MODEBAR_BASE_SP_W16;
			g_libskb.modebarBgSp[i].h = g_pSkbImport->thinkYHn(MODEBAR_BASE_SP_H16);
		}
	}

	// 文字のみ描画 ------
	mrw = (MODEBAR_BASE_SP_W16 - MODEBAR_CHAR_SP_W16)/2;
	mrh = g_pSkbImport->thinkYHn(MODEBAR_BASE_SP_H16 - MODEBAR_CHAR_SP_H16) / 2;
	for(i=0; i<keyNum; i++)
	{
		if((g_libskb.auiModebarType[i] == MODEBAR_CHAR_ALPHABET_J ||
			g_libskb.auiModebarType[i] == MODEBAR_CHAR_ALPHABET_F ||
			g_libskb.auiModebarType[i] == MODEBAR_CHAR_NUM))
		{
			base = g_pSkbImport->thinkYH16(MODEBAR_CHAR_SP_DOWN);	// ベースラインを合わせる
		}
		// spriteを作成
		g_libskb.modebarCharSp[i].x = g_libskb.modebarPos16[0] + (i*MODEBAR_BASE_SP_W16) + mrw;
		g_libskb.modebarCharSp[i].y = g_libskb.modebarPos16[1] + mrh + base;
		g_libskb.modebarCharSp[i].z = 0;
		g_libskb.modebarCharSp[i].w = MODEBAR_CHAR_SP_W16;
		g_libskb.modebarCharSp[i].h = g_pSkbImport->thinkYHn(MODEBAR_CHAR_SP_H16);
		base = 0;
	}
}


//============================================================================
//	makePackets_modebars
//
//	描画用モードバーパケット作成
//
//	sceVif1Packet *pk : VIFパケットバッファへのポインタ
//	skey_t *pKey : ソフトウェアキーボード構造体へのポインタ
//	textureInfo_t *pTexBtn  : ボタンテクスチャのポインタ
//	textureInfo_t *pTexChar : 文字テクスチャのポインタ
static void makePackets_modebars(sceVif1Packet *pk, skey_t *pKey, textureInfo_t *pTexBtn, textureInfo_t *pTexChar)
{
	// モード描画
	int i=0;
	int uvindex = 0;
	int keyNum = g_libskb.uiModebarItemNum;
	u_int col = DEFAULT_RGBA;
	int mrw, mrh;
	int inputMode = g_pSkbImport->skb.getNowInputMode();

	if(keyNum == 0)
	{
		return;
	}


	// 土台のみ描画
	g_pSkbImport->loadTex(pk, pTexBtn);
	// 左側のモードを描画
	if(keyNum == 1)
	{
		// モードが一個しかない場合
		uvindex = MODEBAR_1;

		col = affectAlpha(DEFAULT_RGBA, pKey->ucAlpha[SKBALPHA_BG]);
		g_pSkbImport->drawTex(pk, col, &g_libskb.modebarBgSp[0], (uv_t*)&g_uvModebarCursor[uvindex], pTexBtn);
	}
	else
	{
		for(i=0; i<keyNum; i++)
		{
			if(i == 0)
			{
				uvindex = MODEBAR_LEFT;			// 左
			}
			else if(i == (keyNum-1))
			{
				uvindex = MODEBAR_RIGHT;		// 右
			}
			else
			{
				uvindex = MODEBAR_CENTER;		// 間
			}

			col = affectAlpha(DEFAULT_RGBA, pKey->ucAlpha[SKBALPHA_BG]);
			if(inputMode == i)
			{
				// 現在使えるキータイプ
				g_pSkbImport->drawTex(pk, col, &g_libskb.modebarBgSp[i], (uv_t*)&g_uvModebarCursor[uvindex], pTexBtn);
			}
			else
			{
				g_pSkbImport->drawTex(pk, col, &g_libskb.modebarBgSp[i], (uv_t*)&g_uvModebarBase[uvindex], pTexBtn);
			}
		}
	}

	// 文字のみ描画 ------
	mrw = (MODEBAR_BASE_SP_W16 - MODEBAR_CHAR_SP_W16)/2;
	mrh = g_pSkbImport->thinkYHn(MODEBAR_BASE_SP_H16 - MODEBAR_CHAR_SP_H16) / 2;
	g_pSkbImport->loadTex(pk, pTexChar);
	for(i=0; i<keyNum; i++)
	{
		col = DEFAULT_RGBA;
		if(inputMode == i)
		{
			col = SKBDEF_ATTENTION_COL | SKBDEF_ATTENTION_ALPHA(pKey->ucAlpha[SKBALPHA_CHAR]);
		}
		uvindex = g_libskb.auiModebarType[i];
		g_pSkbImport->drawTex(pk, col, &g_libskb.modebarCharSp[i], (uv_t*)&g_uvModebarChar[uvindex], pTexChar);
	}
}


//============================================================================
//	initHelp
//
//	ヘルプ初期化
static void initHelp(void)
{
	int i;
	int atok = 0;
	u_int lang = g_pSkbImport->usLang;
	u_int region = g_pSkbImport->iRegion;
	helpMaps_t *pMap = &g_HelpMapsNTSC[lang];
	sprite_t *pSp = NULL;

	if(region == REGION_J)
	{
		atok = 0;
		if(lang == SCE_ENGLISH_LANGUAGE)
		{
			lang = HELP_J_ENGLISH;
			atok = 1;
		}
	}
	else
	{
		atok = -1;
		#if 1//SKB_EX LANG
		switch( lang ){
		default:
		case SCE_JAPANESE_LANGUAGE  :
		case SCE_ENGLISH_LANGUAGE   :	lang = HELP_ENGLISH;    break;
		case SCE_FRENCH_LANGUAGE    :	lang = HELP_FRENCH;     break;
		case SCE_SPANISH_LANGUAGE   :	lang = HELP_SPANISH;    break;
		case SCE_GERMAN_LANGUAGE    :	lang = HELP_GERMAN;     break;
		case SCE_ITALIAN_LANGUAGE   :	lang = HELP_ITALIAN;    break;
		case SCE_DUTCH_LANGUAGE     :	lang = HELP_DUTCH;      break;
		case SCE_PORTUGUESE_LANGUAGE:	lang = HELP_PORTUGUESE; break;
		}
		#endif
	}

	pMap = &g_HelpMapsNTSC[lang];

	// スプライト情報をコピー
	// PADアイテム
	pSp = pMap->spItems;
	for(i=0; i<pMap->itemNum; i++)
	{
		g_libskb.SpHelpItems[i].x = g_pSkbImport->thinkXW16(pSp[i].x);
		g_libskb.SpHelpItems[i].y = g_pSkbImport->thinkYH16(pSp[i].y);
		g_libskb.SpHelpItems[i].z = g_pSkbImport->thinkXW16(pSp[i].z);
		g_libskb.SpHelpItems[i].w = g_pSkbImport->thinkXW16(pSp[i].w);
		g_libskb.SpHelpItems[i].h = g_pSkbImport->thinkYH16(pSp[i].h);
	}

	// ヘルプ文字列
	pSp = pMap->spChars;
	for(i=0; i<pMap->charNum; i++)
	{
		g_libskb.SpHelpChars[i].x = g_pSkbImport->thinkXW16(pSp[i].x);
		g_libskb.SpHelpChars[i].y = g_pSkbImport->thinkYH16(pSp[i].y);
		g_libskb.SpHelpChars[i].z = g_pSkbImport->thinkXW16(pSp[i].z);
		g_libskb.SpHelpChars[i].w = g_pSkbImport->thinkXW16(pSp[i].w);
		g_libskb.SpHelpChars[i].h = g_pSkbImport->thinkYH16(pSp[i].h);
	}

	// "基本操作画面”
	g_libskb.SpHelpTitle.x = g_pSkbImport->thinkXW16(pMap->spTitle->x);
	g_libskb.SpHelpTitle.y = g_pSkbImport->thinkYH16(pMap->spTitle->y);
	g_libskb.SpHelpTitle.z = g_pSkbImport->thinkXW16(pMap->spTitle->z);
	g_libskb.SpHelpTitle.w = g_pSkbImport->thinkXW16(pMap->spTitle->w);
	g_libskb.SpHelpTitle.h = g_pSkbImport->thinkYH16(pMap->spTitle->h);

	#ifndef ATOK_NO_LINK
	if(atok != -1)
	{
		// ATOKバージョン
		pSp = &g_spAtokVerSrc[atok];
		g_libskb.SpAtokVer.x = g_pSkbImport->thinkXW16(pSp->x);
		g_libskb.SpAtokVer.y = g_pSkbImport->thinkYH16(pSp->y);
		g_libskb.SpAtokVer.z = g_pSkbImport->thinkXW16(pSp->z);
		g_libskb.SpAtokVer.w = g_pSkbImport->thinkXW16(pSp->w);
		g_libskb.SpAtokVer.h = g_pSkbImport->thinkYH16(pSp->h);

		// ATOKロゴ
		pSp = &g_spAtokLogoSrc[atok];
		g_libskb.SpAtokLogo.x = g_pSkbImport->thinkXW16(pSp->x);
		g_libskb.SpAtokLogo.y = g_pSkbImport->thinkYH16(pSp->y);
		g_libskb.SpAtokLogo.z = g_pSkbImport->thinkXW16(pSp->z);
		g_libskb.SpAtokLogo.w = g_pSkbImport->thinkXW16(pSp->w);
		g_libskb.SpAtokLogo.h = g_pSkbImport->thinkYH16(pSp->h);
	}
	#endif

	for(i=0; i<9; i++)
	{
		g_libskb.rcFrame[i] = pMap->rcFrames[i];		// DUM

		setFrame(&g_libskb.SpFrame[i][0], &pMap->rcFrames[i],0,0);
	}
}


#if 1//SKB_EX HelpOffset
//============================================================================
//	HelpOffset
//
//	ヘルプ初期化 initHelpオフセット指定版
static int helpOffsetX,helpOffsetY;
void HelpOffset( int x, int y )
{
	int i;
	int atok = 0;
	u_int lang = g_pSkbImport->usLang;
	u_int region = g_pSkbImport->iRegion;
	helpMaps_t *pMap = &g_HelpMapsNTSC[lang];
	sprite_t *pSp = NULL;

	if(region == REGION_J)
	{
		atok = 0;
		if(lang == SCE_ENGLISH_LANGUAGE)
		{
			lang = HELP_J_ENGLISH;
			atok = 1;
		}
	}
	else
	{
		atok = -1;
		#if 1//SKB_EX LANG
		switch( lang ){
		default:
		case SCE_JAPANESE_LANGUAGE  :
		case SCE_ENGLISH_LANGUAGE   :	lang = HELP_ENGLISH;    break;
		case SCE_FRENCH_LANGUAGE    :	lang = HELP_FRENCH;     break;
		case SCE_SPANISH_LANGUAGE   :	lang = HELP_SPANISH;    break;
		case SCE_GERMAN_LANGUAGE    :	lang = HELP_GERMAN;     break;
		case SCE_ITALIAN_LANGUAGE   :	lang = HELP_ITALIAN;    break;
		case SCE_DUTCH_LANGUAGE     :	lang = HELP_DUTCH;      break;
		case SCE_PORTUGUESE_LANGUAGE:	lang = HELP_PORTUGUESE; break;
		}
		#endif
	}

	helpOffsetX=g_pSkbImport->thinkXW16(x);
	helpOffsetY=g_pSkbImport->thinkYH16(y);

	pMap = &g_HelpMapsNTSC[lang];

	// スプライト情報をコピー
	// PADアイテム
	pSp = pMap->spItems;
	for(i=0; i<pMap->itemNum; i++)
	{
		g_libskb.SpHelpItems[i].x = g_pSkbImport->thinkXW16(pSp[i].x+x);
		g_libskb.SpHelpItems[i].y = g_pSkbImport->thinkYH16(pSp[i].y+y);
		g_libskb.SpHelpItems[i].z = g_pSkbImport->thinkXW16(pSp[i].z);
		g_libskb.SpHelpItems[i].w = g_pSkbImport->thinkXW16(pSp[i].w);
		g_libskb.SpHelpItems[i].h = g_pSkbImport->thinkYH16(pSp[i].h);
	}

	// ヘルプ文字列
	pSp = pMap->spChars;
	for(i=0; i<pMap->charNum; i++)
	{
		g_libskb.SpHelpChars[i].x = g_pSkbImport->thinkXW16(pSp[i].x+x);
		g_libskb.SpHelpChars[i].y = g_pSkbImport->thinkYH16(pSp[i].y+y);
		g_libskb.SpHelpChars[i].z = g_pSkbImport->thinkXW16(pSp[i].z);
		g_libskb.SpHelpChars[i].w = g_pSkbImport->thinkXW16(pSp[i].w);
		g_libskb.SpHelpChars[i].h = g_pSkbImport->thinkYH16(pSp[i].h);
	}

	// 基本操作画面
	g_libskb.SpHelpTitle.x = g_pSkbImport->thinkXW16(pMap->spTitle->x+x);
	g_libskb.SpHelpTitle.y = g_pSkbImport->thinkYH16(pMap->spTitle->y+y);
	g_libskb.SpHelpTitle.z = g_pSkbImport->thinkXW16(pMap->spTitle->z);
	g_libskb.SpHelpTitle.w = g_pSkbImport->thinkXW16(pMap->spTitle->w);
	g_libskb.SpHelpTitle.h = g_pSkbImport->thinkYH16(pMap->spTitle->h);

	#ifndef ATOK_NO_LINK
	if(atok != -1)
	{
		// ATOKバージョン
		pSp = &g_spAtokVerSrc[atok];
		g_libskb.SpAtokVer.x = g_pSkbImport->thinkXW16(pSp->x+x);
		g_libskb.SpAtokVer.y = g_pSkbImport->thinkYH16(pSp->y+y);
		g_libskb.SpAtokVer.z = g_pSkbImport->thinkXW16(pSp->z);
		g_libskb.SpAtokVer.w = g_pSkbImport->thinkXW16(pSp->w);
		g_libskb.SpAtokVer.h = g_pSkbImport->thinkYH16(pSp->h);

		// ATOKロゴ
		pSp = &g_spAtokLogoSrc[atok];
		g_libskb.SpAtokLogo.x = g_pSkbImport->thinkXW16(pSp->x+x);
		g_libskb.SpAtokLogo.y = g_pSkbImport->thinkYH16(pSp->y+y);
		g_libskb.SpAtokLogo.z = g_pSkbImport->thinkXW16(pSp->z);
		g_libskb.SpAtokLogo.w = g_pSkbImport->thinkXW16(pSp->w);
		g_libskb.SpAtokLogo.h = g_pSkbImport->thinkYH16(pSp->h);
	}
	#endif

	for(i=0; i<9; i++)
	{
		g_libskb.rcFrame[i] = pMap->rcFrames[i];		// DUM

		setFrame(&g_libskb.SpFrame[i][0], &pMap->rcFrames[i],x,y);
	}
}
#endif


//============================================================================
//	setFrame
//
//	ヘルプ用フレーム作成
#if 1//SKB_EX HelpOffset
static void setFrame(sprite_t *sp0, iRect_t *pRc, int x, int y )
#else
static void setFrame(sprite_t *sp0, iRect_t *pRc)
#endif
{
	sprite_t *pSp;
	sprite_t *pSpPrev;
	int s, i;
	iRect_t rc;
	int spw = g_pSkbImport->thinkXW16(FRAME_SP_W);
	int sph = g_pSkbImport->thinkYH16(FRAME_SP_H);

	#if 1//SKB_EX HelpOffset
	rc.x = g_pSkbImport->thinkXW16(pRc->x+x);
	rc.y = g_pSkbImport->thinkYH16(pRc->y+y);
	#else
	rc.x = g_pSkbImport->thinkXW16(pRc->x);
	rc.y = g_pSkbImport->thinkYH16(pRc->y);
	#endif
	rc.w = g_pSkbImport->thinkXW16(pRc->w);
	rc.h = g_pSkbImport->thinkYH16(pRc->h);

	// 初期化
	for(i=0; i<3; i++)
	{
		for(s=0; s<3; s++)
		{
			pSp    = sp0+((3*i)+s);
			pSp->x = rc.x + (i*spw);
			pSp->y = rc.y + (s*sph);
			pSp->z = 0;
			pSp->w = spw;
			pSp->h = sph;
		}
	}

	pSp = sp0+1;
	pSp->h = rc.h;						// Pane[1]の高さ

	pSpPrev = (sp0+1);
	pSp = (sp0+2);
	pSp->y = rc.h + pSpPrev->y;			// Pane[2]のY値

	pSpPrev = (sp0+2);
	pSp = (sp0+3);
	pSp->w = rc.w;						// Pane[3]の幅

	pSpPrev = (sp0+3);
	pSp = (sp0+4);
	pSp->h = rc.h;						// Pane[4]の高さ
	pSp->w = rc.w;						// Pane[4]の幅

	pSpPrev = (sp0+4);
	pSp = (sp0+5);
	pSp->w = rc.w;						// Pane[5]の幅
	pSp->y = rc.h + pSpPrev->y;			// Pane[5]のY値

	pSpPrev = (sp0+5);
	pSp = (sp0+6);
	pSp->x = pSpPrev->x + pSpPrev->w;	// Pane[6]のX値

	pSpPrev = (sp0+6);
	pSp = (sp0+7);
	pSp->x = pSpPrev->x;				// Pane[7]のX値
	pSp->h = rc.h;						// Pane[7]の高さ

	pSpPrev = (sp0+7);
	pSp = (sp0+8);
	pSp->y = pSpPrev->h + pSpPrev->y;	// Pane[8]のY値
	pSp->x = pSpPrev->x;				// Pane[8]のX値
}


//============================================================================
//	searchModebarByPoint
//
//	モードバーのあたり判定
//
//	skey_t *pKey : ソフトウェアキーボード構造体へのポインタ
//	iPoint_t *pt : 位置情報へのポインタ
//
//	戻り値 : int : 当たっていた:モードのインデックス/外れていた:SKBMODE_NOHIT
static int searchModebarByPoint(skey_t *pKey, iPoint_t *pt)
{
	int i;
	int cx = g_pSkbImport->thinkXW16(pt->x);
	int cy = g_pSkbImport->thinkYH16(pt->y);
	int keyNum = g_libskb.uiModebarItemNum;
	sprite_t *pSp = NULL;
	int inputMode = g_pSkbImport->skb.getNowInputMode();

	// 背景であたり判定をする
	for(i=0; i<keyNum; i++)
	{
		pSp = &g_libskb.modebarBgSp[i];
		if((pSp->x <= cx) && (pSp->y <= cy) &&
		   ((pSp->x+pSp->w) > cx) && ((pSp->y+pSp->h) > cy))
		{
			return (i-inputMode);
		}
	}

	return SKBMODE_NOHIT;		// ありえない値を返す
}


//============================================================================
//	drawHelp
//
//	ヘルプ表示
//
//	sceVif1Packet *pk : VIFパケットバッファへのポインタ
//	skey_t *pKey : ソフトウェアキーボード構造体へのポインタ
//	textureInfo_t *pChars : ヘルプ文字テクスチャへのポインタ
//	textureInfo_t *pItems : コントローラテクスチャへのポインタ
static void drawHelp(sceVif1Packet *pk, skey_t *pKey, textureInfo_t *pChars, textureInfo_t *pItems)
{
	int i, j;
	u_int col = 0x80808080;
	u_int lang = g_pSkbImport->usLang;
	helpMaps_t *pMap = &g_HelpMapsNTSC[lang];
	int itemNum = 0;
	iRect_t		pane = {0, 0, 650<<4, (700<<4)};
	u_int region = g_pSkbImport->iRegion;

	SKB_ASSERT(pChars);
	SKB_ASSERT(pItems);

	#if 1//SKB_EX HelpOffset
	pane.x=helpOffsetX;
	pane.y=helpOffsetY;
	#endif
	// 画面を板で隠す
	g_pSkbImport->drawPane(pk, &pane, 0x80000000, 0);

	g_pSkbImport->loadTex(pk, pItems);		// テクスチャロード
	// 言語設定取得
	if(region == REGION_J)
	{
		int atok = 0;
		if(lang == SCE_ENGLISH_LANGUAGE)
		{
			lang = HELP_J_ENGLISH;
			atok = 1;
		}
		// ATOK情報を入れる
		g_pSkbImport->drawTex(pk, col, &g_libskb.SpAtokLogo, &g_uvAtokLogoSrc,		pItems);	// ATOK ロゴ描画
		g_pSkbImport->drawTex(pk, col, &g_libskb.SpAtokVer,  &g_uvAtokVerSrc[atok],	pItems);	// ATOK バージョン描画
	}
	#if 1//SKB_EX LANG
	else{
		switch( lang ){
		default:
		case SCE_JAPANESE_LANGUAGE  :
		case SCE_ENGLISH_LANGUAGE   :	lang = HELP_ENGLISH;    break;
		case SCE_FRENCH_LANGUAGE    :	lang = HELP_FRENCH;     break;
		case SCE_SPANISH_LANGUAGE   :	lang = HELP_SPANISH;    break;
		case SCE_GERMAN_LANGUAGE    :	lang = HELP_GERMAN;     break;
		case SCE_ITALIAN_LANGUAGE   :	lang = HELP_ITALIAN;    break;
		case SCE_DUTCH_LANGUAGE     :	lang = HELP_DUTCH;      break;
		case SCE_PORTUGUESE_LANGUAGE:	lang = HELP_PORTUGUESE; break;
		}
	}
	#endif


	pMap = &g_HelpMapsNTSC[lang];

	/* "基本操作画面"描画 */
	g_pSkbImport->drawTex(pk, 0x80808080, &g_libskb.SpHelpTitle, pMap->uvTitle, pItems);

	/* PADアイテム描画    */
	itemNum = pMap->itemNum;
	for(i=0; i<itemNum; i++)
	{
		g_pSkbImport->drawTex(pk, col, &g_libskb.SpHelpItems[i], &pMap->uvItems[i], pItems);
	}

	/* フレーム描画       */
	for(i=0; i<9; i++)
	{
		for(j=0; j<9; j++)
		{
			g_pSkbImport->drawTex(pk, DEFAULT_RGBA, &g_libskb.SpFrame[i][j], &pMap->uvFrames[j], pItems);
		}
	}

	/* 文字描画 */
	itemNum = pMap->charNum;
	g_pSkbImport->loadTex(pk, pChars);
	for(i=0; i<itemNum; i++)
	{
		g_pSkbImport->drawTex(pk, col, &g_libskb.SpHelpChars[i], &pMap->uvChars[i], pChars);
	}
}


//============================================================================
//	SoftKB_se_play
//
//	効果音コールバック関数呼び出し
//
#if 1//SKB_EX Sound
void SoftKB_se_play(int sound_No)
{
	if( g_pSkbImport->se ){
		g_pSkbImport->se(sound_No);
	}
}
#endif
