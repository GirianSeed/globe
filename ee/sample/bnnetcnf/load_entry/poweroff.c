/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 $Id: poweroff.c,v 1.2 2002/10/20 13:51:31 tetsu Exp $
 */
/* 
 * Emotion Engine Library Sample Program
 *
 * Copyright (C) 2002 Sony Computer Entertainment Inc.
 * All Rights Reserved.
 *
 * bnnetcnf/load_entry - poweroff.c
 *	"PlayStation BB Navigator" Network Configuration Read Library Sample Program
 *	"PlayStation BB Unit" poweroff program
 *
 *      Date            Design      Log
 *  ----------------------------------------------------
 *      2002-10-12      tetsu       first version
 *      2002-10-20      tetsu       correspond to code-review
 */

#include <eekernel.h>
#include <sifdev.h>
#include <libcdvd.h>

#define STACK_SIZE (512 * 16)

static u_char stack[STACK_SIZE] __attribute__ ((aligned(16)));
static int    tid;
static int    sid;

/* プロトタイプ宣言 */
static void poweroff_thread(void *arg);
static void poweroff_handler(void *arg);

/*
 * パワーオフを実行するスレッド
 */
static void poweroff_thread(void *arg)
{
    static int stat = 0;
    int        sema_id = (int)arg;

    /* セマフォ資源獲得し、iSignalSema() されるまで待つ */
    WaitSema(sema_id);

    /* pfs がマウントされている時、ファイルクローズする */
    sceDevctl("pfs:", PDIOC_CLOSEALL, NULL, 0, NULL, 0);

    /* Dev9 being powered off directry */
    while (sceDevctl("dev9x:", DDIOC_OFF, NULL, 0, NULL, 0) < 0);

    /* Power off */
    while (!sceCdPowerOff(&stat) || stat);

    while (1);
}

/*
 * セマフォ資源を返却するコールバック関数
 */
static void poweroff_handler(void *arg)
{
    int sema_id = (int)arg;

    iSignalSema(sema_id);
}

/*
 * DTL-H30000( BAY モデル)を使用して、(オン/スタンバイ/) RESET ボタンが
 * 軽く押された際に、電源 OFF 処理がされるための処理
 */
void prepare_poweroff(void)
{
    struct ThreadParam tparam;
    struct SemaParam   sparam;

    sparam.initCount = 0;
    sparam.maxCount  = 1;
    sparam.option    = 0;
    sid = CreateSema(&sparam);

    ChangeThreadPriority(GetThreadId(), 2);

    tparam.stackSize    = STACK_SIZE;
    tparam.gpReg        = &_gp;
    tparam.entry        = poweroff_thread;
    tparam.stack        = (void *)stack;
    tparam.initPriority = 1;
    tid = CreateThread(&tparam);
    StartThread(tid, (void *)sid);

    sceCdPOffCallback(poweroff_handler, (void *)sid);
}

/*
 * 電源 OFF 処理を削除する
 */
void destory_prepare_poweroff(void)
{
    /* 電源 OFF コールバック関数をクリアする */
    sceCdPOffCallback(NULL, NULL);

    /* poweroff_thread を削除する */
    TerminateThread(tid);
    DeleteThread(tid);

    /* セマフォを削除する */
    DeleteSema(sid);
}
