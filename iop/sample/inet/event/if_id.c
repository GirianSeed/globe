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
 *                         if_id.c
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.1            2001.12.28      tetsu       First version
 *       1.2            2002.02.05      tetsu       Change if_id_stat
 *                                                  Change search_if_id()
 *                                                  Add search_eth_if_id()
 *                                                  Add get_if_id_flag()
 *                                                  Change add_if_id()
 */

#include <string.h>
#include <kernel.h>
#include <inet/netdev.h>

#include "if_id.h"

typedef struct if_id_stat{
    int id;                    /* インタフェース ID */
    int stat;                  /* インタフェース ID の状態 */
    int flag;                  /* デバイスレイヤの種別を表すフラグ */
    unsigned char bus_type;    /* バス種別 */
    unsigned char bus_loc[31]; /* デバイス位置情報 */
    char vendor[256];          /* ベンダ名 */
    char product[256];         /* プロダクト名 */
} if_id_stat_t;

static if_id_stat_t *if_id; /* インタフェース状態を保持するバッファへのポインタ */
static int if_id_num = 0;   /* 保持しているインタフェース状態の数 */

/* インタフェース ID を検索する */
int search_if_id(int id)
{
    int i;

    /* インタフェース ID が存在する場合はインデックスを返す */
    for(i = 0; i < if_id_num; i++){
	if(if_id[i].id == id) return (i);
    }

    /* インタフェース ID が存在しない場合は -1 を返す */
    return (-1);
}

/* PPPoE 用 ppp I/F と、同時に使用している eth I/F の id を検索する */
int search_eth_if_id(int id)
{
    int i, index;

    /* インタフェース ID が存在しない場合は -1 を返す */
    if((index = search_if_id(id)) < 0) return (-1);

    for(i = 0; i < if_id_num; i++){
	if(if_id[index].bus_type == sceInetBus_USB){
	    if(memcmp(if_id[index].bus_loc, if_id[i].bus_loc, sizeof(if_id[index].bus_loc)) == 0 &&
	       memcmp(if_id[index].vendor, if_id[i].vendor, sizeof(if_id[index].vendor)) == 0 &&
	       memcmp(if_id[index].product, if_id[i].product, sizeof(if_id[index].product)) == 0 &&
	       if_id[i].flag == ARP) return (if_id[i].id);
	}else{
	    if(memcmp(if_id[index].vendor, if_id[i].vendor, sizeof(if_id[index].vendor)) == 0 &&
	       memcmp(if_id[index].product, if_id[i].product, sizeof(if_id[index].product)) == 0 &&
	       if_id[i].flag == ARP) return (if_id[i].id);
	}
    }

    /* 該当する eth I/F の id が存在しない場合は -1 を返す */
    return (-1);
}

/* インタフェース ID の状態を取得する */
int get_if_id_stat(int id)
{
    int i;

    /* インタフェース ID が存在する場合はインデックスを返す */
    for(i = 0; i < if_id_num; i++){
	if(if_id[i].id == id) return (if_id[i].stat);
    }

    /* インタフェース ID が存在しない場合は -1 を返す */
    return (-1);
}

/* デバイスレイヤの種別を表すフラグを取得する */
int get_if_id_flag(int id)
{
    int i;

    /* インタフェース ID が存在する場合はインデックスを返す */
    for(i = 0; i < if_id_num; i++){
	if(if_id[i].id == id) return (if_id[i].flag);
    }

    /* インタフェース ID が存在しない場合は -1 を返す */
    return (-1);
}

