/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.7
 */
/*
 *                      Emotion Engine Library
 *                          Version 0.01
 *                           Shift-JIS
 *
 *      Copyright (C) 1998-1999 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                        libpkt - devfont.c
 *                         develop library
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       0.01           Mar,29,1999     shibuya
 *       0.02           Jun,11,1999     shibuya
 */


#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include "devfont.h"
#include "devfont.inc"

/*----------------------------------------------------*/

#define CHA_SPR_HEAD	(0x70000000)
#define CHA_SPR_SIZE	(0x00004000)

#define MAXCONS		(4)
#define MAXWIDTH	(80)
#define MAXHEIGHT	(64)
#define MAXPOOL		(MAXWIDTH * MAXHEIGHT * MAXCONS * sizeof(u_short) / 4)

/*----------------------------------------------------*/

typedef struct{
  u_int ox;		// 表示オフセット（GS座標）
  u_int oy;
  u_int gx;		// 表示間隔
  u_int gy;
  u_int dy;		// 縦方向差分
  u_int ctxt;		// GSコンテキスト 0 / 1
  u_int bcount;
  u_int bcycle;

  u_int rgba[8];	// カラーテーブル

}sceDevFontInfo;

typedef struct{
  u_int width;		// サイズ（キャラクタ単位）
  u_int height;
  u_short* pChr;	// 文字バッファ（width * height * 2 byte）

  u_char attribute;	// 色番号

  u_int lx;		// カーソル位置
  u_int ly;

  sceDevFontInfo font;	// 表示フォント
}sceDevCons;

typedef struct{
  u_int size:28;
  u_int mode:4;
}CHA_MEM_TAG;		// 謎メモリー管理

typedef enum{
  KANJI_UNKNOWN = 0,
  KANJI_SJIS = 1,
  KANJI_EUC = 2,
  KANJI_JIS = 3,
}KANJI_CODE;

/*----------------------------------------------------*/

static sceDevCons s_Cons[MAXCONS];

static CHA_MEM_TAG s_Pool[MAXPOOL] = {{ 0x0fffffff, 0xf }};

/*----------------------------------------------------*/

static u_char const CHA_DEV_FONT_PIECE_L = 0xf4;
static u_char const CHA_DEV_FONT_PIECE_R = 0xf8;
static u_char const CHA_DEV_FONT_PIECE_U = 0xf2;
static u_char const CHA_DEV_FONT_PIECE_D = 0xf1;

static u_char const CHA_DEV_FONT_PIECE_LU = 0xf4 | 0xf2;
static u_char const CHA_DEV_FONT_PIECE_LD = 0xf4 | 0xf1;
static u_char const CHA_DEV_FONT_PIECE_RU = 0xf8 | 0xf2;
static u_char const CHA_DEV_FONT_PIECE_RD = 0xf8 | 0xf1;

static u_char const CHA_DEV_FONT_PIECE_HL = 0xf4 | 0xf8;
static u_char const CHA_DEV_FONT_PIECE_VL = 0xf2 | 0xf1;

