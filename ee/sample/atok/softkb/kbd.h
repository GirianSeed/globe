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

#ifndef __KBD_H__
#define __KBD_H__


#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
extern "C" {
#endif
#include <libusbkb.h>

// リピートウェイト
#define ENV_KEYREPW_SHORT	(0)  //system.ini 準拠
#define ENV_KEYREPW_MIDDLE	(1)
#define ENV_KEYREPW_LONG	(2)
#define KEYREPW_SHORT		(20)
#define KEYREPW_MIDDLE		(30)
#define KEYREPW_LONG		(45)
// リピート開始
#define ENV_KEYREPS_FAST	(0)  //system.ini 準拠
#define ENV_KEYREPS_MIDDLE	(1)
#define ENV_KEYREPS_SLOW	(2)
#define KEYREPS_FAST		(2)
#define KEYREPS_MIDDLE		(3)
#define KEYREPS_SLOW		(5)

int  Kbd_Init( int,int,int,int );
void Kbd_Exit(void);
int  Kbd_Main(USBKBDATA_t *pKbData);
void KbdIgnoreSeveralMessage(void);
int KbdCheckCanPause(void);
int KbdGetNumLockState(void);

void Kbd_SetConfig();


#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
}
#endif


#endif	// __KBD_H__
