/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 $Id: add_entry.c,v 1.5 2003/07/18 04:33:49 ksh Exp $
 */
/* 
 * Emotion Engine Library Sample Program
 *
 * Copyright (C) 2002 Sony Computer Entertainment Inc.
 * All Rights Reserved.
 *
 * bnnetcnf/add_entry - add_entry.c
 *	"PlayStation BB Navigator" Network Configuration Read Library Sample Program
 *	main portion
 *
 *      Date            Design      Log
 *  ----------------------------------------------------
 *      2002-10-12      tetsu       first version
 *      2002-10-20      tetsu       correspond to code-review
 */

#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <eekernel.h>
#include <sifrpc.h>
#include <libmc.h>
#include <netcnfif.h>
#include <bnnetcnf.h>

#include "load_module.h"

#define DEBUG /* デバッグプリントを表示しない場合はコメントアウトして下さい */

#define IOPRP         "host0:/usr/local/sce/iop/modules/"IOP_IMAGE_file /* リプレイスモジュールパス */
#define WORKAREA_SIZE (0x1000) /* IOP におけるワークエリアのサイズ */
#define DEVICE        "hdd0:"
#define DEVICE_NAME   DEVICE"__sysconf"
#define FSNAME        "pfs0:"
#define FNAME         "mc0:/BWNETCNF/BWNETCNF" /* 設定管理ファイルパス */
#define NET_USR_NAME  "Combination1" /* 組み合わせ名 */
#define IFC_USR_NAME  "SCE Ethernet Setting" /* 接続プロバイダ設定名 */

static sceNetcnfifArg_t if_arg; /* Netcnf Interface に必要なデータ領域 */

/* プロトタイプ宣言 */
static void *my_malloc(size_t size);
static void my_free(void *addr);
static int add_entry(char *fname, int type, char *usr_name);
static int get_list(char *fname, int type, sceNetcnfifList_t **list);
static int check_capacity(char *fname);

/*
 * 独自仕様の malloc()
 */
static void *my_malloc(size_t size)
{
    void *addr;

    addr = malloc(size);
    scePrintf("[%s] size = %d, addr = 0x%08x\n", __FUNCTION__, size, (u_int)addr);

    return (addr);
}

/*
 * 独自仕様の free()
 */
static void my_free(void *addr)
{
    free(addr);
    scePrintf("[%s] addr = 0x%08x\n", __FUNCTION__, (u_int)addr);
}

/*
 * 設定を追加する
 */
static int add_entry(char *fname, int type, char *usr_name)
{
    sceNetcnfifAddEntry(fname, type, usr_name);
    while (sceNetcnfifCheck());
    sceNetcnfifGetResult(&if_arg);
    if (if_arg.data < 0) {
	scePrintf("[%s] type(%d):sceNetcnfifAddEntry (%d)\n", __FUNCTION__, type, if_arg.data);
	return (if_arg.data);
    } else {
	return (0);
    }
}

/*
 * 設定のリストを取得する
 */
static int get_list(char *fname, int type, sceNetcnfifList_t **list)
{
    int r;

    /* 数を取得 */
    sceNetcnfifGetCount(fname, type);
    while (sceNetcnfifCheck());
    sceNetcnfifGetResult(&if_arg);
    if (if_arg.data <= 0) {
	scePrintf("[%s] type(%d):sceNetcnfifGetCount (%d)\n", __FUNCTION__, type, if_arg.data);
	return (if_arg.data);
    }
    r = if_arg.data;

    /* リストを取得 */
    *list = (sceNetcnfifList_t *)memalign(64, r * sizeof(sceNetcnfifList_t));
    if (*list == NULL) {
	scePrintf("[%s] type(%d):memalign (0x%08x)\n", __FUNCTION__, type, *list);
	return (sceNETCNFIF_ALLOC_ERROR);
    }
    FlushCache(0);
    sceNetcnfifGetList(fname, type, *list, r);
    while (sceNetcnfifCheck());
    sceNetcnfifGetResult(&if_arg);
    if (if_arg.data < 0) {
	scePrintf("[%s] type(%d):sceNetcnfifGetList (%d)\n", __FUNCTION__, type, if_arg.data);
	return (if_arg.data);
    }

#ifdef DEBUG
    {
	int i;
	scePrintf("[%s] get_list result\n", __FUNCTION__);
	for (i = 0; i < r; i++) {
	    scePrintf("%d,%d,%s,%s\n", (*list + i)->type, (*list + i)->stat, (*list + i)->sys_name, (*list + i)->usr_name);
	}
    }
#endif /* DEBUG */

    return (r);
}

/*
 * デバイスの残り容量をチェックする
 */
static int check_capacity(char *fname)
{
    int r;

    sceNetcnfifCheckCapacity(fname);
    while (sceNetcnfifCheck());
    sceNetcnfifGetResult(&if_arg);
    if (if_arg.data < 0) {
	scePrintf("[%s] sceNetcnfifCheckCapacity (%d)\n", __FUNCTION__, if_arg.data);
	return (if_arg.data);
    }
    r = if_arg.data;

    return (r);
}

