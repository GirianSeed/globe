/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.4.2
 */
/*
 *			INET Library
 *
 *	Copyright (C) 2000 Sony Computer Entertainment Inc.
 *			All Rights Reserved.
 *
 *			libnet - libnet.c
 */

#include <eekernel.h>
#include <stdio.h>
#include <string.h>
#include <sifrpc.h>
#include <common.h>
#include <libnet.h>

#define	PRIVATE	static
#define	PUBLIC

#define BUFSIZE	( 512 )
#define RPCSIZE	( 2048 )
#define MAX_TH	( 5 )

#define	ERR_STOP	for ( ; ; )

#define	rpc_pad(length)    (((length) + 15) & ~15)

PUBLIC	void libnet_init(sceSifMClientData *cd, unsigned int buffersize, unsigned int stacksize, int priority)
{
	int i;

	/* default: buffersize = 2048, stacksize = 8192, priority = 32 */
	while(1){
		if(sceSifMBindRpcParam(cd, SIFNUM_libnet, 0, buffersize, stacksize, priority) < 0)
			scePrintf("bind error\n");
		if(cd->serve != 0)
			break;
		i = 0x10000;
		while(i--);
	}
}

PUBLIC	void libnet_term(sceSifMClientData *cd)
{
	int ret, i;

	while(1){
		if((ret = sceSifMUnBindRpc(cd, 0)) < 0)
			scePrintf("unbind error\n");
		if(ret == SIFM_UB_OK)
			break;
		i = 0x10000;
		while(i--);
	}
}

PUBLIC	int sceInetName2Address(sceSifMClientData *cd, u_int *buf, int flags, struct sceInetAddress *paddr, char *name, int ms, int nretry)
{
	int ret;
	int len;

	buf[0] = flags;
	buf[1] = ms;
	buf[2] = nretry;
	if(name != NULL){
		strcpy((char *)&buf[3], name);
		len = strlen(name);
	}
	else{
		*(char *)&buf[3] = NULL;
		len = 0;
	}

	ret = sceSifMCallRpc(cd, RPC_SCENAME2ADDR, 0,
		(void *)buf, rpc_pad(len + 1 + sizeof(u_int) * 3),
		(void *)buf, rpc_pad(sizeof(struct sceInetAddress) + sizeof(u_int)), 0, 0);
	if(ret < 0){
		scePrintf("call sceInetName2Address() failed.\n");
		ERR_STOP;
	}

	memcpy(paddr, &buf[1], sizeof(struct sceInetAddress));

	return buf[0];
}

PUBLIC	int sceInetAddress2String(sceSifMClientData *cd, u_int *buf, char *name, int len, struct sceInetAddress *paddr)
{
	int ret;
	u_int port;

	buf[0] = len;
	memcpy(&buf[1], paddr, sizeof(struct sceInetAddress));

	ret = sceSifMCallRpc(cd, RPC_SCEADDR2STR, 0,
		(void *)buf, rpc_pad(sizeof(struct sceInetAddress) + sizeof(u_int)),
		(void *)buf, rpc_pad(sizeof(u_int) + len), 0, 0);
	if(ret < 0){
		scePrintf("call sceInetAddress2String() failed.\n");
		ERR_STOP;
	}

	strcpy(name, (char *)&buf[1]);

	return buf[0];
}

PUBLIC	int sceInetAddress2Name(sceSifMClientData *cd, u_int *buf, int flags, char *name, int len, struct sceInetAddress *paddr, int ms, int nretry)
{
	int ret;

	buf[0] = flags;
	buf[1] = len;
	buf[2] = ms;
	buf[3] = nretry;
	memcpy(&buf[4], paddr, sizeof(struct sceInetAddress));

	ret = sceSifMCallRpc(cd, RPC_SCEADDR2NAME, 0,
		(void *)buf, rpc_pad(sizeof(u_int) * 4 + sizeof(struct sceInetAddress)),
		(void *)buf, rpc_pad(sizeof(u_int) + len), 0, 0);
	if(ret < 0){
		scePrintf("call sceInetAddress2Name() failed.\n");
		ERR_STOP;
	}

	strcpy(name, (char *)&buf[1]);

	return buf[0];
}

