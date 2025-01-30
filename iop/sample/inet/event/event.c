/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/* 
 *      Inet Event Sample Program
 *
 *                         Version 1.2
 *                         Shift-JIS
 *
 *      Copyright (C) 2002 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                         event.c
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.1            2001.12.28      tetsu       First version
 *       1.2            2002.02.05      tetsu       Add ModuleInfo
 */

#include <kernel.h>
#include <inet/inetctl.h>
#include <usbmload.h>

#include "get_event.h"
#include "up_down.h"

ModuleInfo Module = {"event", 0x0101};

int start(int argc, char *argv[])
{
    struct ThreadParam th_param;
    int th_id0, th_id1;

    /* USB モデム・Ethernet 機器ドライバのオートロードを許可 */
    sceUsbmlActivateCategory("Ether");
    sceUsbmlActivateCategory("Modem");
    sceUsbmlEnable();

    /* イベントを取得するスレッドを作成 */
    th_param.attr         = TH_C;
    th_param.entry        = get_event;
    th_param.initPriority = USER_LOWEST_PRIORITY - 2;
    th_param.stackSize    = 0x800;
    th_param.option       = 0;

    th_id0 = CreateThread(&th_param);

    /* インタフェースをアップ・ダウンするスレッドを作成 */
    th_param.attr         = TH_C;
    th_param.entry        = up_down;
    th_param.initPriority = USER_LOWEST_PRIORITY - 1;
    th_param.stackSize    = 0x800;
    th_param.option       = 0;

    th_id1 = CreateThread(&th_param);

    /* get_event 環境を初期化 */
    get_event_init();

    /* スレッドをスタート */
    if(th_id0 > 0 && th_id1 > 0){
	StartThread(th_id0, 0);
	StartThread(th_id1, 0);
	return (RESIDENT_END);
    }else{
	return (NO_RESIDENT_END);
    }
}
