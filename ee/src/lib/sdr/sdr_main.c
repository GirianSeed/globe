/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5
 */
/* 
 *                      Emotion Engine Library
 *                          Version 0.50
 *                           Shift-JIS
 *
 *      Copyright (C) 1998-1999 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                        libsdr - sdr_main.c
 *                        main funcitons
 *
 *     Version    Date          Design     Log
 *  --------------------------------------------------------------------
 *     0.60       Oct.12.1999   morita     first checked in.
 */

#include <eekernel.h>
#include <sif.h>
#include <sifcmd.h>
#include <sifrpc.h>
#include <libsdr.h>
#include <stdarg.h>
#include <sdrcmd.h>
#include "sdr_i.h"

#define STACK_SIZE 0x10
#define DATA_SIZE_B (64)
#define UNCHASH 0x20000000

static u_long128 stack [STACK_SIZE];
static u_int sbuff [16] __attribute__((aligned (64)));
static sceSifClientData sceSd_gCd;

int sceSdRemote (int arg, ...);


/* ------------------------------------------------------------------------
   SPU2リモート環境の初期化。ここでIOP側RPC関数のBindを行なう。
   ------------------------------------------------------------------------*/
int
sceSdRemoteInit (void)
{
    int fd, i,cid;
    char *cp;

    sceSifInitRpc(0);
    while (1) {
	if (sceSifBindRpc (&sceSd_gCd, sce_SDR_DEV, 0) < 0) {
	    scePrintf("sceSdRemoteInit() RPC bind error!\n");
	    return (-1);
	}
	i = 10000;
	while (i--) {
	}
	if(sceSd_gCd.serve != 0) break;
    }

    FlushCache (0); /* 念のため */
    /* sceSdRemote (1, rSdInit); */

    return 0;
}

static sceSifEndFunc sceSd_gEnd_func = NULL;

/* ------------------------------------------------------------------------
   EE上のバッファをIOPメモリに転送する。
   *buff はDRAM上に乗っている必要がある。（キャッシュのことは関知しない）
   ------------------------------------------------------------------------*/
/* 注意:
 *     SIF DMA において、現在 EE 側にて sceSifDmaStatIntr() が
 *     実装されていないため、ノンブロック指定 (isBlock: 0) は無効。
 */
int
sceSdTransToIOP (void *buff, u_int sendAddr, u_int size, u_int isBlock)
{
    unsigned int did;
    int i;
    static sceSifDmaData transData;

    if (isBlock == 0) {
	return (-1);	    /* 現在の実装ではノンブロック指定は無効 */
    }

    transData.data = (u_int)buff;
    transData.addr = (u_int)sendAddr;
    transData.size = size;
    transData.mode = 0;

    did = sceSifSetDma (&transData, 1);
    if (did == 0) return (-1);

    if (isBlock == 1) {
	while (sceSifDmaStat (did) >= 0){}
	PRINTF(("send SpuStEnv completed \n"));
    }

    return 0;
}

/* ------------------------------------------------------------------------
   SIFによる転送が終了した時に発生する、割り込みハンドラを設定。
   ------------------------------------------------------------------------*/
sceSifEndFunc
sceSdCallBack (sceSifEndFunc end_func)
{
    sceSifEndFunc old_func;

    old_func = sceSd_gEnd_func;
    sceSd_gEnd_func = end_func;

    return (old_func);
}

/* ------------------------------------------------------------------------
   IOP側のlibsdの関数をリモート実行する。
   ------------------------------------------------------------------------*/
int
sceSdRemote (int arg, ...)
{
    int ret, i, isBlock, command;
    sceSifDmaData transData;
    va_list ap;
    sceSifEndFunc end_func = NULL;

    va_start (ap, arg);
    isBlock = arg;
    command = va_arg (ap, int);
    sbuff [0] = (int)sbuff; 
    for (i = 1; i < 7; i++) {
	sbuff [i] = va_arg (ap, int);
    }

    if (isBlock == 0){
	isBlock = SIF_RPCM_NOWAIT;
	end_func = sceSd_gEnd_func;
    } else {
	isBlock = 0;
    }

#ifdef SCE_OBSOLETE
    if (command == rSdSetTransCallback) {
	if ((void *)sbuff [1] == 0){ /* 第１引数はチャンネル番号 */
	    _sce_sdr_gDMA0CB = (void *)sbuff [2];
	} else {
	    _sce_sdr_gDMA1CB = (void *)sbuff [2];
	}
    } else if (command == rSdSetIRQCallback) {
	_sce_sdr_gIRQCB = (void *)sbuff [1];
    } else
#endif
    if (command == rSdSetTransIntrHandler) {
	if (sbuff [1] == 0) { /* 第 1 引数はチャンネル番号 */
	    _sce_sdr_transIntr0Hdr = (sceSdTransIntrHandler) sbuff [2];
	    _sce_sdr_transIntr0Arg = (void *) sbuff [3];
	} else{
	    _sce_sdr_transIntr1Hdr = (sceSdTransIntrHandler) sbuff [2];
	    _sce_sdr_transIntr1Arg = (void *) sbuff [3];
	}
    } else if (command == rSdSetSpu2IntrHandler) {
	_sce_sdr_spu2IntrHdr = (sceSdSpu2IntrHandler) sbuff [1];
	_sce_sdr_spu2IntrArg = (void *) sbuff [2];
    }

    PRINTF(("sceSifCallRpc start - [%04x] ", command));
    PRINTF(("send value = %x %x, %x, %x\n", sbuff [0], sbuff [1], sbuff [2]));

    if (command == rSdSetEffectAttr ||
	command == rSdSetEffectMode ||
	command == rSdSetEffectModeParams) {
	/* SET系で構造体で値を渡すもの */
	sceSifCallRpc (&sceSd_gCd, (command | sbuff [1]), isBlock, 
		       (void *)(sbuff [2]), DATA_SIZE_B,
		       sbuff, DATA_SIZE_B,
		       end_func, (void *)(sbuff [0]));
	ret = sbuff [0];
    } else if (command == rSdGetEffectAttr) {
	/* GET系で構造体に値が返ってくるもの */
	sceSifCallRpc (&sceSd_gCd, (command | sbuff [1]), isBlock, 
		       (void *)(&sbuff [0]), DATA_SIZE_B,
		       (void *)(sbuff [2]), 64, 
		       end_func, (void *)(sbuff [2]));
    } else if (command >= rSdUserCommandMin &&
	       command <= rSdUserCommandMax) {
	sceSifCallRpc (&sceSd_gCd, command, isBlock,
		       (void *) sbuff [1], sbuff [2],
		       (void *)(&sbuff [0]), 16,
		       end_func, (void *)(&sbuff [0]));
	ret = sbuff [0];
    } else {
	sceSifCallRpc (&sceSd_gCd, command, isBlock,
		       (void *)(&sbuff [0]),
		       DATA_SIZE_B, (void *)(&sbuff [0]), 16,
		       end_func, (void *)(&sbuff [0]));
	ret = sbuff [0];
    }

    PRINTF (("sceSifCallRpc cmplete \n"));
    va_end (ap);

    return ret;
}

