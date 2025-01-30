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
/*============================================================================

以下、注意事項

pFw->stat.FontRatio		: 大きさ 縦横の比率は常に一定
SetTvFontRatio			: FrameMode/FieldModeで変わる
							NTSC : 1.0f
							PAL  : 1.15f

maxHeight : データによって違う。

      BASE POINT X
          │
┌――――┼―――――――――――┬―───────────┬─
｜        │ R BEARING            ｜↑                      ↑
｜        │←――――→｜        ｜｜                      │
｜  L BEARING           ｜        ｜｜                      │
｜｜←―→│            ｜        ｜｜                      │
｜├―――┼――――┬―┼―      ｜｜                      │
｜｜      ｜        ****｜↑      ｜｜                      │
｜｜      ｜      **    * ｜      ｜｜                      │
｜｜      ｜     ** ｜  ｜｜      ｜｜                      │
｜｜      ｜**********  ｜ASCENT  ｜TEXTURE 1 文字 高さ     │ maxHeight
｜｜      ｜  ***   ｜  ｜｜      ｜｜                      │
｜｜      ｜ ***    ｜  ｜｜      ｜｜                      │
｜｜      ｜***     ｜  ｜↓      ｜｜                      │
｜｜＿＿＿ ***＿＿＿｜＿＿__＿＿＿｜＿＿ BASE POINT Y       │
｜｜      **        ｜  ｜↑      ｜｜                      │
｜ *    **｜        ｜  ｜DESCENT ｜｜                      │
｜｜****  ｜        ｜  ｜↓      ｜｜                      │
｜└―――┼――――┼―┴―      ｜｜                      │
｜        ｜←――→｜            ｜｜                      │
｜           WIDTH                ｜↓                      ↓
├――――――――――――――――┼┴───────────┴─
｜←―― TEXTURE 1 文字 幅  ――→｜
｜←―― maxWidth           ――→｜


============================================================================*/
#include <eekernel.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>

#include <sifdev.h>
#include <libpkt.h>
#include <libpfont.h>
#include <libgraph.h>

#include <assert.h>	
#include "fontwrap.h"

#define FIELD_MODE  0
#define PAL_MODE   0
/*---------------------------------------------------------*/
#if FIELD_MODE
	#define VRESOLUTION_ADJUST(_v_)		(_v_)
	#define VRESOLUTION_ADJUST_F(_v_)	(_v_)
#else
	#define VRESOLUTION_ADJUST(_v_)		((_v_)/2)
	#define VRESOLUTION_ADJUST_F(_v_)	((_v_)/2.0f)
#endif		// FIELD_MODE

#if PAL_MODE
	#define SCREEN_VHEIGHT	(512)
	#define SCREEN_HEIGHT	(VRESOLUTION_ADJUST(SCREEN_VHEIGHT))
#else
	#define SCREEN_VHEIGHT	(448)
	#define SCREEN_HEIGHT	(VRESOLUTION_ADJUST(SCREEN_VHEIGHT))
#endif

#define SCREEN_WIDTH	(640)
#define XOFFSET			(2048 - SCREEN_WIDTH / 2)
#define YOFFSET			(2048 - SCREEN_HEIGHT / 2)
#define FONT_GSMEMSIZE	(2048 * 1)
/*---------------------------------------------------------*/

#define FONT_DUMMY_CHAR		(0xd800+24)
#define DEF_FONT_RATIO		(1.0f)
#define DEF_FONT_PITCH		(0)
#define OFFSET_X			(24)
#define OFFSET_Y			(24)
#define FONT_SIZE_FX		(32.0f)
#define FONT_SIZE_FY		(40.0f)
#define FONTCTRL			('\a')

#define UNDERLINE_H		(1)
#define ULDOT_W			(4)

#define _SET_VECTOR(v, r, g, b, a)	(v[0] = r, v[1] = g, v[2] = b, v[3] = a)


typedef struct
{
	int	FontRetHeight;		// \n return 対応用（先頭 X 保存、Y 増分）
	int	FontPropSw;			// 強制字間指定
	int	FontRatioSw;		// 臨時サイズスイッチ
	int	FontPitch;			// ピッチ

	int	FontAdjustY;		// Y座標1/16ピクセル単位調整
	int	Window_x;			// ウインドウ x
	int Window_w;			// ウィンドウ w
	int effect;				// 背景色、下線などの文字効果
    int clip;				// 画面クリップ 横だけ

	float TvFontRatioY;		// アスペクト比設定用
	float FontRatio;		// 基準拡大率
	int pad0;

	sceVu0FVECTOR locate;	// 描画位置
	sceVu0FVECTOR color;	// 描画色

	sceVu0IVECTOR bgcolor[4];	// 背景色
	sceVu0IVECTOR ulcolor;		// 下線色

} fontStatus_t;




