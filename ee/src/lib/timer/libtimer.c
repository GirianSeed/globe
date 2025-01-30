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
 *                         libtimer.c
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.0            2003/05/15      kono        First version
 */


#include <sys/types.h>
#include <string.h>
#include <eeregs.h>
#include <eekernel.h>
#include <sceerrno.h>
#include "libtimer.h"


// EEのBUSCLOCK速度を定義
#define BUSCLOCK		147456000UL
// 147.456MHz = 48kHz * 768 * 4
//              |        |    |
//              |        |    +--- EE(bus)/IOPの分周比
//              |        +-------- IOP/SPU2の分周比
//              +----------------- SPU2周波数


// バスクロック換算で時間を表現する
#define SEC2BUSCLK(_s)	(BUSCLOCK * (_s))
#define MSEC2BUSCLK(_m)	(BUSCLOCK * (_m) / (1000))
#define USEC2BUSCLK(_u)	(BUSCLOCK * (_u) / (1000*1000))

// 使用するタイマを切り替え
#if !defined(USE_TIMER0) && !defined(USE_TIMER1)
// デフォルトはタイマ#1を使用
#if !defined(USE_TIMER0)
#define USE_TIMER0		0
#endif	/* !defined(USE_TIMER0) */
#if !defined(USE_TIMER1)
#define USE_TIMER1		1
#endif	/* !defined(USE_TIMER1) */
#endif	/* !defined(USE_TIMER0) && defined(USE_TIMER1) */

#if USE_TIMER0
// タイマ#0を使用するとき
#define INTC_TIMER		INTC_TIM0
#define TIMER_BASE		T0_COUNT
#define DPUT_T_MODE		DPUT_T0_MODE
#define DGET_T_MODE		DGET_T0_MODE
#define DPUT_T_COUNT	DPUT_T0_COUNT
#define DGET_T_COUNT	DGET_T0_COUNT
#define DPUT_T_COMP		DPUT_T0_COMP
#define DGET_T_COMP		DGET_T0_COMP
#elif USE_TIMER1
// タイマ#1を使用するとき
#define INTC_TIMER		INTC_TIM1
#define TIMER_BASE		T1_COUNT
#define DPUT_T_MODE		DPUT_T1_MODE
#define DGET_T_MODE		DGET_T1_MODE
#define DPUT_T_COUNT	DPUT_T1_COUNT
#define DGET_T_COUNT	DGET_T1_COUNT
#define DPUT_T_COMP		DPUT_T1_COMP
#define DGET_T_COMP		DGET_T1_COMP
#else
#error	"define using counter which timer0 or timer1"
#endif


#define PRESCALE_L(_c, _m)	((_c)<<(((_m) & T_MODE_CLKS_M)<<2))
#define PRESCALE_R(_c, _m)	((_c)>>(((_m) & T_MODE_CLKS_M)<<2))


#undef	EI
#define	EI()			__asm__ volatile("sync.l; ei")


#define MODE_START			0x00000001			// カウンタ動作中
#define MODE_HANDLER		0x00000002			// ハンドラ登録済み

typedef struct COUNTER {
	struct COUNTER *pNext, *pPrev;				// 双方向リンク
	volatile u_int  uiKey;						// キーバリュー
	u_int  uiMode;								// カウンタの動作モード設定
	u_long ulBaseTime;							// ベース時刻
	u_long ulBaseCount;							// ベース時刻でのカウント値
	u_long ulSchedule;							// カウンタハンドラ呼び出し予定時刻
	u_long (*cbHandler)(int, u_long, u_long, void *, void *);
												// カウンタハンドラ関数
	void  *gp_value;							// ハンドラ呼び出し時の$gpレジスタ値
	void  *arg;									// ハンドラ呼び出し時の引数
	u_int pad[3];								// 64バイトにするためのパディング
} COUNTER __attribute__((aligned(64)));

#define COUNTER2ID(_p)		(int)((((u_int)(_p))<<4) | ((_p)->uiKey))
#define ID2COUNTER(_id)		(COUNTER *)((((u_int)(_id))>>10)<<6)
#define IS_VALIDID(_id)		(((int)(_id)>=0) && (((u_int)(_id) & 0x3FF)==((ID2COUNTER(_id))->uiKey)))


// プロトタイプ宣言
extern int _sceTimerAlarmInit(void);

static __inline__ u_long ReferHardCounter(void);
static __inline__ u_long ApplyOverflow(void);
static void InsertAlarm(COUNTER *pCounter);
static COUNTER *UnlinkAlarm(COUNTER *pCounter);
static void SetNextComp(u_long ulNowTime);
static int  cbTimerHandler(int ca, void *arg, void *addr);


static COUNTER g_CounterBuf[SCE_TIMER_MAX_SOFTCOUNTER] __attribute__((aligned(64)));

