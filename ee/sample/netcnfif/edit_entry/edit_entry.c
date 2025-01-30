/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
 */
/* 
 *      Netcnf Interface Library Sample Program
 *
 *                         Version 1.4
 *                         Shift-JIS
 *
 *      Copyright (C) 2002 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                         edit_entry.c
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.1            2002.01.28      tetsu       First version
 *       1.2            2002.01.31      tetsu       Add get_dev_info()
 *       1.3            2002.02.10      tetsu       Add check_additional_at()
 *       1.4            2002.05.05      tetsu       Add check_special_provider()
 */

#include <stdio.h>
#include <malloc.h>
#include <eekernel.h>
#include <sifrpc.h>
#include <libmc.h>
#include <netcnfif.h>
#include <libnet.h>
#include <libmrpc.h>

#include "load_module.h"
#include "set_configuration.h"

#define DEBUG /* デバッグプリントを表示しない場合はコメントアウトして下さい */

#define IOPRP "host0:../../../../iop/modules/"IOP_IMAGE_file /* リプレイスモジュールパス */

#define WORKAREA_SIZE (0x1000) /* IOP におけるワークエリアのサイズ */

#define FNAME "mc0:/BWNETCNF/BWNETCNF"      /* 設定管理ファイルパス */
#define IFC_NEW_USR_NAME "Ethernet Setting" /* 新しい接続プロバイダ設定名 */

#define DEV_PATTERN (1) /* 接続機器設定パターン */
#define IFC_PATTERN (3) /* 接続プロバイダ設定パターン */

static sceNetcnfifArg_t if_arg; /* Netcnf Interface に必要なデータ領域 */

/* 編集する設定が特殊プロバイダのものかどうかチェックする */
static int check_special_provider(char *fname, int type, char *usr_name)
{
    sceNetcnfifCheckSpecialProvider(fname, type, usr_name);
    while(sceNetcnfifCheck());
    sceNetcnfifGetResult(&if_arg);
    if(if_arg.data < 0){
	scePrintf("[%s] type(%d):sceNetcnfifCheckSpecialProvider (%d)\n", __FILE__, type, if_arg.data);
	return (if_arg.data);
    }else{
	return (0);
    }
}

