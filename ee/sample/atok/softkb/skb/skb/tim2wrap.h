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

#ifndef __TIM2WRAP_H__
#define __TIM2WRAP_H__

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
extern "C" {
#endif

#include <shareddef.h>

// 初期化
void Tim2_Init( u_int screenWidth, u_int screenHeight,
                u_int offX, u_int offY,
                u_int texBasePtr, u_int clutBasePtr,
                void* (*malign)(size_t align, size_t size),
                void  (*mfree)(void *p));

// Tim2ファイル読込み
int Tim2_ReadTim2(textureInfo_t *pTex, const char *filename);

// TIM2ファイルデータをテクスチャバッファに転送する為のパケット作成
void Tim2_SendTim2(sceVif1Packet *pk, textureInfo_t *pTex);

// スプライト描画
void Tim2_DrawSprite(sceVif1Packet *pk, u_int col, sprite_t *pSp, uv_t *pUv, textureInfo_t *pTex);

// Tim2ファイル削除
void Tim2_DeleteTim2(textureInfo_t *pTex);



#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
}
#endif

#endif	// !__TIM2WRAP_H__