// タイマ用のワーク
static struct {
	volatile u_long ulHighCount;			// オーバーフロー回数
	int hid;								// タイマ割り込みハンドラのハンドラID
	volatile u_int uniqkey;					// カウンタID生成のためのユニーク値
	volatile u_int uiUsedCounters;			// 使用中のカウンタ数
	COUNTER *pFree;							// 空きカウンタのリンクリスト(単方向)
	COUNTER *pAlarm;						// アラームリストに接続されているカウンタのリンクリスト
	volatile int cbcounterid;				// コールバック処理呼び出し中のカウンタID
} g_Timer = {
	0UL,
	-1,
	1,
	0,
	NULL,
	NULL,
	-1
};


// タイマー仮想化ライブラリ初期化
int sceTimerInit(u_int uiMode)
{
	int hid;
	u_int mode;
	int intr;
	int i;

	if (g_Timer.hid>=0) {
		// タイマ割り込みハンドラが登録されていたとき
		return (SCE_ERROR_ENCODE(SCE_ERROR_PREFIX_TIMER, SCE_EINIT));
	}

	// ワークを初期化
	g_Timer.ulHighCount = 0;
	g_Timer.uiUsedCounters = 0;
	memset(&g_CounterBuf, 0x00, sizeof(g_CounterBuf));

	// リンクリストを初期化
	g_Timer.pFree = &g_CounterBuf[0];
	for (i=0; i<SCE_TIMER_MAX_SOFTCOUNTER; i++) {
		g_CounterBuf[i].pNext = &g_CounterBuf[i + 1];
	}
	g_CounterBuf[SCE_TIMER_MAX_SOFTCOUNTER - 1].pNext = NULL;

	// アラーム用ワークエリアを初期化
	_sceTimerAlarmInit();

	// INTC割り込みハンドラの登録
	hid = AddIntcHandler2(INTC_TIMER, cbTimerHandler, 0, NULL);
	if (hid<0) {
		// エラー
		return (SCE_ERROR_ENCODE(SCE_ERROR_PREFIX_TIMER, SCE_EINT_HANDLER));
	}
	g_Timer.hid = hid;

	// ハードウェアタイマの初期化
	intr = DI();									// 割り込み禁止
	// モードレジスタの値を計算
	mode = DGET_T_MODE();							// 現在のモードレジスタの値を得る
	mode = (mode & (u_int)~T_MODE_CLKS_M) | uiMode;	// プリスケール値を設定
	mode |= (T_MODE_OVFE_M | T_MODE_CMPE_M);		// オーバーフロー割り込み,比較値一致フラグを許可
	if (!(mode & T_MODE_CUE_M)) {
		// 割り込み停止中だったとき
		mode |= T_MODE_CUE_M;						// タイマを起動
		mode |= T_MODE_OVFF_M | T_MODE_EQUF_M;		// オーバーフローフラグ、比較値一致フラグをリセット
		DPUT_T_COUNT(0);							// カウンタ値をリセット
		DPUT_T_COMP(0xFFFF);						// コンペア値をリセット
	}
	DPUT_T_MODE(mode);								// タイマモード設定

	EnableIntc(INTC_TIMER);							// タイマ割り込み許可
	if (intr) EI();									// 割り込み許可状態の復帰
	return (SCE_OK);
}


// タイマー仮想化ライブラリ終了
int sceTimerEnd(void)
{
	int res;
	int intr;

	if (g_Timer.hid<0) {
		// タイマ割り込みハンドラは登録されていないとき
		return (SCE_ERROR_ENCODE(SCE_ERROR_PREFIX_TIMER, SCE_EINIT));
	} else if (g_Timer.uiUsedCounters>0) {
		// カウンタが1個でも使用中だったとき
		return (SCE_ERROR_ENCODE(SCE_ERROR_PREFIX_TIMER, SCE_EBUSY));
	}

	intr = DI();									// 割り込み禁止
	res = RemoveIntcHandler(INTC_TIMER, g_Timer.hid);
	if (res==0) {
		// 最後の割り込みだった場合
		DisableIntc(INTC_TIMER);					// タイマ割り込みを禁止

		// ハードウェアタイマ停止
		DPUT_T_MODE(T_MODE_OVFF_M | T_MODE_EQUF_M);
		DPUT_T_COUNT(0);
	}
	g_Timer.ulHighCount = 0;						// オーバーフロー回数をリセット
	g_Timer.hid = -1;								// 割り込みハンドラIDをリセット
	if (intr) EI();									// 割り込み許可状態の復帰
	return (SCE_OK);
}



// プリスケール値の取得
int sceTimerGetPreScaleFactor(void)
{
	if (g_Timer.hid<0) {
		// タイマ割り込みハンドラは登録されていないとき
		return (SCE_ERROR_ENCODE(SCE_ERROR_PREFIX_TIMER, SCE_EINIT));
	}
	return (DGET_T_MODE() & T_MODE_CLKS_M);			// プリスケール値を設定
}


// システムタイムの起動
int sceTimerStartSystemTime(void)
{
	u_int mode;
	int intr;
	intr = DI();							// 割り込み禁止
	mode = DGET_T_MODE();
	if (mode & T_MODE_CUE_M) {
		// すでに起動していたとき
		if (intr) EI();						// 割り込み許可状態の復帰
		return (1);
	}
	// ハードウェアタイマ起動
	mode = (mode & (u_int)~(T_MODE_OVFF_M | T_MODE_EQUF_M)) | T_MODE_CUE_M;
	DPUT_T_MODE(mode);

	SetNextComp(isceTimerGetSystemTime());	// 次回割り込み時刻を設定

	if (intr) EI();							// 割り込み許可状態の復帰
	return (0);
}