/* 設定を編集する */
static int edit_entry(char *fname, int type, char *usr_name, char *new_usr_name)
{
    sceNetcnfifEditEntry(fname, type, usr_name, new_usr_name);
    while(sceNetcnfifCheck());
    sceNetcnfifGetResult(&if_arg);
    if(if_arg.data < 0){
	scePrintf("[%s] type(%d):sceNetcnfifEditEntry (%d)\n", __FILE__, type, if_arg.data);
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

/* 追加 AT コマンドをチェックする */
static int check_additional_at(char *additional_at)
{
    if(*additional_at != '\0'){
	sceNetcnfifCheckAdditionalAT(additional_at);
	while(sceNetcnfifCheck());
	sceNetcnfifGetResult(&if_arg);
	if(if_arg.data < 0){
	    scePrintf("[%s] sceNetcnfifCheckAdditionalAT (%d)\n", __FILE__, if_arg.data);
	    return (if_arg.data);
	}
    }
    return (0);
}

/* デバイスレイヤの種別、ベンダ名、プロダクト名を取得する */
static int get_dev_info(sceSifMClientData *cd, unsigned int *net_buf, int *dev_type, char *vendor, int vendor_size, char *product, int product_size)
{
    int interface_id_num, interface_id_list[256], i, r;

    /* 全てのネットワークインタフェースリストの数と ID を取得 */
    if((interface_id_num = sceInetGetInterfaceList(cd, net_buf, interface_id_list, 256)) <= 0){
	scePrintf("[%s] sceInetGetInterfaceList (%d)\n", __FILE__, interface_id_num);
	return (interface_id_num);
    }

    /* 全てのネットワークインタフェースリストと既設定をチェック
       (最後に発見された接続機器が有効になるため、使用する接続機器以外の接続機器が接続されていた場合
        意図しない設定ができる可能性があります。) */
    for(i = 0; i < interface_id_num; i++){
	/* デバイスレイヤの種別を取得 */
	if((r = sceInetInterfaceControl(cd, net_buf, interface_id_list[i], (int)sceInetCC_GetFlags, dev_type, (int)sizeof(int))) < 0){
	    scePrintf("[%s] sceInetCC_GetFlags (%d)\n", __FILE__, r);
	    return (r);
	}
	if(*dev_type & sceInetDevF_ARP){
	    if(*dev_type & sceInetDevF_NIC){
		*dev_type  = sceNetcnfifData_type_nic;
	    }else{
		*dev_type  = sceNetcnfifData_type_eth;
	    }
	}else{
	    *dev_type  = sceNetcnfifData_type_ppp;
	}

	/* ベンダ名を取得 */
	if((r = sceInetInterfaceControl(cd, net_buf, interface_id_list[i], (int)sceInetCC_GetVendorName, vendor, vendor_size)) < 0){
	    scePrintf("[%s] sceInetCC_GetVendorName (%d)\n", __FILE__, r);
	    return (r);
	}

	/* プロダクト名を取得 */
	if((r = sceInetInterfaceControl(cd, net_buf, interface_id_list[i], (int)sceInetCC_GetDeviceName, product, product_size)) < 0){
	    scePrintf("[%s] sceInetCC_GetDeviceName (%d)\n", __FILE__, r);
	    return (r);
	}
    }

    return (interface_id_num);
}

int main(void)
{
    static sceNetcnfifData_t data __attribute__((aligned(64)));
    static sceNetcnfifList_t *dev_list, *ifc_list;
    static sceSifMClientData cd;
    static unsigned int g_net_buf[512] __attribute__((aligned(64)));
    char tmp_str[256];
    int dev_num, ifc_num;
    int addr, id, r;

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

    /* Libnet 関係関数の初期化 */
    sceSifMInitRpc(0);
    if((r = sceLibnetInitialize(&cd, sceLIBNET_BUFFERSIZE, sceLIBNET_STACKSIZE, sceLIBNET_PRIORITY)) < 0){
	scePrintf( "[%s] sceLibnetInitialize (%d)\n", __FILE__, r);
	goto done;
    }
    if((r = sceLibnetRegisterHandler(&cd, g_net_buf)) < 0){
	scePrintf( "[%s] sceLibnetRegisterHandler (%d)\n", __FILE__, r);
	goto done;
    }

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

    /* 接続機器設定のリストを取得 */
    if((dev_num = get_list(FNAME, sceNetcnfifArg_type_dev, &dev_list)) <= 0) goto done;

    /* 接続プロバイダ設定のリストを取得 */
    if((ifc_num = get_list(FNAME, sceNetcnfifArg_type_ifc, &ifc_list)) <= 0) goto done;

    /* f_no_decode を設定(タイトルに使用する場合は必ず
       sceNetcnfifArg_f_no_decode_off に設定すること) */
    sceNetcnfifSetFNoDecode(sceNetcnfifArg_f_no_decode_on);

    /* sceNetcnfifData の転送先アドレスを取得 */
    sceNetcnfifGetAddr();
    while(sceNetcnfifCheck());
    sceNetcnfifGetResult(&if_arg);
    addr = if_arg.addr;

    /* 接続機器設定と接続プロバイダ設定を設定 */
    sceNetcnfifDataInit(&data);
    set_attach_cnf(&data, sceNetcnfifArg_type_dev, DEV_PATTERN);
    if(check_additional_at(data.chat_additional) < 0) goto done;
    set_attach_cnf(&data, sceNetcnfifArg_type_ifc, IFC_PATTERN);

    /* デバイスレイヤの種別、ベンダ名、プロダクト名を取得する */
    if(get_dev_info(&cd, g_net_buf, &data.dev_type, data.vendor, sizeof(data.vendor), data.product, sizeof(data.product)) <= 0) goto done;
#ifdef DEBUG
    sceNetcnfifDataDump(&data);
#endif /* DEBUG */

    /* sceNetcnfifData を IOP に転送 */
    id = sceNetcnfifSendIOP((unsigned int)&data, (unsigned int)addr, (unsigned int)sizeof(sceNetcnfifData_t));
    while(sceNetcnfifDmaCheck(id));

    /* 特殊プロバイダの設定かどうかチェックする */
    if(check_special_provider(FNAME, sceNetcnfifArg_type_ifc, ifc_list[0].usr_name) < 0) goto done;

    /* 接続機器設定の編集 */
    sprintf(tmp_str, "%s/%s", data.vendor, data.product);
    if(edit_entry(FNAME, sceNetcnfifArg_type_dev, dev_list[0].usr_name, tmp_str) < 0) goto done;

    /* 接続プロバイダ設定の編集 */
    if(edit_entry(FNAME, sceNetcnfifArg_type_ifc, ifc_list[0].usr_name, IFC_NEW_USR_NAME) < 0) goto done;

 done:
    sceNetcnfifFreeWorkarea();
    scePrintf("[%s] complete\n", __FILE__);

    return (0);
}
