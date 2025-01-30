/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0.2
 */
/* 
 *      Netcnf Interface Library
 *
 *      Copyright (C) 2002-2003 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *       Version   Date        Design     Log
 *  --------------------------------------------------------------------
 *       1.0       2003.02.26  ksh        First version
 */

#include <eekernel.h>
#include <stdio.h>
#include <string.h>
#include <eetypes.h>
#include <sifrpc.h>
#include <netcnfif.h>

static volatile int g_sema_callrpc = -1;

static void sce_cbfunc(void *param)
{
	iSignalSema((int)param);
	ExitHandler();
}

static int sce_create_sema(void)
{
	struct SemaParam sparam;

	if (g_sema_callrpc >= 0) {
		return (-1);
	}
	sparam.initCount = 0;
	sparam.maxCount = 1;
	sparam.option = (u_int)"SceNetcnfifCreateSema";
	g_sema_callrpc = CreateSema(&sparam);
	if (g_sema_callrpc < 0) {
		return (-1);
	}
	return (g_sema_callrpc);
}

static int sce_delete_sema(void)
{
	int sid;
	int intr;

	intr = DI();
	sid  = g_sema_callrpc;
	g_sema_callrpc = -1;
	if (intr) {
		EI();
	}
	if (sid >= 0) {
		DeleteSema(sid);
	}
	return (0);
}

int sce_call_rpc(sceSifClientData *pcd, void *buf, int type)
{
	int sid;
	int ret;

	sid = sce_create_sema();
	if (sid < 0) {
		scePrintf("[fatal error] c_sema in netcnfif(type=%d)\n", type);
		return (sid);
	}
	ret = sceSifCallRpc(
		pcd,
		type,
		SIF_RPCM_NOWAIT,
		buf,
		SCE_NETCNFIF_SSIZE,
		buf,
		SCE_NETCNFIF_SSIZE,
		sce_cbfunc,
		(void *)sid
	);
	return (ret);
}

int sce_sync(int mode)
{
	int ret = 1;

	if (g_sema_callrpc < 0) {
		return (sceNETCNFIF_NG);
	}

	switch (mode) {
	case 0:
		WaitSema(g_sema_callrpc);
		sce_delete_sema();
		ret = 0;
		break;

	case 1:
		if (PollSema(g_sema_callrpc) != -1) {
			sce_delete_sema();
			ret = 0;
		}
		break;

	default:
		return (sceNETCNFIF_INVALID_ARG);
	}
	return (ret);
}

/*** End of file ***/