// システムタイムの停止
int sceTimerStopSystemTime(void)
{
	u_int mode;
	int intr;
	intr = DI();							// 割り込み禁止
	mode = DGET_T_MODE();
	if ((mode & T_MODE_CUE_M)==0) {
		// すでに停止していたとき
		if (intr) EI();						// 割り込み許可状態の復帰
		return (0);
	}
	// ハードウェアタイマ停止
	mode = mode & (u_int)~(T_MODE_OVFF_M | T_MODE_EQUF_M | T_MODE_CUE_M);
	DPUT_T_MODE(mode);
	if (intr) EI();							// 割り込み許可状態の復帰
	return (1);
}



/*
注意! この関数はlibtimer全体から参照されるシステム時刻を変更します。
libtimerは複数のプログラムで1本のハードウェアタイマを安全に共有するという
目的のライブラリですが、どこか1つのプログラムがむやみにlibtimerの全体から
参照されるシステム時刻を変更してしまうと、その目的を成就することができない
可能性がありますので、コメントアウトしてあります。

コメントアウトを外して使用する場合は、システム時刻の変更によってlibtimerの
提供されるカウンタ,アラーム,スレッド遅延に及ぼされる影響についてよくご理解
されてからご使用ください。たとえば、
sceTimerSetSystemTime(sceTimerGetSystemTime() - 147456000);
とするとシステム時刻を約1秒戻すことができますが、このとき全てのカウンタの
ハンドラの呼び出し時刻は1秒遅れることになります。ワンショットタイマは1秒
ずつ遅延し、インターバルタイマは1周期だけ1秒間延びしたサイクルが発生しま
す。

以上のように、libtimer全体に動作が及んでしまいますので、使用にはご注意い
ただきたく、コメントアウトしたコードでの提供とさせていただきました。

// システム時刻設定
void sceTimerSetSystemTime(u_long ulTime)
{
	u_int mode;
	int intr;
	u_long ulPrescale;
	intr = DI();							// 割り込み禁止

	mode = DGET_T_MODE();					// 現在のモード値を得る
	DPUT_T_MODE(mode);						// オーバーフローフラグ,比較フラグをリセット

	ulPrescale = PRESCALE_R(ulTime, mode);	// 分周モードに応じて右シフト
	DPUT_T_COUNT(ulPrescale & 0xFFFF);		// カウンタ値の下位16ビット指定
	g_Timer.ulHighCount = ulPrescale>>16;	// カウンタ値の上位ビット指定
	SetNextComp(ulTime);					// 次回割り込み時刻を設定
	if (intr) EI();							// 割り込み許可状態の復帰
	return;
}
*/



/*
精度を上げるためにはこの関数は高速に実行される必要があるので、インラインアセンブリ関数にしました。
等価コードは残してあります。
// ハードウェアタイマのカウント値取得
static __inline__ u_long ReferHardCounter(void)
{
	u_long ulHighCount, ulSystemTime;
	u_int  low, mode;

	low  = DGET_T_COUNT();					// COUNTレジスタを読み込み
	mode = DGET_T_MODE();					// MODEレジスタを読み込み
	ulHighCount = g_Timer.ulHighCount;
	if (mode & T_MODE_OVFF_M) {
		// オーバーフローが発生していたとき
		ulHighCount++;
		low = DGET_T_COUNT();						// COUNTレジスタ再読み込み
	}
	ulSystemTime = (ulHighCount<<16) | low;
	ulSystemTime = PRESCALE_L(ulSystemTime, mode);	// 分周比に応じて左シフト
	return (ulSystemTime);
}


// オーバーフローフラグをリセットし、現在時刻を生成
static __inline__ u_long ApplyOverflow(void)
{
	u_long ulSystemTime;
	u_int  mode;
	u_int  low;

	low  = DGET_T_COUNT();					// COUNTレジスタを読み込み
	mode = DGET_T_MODE();					// MODEレジスタを読み込み
	if (mode & T_MODE_OVFF_M) {
		// オーバーフローが発生していたとき
		g_Timer.ulHighCount++;
		DPUT_T_MODE(mode & (u_int)~T_MODE_EQUF_M);	// オーバーフローフラグを降ろす
		low = DGET_T_COUNT();						// COUNTレジスタ再読み込み
	}
	ulSystemTime = (g_Timer.ulHighCount<<16) | low;
	ulSystemTime = PRESCALE_L(ulSystemTime, mode);	// 分周比に応じて左シフト
	return (ulSystemTime);
}
*/


