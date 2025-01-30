/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5
 */
/* 
 *      Netcnf Interface Library
 *
 *                         Version 1.2
 *                         Shift-JIS
 *
 *      Copyright (C) 2002 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                         write_env.c
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.1            2002.01.28      tetsu       First version
 *       1.2            2002.02.10      tetsu       Correspond to sceNetCnfConvA2S()
 */

#include <stdio.h>
#include <string.h>
#include <netcnf.h>

#include "../../../../common/include/netcnfif.h"

typedef struct route{
    sceNetCnfCommand_t cmd;
    sceNetCnfRoutingEntry_t re;
} route_t;

static route_t gateway __attribute__((aligned(64))); 

typedef struct nameserver{
    sceNetCnfCommand_t cmd;
    sceNetCnfAddress_t address;
} nameserver_t;

static nameserver_t dns1 __attribute__((aligned(64)));
static nameserver_t dns2 __attribute__((aligned(64)));

static void init_usrntcnf(sceNetCnfInterface_t *ifc)
{
    int i;

    ifc->type = -1;
    ifc->dhcp = sceNetCnf_BOOL_DEFAULT;
    ifc->dhcp_host_name = NULL;
    ifc->address = NULL;
    ifc->netmask = NULL;
    ifc->cmd_head = NULL;
    ifc->cmd_tail = NULL;
    for(i = 0; i < 3; i++){
	ifc->phone_numbers[i] = NULL;
    }
    ifc->auth_name = NULL;
    ifc->auth_key = NULL;
    ifc->peer_name = NULL;
    ifc->want.dns1_nego = sceNetCnf_BOOL_DEFAULT;
    ifc->want.dns2_nego = sceNetCnf_BOOL_DEFAULT;
    ifc->allow.f_auth = 0;
    ifc->allow.auth = 4; /* chap/pap モード以外になり得ないのでとりあえず設定しておく */
    ifc->pppoe = sceNetCnf_BOOL_DEFAULT;
    ifc->want.prc_nego = sceNetCnf_BOOL_DEFAULT;
    ifc->want.acc_nego = sceNetCnf_BOOL_DEFAULT;
    ifc->want.accm_nego = sceNetCnf_BOOL_DEFAULT;
    ifc->mtu = -1;
    ifc->phy_config = -1;
    ifc->vendor = NULL;
    ifc->product = NULL;
    ifc->chat_additional = NULL;
    ifc->outside_number = NULL;
    ifc->outside_delay = NULL;
    ifc->dialing_type = -1;
    ifc->idle_timeout = -1;
}

static int check_address(char *str)
{
    int r;

    for(r = 0; *str != '\0'; str++){
      if(*str == '.') continue;
      if(*str != '0') r = 1;
    }

    return (r);
}

static int put_gw(sceNetCnfEnv_t *e, char *gw)
{
    int r = 1;
    sceNetCnfCommand_t *p;
    sceNetCnfRoutingEntry_t *re;

    /* sceNetCnfCommand + sceNetCnfRoutingEntry 領域を取得 */
    bzero(&gateway, sizeof(route_t));
    p = (sceNetCnfCommand_t *)&gateway;
    p->code = sceNetCnf_CMD_ADD_ROUTING;

    /* リンクを作成 */
    (((p->back = e->ifc->cmd_tail) != NULL) ? p->back->forw : e->ifc->cmd_head) = p;
    p->forw = NULL, e->ifc->cmd_tail = p;

    /* sceNetCnfRoutingEntry ポインタを取得 */
    re = (sceNetCnfRoutingEntry_t *)(p + 1);

    /* Default Gateway を形式変換 */
    if(gw == NULL){
	if((r = sceNetCnfName2Address(&re->dstaddr, NULL)) < 0) return(r);
	if((r = sceNetCnfName2Address(&re->gateway, NULL)) < 0) return(r);
	if((r = sceNetCnfName2Address(&re->genmask, NULL)) < 0) return(r);
	re->flags = 0;
    }else{
	if((r = sceNetCnfName2Address(&re->dstaddr, NULL)) < 0) return(r);
	if((r = sceNetCnfName2Address(&re->gateway, gw)) < 0) return(r);
	if((r = sceNetCnfName2Address(&re->genmask, NULL)) < 0) return(r);
	re->flags |= sceNetCnfRoutingF_Gateway;
    }

    return (r);
}

