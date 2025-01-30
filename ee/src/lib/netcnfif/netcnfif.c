/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
 */
/* 
 *      Netcnf Interface Library
 *
 *                         Version 1.4
 *                         Shift-JIS
 *
 *      Copyright (C) 2002 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                         netcnfif.c
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.1            2002.01.28      tetsu       First version
 *       1.2            2002.01.31      tetsu       Add release version string
 *       1.3            2002.02.10      tetsu       Add sceNetcnfifCheckAdditionalAT()
 *       1.4            2002.05.05      tetsu       Add sceNetcnfifCheckSpecialProvider()
 */

#include <stdio.h>
#include <string.h>
#include <eetypes.h>
#include <sifrpc.h>

#include "../../../../common/include/netcnfif.h"
#include "netcnfif_data.h"
#include "dma.h"

/* Sifrpc 用 */
static sceSifClientData cd;
static u_char buf[SCE_NETCNFIF_SSIZE] __attribute__((aligned(64)));

/* no_decode フラグ */
static int _f_no_decode = 0;

/* Sifrpc 用送受信バッファの状態を初期値に戻す */
void sceNetcnfifInit(void)
{
    sceNetcnfifArg_t *p;

    p = (sceNetcnfifArg_t *)buf;

    bzero(p, sizeof(sceNetcnfifArg_t));
}

/* Sifrpc サーバとバインド */
void sceNetcnfifSetup(void)
{
    int i;

    while(1){
	if(sceSifBindRpc(&cd, SCE_NETCNFIF_INTERFACE, SIF_RPCM_NOWAIT) < 0){
	    while(1);
	}
	while(sceSifCheckStatRpc((sceSifRpcData *)&cd) != 0);
	if(cd.serve != 0) break;
	i = 0x10000;
	while(i --);
    }
}

/* Sifrpc 処理状況の判定(netcnf_if) */
int sceNetcnfifCheck(void)
{
    return (sceSifCheckStatRpc((sceSifRpcData *)&cd));
}

/* Sifrpc サービスの結果を取得 */
void sceNetcnfifGetResult(sceNetcnfifArg_t *result)
{
    sceNetcnfifArg_t *p;

    p = (sceNetcnfifArg_t *)buf;
    memcpy(result, p, sizeof(sceNetcnfifArg_t));
}

/* no_decode フラグを設定する */
void sceNetcnfifSetFNoDecode(int f_no_decode)
{
    _f_no_decode = f_no_decode;
}

/* ネットワーク設定ファイルの数を取得 */
void sceNetcnfifGetCount(char *fname, int type)
{
    sceNetcnfifArg_t *p;

    sceNetcnfifInit();
    p = (sceNetcnfifArg_t *)buf;

    strcpy(p->fname, fname);
    p->type = type;

    sceSifCallRpc(&cd, SCE_NETCNFIF_GET_COUNT, SIF_RPCM_NOWAIT, &buf[0], SCE_NETCNFIF_SSIZE, &buf[0], SCE_NETCNFIF_SSIZE, 0, 0);
}

/* ネットワーク設定ファイルのリストを取得 */
void sceNetcnfifGetList(char *fname, int type, sceNetcnfifList_t *addr, int size)
{
    sceNetcnfifArg_t *p;

    sceNetcnfifInit();
    p = (sceNetcnfifArg_t *)buf;

    strcpy(p->fname, fname);
    p->type = type;
    p->addr = (int)addr;
    p->data = size;

    sceSifCallRpc(&cd, SCE_NETCNFIF_GET_LIST, SIF_RPCM_NOWAIT, &buf[0], SCE_NETCNFIF_SSIZE, &buf[0], SCE_NETCNFIF_SSIZE, 0, 0);
}