PUBLIC	int sceInetCreate(sceSifMClientData *cd, u_int *buf, struct sceInetParam *param)
{
	int ret;

	memcpy(buf, param, sizeof(struct sceInetParam));
	ret = sceSifMCallRpc(cd, RPC_SCECREATE, 0,
		(void *)buf, rpc_pad(sizeof(struct sceInetParam)),
		(void *)buf, rpc_pad(sizeof(u_int)), 0, 0);
	if(ret < 0){
		scePrintf("call sceInetCreate() failed.\n");
		ERR_STOP;
	}

	return buf[0];
}

PUBLIC	int sceInetOpen(sceSifMClientData *cd, u_int *buf, int cid, int ms)
{
	int ret;

	buf[0] = cid;
	buf[1] = ms;

	ret = sceSifMCallRpc(cd, RPC_SCEOPEN, 0,
		(void *)buf, rpc_pad(sizeof(u_int) * 2),
		(void *)buf, rpc_pad(sizeof(u_int)), 0, 0);
	if(ret < 0){
		scePrintf("call sceInetOpen() failed.\n");
		ERR_STOP;
	}

	return buf[0];
}

PUBLIC	int sceInetClose(sceSifMClientData *cd, u_int *buf, int cid, int ms)
{
	int ret;

	buf[0] = cid;
	buf[1] = ms;

	ret = sceSifMCallRpc(cd, RPC_SCECLOSE, 0,
		(void *)buf, rpc_pad(sizeof(u_int) * 2),
		(void *)buf, rpc_pad(sizeof(u_int)), 0, 0);
	if(ret < 0){
		scePrintf("call sceInetClose() failed.\n");
		ERR_STOP;
	}

	return buf[0];
}

PUBLIC	int sceInetRecv(sceSifMClientData *cd, u_int *buf, int cid, void *ptr, int count, int *pflags, int ms)
{
	int ret, flags;

	buf[0] = cid;
	if( pflags ) {
		buf[ 1 ] = *pflags;
	}
	else {
		buf[ 1 ] = 0;
	}
	if((((int *)buf)[2] = count) <= 0){
		scePrintf("%s(): invalid count(%d)\n", __FUNCTION__, count);
		return -1;
	}
	buf[3] = ms;

	ret = sceSifMCallRpc(cd, RPC_SCERECV, 0,
		(void *)buf, rpc_pad(sizeof(u_int) * 4),
		(void *)buf, rpc_pad(sizeof(u_int) * 2 + count), 0, 0);
	if(ret < 0){
		scePrintf("call sceInetRecv() failed.\n");
		ERR_STOP;
	}

	if(pflags)
		*pflags = buf[1];
	if((int)buf[0] > 0)
		memcpy(ptr, &buf[2], buf[0]);

	return buf[0];
}

PUBLIC	int sceInetRecvFrom(sceSifMClientData *cd, u_int *buf, int cid, void *ptr, int count, int *pflags, struct sceInetAddress *addr, int *port, int ms)
{
	int ret, flags;

	buf[0] = cid;
	if( pflags ) {
		buf[ 1 ] = *pflags;
	}
	else {
		buf[ 1 ] = 0;
	}
	if((((int *)buf)[2] = count) <= 0){
		scePrintf("%s(): invalid count(%d)\n", __FUNCTION__, count);
		return -1;
	}
	buf[3] = ms;

	ret = sceSifMCallRpc(cd, RPC_SCERECVF, 0,
		(void *)buf, rpc_pad(sizeof(u_int) * 4),
		(void *)buf, rpc_pad(sizeof(u_int) * 3 + sizeof(struct sceInetAddress) + count), 0, 0);
	if(ret < 0){
		scePrintf("call sceInetRecvFrom() failed.\n");
		ERR_STOP;
	}

	if(pflags)
		*pflags = buf[1];
	if(addr)
		memcpy(addr, &buf[2], sizeof(struct sceInetAddress));
	if(port)
		*port = buf[6];
	if((int)buf[0] > 0)
		memcpy(ptr, &buf[7], buf[0]);

	return buf[0];
}

