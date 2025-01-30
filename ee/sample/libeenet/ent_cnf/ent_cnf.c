/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
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
 *       1.10           2003/08/26      ksh
 */

#include <eekernel.h>
#include <eetypes.h>
#include <string.h>
#include <sifrpc.h>
#include "ent_cnf.h"
#include "common.h"

#define NETBUFSIZE	(512)


static sceSifClientData g_cd;
static int g_rpc_buffer[NETBUFSIZE]  __attribute__((aligned(64)));

static int bind_sifrpc(sceSifClientData *bd, unsigned int request, unsigned int mode)
{
	int i;

	for (;;) {
		if (sceSifBindRpc(bd, request, mode) < 0) {
			scePrintf("bind_sifrpc error\n");
			return (-1);
		}
		if (bd->serve != 0) {
			break;
		}
		i = 0x10000;
		while (--i);
	}
	return (0);
}

static int call_sifrpc(sceSifClientData *bd, unsigned int fno,
	unsigned int mode, void *send, int ssize, void *receive, int rsize,
	sceSifEndFunc end_func, void *end_parm)
{
	int i;

	for (;;) {
		if (sceSifCallRpc(bd, fno, mode, send, ssize, receive, rsize, end_func, end_parm) == 0) {
			break;
		}
		i = 0x10000;
		while (--i);
	}
	return (0);
}

int ent_cnf_init(void)
{
	return (bind_sifrpc(&g_cd, ENT_CNF_SIFRPC_NUM, 0));
}

int ent_cnf_load_config(const char *fname, const char *usr_name)
{
	int len;

	strcpy((char *)g_rpc_buffer, fname);
	len = strlen(fname) + 1;
	strcpy((char *)g_rpc_buffer + len, usr_name);
	len += strlen(usr_name) + 1;

	call_sifrpc(
		&g_cd,
		ENT_CNF_SIFRPC_LOAD_CONFIG,
		0,
		(void *)g_rpc_buffer,
		ee_rpc_size(len),
		(void *)g_rpc_buffer,
		ee_rpc_size(sizeof(u_int)),
		0,
		0
	);
	return (g_rpc_buffer[0]);
}

int ent_cnf_set_config(void)
{
	call_sifrpc(
		&g_cd,
		ENT_CNF_SIFRPC_SET_CONFIG,
		0,
		NULL,
		0,
		(void *)g_rpc_buffer,
		ee_rpc_size(sizeof(u_int)),
		0,
		0
	);
	return (g_rpc_buffer[0]);
}

int ent_cnf_set_config_addr(u_int addr)
{
	g_rpc_buffer[0] = (int)addr;
	call_sifrpc(
		&g_cd,
		ENT_CNF_SIFRPC_SET_CONFIG_ADDR,
		0,
		(void *)g_rpc_buffer,
		ee_rpc_size(sizeof(u_int)),
		(void *)g_rpc_buffer,
		ee_rpc_size(sizeof(u_int)),
		0,
		0
	);
	return (g_rpc_buffer[0]);
}

/*** End of file ***/