// ハードウェアタイマのカウント値取得
static __inline__ u_long ReferHardCounter(void)
{
	u_long ulSystemTime;
	__asm__ volatile (
		".set			push\n"
		".set			noreorder\n"
		"lwu			%0,  0(%1)\n"		// %0 = Tx_COUNT
		"lw				$8,  0x10(%1)\n"	// t0 = Tx_MODE
		"ld				$10, %2\n"			// t2 = g_Timer.ulHighCount
		"andi			$9,  $8, 0x0800\n"	// t1 = t0 & T_MODE_OVFF_M
		"beq			$9,  $0, 0f\n"		// t1==0のとき(オーバーフローしていないとき)
		"addiu			$9,  $0,  0x0002\n"	//t1 = 2
		"lwu			%0,  0(%1)\n"		// %0 = Tx_COUNT
		"daddiu			$10, $10, 1\n"		// t2 = t2 + 1
	"0:\n"
		"andi			$8,  $8,  0x0003\n"	// t0 = t0 & 3 = (Tx_MODE & T_MODE_CLKS_M)
		"dsll			$10, $10, 16\n"		// t2 = t2 << 16
		"movz			$9,  $0,  $8\n"		// t1 = (t0==0) ? 0 : t1 = ((Tx_MODE & T_MODE_CLKS_M)==0) ? 0 : 2
		"or				%0,  %0,  $10\n"	// %0 = %0 | t2
		"dsllv			$9,  $9,  $8\n"		// t1 = t1 << t0
		"dsll			%0,  %0,  $9\n"		// %0 = %0 << t1
		".set			pop\n"
		: "=&r"(ulSystemTime)
		: "r"(TIMER_BASE), "m"(g_Timer.ulHighCount)
		: "$8", "$9", "$10", "memory"
	);
	return (ulSystemTime);
}


// オーバーフローフラグをリセットし、現在時刻を生成
static __inline__ u_long ApplyOverflow(void)
{
	u_long ulSystemTime;
	__asm__ volatile (
		".set			push\n"
		".set			noreorder\n"
		"lwu			%0,  0(%1)\n"		// %0 = Tx_COUNT
		"lw				$8,  0x10(%1)\n"	// t0 = Tx_MODE
		"ld				$10, %2\n"			// t2 = g_Timer.ulHighCount
		"andi			$9,  $8, 0x0800\n"	// t1 = t0 & T_MODE_OVFF_M
		"beq			$9,  $0, 0f\n"		// t1==0のとき(オーバーフローしていないとき)
		"addiu			$9,  $0,  0x0002\n"	//t1 = 2
		"lwu			%0,  0(%1)\n"		// %0 = Tx_COUNT
		"addiu			$9,  $0,  0xFBFF\n"	// t1 = 0xFFFFFBFF = ~T_MODE_EQUF_M
		"daddiu			$10, $10, 1\n"		// t2 = t2 + 1
		"and			$8,  $8,  $9\n"		// t0 = t0 & t1 = t1 & ~T_MODE_EQUF_M
		"sd				$10, %2\n"			// g_Timer.ulHighCount = t2
		"sw				$8,  0x10(%1)\n"	// Tx_MODE = t0
		"addiu			$9,  $0,  0x0002\n"	// t1 = 2
	"0:\n"
		"andi			$8,  $8,  0x0003\n"	// t0 = t0 & 3 = (Tx_MODE & T_MODE_CLKS_M)
		"dsll			$10, $10, 16\n"		// t2 = t2 << 16
		"movz			$9,  $0,  $8\n"		// t1 = (t0==0) ? 0 : t1 = ((Tx_MODE & T_MODE_CLKS_M)==0) ? 0 : 2
		"or				%0,  %0,  $10\n"	// %0 = %0 | t2
		"dsllv			$9,  $9,  $8\n"		// t1 = t1 << t0
		"dsll			%0,  %0,  $9\n"		// %0 = %0 << t1
		".set			pop\n"
		: "=&r"(ulSystemTime)
		: "r"(TIMER_BASE), "m"(g_Timer.ulHighCount)
		: "$8", "$9", "$10", "memory"
	);
	return (ulSystemTime);
}


// 次回割り込み時刻をCOMPレジスタに設定と、一致フラグのリセット
static void SetNextComp(u_long ulNowTime)
{
	u_long a0, a1;
	long lDiff;
	u_int mode;
	COUNTER *pCounter = g_Timer.pAlarm;

	if (g_Timer.cbcounterid>=0) {
		// タイマ割り込み実行中だった場合、COMPレジスタの再設定はキャンセルします。
		return;
	}
	mode = DGET_T_MODE();
	if (pCounter==NULL) {
		// アラームが存在しないとき、オーバーフロー割り込みと同タイミングにしておく
		DPUT_T_COMP(0);
		DPUT_T_MODE(mode & (u_int)~T_MODE_OVFF_M);		// 一致フラグをリセットする
		return;
	}

	// キューの先頭と、その200usecのエントリを近接エントリを探す
	a0 = pCounter->ulSchedule + pCounter->ulBaseTime - pCounter->ulBaseCount;
	pCounter = pCounter->pNext;
	while (pCounter) {
		a1 = pCounter->ulSchedule + pCounter->ulBaseTime - pCounter->ulBaseCount;
		if (a1 < (a0 + USEC2BUSCLK(200))) {
			a0 = a1;
		} else {
			break;
		}
		pCounter = pCounter->pNext;
	}
	lDiff = a0 - ulNowTime;
	if (lDiff < (long)USEC2BUSCLK(200)) {
		// すでに予定時刻をオーバーしていたときとき、
		// 現時刻から200usecs先を指しておく
		DPUT_T_COMP(DGET_T_COUNT() + PRESCALE_R(USEC2BUSCLK(200), mode));
		DPUT_T_MODE(mode & (u_int)~T_MODE_OVFF_M);		// 一致フラグをリセットする
	} else {
		//FIXME: はるか未来だったら、DPUT_T_COMP(0)したほうがいいかも。
		DPUT_T_MODE(mode & (u_int)~T_MODE_OVFF_M);		// 一致フラグをリセットする
		DPUT_T_COMP(PRESCALE_R(a0, mode));
	}
	return;
}


