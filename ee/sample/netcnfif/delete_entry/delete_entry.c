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
 *                         delete_entry.c
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.1            2002.01.28      tetsu       First version
 */

#include <malloc.h>
#include <eekernel.h>
#include <sifrpc.h>
#include <libmc.h>
#include <netcnfif.h>

#include "load_module.h"

#define DEBUG

#define FNAME "mc0:/BWNETCNF/BWNETCNF" /* 設定管理ファイルパス */

static sceNetcnfifArg_t if_arg; /* Netcnf Interface に必要なデータ領域 */

/* 設定を削除する */
static int delete_entry(char *fname, int type, char *usr_name)
{
    sceNetcnfifDeleteEntry(fname, type, usr_name);
    while(sceNetcnfifCheck());
    sceNetcnfifGetResult(&if_arg);
    if(if_arg.data < 0){
	scePrintf("[%s] type(%d):sceNetcnfifDeleteEntry (%d)\n", __FILE__, type, if_arg.data);
	return (if_arg.data);
    }else{
	return (0);
    }
}

/* 設定のリストを取得する */
static int get_list(char *fname, int type, sceNetcnfifList_t **list)
{
    int r;

    /* 数を取得 */
    sceNetcnfifGetCount(fname, type);
    while(sceNetcnfifCheck());
    sceNetcnfifGetResult(&if_arg);
    if(if_arg.data <= 0){
	scePrintf("[%s] type(%d):sceNetcnfifGetCount (%d)\n", __FILE__, type, if_arg.data);
	return (if_arg.data);
    }
    r = if_arg.data;

    /* リストを取得 */
    *list = (sceNetcnfifList_t *)memalign(64, r * sizeof(sceNetcnfifList_t));
    if(*list == NULL){
	scePrintf("[%s] type(%d):memalign (0x%08x)\n", __FILE__, type, *list);
	return (sceNETCNFIF_ALLOC_ERROR);
    }
    FlushCache(0);
    sceNetcnfifGetList(fname, type, *list, r);
    while(sceNetcnfifCheck());
    sceNetcnfifGetResult(&if_arg);
    if(if_arg.data < 0){
	scePrintf("[%s] type(%d):sceNetcnfifGetList (%d)\n", __FILE__, type, if_arg.data);
	return (if_arg.data);
    }

#ifdef DEBUG
    {
	int i;
	scePrintf("[%s] get_list result\n", __FILE__);
	for(i = 0; i < r; i++){
	    scePrintf("%d,%d,%s,%s\n", (*list + i)->type, (*list + i)->stat, (*list + i)->sys_name, (*list + i)->usr_name);
	}
    }
#endif /* DEBUG */

    return (r);
}

int main(void)
{
    static sceNetcnfifList_t *net_list, *dev_list, *ifc_list;
    int net_num, dev_num, ifc_num, i;

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

    /* 組み合わせのリストを取得 */
    if((net_num = get_list(FNAME, sceNetcnfifArg_type_net, &net_list)) <= 0) goto done;

    /* 接続機器設定のリストを取得 */
    if((dev_num = get_list(FNAME, sceNetcnfifArg_type_dev, &dev_list)) <= 0) goto done;

    /* 接続プロバイダ設定のリストを取得 */
    if((ifc_num = get_list(FNAME, sceNetcnfifArg_type_ifc, &ifc_list)) <= 0) goto done;

    /* 組み合わせの削除 */
    for(i = 0; i < net_num; i++){
	if(delete_entry(FNAME, sceNetcnfifArg_type_net, net_list[i].usr_name) < 0) goto done;
    }

    /* 接続機器設定の削除 */
    for(i = 0; i < dev_num; i++){
	if(delete_entry(FNAME, sceNetcnfifArg_type_dev, dev_list[0].usr_name) < 0) goto done;
    }

    /* 接続プロバイダ設定の削除 */
    for(i = 0; i < ifc_num; i++){
	if(delete_entry(FNAME, sceNetcnfifArg_type_ifc, ifc_list[0].usr_name) < 0) goto done;
    }

 done:
    scePrintf("[%s] complete\n", __FILE__);

    return (0);
}