/* ネットワーク設定ファイルの内容を取得 */
void sceNetcnfifLoadEntry(char *fname, int type, char *usr_name, sceNetcnfifData_t *addr)
{
    sceNetcnfifArg_t *p;

    sceNetcnfifInit();
    p = (sceNetcnfifArg_t *)buf;

    strcpy(p->fname, fname);
    p->type = type;
    strcpy(p->usr_name, usr_name);
    p->addr = (int)addr;
    p->f_no_decode = _f_no_decode;

    sceSifCallRpc(&cd, SCE_NETCNFIF_LOAD_ENTRY, SIF_RPCM_NOWAIT, &buf[0], SCE_NETCNFIF_SSIZE, &buf[0], SCE_NETCNFIF_SSIZE, 0, 0);
}

/* ネットワーク設定ファイルの追加 */
void sceNetcnfifAddEntry(char *fname, int type, char *usr_name)
{
    sceNetcnfifArg_t *p;

    sceNetcnfifInit();
    p = (sceNetcnfifArg_t *)buf;

    strcpy(p->fname, fname);
    p->type = type;
    strcpy(p->usr_name, usr_name);
    p->f_no_decode = _f_no_decode;

    sceSifCallRpc(&cd, SCE_NETCNFIF_ADD_ENTRY, SIF_RPCM_NOWAIT, &buf[0], SCE_NETCNFIF_SSIZE, &buf[0], SCE_NETCNFIF_SSIZE, 0, 0);
}

/* ネットワーク設定ファイルの編集 */
void sceNetcnfifEditEntry(char *fname, int type, char *usr_name, char *new_usr_name)
{
    sceNetcnfifArg_t *p;

    sceNetcnfifInit();
    p = (sceNetcnfifArg_t *)buf;

    strcpy(p->fname, fname);
    p->type = type;
    strcpy(p->usr_name, usr_name);
    strcpy(p->new_usr_name, new_usr_name);
    p->f_no_decode = _f_no_decode;

    sceSifCallRpc(&cd, SCE_NETCNFIF_EDIT_ENTRY, SIF_RPCM_NOWAIT, &buf[0], SCE_NETCNFIF_SSIZE, &buf[0], SCE_NETCNFIF_SSIZE, 0, 0);
}

/* ネットワーク設定ファイルの削除 */
void sceNetcnfifDeleteEntry(char *fname, int type, char *usr_name)
{
    sceNetcnfifArg_t *p;

    sceNetcnfifInit();
    p = (sceNetcnfifArg_t *)buf;

    strcpy(p->fname, fname);
    p->type = type;
    strcpy(p->usr_name, usr_name);

    sceSifCallRpc(&cd, SCE_NETCNFIF_DELETE_ENTRY, SIF_RPCM_NOWAIT, &buf[0], SCE_NETCNFIF_SSIZE, &buf[0], SCE_NETCNFIF_SSIZE, 0, 0);
}

/* ネットワーク設定ファイルのリストを編集 */
void sceNetcnfifSetLatestEntry(char *fname, int type, char *usr_name)
{
    sceNetcnfifArg_t *p;

    sceNetcnfifInit();
    p = (sceNetcnfifArg_t *)buf;

    strcpy(p->fname, fname);
    p->type = type;
    strcpy(p->usr_name, usr_name);

    sceSifCallRpc(&cd, SCE_NETCNFIF_SET_LATEST_ENTRY, SIF_RPCM_NOWAIT, &buf[0], SCE_NETCNFIF_SSIZE, &buf[0], SCE_NETCNFIF_SSIZE, 0, 0);
}

/* あなたのネットワーク設定ファイルを削除 */
void sceNetcnfifDeleteAll(char *fname)
{
    sceNetcnfifArg_t *p;

    sceNetcnfifInit();
    p = (sceNetcnfifArg_t *)buf;

    strcpy(p->fname, fname);

    sceSifCallRpc(&cd, SCE_NETCNFIF_DELETE_ALL, SIF_RPCM_NOWAIT, &buf[0], SCE_NETCNFIF_SSIZE, &buf[0], SCE_NETCNFIF_SSIZE, 0, 0);
}

