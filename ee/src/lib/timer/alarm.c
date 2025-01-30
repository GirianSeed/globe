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
 *                         alarm.c
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.0            2003/05/15      kono        First version
 */


#include <sys/types.h>
#include <eekernel.h>
#include <sceerrno.h>
#include "libtimer.h"


typedef struct ALARM {
	struct ALARM *pNext;						// 単方向リンクリスト
	volatile int counterid;						// カウンタID
	u_long (*cbHandler)(int, u_long, u_long, void *, void *);	// アラームハンドラの関数ポインタ
	void *arg;									// アラームハンドラに渡す引数
} ALARM __attribute__((aligned(16)));

#define ALARM2ID(_p, _u)	(int)(((((u_int)(_p))<<4) | ((_u) & 0xFE) | 1))
#define ID2ALARM(_p)		(ALARM *)((((u_int)(_p))>>8)<<4)
#define IS_VALIDID(_id)		(((int)(_id)>=0) && (((((u_int)(_id)) ^ ((ID2ALARM(_id))->counterid)) & 0xFF)==0))


// プロトタイプ宣言
int _sceTimerAlarmInit(void);
static __inline__ ALARM *AllocAlarm(void);
static __inline__ void FreeAlarm(ALARM *pAlarm);
static u_long AlarmHandler(int counterid, u_long ulSchedule, u_long ulActual, void *arg, void *addr);

static ALARM g_AlarmBuf[SCE_TIMER_MAX_SOFTALARM] __attribute__((aligned(16)));
static ALARM *g_pFreeAlarm;


// アラーム用ワークエリアのリンクリストを初期化
int _sceTimerAlarmInit(void)
{
	int i;
	g_pFreeAlarm = &g_AlarmBuf[0];
	for (i=0; i<SCE_TIMER_MAX_SOFTALARM; i++) {
		g_AlarmBuf[i].pNext = &g_AlarmBuf[i + 1];
	}
	g_AlarmBuf[SCE_TIMER_MAX_SOFTALARM - 1].pNext = NULL;
	return (0);
}


// アラーム用ワークを確保
static __inline__ ALARM *AllocAlarm(void)
{
	ALARM *pAlarm;
	pAlarm = g_pFreeAlarm;
	if (pAlarm!=NULL) {
		g_pFreeAlarm = pAlarm->pNext;
	}
	return (pAlarm);
}


// アラーム用ワークを解放
static __inline__ void FreeAlarm(ALARM *pAlarm)
{
	pAlarm->pNext = g_pFreeAlarm;
	pAlarm->counterid = 0;					// カウンタIDを無効状態にする
	g_pFreeAlarm  = pAlarm;
	return;
}


// アラームのためのカウンタ一致ハンドラ
// ここから本来のアラームハンドラを呼び出し
static u_long AlarmHandler(int counterid, u_long ulSchedule, u_long ulActual, void *arg, void *addr)
{
	ALARM *pAlarm = (ALARM *)arg;
	u_long ulNext = pAlarm->cbHandler(ALARM2ID(pAlarm, counterid), ulSchedule, ulActual, pAlarm->arg, addr);

	if (ulNext==0) {
		// アラーム終了時、カウンタを削除する
		FreeAlarm(pAlarm);
		return ((u_long)-1);
	}
	return (ulNext);
}


// アラームを設定
int sceTimerSetAlarm(u_long ulClock, u_long (*cbHandler)(int, u_long, u_long, void *, void *), void *arg)
{
	int intr;
	ALARM *pAlarm;
	int counterid;

	if (cbHandler==NULL) {
		// ハンドラ関数としてNULLポインタが渡された
		return (SCE_ERROR_ENCODE(SCE_ERROR_PREFIX_TIMER, SCE_EINVAL));
	}

	intr = DI();							// 割り込み禁止

	// アラーム用ワークエリアを確保
	pAlarm = AllocAlarm();
	if (pAlarm==NULL) {
		// アラームワークのアロケートに失敗
		if (intr) EI();
		return (SCE_ERROR_ENCODE(SCE_ERROR_PREFIX_TIMER, SCE_ETIMER));
	}

	// カウンタを確保
	counterid = sceTimerAllocCounter();
	if (counterid<0) {
		// カウンタのアロケートに失敗
		FreeAlarm(pAlarm);					// アラームのワークを解放
		if (intr) EI();
		return (counterid);
	}

	// カウント値一致ハンドラを登録
	pAlarm->counterid = counterid;
	pAlarm->cbHandler = cbHandler;
	pAlarm->arg       = arg;
	sceTimerSetHandler(counterid, ulClock, AlarmHandler, pAlarm);

	sceTimerStartCounter(counterid);		// カウンタを起動

	if (intr) EI();							// 割り込み許可状態の復帰
	return (ALARM2ID(pAlarm, counterid));
}


// アラームを設定
int isceTimerSetAlarm(u_long ulClock, u_long (*cbHandler)(int, u_long, u_long, void *, void *), void *arg)
{
	ALARM *pAlarm;
	int counterid;

	if (cbHandler==NULL) {
		// ハンドラ関数としてNULLポインタが渡された
		return (SCE_ERROR_ENCODE(SCE_ERROR_PREFIX_TIMER, SCE_EINVAL));
	}

	// アラーム用ワークエリアを確保
	pAlarm = AllocAlarm();
	if (pAlarm==NULL) {
		// アラームワークのアロケートに失敗
		return (SCE_ERROR_ENCODE(SCE_ERROR_PREFIX_TIMER, SCE_ETIMER));
	}

	// カウンタを確保
	counterid = isceTimerAllocCounter();
	if (counterid<0) {
		// カウンタのアロケートに失敗
		FreeAlarm(pAlarm);					// アラームのワークを解放
		return (counterid);
	}

	// カウント値一致ハンドラを登録
	pAlarm->counterid = counterid;
	pAlarm->cbHandler = cbHandler;
	pAlarm->arg       = arg;
	isceTimerSetHandler(counterid, ulClock, AlarmHandler, pAlarm);

	isceTimerStartCounter(counterid);		// カウンタを起動
	return (ALARM2ID(pAlarm, counterid));
}


// アラームを解放
int sceTimerReleaseAlarm(int id)
{
	int intr;
	ALARM *pAlarm = ID2ALARM(id);

	intr = DI();							// 割り込み禁止
	if (!IS_VALIDID(id)) {
		// 正しくないアラームIDだった場合
		if (intr) EI();						// 割り込み許可状態の復帰
		return (SCE_ERROR_ENCODE(SCE_ERROR_PREFIX_TIMER, SCE_EID));
	}

	sceTimerFreeCounter(pAlarm->counterid);	// カウンタを開放
	FreeAlarm(pAlarm);						// アラーム用ワークエリアを開放
	if (intr) EI();							// 割り込み許可状態の復帰
	return (SCE_OK);
}


// アラームを解放
int isceTimerReleaseAlarm(int id)
{
	ALARM *pAlarm = ID2ALARM(id);
	int res;
	if (!IS_VALIDID(id)) {
		// 正しくないアラームIDだった場合
		return (SCE_ERROR_ENCODE(SCE_ERROR_PREFIX_TIMER, SCE_EID));
	}

	res = isceTimerFreeCounter(pAlarm->counterid);	// カウンタを開放
	if (res==SCE_OK) {
		FreeAlarm(pAlarm);					// アラーム用ワークエリアを開放
	}
	return (res);
}