static int put_ns(sceNetCnfEnv_t *e, char *ns, int ns_count)
{
    int r = 1;
    sceNetCnfCommand_t *p = NULL;
    sceNetCnfAddress_t *address;

    /* sceNetCnfCommand + sceNetCnfAddress 領域を取得 */
    switch(ns_count){
    case 1:
	bzero(&dns1, sizeof(nameserver_t));
	p = (sceNetCnfCommand_t *)&dns1;
	break;
    case 2:
	bzero(&dns2, sizeof(nameserver_t));
	p = (sceNetCnfCommand_t *)&dns2;
	break;
    }
    p->code = sceNetCnf_CMD_ADD_NAMESERVER;

    /* リンクを作成 */
    (((p->back = e->ifc->cmd_tail) != NULL) ? p->back->forw : e->ifc->cmd_head) = p;
    p->forw = NULL, e->ifc->cmd_tail = p;

    /* sceNetCnfAddress ポインタを取得 */
    address = (sceNetCnfAddress_t *)(p + 1);

    /* Name Server Address を形式変換 */
    if((r = sceNetCnfName2Address(address, ns)) < 0) return(r);

    return (r);
}

static int put_cmd(sceNetCnfEnv_t *e, sceNetcnfifData_t *data)
{
    int r = 0;

    if(data->dhcp == 0){
	/* Default Gateway */
	if(data->gateway[0] != '\0' && check_address(data->gateway)){
	    r = put_gw(e, data->gateway);
	}else{
	    r = put_gw(e, NULL);
	}
    }

    /* Name Server */
    if(data->dns1_address[0] != '\0' && check_address(data->dns1_address)){
	r = put_ns(e, data->dns1_address, 1);
	if(data->dns2_address[0] != '\0' && check_address(data->dns2_address)){
	    r = put_ns(e, data->dns2_address, 2);
	}
    }

    return (r);
}

static int root_link(sceNetCnfEnv_t *e, int type)
{
    int r = 0;
    sceNetCnfPair_t *p;

    /* e->ifc が設定されてなければ何もせずに抜ける */
    if(!e->ifc) return (r);

    /* e->ifc の内容を root の下に持って来る */
    if(e->root != NULL){
	if(e->root->pair_head != NULL){
	    if(type == 1) e->root->pair_head->ifc = e->ifc;
	    if(type == 2) e->root->pair_head->dev = e->ifc;
	}else{
	    goto MAKE_PAIR;
	}
    }else{
	/* sceNetCnfRoot 領域を確保 */
	if(NULL == (e->root = sceNetCnfAllocMem(e, sizeof(sceNetCnfRoot_t), 2))) return(sceNETCNFIF_ALLOC_ERROR);

	/* sceNetCnfRoot の初期化 */
	e->root->version         = sceNetCnf_CURRENT_VERSION;
	e->root->redial_count    = -1;
	e->root->redial_interval = -1;
	e->root->dialing_type    = -1;

MAKE_PAIR:
	/* sceNetCnfPair 領域を確保 */
	if(NULL == (p = sceNetCnfAllocMem(e, sizeof(sceNetCnfPair_t), 2))) return(sceNETCNFIF_ALLOC_ERROR);

	if(type == 1) p->ifc = e->ifc;
	if(type == 2) p->dev = e->ifc;

	/* リンクを作成 */
	(((p->back = e->root->pair_tail) != NULL) ? p->back->forw : e->root->pair_head) = p;
	p->forw = NULL, e->root->pair_tail = p;
    }

    return (r);
}

