/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
 */
/* 
 *                          Libnet Library
 *
 *      Copyright (C) 2000-2002 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *       Version      Date        Design      Log
 *  --------------------------------------------------------------------
 *       1.1.0        2002/01/24  ksh         remake
 */

/**********************************************************************/
#if 1
#define USB_ETHERNET
#endif
#if 0
#define USB_ETHERNET_WITH_PPPOE
#endif
#if 0
#define HDD_ETHERNET
#endif
#if 0
#define HDD_ETHERNET_WITH_PPPOE
#endif
#if 0
#define MODEM
#endif

#if 1
#define UP_INTERFACE_AUTO
#endif

#include <eekernel.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <sifcmd.h>
#include <sifrpc.h>
#include <sifdev.h>
#include <libnet.h>
#include <libmrpc.h>

#if	( defined( HDD_ETHERNET ) || defined( HDD_ETHERNET_WITH_PPPOE ) )
#include "poweroff.h"
#endif

#define	SCETOP            "host0:/usr/local/sce/"
#define	MAIN_MOD_ROOT     SCETOP "iop/modules/"
#define	LIBNET_MOD_ROOT   SCETOP "iop/modules/"
#define	SETAPP_ROOT       SCETOP "conf/neticon/japanese/"
#define	IOP_MOD_INET      MAIN_MOD_ROOT "inet.irx"
#define	IOP_MOD_AN986     MAIN_MOD_ROOT "an986.irx"
#define	IOP_MOD_USBD      MAIN_MOD_ROOT "usbd.irx"
#define	IOP_MOD_NETCNF    MAIN_MOD_ROOT "netcnf.irx"
#define	IOP_MOD_INETCTL   MAIN_MOD_ROOT "inetctl.irx"
#define	IOP_MOD_MSIFRPC   MAIN_MOD_ROOT "msifrpc.irx"
#define	IOP_MOD_DEV9      MAIN_MOD_ROOT "dev9.irx"
#define	IOP_MOD_SMAP      MAIN_MOD_ROOT "smap.irx"
#define	IOP_MOD_PPP       MAIN_MOD_ROOT "ppp.irx"
#define	IOP_MOD_PPPOE     MAIN_MOD_ROOT "pppoe.irx"
#define	IOP_MOD_LIBNET    LIBNET_MOD_ROOT "libnet.irx"
#define	IOP_MOD_MODEMDRV  ""
#define	NET_DB            SCETOP "conf/net/net.db"
#define	INETCTL_ARG       "-no_auto" "\0" "-no_decode"
#define	NETCNF_ICON       SETAPP_ROOT "SYS_NET.ICO"
#define	NETCNF_ICONSYS    SETAPP_ROOT "icon.sys"
#define	NETCNF_ARG        "icon=" NETCNF_ICON "\0" "iconsys=" NETCNF_ICONSYS
#define	MODEMDRV_ARG      ""

#if	defined( USB_ETHERNET )
#define	USR_CONF_NAME     "Combination4"
#elif	defined( USB_ETHERNET_WITH_PPPOE )
#define	USR_CONF_NAME     "Combination5"
#elif	defined( HDD_ETHERNET )
#define	USR_CONF_NAME     "Combination6"
#elif	defined( HDD_ETHERNET_WITH_PPPOE )
#define	USR_CONF_NAME     "Combination7"
#elif	defined( MODEM )
#define	USR_CONF_NAME     ""
#endif


/**********************************************************************/
#define PRIVATE static
#define PUBLIC
#define ERR_STOP        for ( ; ; )

#define BUFSIZE	( 2048 )
#define NETBUFSIZE	( 512 )
#define RPCSIZE	( NETBUFSIZE * 4 )
#define STACKSIZE	( 0x4000 )
#define PRIO	( 63 )
#define MAX_THREAD	( 10 )
#define SERVER_PORT	( 9012 )

struct connection_data {
	u_char *stack;
	int sema;
	int fid;
};

PRIVATE	int pktsize = 0;
PRIVATE	int th_num = 0;
PRIVATE	char server_name[128];
PRIVATE	struct connection_data *connection;


PRIVATE void	LoadModule( const char *module, int args, const char *argp )
{
	while ( sceSifLoadModule( module, args, argp ) < 0 ) {
		scePrintf( "%s cannot be loaded.\n", module );
	}
}

