/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0.2
 */
/* 
 *      Netcnf Interface Library
 *
 *                         Version 1.6
 *                         Shift-JIS
 *
 *      Copyright (C) 2002-2003 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                         netcnfif.c
 *
 *       Version   Date        Design     Log
 *  --------------------------------------------------------------------
 *       1.1       2002.01.28  tetsu      First version
 *       1.2       2002.01.31  tetsu      Add release version string
 *       1.3       2002.02.10  tetsu      Add sceNetcnfifCheckAdditionalAT()
 *       1.4       2002.05.05  tetsu      Add sceNetcnfifCheckSpecialProvider()
 *       1.5       2002.10.19  tetsu      Change path of netcnfif.h
 *       1.5.1     2002.10.31  ksh        Add sceNetcnfifLoadEntryAuto
 *       1.6       2003.01.14  ksh        callback & remake
 *       1.6.1     2003.02.26  ksh        sync added
 *       1.7       2003.04.02  ksh        init & term
 *       1.7.1     2003.04.10  ksh        char * to const char *
 */

#include <eekernel.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <eetypes.h>
#include <sifrpc.h>
#include <netcnfif.h>
#include "rpc.h"
#include "callback.h"


#define	FREE_AND_RETURN(x)	{ free(list_ifc_or_dev); free(list_net); return(x); }

static sceSifClientData cd;
static u_char buf[SCE_NETCNFIF_SSIZE] __attribute__((aligned(64)));

static int _f_no_decode = 0;

static int ginit = 0;

int sceNetcnfifSetup(void)
{
	int i;

	if (ginit) {
		return (sceNETCNFIF_OK);
	}
	for ( ; ; ) {
		if (sceSifBindRpc(&cd, SCE_NETCNFIF_INTERFACE, SIF_RPCM_NOWAIT) < 0) {
			return (sceNETCNFIF_NG);
		}
		while (sceSifCheckStatRpc((sceSifRpcData *)&cd) != 0);
		if (cd.serve != 0) {
			break;
		}
		i = 0x10000;
		while (i--);
	}
	ginit = 1;
	return (sceNETCNFIF_OK);
}

int sceNetcnfifInit(void)
{
	int ret;
	
	ret = sceNetcnfifSetup();
	memset(buf, 0, sizeof(sceNetcnfifArg_t));
	return (ret);
}

int sceNetcnfifCreateCallbackThread(sceNetcnfifCallback_t *pcallback, char *stack, int size, int priority)
{
	sceNetcnfifArg_t *p;
	int tid;
	int ret;

	if (pcallback == NULL) {
		return (sceNETCNFIF_INVALID_ARG);
	}

	tid = sce_create_callback_thread(pcallback, stack, size, 1);
	if (tid < 0) {
		return (sceNETCNFIF_NG);
	}

	sceNetcnfifInit();
	p = (sceNetcnfifArg_t *)buf;

	p->type = pcallback->type;

	ret = sceSifCallRpc(
		&cd,
		SCE_NETCNFIF_CALLBACK,
		0,
		&buf[0],
		SCE_NETCNFIF_SSIZE,
		&buf[0],
		SCE_NETCNFIF_SSIZE,
		0,
		0
	);
	if (ret < 0) {
		sce_delete_callback_thread(tid);
		return (ret);
	}
	ChangeThreadPriority(tid, priority);

	return (tid);
}

int sceNetcnfifDeleteCallbackThread(int tid)
{
	int ret;

	sce_delete_callback_thread(tid);

	sceNetcnfifInit();

	ret = sceSifCallRpc(
		&cd,
		SCE_NETCNFIF_CALLBACK_END,
		0,
		&buf[0],
		SCE_NETCNFIF_SSIZE,
		&buf[0],
		SCE_NETCNFIF_SSIZE,
		0,
		0
	);
	if (ret < 0) {
		return (ret);
	}
	return (sceNETCNFIF_OK);
}

int sceNetcnfifSync(int mode)
{
	return (sce_sync(mode));
}

int sceNetcnfifCheck(void)
{
	return (sce_sync(1));
}

