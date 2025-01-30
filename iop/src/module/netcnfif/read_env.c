/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 $Id: read_env.c,v 1.4 2002/10/19 12:32:57 tetsu Exp $
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
 *                         read_env.c
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.1            2002.01.28      tetsu       First version
 *       1.2            2002.01.31      tetsu       Modified get_attach()'s bug
 *       1.3            2002.02.10      tetsu       Correspond to sceNetCnfConvS2A()
 *       1.4            2002.10.19      tetsu       Change path of netcnfif.h
 */

#include <stdio.h>
#include <string.h>
#include <netcnf.h>
#include <netcnfif.h>

static int ns_count;

static int get_cmd(sceNetcnfifData_t *data, sceNetCnfCommand_t *p, int *ns_count)
{
    int r = 0;
    sceNetCnfRoutingEntry_t *re;

    switch(p->code){
    case sceNetCnf_CMD_ADD_NAMESERVER:
	switch(*ns_count){
	case 0: /* プライマリ DNS */
	    r = sceNetCnfAddress2String(data->dns1_address, sizeof(data->dns1_address), (sceNetCnfAddress_t *)(p + 1));
	    *ns_count = *ns_count + 1;
	    break;
	case 1: /* セカンダリ DNS */
	    r = sceNetCnfAddress2String(data->dns2_address, sizeof(data->dns2_address), (sceNetCnfAddress_t *)(p + 1));
	    *ns_count = *ns_count + 1;
	    break;
	}
	break;
    case sceNetCnf_CMD_ADD_ROUTING:
	re = (sceNetCnfRoutingEntry_t *)(p + 1);
	r = sceNetCnfAddress2String(data->gateway, sizeof(data->gateway), &re->gateway); /* Default Gateway */
	break;
    }

    return (r);
}

static int get_attach(sceNetcnfifData_t *data, sceNetCnfInterface_t *p, int type)
{
    int i, r = 0;
    char *str;
    sceNetCnfCommand_t *cmd;

    switch(type){
    case 1:
	/* デバイスレイヤの種別 */
	data->ifc_type = p->type;

	/* DHCP を使用する・しない */
	data->dhcp = p->dhcp;

	/* DHCP 用ホスト名 */
	if(p->dhcp_host_name != NULL) strcpy(data->dhcp_host_name, p->dhcp_host_name);

	/* IP アドレス */
	if(p->address != NULL) strcpy(data->address, p->address);

	/* ネットマスク */
	if(p->netmask != NULL) strcpy(data->netmask, p->netmask);

	/* デフォルトルータ(最後のものが有効), プライマリ DNS/セカンダリ DNS(最初の２つが有効) */
	for(ns_count = 0, cmd = p->cmd_head; cmd != NULL; cmd = cmd->forw){
	    r = get_cmd(data, cmd, &ns_count);
	    if(r < 0) return (r);
	}

	/* 接続先電話番号1-3(最初から３番目までの電話番号が有効) */
	for(i = 0; i < sceNetCnf_MAX_PHONE_NUMBERS; i++){
	    if(NULL == (str = p->phone_numbers[i])) continue;
	    switch(i){
	    case 0:
		strcpy(data->phone_numbers1, str);
		break;
	    case 1:
		strcpy(data->phone_numbers2, str);
		break;
	    case 2:
		strcpy(data->phone_numbers3, str);
		break;
	    }
	}

	/* ユーザ ID */
	if(p->auth_name != NULL) strcpy(data->auth_name, p->auth_name);

	/* パスワード */
	if(p->auth_key != NULL) strcpy(data->auth_key, p->auth_key);

	/* 接続先の認証名 */
	if(p->peer_name != NULL) strcpy(data->peer_name, p->peer_name);

	/* DNS サーバアドレスを自動取得する・しない(プライマリ DNS) */
	data->dns1_nego = p->want.dns1_nego;

	/* DNS サーバアドレスを自動取得する・しない(セカンダリ DNS) */
	data->dns2_nego = p->want.dns2_nego;

	/* 認証方法を設定する・しない */
	data->f_auth = p->allow.f_auth;

	/* 認証方法 */
	data->auth = p->allow.auth;

	/* PPPoE を使用する */
	data->pppoe = p->pppoe;

	/* PFC ネゴシエーションの禁止 */
	data->prc_nego = p->want.prc_nego;

	/* ACFC ネゴシエーションの禁止 */
	data->acc_nego = p->want.acc_nego;

	/* ACCM ネゴシエーションの禁止 */
	data->accm_nego = p->want.accm_nego;

	/* MTU と MRU の設定 */
	data->mtu = p->mtu;

	/* 回線切断設定 */
	data->ifc_idle_timeout = p->idle_timeout;
	break;

    case 2:
	/* デバイスレイヤの種別 */
	data->dev_type = p->type;

	/* ベンダ名 */
	if(p->vendor != NULL) strcpy(data->vendor, p->vendor);

	/* プロダクト名 */
	if(p->product != NULL) strcpy(data->product, p->product);

	/* イーサネット接続機器の動作モード */
	data->phy_config = p->phy_config;

	/* 追加 AT コマンド */
	if(p->chat_additional != NULL){
	    if((r = sceNetCnfConvS2A(p->chat_additional, data->chat_additional, sizeof(data->chat_additional))) < 0) return (r);
	}

	/* 外線発信番号設定 */
	if(p->outside_number != NULL) strcpy(data->outside_number, p->outside_number);
	if(p->outside_delay != NULL) strcpy(data->outside_delay, p->outside_delay);

	/* ダイアル方法 */
	data->dialing_type = p->dialing_type;

	/* 回線切断設定 */
	data->dev_idle_timeout = p->idle_timeout;
	break;
    }

    return (r);
}

static int get_net(sceNetcnfifData_t *data, sceNetCnfRoot_t *p)
{
    int r = 0;
    sceNetCnfPair_t *pair;

    /* 一番最後の sceNetCnfPair の設定が sceNetcnfifData に残る */
    for(pair = p->pair_head; pair != NULL; pair = pair->forw){
	/* sceNetcnfifData を初期化する */
	sceNetcnfifDataInit(data);

	/* ファイル名を取得 */
	strcpy(data->attach_ifc, pair->attach_ifc);
	strcpy(data->attach_dev, pair->attach_dev);

	/* ATTACH_CNF の内容を取得 */
	if(pair->ifc){
	    r = get_attach(data, pair->ifc, 1);
	}
	if(pair->dev){
	    r = get_attach(data, pair->dev, 2);
	}
    }

    return (r);
}

int sceNetcnfifReadEnv(sceNetcnfifData_t *data, sceNetCnfEnv_t *e, int type)
{
    int r = 0;

    switch(type){
    case 0:
	/* NET_CNF と ATTACH_CNF を sceNetCnfEnv から sceNetcnfifData へ読み込む */
	r = get_net(data, e->root);
	break;
    case 1:
    case 2:
	/* ATTACH_CNF を sceNetCnfEnv から sceNetcnfifData へ読み込む */
	r = get_attach(data, e->ifc, type);
	break;
    default:
	printf("[%s] unknown type (%d)\n", __FUNCTION__, type);
	break;
    }

    return (r);
}
