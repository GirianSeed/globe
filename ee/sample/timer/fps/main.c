/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/*
 *                      Timer Library Sample
 *
 *                      - <libtimer fps> -
 *
 *                         Version 1.00
 *                           Shift-JIS
 *
 *      Copyright (C) 2003 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                          <main.c>
 *                   <measuring frames / sec>
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.00           May,15,2003     kono        first version
 */

#include <sys/types.h>
#include <stdio.h>
#include <malloc.h>

#include <eekernel.h>
#include <sifdev.h>
#include <libgraph.h>
#include <libtimer.h>

#if USE_ERX

#include <liberx.h>
#include <libc_export.h>
#include <libgcc_export.h>

#define LIBERX_THREAD_PRIORITY		10
#define LIBERX_MEMORY_SIZE			(8*1024*1024)
#define LIBERX_MAX_MODULES			32

#define LIBTIMER_ERX	"host0:/usr/local/sce/ee/modules/libtimer.erx"

#endif	/* USE_ERX */


int main(int argc, const char *const argv[]);

void MeasureV(void);
void MeasureH(void);
static void cbAlarm(int id, u_short t, void *param);


int main(int argc, const char *const argv[])
{
	(void)argc;
	(void)argv;

	// sceGsResetGraph()を呼ぶ前にsceSifRebootIop()を呼びます。
	sceSifRebootIop("host0:/usr/local/sce/iop/modules/" IOP_IMAGE_file);
	while (!sceSifSyncIop());					// IOPが再起動するまで待機

#if USE_ERX
	{
		static const char *const apszErxFiles[] = {
			LIBTIMER_ERX
		};
		void *pBuf;
		int modid, res;
		int i;

		// liberx内のsysmemライブラリに使わせるメモリの確保
		pBuf = memalign(256, LIBERX_MEMORY_SIZE);
		res = sceErxInit(LIBERX_THREAD_PRIORITY, pBuf, LIBERX_MEMORY_SIZE, LIBERX_MAX_MODULES);
		printf("sceErxInit: %d\n", res);

		// ダイナミックリンクでロードされるERXのために
		// 本体ELF部分からエクスポートされる関数をliberxに登録

		// libgcc関係の関数をエクスポート
		sceErxRegisterLibraryEntries(sceLibgccCommonGetErxEntries());	// libgcc 算術補助関数をエクスポート

		sceErxRegisterLibraryEntries(sceLibcStringGetErxEntries());	// string関係の関数をエクスポート

		for (i=0; i<(int)(sizeof(apszErxFiles)/sizeof(const char *)); i++) {
			// ライブラリのERXモジュールを読み込み
			do {
				modid = sceErxLoadModuleFile(apszErxFiles[i], SCE_ERX_SMEM_LOW, NULL);
			} while (modid<0);

			// 読み込んだERXモジュールを起動
			res = sceErxStartModule(modid, apszErxFiles[i], 0, "", &res);
			if (res < 0) {
				// モジュールの起動に失敗
				printf("module '%s' can not start. (error=%x)\n", apszErxFiles[i], res);
				printf("maybe dependeg library is not found...\n");
				while (1);
			}
			printf("module '%s' loaded. (modid=%d)\n", apszErxFiles[i], modid);
		}
	}
#endif

	// タイマライブラリを初期化
	sceTimerInit(SCE_TIMER_PRESCALE1);

	// 各画面モードごとに、VSYNC,HSYNCの時間を計測してみます
	printf("NTSC interlace ====\n");
	sceGsResetGraph(0, SCE_GS_INTERLACE, SCE_GS_NTSC, SCE_GS_FRAME);
	MeasureV();
	MeasureH();

	printf("NTSC non-interlace ===\n");
	sceGsResetGraph(0, SCE_GS_NOINTERLACE, SCE_GS_NTSC, SCE_GS_FRAME);
	MeasureV();
	MeasureH();

	printf("NTSC progressive ====\n");
	sceGsResetGraph(0, SCE_GS_NOINTERLACE, SCE_GS_DTV480P, SCE_GS_FRAME);
	MeasureV();
	MeasureH();

	printf("PAL interlace ===\n");
	sceGsResetGraph(0, SCE_GS_INTERLACE, SCE_GS_PAL, SCE_GS_FRAME);
	MeasureV();
	MeasureH();

	printf("PAL non-interlace ===\n");
	sceGsResetGraph(0, SCE_GS_NOINTERLACE, SCE_GS_PAL, SCE_GS_FRAME);
	MeasureV();
	MeasureH();
	return (0);
}


// 100Vの長さを5回はかってみる
void MeasureV(void)
{
	int i;
	int cid;

	cid = sceTimerAllocCounter();
	sceTimerStartCounter(cid);

	for (i=0; i<5; i++) {
		u_long ul0, ul1, ulClock;
		u_int uiSec, uiUsec;
		int j;
		float fFreq;

		// ここから100Vの時間を計測します
		sceGsSyncV(0);
		ul0 = sceTimerGetCount(cid);
		for (j=0; j<100; j++) {
			sceGsSyncV(0);
			ul1 = sceTimerGetCount(cid);
		}
		ulClock = ul1 - ul0;

		// BUSCLOCK単位を実時間(usec),周波数に変換
		sceTimerBusClock2USec(ulClock, &uiSec, &uiUsec);
		fFreq = sceTimerBusClock2Freq(ulClock);

		printf("100V = %10ld BUSCLOCK(%d.%06dsec) ----> about %.2fHz\n", ulClock, uiSec, uiUsec, fFreq*100.0f);
	}
	sceTimerFreeCounter(cid);
	return;
}


// 100Hの長さを5回はかってみる
void MeasureH(void)
{
	int i;
	int cid;
	int sid;
	struct SemaParam sp;

	// セマフォを作成する
	sp.initCount = 0;
	sp.maxCount  = 1;
	sid = CreateSema(&sp);

	cid = sceTimerAllocCounter();
	sceTimerStartCounter(cid);

	for (i=0; i<5; i++) {
		u_long ul0, ul1, ulClock;
		u_int uiSec, uiUsec;
		float fFreq;

		// SetAlarm()～sceTimerGetCount()の誤差を小さくするため、
		// ダミーで1回アラーム待ちします。
		SetAlarm(10, cbAlarm, (void *)sid);
		WaitSema(sid);

		// ここから100Hの時間を計測します
		SetAlarm(100, cbAlarm, (void *)sid);
		ul0 = sceTimerGetCount(cid);
		WaitSema(sid);
		ul1 = sceTimerGetCount(cid);
		ulClock = ul1 - ul0;

		// BUSCLOCK単位を実時間(usec),周波数に変換
		sceTimerBusClock2USec(ulClock, &uiSec, &uiUsec);
		fFreq = sceTimerBusClock2Freq(ulClock);

		printf("100H = %10ld BUSCLOCK(%d.%06dsec) ----> about %.2fkHz\n", ulClock, uiSec, uiUsec, fFreq * 100.0f / 1000.0f);
	}
	sceTimerFreeCounter(cid);

	// セマフォを削除
	DeleteSema(sid);
	return;
}


// SetAlarmハンドラ
static void cbAlarm(int id, u_short t, void *param)
{
	(void)id;
	(void)t;
	iSignalSema((int)param);
	ExitHandler();
	return;
}