int sceNetcnfifGetResult(sceNetcnfifArg_t *result)
{
	sceNetcnfifArg_t *p;

	p = (sceNetcnfifArg_t *)buf;
	memcpy(result, p, sizeof(sceNetcnfifArg_t));
	return (sceNETCNFIF_OK);
}

int sceNetcnfifSetFNoDecode(int f_no_decode)
{
    _f_no_decode = f_no_decode;
	return (sceNETCNFIF_OK);
}

int sceNetcnfifGetCount(const char *fname, int type)
{
	sceNetcnfifArg_t *p;
	int ret;

	sceNetcnfifInit();
	p = (sceNetcnfifArg_t *)buf;

	strcpy(p->fname, fname);
	p->type = type;

	ret = sce_call_rpc(&cd, &buf[0], SCE_NETCNFIF_GET_COUNT);
	return (ret);
}

int sceNetcnfifGetList(const char *fname, int type, sceNetcnfifList_t *addr, int size)
{
	sceNetcnfifArg_t *p;
	int ret;

	sceNetcnfifInit();
	p = (sceNetcnfifArg_t *)buf;

	strcpy(p->fname, fname);
	p->type = type;
	p->addr = (int)addr;
	p->data = size;

	ret = sce_call_rpc(&cd, &buf[0], SCE_NETCNFIF_GET_LIST);
	return (ret);
}

int sceNetcnfifLoadEntry(const char *fname, int type, const char *usr_name, sceNetcnfifData_t *addr)
{
	sceNetcnfifArg_t *p;
	int ret;

	sceNetcnfifInit();
	p = (sceNetcnfifArg_t *)buf;

	strcpy(p->fname, fname);
	p->type = type;
	strcpy(p->usr_name, usr_name);
	p->addr = (int)addr;
	p->f_no_decode = _f_no_decode;

	ret = sce_call_rpc(&cd, &buf[0], SCE_NETCNFIF_LOAD_ENTRY);
	return (ret);
}