static u_long const fontMask[256] = {
  0x0000dddddddddd10L, 0x0000dddddddd5d10L, 0x0000ddddddd5dd10L, 0x0000ddddddd55d10L,	// 0x00
  0x0000dddddd5ddd10L, 0x0000dddddd5d5d10L, 0x0000dddddd55dd10L, 0x0000dddddd555d10L,
  0x0000ddddd5dddd10L, 0x0000ddddd5dd5d10L, 0x0000ddddd5d5dd10L, 0x0000ddddd5d55d10L,
  0x0000ddddd55ddd10L, 0x0000ddddd55d5d10L, 0x0000ddddd555dd10L, 0x0000ddddd5555d10L,
  0x0000dddd5ddddd10L, 0x0000dddd5ddd5d10L, 0x0000dddd5dd5dd10L, 0x0000dddd5dd55d10L,	// 0x10
  0x0000dddd5d5ddd10L, 0x0000dddd5d5d5d10L, 0x0000dddd5d55dd10L, 0x0000dddd5d555d10L,
  0x0000dddd55dddd10L, 0x0000dddd55dd5d10L, 0x0000dddd55d5dd10L, 0x0000dddd55d55d10L,
  0x0000dddd555ddd10L, 0x0000dddd555d5d10L, 0x0000dddd5555dd10L, 0x0000dddd55555d10L,
  0x0000ddd5dddddd10L, 0x0000ddd5dddd5d10L, 0x0000ddd5ddd5dd10L, 0x0000ddd5ddd55d10L,	// 0x20
  0x0000ddd5dd5ddd10L, 0x0000ddd5dd5d5d10L, 0x0000ddd5dd55dd10L, 0x0000ddd5dd555d10L,
  0x0000ddd5d5dddd10L, 0x0000ddd5d5dd5d10L, 0x0000ddd5d5d5dd10L, 0x0000ddd5d5d55d10L,
  0x0000ddd5d55ddd10L, 0x0000ddd5d55d5d10L, 0x0000ddd5d555dd10L, 0x0000ddd5d5555d10L,
  0x0000ddd55ddddd10L, 0x0000ddd55ddd5d10L, 0x0000ddd55dd5dd10L, 0x0000ddd55dd55d10L,	// 0x30
  0x0000ddd55d5ddd10L, 0x0000ddd55d5d5d10L, 0x0000ddd55d55dd10L, 0x0000ddd55d555d10L,
  0x0000ddd555dddd10L, 0x0000ddd555dd5d10L, 0x0000ddd555d5dd10L, 0x0000ddd555d55d10L,
  0x0000ddd5555ddd10L, 0x0000ddd5555d5d10L, 0x0000ddd55555dd10L, 0x0000ddd555555d10L,
  0x0000dd5ddddddd10L, 0x0000dd5ddddd5d10L, 0x0000dd5dddd5dd10L, 0x0000dd5dddd55d10L,	// 0x40
  0x0000dd5ddd5ddd10L, 0x0000dd5ddd5d5d10L, 0x0000dd5ddd55dd10L, 0x0000dd5ddd555d10L,
  0x0000dd5dd5dddd10L, 0x0000dd5dd5dd5d10L, 0x0000dd5dd5d5dd10L, 0x0000dd5dd5d55d10L,
  0x0000dd5dd55ddd10L, 0x0000dd5dd55d5d10L, 0x0000dd5dd555dd10L, 0x0000dd5dd5555d10L,
  0x0000dd5d5ddddd10L, 0x0000dd5d5ddd5d10L, 0x0000dd5d5dd5dd10L, 0x0000dd5d5dd55d10L,	// 0x50
  0x0000dd5d5d5ddd10L, 0x0000dd5d5d5d5d10L, 0x0000dd5d5d55dd10L, 0x0000dd5d5d555d10L,
  0x0000dd5d55dddd10L, 0x0000dd5d55dd5d10L, 0x0000dd5d55d5dd10L, 0x0000dd5d55d55d10L,
  0x0000dd5d555ddd10L, 0x0000dd5d555d5d10L, 0x0000dd5d5555dd10L, 0x0000dd5d55555d10L,
  0x0000dd55dddddd10L, 0x0000dd55dddd5d10L, 0x0000dd55ddd5dd10L, 0x0000dd55ddd55d10L,	// 0x60
  0x0000dd55dd5ddd10L, 0x0000dd55dd5d5d10L, 0x0000dd55dd55dd10L, 0x0000dd55dd555d10L,
  0x0000dd55d5dddd10L, 0x0000dd55d5dd5d10L, 0x0000dd55d5d5dd10L, 0x0000dd55d5d55d10L,
  0x0000dd55d55ddd10L, 0x0000dd55d55d5d10L, 0x0000dd55d555dd10L, 0x0000dd55d5555d10L,
  0x0000dd555ddddd10L, 0x0000dd555ddd5d10L, 0x0000dd555dd5dd10L, 0x0000dd555dd55d10L,	// 0x70
  0x0000dd555d5ddd10L, 0x0000dd555d5d5d10L, 0x0000dd555d55dd10L, 0x0000dd555d555d10L,
  0x0000dd5555dddd10L, 0x0000dd5555dd5d10L, 0x0000dd5555d5dd10L, 0x0000dd5555d55d10L,
  0x0000dd55555ddd10L, 0x0000dd55555d5d10L, 0x0000dd555555dd10L, 0x0000dd5555555d10L,
  0x0000d5dddddddd10L, 0x0000d5dddddd5d10L, 0x0000d5ddddd5dd10L, 0x0000d5ddddd55d10L,	// 0x80
  0x0000d5dddd5ddd10L, 0x0000d5dddd5d5d10L, 0x0000d5dddd55dd10L, 0x0000d5dddd555d10L,
  0x0000d5ddd5dddd10L, 0x0000d5ddd5dd5d10L, 0x0000d5ddd5d5dd10L, 0x0000d5ddd5d55d10L,
  0x0000d5ddd55ddd10L, 0x0000d5ddd55d5d10L, 0x0000d5ddd555dd10L, 0x0000d5ddd5555d10L,
  0x0000d5dd5ddddd10L, 0x0000d5dd5ddd5d10L, 0x0000d5dd5dd5dd10L, 0x0000d5dd5dd55d10L,	// 0x90
  0x0000d5dd5d5ddd10L, 0x0000d5dd5d5d5d10L, 0x0000d5dd5d55dd10L, 0x0000d5dd5d555d10L,
  0x0000d5dd55dddd10L, 0x0000d5dd55dd5d10L, 0x0000d5dd55d5dd10L, 0x0000d5dd55d55d10L,
  0x0000d5dd555ddd10L, 0x0000d5dd555d5d10L, 0x0000d5dd5555dd10L, 0x0000d5dd55555d10L,
  0x0000d5d5dddddd10L, 0x0000d5d5dddd5d10L, 0x0000d5d5ddd5dd10L, 0x0000d5d5ddd55d10L,	// 0xa0
  0x0000d5d5dd5ddd10L, 0x0000d5d5dd5d5d10L, 0x0000d5d5dd55dd10L, 0x0000d5d5dd555d10L,
  0x0000d5d5d5dddd10L, 0x0000d5d5d5dd5d10L, 0x0000d5d5d5d5dd10L, 0x0000d5d5d5d55d10L,
  0x0000d5d5d55ddd10L, 0x0000d5d5d55d5d10L, 0x0000d5d5d555dd10L, 0x0000d5d5d5555d10L,
  0x0000d5d55ddddd10L, 0x0000d5d55ddd5d10L, 0x0000d5d55dd5dd10L, 0x0000d5d55dd55d10L,	// 0xb0
  0x0000d5d55d5ddd10L, 0x0000d5d55d5d5d10L, 0x0000d5d55d55dd10L, 0x0000d5d55d555d10L,
  0x0000d5d555dddd10L, 0x0000d5d555dd5d10L, 0x0000d5d555d5dd10L, 0x0000d5d555d55d10L,
  0x0000d5d5555ddd10L, 0x0000d5d5555d5d10L, 0x0000d5d55555dd10L, 0x0000d5d555555d10L,
  0x0000d55ddddddd10L, 0x0000d55ddddd5d10L, 0x0000d55dddd5dd10L, 0x0000d55dddd55d10L,	// 0xc0
  0x0000d55ddd5ddd10L, 0x0000d55ddd5d5d10L, 0x0000d55ddd55dd10L, 0x0000d55ddd555d10L,
  0x0000d55dd5dddd10L, 0x0000d55dd5dd5d10L, 0x0000d55dd5d5dd10L, 0x0000d55dd5d55d10L,
  0x0000d55dd55ddd10L, 0x0000d55dd55d5d10L, 0x0000d55dd555dd10L, 0x0000d55dd5555d10L,
  0x0000d55d5ddddd10L, 0x0000d55d5ddd5d10L, 0x0000d55d5dd5dd10L, 0x0000d55d5dd55d10L,	// 0xd0
  0x0000d55d5d5ddd10L, 0x0000d55d5d5d5d10L, 0x0000d55d5d55dd10L, 0x0000d55d5d555d10L,
  0x0000d55d55dddd10L, 0x0000d55d55dd5d10L, 0x0000d55d55d5dd10L, 0x0000d55d55d55d10L,
  0x0000d55d555ddd10L, 0x0000d55d555d5d10L, 0x0000d55d5555dd10L, 0x0000d55d55555d10L,
  0x0000d555dddddd10L, 0x0000d555dddd5d10L, 0x0000d555ddd5dd10L, 0x0000d555ddd55d10L,	// 0xe0
  0x0000d555dd5ddd10L, 0x0000d555dd5d5d10L, 0x0000d555dd55dd10L, 0x0000d555dd555d10L,
  0x0000d555d5dddd10L, 0x0000d555d5dd5d10L, 0x0000d555d5d5dd10L, 0x0000d555d5d55d10L,
  0x0000d555d55ddd10L, 0x0000d555d55d5d10L, 0x0000d555d555dd10L, 0x0000d555d5555d10L,
  0x0000d5555ddddd10L, 0x0000d5555ddd5d10L, 0x0000d5555dd5dd10L, 0x0000d5555dd55d10L,	// 0xf0
  0x0000d5555d5ddd10L, 0x0000d5555d5d5d10L, 0x0000d5555d55dd10L, 0x0000d5555d555d10L,
  0x0000d55555dddd10L, 0x0000d55555dd5d10L, 0x0000d55555d5dd10L, 0x0000d55555d55d10L,
  0x0000d555555ddd10L, 0x0000d555555d5d10L, 0x0000d5555555dd10L, 0x0000d55555555d10L,
};	// 256 * 8 = 2k bytes

