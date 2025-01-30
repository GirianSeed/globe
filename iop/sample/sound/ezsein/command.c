/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/*
 * I/O Processor Library Sample Program
 *
 * Copyright (C) 2003 Sony Computer Entertainment Inc.
 * All Rights Reserved.
 *
 * ezsein - command.c
 *    SE handling
 */

#include <kernel.h>
#include <stdio.h>
#include <sys/file.h>
#include <sif.h>
#include <timerman.h>
#include <libsd.h>
#include <csl.h>
#include <cslse.h>
#include <modhsyn.h>
#include <modsein.h>
#include "ezsein.h"

#define True 1

#define STREAMBUF_SIZE 1024

// 4167 micro sec. = 1000 * 1000 / 240 = 1/240 sec
#define ONE_240TH 4167

// CSL
static sceCslCtx     synthCtx;
static sceCslBuffGrp synthGrp;
static sceCslBuffCtx sInBfCtx[2];
static sceHSynEnv    synthEnv[1];

// data buffer
static char seinBf_T[SEIN_TRANS_SIZE] __attribute__((aligned (64)));
static char seinBf_R[SEIN_TRANS_SIZE];

sceHSyn_VoiceStat gVstat;
int gThid = 0;

typedef struct TimerCtx {
    int thread_id;
    int timer_id;
    int count;
} TimerCtx;
TimerCtx timer;

/* ================================================================ */

/* BD ファイル分割転送時のバッファ */
  // バッファサイズ
int gSpuTransPacketSize;
  // 先頭アドレス
int gSpuTransPacketAddr;

/* tick 毎の処理 */
static int atick(void)
{
    while (1){
	SleepThread();

	// seinBf_T->flag が 1 なら EE から転送があった
	// → seinBf_R にデータをコピー: modhsyn は seinBf_R を使用する

	if (((SEInBuf *)seinBf_T)->flag != 0){
	    memcpy(seinBf_R, seinBf_T, SEIN_TRANS_SIZE);
	    ((SEInBuf *)seinBf_T)->flag = 0;
	}
	sceHSyn_ATick(&synthCtx);
    }

    return 0;
}

/* スレッドの作成 */
static int make_thread(void)
{
    struct ThreadParam param;
    int	thid;

    param.attr         = TH_C;
    param.entry        = atick;
    param.initPriority = BASE_priority-3;
    param.stackSize    = 0x800;
    param.option = 0;

    thid = CreateThread(&param);	

    return thid;
}

/* タイマー用割り込みハンドラ */
unsigned int timer_handler(void *common)
{
    TimerCtx *tc = (TimerCtx *)common;

    iWakeupThread(tc->thread_id); // wakeup atick()

    /* 新たな比較値を設定しカウントを続行 */
    return (tc->count);
}

/* タイマー設定 */
int set_timer(TimerCtx *t)
{
    struct SysClock clock;
    int timer_id;

    // 1/240 sec = ??? sysclock
    USec2SysClock(ONE_240TH, & clock);
    t->count = clock.low;	/* within 32-bit */

    // Use sysclock timer
    if ((timer_id = AllocHardTimer(TC_SYSCLOCK, 32, 1)) <= 0){
	printf("Can NOT allocate hard timer ...\n");
	return(-1);
    }
    t->timer_id = timer_id;

    if (SetTimerHandler(timer_id, t->count,
			timer_handler, (void *)t) != KE_OK){
	printf("Can NOT set timeup timer handler ...\n");
	return(-1);
    }

    if (SetupHardTimer(timer_id, TC_SYSCLOCK, TM_NO_GATE, 1) != KE_OK){
	printf("Can NOT setup hard timer ...\n");
	return(-1);
    }

    return 0;
}

/* タイマー動作開始 */
int start_timer(TimerCtx *t)
{
    if (StartHardTimer(t->timer_id) != KE_OK){
	printf("Can NOT start hard timer ...\n");
	return(-1);
    }

    return 0;
}

/* タイマー削除 */
int clear_timer(TimerCtx *t)
{
    if (FreeHardTimer(t->timer_id) != KE_OK){
	printf("Can NOT free hard timer ...\n");
	return(-1);
    }

    return 0;
}

/* タイマー停止 */
int stop_timer(TimerCtx* t)
{
    int ret;

    ret = StopHardTimer(t->timer_id);
    if (! (ret == KE_OK || ret == KE_TIMER_NOT_INUSE)){
	printf("Can NOT stop hard timer ...\n");
	return(-1);
    }

    return 0;
}

/* ================================================================ */

/* CSL モジュールコンテキスト設定 */
void set_module_context(void)
{
    ((sceCslSeStream *)seinBf_R)->buffsize  = SEIN_BUF_SIZE;
    ((sceCslSeStream *)seinBf_R)->validsize = 0;
	  ((SEInBuf  *)seinBf_R)->flag      = 0;
    ((sceCslSeStream *)seinBf_T)->buffsize  = SEIN_BUF_SIZE;
    ((sceCslSeStream *)seinBf_T)->validsize = 0;
	  ((SEInBuf  *)seinBf_T)->flag      = 0;

    synthCtx.extmod     = NULL;
    synthCtx.callBack   = NULL;
    synthCtx.buffGrpNum = 1;
    synthCtx.buffGrp    = &synthGrp;
      synthGrp.buffNum = 4;
      synthGrp.buffCtx = sInBfCtx;
	sInBfCtx[0].sema = 0;
	sInBfCtx[0].buff = seinBf_R;
	sInBfCtx[1].sema = 0;
	sInBfCtx[1].buff = &synthEnv [0];

    return;
}

