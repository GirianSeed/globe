/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.7.1
 */
/* 
 *
 *      Copyright (C) 2002-2003 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.1            2003/01/08      ksh         First version
 */

#include <eekernel.h>
#include <sifrpc.h>
#include <netcnfif.h>

static	sceNetcnfifCallback_t gcallback;
static	sceSifServeData gsd;
static	sceSifQueueData gqd;

static	void	*sce_callback(u_int fno, void *buf, int size)
{
	int *rpcbuf = (int *)buf;
	int ret = -1;

	switch (fno) {
	case SCE_NETCNFIF_CALLBACK_OPEN:
		{
		const int flags = rpcbuf[0];
		const int mode = rpcbuf[1];
		const int devicelen = rpcbuf[2];
		const int pathnamelen = rpcbuf[3];
		const char *device = (const char *)&rpcbuf[4];
		const char *pathname = (const char *)&rpcbuf[4] + devicelen;

		if (gcallback.open) {
			ret = (*gcallback.open)(
				device,
				pathname,
				flags,
				mode,
				&rpcbuf[1]
			);
		}
		}
		break;

	case SCE_NETCNFIF_CALLBACK_READ:
		{
		const int fd = rpcbuf[0];
		const int devicelen = rpcbuf[1];
		const int pathnamelen = rpcbuf[2];
		const int offset = rpcbuf[3];
		const int size = rpcbuf[4];
		const char *device = (const char *)&rpcbuf[5];
		const char *pathname = (const char *)&rpcbuf[5] + devicelen;
		
		if (gcallback.read) {
			ret = (*gcallback.read)(
				fd,
				device,
				pathname,
				&rpcbuf[1],
				offset,
				size
			);
		}
		}
		break;
		
	case SCE_NETCNFIF_CALLBACK_CLOSE:
		{
		const int fd = rpcbuf[0];
		
		if (gcallback.close) {
			ret = (*gcallback.close)(fd);
		}
		}
		break;
		
	default:
		break;
	}

	rpcbuf[0] = ret;

	return (buf);
}

static void sce_callback_function(void *data)
{
	static u_int buf[SCE_NETCNFIF_SSIZE / 4] __attribute__((aligned(64)));

	sceSifInitRpc(0);
	sceSifSetRpcQueue(&gqd, GetThreadId());
	sceSifRegisterRpc(
		&gsd,
		SCE_NETCNFIF_INTERFACE,
		sce_callback,
		(void *)&buf[0],
		0,
		0,
		&gqd
	);
	sceSifRpcLoop(&gqd);
}

int	sce_create_callback_thread(sceNetcnfifCallback_t *pcallback, char *stack, int size, int priority)
{
	struct ThreadParam tp;
	int tid;
	int ret;

	tp.entry = sce_callback_function;
	tp.stack = stack;
	tp.stackSize = size;
	tp.initPriority = priority;
	tp.gpReg = &_gp;

	tid = CreateThread(&tp);
	if (tid <= 0) {
		return (-1);
	}

	gcallback = *pcallback;
	ret = StartThread(tid, NULL);
	if (ret < 0) {
		DeleteThread(tid);
		return (-1);
	}
	return (tid);
}

void sce_delete_callback_thread(int tid)
{
	sceSifRemoveRpc(&gsd, &gqd);
	sceSifRemoveRpcQueue(&gqd);
	TerminateThread(tid);
	DeleteThread(tid);
}
																				
/*** End of file ***/

