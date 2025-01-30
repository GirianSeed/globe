/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5.5
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
 */
			 
#include <kernel.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <ctype.h>
#include <sifrpc.h>
#include <sifcmd.h>
#include <netcnf.h>
#include <eenetctl.h>
#include <common.h>

#if defined(REMOVABLE_RESIDENT_END) && !defined(DISABLE_MODULE_UNLOAD)
#define ENABLE_MODULE_UNLOAD
#endif

ModuleInfo Module = { "ent_cnf", ((1 << 8) | 0x100)};


#define kprintf(args...)	Kprintf("ent_cnf " args)
#if defined(DEBUG)
#define dprintf(args...)	kprintf(args)
#else
#define dprintf(args...)
#endif

static int unload_ok = 1;
static int thread_id;
static int ent_cnf_priority = 32;
static unsigned int rpc_buffer[iop_rpc_size(1024)/4];
static sceNetCnfEnv_t env;
static u_char mem_area[0x1000];

sceSifCmdData *cmddata;
#define alloc_sys_memory_pad_size(size) ((size + 255) & ~255)

static int start_thread(void *func, int size, int attr, int prio, void *argp){

    struct ThreadParam thread_param;
    int thread_id, r;

    thread_param.attr = attr;
    thread_param.entry = func;
    thread_param.initPriority = prio;
    thread_param.stackSize = size;
    thread_param.option = 0;

    if(0 >= (thread_id = CreateThread(&thread_param))){
	printf("CreateThread (%d)\n", thread_id);
	return(-1);
    }
    if(KE_OK != (r = StartThread(thread_id, (u_long)argp))){
	printf("StartThread (%d)\n", r);
	DeleteThread(thread_id);
	return(-1);
    }
    return(0);

}

static void *ent_cnf_rpc_server_func(unsigned int fno, void *data, int size){

	switch(fno){
	case ENT_CNF_SIFRPC_LOAD_CONFIG:
		{
		char fname[sceNETCNF_STR_MAX];
		char usr_name[sceNETCNF_STR_MAX];
		int len;

		strncpy(fname, (char *)&((int *)data)[0], sceNETCNF_STR_MAX);
		len = strlen(fname) + 1;
		strncpy(usr_name, (char *)&((int *)data)[0] + len, sceNETCNF_STR_MAX);

		env.f_no_decode = 1;
		env.mem_last
			= (env.mem_base = env.mem_ptr = mem_area) + sizeof(mem_area);

		((int *)data)[0]
			= sceNetCnfLoadEntry(fname, 0, usr_name, &env);
		}
		break;
	case ENT_CNF_SIFRPC_SET_CONFIG:
		{
		((int *)data)[0] = sceEENetCtlSetConfiguration(&env);
		}
		break;
	case ENT_CNF_SIFRPC_SET_SIFCMD:
		{
		int sifcmd_num = ((int *)data)[0];
		int size = alloc_sys_memory_pad_size(sizeof(sceSifCmdData) * sifcmd_num);
		int oldstat;

		/* malloc */
		CpuSuspendIntr(&oldstat);
		if((cmddata = (sceSifCmdData *)AllocSysMemory(SMEM_Low, size, NULL)) == NULL){
			CpuResumeIntr(oldstat);
			((int *)data)[0] = -1;
			goto ret;
		}

		sceSifSetCmdBuffer(cmddata, sifcmd_num);
		CpuResumeIntr(oldstat);
		((int *)data)[0] = 0;
		}
		break;
	default:
		printf("ent_cnf: unknown sifrpc control code.\n");
	}

ret:
	return data;
}

static void rpc_server_thread(u_long arg){
	sceSifQueueData qd;
	sceSifServeData sd;

	/* do server function */
	sceSifSetRpcQueue(&qd, GetThreadId());
	sceSifRegisterRpc(&sd, ENT_CNF_SIFRPC_NUM, ent_cnf_rpc_server_func, (void *) rpc_buffer, 0, 0, &qd);
	sceSifRpcLoop(&qd);

	return;
}

static int ent_cnf_init(int argc, char **argv){

	if((thread_id = start_thread(rpc_server_thread, 0x4000, TH_C, ent_cnf_priority, NULL)) < 0){
		dprintf("start_thread -> %d\n", thread_id);
		goto fail;
	}

	return 0;

fail:
	return -1;
}

#if defined(ENABLE_MODULE_UNLOAD)
static int unloadable(void){
	int dummy, mcount = -1;

	GetModuleIdList(&dummy, 1, &mcount);
	return((mcount < 0)? 0: 1);
}
#endif

int start(int ac, char *av[]){
	extern libhead ent_cnf_entry;
	int r;
#if defined(ENABLE_MODULE_UNLOAD)
	if(ac < 0){	/* other only */
		if(unload_ok == 0){
			kprintf("can't unload (busy)\n");
			return(REMOVABLE_RESIDENT_END);
		}
		TerminateThread(thread_id);
		DeleteThread(thread_id);
		ReleaseLibraryEntries(&ent_cnf_entry);	/* never one use me */
		return(NO_RESIDENT_END);
	}
#endif
	if(KE_OK != RegisterLibraryEntries(&ent_cnf_entry)){
		kprintf("module already loaded\n");
		return(NO_RESIDENT_END);
	}
	if(0 > (r = ent_cnf_init(ac, av))){
		kprintf("ent_cnf_init failed.\n");
		return(NO_RESIDENT_END);
	}
	// kprintf("resident_end\n");
#if defined(ENABLE_MODULE_UNLOAD)
	if(unloadable())
		return(REMOVABLE_RESIDENT_END);
#endif
	return(RESIDENT_END);
}