/*
 * IOP 側のワークエリアを確保する
 */
static int alloc_workarea(void)
{
    int r;

    sceNetcnfifAllocWorkarea(WORKAREA_SIZE);
    while (sceNetcnfifCheck());
    sceNetcnfifGetResult(&if_arg);
    if (if_arg.data < 0) {
	scePrintf("[%s] sceNetcnfifAllocWorkarea (%d)\n", __FUNCTION__, if_arg.data);
	return (if_arg.data);
    }
    r = if_arg.data;

    return (r);
}

/*
 * 必要な初期化処理
 */
int initialize(void)
{
    int r;

    sceSifInitRpc(0);
    while (sceSifRebootIop(IOPRP) == 0); /* IOP リブート・モジュール置き換え */
    while (sceSifSyncIop() == 0);        /* 終了を待つ */
    sceSifInitRpc(0);
    sceSifLoadFileReset();
    sceFsReset();

    /* 必要なモジュールをロード */
    load_module();

    /* DEVICE_NAME を FSNAME にマウント */
    if ((r = sceDevctl(DEVICE, HDIOC_STATUS, NULL, 0, NULL, 0)) == 0) {
	if ((r = sceMount(FSNAME, DEVICE_NAME, SCE_MT_RDONLY, NULL, 0)) < 0) {
	    scePrintf("[%s] sceMount (%d)\n", __FUNCTION__, r);
	    return (r);
	}
    } else {
	scePrintf("[%s] sceDevctl (%d)\n", __FUNCTION__, r);
	return (r);
    }

    /* Sifrpc サーバとバインド */
    sceNetcnfifInit();
    sceNetcnfifSetup();

    /* メモリーカード環境の初期化 */
    if ((r = sceMcInit()) < 0) {
	scePrintf("[%s] sceMcInit (%d)\n", __FUNCTION__, r);
	return (r);
    }

    return (0);
}

int main(void)
{
    static sceNetcnfifData_t data __attribute__((aligned(64)));
    static sceNetcnfifList_t *dev_list;
    static sceNetcnfifList_t *ifc_list;
    char                     tmp_str[256];
    int                      dev_num;
    int                      ifc_num;
    int                      addr;
    int                      id;
    int                      r;

    /* 必要な初期化処理 */
    if (initialize() < 0) goto done;

    /* デバイスの残り容量をチェックする */
    if (check_capacity(FNAME) < 0) goto done;

    /* IOP 側のワークエリアを確保 */
    if (alloc_workarea() < 0) goto done;

    /* sceNetcnfifData の転送先アドレスを取得 */
    sceNetcnfifGetAddr();
    while (sceNetcnfifCheck());
    sceNetcnfifGetResult(&if_arg);
    addr = if_arg.addr;

    /* "PlayStation BB Navigator" のネットワーク設定を読み込む */
    r = sceBnnetcnfRead(FSNAME, (SceBnnetcnfData *)&data, my_malloc, my_free);
    if (r < 0) {
	scePrintf("[%s] sceBnnetcnfRead (%d)\n", __FUNCTION__, -SCE_ERROR_ERRNO(r));
	goto done;
    }
#ifdef DEBUG
    sceNetcnfifDataDump(&data);
#endif /* DEBUG */

    /* sceNetcnfifData を IOP に転送 */
    id = sceNetcnfifSendIOP((u_int)&data, (u_int)addr, (u_int)sizeof(sceNetcnfifData_t));
    while (sceNetcnfifDmaCheck(id));

    /* 接続機器設定の追加 */
    sprintf(tmp_str, "%s/%s", data.vendor, data.product);
    if (add_entry(FNAME, sceNetcnfifArg_type_dev, tmp_str) < 0) goto done;

    /* 接続プロバイダ設定の追加 */
    if (add_entry(FNAME, sceNetcnfifArg_type_ifc, IFC_USR_NAME) < 0) goto done;

    /* 接続機器設定のリストを取得 */
    if ((dev_num = get_list(FNAME, sceNetcnfifArg_type_dev, &dev_list)) <= 0) goto done;

    /* 接続プロバイダ設定のリストを取得 */
    if ((ifc_num = get_list(FNAME, sceNetcnfifArg_type_ifc, &ifc_list)) <= 0) goto done;

    /* 組み合わせを設定 */
    sceNetcnfifDataInit(&data);
    strcpy(data.attach_ifc, ifc_list[0].sys_name);
    strcpy(data.attach_dev, dev_list[0].sys_name);

    /* sceNetcnfifData を IOP に転送 */
    id = sceNetcnfifSendIOP((u_int)&data, (u_int)addr, (u_int)sizeof(sceNetcnfifData_t));
    while (sceNetcnfifDmaCheck(id));

    /* 組み合わせの追加 */
    if (add_entry(FNAME, sceNetcnfifArg_type_net, NET_USR_NAME) < 0) goto done;

 done:
    sceNetcnfifFreeWorkarea();
    scePrintf("[add_entry] complete\n");

    return (0);
}