// カウンタをアラームリストに追加
static void InsertAlarm(COUNTER *pCounter)
{
	COUNTER *pp = NULL;
	COUNTER *pn = g_Timer.pAlarm;
	u_long ulAbsCompare = pCounter->ulSchedule + pCounter->ulBaseTime - pCounter->ulBaseCount;
	while (pn) {
		u_long ulTarget = pn->ulSchedule + pn->ulBaseTime - pn->ulBaseCount;
		if (ulAbsCompare < ulTarget) {
			// 追加すべき位置が見つかった
			break;
		}
		pp = pn;
		pn = pn->pNext;
	}

	pCounter->pPrev = pp;
	pCounter->pNext = pn;
	if (pn) {
		// 途中のアラームに追加
		pn->pPrev      = pCounter;
	}
	if (pp) {
		// 途中のアラームに追加
		pp->pNext      = pCounter;
	} else {
		// 先頭のアラームとして追加
		g_Timer.pAlarm = pCounter;
	}
	return;
}


// 登録されていたカウンタを、アラームリストから外す
static COUNTER *UnlinkAlarm(COUNTER *pCounter)
{
	COUNTER *pNext = pCounter->pNext;
	if (pCounter->pPrev) {
		pCounter->pPrev->pNext = pNext;
	} else {
		g_Timer.pAlarm  = pNext;
	}
	if (pNext) {
		pNext->pPrev = pCounter->pPrev;
	}
	pCounter->pPrev     = NULL;
	return (pNext);
}


// タイマ割り込みハンドラ
static int cbTimerHandler(int ca, void *arg, void *addr)
{
	u_long ulAbsTime;
	u_int mode = DGET_T_MODE();

	(void)ca;
	(void)arg;

	if (mode & T_MODE_EQUF_M) {
		// 比較値一致割り込みだった場合
		COUNTER *pCounter = g_Timer.pAlarm;

		while (pCounter) {
			u_long ulTargetCompare, ulTargetTime;
			u_long ulNext;
			COUNTER *pNext;

			ulTargetCompare = pCounter->ulSchedule + pCounter->ulBaseTime - pCounter->ulBaseCount;

			// 現在時刻を取得, OVFFフラグをリセット
			ulAbsTime = ApplyOverflow();				// ulAbsClockを更新

			if (ulAbsTime < ulTargetCompare) {
				// もうアラームを発生させるべきカウンタがのこっていない
				break;
			}
			// いったんアラームリストから削除する
			pNext = UnlinkAlarm(pCounter);

/*
			// ここで再度ulAbsTimeを更新すればulTargetTimeが精密に計算できるが、
			// 調べなおすコストもタダではないので、ここはコメントアウトしておく。
			ulAbsTime = ReferHardCounter();				// ulAbsClockを更新
*/
			// 現在時刻をカウンタの基準時刻に調整
			ulTargetTime = ulAbsTime + pCounter->ulBaseCount - pCounter->ulBaseTime;

			// アラームハンドラの呼び出しを行う
			g_Timer.cbcounterid = COUNTER2ID(pCounter);
			__asm__ volatile ( "lw   $28, %0\n" : : "m"(pCounter->gp_value) : "memory" );
			ulNext = pCounter->cbHandler(COUNTER2ID(pCounter), pCounter->ulSchedule, ulTargetTime, pCounter->arg, addr);
			if (ulNext==0) {
				// このカウンタのコールバックは終了
				pCounter->uiMode &= ~MODE_HANDLER;		// ハンドラを削除
			} else if (ulNext==(u_long)-1) {
				// このカウンタごと削除
				pCounter->uiKey  = 0;					// カウンタIDを無効状態にする
				pCounter->uiMode = 0;					// カウンタモードを未使用状態に

				// 空きリンクリストの先頭に登録
				pCounter->pNext  = g_Timer.pFree;
				g_Timer.pFree    = pCounter;

				g_Timer.uiUsedCounters--;				// アロケート済みカウンタ数をデクリメント
			} else {
				// ulNextがあまりにも小さすぎる値だった場合には、切り上げを行う。
				if (ulNext < USEC2BUSCLK(100)) {
					ulNext = USEC2BUSCLK(100);
				}
				pCounter->ulSchedule += ulNext;			// 次の予定時刻を設定
				InsertAlarm(pCounter);					// アラームリストに追加
			}

			// 次のアラームリスト時刻を調べる
			pCounter = pNext;
		}
	}
	g_Timer.cbcounterid = -1;							// コールバック処理呼び出し中のカウンタIDをリセット

	ulAbsTime = ApplyOverflow();						// カウンタオーバーフロー処理
	SetNextComp(ulAbsTime);								// 次回割り込み時刻を設定
	ApplyOverflow();									// カウンタオーバーフロー処理
	ExitHandler();										// タイマ割り込みハンドラの終了
	return (0);
}