/*----------------------------------------------------*/

static void sceDevFontDefault(sceDevFontInfo* pFont, u_int gs_x, u_int gs_y);

static void sceDevFontIdle(sceDevFontInfo* pFont, u_int count);


/*----------------------------------------------------*/

static u_short euc2jis(u_short euc);
static u_short sjis2jis(u_short sjis);

static void sceDevFontRefDirectImage(sceGifPacket* pPacket, u_long gsxyz, u_int dy, u_int rgba, u_long image, u_int ctxt);
static u_int sceDevFontRefStrN(sceGifPacket* pPacket, sceDevFontInfo const* pFont, u_int x, u_int y, u_short const* str, u_int count);

static void sceDevConsPutc(sceDevCons* pCons, int x, int y, u_short ac);
static u_short sceDevConsGetc(sceDevCons* pCons, int x, int y);
static void sceDevConsPiece(sceDevCons* pCons, int x, int y, u_char c, u_char a);

static u_char sceDevFontKnj2Chr(u_short code);

static void chaGifPkOpenGifTag2(sceGifPacket* pPacket, u_long tagLow, u_long tagHigh);

static void* chaMemAlloc(u_int size);
static void chaMemFree(void* mem);

/*----------------------------------------------------*/

static void sceDevFontDefault(sceDevFontInfo* pFont, u_int const gs_x, u_int const gs_y){

  pFont->ox = gs_x;
  pFont->oy = gs_y;
  pFont->dy = 0x10;
  pFont->gx = 8 << 4;
  pFont->gy = 8 << 4;
  pFont->ctxt = 0;
  pFont->bcount = 0;
  pFont->bcycle = 60;

  pFont->rgba[0] = 0x80000000;
  pFont->rgba[1] = 0x80ff0000;
  pFont->rgba[2] = 0x800000ff;
  pFont->rgba[3] = 0x80ff00ff;
  pFont->rgba[4] = 0x8000ff00;
  pFont->rgba[5] = 0x80ffff00;
  pFont->rgba[6] = 0x8000ffff;
  pFont->rgba[7] = 0x80ffffff;
}

