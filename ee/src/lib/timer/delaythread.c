/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/* 
 *                       Timer Library
 *                        Version 1.0
 *                         Shift-JIS
 *
 *      Copyright (C) 2001-2003 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                         delaythread.c
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.0            2003/05/15      kono        First version
 */


#include <sys/types.h>
#include <eekernel.h>
#include <sceerrno.h>
#include "libtimer.h"


// プロトタイプ宣言
static u_long _sceTimerThreadWakeup(int id, u_long ulSchedule, u_long ulActual, void *arg, void *addr);


// スレッド遅延を行う
int sceTimerDelayThread(u_int uiUsec)
{
	struct SemaParam sp;
	int sid;
	int aid;
	u_long ulClock;
	int status;

	// 割り込み禁止状態かどうかチェック
	__asm__ volatile (
		"mfc0     %0, $12"
		: "=r"(status)
	);
	if (!(status & 0x00010000)) {
		// 割り込み禁止だったとき
		return (SCE_ERROR_ENCODE(SCE_ERROR_PREFIX_TIMER, SCE_ECPUDI));
	}

	// テンポラリにイベント通知用セマフォを作成する
	sp.initCount = 0;
	sp.maxCount  = 1;
	sp.option    = (u_int)"SceTimerDelayThread";	// セマフォに名前をつける
	sid = CreateSema(&sp);
	if (sid < 0) {
		// セマフォが作成できなかったとき
		return (SCE_ERROR_ENCODE(SCE_ERROR_PREFIX_TIMER, SCE_ESEMAPHORE));
	}

	// usec単位からBUSCLOCK単位に変換
	ulClock = sceTimerUSec2BusClock(0, uiUsec);

	// アラームを登録
	aid = sceTimerSetAlarm(ulClock, _sceTimerThreadWakeup, (void *)sid);
	if (aid < 0) {
		DeleteSema(sid);
		return (aid);
	}

	// アラームからセマフォが発火されるのを待機
	WaitSema(sid);

	// テンポラリに作成したセマフォを削除
	DeleteSema(sid);
	return (SCE_OK);
}


// スレッド遅延用のアラームハンドラ
static u_long _sceTimerThreadWakeup(int id, u_long ulSchedule, u_long ulActual, void *arg, void *addr)
{
	(void)id;
	(void)ulSchedule;
	(void)ulActual;
	(void)addr;

	// arg経由で渡されるセマフォを発火
	iSignalSema((int)arg);
	ExitHandler();

	// 戻り値として0を返すことで、アラームを削除
	return (0);
}