/* ================================================================ */

/* ファイルサイズ取得 */
int SeGetIopFileLength(char *filename)
{
    int flen, fd;

    if ((fd = open(filename, O_RDONLY)) < 0){
	ERROR(("file open failed. %s \n", filename)); return -1;
    }
    if ((flen = lseek(fd, 0, SEEK_END)) < 0){
	ERROR(("file open failed. %s \n", filename)); return -1;
    }
    if (lseek(fd, 0, SEEK_SET) < 0){
	ERROR(("file open failed. %s \n", filename)); return -1;
    }
    close(fd);

    return flen;
}


/* BD ファイルの分割ロード & 転送 */
int SeTransBdPacket(EzSEInBank *bank)
{
    int mod, count, fd, size;
    int  total = 0;

    count = bank->bdSize / gSpuTransPacketSize;
    mod = bank->bdSize - count * gSpuTransPacketSize;

    if ((fd = open(bank->bdName, O_RDONLY)) < 0){
	ERROR(("bd file open failed. %s \n", bank->bdName));
	return -1;
    }
    printf(" bd file open %s \n", bank->bdName);

    while (1){
	size = read(fd, (unsigned char *)gSpuTransPacketAddr,
		    gSpuTransPacketSize);
	if (sceHSyn_VoiceTrans(1,
			       (unsigned char *)gSpuTransPacketAddr,
			       (unsigned char *)bank->spuAddr + total, size)
	    != sceHSynNoError){
	    printf("sceHSyn_VoisTrans Error\n");
	    return -1;
	}

	total += size;
	if (size != gSpuTransPacketSize) break;
    }

    close(fd);

    return total;
}

/* BD ファイルの転送 */
int SeTransBd(EzSEInBank *bank)
{
    if (sceHSyn_VoiceTrans(1, (unsigned char *)bank->bdAddr,
			   (unsigned char *)bank->spuAddr, bank->bdSize)
	!= sceHSynNoError){
	printf("sceHSyn_VoisTrans Error\n");
	return(-1);
    }

    return 0;
}

/* CSL 環境構造体設定 & バンク設定 */
int SeSetHd(EzSEInBank *bank)
{
    int ret;
    int oldei;

    synthEnv[0].priority = 0;
    synthEnv[0].portMode = sceHSynModeSESyn; /* SE mode */
    synthEnv[0].waveType = bank->type;
    synthEnv[0].lfoWaveNum = 0;
    synthEnv[0].lfoWaveTbl = NULL;
    synthEnv[0].velocityMapNum = 0;
    synthEnv[0].velocityMapTbl = NULL;

    CpuSuspendIntr(&oldei);
    ret = sceHSyn_Load(&synthCtx, 0,
		       (void *)bank->spuAddr, (void *)bank->hdAddr, 0);
    CpuResumeIntr(oldei);
    if (ret != sceHSynNoError){
	printf("sceHSyn_Load(%d) error\n", 0);
	return(-1);
    }

    return 0;
}

/* CSL ハードウェアシンセサイザ: se-stream による全効果音発音数制限の設定 */
void SeSetMaxVoices(unsigned int attr)
{
    if (attr > 48) attr = 48;
    sceHSyn_SESetMaxVoices(attr);
    return;
}


/* CSL ハードウェアシンセサイザ: ポート属性の設定 */
void SeSetPortAttr(int attr)
{
    synthEnv[0].priority     = (attr & PORTATTR_MASK_PRIORITY);
    synthEnv[0].maxPolyphony = (attr & PORTATTR_MASK_MAXNOTE) >> 8;
#if 0
    printf("port %d pri %d, note %d \n",
	   0, synthEnv[0].priority, synthEnv[0].maxPolyphony);
#endif
    return;
}

/* CSL ハードウェアシンセサイザ: 演奏ボリュームの設定 */
void SeSetPortVolume(int vol)
{
    sceHSyn_SetVolume(&synthCtx, 0, vol);
    return;
}

/* CSL 環境の初期化と処理の開始 */
int SeInit(int allocsize)
{
    int oldei;

    gSpuTransPacketSize = allocsize;
    CpuSuspendIntr(&oldei);
    gSpuTransPacketAddr = (int)AllocSysMemory(0, allocsize, NULL);	
    CpuResumeIntr(oldei);

    // initialize modules
    set_module_context();
    if (sceHSyn_Init(&synthCtx, ONE_240TH) != sceHSynNoError){ // 1/240sec
	printf("sceHSyn_Init Error\n");
	return 0;
    }
    CpuSuspendIntr(&oldei);
    sceHSyn_SetVoiceStatBuffer(&gVstat);
    CpuResumeIntr(oldei);

    if ((gThid = make_thread()) < 0){
	printf("Thread creating Error\n");
	return 0;
    }
    timer.thread_id = gThid;
    printf("EzSEIn: create thread ID= %d, ", gThid);

    /* スレッドを起動 */
    StartThread(gThid, (u_long)NULL);

    set_timer(&timer);
    start_timer(&timer);

    return ((int)(&seinBf_T[0]));
}

/* 終了処理 */
int SeQuit (void)
{
    int oldei;

    CpuSuspendIntr(&oldei);
    FreeSysMemory((void*)gSpuTransPacketAddr);
    CpuResumeIntr(oldei);
    stop_timer(&timer);
    clear_timer(&timer);
    if (gThid > 0){
	DeleteThread(gThid);
    }

    return 0;
}

/* ----------------------------------------------------------------
 *	End on File
 * ---------------------------------------------------------------- */
/* This file ends here, DON'T ADD STUFF AFTER THIS */