static void sceDevFontIdle(sceDevFontInfo* pFont, u_int count){
  pFont->bcount += count;

  while(pFont->bcount >= pFont->bcycle){
    pFont->bcount -= pFont->bcycle;
  }
}

static void sceDevFontSetColor(sceDevFontInfo* pFont, u_char c, u_char r, u_char g, u_char b){
  pFont->rgba[c & 0x7] = (0x80 << 24) | (b << 16) | (g << 8) | (r);
}


/*----------------------------------------------------*/

void sceDevConsInit(void){

  u_int i;
  for(i = 0; i < MAXCONS; i++){
    s_Cons[i].pChr = NULL;
  }
}

int sceDevConsOpen(u_int const gs_x, u_int const gs_y, u_int const ch_w, u_int const ch_h){

  sceDevCons* pCons = NULL;

  if(ch_w > MAXWIDTH){ return 0; }
  if(ch_h > MAXHEIGHT){ return 0; }

  {
    u_int i;
    for(i = 0; i < MAXCONS; i++){
      if(NULL == s_Cons[i].pChr){
	pCons = &s_Cons[i];
	break;
      }
    }
  }

  if(NULL == pCons){
    return 0;
  }

  {
    pCons->pChr = chaMemAlloc(ch_w * ch_h * sizeof(u_short));
    if(NULL == pCons->pChr){
      return 0;
    }

    pCons->width = ch_w;
    pCons->height = ch_h;

    pCons->attribute = 0x07;

    sceDevFontDefault(&pCons->font, gs_x, gs_y);

    sceDevConsClear((int)pCons);
  }

  return (int)pCons;
}

void sceDevConsClose(int const cd){

  sceDevCons* const pCons = (sceDevCons*)cd;

  chaMemFree(pCons->pChr);
  pCons->pChr = NULL;
  pCons->width = 0;
  pCons->height = 0;
}

void sceDevConsRef(int const cd, sceGifPacket* const pPacket){

  sceDevCons* const pCons = (sceDevCons*)cd;
  u_int const w = pCons->width;
  u_int const h = pCons->height;
  u_short const* pChr = pCons->pChr;
  u_int y;

  for(y = 0; y < h; y++){
	sceDevFontRefStrN(pPacket, &pCons->font, 0, y, pChr, w);
	pChr += w;
  }
}

void sceDevConsDraw(int const cd){

  sceDevCons* const pCons = (sceDevCons*)cd;
  sceDmaChan* const devGif = sceDmaGetChan(SCE_DMA_GIF);
  sceGifPacket packet[2];

  u_int const w = pCons->width;
  u_int const h = pCons->height;
  u_short const* pChr = pCons->pChr;
  u_int y;

  u_long128 work[2][(6 + 1) * 16 * MAXWIDTH];
  u_int side = 0;

  sceGifPkInit(&packet[0], work[0]);
  sceGifPkInit(&packet[1], work[1]);
  devGif->chcr.TTE = 1;

  for(y = 0; y < h; y++){
    sceGifPkReset(&packet[side]);
    sceDevFontRefStrN(&packet[side], &pCons->font, 0, y, pChr, w);
    sceGifPkEnd(&packet[side], 0, 0, 0);
    sceGifPkTerminate(&packet[side]);

    FlushCache(0);
    sceGsSyncPath(0, 0);
    sceDmaSend(devGif, packet[side].pBase);

    side ^= 1;

    pChr += w;
  }
  sceGsSyncPath(0, 0);

}

