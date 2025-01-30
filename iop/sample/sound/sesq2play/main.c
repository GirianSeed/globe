/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0.2
 */
/*
 *                     I/O Processor Library
 *                          Version 3.0
 *                           Shift-JIS
 *
 *      Copyright (C) 2004 Sony Computer Entertainment Inc.
 *                       All Rights Reserved.
 *
 *                         sesq2ply - main.c
 *                        SE Sequence Sample
 *
 *     Version   Date          Design   Log
 *  --------------------------------------------------------------------
 */

#include <kernel.h>
#include <stdio.h>
#include <libsd.h>
#include <csl.h>
#include <cslse.h>
#include <modsesq2.h>
#include <modhsyn.h>
#include <sceerrno.h>

#define PROCESS_OK        0
#define PROCESS_ERROR   (-1)

#define BASE_priority   60

#define IOP_SQADDR    0x101000
#define IOP_HDADDR    0x120000
#define IOP_BDADDR    0x150000
#define BD_SIZE       180000
#define SPU_TOPADDR   0x5010

#define TIMER_DIVISION   1000 * 1000 / 240 /* ATick */

typedef struct {
	int   data;
	int   ATickThID;
} thData;
thData              myThData;

sceHSyn_VoiceStat   vstat;
sceCslIdMonitor     idCtx;  /* IDモニター構造体 */
volatile int        semaID;

/*------------------------------------------------------------------------------
各MuduleContext の設定
------------------------------------------------------------------------------*/
sceCslCtx        sesqCtx;
sceCslBuffGrp    sesqBuffGrp[2];
sceCslBuffCtx    sesqInBuffCtx[2];
sceCslBuffCtx    sesqOutBuffCtx[1];
sceSESq2Env      sesqSESqEnv[1];

u_char           sesqOutBuff[sizeof(sceCslSeStream) + 2048];

sceCslCtx        hsynCtx;
sceCslBuffGrp    hsynBuffGrp[1];
sceCslBuffCtx    hsynBuffCtx[2];
sceHSynEnv       hsynEnv[1];

static void main_setModuleContext(void)
{
	sesqCtx.buffGrpNum = 2;
	sesqCtx.buffGrp    = sesqBuffGrp;
	sesqCtx.conf       = NULL;
	sesqCtx.callBack   = NULL;
	sesqCtx.extmod     = NULL;

		sesqBuffGrp[0].buffNum = 2;
		sesqBuffGrp[0].buffCtx = sesqInBuffCtx;

		sesqInBuffCtx[0].sema = 0;
		sesqInBuffCtx[0].buff = NULL; 
		sesqInBuffCtx[1].sema = 0;
			sesqInBuffCtx[1].buff = sesqSESqEnv;

		sesqBuffGrp[1].buffNum = 1;
		sesqBuffGrp[1].buffCtx = sesqOutBuffCtx;

		sesqOutBuffCtx[0].sema = 0;
		sesqOutBuffCtx[0].buff = &sesqOutBuff;
		
		((sceCslSeStream*)sesqOutBuff)->buffsize 
		= sizeof (sceCslSeStream) + 2048;

			hsynBuffCtx[0].sema = 0;
			hsynBuffCtx[0].buff = &sesqOutBuff;
			hsynBuffCtx[1].sema = 0;
			hsynBuffCtx[1].buff = hsynEnv;

		hsynBuffGrp[0].buffNum = 2;
		hsynBuffGrp[0].buffCtx = hsynBuffCtx;

	hsynCtx.buffGrpNum = 1;
	hsynCtx.buffGrp    = hsynBuffGrp;
	hsynCtx.conf       = NULL;
	hsynCtx.callBack   = NULL;
	hsynCtx.extmod     = NULL;

	((sceCslSeStream*)sesqOutBuff)->validsize = 0;
}

/*------------------------------------------------------------------------------
SESequencer側設定
------------------------------------------------------------------------------*/
static void main_setupSESqCtx(void)
{
	int   ret;

	/* seCslCtx(SESQ2)を初期化 */
	ret = sceSESq2Init(&sesqCtx, TIMER_DIVISION);

	/* SQ を入力ポート 0 に登録 */
	sesqInBuffCtx[0].buff = (void *)IOP_SQADDR;
	sceSESq2Load(&sesqCtx, 0);

	sesqSESqEnv[0].defaultOutPort   = 0;
	sesqSESqEnv[0].outPort [0].port = sceSESq2Env_NoOutPortAssignment;
	sesqSESqEnv[0].masterVolume     = sceSESq2_Volume0db;
	sesqSESqEnv[0].masterPanpot     = sceSESq2_PanpotCenter;
	sesqSESqEnv[0].masterTimeScale  = sceSESq2_BaseTimeScale;
}

