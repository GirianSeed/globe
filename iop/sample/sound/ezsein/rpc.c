/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/* 
 * I/O Processor Library Sample Program
 *
 * Copyright (C) 2003 Sony Computer Entertainment Inc.
 * All Rights Reserved.
 *
 * ezsein - rpc.c
 *     command dispatch routine
 */

#include <kernel.h>
#include <sys/types.h>
#include <stdio.h>
#include <sif.h>
#include <sifcmd.h>
#include <sifrpc.h>
#include <libsd.h>
#include <csl.h>
#include <cslse.h>
#include "ezsein.h"

/* EEから転送されるRPCの引数の受け口 */
int gRpcArg[16];

static void *rpc_dispatch(unsigned int fno, void *data, int size);

extern int SeInit(int foo);
extern int SeQuit(void);
extern int SeSetHd(EzSEInBank *bank);
extern int SeStop(int addr);
extern int SeTransBd(EzSEInBank *bank);
extern int SeTransBdPacket(EzSEInBank *bank);
extern int SeGetIopFileLength(char *filename);
extern int SeGetStatus(void);
extern void SeSetMaxVoices(unsigned int attr);
extern void SeSetPortAttr(int attr);
extern void SeSetPortVolume(int vol);

/* ---------------------------------------------------------------- */
/*  SIF RPC サーバ: EE のリクエストを受けとり、処理を振り分ける     */
/* ---------------------------------------------------------------- */
int rpc_server(void)
{
    sceSifQueueData qd;
    sceSifServeData sd;

    sceSifInitRpc(0);

    sceSifSetRpcQueue(&qd, GetThreadId());
    sceSifRegisterRpc(&sd, EZSEIN_DEV,
		      rpc_dispatch, (void *)gRpcArg, NULL, NULL, &qd);
    PRINTF(("goto SE command loop\n"));

    /* コマンド待ちループ */
    sceSifRpcLoop(&qd);

    return 0;
}

/* ---------------------------------------------------------------- */
/*  EEからのリクエストによって呼ばれる関数                          */
/* ---------------------------------------------------------------- */
static int ret = 0;

static void *rpc_dispatch(unsigned int command, void *data, int size)
{ 
    PRINTF ((" sefunc [%x] %x, %x, %x, %x\n",
	     command,
	     *((int *)data + 0), *((int *)data + 1),
	     *((int *)data + 2), *((int *)data + 3)));
    
    switch(command & 0xfff0) {
    case EZSEIN_INIT:
	/* CSL 環境の初期化と処理の開始 */
	ret = SeInit(*((int *)data));
	break;
    case EZSEIN_QUIT:
	/* 処理終了 */
	SeQuit();
	break;
    case EZSEIN_SETHD:
	/* HD データ情報の設定 */
	SeSetHd((EzSEInBank *)((int *)data));
	break;
    case EZSEIN_TRANSBD:
	/* BD データの転送 */
	ret = SeTransBd((EzSEInBank *)((int *)data));
	break;
    case EZSEIN_TRANSBDPACKET:
	/* BD ファイルの読み込み + 転送 */
	ret = SeTransBdPacket((EzSEInBank *)((int *)data));
	break;
    case EZSEIN_GETFILELENGTH:
	/* ファイルサイズの取得 */
	ret = SeGetIopFileLength((char *)((int *)data));
	break;
    case EZSEIN_SETATTR:
	/* CSL ハードウェアシンセサイザ: ポート属性の設定 */
	SeSetPortAttr(*((int *)data));
	break;
    case EZSEIN_SETVOL:
	/* CSL ハードウェアシンセサイザ: 演奏ボリュームの設定 */
	SeSetPortVolume(*((int *)data));
	break;
    case EZSEIN_SETMAXVOICES:
	/* CSL ハードウェアシンセサイザ: se-stream による全効果音発音数制限の設定 */
	
	SeSetMaxVoices(*((unsigned int *)data));
	break;
    default:
	ERROR(("EzSEIn driver error: unknown command %d \n", *((int *)data)));
	break;
    }
    PRINTF(("return value = %x\n", ret)); 

    return((void *)(&ret));
}

/* ----------------------------------------------------------------
 *	End on File
 * ---------------------------------------------------------------- */
/* DON'T ADD STUFF AFTER THIS */