void sceDevConsDrawS(int const cd){

  sceDevCons* const pCons = (sceDevCons*)cd;
  sceDmaChan* const devGif = sceDmaGetChan(SCE_DMA_GIF);
  sceGifPacket packet[2];

  u_int const w = pCons->width;
  u_int const h = pCons->height;
  u_short const* pChr = pCons->pChr;
  u_int const fmax = (0x200 / (2 + (6 + 1) * 16));
  u_int y;
  u_int side = 0;

  sceGifPkInit(&packet[0], (u_long128*)(CHA_SPR_HEAD));
  sceGifPkInit(&packet[1], (u_long128*)(CHA_SPR_HEAD + CHA_SPR_SIZE / 2));
  devGif->chcr.TTE = 1;

  for(y = 0; y < h; y++){
    u_int x = 0;

    while(x < w){
      u_int count = ((w - x) < fmax) ? (w - x) : fmax;

      sceGifPkReset(&packet[side]);
      sceDevFontRefStrN(&packet[side], &pCons->font, x, y, pChr, count);
      sceGifPkEnd(&packet[side], 0, 0, 0);
      sceGifPkTerminate(&packet[side]);

      sceGsSyncPath(0, 0);
      sceDmaSend(devGif, (u_long128*)(((u_int)packet[side].pBase & 0x3fff) | 0x80000000));

      side ^= 1;

      pChr += count;
      x += count;
    }
  }
  sceGsSyncPath(0, 0);
}

void sceDevConsClear(int const cd){
  sceDevCons* const pCons = (sceDevCons*)cd;

  u_short const data = (u_short)' ' | ((u_short)0x07 << 8);
  u_short* pChr = pCons->pChr;
  u_int count = pCons->width * pCons->height;

  while(count--){
    *pChr++ = data;
  }
	pCons->lx = 0;
	pCons->ly = 0;
}

void sceDevConsSetColor(int cd, u_char c, u_char r, u_char g, u_char b){
  sceDevCons* const pCons = (sceDevCons*)cd;

  sceDevFontSetColor(&pCons->font, c, r, g, b);
}

u_int sceDevConsPrintf(int const cd, char const* const str, ...){

  u_int result = 0;
  u_char work[1024];

  {
    va_list list;
    va_start(list, str);
    vsprintf(work, str, list);
    va_end(list);
  }

  {
    u_char* ptr = work;
    u_short c0;
    KANJI_CODE mode = KANJI_UNKNOWN;

    while((u_short)'\0' != (c0 = (u_short)(*ptr++))){

      if('\x1b' == c0){
	u_char const c1 = ptr[0];
	u_char const c2 = ptr[1];
	u_short code;

	if(('\0' == c1) || ('\0' == c2)){
	  break;
	}

	code = ((u_short)c1 << 8) | (u_short)c2;

	switch(code){
	case 0x2440:
	case 0x2442:
	  mode = KANJI_UNKNOWN;
	  ptr += 2;
	  break;

	case 0x2840:
	case 0x2842:
	  mode = KANJI_JIS;
	  ptr += 2;
	  break;

	default:
	  break;
	}

      }else if(c0 == '&'){
	u_char c1 = *ptr++;

	if('\0' == c1){
	  break;
	}

	if((c1 >= '0') && (c1 <= '7')){
	  if(0 != cd){
	    ((sceDevCons*)cd)->attribute = (c1 - '0');
	  }
	  continue;

	}else if('&' != c1){
	  continue;

	}else{
	  c0 = c1;
	}

      }else if(c0 >= 0x80){
	u_short const c1 = *ptr++;
	u_short code;

	if('\0' == c1){
	  break;
	}

	code = (c0 << 8) | (u_short)c1;

	switch(mode){
	case KANJI_UNKNOWN:
	  if(c0 <= 0x9f){
	    mode = KANJI_SJIS;
	    c0 = sjis2jis(code);

	  }else if((c0 >= 0xe0) && (c0 <= 0xef)){
	    if((code & 0xff) < 0xa1){
	      mode = KANJI_SJIS;
	      c0 = sjis2jis(code);

	    }else{
	      mode = KANJI_UNKNOWN;
	      c0 = euc2jis(code);
	    }

	  }else{
	    mode = KANJI_EUC;
	    c0 = euc2jis(code);
	  }
	  break;

	case KANJI_SJIS:
	  if(c0 >= 0x80){
	    c0 = sjis2jis(code);
	  }
	  break;

	case KANJI_EUC:
	  if(c0 >= 0x80){
	    c0 = euc2jis(code);
	  }
	  break;

	case KANJI_JIS:
	  break;
	}

      }else{
	if(KANJI_JIS == mode){
	  u_char const c1 = (*ptr++);

	  if('\0' == c1){
	    break;
	  }
	  c0 = (c0 << 8) | (u_short)c1;
	}
      }

      if(0 != cd){
	sceDevCons* const pCons = (sceDevCons*)cd;
	sceDevConsPut(cd, sceDevFontKnj2Chr(c0), pCons->attribute);
      }

      result++;
    }
  }

  return result;
}