typedef struct
{
	u_long128 *fontData;
	int  fd;
	int calc;

	int MaxWidth;
	int MaxAscent;
	int MaxHeight;
	int transmodified;


	float left;				// 描画単位毎の左端座標保存
	float top;				// 描画単位毎の上端座標保存
	float right;
	float bottom;


	fontStatus_t	stat;

	sceVu0FMATRIX screen;	// ローカル→スクリーンマトリクス(単位行列)
	sceVu0FMATRIX trans;	// 変形マトリクス
	int bExit;
	u_long128 font_cache[64];	// 1k bytes


	u_int qwc;
	sceVif1Packet* pPacket;
	sceVif1Packet packet;

} fontWork_t;
static fontWork_t g_FontWork;
static fontWork_t *pFw = &g_FontWork;



static int  fontDrawBG(int left, int top, int right, int bottom);
static int  fontDrawUL(int left, int top, int right, int bottom);
static int  fontDrawULD(int left, int top, int right, int bottom);
static int  calcPacketSpace(void);
static void calcDrawArea(char const *str, int *width, int *height);
static void _memcpy128(u_long128* pDst, u_long128 const* pSrc, int qwc);
static int  gethex(int c);
static int  fontFilter(int fd, scePFontControl* pCtrl);
static int  fontFilterPutc(int fd, scePFontControl* pCtrl, int c);
static void updateTransMatrix(void);
static void updateScreenMatrix(void);


static union
{
	u_long d[2];
	u_long128 tag;
} const _gif_ad = {{ SCE_GIF_SET_TAG(0, 1, 0, 0, 0, 1), 0xeUL }};


//============================================================================
// Color
static int const FontCol[][3] =
{
	{  96,  96,  96 },		// 0 WHITE
	{ 110, 110,   0 },		// 1 YELLOW
	{  47,  87, 127 },		// 2 BLUE
	{ 190, 128, 150 },		// 3 PINK
	{  44,  44,  44 },		// 4 GRAY
	{  80, 140, 140 },		// 5 CYAN
	{  98,  56,  56 },		// 6 ORANGE

	// original
	{  60,  60,  60 },		// 7 LGRAY		// BROWSER COPY「↓」
	{  90,  90,  90 },		// 8 BWHITE		// not used
	{  20,  90,  60 },		// 9 GREEN		// not used
};




//============================================================================
// PUBLIC FUNCTIONS
//============================================================================
//============================================================================
//  Font_Init												ref : fontwrap.h
//
//  フォント初期化
//
//  戻り値 : void
void Font_Init(void)
{
	assert(pFw);

	memset(pFw, 0x0, sizeof(fontWork_t));		// フォントワーカ初期化
}


//============================================================================
//  Font_Load												ref : fontwrap.h
//
//  フォントロード
//
//  int gsclutadr   : CLUTアドレス
//  int gstexadr    : TEXTUREアドレス
//  u_long128 *pFontData : フォントデータへのポインタ
//
//  戻り値 int : 0:正常終了/ 1:エラー
int Font_Load(int gsclutadr, int gstexadr, u_long128 *pFontData)
{
	{
		int csize = scePFontCalcCacheSize(2) - scePFontCalcCacheSize(1);
		int fsize = scePFontCalcCacheSize(1) - csize;
		int num = (sizeof(pFw->font_cache) - fsize) / csize;
		pFw->fd = scePFontInit(num, pFw->font_cache);
		//assert(sizeof(SPR.font_cache) >= scePFontCalcCacheSize(num));
		scePFontSetTexMem(pFw->fd, gstexadr, FONT_GSMEMSIZE, gsclutadr);
	}

	pFw->fontData = pFontData;						// フォントデータ設定
	if(0 != scePFontAttachData(pFw->fd, pFw->fontData)){
		printf("scePFontAttachData(pFw->fd, pFw->fontData) failed\n");
		return 1;
	}
	scePFontSetFilter(pFw->fd, fontFilter);

	{
		// 位置設定初期化
		pFw->stat.locate[0] = 0.0f;
		pFw->stat.locate[1] = 0.0f;
		pFw->stat.locate[2] = 0.0f;
		pFw->stat.locate[3] = 0.0f;

		// フォントの高さをキャッシュ
		pFw->MaxWidth  = scePFontGetFontInfo(pFw->fd)->max_width;
		pFw->MaxAscent = scePFontGetFontInfo(pFw->fd)->max_ascent;
		pFw->MaxHeight = pFw->MaxAscent - scePFontGetFontInfo(pFw->fd)->max_descent;

		pFw->transmodified = 1;
		updateTransMatrix();
		updateScreenMatrix();
	}

	// デフォルト設定
	Font_SetLocate(0, 0);
	Font_SetPitch(DEF_FONT_PITCH);
	Font_SetRetHeight(0);
	Font_SetColor(128, 128, 128, 128);
	Font_SetProp(0);
	Font_SetRatio(DEF_FONT_RATIO);
	Font_SetTvFontRatio(1.0f);

	Font_SetULColor(255, 0, 0, 64);
	Font_SetBGColor(0, 0, 64,  64);
	Font_SetBGColor(0, 0, 64,  64);
	Font_SetBGColor(0, 0, 255, 64);
	Font_SetBGColor(0, 0, 255, 64);

	return 0;
}