// システム時刻参照
u_long isceTimerGetSystemTime(void)
{
	return (ReferHardCounter());
}


// システム時刻参照
u_long sceTimerGetSystemTime(void)
{
	int intr;
	u_long ulSystemTime;
	intr = DI();							// 割り込み禁止
	ulSystemTime = ReferHardCounter();
	if (intr) EI();							// 割り込み許可状態の復帰
	return (ulSystemTime);
}


// カウンタをアロケート
int isceTimerAllocCounter(void)
{
	COUNTER *pCounter;

	pCounter = g_Timer.pFree;
	if (pCounter==NULL) {
		// カウンタがアロケートできなかったとき
		return (SCE_ERROR_ENCODE(SCE_ERROR_PREFIX_TIMER, SCE_ETIMER));
	}
	g_Timer.pFree   = pCounter->pNext;

	g_Timer.uiUsedCounters++;							// アロケート済みカウンタ数をインクリメント
	pCounter->uiMode      = 0;
	pCounter->cbHandler   = NULL;
	pCounter->ulBaseCount = 0;
	g_Timer.uniqkey++;
	pCounter->uiKey       = ((g_Timer.uniqkey<<1) & 0x3FE) | 1;
	return (COUNTER2ID(pCounter));
}


// カウンタをアロケート
int sceTimerAllocCounter(void)
{
	int intr;
	int id;

	intr = DI();										// 割り込み禁止
	id = isceTimerAllocCounter();						// カウンタをアロケート
	if (intr) EI();										// 割り込み許可状態の復帰
	return (id);
}


// カウンタを開放
int isceTimerFreeCounter(int id)
{
	COUNTER *pCounter = ID2COUNTER(id);					// カウンタIDよりCOUNTER*構造体のアドレスを取得
	if (!IS_VALIDID(id)) {
		// 正しくないカウンタIDが指定されていたとき
		return (SCE_ERROR_ENCODE(SCE_ERROR_PREFIX_TIMER, SCE_EID));
	} else if (g_Timer.cbcounterid==id) {
		// ハンドラ実行中カウンタIDが指定されていたとき
		return (SCE_ERROR_ENCODE(SCE_ERROR_PREFIX_TIMER, SCE_EBUSY));
	}

	// アラームハンドラが設定されていたとき
	if (pCounter->uiMode & MODE_HANDLER) {
		// ハンドラを削除
		UnlinkAlarm(pCounter);
	}
	pCounter->uiKey       = 0;							// カウンタIDを無効状態にする
	pCounter->uiMode      = 0;							// カウンタモードを未使用状態に

	// 空きリンクリストの先頭に登録
	pCounter->pNext       = g_Timer.pFree;
	g_Timer.pFree         = pCounter;

	g_Timer.uiUsedCounters--;							// アロケート済みカウンタ数をデクリメント
	return (SCE_OK);
}


// カウンタを開放
int sceTimerFreeCounter(int id)
{
	int res;
	int intr;
	intr = DI();										// 割り込み禁止
	res = isceTimerFreeCounter(id);						// カウンタを開放
	if (intr) EI();										// 割り込み許可状態の復帰
	return (res);
}


// 使用済み,未使用のカウンタ数を返す
int isceTimerGetUsedUnusedCounters(int *pnUsed, int *pnUnused)
{
	if (g_Timer.hid<0) {
		// タイマ割り込みハンドラは登録されていないとき
		return (SCE_ERROR_ENCODE(SCE_ERROR_PREFIX_TIMER, SCE_EINIT));
	}

	if (pnUsed!=NULL) {
		// 使用中のカウンタ個数
		*pnUsed = g_Timer.uiUsedCounters;
	}
	if (pnUnused!=NULL) {
		// 未使用のカウンタ個数
		*pnUnused = SCE_TIMER_MAX_SOFTCOUNTER - g_Timer.uiUsedCounters;
	}
	return (SCE_OK);
}


// 使用済み,未使用のカウンタ数を返す
int sceTimerGetUsedUnusedCounters(int *pnUsed, int *pnUnused)
{
	int res;
	int intr;
	intr = DI();										// 割り込み禁止
	res = isceTimerGetUsedUnusedCounters(pnUsed, pnUnused);	// 使用済み,未使用のカウンタ数を返す
	if (intr) EI();										// 割り込み許可状態の復帰
	return (res);
}


