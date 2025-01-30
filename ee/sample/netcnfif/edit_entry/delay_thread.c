/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
 */
/* 
 *      Netcnf Interface Library Sample Program
 *
 *                         Version 1.1
 *                         Shift-JIS
 *
 *      Copyright (C) 2002 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                         delay_thread.c
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.1            2002.03.13      tetsu       First version
 */

#include <eekernel.h>

static volatile int alarm_id; /* アラーム ID */
static volatile int sema_id;  /* セマフォ ID */

/* セマフォ資源を返却するコールバック関数 */
static void cbfunc(int id, unsigned short time, void *arg)
{
    if(id == alarm_id) iSignalSema(sema_id);
}

/* スレッドを指定時間 delay させる */
void delay_thread(unsigned short time)
{
    struct SemaParam sema;

    /* アラームを設定する */
    alarm_id = SetAlarm(time, cbfunc, NULL);

    /* セマフォを作成 */
    sema.initCount = 0;
    sema.maxCount  = 1;
    sema.option    = 0;
    sema_id = CreateSema(&sema);

    /* セマフォ資源獲得し、指定時間待つ */
    WaitSema(sema_id);

    /* セマフォの削除 */
    DeleteSema(sema_id);
}