//============================================================================
//  Font_Unload												ref : fontwrap.h
//
//  フォントを削除
void Font_Unload(void)
{
//	if(pFw->fontData != NULL)
//	{
//		free(pFw->fontData);
//	}

	scePFontRelease(pFw->fd);
}


//============================================================================
//  Font_SetLocate											ref : fontwrap.h
//
//  位置設定
//
//  int x : X座標(16倍しない値)
//  int y : Y座標(16倍しない値)
void Font_SetLocate(int x, int y)
{
	pFw->stat.locate[0] = (float)x;
	pFw->stat.locate[1] = (float)y;
}


//============================================================================
//  Font_SetLocate_f											ref : fontwrap.h
//
//  位置設定
//
//  float x : X座標(16倍しない値)
//  float y : Y座標(16倍しない値)
void Font_SetLocate_f( float x, float y )
{
	pFw->stat.locate[0] = (float)x;
	pFw->stat.locate[1] = (float)y;
}


//============================================================================
//  Font_SetPitch											ref : fontwrap.h
//
//  ピッチ設定
//
//  int pitch : ピッチ
void Font_SetPitch(int pitch)
{
	pFw->stat.FontPitch = (float)(pitch - DEF_FONT_PITCH);
	pFw->transmodified = 1;
}


//============================================================================
//  Font_SetRetHeight										ref : fontwrap.h
//
//  \n(return)時の y 増分設定
//
//  int y : y 増分
void Font_SetRetHeight(int y)
{
	pFw->stat.FontRetHeight = y;
}


//============================================================================
//  Font_SetProp											ref : fontwrap.h
//
//  プロポーション設定
//
//  int sw : プロポーション値
void Font_SetProp(int sw)
{
	pFw->stat.FontPropSw = sw;
}


//============================================================================
//  Font_SetRatio											ref : fontwrap.h
//
//  大きさ設定
//
//  float flRatio : 文字大きさ
void Font_SetRatio(float flRatio)
{
	pFw->stat.FontRatio = flRatio;
	pFw->transmodified = 1;
}


//============================================================================
//  Font_SetRatioPixel										ref : fontwrap.h
//
//  大きさ設定
//
//  int size : 文字大きさ(ピクセルで設定)
void Font_SetRatioPixel(int size)
{
	float fsize = (float)size/FONT_SIZE_FY;

	Font_SetRatio(fsize);
}


//============================================================================
//  Font_SetTvFontRatio										ref : fontwrap.h
//
//  NTSC/PALアスペクト設定
//
//  float flRatio : アスペクト比
void Font_SetTvFontRatio(float flRatio)
{
	pFw->stat.TvFontRatioY = flRatio;
	pFw->transmodified = 1;
}


//============================================================================
//  Font_SetColor											ref : fontwrap.h
//
//  文字色設定
//
//  int r : 赤
//  int g : 緑
//  int b : 青
//  int a : アルファ値
void Font_SetColor(int r, int g, int b, int a)
{
	pFw->stat.color[0] = (float)r / 128.0f;
	pFw->stat.color[1] = (float)g / 128.0f;
	pFw->stat.color[2] = (float)b / 128.0f;
	pFw->stat.color[3] = (float)a / 128.0f;
}


//============================================================================
//  Font_SetBGColor											ref : fontwrap.h
//
//  背景色設定(4頂点同色)
//
//  int r : 赤
//  int g : 緑
//  int b : 青
//  int a : アルファ値
void Font_SetBGColor(int r, int g, int b, int a)
{
	_SET_VECTOR(pFw->stat.bgcolor[0], r, g, b, a);
	_SET_VECTOR(pFw->stat.bgcolor[1], r, g, b, a);
	_SET_VECTOR(pFw->stat.bgcolor[2], r, g, b, a);
	_SET_VECTOR(pFw->stat.bgcolor[3], r, g, b, a);
}


//============================================================================
//  Font_SetBGColorEx										ref : fontwrap.h
//
//  背景色設定(頂点色)
//
//  int no : 頂点のインデックス
//  int r : 赤
//  int g : 緑
//  int b : 青
//  int a : アルファ値
void Font_SetBGColorEx(int no, int r, int g, int b, int a)
{
	_SET_VECTOR(pFw->stat.bgcolor[no], r, g, b, a);
}


//============================================================================
//  Font_SetULColor											ref : fontwrap.h
//
//  アンダーライン色設定
//
//  int r : 赤
//  int g : 緑
//  int b : 青
//  int a : アルファ値
void Font_SetULColor(int r, int g, int b, int a)
{
	_SET_VECTOR(pFw->stat.ulcolor, r, g, b, a);
}


//============================================================================
//  Font_GetStrLength										ref : fontwrap.h
//
//  文字の描画幅取得
//
//  const char *pszText : 幅を取得したい文字列へのポインタ
//
//  戻り値 int : 文字列の幅（ピクセル値）
int Font_GetStrLength(const char *pszText)
{
	int width;
	calcDrawArea(pszText, &width, NULL);
	width += pFw->stat.FontPitch * pFw->stat.FontRatio;
	return width;
}