int sceNetcnfifLoadEntryAuto(const char *fname, int type_with_flags, const char *usr_name, sceNetcnfifData_t *addr)
{
	sceNetcnfifArg_t if_arg;
	sceNetcnfifList_t *list_ifc_or_dev = NULL;
	sceNetcnfifList_t *list_net = NULL;
	sceNetcnfifList_t *ptr_target_ifc_or_dev = NULL;
	sceNetcnfifList_t *ptr_target_net = NULL;
	sceNetcnfifList_t *ptr;
	int type = type_with_flags & ~sceNETCNFIF_F_MASK;
	int flags = type_with_flags & sceNETCNFIF_F_MASK;
	int mode = 1;
	int r;
	int i;

	if (type != sceNetcnfifArg_type_ifc && type != sceNetcnfifArg_type_dev) {
		FREE_AND_RETURN(sceNETCNFIF_INVALID_TYPE);
	}
	if (flags & sceNETCNFIF_F_USE_WAITSEMA) {
		mode = 0;
	}

	/*** Try to find ifc or dev ***/
	sceNetcnfifGetCount(fname, type);
	while (sceNetcnfifSync(mode));
	sceNetcnfifGetResult(&if_arg);
	if (if_arg.data <= 0) {
		FREE_AND_RETURN(sceNETCNFIF_NG);
	}
	r = if_arg.data;

	list_ifc_or_dev = (sceNetcnfifList_t *)memalign(64, r * sizeof(sceNetcnfifList_t));
	if (list_ifc_or_dev == NULL) {
		FREE_AND_RETURN(sceNETCNFIF_ALLOC_ERROR);
	}
	FlushCache(0);
	sceNetcnfifGetList(fname, type, list_ifc_or_dev, r);
	while (sceNetcnfifSync(mode));
	sceNetcnfifGetResult(&if_arg);
	if (if_arg.data < 0) {
		FREE_AND_RETURN(sceNETCNFIF_NG);
	}

	ptr = list_ifc_or_dev;
	for (i = 0; i < r; i++) {
		if (strcmp(usr_name, ptr->usr_name) == 0) {
			ptr_target_ifc_or_dev = ptr;
			break;
		}
		ptr++;
	}
	if (ptr_target_ifc_or_dev == NULL) {
		FREE_AND_RETURN(sceNETCNFIF_ENTRY_NOT_FOUND);
	}

	/*** Try to find net ***/
	sceNetcnfifGetCount(fname, sceNetcnfifArg_type_net);
	while (sceNetcnfifSync(mode));
	sceNetcnfifGetResult(&if_arg);
	if (if_arg.data <= 0) {
		FREE_AND_RETURN(sceNETCNFIF_NG);
	}
	r = if_arg.data;

	list_net = (sceNetcnfifList_t *)memalign(64, r * sizeof(sceNetcnfifList_t));
	if (list_net == NULL) {
		FREE_AND_RETURN(sceNETCNFIF_ALLOC_ERROR);
	}
	FlushCache(0);
	sceNetcnfifGetList(fname, sceNetcnfifArg_type_net, list_net, r);
    while (sceNetcnfifSync(mode));
	sceNetcnfifGetResult(&if_arg);
	if (if_arg.data < 0) {
		FREE_AND_RETURN(sceNETCNFIF_NG);
	}

	ptr = list_net;
	for (i = 0; i < r; i++) {
		FlushCache(0);
		sceNetcnfifLoadEntry(fname, sceNetcnfifArg_type_net, ptr->usr_name, addr);
		while(sceNetcnfifSync(mode));
		sceNetcnfifGetResult(&if_arg);
		if (if_arg.data < 0) {
			FREE_AND_RETURN(sceNETCNFIF_NG);
		}
		if (type == sceNetcnfifArg_type_ifc) {
			if (strcmp(ptr_target_ifc_or_dev->sys_name, addr->attach_ifc) == 0) {
				if (ptr_target_net != NULL) {
					FREE_AND_RETURN(sceNETCNFIF_TOO_MANY_ENTRIES);
				}
				ptr_target_net = ptr;
			}
		}
		else {
			if (strcmp(ptr_target_ifc_or_dev->sys_name, addr->attach_dev) == 0) {
				if (ptr_target_net != NULL) {
					FREE_AND_RETURN(sceNETCNFIF_TOO_MANY_ENTRIES);
				}
				ptr_target_net = ptr;
			}
		}
		ptr++;
	}
	if (ptr_target_net == NULL) {
		FREE_AND_RETURN(sceNETCNFIF_ENTRY_NOT_FOUND);
	}

	/*** load entry ***/
	FlushCache(0);
	sceNetcnfifLoadEntry(fname, sceNetcnfifArg_type_net, ptr_target_net->usr_name, addr);
	while(sceNetcnfifSync(mode));
	sceNetcnfifGetResult(&if_arg);
	if (if_arg.data < 0) {
		FREE_AND_RETURN(sceNETCNFIF_NG);
	}

	FREE_AND_RETURN(0);
}

int sceNetcnfifAddEntry(const char *fname, int type, const char *usr_name)
{
	sceNetcnfifArg_t *p;
	int ret;

	sceNetcnfifInit();
	p = (sceNetcnfifArg_t *)buf;

	strcpy(p->fname, fname);
	p->type = type;
	strcpy(p->usr_name, usr_name);
	p->f_no_decode = _f_no_decode;

	ret = sce_call_rpc(&cd, &buf[0], SCE_NETCNFIF_ADD_ENTRY);
	return (ret);
}

int sceNetcnfifEditEntry(const char *fname, int type, const char *usr_name, const char *new_usr_name)
{
	sceNetcnfifArg_t *p;
	int ret;

	sceNetcnfifInit();
	p = (sceNetcnfifArg_t *)buf;

	strcpy(p->fname, fname);
	p->type = type;
	strcpy(p->usr_name, usr_name);
	strcpy(p->new_usr_name, new_usr_name);
	p->f_no_decode = _f_no_decode;

	ret = sce_call_rpc(&cd, &buf[0], SCE_NETCNFIF_EDIT_ENTRY);
	return (ret);
}

