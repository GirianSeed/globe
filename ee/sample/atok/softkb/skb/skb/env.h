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
#ifndef __ENV_H__
#define __ENV_H__

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
extern "C" {
#endif

#if FIELD_MODE
	#define VRESOLUTION_RATIO	(1.000f)
#else
	#define VRESOLUTION_RATIO	(0.500f)
#endif


#if FIELD_MODE
	#define VRESOLUTION_ADJUST(_v_)		(_v_)
	#define VRESOLUTION_ADJUST_F(_v_)	(_v_)
#else
	#define VRESOLUTION_ADJUST(_v_)		((_v_)/2)
	#define VRESOLUTION_ADJUST_F(_v_)	((_v_)/2.0f)
#endif		// FIELD_MODE

#if PAL_MODE
	#define TV_RATIO	(1.15555f)
	#define SCREEN_VHEIGHT	(512)
	#define SCREEN_HEIGHT	(VRESOLUTION_ADJUST(SCREEN_VHEIGHT))
#else
	#define TV_RATIO	(1.000f)
	#define SCREEN_VHEIGHT	(448)
	#define SCREEN_HEIGHT	(VRESOLUTION_ADJUST(SCREEN_VHEIGHT))
#endif

#define NumItems(_p_) (sizeof(_p_)/sizeof(*(_p_)))		// 要素数取得用マクロ

#define SCREEN_WIDTH	(640)
#define SCREEN_CENTERX	(4096 / 2)
#define SCREEN_CENTERY	(4096 / 2)
#define SCREEN_TOP		(SCREEN_CENTERY - SCREEN_HEIGHT / 2)
#define SCREEN_LEFT		(SCREEN_CENTERX - SCREEN_WIDTH  / 2)
#define OFFX			(((4096-SCREEN_WIDTH) /2)<<4)
#define OFFY			(((4096-SCREEN_HEIGHT)/2)<<4)
#define SAFESCREEN_X	(24)
#define SAFESCREEN_X16	(SAFESCREEN_X<<4)

#define XOFFSET			(2048 - SCREEN_WIDTH / 2)
#define YOFFSET			(2048 - SCREEN_HEIGHT / 2)

#define CLUT_GSMEM		(SCREEN_WIDTH * SCREEN_HEIGHT * 2)
#define CLUT_GSMEMSIZE	(2048 * 1)
#define FONT_GSMEM		(CLUT_GSMEM + CLUT_GSMEMSIZE)
#define FONT_GSMEMSIZE	(2048 * 1)

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
}
#endif

#endif	// !__ENV_H__
