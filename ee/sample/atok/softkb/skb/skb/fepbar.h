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
#ifndef __FEPBAR_H__
#define __FEPBAR_H__

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
extern "C" {
#endif

#include <libfep.h>

int  Fepbar_Init(gImport_t*);
void Fepbar_Destroy(void);
void Fepbar_Run(sceVif1Packet *pk);
void Fepbar_Move(int x, int y);
void Fepbar_SetPadData(u_long paddata);
void Fepbar_CreateModeType(const u_int *pTypes, int typeNum);
void Fepbar_ClearModeType(void);
void Fepbar_SetModeType(int type);
#if 1//SKB_EX Fepbar_GetNowMode
int  Fepbar_GetNowMode(void);
#endif
#if 1//SKB_EX Fepbar_NextMode
void Fepbar_NextMode(void);
#endif


#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
}
#endif

#endif	// !__FEPBAR_H__