/*----------------------------------------------------*/

void sceDevConsLocate(int const cd, u_int lx, u_int ly){
  sceDevCons* const pCons = (sceDevCons*)cd;

  if(lx >= pCons->width){ lx = pCons->width - 1; }
  if(ly >= pCons->height){ ly = pCons->height - 1; }

  pCons->lx = lx;
  pCons->ly = ly;
}

void sceDevConsPut(int const cd, u_char const c, u_char const a){

  sceDevCons* const pCons = (sceDevCons*)cd;

  if(c >= 0x20){
    sceDevConsPutc(pCons, pCons->lx, pCons->ly, (u_short)c | ((u_short)a << 8));
    pCons->lx++;

  }else{
    if('\n' == c){ pCons->ly++; pCons->lx = 0; }
    if('\r' == c){ pCons->lx = 0; }
  }

  if(pCons->lx >= pCons->width){
    pCons->lx = 0;
    pCons->ly++;
  }

  if(pCons->ly >= pCons->height){
    sceDevConsRollup(cd, pCons->ly - pCons->height + 1);
    pCons->ly = pCons->height - 1;
  }
}

u_short sceDevConsGet(int const cd){

  sceDevCons* const pCons = (sceDevCons*)cd;

  u_short c = sceDevConsGetc(pCons, pCons->lx, pCons->ly);

  pCons->lx++;
  if(pCons->lx >= pCons->width){
    if(pCons->ly >= pCons->height){
      pCons->lx = pCons->width;
      pCons->ly = pCons->height;

    }else{
      pCons->lx = 0;
      pCons->ly++;
    }
  }

  return c;
}

void sceDevConsAttribute(int cd, u_char col){
  sceDevCons* const pCons = (sceDevCons*)cd;

  pCons->attribute = (u_char)col;
}

/*----------------------------------------------------*/

void sceDevConsClearBox(int const cd, int const x, int const y, u_int const w, u_int const h){

  sceDevCons* const pCons = (sceDevCons*)cd;

  u_int lx;
  u_int ly;

  for(ly = 0; ly < h; ly++){
    for(lx = 0; lx < w; lx++){
      sceDevConsPutc(pCons, x + lx, y + ly, (u_short)' ' | ((u_short)0x07 << 8));
    }
  }
}

void sceDevConsMove(int const cd, int const dx, int const dy, int const sx, int const sy, u_int const w, u_int const h){

  sceDevCons* const pCons = (sceDevCons*)cd;
  u_int lx;
  u_int ly;

  if((dy < sy)||((dy == sy)&&(dx < sx))){

    for(ly = 0; ly < h; ly++){
      for(lx = 0; lx < w; lx++){
	sceDevConsPutc(pCons, dx + lx, dy + ly, sceDevConsGetc(pCons, sx + lx, sy + ly));
      }
    }

  }else{

    for(ly = h - 1; ly >= 0; ly--){
      for(lx = w - 1; lx >= 0; lx--){
	sceDevConsPutc(pCons, dx + lx, dy + ly, sceDevConsGetc(pCons, sx + lx, sy + ly));
      }
    }
  }
}


void sceDevConsRollup(int const cd, u_int const line){

  sceDevCons* const pCons = (sceDevCons*)cd;

  u_int const w = pCons->width;
  u_int const h = pCons->height;

  if(line < h){
    sceDevConsMove(cd, 0, 0, 0, line, w, h - line);
    sceDevConsClearBox(cd, 0, h - line, w, line);

  }else{
    sceDevConsClearBox(cd, 0, 0, w, h);
  }
}

/*----------------------------------------------------*/

void sceDevConsMessage(int cd, int const x, int const y, char const* const str){

  u_int len = sceDevConsPrintf(0, str);

  if(0 == cd){
    cd = sceDevConsOpen((u_int)x, (u_int)y, len + 2, 3);

    if(0 != cd){
      sceDevConsFrame(cd, 0, 0, len + 2, 3);
      sceDevConsLocate(cd, 1, 1);
      sceDevConsPrintf(cd, str);
    }

  }else{
    sceDevConsFrame(cd, x - 1, y - 1, len + 2, 3);
    sceDevConsLocate(cd, x, y);
    sceDevConsPrintf(cd, str);
  }
}

