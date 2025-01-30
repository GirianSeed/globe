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
 *                         set_latest_entry.c
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
#include "set_configuration.h"

#define DEBUG

#define WORKAREA_SIZE (0x1000) /* IOP におけるワークエリアのサイズ */

#define FNAME "mc0:/BWNETCNF/BWNETCNF"   /* 設定管理ファイルパス */
#define IFC_USR_NAME0 "Ethernet Setting" /* 接続プロバイダ設定名0 */
#define IFC_USR_NAME1 "PPPoE Setting"    /* 接続プロバイダ設定名1 */
#define IFC_USR_NAME2 "Modem Setting"    /* 接続プロバイダ設定名2 */

#define IFC_PATTERN0 (3) /* 接続プロバイダ設定パターン0 */
#define IFC_PATTERN1 (5) /* 接続プロバイダ設定パターン1 */
#define IFC_PATTERN2 (7) /* 接続プロバイダ設定パターン2 */

static sceNetcnfifArg_t if_arg; /* Netcnf Interface に必要なデータ領域 */

/* 設定を追加する */
static int add_entry(char *fname, int type, char *usr_name)
{
    sceNetcnfifAddEntry(fname, type, usr_name);
    while(sceNetcnfifCheck());
    sceNetcnfifGetResult(&if_arg);
    if(if_arg.data < 0){
	scePrintf("[%s] type(%d):sceNetcnfifAddEntry (%d)\n", __FILE__, type, if_arg.data);
	return (if_arg.data);
    }else{
	return (0);
    }
}

/* 設定のリストを編集する */
static int set_latest_entry(char *fname, int type, char *usr_name)
{
    sceNetcnfifSetLatestEntry(fname, type, usr_name);
    while(sceNetcnfifCheck());
    sceNetcnfifGetResult(&if_arg);
    if(if_arg.data < 0){
	scePrintf("[%s] type(%d):sceNetcnfifSetLatestEntry (%d)\n", __FILE__, type, if_arg.data);
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

/* デバイスの残り容量をチェックする */
static int check_capacity(char *fname)
{
    int r;

    sceNetcnfifCheckCapacity(fname);
    while(sceNetcnfifCheck());
    sceNetcnfifGetResult(&if_arg);
    if(if_arg.data < 0){
	scePrintf("[%s] sceNetcnfifCheckCapacity (%d)\n", __FILE__, if_arg.data);
	return (if_arg.data);
    }
    r = if_arg.data;

    return (r);
}

int main(void)
{
    static sceNetcnfifData_t data __attribute__((aligned(64)));
    static sceNetcnfifList_t *ifc_list;
    int ifc_num;
    int addr, id, i, r;

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

    /* デバイスの残り容量をチェックする */
    if(check_capacity(FNAME) < 0) goto done;

    /* IOP 側のワークエリアを確保 */
    sceNetcnfifAllocWorkarea(WORKAREA_SIZE);
    while(sceNetcnfifCheck());
    sceNetcnfifGetResult(&if_arg);
    if(if_arg.data < 0){
	scePrintf("[%s] sceNetcnfifAllocWorkarea (%d)\n", __FILE__, if_arg.data);
	goto done;
    }

    /* f_no_decode を設定(タイトルに使用する場合は必ず
       sceNetcnfifArg_f_no_decode_off に設定すること) */
    sceNetcnfifSetFNoDecode(sceNetcnfifArg_f_no_decode_on);

    /* sceNetcnfifData の転送先アドレスを取得 */
    sceNetcnfifGetAddr();
    while(sceNetcnfifCheck());
    sceNetcnfifGetResult(&if_arg);
    addr = if_arg.addr;

    for(i = 0, r = 0; i < 3; i++){
	/* 接続プロバイダ設定を設定 */
	sceNetcnfifDataInit(&data);
	switch(i){
	case 0: set_attach_cnf(&data, sceNetcnfifArg_type_ifc, IFC_PATTERN0); break;
	case 1: set_attach_cnf(&data, sceNetcnfifArg_type_ifc, IFC_PATTERN1); break;
	case 2: set_attach_cnf(&data, sceNetcnfifArg_type_ifc, IFC_PATTERN2); break;
	}

	/* sceNetcnfifData を IOP に転送 */
	id = sceNetcnfifSendIOP((unsigned int)&data, (unsigned int)addr, (unsigned int)sizeof(sceNetcnfifData_t));
	while(sceNetcnfifDmaCheck(id));

	/* 接続プロバイダ設定の追加 */
	switch(i){
	case 0: r = add_entry(FNAME, sceNetcnfifArg_type_ifc, IFC_USR_NAME0); break;
	case 1: r = add_entry(FNAME, sceNetcnfifArg_type_ifc, IFC_USR_NAME1); break;
	case 2: r = add_entry(FNAME, sceNetcnfifArg_type_ifc, IFC_USR_NAME2); break;
	}
	if(r < 0) goto done;
    }

    /* 接続プロバイダ設定のリストを取得 */
    if((ifc_num = get_list(FNAME, sceNetcnfifArg_type_ifc, &ifc_list)) <= 0) goto done;

    /* 接続プロバイダ設定のリストを編集する */
    for(i = 0; i < ifc_num; i++){
	if(set_latest_entry(FNAME, sceNetcnfifArg_type_ifc, ifc_list[i].usr_name) < 0) goto done;
    }

    /* 接続プロバイダ設定のリストを取得 */
    if((ifc_num = get_list(FNAME, sceNetcnfifArg_type_ifc, &ifc_list)) <= 0) goto done;

 done:
    sceNetcnfifFreeWorkarea();
    scePrintf("[%s] complete\n", __FILE__);

    return (0);
}