//============================================================================
//  Font_SetEffect											ref : fontwrap.h
//
//  文字の装飾を設定
//
//  int mode : 装飾NO
void Font_SetEffect(int mode)
{
	pFw->stat.effect = mode;
}


//============================================================================
//  Font_PutsPacket											ref : fontwrap.h
//
//  文字パケットを作成
//
//  const char *str : 描画したい文字列
//  sceVif1Packet* pPacket : パケットバッファへのポインタ
//  u_int qwc : パケットバッファの大きさ
//
//  戻り値 int : 通常終了(1)
//               -1:パケットが小さすぎる ??
int Font_PutsPacket(const char *str, sceVif1Packet* pPacket, u_int qwc)
{
	int result;

	pFw->pPacket = pPacket;
	pFw->qwc     = qwc;

	if(4 > calcPacketSpace())
	{
		return 1;
	}

	updateTransMatrix();

	scePFontSetFontMatrix(pFw->fd, (sceVu0FMATRIX const*)&pFw->trans);
	scePFontSetLocate(pFw->fd, (sceVu0FVECTOR const*)&pFw->stat.locate);
	scePFontSetColor(pFw->fd, (sceVu0FVECTOR const*)&pFw->stat.color);

	pFw->calc = 0;
	pFw->left   = pFw->stat.locate[0];
	pFw->top    = pFw->stat.locate[1];
	pFw->right  = pFw->left;
	pFw->bottom = pFw->top;

	{
		sceVif1PkCnt(pFw->pPacket, 0);
		sceVif1PkOpenDirectCode(pFw->pPacket, 0);
		sceVif1PkOpenGifTag(pFw->pPacket, _gif_ad.tag);
		sceVif1PkAddGsAD(pFw->pPacket, SCE_GS_TEST_1, SCE_GS_SET_TEST(0, 0, 0, 0, 0, 0, 0, 0));
		sceVif1PkAddGsAD(pFw->pPacket, SCE_GS_ALPHA_1, SCE_GS_SET_ALPHA(0, 1, 0, 1, 0));
		sceVif1PkCloseGifTag(pFw->pPacket);
		sceVif1PkCloseDirectCode(pFw->pPacket);
	}

	result = scePFontPuts(pFw->fd, pFw->pPacket, (int)pFw->qwc, (u_char const*)str);
	if(-1 == result)
	{
		return result;
	}

	scePFontGetLocate(pFw->fd, &pFw->stat.locate);
	scePFontGetColor(pFw->fd, &pFw->stat.color);

	return 1;
}


//============================================================================
//  Font_SetClipOn											ref : fontwrap.h
//
//  クリップON
//
void Font_SetClipOn(void)
{
	pFw->stat.clip = 1;
}


//============================================================================
//  Font_SetClipOn											ref : fontwrap.h
//
//  クリップOFF
//
void Font_SetClipOff(void)
{
	pFw->stat.clip = 0;
}

//============================================================================
//  Font_GetHeight											ref : fontwrap.h
//
//  文字の高さを得る (上下の余白あり)
//
//  戻り値 ：int : 文字の高さ
int Font_GetHeight(void)
{
	return pFw->MaxHeight * pFw->stat.FontRatio * pFw->stat.TvFontRatioY;
}


//============================================================================
//  Font_GetWidth											ref : fontwrap.h
//
//  文字の幅を得る
//
//  戻り値 ：int : 文字の高さ
//int Font_GetWidth(void)
//{
//}



//============================================================================
//  Font_GetLocateX											ref : fontwrap.h
//
//  X値を取得
//
//  戻り値 ：int : X値
int Font_GetLocateX(void)
{
	return (int)(pFw->stat.locate[0] * 16.0f);//FONT_SIZE_FX);
}


//============================================================================
//  Font_GetLocateY											ref : fontwrap.h
//
//  Y値を取得
//
//  戻り値 ：int : Y値
int Font_GetLocateY(void)
{
	return (int)(pFw->stat.locate[1] * 16.0f);
}


//============================================================================
//  Font_GetLocate_f										ref : fontwrap.h
//
//  XY値を取得
//
void Font_GetLocate_f( float *x, float *y )
{
	if(x)	*x = pFw->stat.locate[0];
	if(y)	*y = pFw->stat.locate[1];
}


