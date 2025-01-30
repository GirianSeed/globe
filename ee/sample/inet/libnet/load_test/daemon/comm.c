/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/*
 *                     INET Library
 *
 *      Copyright (C) 2000 Sony Computer Entertainment Inc.
 *                       All Rights Reserved.
 *
 *                         daemon - comm.c
 */

#include <eekernel.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sifcmd.h>
#include <sifrpc.h>
#include <sifdev.h>
#include <malloc.h>
#include <libnet.h>
#include <libmrpc.h>
#include "main.h"
#include "comm.h"

#define RECVSIZE 2048

static int recvmsg(sceSifMClientData *cd, u_int *buf, int cid, u_char *ptr, int count, int *pflags, int ms){
	int len, left = count;

	while(left > 0){
		len = sceInetRecv(cd, buf, cid, ptr, count, pflags, ms);
		if(len < 0)
			return len;
		else if(len == 0)
			break;
		left -= len;
		ptr += len;
	}

	return (count - left);
}

void load_testd(void *data){
	u_char trans_buf[RECVSIZE];
	int pktsize;
	int cid;
	int ret;
	struct sceInetParam param;
	sceSifMClientData cd;
	int sid = *(int *)data;
	u_int *net_buf;
	int flag;

	if((net_buf = (u_int *)memalign(64, RPCSIZE)) == NULL){
		scePrintf("malloc failed.\n");
		return;
	}

	libnet_init(&cd, 2048, 8192, 32);

	memset(&param, 0, sizeof(struct sceInetParam));
	param.type = sceINETT_LISTEN;
	param.local_port = PASSIVE_PORT;
	if(sceINETE_OK != (ret = sceInetName2Address(&cd, net_buf, 0, &param.remote_addr, NULL, -1, -1)))
		scePrintf("sceInetName2Address() failed.\n");
	param.remote_port = sceINETP_ANY;

	if((cid = sceInetCreate(&cd, net_buf, &param)) <= 0)
		scePrintf("sceInetCreate() returns %d\n", cid);

	scePrintf("before sceInetOpen(). cid = 0x%x, sid = %d\n", cid, sid);
	if(sceINETE_OK != (ret = sceInetOpen(&cd, net_buf, cid, -1))){
		scePrintf("sceInetOpen() failed. ret = %d\n", ret);
		return;
	}
	scePrintf("after sceInetOpen(). sid = %d\n", sid);

	SignalSema(passive_sema);

	srand((u_int)GetThreadId);

	flag = 0;
	if((ret = sceInetRecv(&cd, net_buf, cid, &pktsize, 4, &flag, -1)) < 0){
		scePrintf("sceInetRecv() failed. ret = %d\n", ret);
		return;
	}

	pktsize = ntohl(pktsize);
	scePrintf("pktsize = %d\n", pktsize);

	/* get msg */
	while(1) {
		ret = recvmsg(&cd, net_buf, cid, trans_buf, pktsize, &flag, -1);
		if(flag & sceINETF_FIN)
			break;
		else if(ret != pktsize)
			scePrintf("read byte is not %d but %d\n", pktsize, ret);

		flag = 0;
		ret = sceInetSend(&cd, net_buf, cid, trans_buf, pktsize, &flag, -1);
		if(ret != pktsize)
			scePrintf("written byte is not %d but %d\n", pktsize, ret);

	}

	sceInetClose(&cd, net_buf, cid, -1);
	scePrintf("after sceInetClose(). sid = %d\n", sid);
	connection[sid].fin = 1;
	libnet_term(&cd);
	free(net_buf);
	ChangeThreadPriority(GetThreadId(), 1);
	SignalSema(fin_sema);
	ExitThread();
}

/*** End of file ***/