void sceDevConsFrame(int const cd, int const x, int const y, u_int const w, u_int const h){

  sceDevCons* const pCons = (sceDevCons*)cd;

  u_char const a = 0x07;
  int yy;
  int xx;
  u_int const ww = w - 1;
  u_int const hh = h - 1;

  sceDevConsPiece(pCons, x, y, CHA_DEV_FONT_PIECE_RD, a);
  sceDevConsPiece(pCons, x + ww, y, CHA_DEV_FONT_PIECE_LD, a);
  sceDevConsPiece(pCons, x, y + hh, CHA_DEV_FONT_PIECE_RU, a);
  sceDevConsPiece(pCons, x + ww, y + hh, CHA_DEV_FONT_PIECE_LU, a);

  for(xx = x + 1; xx < (x + ww); xx++){
    sceDevConsPiece(pCons, xx, y, CHA_DEV_FONT_PIECE_HL, a);
    sceDevConsPiece(pCons, xx, y + hh, CHA_DEV_FONT_PIECE_HL, a);
  }

  for(yy = y + 1; yy < (y + hh); yy++){
    sceDevConsPiece(pCons, x, yy, CHA_DEV_FONT_PIECE_VL, a);
    sceDevConsPiece(pCons, x + ww, yy, CHA_DEV_FONT_PIECE_VL, a);
  }
}



/*----------------------------------------------------*/

static u_short euc2jis(u_short const euc){
  return euc ^ 0x8080;
}

static u_short sjis2jis(u_short const sjis){
  u_char ch = (u_char)((sjis >> 8) & 0xff);
  u_char cl = (u_char)(sjis & 0xff);

  ch -= (ch <= 0x9f) ? 0x71 : 0xb1;
  ch = (ch << 1) + 1;
  if(cl > 0x7f){ cl--; }
  if(cl >= 0x9e){
    cl -= 0x7d;
    ch++;

  }else{
    cl -= 0x1f;
  }

  return ((u_short)ch << 8) | (u_short)cl;
}

static void sceDevFontRefDirectImage(sceGifPacket* const pPacket, u_long gsxyz, u_int dy, u_int const rgba, u_long image, u_int const ctxt){

  if(0L == image){ return; }

  {
    u_long const gifTag = SCE_GIF_SET_TAG(0, 1, 0, 0, 1, 12);
    u_int count = 8;

    dy <<= 16;
    gsxyz -= (4 << 16) + dy;

    while(count--){
      u_char const lineImg = image & 0xff;
      image >>= 8;
      gsxyz += dy;

      if(0 == lineImg){ continue; }

      chaGifPkOpenGifTag2(pPacket, gifTag, fontMask[lineImg]);
      sceGifPkAddGsData(pPacket, SCE_GS_SET_PRIM(2, // CHA_GS_PRIM_LINESTRIP,
						 0, // CHA_GS_PRIM_FLAT,
						 0, // CHA_GS_PRIM_TEXT_OFF,
						 0, // CHA_GS_PRIM_FOG_OFF,
						 1, // CHA_GS_PRIM_ALPHA_ON,
						 1, 0,
						 ctxt, // CHA_GS_CTXT_1,
						 0));// prim
      sceGifPkAddGsData(pPacket, (u_long)rgba);	// rgba
      sceGifPkAddGsData(pPacket, gsxyz + 0x00);	// xyzf 0
      sceGifPkAddGsData(pPacket, gsxyz + 0x10);	// xyzf 1
      sceGifPkAddGsData(pPacket, gsxyz + 0x20);	// xyzf 2
      sceGifPkAddGsData(pPacket, gsxyz + 0x30);	// xyzf 3
      sceGifPkAddGsData(pPacket, gsxyz + 0x40);	// xyzf 4
      sceGifPkAddGsData(pPacket, gsxyz + 0x50);	// xyzf 5
      sceGifPkAddGsData(pPacket, gsxyz + 0x60);	// xyzf 6
      sceGifPkAddGsData(pPacket, gsxyz + 0x70);	// xyzf 7
      sceGifPkAddGsData(pPacket, gsxyz + 0x80);	// xyzf 8
      sceGifPkAddGsData(pPacket, gsxyz + 0x90);	// xyzf 9
      sceGifPkCloseGifTag(pPacket);

    }
  }
}

static u_int sceDevFontRefStrN(sceGifPacket* const pPacket, sceDevFontInfo const* const pFont, u_int const x, u_int const y, u_short const* str, u_int count){

  u_int result = 0;

  u_int fx = pFont->ox + x * pFont->gx;
  u_int const fy = pFont->oy + y * pFont->gy;
  u_int const gx = pFont->gx;
  u_int const dy = pFont->dy;

  sceGifPkCnt(pPacket, 0, 0, 0);

  while(count--){

    u_short const ch = *str++;
    u_char const c = ch & 0xff;
    u_char const a = ch >> 8;

    if('\0' == c){ break; }

    {
      u_long const image = sceFont8[(u_int)c].image.data;
      u_int const rgba = pFont->rgba[a & 0x7];

      sceDevFontRefDirectImage(pPacket, SCE_GS_SET_XYZ(fx, fy, 0xffffffff), dy, rgba, image, pFont->ctxt);
    }

    fx += gx;

    result++;
  }

  return result;
}

static void sceDevConsPutc(sceDevCons* const pCons, int const x, int const y, u_short const ac){

  if((x < 0) || (x >= pCons->width)){ return; }
  if((y < 0) || (y >= pCons->height)){ return; }

  pCons->pChr[y * pCons->width + x] = ac;
}