PUBLIC	int sceInetSend(sceSifMClientData *cd, u_int *buf, int cid, void *ptr, int count, int *pflags, int ms)
{
	int ret;

	buf[0] = cid;
	if( pflags ) {
		buf[ 1 ] = *pflags;
	}
	else {
		buf[ 1 ] = 0;
	}
	buf[2] = count;
	buf[3] = ms;

	memcpy(&buf[4], ptr, count);

	ret = sceSifMCallRpc(cd, RPC_SCESEND, 0,
		(void *)buf, rpc_pad(sizeof(u_int) * 4 + count),
		(void *)buf, rpc_pad(sizeof(u_int) * 2), 0, 0);
	if(ret < 0){
		scePrintf("call sceInetSend() failed.\n");
		ERR_STOP;
	}

	if(pflags)
		*pflags = buf[1];

	return buf[0];
}

PUBLIC	int sceInetSendTo(sceSifMClientData *cd, u_int *buf, int cid, void *ptr, int count, int *pflags, struct sceInetAddress *addr, int port, int ms)
{
	int ret;

	buf[0] = cid;
	buf[1] = count;
	if( pflags ) {
		buf[ 2 ] = *pflags;
	}
	else {
		buf[ 2 ] = 0;
	}
	buf[3] = ms;
	buf[4] = port;
	memcpy(&buf[5], addr, sizeof(struct sceInetAddress));
	memcpy(&buf[9], ptr, count);

	ret = sceSifMCallRpc(cd, RPC_SCESENDT, 0,
		(void *)buf, rpc_pad(sizeof(u_int) * 5 + sizeof(struct sceInetAddress) + count), (void *)buf, rpc_pad(sizeof(u_int) * 2), 0, 0);
	if(ret < 0){
		scePrintf("call sceInetSendTo() failed.\n");
		ERR_STOP;
	}

	if(pflags)
		*pflags = buf[1];

	return buf[0];
}

PUBLIC	int sceInetAbort(sceSifMClientData *cd, u_int *buf, int cid, int flags)
{
	int ret;

	buf[0] = cid;
	buf[1] = flags;

	ret = sceSifMCallRpc(cd, RPC_SCEABORT, 0,
		(void *)buf, rpc_pad(sizeof(u_int) * 2),
		(void *)buf, rpc_pad(sizeof(u_int)), 0, 0);
	if(ret < 0){
		scePrintf("call sceInetClose() failed.\n");
		ERR_STOP;
	}

	return buf[0];
}

PUBLIC	int sceInetControl(sceSifMClientData *cd, u_int *buf, int cid, int code, void *ptr, int len)
{
	int ret;

	buf[0] = cid;
	buf[1] = code;
	buf[2] = len;

	if(ptr != NULL)
		memcpy((void *)&buf[3], ptr, len);

	ret = sceSifMCallRpc(cd, RPC_SCECTL, 0,
		(void *)buf, rpc_pad(sizeof(u_int) * 3 + len),
		(void *)buf, rpc_pad(len + sizeof(u_int) * 3), 0, 0);
	if(ret < 0){
		scePrintf("call sceInetControl() failed.\n");
		ERR_STOP;
	}

	if(ptr != NULL)
		memcpy(ptr, (void *)&buf[3], len);

	return buf[0];
}

PUBLIC	int sceInetGetInterfaceList(sceSifMClientData *cd, u_int *buf, int *interface_id_list, int n)
{
	int ret;

	buf[0] = n;

	ret = sceSifMCallRpc(cd, RPC_SCEGIFLIST, 0,
		(void *)buf, rpc_pad(sizeof(u_int)),
		(void *)buf, rpc_pad((1 + n) * sizeof(u_int)), 0, 0);
	if(ret < 0){
		scePrintf("call sceInetGetInterfaceList() failed.\n");
		ERR_STOP;
	}

	if((int)buf[0] > 0)
		memcpy(interface_id_list, (int *)&buf[1], buf[0]*sizeof(int));

	return buf[0];
}