// カウンタを起動
int isceTimerStartCounter(int id)
{
	u_long ulNowTime;
	COUNTER *pCounter = ID2COUNTER(id);					// カウンタIDよりCOUNTER*構造体のアドレスを取得

	if (!IS_VALIDID(id)) {
		// 正しくないカウンタIDが指定されていたとき
		return (SCE_ERROR_ENCODE(SCE_ERROR_PREFIX_TIMER, SCE_EID));
	} else if (g_Timer.cbcounterid==id) {
		// ハンドラ実行中カウンタIDが指定されていたとき
		return (SCE_ERROR_ENCODE(SCE_ERROR_PREFIX_TIMER, SCE_EBUSY));
	}
	if (pCounter->uiMode & MODE_START) {
		// すでに起動中
		return (1);
	}

	ulNowTime = isceTimerGetSystemTime();				// 現在時刻を得る
	pCounter->ulBaseTime = ulNowTime;					// 現在時刻をベースタイムとして設定
	pCounter->uiMode |= MODE_START;						// カウンタ起動
	if (pCounter->uiMode & MODE_HANDLER) {
		// ハンドラが登録されているカウンタだった場合
		InsertAlarm(pCounter);							// このカウンタを割り込み予定リストに追加
		SetNextComp(ulNowTime);							// 次回割り込み時刻のスケジューリング
	}
	return (SCE_OK);
}


// カウンタを起動
int sceTimerStartCounter(int id)
{
	int res;
	int intr;
	intr = DI();										// 割り込み禁止
	res = isceTimerStartCounter(id);					// カウンタを起動
	if (intr) EI();										// 割り込み許可状態の復帰
	return (res);
}


// カウンタを停止
int isceTimerStopCounter(int id)
{
	u_long ulNowTime;
	COUNTER *pCounter = ID2COUNTER(id);					// カウンタIDよりCOUNTER*構造体のアドレスを取得

	if (!IS_VALIDID(id)) {
		// 正しくないカウンタIDが指定されていたとき
		return (SCE_ERROR_ENCODE(SCE_ERROR_PREFIX_TIMER, SCE_EID));
	} else if (g_Timer.cbcounterid==id) {
		// ハンドラ実行中カウンタIDが指定されていたとき
		return (SCE_ERROR_ENCODE(SCE_ERROR_PREFIX_TIMER, SCE_EBUSY));
	}
	if ((pCounter->uiMode & MODE_START)==0) {
		// すでに停止中
		return (0);
	}
	ulNowTime = isceTimerGetSystemTime();				// 現在のシステム時刻を取得
	pCounter->ulBaseCount += (ulNowTime - pCounter->ulBaseTime);
	pCounter->uiMode &= ~MODE_START;					// カウンタ停止
	if (pCounter->uiMode & MODE_HANDLER) {
		UnlinkAlarm(pCounter);							// アラームリストからも削除
		SetNextComp(ulNowTime);							// 次回割り込み時刻を設定
	}
	return (1);
}


// カウンタを停止
int sceTimerStopCounter(int id)
{
	int res;
	int intr;
	intr = DI();										// 割り込み禁止
	res = isceTimerStopCounter(id);						// カウンタを停止
	if (intr) EI();										// 割り込み許可状態の復帰
	return (res);
}


// カウンタにカウント値設定
u_long sceTimerSetCount(int id, u_long ulNewCount)
{
	int intr;
	u_long ulOldCount;
	COUNTER *pCounter = ID2COUNTER(id);					// カウンタIDよりCOUNTER*構造体のアドレスを取得

	intr = DI();										// 割り込み禁止
	if (!IS_VALIDID(id) || g_Timer.cbcounterid==id) {
		// 正しくないカウンタIDが指定されていたとき
		if (intr) EI();									// 割り込み許可状態の復帰
		return ((u_long)-1);
	}

	ulOldCount  = pCounter->ulBaseCount;				// 現在のカウント値を得る
	if (pCounter->uiMode & MODE_START) {
		// カウンタ動作中だったとき、ベース時刻ごと新しいカウンタ値に補正
		u_long ulNowTime = isceTimerGetSystemTime();	// 現在時刻を得る
		ulOldCount += (ulNowTime - pCounter->ulBaseTime);	// ベースタイムからの経過時間を加算
		pCounter->ulBaseTime  = ulNowTime;				// 新しいベース時刻を設定
		pCounter->ulBaseCount = ulNewCount;				// 新しいカウント値を設定
	} else {
		pCounter->ulBaseCount = ulNewCount;				// 新しいカウント値を設定
	}
	if (intr) EI();										// 割り込み許可状態の復帰
	return (ulOldCount);
}


// カウンタのベース時刻を取得
u_long isceTimerGetBaseTime(int id)
{
	u_long ulBaseTime;
	COUNTER *pCounter = ID2COUNTER(id);					// カウンタIDよりCOUNTER*構造体のアドレスを取得
	if (!IS_VALIDID(id)) {
		// 正しくないカウンタIDが指定されていたとき
		return ((u_long)-1);
	}
	if (pCounter->uiMode & MODE_START) {
		// カウンタ動作中だったとき
		ulBaseTime = pCounter->ulBaseTime - pCounter->ulBaseCount;
	} else {
		// カウンタが停止中だったとき
		ulBaseTime = 0;
	}
	return (ulBaseTime);
}