/*------------------------------------------------------------------------------
HSyn側設定
------------------------------------------------------------------------------*/
static void main_setupHSynCtx(void)
{
	int   ret;

	ret = sceHSyn_Init(&hsynCtx, TIMER_DIVISION);

	/* HDとBD「入力ポート 0 のバンク番号 0」として登録 */
	ret = sceHSyn_Load(&hsynCtx, 0, (void *)SPU_TOPADDR, (void *)IOP_HDADDR, 0);

	hsynEnv[0].priority         = 0;
	hsynEnv[0].portMode         = sceHSynModeSESyn;
	hsynEnv[0].waveType         = sceHSynTypeTimbre;
	hsynEnv[0].lfoWaveNum 	    = 0;
	hsynEnv[0].lfoWaveTbl 	    = NULL;
	hsynEnv[0].velocityMapNum   = 0;
	hsynEnv[0].velocityMapTbl   = NULL;

	sceHSyn_SetVoiceStatBuffer(&vstat);

	sceHSyn_SetIdMonitor(&idCtx);
	sceSESq2SetIdMonitor(&idCtx);
}


/*------------------------------------------------------------------------------
.bd転送
------------------------------------------------------------------------------*/
static int main_transBd(void)
{
	int   ret;

	ret = sceSdVoiceTrans(0, SD_TRANS_MODE_WRITE | SD_TRANS_BY_DMA,
				(u_char *)IOP_BDADDR, SPU_TOPADDR, BD_SIZE);

	if (ret <= 0){
	return PROCESS_ERROR;
	}

	sceSdVoiceTransStatus(0, SD_TRANS_STATUS_WAIT);

	return PROCESS_OK;
}

/*------------------------------------------------------------------------------
SPU2初期化
------------------------------------------------------------------------------*/
static int main_initSpu(void)
{
	u_int   i;

	if (!sceSdInit(SD_INIT_COLD)){
	/* ボリューム設定 */
	for (i = 0; i < 2; i++) {
		sceSdSetParam(i | SD_P_MVOLL, 0x3fff);
		sceSdSetParam(i | SD_P_MVOLR, 0x3fff);
	}
	}
	return PROCESS_OK;
}

/*------------------------------------------------------------------------------
セマフォ作成
------------------------------------------------------------------------------*/

static int main_createSema(void)
{
	struct SemaParam   sema;

	sema.initCount = 0;
	sema.maxCount  = 1;
	sema.attr      = AT_THFIFO;

	semaID = CreateSema(&sema);
	if (semaID >= 0){
	return PROCESS_OK;
	} else {
	return PROCESS_ERROR;
	}
}