PUBLIC	int sceInetInterfaceControl(sceSifMClientData *cd, u_int *buf, int interface_id, int code, void *ptr, int len)
{
	int ret;

	buf[0] = interface_id;
	buf[1] = code;
	buf[2] = len;

	if(ptr != NULL)
		memcpy((void *)&buf[3], ptr, len);

	ret = sceSifMCallRpc(cd, RPC_SCEIFCTL, 0,
		(void *)buf, rpc_pad(sizeof(u_int) * 3 + len),
		(void *)buf, rpc_pad(len + 3 * sizeof(u_int)), 0, 0);
	if(ret < 0){
		scePrintf("call sceInetInterfaceControl() failed.\n");
		ERR_STOP;
	}

	if(ptr != NULL)
		memcpy(ptr, (void *)&buf[3], len);

	return buf[0];
}

PUBLIC	int sceInetGetRoutingTable(sceSifMClientData *cd, u_int *buf, struct sceInetRoutingEntry *p, int n)
{
	int ret;

	buf[0] = n;

	ret = sceSifMCallRpc(cd, RPC_SCEGETRT, 0,
		(void *)buf, rpc_pad(sizeof(u_int)),
		(void *)buf, rpc_pad(sizeof(u_int) + sizeof(struct sceInetRoutingEntry) * n), 0, 0);
	if(ret < 0){
		scePrintf("call sceInetGetRoutingTable() failed.\n");
		ERR_STOP;
	}

	memcpy(p, (void *)&buf[1], sizeof(struct sceInetRoutingEntry)*n);

	return buf[0];
}

PUBLIC	int sceInetGetNameServers(sceSifMClientData *cd, u_int *buf, struct sceInetAddress *paddr, int n)
{
	int ret;

	buf[0] = n;

	ret = sceSifMCallRpc(cd, RPC_SCEGETNS, 0,
		(void *)buf, rpc_pad(sizeof(u_int)),
		(void *)buf, rpc_pad(sizeof(u_int) + sizeof(struct sceInetAddress) * n), 0, 0);
	if(ret < 0){
		scePrintf("call sceInetGetNameServers() failed.\n");
		ERR_STOP;
	}

	memcpy(paddr, (void *)&buf[1], sizeof(struct sceInetAddress)*n);

	return buf[0];
}

PUBLIC	int sceInetChangeThreadPriority(sceSifMClientData *cd, u_int *buf, int prio)
{
	int ret;

	buf[0] = prio;

	ret = sceSifMCallRpc(cd, RPC_SCECHPRI, 0,
		(void *)buf, rpc_pad(sizeof(u_int)),
		(void *)buf, rpc_pad(sizeof(u_int)), 0, 0);
	if(ret < 0){
		scePrintf("call sceInetChangePriority() failed.\n");
		ERR_STOP;
	}

	return buf[0];
}

PUBLIC	int sceInetGetLog(sceSifMClientData *cd, u_int *buf, char *log_buf, int len, int ms)
{
	int ret;

	buf[0] = len;
	buf[1] = ms;

	ret = sceSifMCallRpc(cd, RPC_SCEGETLOG, 0,
		(void *)buf, rpc_pad(sizeof(u_int) * 2),
		(void *)buf, rpc_pad(sizeof(u_int) + len), 0, 0);
	if(ret < 0){
		scePrintf("call sceInetGetLog() failed.\n");
		ERR_STOP;
	}

	if((int)buf[0] > 0)
		memcpy(log_buf, (void *)&buf[1], buf[0]);

	return buf[0];
}

