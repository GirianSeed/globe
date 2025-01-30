/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/* 
 * I/O Processor Library Sample Program
 *
 * Copyright (C) 2003 Sony Computer Entertainment Inc.
 * All Rights Reserved.
 *
 * ezsein - entry.c
 *    entry function
 */

#include <kernel.h>
#include <sys/types.h>
#include <stdio.h>
#include <sif.h>
#include <sifcmd.h>
#include <sifrpc.h>
#include <csl.h>
#include <cslmidi.h>
#include <cslse.h>
#include "ezsein.h"

ModuleInfo Module = { "ezsein_driver", 0x0201 };

extern int rpc_server(void);	/* rpc.c */

/* 開始エントリー */
int start(void)
{
    struct ThreadParam param;
    int th;

    sceSifInitRpc(0);

    printf("EzSEIn driver version 1.0\n");

    param.attr         = TH_C;
    param.entry        = rpc_server;
    param.initPriority = BASE_priority - 2;
    param.stackSize    = 0x800;
    param.option       = 0;
    th = CreateThread(&param);
    if (th > 0) {
	/* メイン処理スレッドの開始 */
	StartThread(th, 0);
	printf(" Exit EzSEIn loader thread\n");
	return RESIDENT_END;
    } else {
	return NO_RESIDENT_END;
    }
}

/* ----------------------------------------------------------------
 *	End on File
 * ---------------------------------------------------------------- */
/* DON'T ADD STUFF AFTER THIS */