int sceNetcnfifDeleteEntry(const char *fname, int type, const char *usr_name)
{
	sceNetcnfifArg_t *p;
	int ret;

	sceNetcnfifInit();
	p = (sceNetcnfifArg_t *)buf;

	strcpy(p->fname, fname);
	p->type = type;
	strcpy(p->usr_name, usr_name);

	ret = sce_call_rpc(&cd, &buf[0], SCE_NETCNFIF_DELETE_ENTRY);
	return (ret);
}

int sceNetcnfifSetLatestEntry(const char *fname, int type, const char *usr_name)
{
	sceNetcnfifArg_t *p;
	int ret;

	sceNetcnfifInit();
	p = (sceNetcnfifArg_t *)buf;

	strcpy(p->fname, fname);
	p->type = type;
	strcpy(p->usr_name, usr_name);

	ret = sce_call_rpc(&cd, &buf[0], SCE_NETCNFIF_SET_LATEST_ENTRY);
	return (ret);
}

int sceNetcnfifDeleteAll(const char *fname)
{
	sceNetcnfifArg_t *p;
	int ret;

	sceNetcnfifInit();
	p = (sceNetcnfifArg_t *)buf;

	strcpy(p->fname, fname);

	ret = sce_call_rpc(&cd, &buf[0], SCE_NETCNFIF_DELETE_ALL);
	return (ret);
}

int sceNetcnfifCheckCapacity(const char *fname)
{
	sceNetcnfifArg_t *p;
	int ret;

	sceNetcnfifInit();
	p = (sceNetcnfifArg_t *)buf;

	strcpy(p->fname, fname);

	ret = sce_call_rpc(&cd, &buf[0], SCE_NETCNFIF_CHECK_CAPACITY);
	return (ret);
}

int sceNetcnfifCheckAdditionalAT(const char *additional_at)
{
	sceNetcnfifArg_t *p;
	int ret;

	sceNetcnfifInit();
	p = (sceNetcnfifArg_t *)buf;

	strcpy(p->fname, additional_at);

	ret = sce_call_rpc(&cd, &buf[0], SCE_NETCNFIF_CHECK_ADDITIONAL_AT);
	return (ret);
}

int sceNetcnfifCheckSpecialProvider(const char *fname, int type, const char *usr_name)
{
	sceNetcnfifArg_t *p;
	int ret;

	sceNetcnfifInit();
	p = (sceNetcnfifArg_t *)buf;

	strcpy(p->fname, fname);
	p->type = type;
	strcpy(p->usr_name, usr_name);
	p->f_no_decode = _f_no_decode;

	ret = sce_call_rpc(&cd, &buf[0], SCE_NETCNFIF_CHECK_SPECIAL_PROVIDER);
	return (ret);
}

int sceNetcnfifGetAddr(void)
{
	int ret;

	sceNetcnfifInit();

	ret = sce_call_rpc(&cd, &buf[0], SCE_NETCNFIF_GET_ADDR);
	return (ret);
}

int sceNetcnfifAllocWorkarea(int size)
{
	sceNetcnfifArg_t *p;
	int ret;

	sceNetcnfifInit();
	p = (sceNetcnfifArg_t *)buf;

	p->data = size;

	ret = sce_call_rpc(&cd, &buf[0], SCE_NETCNFIF_ALLOC_WORKAREA);
	return (ret);
}

int sceNetcnfifFreeWorkarea(void)
{
	int ret;

	sceNetcnfifInit();

	ret = sce_call_rpc(&cd, &buf[0], SCE_NETCNFIF_FREE_WORKAREA);
	return (ret);
}

int sceNetcnfifSetEnv(int type)
{
	sceNetcnfifArg_t *p;
	int ret;

	sceNetcnfifInit();
	p = (sceNetcnfifArg_t *)buf;

	p->type = type;
	p->f_no_decode = _f_no_decode;

	ret = sce_call_rpc(&cd, &buf[0], SCE_NETCNFIF_SET_ENV);
	return (ret);
}

int sceNetcnfifTerm(void)
{
	int ret;

	ret = sceNetcnfifFreeWorkarea();
	sce_sync(0);
	ginit = 0;
	return (ret);
}

/*** End of file ***/

