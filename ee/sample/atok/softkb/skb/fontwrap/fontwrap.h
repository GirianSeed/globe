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


#ifndef __FONTWRAP_H__
#define __FONTWRAP_H__

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
extern "C" {
#endif

#include <libvu0.h>
#include <libpkt.h>

#define FONT_EFFECT_BG	(1 << 0)	// ”wŒi‚Â‚«
#define FONT_EFFECT_UL	(1 << 1)	// ‰ºü‚Â‚«
#define FONT_EFFECT_ULD	(1 << 2)	// “_ü‚Â‚«


#define FONT_DATASIZE	(3 * 1024 * 1024)


void Font_Init(void);
int  Font_Load(int gsclutadr, int gstexadr, u_long128 *pFontData);
void Font_Unload(void);

void Font_SetLocate(int x, int y);
void Font_SetPitch(int pitch);
void Font_SetRetHeight(int y);
void Font_SetProp(int sw);
void Font_SetRatio(float flRatio);
void Font_SetRatioPixel(int size);
void Font_SetTvFontRatio(float flRatio);

void Font_SetEffect(int mode);
void Font_SetColor(int r, int g, int b, int a);
void Font_SetBGColor(int r, int g, int b, int a);
void Font_SetBGColorEx(int no, int r, int g, int b, int a);
void Font_SetULColor(int r, int g, int b, int a);

int  Font_GetStrLength(const char *pszText);
int  Font_PutsPacket(const char *str, sceVif1Packet* pPacket, u_int qwc);
int  Font_GetLocateX(void);

void Font_SetClipOn(void);
void Font_SetClipOff(void);

int Font_GetHeight(void);

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
}
#endif

#endif	// ! __FONTWRAP_H__
