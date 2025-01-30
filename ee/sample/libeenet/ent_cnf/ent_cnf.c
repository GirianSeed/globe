/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5.5
 */
/*
 *                      Emotion Engine Library
 *                          Version <1.00>
 *                           Shift-JIS
 *
 *      Copyright (C) 2000 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                       <ent_cnf - ent_cnf.c>
 *                    <handling eenetctl function>
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.00           Nov,26,2001     komaki      first version
 */

#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <eekernel.h>
#include <eetypes.h>
#include <eeregs.h>
#include <sifrpc.h>
#include <common.h>
#include <eenetctl.h>

#define NETBUFSIZE 512
#define RPCSIZE NETBUFSIZE * 4


static sceSifClientData cd;
static u_int rpc_buffer[NETBUFSIZE]  __attribute__((aligned(64)));

int ent_cnf_init(void){
	int i;

	/* bind rpc */
	while(1){
		if (sceSifBindRpc(&cd, ENT_CNF_SIFRPC_NUM, 0) < 0) {
			scePrintf("ent_cnf: sceSifBindRpc failed.\n");
			while(1);
		}
		if (cd.serve != 0) break;
		i = 0x10000;
		while(i --);
	}

	return 0;
}

int ent_cnf_load_config(const char *fname, const char *usr_name){
	int ret, len;

	strcpy((char *)rpc_buffer, fname);
	len = strlen(fname) + 1;
	strcpy((char *)rpc_buffer + len, usr_name);
	len += strlen(usr_name) + 1;

	ret = sceSifCallRpc(&cd, ENT_CNF_SIFRPC_LOAD_CONFIG, 0,
		(void *)rpc_buffer, ee_rpc_size(len),
		(void *)rpc_buffer, ee_rpc_size(sizeof(u_int)), 0, 0);
	if(ret < 0){
		scePrintf("ent_cnf: RPC call in ent_cnf_load_config() failed.\n");
		return -1;
	}

	return rpc_buffer[0];
}

int ent_cnf_set_config(void){
	int ret;

	ret = sceSifCallRpc(&cd, ENT_CNF_SIFRPC_SET_CONFIG, 0,
		NULL, 0, (void *)rpc_buffer, ee_rpc_size(sizeof(u_int)), 0, 0);
	if(ret < 0){
		scePrintf("ent_cnf: RPC call in ent_cnf_set_config() failed.\n");
		return -1;
	}

	return rpc_buffer[0];
}

int ent_cnf_set_sifcmdbuffer(int sifcmd_num){
	int ret;

	rpc_buffer[0] = sifcmd_num;
	ret = sceSifCallRpc(&cd, ENT_CNF_SIFRPC_SET_SIFCMD, 0,
		(void *)rpc_buffer, ee_rpc_size(sizeof(u_int)),
		(void *)rpc_buffer, ee_rpc_size(sizeof(u_int)), 0, 0);
	if(ret < 0){
		scePrintf("ent_cnf: RPC call in ent_cnf_set_sifcmdbuffer() failed.\n");
		return -1;
	}

	return rpc_buffer[0];
}