/* インタフェース ID を追加する */
int add_if_id(int id, int stat, int flag, unsigned char bus_type, unsigned char *bus_loc, char *vendor, char *product)
{
    static int oldstat;
    if_id_stat_t *new_if_id;

    /* インタフェース ID が存在する場合は -1 を返す */
    if(search_if_id(id) >= 0) return (-1);

    CpuSuspendIntr(&oldstat);
    if(if_id_num){
	/* インタフェース ID 数をひとつ増やす */
	if_id_num++;

	/* 新しい領域を確保、コピー */
	new_if_id = (if_id_stat_t *)AllocSysMemory(SMEM_Low, sizeof(if_id_stat_t) * if_id_num, NULL);
	memcpy((void *)new_if_id, (void *)if_id, sizeof(if_id_stat_t) * (if_id_num - 1));
	FreeSysMemory(if_id);

	/* インタフェース ID を追加 */
	new_if_id[if_id_num - 1].id = id;
	new_if_id[if_id_num - 1].stat = stat;
	new_if_id[if_id_num - 1].flag = flag;
	new_if_id[if_id_num - 1].bus_type = bus_type;
	memcpy(new_if_id[if_id_num - 1].bus_loc, bus_loc, sizeof(new_if_id[if_id_num - 1].bus_loc));
	memcpy(new_if_id[if_id_num - 1].vendor, vendor, sizeof(new_if_id[if_id_num - 1].vendor));
	memcpy(new_if_id[if_id_num - 1].product, product, sizeof(new_if_id[if_id_num - 1].product));

	/* ポインタを更新 */
	if_id = new_if_id;
    }else{
	/* インタフェース ID 数をひとつ増やす */
	if_id_num++;

	/* 新しい領域を確保 */
	if_id = (if_id_stat_t *)AllocSysMemory(SMEM_Low, sizeof(if_id_stat_t) * if_id_num, NULL);

	/* インタフェース ID を追加 */
	if_id[if_id_num - 1].id   = id;
	if_id[if_id_num - 1].stat = stat;
	if_id[if_id_num - 1].flag = flag;
	if_id[if_id_num - 1].bus_type = bus_type;
	memcpy(if_id[if_id_num - 1].bus_loc, bus_loc, sizeof(if_id[if_id_num - 1].bus_loc));
	memcpy(if_id[if_id_num - 1].vendor, vendor, sizeof(if_id[if_id_num - 1].vendor));
	memcpy(if_id[if_id_num - 1].product, product, sizeof(if_id[if_id_num - 1].product));
    }
    CpuResumeIntr(oldstat);

    return (0);
}

/* インタフェース ID の状態を変更する */
int change_if_id(int id, int stat)
{
    int index;

    /* インタフェース ID が存在しない場合は -1 を返す */
    if((index = search_if_id(id)) < 0) return (-1);

    /* インタフェース ID の状態を変更する */
    if_id[index].stat = stat;

    return (0);
}

/* インタフェース ID を削除する */
int delete_if_id(int id)
{
    static int oldstat;
    int index, i, j;
    if_id_stat_t *new_if_id;

    /* インタフェース ID が存在しない場合は -1 を返す */
    if((index = search_if_id(id)) < 0) return (-1);

    /* インタフェース ID をひとつ減らす */
    if_id_num--;

    CpuSuspendIntr(&oldstat);
    if(if_id_num){
	/* 新しい領域を確保 */
	new_if_id = (if_id_stat_t *)AllocSysMemory(SMEM_Low, sizeof(if_id_stat_t) * if_id_num, NULL);

	/* インタフェース ID を削除 */
	for(i = 0, j = 0; i < if_id_num + 1; i++){
	    if(i != index){
		new_if_id[j].id   = if_id[i].id;
		new_if_id[j].stat = if_id[i].stat;
		new_if_id[j].flag = if_id[i].flag;
		new_if_id[j].bus_type = if_id[i].bus_type;
		memcpy(new_if_id[j].bus_loc, if_id[i].bus_loc, sizeof(new_if_id[j].bus_loc));
		memcpy(new_if_id[j].vendor, if_id[i].vendor, sizeof(new_if_id[j].vendor));
		memcpy(new_if_id[j].product, if_id[i].product, sizeof(new_if_id[j].product));
		j++;
	    }
	}

	/* 元の領域を解放 */
	FreeSysMemory(if_id);

	/* ポインタを更新 */
	if_id = new_if_id;
    }else{
	/* 領域を解放 */
	FreeSysMemory(if_id);
    }
    CpuResumeIntr(oldstat);

    return (0);
}