// カウンタのベース時刻を取得
u_long sceTimerGetBaseTime(int id)
{
	int intr;
	u_long ulBaseTime;
	intr = DI();										// 割り込み禁止
	ulBaseTime = isceTimerGetBaseTime(id);				// カウンタのベースタイム値を得る
	if (intr) EI();										// 割り込み許可状態の復帰
	return (ulBaseTime);
}


// カウンタの現在値を取得
u_long isceTimerGetCount(int id)
{
	u_long ulCount;
	COUNTER *pCounter = ID2COUNTER(id);					// カウンタIDよりCOUNTER*構造体のアドレスを取得

	if (!IS_VALIDID(id)) {
		// 正しくないカウンタIDが指定されていたとき
		return ((u_long)-1);
	}
	ulCount = pCounter->ulBaseCount;					// 現在のカウント値を得る
	if (pCounter->uiMode & MODE_START) {
		// カウンタ動作中だったとき
		u_long ulNowTime = isceTimerGetSystemTime();	// 現在時刻を得る
		ulCount += (ulNowTime - pCounter->ulBaseTime);	// ベース時刻からの経過時間を加算
	}
	return (ulCount);
}


// カウンタの現在値を取得
u_long sceTimerGetCount(int id)
{
	int intr;
	u_long ulCount;
	intr = DI();										// 割り込み禁止
	ulCount = isceTimerGetCount(id);					// 現在のカウント値を得る
	if (intr) EI();										// 割り込み許可状態の復帰
	return (ulCount);
}


// カウンタにハンドラを設定
int isceTimerSetHandler(int id, u_long ulSchedule, u_long (*cbHandler)(int, u_long, u_long, void *, void *), void *arg)
{
	u_long ulNowTime;
	COUNTER *pCounter = ID2COUNTER(id);					// カウンタIDよりCOUNTER*構造体のアドレスを取得

	if (!IS_VALIDID(id)) {
		// 正しくないカウンタIDが指定されていたとき
		return (SCE_ERROR_ENCODE(SCE_ERROR_PREFIX_TIMER, SCE_EID));
	} else if (g_Timer.cbcounterid==id) {
		// ハンドラ実行中カウンタIDが指定されていたとき
		return (SCE_ERROR_ENCODE(SCE_ERROR_PREFIX_TIMER, SCE_EBUSY));
	}

	if (pCounter->uiMode & MODE_HANDLER) {
		// すでに登録されていたら、いったんアラームリストから削除する
		UnlinkAlarm(pCounter);
	}
	pCounter->cbHandler = cbHandler;					// ハンドラ関数を設定
	if (cbHandler==NULL) {
		pCounter->uiMode   &= ~MODE_HANDLER;			// ハンドラ登録済みフラグを降ろす
	} else {
		// ハンドラ関数ポインタが非NULLだったとき
		pCounter->uiMode   |= MODE_HANDLER;				// ハンドラ登録済みフラグを立てる
		pCounter->ulSchedule = ulSchedule;
		__asm__ volatile ( "sw   $28, %0\n" : : "m"(pCounter->gp_value) : "memory" );
		pCounter->arg       = arg;
		if (pCounter->uiMode & MODE_START) {
			// 動作中のカウンタにハンドラを設定したとき、アラームリストに追加
			InsertAlarm(pCounter);
		}
	}
	ulNowTime = isceTimerGetSystemTime();				// 現在時刻を得る
	SetNextComp(ulNowTime);								// 次回割り込み時刻を設定
	return (SCE_OK);
}


// カウンタにハンドラを設定
int sceTimerSetHandler(int id, u_long ulSchedule, u_long (*cbHandler)(int, u_long, u_long, void *, void *), void *arg)
{
	int res;
	int intr;
	intr = DI();										// 割り込み禁止
	res = isceTimerSetHandler(id, ulSchedule, cbHandler, arg);
														// カウンタにハンドラを設定
	if (intr) EI();										// 割り込み許可状態の復帰
	return (res);
}


// BUSCLOCK値(147.456MHz)を実時間に変換
void sceTimerBusClock2USec(u_long ulClock, u_int *puiSec, u_int *puiUsec)
{
	u_int uiSec = ulClock / BUSCLOCK;
	if (puiSec) {
		*puiSec = uiSec;
	}
	if (puiUsec) {
		*puiUsec = (ulClock - uiSec*BUSCLOCK) * 1000 * 1000 / BUSCLOCK;
	}
	return;
}


// 実時間をBUSCLOCK値(147.456MHz)に変換
u_long sceTimerUSec2BusClock(u_int uiSec, u_int uiUsec)
{
	u_long ulClock = (uiSec * BUSCLOCK) + (uiUsec * (u_long)BUSCLOCK / (1000 * 1000));
	return (ulClock);
}


// BUSCLOCK値(147.456MHz)を周波数に変換
float sceTimerBusClock2Freq(u_long ulClock)
{
	float fFreq = (float)BUSCLOCK / (float)ulClock;
	return (fFreq);
}


// 周波数をBUSCLOCK値(147.456MHz)に変換
u_long sceTimerFreq2BusClock(float fFreq)
{
	u_long ulClock = (float)BUSCLOCK / fFreq;
	return (ulClock);
}

