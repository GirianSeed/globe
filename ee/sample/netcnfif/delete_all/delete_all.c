/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
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
 *                         delete_all.c
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.1            2002.01.28      tetsu       First version
 */

#include <eekernel.h>
#include <sifrpc.h>
#include <libmc.h>
#include <netcnfif.h>

#include "load_module.h"

#define FNAME "mc0:/BWNETCNF/BWNETCNF" /* 設定管理ファイルパス */

static sceNetcnfifArg_t if_arg; /* Netcnf Interface に必要なデータ領域 */

/* あなたのネットワーク設定ファイルを削除する */
static int delete_all(char *fname)
{
    sceNetcnfifDeleteAll(fname);
    while(sceNetcnfifCheck());
    sceNetcnfifGetResult(&if_arg);
    if(if_arg.data < 0){
	scePrintf("[%s] sceNetcnfifDeleteAll (%d)\n", __FILE__, if_arg.data);
	return (if_arg.data);
    }else{
	return (0);
    }
}

int main(void)
{
    sceSifInitRpc(0);
    while(!sceSifRebootIop(IOPRP)); /* IOP リブート・モジュール置き換え */
    while(!sceSifSyncIop());        /* 終了を待つ */
    sceSifInitRpc(0);
    sceSifLoadFileReset();
    sceFsReset();

    /* 必要なモジュールをロード */
    load_module();

    /* Sifrpc サーバとバインド */
    sceNetcnfifInit();
    sceNetcnfifSetup();

    /* メモリーカード環境の初期化 */
    sceMcInit();

    /* あなたのネットワーク設定ファイルを削除 */
    if(delete_all(FNAME) < 0) goto done;

 done:
    scePrintf("[%s] complete\n", __FILE__);

    return (0);
}