static int put_attach(sceNetCnfEnv_t *e, sceNetcnfifData_t *data, int type)
{
    static char chat_additional[256];
    int r = 0;
    int init_flag;

    /* sceNetCnfInterface 領域を確保 */
    if(e->ifc == NULL){
	if(NULL == (e->ifc = sceNetCnfAllocMem(e, sizeof(sceNetCnfInterface_t), 2))) return(sceNETCNFIF_ALLOC_ERROR);
	sceNetCnfInitIFC(e->ifc);
    }

    /* あなたのネットワーク設定ファイルで使用する領域を初期化 */
    init_usrntcnf(e->ifc);
    init_flag = 1;

    switch(type){
    case 1:
	/* デバイスレイヤの種別 */
	if(data->ifc_type != -1){
	    e->ifc->type = data->ifc_type;
	    init_flag = 0;
	}

	/* DHCP を使用する・しない */
	if(data->dhcp != sceNetCnf_BOOL_DEFAULT){
	    e->ifc->dhcp = data->dhcp;
	    init_flag = 0;
	}

	/* DHCP 用ホスト名 */
	if(data->dhcp_host_name[0] != '\0'){
	    e->ifc->dhcp_host_name = data->dhcp_host_name;
	    init_flag = 0;
	}

	/* IP アドレス */
	if(data->address[0] != '\0'){
	    e->ifc->address = data->address;
	    init_flag = 0;
	}

	/* ネットマスク */
	if(data->netmask[0] != '\0'){
	    e->ifc->netmask = data->netmask;
	    init_flag = 0;
	}

	/* デフォルトルータ, プライマリ DNS/セカンダリ DNS */
	if((r = put_cmd(e, data)) < 0){
	    return (r);
	}else{
	    if(r) init_flag = 0;
	}

	/* 接続先電話番号1 */
	if(data->phone_numbers1[0] != '\0'){
	    e->ifc->phone_numbers[0] = data->phone_numbers1;
	    init_flag = 0;
	}

	/* 接続先電話番号2 */
	if(data->phone_numbers2[0] != '\0'){
	    e->ifc->phone_numbers[1] = data->phone_numbers2;
	    init_flag = 0;
	}

	/* 接続先電話番号3 */
	if(data->phone_numbers3[0] != '\0'){
	    e->ifc->phone_numbers[2] = data->phone_numbers3;
	    init_flag = 0;
	}

	/* ユーザ ID */
	if(data->auth_name[0] != '\0'){
	    e->ifc->auth_name = data->auth_name;
	    init_flag = 0;
	}

	/* パスワード */
	if(data->auth_key[0] != '\0'){
	    e->ifc->auth_key = data->auth_key;
	    init_flag = 0;
	}

	/* 接続先の認証名 */
	if(data->peer_name[0] != '\0'){
	    e->ifc->peer_name = data->peer_name;
	    init_flag = 0;
	}

	/* DNS サーバアドレスを自動取得する・しない(プライマリ DNS) */
	if(data->dns1_nego != sceNetCnf_BOOL_DEFAULT){
	    e->ifc->want.dns1_nego = data->dns1_nego;
	    init_flag = 0;
	}

	/* DNS サーバアドレスを自動取得する・しない(セカンダリ DNS) */
	if(data->dns2_nego != sceNetCnf_BOOL_DEFAULT){
	    e->ifc->want.dns2_nego = data->dns2_nego;
	    init_flag = 0;
	}

	/* 認証方法を設定する・しない */
	if(data->f_auth != 0){
	    e->ifc->allow.f_auth = data->f_auth;
	    init_flag = 0;
	}

	/* 認証方法 */
	e->ifc->allow.auth = data->auth;

	/* PPPoE を使用する */
	if(data->pppoe != sceNetCnf_BOOL_DEFAULT){
	    e->ifc->pppoe = data->pppoe;
	    init_flag = 0;
	}

	/* PFC ネゴシエーションの禁止 */
	if(data->prc_nego != sceNetCnf_BOOL_DEFAULT){
	    e->ifc->want.prc_nego = data->prc_nego;
	    init_flag = 0;
	}

	/* ACFC ネゴシエーションの禁止 */
	if(data->acc_nego != sceNetCnf_BOOL_DEFAULT){
	    e->ifc->want.acc_nego = data->acc_nego;
	    init_flag = 0;
	}

	/* ACCM ネゴシエーションの禁止 */
	if(data->accm_nego != sceNetCnf_BOOL_DEFAULT){
	    e->ifc->want.accm_nego = data->accm_nego;
	    init_flag = 0;
	}

	/* MTU と MRU の設定 */
	if(data->mtu != -1){
	    e->ifc->mtu = data->mtu;
	    init_flag = 0;
	}

	/* 回線切断設定 */
	if(data->ifc_idle_timeout != -1){
	    e->ifc->idle_timeout = data->ifc_idle_timeout;
	    init_flag = 0;
	}
	break;

    case 2:
	/* デバイスレイヤの種別 */
	if(data->dev_type != -1){
	    e->ifc->type = data->dev_type;
	    init_flag = 0;
	}

	/* ベンダ名 */
	if(data->vendor[0] != '\0'){
	    e->ifc->vendor = data->vendor;
	    init_flag = 0;
	}

	/* プロダクト名 */
	if(data->product[0] != '\0'){
	    e->ifc->product = data->product;
	    init_flag = 0;
	}

	/* イーサネット接続機器の動作モード */
	if(data->phy_config != -1){
	    e->ifc->phy_config = data->phy_config;
	    init_flag = 0;
	}

	/* 追加 AT コマンド */
	if(data->chat_additional[0] != '\0'){
	    if((r = sceNetCnfConvA2S(data->chat_additional, chat_additional, sizeof(chat_additional))) < 0) return (r);
	    e->ifc->chat_additional = chat_additional;
	    init_flag = 0;
	}

	/* 外線発信番号設定 */
	if(data->outside_number[0] != '\0'){
	    e->ifc->outside_number = data->outside_number;
	    init_flag = 0;
	}
	if(data->outside_delay[0] != '\0'){
	    e->ifc->outside_delay = data->outside_delay;
	    init_flag = 0;
	}

	/* ダイアル方法 */
	if(data->dialing_type != -1){
	    e->ifc->dialing_type = data->dialing_type;
	    init_flag = 0;
	}

	/* 回線切断設定 */
	if(data->dev_idle_timeout != -1){
	    e->ifc->idle_timeout = data->dev_idle_timeout;
	    init_flag = 0;
	}
	break;
    }

    /* 何も設定されてなかったら e->ifc を初期化してエラーを返す */
    if(init_flag) return (e->ifc = NULL, sceNETCNFIF_NO_DATA);

    return (r);
}