static u_short sceDevConsGetc(sceDevCons* const pCons, int const x, int const y){

  if((x < 0) || (x >= pCons->width)){ return (u_short)' ' | ((u_short)0x07 << 8); }
  if((y < 0) || (y >= pCons->height)){ return (u_short)' ' | ((u_short)0x07 << 8); }

  return pCons->pChr[y * pCons->width + x];
}

static void sceDevConsPiece(sceDevCons* const pCons, int const x, int const y, u_char c, u_char const a){
  if((x < 0) || (x >= pCons->width)){ return; }
  if((y < 0) || (y >= pCons->height)){ return; }

  {
    u_char cc = (u_char)(pCons->pChr[y * pCons->width + x] & 0xff);

    if((0xf1 <= cc) && (0xff >= cc)){
      c = (cc | c);
    }
    pCons->pChr[y * pCons->width + x] = (u_short)c | ((u_short)a << 8);
  }
}

static u_char sceDevFontKnj2Chr(u_short code){
  if(code & 0xff00){
    if((0x2421 <= code) && (0x2473 >= code)){
      code -= 0x2421 - 0x80;

    }else{
      code = 0x7f;
    }
  }
  return (u_char)code;
}

/*----------------------------------------------------*/

static void chaGifPkOpenGifTag2(sceGifPacket* const pPacket, u_long const tagLow, u_long const tagHigh){
  u_long* ptr = (u_long*)pPacket->pCurrent;
  pPacket->pGifTag = ptr;
  *ptr++ = tagLow;
  *ptr++ = tagHigh;
  pPacket->pCurrent = (u_int*)ptr;
}

static void* chaMemAlloc(u_int size){

  void* result = NULL;

  if(0x0fffffff == s_Pool[0].size){
    s_Pool[0].size = MAXPOOL - 2;
    s_Pool[0].mode = 0;
    s_Pool[MAXPOOL - 1].size = 0;
    s_Pool[MAXPOOL - 1].mode = 3;
  }

  size = (size + sizeof(CHA_MEM_TAG) - 1) / sizeof(CHA_MEM_TAG);

  {
    u_int pos = 0;
    while(3 != s_Pool[pos].mode){
      u_int p_size = s_Pool[pos].size;

      if((0 == s_Pool[pos].mode) && (size <= p_size)){

	if(size != p_size){
	  s_Pool[pos + 1 + size].size = p_size - size - 1;
	  s_Pool[pos + 1 + size].mode = 0;
	  s_Pool[pos].size = size;
	}
	s_Pool[pos].mode = 1;
	result = &s_Pool[pos + 1];
	break;
      }

      pos += 1 + p_size;
    }
  }

//printf("chaMemAlloc %08x\n", (u_int)result);

  return result;
}

static void chaMemFree(void* const mem){

  if(0x0fffffff == s_Pool[0].size){
    return;
  }

  if(NULL == mem){
    return;
  }

  {
    u_int prev = 0;
    u_int pos = 0;
    while(3 != s_Pool[pos].mode){

      u_int next = pos + 1 + s_Pool[pos].size;

      if(mem == &s_Pool[pos + 1]){
//printf("chaMemFree %08x\n", (u_int)mem);

	if(0 == s_Pool[next].mode){
	  s_Pool[pos].size += 1 + s_Pool[next].size;
	}
	if(0 == s_Pool[prev].mode){
	  s_Pool[prev].size += 1 + s_Pool[pos].size;

	}else{
	  s_Pool[pos].mode = 0;
	}
	break;
      }

      prev = pos;
      pos = next;
    }
  }
}



/*----------------------------------------------------*/

u_long128* sceDevFont(u_long128* p, int x, int y, int z, char *s, int ctxt){
  sceGifPacket packet;

  u_int fx = x;
  u_int fy = y;
  u_int const gx = 8 << 4;
  u_int const gy = 6 << 4;
  u_int const dy = 0xc;
  u_char c;
  u_int* pVifCode;

  sceGifPkInit(&packet, p);
  sceGifPkCnt(&packet, 0, 0, 0);
  pVifCode = packet.pCurrent - 1;

  while('\0' != (c = *s++)){

    if('\n' == c){
      fx = x;
      fy += gy;
    }

    {
      u_long const image = sceFont8[(u_int)c].image.data;

      sceDevFontRefDirectImage(&packet, SCE_GS_SET_XYZ(fx, fy, 0xffffffff), dy, 0x80ffffff, image, ctxt);
    }

    fx += gx;
  }
  {
    *pVifCode = (0x50 << 24) | (packet.pCurrent - (pVifCode + 1)) / (sizeof(u_long128) / sizeof(u_int));
  }
  sceGifPkEnd(&packet, 0, 0, 0);

  return (u_long128*)(packet.pCurrent - 4);
}

/* [eof] */