/*------------------------------------------------------------------------------
ATickスレッド
------------------------------------------------------------------------------*/
static void main_ATickThread(int arg)
{
	int    a = 0;
	int    id1 = 0, id2 = 0, id3 = 0;
	int    act1  = 0; 
	int    act2  = 1; 
	int    act3  = 100; 
	int    act4  = 800;
	int    act4p = 1000;
	int    act4r = 2000;
	int    act5  = 4500;
	int    act5p = 6500;
	int    car1  = 0;
	int    car2  = 700;
	int    car3  = 1500;
	int    ret1 = 0, ret2 = 0, ret3 = 0;

	while (1){
	SleepThread();
	sceHSyn_ATick(&hsynCtx);
	sceSESq2ATick(&sesqCtx);

	/* car1 */
	if (a == act1 + car1){
		/* 演奏開始 */
		ret1 = sceSESq2Play( &sesqCtx, 0, 0, 0, 0, 0, 0);

		/* 先頭1 バイトを判別する */
		if (((ret1 >> 24) & sceSESq2MessageIdPrefix) == sceSESq2MessageIdPrefix) {
			/* 正常再生 */
			id1 = ret1;
			Kprintf("car1[ID %x] play\n", id1);
		} else if (SCE_ERROR_LIB_PREFIX(ret1) == SCE_ERROR_PREFIX_SESQ2) {
			/* modsesq2 エラー */
			Kprintf("sesq2 error\n");
			Kprintf("err : %x\n", SCE_ERROR_ERRNO(ret1));
		} else {
			/* 起こり得ないエラー */
			Kprintf("It cannot be.\n");
		}
	}
	if (a == act2 + car1){
		/* ボリュームコントロール */
		sceSESq2SetVolume(&sesqCtx, id1, 80, 2000);
		Kprintf("car1[ID %x] set volume\n", id1);
	}
	if (a == act3 + car1){
		/* パンポットコントロール */
		sceSESq2SetPanpot(&sesqCtx, id1, 127, 5000);
		Kprintf("car1[ID %x] set panpot\n", id1);
	}
	if (a == act4 + car1){
		/* ピッチコントロール */
		sceSESq2SetPitch(&sesqCtx, id1, -200, 1000);
		/* ボリュームコントロール */
		sceSESq2SetVolume(&sesqCtx, id1, 0, 15000);

		Kprintf("car1[ID %x] set pitch\n", id1);
		Kprintf("car1[ID %x] set volume\n", id1);
	}
	if (a == act5 + car1){
		/* 演奏停止 */
		sceSESq2Stop(&sesqCtx, id1);
		Kprintf("car1[ID %x] stop\n", id1);
	}

	/* car2 */
	if (a == act1 + car2){
		/* 演奏開始 */
		ret2 = sceSESq2Play( &sesqCtx, 0, 0, 0, 0, 0, 0);

		/* 先頭1 バイトを判別する */
		if (((ret2 >> 24) & sceSESq2MessageIdPrefix) == sceSESq2MessageIdPrefix) {
			/* 正常再生 */
			id2 = ret2;
			Kprintf("car2[ID %x] play\n", id2);
		} else if (SCE_ERROR_LIB_PREFIX(ret2) == SCE_ERROR_PREFIX_SESQ2) {
			/* modsesq2 エラー */
			Kprintf("sesq2 error\n");
			Kprintf("err : %x\n", SCE_ERROR_ERRNO(ret2));
		} else {
			/* 起こり得ないエラー */
			Kprintf("It cannot be.\n");
		}

	}
	if (a == act2 + car2){
		/* ボリュームコントロール */
		sceSESq2SetVolume(&sesqCtx, id2, 80, 2000);
		Kprintf("car2[ID %x] set volume\n", id2);
	}
	if (a == act3 + car2){
		/* パンポットコントロール */
		sceSESq2SetPanpot(&sesqCtx, id2, 127, 5000);
		Kprintf("car2[ID %x] set panpot\n", id2);
	}
	if (a == act4 + car2){
		/* ピッチコントロール */
		sceSESq2SetPitch(&sesqCtx, id2, -200, 1000);
		/* ボリュームコントロール */
		sceSESq2SetVolume(&sesqCtx, id2, 0, 15000);

		Kprintf("car2[ID %x] set pitch\n", id2);
		Kprintf("car2[ID %x] set volume\n", id2);
	}
	if (a == act5 + car2){
		/* 演奏停止 */
		sceSESq2Stop(&sesqCtx, id2);
		Kprintf("car2[ID %x] stop\n", id2); 
	}

	/* car3 */
	if (a == act1 + car3){
		/* 演奏開始 */
		ret3 = sceSESq2Play( &sesqCtx, 0, 0, 0, 0, 127, 0);

		/* 先頭1 バイトを判別する */
		if (((ret3 >> 24) & sceSESq2MessageIdPrefix) == sceSESq2MessageIdPrefix) {
			/* 正常再生 */
			id3 = ret3;
			Kprintf("car3[ID %x] play\n", id3);
		} else if (SCE_ERROR_LIB_PREFIX(ret3) == SCE_ERROR_PREFIX_SESQ2) {
			/* modsesq2 エラー */
			Kprintf("sesq2 error\n");
			Kprintf("err : %x\n", SCE_ERROR_ERRNO(ret3));
		} else {
			/* 起こり得ないエラー */
			Kprintf("It cannot be.\n");
		}
	}
	if (a == act2 + car3){
		/* ボリュームコントロール */
		sceSESq2SetVolume(&sesqCtx, id3, 80, 2000);
		Kprintf("car3[ID %x] set volume\n", id3);
	}
	if (a == act3 + car3){
		/* パンポットコントロール */
		sceSESq2SetPanpot(&sesqCtx, id3, 0, 5000);
		Kprintf("car3[ID %x] set panpot\n", id3);
	}
	if (a == act4 + car3){
		/* ピッチコントロール */
		sceSESq2SetPitch(&sesqCtx, id3, -200, 1000);
		/* ボリュームコントロール */
		sceSESq2SetVolume(&sesqCtx, id3, 0, 15000);

		Kprintf("car3[ID %x] set pitch\n", id3);
		Kprintf("car3[ID %x] set volume\n", id3);
	}

	if (a == act4p + car3){
		/* 演奏一時停止(シーケンスのみ) */
		sceSESq2Pause(&sesqCtx, id3, sceSESq2_Stop); 
		Kprintf("car3[ID %x] pause\n", id3);
	}

	if (a == act4r + car3){
		/* 演奏再開 */
		sceSESq2Resume(&sesqCtx, id3);
		Kprintf("car3[ID %x] resume\n", id3);
	}

	if (a == act5p + car3){
		/* 演奏停止 */
		sceSESq2Stop(&sesqCtx, id3);
		Kprintf("car3[ID %x] stop\n", id3);

		SignalSema( semaID );
	}

	a++;
	}
}