static int put_net(sceNetCnfEnv_t *e, sceNetcnfifData_t *data)
{
    static char display_name[256];
    sceNetCnfPair_t *p;
    int i, r = 0;

    /* データが設定されていなかったらエラーを返す */
    if(data->attach_ifc[0] == '\0') return (sceNETCNFIF_NO_DATA);
    if(data->attach_dev[0] == '\0') return (sceNETCNFIF_NO_DATA);

    /* sceNetCnfRoot 領域を確保 */
    if(e->root == NULL){
	if(NULL == (e->root = sceNetCnfAllocMem(e, sizeof(sceNetCnfRoot_t), 2))) return(sceNETCNFIF_ALLOC_ERROR);
    }

    /* sceNetCnfRoot の初期化 */
    e->root->version         = sceNetCnf_CURRENT_VERSION;
    e->root->redial_count    = -1;
    e->root->redial_interval = -1;
    e->root->dialing_type    = -1;

    /* sceNetCnfPair 領域を確保 */
    if(e->root->pair_head == NULL){
	if(NULL == (p = sceNetCnfAllocMem(e, sizeof(sceNetCnfPair_t), 2))) return(sceNETCNFIF_ALLOC_ERROR);

	/* リンクを作成 */
	(((p->back = e->root->pair_tail) != NULL) ? p->back->forw : e->root->pair_head) = p;
	p->forw = NULL, e->root->pair_tail = p;
    }else{
	p = e->root->pair_head;
    }

    /* display_name の設定 */
    strcpy(display_name, data->attach_ifc);
    strcat(display_name, " + ");
    strcat(display_name, data->attach_dev);
    p->display_name = display_name;

    /* 接続プロバイダ設定ファイル名(sys_name)を設定 */
    p->attach_ifc = data->attach_ifc;

    /* 接続機器設定ファイル名(sys_name)を設定 */
    p->attach_dev = data->attach_dev;

    for(i = 0; i < 2; i++){
	/* e->ifc を初期化 */
	e->ifc = NULL;

	/* ATTACH_CNF を sceNetcnfifData から sceNetCnfEnv に書き込む */
	r = put_attach(e, data, i + 1);
	if(r < 0 && r != sceNETCNFIF_NO_DATA) break;

	/* sceNetCnfEnv 直下の sceNetCnfInterface メンバ ifc の値を
	   e->root->pair_head->ifc あるいは e->root->pair_head->dev に
	   設定する */
	r = root_link(e, i + 1);
	if(r < 0) break;
    }

    return(r);
}

int sceNetcnfifWriteEnv(sceNetCnfEnv_t *e, sceNetcnfifData_t *data, int type)
{
    int r = 0;

    switch(type){
    case 0:
	/* NET_CNF を sceNetcnfifData から sceNetCnfEnv に書き込む */
	r = put_net(e, data);
	break;
    case 1:
    case 2:
	/* ATTACH_CNF を sceNetcnfifData から sceNetCnfEnv に書き込む */
	r = put_attach(e, data, type);
	if(r < 0) break;

	/* sceNetCnfEnv 直下の sceNetCnfInterface メンバ ifc の値を
	   e->root->pair_head->ifc あるいは e->root->pair_head->dev に
	   設定する */
	r = root_link(e, type);
	break;
    default:
	printf("[%s] unknown type (%d)\n", __FUNCTION__, type);
	break;
    }

    /* 使用した分 base をずらす */
    if(r >= 0){
	e->mem_ptr = (void *)(((int)e->mem_ptr + 3) & ~3);
	e->mem_base = e->mem_ptr;
    }

    return (r);
}