//============================================================================
// STATIC FUNCTIONS
//============================================================================
//============================================================================
//	fontDrawBG
//
//	フォントの背景を描画
//
//	int left : 左側X座標
//	int top : 上側Y座標
//	int right : 右側X座標
//	int bottom : 下側Y座標
//
//	戻り値 : int : 0:成功/-1:パケット格納領域が不足している
static int fontDrawBG(int left, int top, int right, int bottom)
{
	if(11 > calcPacketSpace())
	{
		printf("ERROR : _FontDrawBG - packet space is low\n");
		return -1;
	}

	{
		sceVif1PkCnt(pFw->pPacket, 0);
		sceVif1PkOpenDirectCode(pFw->pPacket, 0);
		sceVif1PkOpenGifTag(pFw->pPacket, _gif_ad.tag);
		sceVif1PkAddGsAD(pFw->pPacket, SCE_GS_PRIM, SCE_GS_SET_PRIM(SCE_GS_PRIM_TRISTRIP, 1, 0, 0, 1, 0, 0, 0, 0));
		sceVif1PkAddGsAD(pFw->pPacket, SCE_GS_RGBAQ, SCE_GS_SET_RGBAQ(pFw->stat.bgcolor[0][0], pFw->stat.bgcolor[0][1], pFw->stat.bgcolor[0][2], pFw->stat.bgcolor[0][3], 0));
		sceVif1PkAddGsAD(pFw->pPacket, SCE_GS_XYZ2, SCE_GS_SET_XYZ((XOFFSET + left) << 4, (YOFFSET + top) << 4, 0));
		sceVif1PkAddGsAD(pFw->pPacket, SCE_GS_RGBAQ, SCE_GS_SET_RGBAQ(pFw->stat.bgcolor[1][0], pFw->stat.bgcolor[1][1], pFw->stat.bgcolor[1][2], pFw->stat.bgcolor[1][3], 0));
		sceVif1PkAddGsAD(pFw->pPacket, SCE_GS_XYZ2, SCE_GS_SET_XYZ((XOFFSET + right) << 4, (YOFFSET + top) << 4, 0));
		sceVif1PkAddGsAD(pFw->pPacket, SCE_GS_RGBAQ, SCE_GS_SET_RGBAQ(pFw->stat.bgcolor[2][0], pFw->stat.bgcolor[2][1], pFw->stat.bgcolor[2][2], pFw->stat.bgcolor[2][3], 0));
		sceVif1PkAddGsAD(pFw->pPacket, SCE_GS_XYZ2, SCE_GS_SET_XYZ((XOFFSET + left) << 4, (YOFFSET + bottom) << 4, 0));
		sceVif1PkAddGsAD(pFw->pPacket, SCE_GS_RGBAQ, SCE_GS_SET_RGBAQ(pFw->stat.bgcolor[3][0], pFw->stat.bgcolor[3][1], pFw->stat.bgcolor[3][2], pFw->stat.bgcolor[3][3], 0));
		sceVif1PkAddGsAD(pFw->pPacket, SCE_GS_XYZ2, SCE_GS_SET_XYZ((XOFFSET + right) << 4, (YOFFSET + bottom) << 4, 0));
		sceVif1PkCloseGifTag(pFw->pPacket);
		sceVif1PkCloseDirectCode(pFw->pPacket);
	}

	return 0;
}


//============================================================================
//	fontDrawUL
//
//	下線を描画(実線)
//
//	int left : 左側X座標
//	int top : 上側Y座標
//	int right : 右側X座標
//	int bottom : 下側Y座標
//
//	戻り値 : int : 0:成功/-1:パケット格納領域が不足している
static int fontDrawUL(int left, int top, int right, int bottom)
{

	if(6 > calcPacketSpace())
	{
		printf("ERROR : _FontDrawBG - packet space is low\n");
		return -1;
	}

	{
		sceVif1PkCnt(pFw->pPacket, 0);
		sceVif1PkOpenDirectCode(pFw->pPacket, 0);
		sceVif1PkOpenGifTag(pFw->pPacket, _gif_ad.tag);
		sceVif1PkAddGsAD(pFw->pPacket, SCE_GS_PRIM, SCE_GS_SET_PRIM(SCE_GS_PRIM_SPRITE, 0, 0, 0, 1, 0, 0, 0, 0));
		sceVif1PkAddGsAD(pFw->pPacket, SCE_GS_RGBAQ, SCE_GS_SET_RGBAQ(pFw->stat.ulcolor[0], pFw->stat.ulcolor[1], pFw->stat.ulcolor[2], pFw->stat.ulcolor[3], 0));
		sceVif1PkAddGsAD(pFw->pPacket, SCE_GS_XYZ2, SCE_GS_SET_XYZ((XOFFSET + left) << 4, (YOFFSET + bottom - UNDERLINE_H) << 4, 0));
		sceVif1PkAddGsAD(pFw->pPacket, SCE_GS_XYZ2, SCE_GS_SET_XYZ((XOFFSET + right) << 4, (YOFFSET + bottom) << 4, 0));
		sceVif1PkCloseGifTag(pFw->pPacket);
		sceVif1PkCloseDirectCode(pFw->pPacket);
	}

	return 0;
}