PRIVATE	int recvmsg(sceSifMClientData *cd, u_int *buf, int cid, void *ptr, int count, int *pflags, int ms)
{
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

PRIVATE	void make_connection(sceSifMClientData *cd, u_int *net_buf, int fid)
{
	int ret, w = 1500;
	int cid;
	struct sceInetParam param;
	char trans_buf[BUFSIZE], mem[BUFSIZE];
	int ps;
	int flag;
	int i;

	memset(&param, 0, sizeof(struct sceInetParam));

	param.type = sceINETT_CONNECT;
	param.local_port = sceINETP_AUTO;
	if((ret = sceInetName2Address(cd, net_buf, 0, &param.remote_addr, server_name, 0, 0)) < 0){
		scePrintf("return value of sceInetName2Address(): %d\n", ret);
		return;
	}
	param.remote_port = SERVER_PORT;

	if((cid = sceInetCreate(cd, net_buf, &param)) <= 0){
		scePrintf("sceInetCreate() returns %d\n", cid);
		return;
	}

	if(sceINETE_OK != (ret = sceInetOpen(cd, net_buf, cid, -1))){
		scePrintf("test: sceInetOpen() failed.(%d)\n", ret);
		(void) sceInetClose(cd, net_buf, cid, -1);
		return;
	}

	ps = htonl(pktsize);
	flag = 0;
	ret = sceInetSend(cd, net_buf, cid, &ps, 4, &flag, -1);

	srand((u_int)GetThreadId);

	while(w-- > 0){
		if(w%100 == 0)
			scePrintf("thread %d : w = %d\n", fid, w);

		for(i = 0; i < pktsize; i++)
			trans_buf[i] =
				(int)((double)rand() / ((double)RAND_MAX + 1) * pktsize);
		memcpy(mem, trans_buf, pktsize);

		flag = 0;
		ret = sceInetSend(cd, net_buf, cid, trans_buf, pktsize, &flag, -1);

		if(ret != pktsize)
			scePrintf("write count is not %d byte, but %d byte\n", pktsize, ret);

		flag = 0;
		ret = recvmsg(cd, net_buf, cid, trans_buf, pktsize, &flag, -1);

		if(flag & sceINETF_FIN)
			return;
		else if(ret != pktsize)
			scePrintf("read count is not %d byte, but %d byte\n", pktsize, ret);

		if(memcmp(trans_buf, mem, pktsize))
			scePrintf("line %d: sent data and receipt data is not same.\n",
				__LINE__);
	}

	(void) sceInetClose(cd, net_buf, cid, -1);

	return;
}

PRIVATE	void thread_func(void *data)
{
	int fid = *(int *)data;
	sceSifMClientData cd;
	u_int *net_buf;

	if((net_buf = (u_int *)memalign(64, RPCSIZE)) == NULL){
		scePrintf("malloc failed.\n");
		return;
	}
	
	if ( libnet_init(&cd, 2048, 8192, 32) == sceLIBNETE_OK ) {
		make_connection( &cd, net_buf, fid );
		libnet_term( &cd );
	}
	else {
		scePrintf( "libnet_init() failed.\n" );
	}

	free(net_buf);
	SignalSema(connection[fid].sema);
}

PRIVATE	int start_thread(void *func, char *stackp, int size, int prio, void *argp)
{
	struct ThreadParam tp;
	int tid;

	tp.entry = func;
	tp.stack = stackp;
	tp.stackSize = size;
	tp.initPriority = prio;
	tp.gpReg = &_gp;

	if ((tid = CreateThread(&tp)) <= 0){
		scePrintf("start_thread(): CreateThread() failed.\n");
		return -1;
	}
	if(StartThread(tid, argp) < 0){
		scePrintf("start_thread(): StartThread() failed.\n");
		DeleteThread(tid);
		return -1;
	}
	return 0;
}

int main(int argc, char **argv)
{
	int i;
	int ret;
	int if_id;
	struct SemaParam sema_param;
	sceSifMClientData cd;
	u_int *net_buf;
	struct sceInetAddress myaddr;
	char myname[128];
	char myptrname[128];

	if(argc != 4){
		scePrintf("usage: %s <saddr> <th_num> <pktsize>\n", argv[0]);
		ERR_STOP;
	}
	
	strcpy(server_name, argv[1]);

	if ((th_num = atoi(argv[2])) <= 0){
		scePrintf("th_num = %d\n", th_num);
		ERR_STOP;
	}

	if ((pktsize = atoi(argv[3])) <= 0){
		scePrintf("pktsize = %d\n", pktsize);
		ERR_STOP;
	}


	/***************************************************************
		Preparation
	***************************************************************/
	sceSifInitRpc( 0 );

	LoadModule( IOP_MOD_INET, 0, NULL );
	LoadModule( IOP_MOD_NETCNF, sizeof( NETCNF_ARG ), NETCNF_ARG );
	LoadModule( IOP_MOD_INETCTL, sizeof( INETCTL_ARG ), INETCTL_ARG );

#if defined( USB_ETHERNET ) || defined( USB_ETHERNET_WITH_PPPOE )
	LoadModule( IOP_MOD_USBD, 0, NULL );
	LoadModule( IOP_MOD_AN986, 0, NULL );
#endif

#if defined( HDD_ETHERNET ) || defined( HDD_ETHERNET_WITH_PPPOE )
	LoadModule( IOP_MOD_DEV9, 0, NULL );
	LoadModule( IOP_MOD_SMAP, 0, NULL );
#endif

#if defined( USB_ETHERNET_WITH_PPPOE ) || defined( HDD_ETHERNET_WITH_PPPOE )
	LoadModule( IOP_MOD_PPP, 0, NULL );
	LoadModule( IOP_MOD_PPPOE, 0, NULL );
#endif

#if defined( MODEM )
	LoadModule( IOP_MOD_PPP, 0, NULL );
	LoadModule( IOP_MOD_USBD, 0, NULL );
	LoadModule( IOP_MOD_MODEMDRV, sizeof( MODEMDRV_ARG ), MODEMDRV_ARG );
#endif

	LoadModule( IOP_MOD_MSIFRPC, 0, NULL );
	LoadModule( IOP_MOD_LIBNET, 0, NULL );

#if defined( HDD_ETHERNET ) || defined( HDD_ETHERNET_WITH_PPPOE )
	PreparePowerOff();
#endif


	/***************************************************************
		Initialize libnet
	***************************************************************/
	sceSifMInitRpc(0);
	libnet_init(&cd, 2048, 8192, 32);
	if((net_buf = (u_int *)memalign(64, RPCSIZE)) == NULL){
		scePrintf("malloc failed.\n");
		ERR_STOP;
	}
	if((ret = reg_handler(&cd, net_buf)) < 0){
		scePrintf("reg_handler() failed.\n");
		ERR_STOP;
	}

#ifdef  UP_INTERFACE_AUTO
	scePrintf("up interface auto\n");
	if((ret = load_set_conf(&cd, net_buf, NET_DB, USR_CONF_NAME)) < 0){
		scePrintf("load_set_conf() failed.\n");
		ERR_STOP;
	}
	if((ret = wait_get_addr(&cd, net_buf, &if_id, &myaddr)) < 0){
		scePrintf("wait_get_addr() failed.\n");
		ERR_STOP;
	}
#else
	scePrintf("up interface no auto\n");
	if((ret = load_set_conf_only(&cd, net_buf, NET_DB, USR_CONF_NAME)) < 0){
		scePrintf("load_set_conf_only() failed.\n");
		ERR_STOP;
	}

	if((ret = get_interface_id(&cd, net_buf, &if_id)) < 0){
		scePrintf("get_interface_id()\n");
		ERR_STOP;
	}
	up_interface(&cd, net_buf, 0);

	if((ret = wait_get_addr_only(&cd, net_buf, &if_id, &myaddr)) < 0){
		scePrintf("wait_get_addr_only() failed.\n");
		ERR_STOP;
	}
#endif  /*** UP_INTERFACE_AUTO ***/


	/***************************************************************
		Display IP address
	***************************************************************/
	if((ret = sceInetAddress2String(&cd, net_buf, myptrname, 128, &myaddr)) != sceINETE_OK){
		scePrintf("invalid addr\n");
		ERR_STOP;
	}
	if((ret = sceInetAddress2Name(&cd, net_buf, 0, myname, 128, &myaddr, 0, 0)) != sceINETE_OK)
		scePrintf("My address: %s\n", myptrname);
	else
		scePrintf("My address: %s (%s)\n", myname, myptrname);


	/***************************************************************
		Main
	***************************************************************/
 	if((connection = (struct connection_data *)malloc(sizeof(struct connection_data) * th_num)) == NULL){
		scePrintf("malloc failed.\n");
		ERR_STOP;
	}

	sema_param.initCount = 0;
	sema_param.maxCount = 1;
	for(i = 0; i < th_num; i++)
		connection[i].sema = CreateSema(&sema_param);

	for(i = 0; i < th_num; i++){
		connection[i].fid = i;
		if((connection[i].stack = (char *)memalign(16, STACKSIZE)) == NULL){
			scePrintf("malloc failed.\n");
			ERR_STOP;
		}
		start_thread(thread_func, connection[i].stack, STACKSIZE, PRIO, &connection[i].fid);
	}

	for(i = 0; i < th_num; i++)
		WaitSema(connection[i].sema);

	down_interface(&cd, net_buf, if_id);


	/***************************************************************
		Terminate libnet
	***************************************************************/
	down_interface(&cd, net_buf, 0);
	if((ret = unreg_handler(&cd, net_buf)) < 0){
		scePrintf("unreg_handler() failed.\n");
		ERR_STOP;
	}
	free(net_buf);
	net_buf = NULL;
	libnet_term(&cd);

	return 0;
}

/*** End of file ***/