PUBLIC	int sceInetAbortLog(sceSifMClientData *cd, u_int *buf)
{
	int ret;

	ret = sceSifMCallRpc(cd, RPC_SCEABORTLOG, 0,
		NULL, 0,
		(void *)buf, rpc_pad(sizeof(u_int)), 0, 0);
	if(ret < 0){
		scePrintf("call sceInetAbortLog() failed.\n");
		ERR_STOP;
	}

	return buf[0];
}

PUBLIC	int reg_handler(sceSifMClientData *cd, u_int *buf)
{
	int ret;

	ret = sceSifMCallRpc(cd, RPC_REGHANDLER, 0,
		NULL, 0,
		(void *)buf, rpc_pad(sizeof(u_int)), 0, 0);
	if(ret < 0){
		scePrintf("RPC call in reg_handler() failed.\n");
		ERR_STOP;
	}

	return buf[0];
}

PUBLIC	int unreg_handler(sceSifMClientData *cd, u_int *buf)
{
	int ret;

	ret = sceSifMCallRpc(cd, RPC_UNREGHANDLER, 0,
		NULL, 0,
		(void *)buf, rpc_pad(sizeof(u_int)), 0, 0);
	if(ret < 0){
		scePrintf("RPC call in unreg_handler() failed.\n");
		ERR_STOP;
	}

	return buf[0];
}

PRIVATE	int load_inet_cnf(sceSifMClientData *cd, u_int *buf, char *fname, char *usr_name)
{
	int ret, len = 0;

	strcpy((char *)&buf[0], fname);
	len = strlen(fname) + 1;
	strcpy((char *)&buf[0] + len, usr_name);
	len += strlen(usr_name) + 1;

	ret = sceSifMCallRpc(cd, RPC_LOADNETCNF, 0,
		(void *)buf, rpc_pad(len),
		(void *)buf, rpc_pad(sizeof(u_int)), 0, 0);
	if(ret < 0){
		scePrintf("RPC call in load_inet_cnf() failed.\n");
		ERR_STOP;
	}

	return buf[0];
}

PRIVATE	int set_config(sceSifMClientData *cd, u_int *buf)
{
	int ret;

	ret = sceSifMCallRpc(cd, RPC_SETCONFIG, 0,
		NULL, 0,
		(void *)buf, rpc_pad(sizeof(u_int)), 0, 0);
	if(ret < 0){
		scePrintf("RPC call in set_config() failed.\n");
		ERR_STOP;
	}

	return buf[0];
}

PRIVATE	int set_config_only(sceSifMClientData *cd, u_int *buf)
{
	int ret;

	ret = sceSifMCallRpc(cd, RPC_SETCONFIGONLY, 0,
		NULL, 0,
		(void *)buf, rpc_pad(sizeof(u_int)), 0, 0);
	if(ret < 0){
		scePrintf("RPC call in set_config_only() failed.\n");
		ERR_STOP;
	}

	return buf[0];
}

PUBLIC	int load_set_conf(sceSifMClientData *cd, u_int *buf, char *fname, char *usr_name)
{
	int ret;

	if((ret = load_inet_cnf(cd, buf, fname, usr_name)) < 0)
		return -1;
	if((ret = set_config(cd, buf)) < 0)
		return -1;

	return ret;
}

PUBLIC	int load_set_conf_only(sceSifMClientData *cd, u_int *buf, char *fname, char *usr_name)
{
	int ret;

	if((ret = load_inet_cnf(cd, buf, fname, usr_name)) < 0)
		return -1;
	if((ret = set_config_only(cd, buf)) < 0)
		return -1;

	return ret;
}

PRIVATE	int get_if_id(sceSifMClientData *cd, u_int *buf, int *if_id)
{
	int if_count, id, if_list[256], ret, flag;

	if(0 > (if_count = sceInetGetInterfaceList(cd, buf, if_list,
			sizeof(if_list) / sizeof(int)))){
		scePrintf("sceInetInterfaceList() failed.\n");
		return -1;
	}
	if(if_count == 0){
		return -2;
	}

	for(id = 0; id < if_count; id++){
		if(0 > (ret = sceInetInterfaceControl(cd, buf, if_list[id], sceInetCC_GetFlags, &flag, sizeof(int)))){
			scePrintf("call sceInetInterfaceControl() failed.\n");
			return -1;
		}
		if(flag & sceInetDevF_Running)
			break;
	}
	if(id == if_count){
		return -2;
	}

	*if_id = if_list[id];

	return 0;
}

