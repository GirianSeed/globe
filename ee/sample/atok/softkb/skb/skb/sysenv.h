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

#ifndef __SYSENV_H__
#define __SYSENV_H__

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
extern "C" {
#endif

#include "shareddef.h"

int Sysenv_GetLang(void);						// 言語取得
#if 1//SKB_EX SysEnvPath
int Sysenv_ReadSystemIni(u_short *pSystemIni, char*fsName);	// SYSTEM.INIを読む
#else
int Sysenv_ReadSystemIni(u_short *pSystemIni);	// SYSTEM.INIを読む
#endif

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
}
#endif

#endif	// !__SYSENV_H__