/*------------------------------------------------------------------------------
タイムアップハンドラ
------------------------------------------------------------------------------*/
static u_int main_timeupHandler(void *common)
{
	thData    *theMyData;

	theMyData = (thData *)common;
	iWakeupThread(theMyData->ATickThID);

	return theMyData->data;
}

/*------------------------------------------------------------------------------
タイマー作成
------------------------------------------------------------------------------*/
int   timerID;
static int main_createTimer(void)
{
	struct SysClock   clock;

	Kprintf("start\n");

	timerID = AllocHardTimer(TC_SYSCLOCK, 32, 1);

	if (timerID <= 0){
	return PROCESS_ERROR;
	}

	USec2SysClock( TIMER_DIVISION, &clock);
	myThData.data = clock.low;

	if (!SetupHardTimer(timerID, TC_SYSCLOCK, TM_NO_GATE, 1)){
	if (!SetTimerHandler(timerID, myThData.data, main_timeupHandler, 
					&myThData)){
		return PROCESS_OK;
	}
	}

	FreeHardTimer(timerID);
	return PROCESS_ERROR;
}

/*------------------------------------------------------------------------------
ATickスレッド作成
------------------------------------------------------------------------------*/
static int main_createATickThread(void)
{
	struct ThreadParam   param;

	param.attr         = TH_C;
	param.entry        = main_ATickThread;
	param.initPriority = BASE_priority-50;
	param.stackSize    = 0x1000;
	param.option       = 0;

	myThData.ATickThID = CreateThread(&param);
	if (myThData.ATickThID > 0) {
	return PROCESS_OK;
	} else {
	return PROCESS_ERROR;
	}
}

/*------------------------------------------------------------------------------
メインスレッド
------------------------------------------------------------------------------*/
static void main_firstThread(int arg)
{
	if (!main_createATickThread()){
	if (!main_createTimer()){
		if (!main_createSema()){
		if (!main_initSpu()){
			main_transBd();
			main_setModuleContext();
			main_setupSESqCtx();
			main_setupHSynCtx();

			StartThread(myThData.ATickThID, 0);
			StartHardTimer(timerID);

			WaitSema(semaID);

			Kprintf("Fine..............\n");
		}
		}
	}
	}
}

/*------------------------------------------------------------------------------
スタート関数
------------------------------------------------------------------------------*/
int start(int argc, char *argv [])
{
	struct ThreadParam   param;
	int	thid;

	param.attr         = TH_C;
	param.entry        = main_firstThread;
	param.initPriority = BASE_priority;
	param.stackSize    = 0x800;
	param.option       = 0;

	thid = CreateThread(&param);

	if (thid > 0) {
	StartThread(thid, 0);
	return RESIDENT_END;
	} else {
	return NO_RESIDENT_END;
	}
}

/* ----------------------------------------------------------------
 *	End on File
 * ---------------------------------------------------------------- */
/* This file ends here, DON'T ADD STUFF AFTER THIS */