PRIVATE	int wait_if_attached(sceSifMClientData *cd, u_int *buf, int *if_id)
{
	int ret;

	ret = sceSifMCallRpc(cd, RPC_WAITIFATTACHED, 0,
		NULL, 0, (void *)buf, rpc_pad(sizeof(u_int) * 2), 0, 0);
	if(ret < 0){
		scePrintf("RPC call in wait_if_attached() failed.\n");
		ERR_STOP;
	}

	*if_id = buf[1];

	return buf[0];
}

PUBLIC	int get_interface_id(sceSifMClientData *cd, u_int *buf, int *if_id)
{
	int ret;

	if((ret = get_if_id(cd, buf, if_id)) < 0){
		if((ret = wait_if_attached(cd, buf, if_id)) < 0)
			return -1;
	}

	return ret;
}

PRIVATE	int wait_if_started(sceSifMClientData *cd, u_int *buf, int *if_id)
{
	int ret;

	ret = sceSifMCallRpc(cd, RPC_WAITIFSTARTED, 0,
		NULL, 0, (void *)buf, rpc_pad(sizeof(u_int) * 2), 0, 0);
	if(ret < 0){
		scePrintf("RPC call in wait_if_started() failed.\n");
		ERR_STOP;
	}

	*if_id = buf[1];

	return buf[0];
}

PRIVATE	int get_myaddr(sceSifMClientData *cd, u_int *buf, int if_id, struct sceInetAddress *myaddr)
{
	int ret;

	if(0 > (ret = sceInetInterfaceControl(cd, buf, if_id, sceInetCC_GetAddress, myaddr, sizeof(struct sceInetAddress)))){
		scePrintf("call sceInetInterfaceControl() failed.\n");
		return -1;
	}
	return 0;
}

PUBLIC	int wait_get_addr_only(sceSifMClientData *cd, u_int *buf, int *if_id, struct sceInetAddress *addr)
{
	int ret;

	if((ret = wait_if_started(cd, buf, if_id)) < 0){
		return -1;
	}
	if((ret = get_myaddr(cd, buf, *if_id, addr)) < 0){
		return -1;
	}

	return ret;
}

PUBLIC	int wait_get_addr(sceSifMClientData *cd, u_int *buf, int *if_id, struct sceInetAddress *addr)
{
	int ret;

	if((ret = get_if_id(cd, buf, if_id)) < 0){
		if((ret = wait_if_started(cd, buf, if_id)) < 0)
			return -1;
	}
	if((ret = get_myaddr(cd, buf, *if_id, addr)) < 0){
		return -1;
	}

	return ret;
}

PUBLIC	int up_interface(sceSifMClientData *cd, u_int *buf, int id)
{
	int ret;

	buf[0] = (u_int)id;

	ret = sceSifMCallRpc(cd, RPC_UPIF, 0,
		(void *)buf, rpc_pad(sizeof(u_int)),
		(void *)buf, rpc_pad(sizeof(u_int)), 0, 0);
	if(ret < 0){
		scePrintf("RPC call in up_interface() failed.\n");
		ERR_STOP;
	}

	return buf[0];
}

PUBLIC	int down_interface(sceSifMClientData *cd, u_int *buf, int id)
{
	int ret;

	buf[0] = (u_int)id;

	ret = sceSifMCallRpc(cd, RPC_DOWNIF, 0,
		(void *)buf, rpc_pad(sizeof(u_int)),
		(void *)buf, rpc_pad(sizeof(u_int)), 0, 0);
	if(ret < 0){
		scePrintf("RPC call in down_interface() failed.\n");
		ERR_STOP;
	}

	return buf[0];
}

/*** End of file ***/