//============================================================================
//	fontDrawULD
//
//	下線を描画(点線)
//
//	int left : 左側X座標
//	int top : 上側Y座標
//	int right : 右側X座標
//	int bottom : 下側Y座標
//
//	戻り値 : int : 0:成功/-1:パケット格納領域が不足している
static int fontDrawULD(int left, int top, int right, int bottom)
{
	int x0 = left - (left % ULDOT_W);
	int count = ((right - (right % ULDOT_W)) - x0) / ULDOT_W;

	if((4 + 2 * count) > calcPacketSpace())
	{
		printf("ERROR : _FontDrawBG - packet space is low\n");
		return -1;
	}

	{
		sceVif1PkCnt(pFw->pPacket, 0);
		sceVif1PkOpenDirectCode(pFw->pPacket, 0);
		sceVif1PkOpenGifTag(pFw->pPacket, _gif_ad.tag);
		sceVif1PkAddGsAD(pFw->pPacket, SCE_GS_PRIM, SCE_GS_SET_PRIM(SCE_GS_PRIM_SPRITE, 0, 0, 0, 1, 0, 0, 0, 0));
		sceVif1PkAddGsAD(pFw->pPacket, SCE_GS_RGBAQ, SCE_GS_SET_RGBAQ(pFw->stat.ulcolor[0], pFw->stat.ulcolor[1], pFw->stat.ulcolor[2], pFw->stat.ulcolor[3], 0));

		while(count--){
			sceVif1PkAddGsAD(pFw->pPacket, SCE_GS_XYZ2, SCE_GS_SET_XYZ((XOFFSET + x0) << 4, (YOFFSET + bottom - UNDERLINE_H) << 4, 0));
			sceVif1PkAddGsAD(pFw->pPacket, SCE_GS_XYZ2, SCE_GS_SET_XYZ((XOFFSET + x0 + ULDOT_W / 2) << 4, (YOFFSET + bottom) << 4, 0));
			x0 += ULDOT_W;
		}
		sceVif1PkCloseGifTag(pFw->pPacket);
		sceVif1PkCloseDirectCode(pFw->pPacket);
	}

	return 0;
}


//============================================================================
//	calcPacketSpace
//
//	パケットバッファ空き領域を計算
//
//	戻り値 : int : 空き領域
static int calcPacketSpace(void)
{
	return pFw->qwc - 1 - (((u_int)pFw->pPacket->pCurrent - (u_int)pFw->pPacket->pBase)/sizeof(u_long128));

}


//============================================================================
//	calcDrawArea
//
//	描画領域を計算
//	文字ではなく、背景や下線、点線に使われる
//
//	char const *str : 描画したい文字列
//	int *width : 描画領域の幅
//	int *height: 描画領域の高さ
static void calcDrawArea(char const *str, int *width, int *height)
{
	fontStatus_t back __attribute__ ((aligned(16)));
	sceVu0FVECTOR pos = { 0.0f, 0.0f, 0.0f, 0.0f };
	_memcpy128((u_long128*)&back, (u_long128*)&pFw->stat, sizeof(fontStatus_t) / sizeof(u_long128));

	updateTransMatrix();

	pFw->calc = 1;
	pFw->left = 0.0f;
	pFw->top  = 0.0f;
	pFw->right  = pFw->left;
	pFw->bottom = pFw->top;

	scePFontSetFontMatrix(pFw->fd, (sceVu0FMATRIX const*)&pFw->trans);
	scePFontSetLocate(pFw->fd, (sceVu0FVECTOR const*)&pos);

	scePFontCalcRect(pFw->fd, (u_char const*)str, NULL, NULL);

	if(NULL != width){
		*width = (int)pFw->right;
	}
	if(NULL != height){
		*height = (int)pFw->bottom;
	}

	_memcpy128((u_long128*)&pFw->stat, (u_long128*)&back, sizeof(fontStatus_t) / sizeof(u_long128));
}


//============================================================================
//	_memcpy128
//
//	memcpyの高速化
//
//	u_long128* pDst : コピー先
//	u_long128 const* pSrc : コピー元
//	int qwc : コピーしたいサイズ
static void _memcpy128(u_long128* pDst, u_long128 const* pSrc, int qwc)
{
	int count0 = qwc % 4;
	int count1 = qwc / 4;

	while(count0--)
	{
		*pDst++ = *pSrc++;
	}
	while(count1--)
	{
		u_long128 d0 = *pSrc++;
		u_long128 d1 = *pSrc++;
		u_long128 d2 = *pSrc++;
		u_long128 d3 = *pSrc++;
		*pDst++ = d0;
		*pDst++ = d1;
		*pDst++ = d2;
		*pDst++ = d3;
	}
}


//============================================================================
//	gethex
//
//	文字列で表現された数字を16進->10進
//
//	int c : 文字列で表現された10進数
//
//	戻り値: int : 16進数
static int gethex(int c)
{
	if(('0' <= c) && ('9' >= c)){
		return c - '0';
	}
	c |= 0x20;		// tolower ?
	if(('a' <= c) && ('f' >= c)){
		return (c - 'a') + 10;
	}
	return 0;
}


