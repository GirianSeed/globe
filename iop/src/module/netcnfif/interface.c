/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/* 
 *      Netcnf Interface Library
 *
 *                         Version 1.7
 *                         Shift-JIS
 *
 *      Copyright (C) 2002-2003 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                         interface.c
 *
 *       Version  Date        Design  Log
 *  --------------------------------------------------------------------
 *       1.1      2002.01.28  tetsu   First version
 *       1.2      2002.02.06  tetsu   Modified SCE_NETCNFIF_EDIT_ENTRY
 *       1.3      2002.02.10  tetsu   Modified SCE_NETCNFIF_CHECK_ADDITIONAL_AT
 *       1.4      2002.05.05  tetsu   Add SCE_NETCNFIF_CHECK_SPECIAL_PROVIDER
 *       1.5      2002.10.19  tetsu   Change path of netcnfif.h
 *       1.6      2003.01.14  ksh     callback & remake
 *       1.7      2003.09.18  ksh     heap
 */

#include <stdio.h>
#include <string.h>
#include <kernel.h>
#include <sifrpc.h>
#include <netcnf.h>

#include "dma.h"
#include "netcnf_env.h"
#include "read_env.h"
#include "write_env.h"
#include "callback.h"
#include "mem.h"

static sceSifServeData sd;
static sceSifQueueData qd;

static void *mem_area = NULL;
static int mem_area_size = 0;

