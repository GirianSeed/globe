/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/* 
 *      Inet Event Sample Program
 *
 *                         Version 1.4
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
 *       1.2            2002.02.01      tetsu       Change UP
 *       1.3            2002.02.13      tetsu       Change UP, DOWN
 *       1.4            2002.02.18      tetsu       Change path of common.h
 */

#include <stdio.h>
#include <string.h>
#include <kernel.h>
#include <sifrpc.h>
#include <netcnf.h>
#include <inet/inetctl.h>

#define DEBUG /* デバッグプリントを表示しない場合は
		 コメントアウトして下さい */

#include "../../../../ee/sample/inet/original/event/common.h"
#ifdef DEBUG
#include "dump.h"
#endif /* DEBUG */

/* Sifrpc サーバ */
static void *up_down_server(u_int fno, void *data, int size)
{
    rpc_data_t *p;

    p = (rpc_data_t *)data;

    switch(fno){
    case UP:
#ifdef DEBUG
	dump_env((sceNetCnfEnv_t *)p->addr);
#endif /* DEBUG */

	sceInetCtlSetConfiguration((sceNetCnfEnv_t *)p->addr);
	sceInetCtlUpInterface(0);
	break;
    case DOWN:
	sceInetCtlDownInterface(0);
	break;
    }

    return (data);
}

/* Sifrpc スレッド */
void up_down(void)
{
    static u_int buf[SSIZE / 4];
    sceSifQueueData qd;
    sceSifServeData sd;

    sceSifInitRpc(0);
    sceSifSetRpcQueue(&qd, GetThreadId());
    sceSifRegisterRpc(&sd, UP_DOWN, up_down_server, (void *)&buf[0], 0, 0, &qd);
    sceSifRpcLoop(&qd);
}