//============================================================================
//	fontFilter
//
//	文字装飾判別用のフィルタ
//
//	int fd : 
//	scePFontControl* pCtrl : 
//
//	戻り値: int : 
static int fontFilter(int fd, scePFontControl* pCtrl)
{
	int stat = -1;
	int c;
	while(0 < (c = pCtrl->Getc(fd)))
	{
		if(0x20 <= c)
		{
			if((0xFEFFU == c) || (0xFFFEU == c) || (0xFFFFU == c))
			{
			}
			else
			{
				if(0 > fontFilterPutc(pFw->fd, pCtrl, c))
				{
					pCtrl->Ungetc(pFw->fd);
					return stat;
				}
				stat = 1;
			}
		}
		else
		{
			switch(c)
			{
			case '\n':
				{
					sceVu0FVECTOR v;
					scePFontGetLocate(fd, &v);
					v[0] = FONT_SIZE_FX;
					v[1] += FONT_SIZE_FY;
					scePFontSetLocate(fd, (sceVu0FVECTOR const*)&v);
				}
				break;

			case '\t':
				{
					sceVu0FVECTOR v;
					scePFontGetLocate(fd, &v);
					v[0] += 32.0f;
					scePFontSetLocate(fd, (sceVu0FVECTOR const*)&v);
				}
				break;

			case FONTCTRL:
				c = pCtrl->Getc(fd);
				switch(c){
				case 'c':		// \acx x...Color code
					{
						int no = pCtrl->Getc(fd) - '0';
						sceVu0FVECTOR v;
						scePFontGetColor(fd, &v);
						v[0] = (float)FontCol[no][0] / 128.0f;
						v[1] = (float)FontCol[no][1] / 128.0f;
						v[2] = (float)FontCol[no][2] / 128.0f;
						scePFontSetColor(fd, (sceVu0FVECTOR const*)&v);
					}
					break;

				case 'p':		// \apxx xx...Propotional Width   \ap@c cの文字に合わせる
					{
						int c1 = pCtrl->Getc(fd);
						if('@' == c1){
							int c2 = pCtrl->Getc(fd);
							scePFontGlyph glyph;
							if(0 == scePFontGetGlyph(fd, c2, &glyph)){
								pFw->stat.FontPropSw = glyph.prop->width;
							}
						}else{
							int c2 = pCtrl->Getc(fd);
							pFw->stat.FontPropSw = (c1 - '0') * 10 + (c2 - '0');
						}
					}
					break;

				case 'r':		// \arx.xx x.xx...
					{
						int c1;
						int c2;
						int c3;
						int c4;
						c1 = pCtrl->Getc(fd);
						c2 = pCtrl->Getc(fd);
						c3 = pCtrl->Getc(fd);
						c4 = pCtrl->Getc(fd);
						pFw->stat.FontRatioSw = (int)(c1 - '0') * 100 + (int)(c3 - '0') * 10 + (int)(c4 - '0');
						pFw->transmodified = 1;
						updateTransMatrix();
						scePFontSetFontMatrix(fd, (sceVu0FMATRIX const*)&pFw->trans);
					}
					break;

				case 'y':		// \ay+xx \ay-xx xx:16進数 Y座標を1/16ピクセル単位で調整
					{
						int c1 = pCtrl->Getc(fd);
						int c2 = pCtrl->Getc(fd);
						int c3 = pCtrl->Getc(fd);
						sceVu0FVECTOR v;
						scePFontGetLocate(fd, &v);
						if(0 != (pFw->stat.FontAdjustY = ((gethex(c2) << 4) + gethex(c3)))){
							pFw->stat.FontAdjustY *= (c1 == '-') ? -1 : 1;
						}
						v[1] = pFw->top + (float)pFw->stat.FontAdjustY / 16.0f;
						scePFontSetLocate(fd, (sceVu0FVECTOR const*)&v);
					}
					break;

				case 'a':		// \aaxxx xxx...ambient
					{
						int c1 = pCtrl->Getc(fd);
						int c2 = pCtrl->Getc(fd);
						int c3 = pCtrl->Getc(fd);
						sceVu0FVECTOR v;
						scePFontGetColor(fd, &v);
						v[3] = (float)((c1 - '0') * 100 + (c2 - '0') * 10 + (c3 - '0')) / 255.0f;
						scePFontSetColor(fd, (sceVu0FVECTOR const*)&v);
					}
					break;

				case 's':		// \as zenkaku space
					{
						sceVu0FVECTOR v;
						scePFontGetLocate(fd, &v);
						v[0] += pFw->MaxWidth;
						scePFontSetLocate(fd, (sceVu0FVECTOR const*)&v);
					}
					break;
				default:
					// 不明なフォント制御コード
					break;
				}
			}
		}


		{
			sceVu0FVECTOR v;
			scePFontGetLocate(fd, &v);
			if(v[0] > pFw->right)
			{
				pFw->right = v[0];
			}
			if(v[0] > pFw->bottom)
			{
				pFw->bottom = v[0];
			}
		}

	}

	return 0;
}