/* デバイスの残り容量をチェック */
void sceNetcnfifCheckCapacity(char *fname)
{
    sceNetcnfifArg_t *p;

    sceNetcnfifInit();
    p = (sceNetcnfifArg_t *)buf;

    strcpy(p->fname, fname);

    sceSifCallRpc(&cd, SCE_NETCNFIF_CHECK_CAPACITY, SIF_RPCM_NOWAIT, &buf[0], SCE_NETCNFIF_SSIZE, &buf[0], SCE_NETCNFIF_SSIZE, 0, 0);
}

/* 追加 AT コマンドをチェック */
void sceNetcnfifCheckAdditionalAT(char *additional_at)
{
    sceNetcnfifArg_t *p;

    sceNetcnfifInit();
    p = (sceNetcnfifArg_t *)buf;

    strcpy(p->fname, additional_at);

    sceSifCallRpc(&cd, SCE_NETCNFIF_CHECK_ADDITIONAL_AT, SIF_RPCM_NOWAIT, &buf[0], SCE_NETCNFIF_SSIZE, &buf[0], SCE_NETCNFIF_SSIZE, 0, 0);
}

/* 特殊プロバイダの設定であるかどうかをチェック */
void sceNetcnfifCheckSpecialProvider(char *fname, int type, char *usr_name)
{
    sceNetcnfifArg_t *p;

    sceNetcnfifInit();
    p = (sceNetcnfifArg_t *)buf;

    strcpy(p->fname, fname);
    p->type = type;
    strcpy(p->usr_name, usr_name);
    p->f_no_decode = _f_no_decode;

    sceSifCallRpc(&cd, SCE_NETCNFIF_CHECK_SPECIAL_PROVIDER, SIF_RPCM_NOWAIT, &buf[0], SCE_NETCNFIF_SSIZE, &buf[0], SCE_NETCNFIF_SSIZE, 0, 0);
}

/* IOP 側の受信領域(sceNetcnfifData)のアドレスを取得 */
void sceNetcnfifGetAddr(void)
{
    sceNetcnfifInit();
    sceSifCallRpc(&cd, SCE_NETCNFIF_GET_ADDR, SIF_RPCM_NOWAIT, &buf[0], SCE_NETCNFIF_SSIZE, &buf[0], SCE_NETCNFIF_SSIZE, 0, 0);
}

/* IOP 側のワークエリアを確保 */
void sceNetcnfifAllocWorkarea(int size)
{
    sceNetcnfifArg_t *p;

    sceNetcnfifInit();
    p = (sceNetcnfifArg_t *)buf;

    p->data = size;

    sceSifCallRpc(&cd, SCE_NETCNFIF_ALLOC_WORKAREA, SIF_RPCM_NOWAIT, &buf[0], SCE_NETCNFIF_SSIZE, &buf[0], SCE_NETCNFIF_SSIZE, 0, 0);
}

/* IOP 側のワークエリアを解放 */
void sceNetcnfifFreeWorkarea(void)
{
    sceNetcnfifInit();
    sceSifCallRpc(&cd, SCE_NETCNFIF_FREE_WORKAREA, SIF_RPCM_NOWAIT, &buf[0], SCE_NETCNFIF_SSIZE, &buf[0], SCE_NETCNFIF_SSIZE, 0, 0);
}

/* IOP 側の sceNetCnfEnv 領域に sceNetcnfifData の内容を設定 */
void sceNetcnfifSetEnv(int type)
{
    sceNetcnfifArg_t *p;

    sceNetcnfifInit();
    p = (sceNetcnfifArg_t *)buf;

    p->type = type;
    p->f_no_decode = _f_no_decode;

    sceSifCallRpc(&cd, SCE_NETCNFIF_SET_ENV, SIF_RPCM_NOWAIT, &buf[0], SCE_NETCNFIF_SSIZE, &buf[0], SCE_NETCNFIF_SSIZE, 0, 0);
}
