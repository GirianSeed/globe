/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/* 
 *      Netcnf Interface Library
 *
 *                         Version 1.1
 *                         Shift-JIS
 *
 *      Copyright (C) 2002 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                         netcnf_env.c
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.1            2002.01.28      tetsu       First version
 */

#include <stdio.h>
#include <string.h>
#include <netcnf.h>

/* sceNetCnfEnv 構造体を初期化する */
void sceNetcnfifEnvInit(sceNetCnfEnv_t *env, void *mem_area, int size, int f_no_decode)
{
    memset(env, 0, sizeof(sceNetCnfEnv_t));
    env->mem_last  = (env->mem_base = env->mem_ptr = mem_area) + size;
    env->f_no_decode = f_no_decode;
}

#ifdef DEBUG				
/* sceNetCnfEnv 構造体をダンプする */
void sceNetcnfifEnvDump(sceNetCnfEnv_t *e)
{
    int i, dns_no;
    char str_tmp[256];
    sceNetCnfRoot_t *r = NULL;
    sceNetCnfPair_t *p = NULL;
    sceNetCnfInterface_t *ifc = NULL;
    sceNetCnfInterface_t *dev = NULL;
    sceNetCnfCommand_t *c = NULL;
    sceNetCnfRoutingEntry_t *re = NULL;

    printf("-----------------------\n");
    if(e->root != NULL){
	r = e->root;
	if(r->pair_head != NULL){
	    p = r->pair_head;
	    printf("display_name     : \"%s\"\n", p->display_name);
	    printf("attach_ifc       : \"%s\"\n", p->attach_ifc);
	    printf("attach_dev       : \"%s\"\n", p->attach_dev);
	    if(p->ifc != NULL){
		ifc = p->ifc;
		printf("address          : \"%s\"\n", ifc->address);
		printf("netmask          : \"%s\"\n", ifc->netmask);
		printf("auth_name        : \"%s\"\n", ifc->auth_name);
		printf("auth_key         : \"%s\"\n", ifc->auth_key);
		printf("dhcp_host_name   : \"%s\"\n", ifc->dhcp_host_name);
		printf("peer_name        : \"%s\"\n", ifc->peer_name);
		printf("type(ifc)        : \"%d\"\n", ifc->type);
		printf("dhcp             : \"%x\"\n", ifc->dhcp);
		printf("want.dns1_nego   : \"%x\"\n", ifc->want.dns1_nego);
		printf("want.dns2_nego   : \"%x\"\n", ifc->want.dns2_nego);
		printf("allow.f_auth     : \"%d\"\n", ifc->allow.f_auth);
		printf("allow.auth       : \"%d\"\n", ifc->allow.auth);
		printf("pppoe            : \"%x\"\n", ifc->pppoe);
		printf("want.prc_nego    : \"%x\"\n", ifc->want.prc_nego);
		printf("want.acc_nego    : \"%x\"\n", ifc->want.acc_nego);
		printf("want.accm_nego   : \"%x\"\n", ifc->want.accm_nego);
		printf("mtu              : \"%d\"\n", ifc->mtu);
		printf("idle_timeout(ifc): \"%d\"\n", ifc->idle_timeout);
		printf("redial_count     : \"%d\"\n", ifc->redial_count);
		for(i = 0, dns_no = 0, c = ifc->cmd_head; c != NULL; c = c->forw){
		    switch(c->code){
		    case sceNetCnf_CMD_ADD_NAMESERVER:
			sceNetCnfAddress2String(str_tmp, sizeof(str_tmp), (void *)(c + 1));
			printf("dns%d_address     : \"%s\"\n", dns_no + 1, str_tmp);
			dns_no++;
			break;
		    case sceNetCnf_CMD_ADD_ROUTING:
			re = (void *)(c + 1);
			sceNetCnfAddress2String(str_tmp, sizeof(str_tmp), &re->gateway);
			printf("gateway          : \"%s\"\n", str_tmp);
			break;
		    }
		}
		for(i = 0; i < sceNetCnf_MAX_PHONE_NUMBERS; i++){
		    if(NULL == (ifc->phone_numbers[i])) continue;
		    switch(i){
		    case 0:
			printf("phone_numbers1   : \"%s\"\n", ifc->phone_numbers[0]);
			break;
		    case 1:
			printf("phone_numbers2   : \"%s\"\n", ifc->phone_numbers[1]);
			break;
		    case 2:
			printf("phone_numbers3   : \"%s\"\n", ifc->phone_numbers[2]);
			break;
		    }
		}
	    }
	    if(p->dev != NULL){
		dev = p->dev;
		printf("type(dev)        : \"%d\"\n", dev->type);
		printf("vendor           : \"%s\"\n", dev->vendor);
		printf("product          : \"%s\"\n", dev->product);
		printf("chat_additional  : \"%s\"\n", dev->chat_additional);
		printf("outside_number   : \"%s\"\n", dev->outside_number);
		printf("outside_delay    : \"%s\"\n", dev->outside_delay);
		printf("dialing_type     : \"%d\"\n", dev->dialing_type);
		printf("phy_config       : \"%d\"\n", dev->phy_config);
		printf("idle_timeout(dev): \"%d\"\n", dev->idle_timeout);
	    }
	}
    }
    printf("-----------------------\n");
}
#endif /* DEBUG */