static void *sceNetcnfifInterfaceServer(u_int fno, void *buf, int size)
{
    static sceNetCnfEnv_t env;
    static sceNetcnfifData_t data;
    static char chat_additional[256];
    sceNetcnfifArg_t *p;
    sceNetCnfList_t *list_iop;
    sceNetcnfifList_t *list_ee;
	sceNetCnfCallback_t callback;
    int i;
	int id;
	int redial_count;
	int r = 0;

	(void)size;
    p = (sceNetcnfifArg_t *)buf;

    switch (fno) {
    case SCE_NETCNFIF_GET_COUNT:
		r = sceNetCnfGetCount(p->fname, p->type);
		break;

    case SCE_NETCNFIF_GET_LIST:
		r = sceNetCnfGetCount(p->fname, p->type);
		if (r < 0) {
			break;
		}
		
		list_iop  = (sceNetCnfList_t *)my_alloc(r * sizeof(sceNetCnfList_t));
		if (!list_iop) {
	    	r = sceNETCNFIF_ALLOC_ERROR;
	    	break;
		}
		list_ee  = (sceNetcnfifList_t *)my_alloc(p->data * sizeof(sceNetcnfifList_t));
		if (!list_ee) {
	    	my_free(list_iop);
	    	r = sceNETCNFIF_ALLOC_ERROR;
	    	break;
		}

		r = sceNetCnfGetList(p->fname, p->type, list_iop);
		if (r < 0) {
	    	my_free(list_iop); 
	    	my_free(list_ee); 
	    	break;
		}
		for (i = 0; i < p->data && i < r; i++) {
			memcpy(list_ee + i, list_iop + i, sizeof(sceNetCnfList_t));
		}
		
#ifdef DEBUG
		printf("[SCE_NETCNFIF_GET_LIST]\n");
		for(i = 0; i < r; i++){
	    	printf("%d,%d,%s,%s\n",	list_ee[i].type, list_ee[i].stat, list_ee[i].sys_name, list_ee[i].usr_name);
		}
#endif /* DEBUG */

		id = sceNetcnfifSendEE(
			(u_int)list_ee,
			(u_int)p->addr,
			(u_int)(p->data * sizeof(sceNetcnfifList_t))
		);
		while (sceNetcnfifDmaCheck(id));
	    my_free(list_iop); 
	    my_free(list_ee); 
		break;

    case SCE_NETCNFIF_LOAD_ENTRY:
		sceNetcnfifEnvInit(&env, mem_area, mem_area_size, p->f_no_decode);
		r = sceNetCnfLoadEntry(p->fname, p->type, p->usr_name, &env);
		if (r < 0) {
			break;
		}
#ifdef DEBUG
		printf("[SCE_NETCNFIF_LOAD_ENTRY]\n");
		sceNetcnfifEnvDump(&env);
#endif /* DEBUG */

		sceNetcnfifDataInit(&data);
		r = sceNetcnfifReadEnv(&data, &env, p->type);
		if (r < 0) {
			break;
		}
#ifdef DEBUG
		printf("[SCE_NETCNFIF_LOAD_ENTRY]\n");
		sceNetcnfifDataDump(&data);
#endif /* DEBUG */

		id = sceNetcnfifSendEE(
			(u_int)&data,
			(u_int)p->addr,
			(u_int)sizeof(sceNetcnfifData_t)
		);
		while (sceNetcnfifDmaCheck(id));
		break;

    case SCE_NETCNFIF_ADD_ENTRY:
#ifdef DEBUG
		printf("[SCE_NETCNFIF_ADD_ENTRY]\n");
		sceNetcnfifDataDump(&data);
#endif /* DEBUG */
		sceNetcnfifEnvInit(&env, mem_area, mem_area_size, p->f_no_decode);
		r = sceNetcnfifWriteEnv(&env, &data, p->type);
		if (r < 0) {
			break;
		}
#ifdef DEBUG
		printf("[SCE_NETCNFIF_ADD_ENTRY]\n");
		sceNetcnfifEnvDump(&env);
#endif /* DEBUG */

		r = sceNetCnfAddEntry(p->fname, p->type, p->usr_name, &env);
		break;

    case SCE_NETCNFIF_EDIT_ENTRY:
#ifdef DEBUG
		printf("[SCE_NETCNFIF_EDIT_ENTRY]\n");
		sceNetcnfifDataDump(&data);
#endif /* DEBUG */
		sceNetcnfifEnvInit(&env, mem_area, mem_area_size, p->f_no_decode);
		r = sceNetCnfLoadEntry(p->fname, p->type, p->usr_name, &env);
		if (r < 0) {
			break;
		}
		env.mem_ptr = (void *)(((int)env.mem_ptr + 3) & ~3);
		env.mem_base = env.mem_ptr;
		r = sceNetcnfifWriteEnv(&env, &data, p->type);
		if (r < 0) {
			break;
		}
#ifdef DEBUG
		printf("[SCE_NETCNFIF_EDIT_ENTRY]\n");
		sceNetcnfifEnvDump(&env);
#endif /* DEBUG */

		r = sceNetCnfEditEntry(
			p->fname, p->type, p->usr_name, p->new_usr_name, &env
		);
		break;

    case SCE_NETCNFIF_DELETE_ENTRY:
		r = sceNetCnfDeleteEntry(p->fname, p->type, p->usr_name);
		break;

    case SCE_NETCNFIF_SET_LATEST_ENTRY:
		r = sceNetCnfSetLatestEntry(p->fname, p->type, p->usr_name);
		break;

    case SCE_NETCNFIF_DELETE_ALL:
		r = sceNetCnfDeleteAll(p->fname);
		break;

    case SCE_NETCNFIF_CHECK_CAPACITY:
		r = sceNetCnfCheckCapacity(p->fname);
		break;

    case SCE_NETCNFIF_CHECK_ADDITIONAL_AT:
		r = sceNetCnfConvA2S(
			p->fname, chat_additional, sizeof(chat_additional)
		);
		break;

    case SCE_NETCNFIF_CHECK_SPECIAL_PROVIDER:
		sceNetcnfifEnvInit(&env, mem_area, mem_area_size, p->f_no_decode);
		r = sceNetCnfCheckSpecialProvider(p->fname, p->type, p->usr_name, &env);
		break;

    case SCE_NETCNFIF_GET_ADDR:
		p->addr = (int)&data;
		break;

	case SCE_NETCNFIF_CALLBACK:
		callback.type = p->type;
		callback.open = sce_callback_open;
		callback.read = sce_callback_read;
		callback.close = sce_callback_close;
		sce_callback_initialize();
		sceNetCnfSetCallback(&callback);
		break;

	case SCE_NETCNFIF_CALLBACK_END:
		sceNetCnfSetCallback(NULL);
		break;

    case SCE_NETCNFIF_ALLOC_WORKAREA:
		if (mem_area) {
			break;
		}
		mem_area_size = p->data;
		mem_area  = my_alloc(mem_area_size);
		if (!mem_area) {
	    	r = sceNETCNFIF_ALLOC_ERROR;
		}
		break;

    case SCE_NETCNFIF_FREE_WORKAREA:
		if (mem_area) {
	    	my_free(mem_area);
		}
		mem_area_size = 0;
		mem_area = NULL;
		break;

    case SCE_NETCNFIF_SET_ENV:
#ifdef DEBUG
		printf("[SCE_NETCNFIF_SET_ENV]\n");
		sceNetcnfifDataDump(&data);
#endif /* DEBUG */
		sceNetcnfifEnvInit(&env, mem_area, mem_area_size, p->f_no_decode);
		r = sceNetcnfifWriteEnv(&env, &data, p->type);
		if (r < 0) {
			break;
		}

		if (env.root && env.root->pair_head && env.root->pair_head->ifc) {
			redial_count = 0;
	    	for (i = 0; i < sceNetCnf_MAX_PHONE_NUMBERS; i++) {
				if (NULL == (env.root->pair_head->ifc->phone_numbers[i])) {
					continue;
				}
				switch (i) {
				case 0:
				case 1:
				case 2:
					redial_count++;
					break;

				default:
					break;
				}
	    	}
	    	env.root->pair_head->ifc->redial_count = redial_count - 1;

	    	if (env.root->pair_head->ifc->pppoe != 1 &&
			env.root->pair_head->ifc->type != sceNetCnf_IFC_TYPE_PPP) {
        		env.root->pair_head->ifc->type = env.root->pair_head->dev->type;
	    	}
		}
#ifdef DEBUG
		printf("[SCE_NETCNFIF_SET_ENV]\n");
		sceNetcnfifEnvDump(&env);
#endif /* DEBUG */
		p->addr = (int)&env;
		break;

	default:
		break;
    }

    p->data = r;

    return (buf);
}

/* Sifrpc サービススタート */
void sceNetcnfifInterfaceStart(void)
{
    static u_int buf[SCE_NETCNFIF_SSIZE / 4] __attribute__((aligned(64)));

    sceSifInitRpc(0);
    sceSifSetRpcQueue(&qd, GetThreadId());
    sceSifRegisterRpc(
		&sd,
		SCE_NETCNFIF_INTERFACE,
		sceNetcnfifInterfaceServer,
		(void *)&buf[0],
		0,
		0,
		&qd
	);
    sceSifRpcLoop(&qd);
}

/* Sifrpc サービスストップ */
void sceNetcnfifInterfaceStop(void)
{
    if (mem_area) {
		my_free(mem_area);
		mem_area_size = 0;
		mem_area = NULL;
    }
    sceSifRemoveRpc(&sd, &qd);
    sceSifRemoveRpcQueue(&qd);
}

/*** End of file ***/