//============================================================================
//	fontFilterPutc
//
//	文字装飾実装用フィルタ
//
//	int fd : 
//	scePFontControl* pCtrl :
//	int c :
//
//	戻り値: int : 
static int fontFilterPutc(int fd, scePFontControl* pCtrl, int c)
{
	static u_int _effectmsk = 0xffffffffU;
	u_int _effect = pFw->stat.effect & _effectmsk;

	sceVu0FVECTOR locate;
	float xl = 0.0f;
	float xr = 0.0f;

	if(0 != pFw->calc)
	{
		_effect = 0;
	}

	if((0 != pFw->stat.FontPropSw) || (0 != _effect) || pFw->stat.clip )
	{
		float xpitch = pFw->trans[3][0];
		float xratio = pFw->trans[0][0];

		scePFontGlyph glyph;

		if(0 > scePFontGetGlyph(fd, c, &glyph))
		{
#if defined(FONT_DUMMY_CHAR)
			if(0 > scePFontGetGlyph(fd, FONT_DUMMY_CHAR, &glyph))
			{
				return 0;
			}
#else
			return 0;
#endif
		}

		scePFontGetLocate(fd, &locate);
		xl = locate[0];
		xr = xl;

		if(0 != pFw->stat.FontPropSw){
			xr = xl + xpitch + (float)pFw->stat.FontPropSw * xratio;

		}else{
			xr = xl + xpitch + glyph.prop->width * xratio;
		}

		if(0 != _effect)
		{
			float ypitch = pFw->trans[3][1];
			float yratio = pFw->trans[1][1];
			float yt = locate[1] + (float)ypitch - (float)pFw->MaxAscent * yratio;
			float yb = yt + (float)pFw->MaxHeight * yratio;

			if(_effect & FONT_EFFECT_BG){
				if(-1 == fontDrawBG((int)xl, (int)yt, (int)xr, (int)yb)){
					return -1;
				}
				_effectmsk &= ~FONT_EFFECT_BG;
			}
			if(_effect & FONT_EFFECT_UL){
				if(-1 == fontDrawUL((int)xl, (int)yt, (int)xr, (int)yb)){
					return -1;
				}
				_effectmsk &= ~FONT_EFFECT_UL;
			}
			if(_effect & FONT_EFFECT_ULD){
				if(-1 == fontDrawULD((int)xl, (int)yt, (int)xr, (int)yb)){
					return -1;
				}
				_effectmsk &= ~FONT_EFFECT_ULD;
			}
		}

		if(0 != pFw->stat.FontPropSw){
			locate[0] = xl + (float)((pFw->stat.FontPropSw - glyph.prop->width) / 2) * xratio;
			scePFontSetLocate(fd, (sceVu0FVECTOR const*)&locate);
		}
	}

    if( pFw->stat.clip && ( xl > 640.0f || xr < 0.0f ) )
    {
        // この文字はクリップされた
    }
    else
	{
		int result;
		result = pCtrl->Putc(fd, c);
#if defined(FONT_DUMMY_CHAR)
		if(-2 == result){
			// グリフがない時 :D~
			result = pCtrl->Putc(fd, FONT_DUMMY_CHAR);
		}
#endif
		if(-1 == result){
			if(xl != xr){
				locate[0] = xl;
				scePFontSetLocate(fd, (sceVu0FVECTOR const*)&locate);
			}
			return -1;
		}
	}

	if(xl != xr){
		locate[0] = xr;
		scePFontSetLocate(fd, (sceVu0FVECTOR const*)&locate);
	}

	_effectmsk = 0xffffffffU;

	return 0;
}


//============================================================================
//	updateTransMatrix
//
//	位置座標更新
static void updateTransMatrix(void)
{
	float ratio;

	ratio = pFw->stat.FontRatio;
	if(0 != pFw->stat.FontRatioSw)
	{
		ratio = ratio * (float)pFw->stat.FontRatioSw/100.0f;
	}

	if(0 != pFw->transmodified)
	{
		pFw->transmodified = 0;

		sceVu0UnitMatrix(pFw->trans);
		pFw->trans[0][0] = ratio;
		pFw->trans[1][1] = ratio * pFw->stat.TvFontRatioY;

		pFw->trans[3][0] = pFw->stat.FontPitch * pFw->stat.FontRatio;
		pFw->trans[3][1] = pFw->MaxAscent * pFw->stat.FontRatio * pFw->stat.TvFontRatioY;
	}

	scePFontSetPitch(pFw->fd, pFw->stat.FontPitch * ratio);
	scePFontSetFontMatrix(pFw->fd, (sceVu0FMATRIX const*)&pFw->trans);
}


//============================================================================
//	updateScreenMatrix
//
//	スクリーン座標更新
static void updateScreenMatrix(void)
{
	sceVu0UnitMatrix(pFw->screen);
	pFw->screen[3][0] = 2048.0f - (float)(SCREEN_WIDTH / 2);
	pFw->screen[3][1] = 2048.0f - (float)(SCREEN_HEIGHT / 2);
	scePFontSetScreenMatrix(pFw->fd, (sceVu0FMATRIX const*)&pFw->screen);
}
