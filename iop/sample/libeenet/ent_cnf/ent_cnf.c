/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
*/
/*
 *                         Version <1.00>
 *                           Shift-JIS
 *
 *      Copyright (C) 2001-2002 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.00           2002/07/01     komaki      first version
 *       1.10           2003/08/26     ksh
 */
			 
#include <kernel.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sifrpc.h>
#include <netcnf.h>
#include <eenetctl.h>
#include "common.h"

ModuleInfo Module = { "ent_cnf", ((2 << 8) | 1) };


static int g_thread_id;
static sceSifQueueData g_qd;
static sceSifServeData g_sd;

static int start_thread(void *func, int size, int attr, int prio, void *argp)
{
    struct ThreadParam thread_param;
    int thread_id;
	int ret;

    thread_param.attr = attr;
    thread_param.entry = func;
    thread_param.initPriority = prio;
    thread_param.stackSize = size;
    thread_param.option = 0;

	thread_id = CreateThread(&thread_param);
    if (thread_id <= 0) {
		Kprintf("CreateThread (%d)\n", thread_id);
		return (-1);
    }
	ret = StartThread(thread_id, (u_long)argp);
    if (ret != KE_OK) {
		Kprintf("StartThread (%d)\n", ret);
		DeleteThread(thread_id);
		return (-1);
    }
    return(thread_id);
}

static void *ent_cnf_rpc_server_func(unsigned int fno, void *buf, int size)
{
	static sceNetCnfEnv_t env;
	static u_char mem_area[4 * 1024];
	int *data = (int *)buf;

	switch (fno) {
	case ENT_CNF_SIFRPC_LOAD_CONFIG:
		{
		char fname[sceNETCNF_STR_MAX];
		char usr_name[sceNETCNF_STR_MAX];
		int len;

		strncpy(fname, (const char *)&data[0], sceNETCNF_STR_MAX);
		len = strlen(fname) + 1;
		strncpy(usr_name, (const char *)&data[0] + len, sceNETCNF_STR_MAX);

		env.f_no_decode = 1;
		env.mem_last
			= (char *)(env.mem_base = env.mem_ptr = mem_area) + sizeof(mem_area);

		data[0] = sceNetCnfLoadEntry(fname, 0, usr_name, &env);
		}
		break;

	case ENT_CNF_SIFRPC_SET_CONFIG:
		{
		data[0] = sceEENetCtlSetConfiguration(&env);
		}
		break;

	case ENT_CNF_SIFRPC_SET_CONFIG_ADDR:
		{
		const u_int env_addr = (u_int)data[0];

		data[0] = sceEENetCtlSetConfiguration((sceNetCnfEnv_t *)env_addr);
		}
		break;

	default:
		printf("ent_cnf: unknown sifrpc control code.(%d)\n", fno);
	}

	return (buf);
}

static void rpc_server_thread(u_long arg)
{
	static unsigned int rpc_buffer[iop_rpc_size(1024)/4];

	sceSifSetRpcQueue(&g_qd, GetThreadId());
	sceSifRegisterRpc(&g_sd, ENT_CNF_SIFRPC_NUM, ent_cnf_rpc_server_func, (void *) rpc_buffer, 0, 0, &g_qd);
	sceSifRpcLoop(&g_qd);
}

static int ent_cnf_init(int argc, char **argv)
{
	g_thread_id = start_thread(rpc_server_thread, 0x4000, TH_C, 32, NULL);
	if (g_thread_id < 0) {
		Kprintf("start_thread -> %d\n", g_thread_id);
		return (-1);
	}
	return (0);
}

static int unloadable(void)
{
	int dummy, mcount = -1;

	GetModuleIdList(&dummy, 1, &mcount);
	return((mcount < 0)? 0: 1);
}

int start(int ac, char *av[])
{
	extern libhead ent_cnf_entry;
	int r, oldei;

	if (ac < 0) {
		sceSifRemoveRpc(&g_sd, &g_qd);
		sceSifRemoveRpcQueue(&g_qd);
		TerminateThread(g_thread_id);
		DeleteThread(g_thread_id);
		CpuSuspendIntr(&oldei);
		r = ReleaseLibraryEntries(&ent_cnf_entry);
		CpuResumeIntr(oldei);
		if (r == KE_OK || r == KE_LIBRARY_NOTFOUND) {
			Kprintf("ent_cnf stopped.\n");
			return (NO_RESIDENT_END);
		}
		if(r == KE_LIBRARY_INUSE) {
			Kprintf("ent_cnf library inuse.\n");
		}
		Kprintf("ent_cnf can't stop.\n");
		return (REMOVABLE_RESIDENT_END);
	}
	CpuSuspendIntr(&oldei);
	r = RegisterLibraryEntries(&ent_cnf_entry);
	CpuResumeIntr(oldei);
	if (r != KE_OK) {
		Kprintf("module already loaded\n");
		return (NO_RESIDENT_END);
	}
	if (0 > (r = ent_cnf_init(ac, av))) {
		Kprintf("ent_cnf_init failed.\n");
		return (NO_RESIDENT_END);
	}
	if (unloadable()) {
		return (REMOVABLE_RESIDENT_END);
	}

	return(RESIDENT_END);
}

/*** End of file ***/
