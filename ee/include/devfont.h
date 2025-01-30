/* SCEI CONFIDENTIAL
 DTL-S12000 Next Generation PlayStation Programmer Tool Runtime Library Release 1.0
 */
/* 
 *                      Emotion Engine Library
 *                          Version 0.01
 *                           Shift-JIS
 *
 *      Copyright (C) 1998-1999 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                        libdev - devfont.h
 *                         develop library
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       0.01           Mar,29,1999     shibuya
 *       0.02           Jun,11,1999     shibuya
 */

#ifndef __devfont__
#define __devfont__

#include <eekernel.h>
#include <eetypes.h>
#include <eestruct.h>

#include <libdma.h>
#include <libgraph.h>
#include <libgifpk.h>

#ifdef __cplusplus
extern "C" {
#endif

// コンソールライブラリを初期化
void sceDevConsInit();

// コンソールをオープン。（１つしかない。サイズの最大は80 x 64キャラクタ）
int sceDevConsOpen(u_int gs_x, u_int gs_y, u_int chr_w, u_int chr_h);

// コンソールをクローズ
void sceDevConsClose(int cd);


// コンソールのイメージをパケットにつける。
void sceDevConsRef(int cd, sceGifPacket* pPacket);

// コンソールのイメージを描画
void sceDevConsDraw(int cd);

// コンソールのイメージを描画（スクラッチパッド使用）
void sceDevConsDrawS(int cd);

// コンソールクリア
void sceDevConsClear(int cd);

// コンソールへ文字表示
u_int sceDevConsPrintf(int cd, const u_char* str, ...);


// 旧ライブラリサポート
u_long128* sceDevFont(u_long128*, int, int, int, char *, int);
  /* packet, x, y, z(無効), str, ctxt */


#ifdef __cplusplus
}
#endif

#endif /* __devfont__ */
