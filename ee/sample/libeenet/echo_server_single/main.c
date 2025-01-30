/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/* 
 *        Emotion Engine (I/O Processor) Library Sample Program
 *
 *                - <libeenet echo_server_single> -
 *
 *                         Version 1.00
 *                           Shift-JIS
 *
 *      Copyright (C) 2002 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                            <main.c>
 *          <main function of libeenet echo_server_single>
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.00           Dec,13,2002     wparks      first version
 */

#if 1
#define HDD_ETHERNET
#endif
#if 0
#define HDD_ETHERNET_WITH_PPPOE
#endif
#if 0
#define USB_ETHERNET
#endif
#if 0
#define USB_ETHERNET_WITH_PPPOE
#endif
#if 0
#define MODEM
#endif

#if 1
#include <ent_dump.h>
#define TCPDUMP
#endif

#if 1
#define UP_INTERFACE_AUTO
#endif

#include <sys/types.h>
#include <eekernel.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <errno.h>
#include <sifrpc.h>
#include <sifdev.h>
#include "iopmodules.h"
#if defined(HDD_ETHERNET) || defined(HDD_ETHERNET_WITH_PPPOE)
#include "poweroff.h"
#endif

#include <libeenet.h>
#include <eenetctl.h>
#include "../ent_cnf/ent_cnf.h"
#include "../util/ifinfo/ifinfo.h"
#include <ifaddrs.h>
#include <sys/select.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

//defines for eenet 
#define EENET_MEMSIZE	512*1024	/* 512KB */
#define EENET_TPL		32
#define EENET_APP_PRIO	48

static int eenetctl_mid;
static int ent_cnf_mid;
static int ent_devm_mid;
#if defined(HDD_ETHERNET) || defined(HDD_ETHERNET_WITH_PPPOE)
#define EENET_IFNAME "smap0"
static int ent_smap_mid;
#endif
#if defined(USB_ETHERNET) || defined(USB_ETHERNET_WITH_PPPOE)
#define EENET_IFNAME "eth0"
static int ent_eth_mid;
#endif
#if defined(MODEM)
#define EENET_IFNAME "ppp0"
static int ent_ppp_mid;
static int modem_mid;
#endif

#define ERR_STOP while(1)
#define BUFSIZE 1460

static int sema_id;
static int event_flag = 0;
#define Ev_Attach         0x01
#define Ev_UpCompleted    0x02
#define Ev_DownCompleted  0x04
#define Ev_DetachCompleted 0x08

#define WaitEvent(event) \
	while (1) { \
		WaitSema(sema_id); \
		if(event_flag & (event)) \
			break; \
	} \

#define SERVER_PORT (7)

typedef struct fd_list {
	struct fd_list *next;
	int sid;
} FD_LIST;

static FD_LIST *fd_list = NULL;

static void event_handler(const char *ifname, int af, int type)
{

	scePrintf("event_handler: event happened. af = %d, type = %d\n",
		af, type);

	switch (type) {
	case sceEENETCTL_IEV_Attach:
		if (strcmp(ifname, EENET_IFNAME))
			break;
		event_flag |= Ev_Attach;
		SignalSema(sema_id);
		break;
	case sceEENETCTL_IEV_UpCompleted:
		event_flag |= Ev_UpCompleted;
		SignalSema(sema_id);
		break;
	case sceEENETCTL_IEV_DownCompleted:
		event_flag |= Ev_DownCompleted;
		SignalSema(sema_id);
		break;
	case sceEENETCTL_IEV_DetachCompleted:
		if (strcmp(ifname, EENET_IFNAME))
			break;
		event_flag |= Ev_DetachCompleted;
		SignalSema(sema_id);
		break;
	}

	return;
}

static int create_sema(int init_count, int max_count)
{
	struct SemaParam sema_param;
	int sid;

	memset(&sema_param, 0, sizeof(struct SemaParam));
	sema_param.initCount = init_count;
	sema_param.maxCount = max_count;
	sid = CreateSema(&sema_param);

	return sid;
}

static void add_node_to_list(int n)
{
	FD_LIST *tmp;

	if ((tmp = (FD_LIST *)memalign(16, sizeof(*tmp))) == NULL) {
		scePrintf("memalign failed.\n");
		ERR_STOP;
	}
	tmp->sid = n;
	tmp->next = fd_list;
	fd_list = tmp;
}

static void remove_node_from_list(int n)
{
	FD_LIST *tmp = fd_list;
	FD_LIST *prev = NULL;

	while (tmp != NULL) {
		if (tmp->sid == n) {
			if (prev != NULL) {
				prev->next = tmp->next;
			} else {
				fd_list = tmp->next;
			}
			free(tmp);
			return;
		}
		prev = tmp;
		tmp = tmp->next;
	}
}

static void processSocketData(FD_LIST *tmp_list, int s, fd_set *fds, fd_set *fds0)
{
	int n;
	int r;
	struct sockaddr_in sock_in;
	int ns;
	int len;
	char buf[BUFSIZE];
	int tmps;

	memset(&sock_in, 0, sizeof(sock_in));
	len = sizeof(sock_in);

	/* goes through list of sockets to determine which was have
	   activity.  if so, handle the data appropriately */
	while (tmp_list != NULL) {
		if (FD_ISSET(tmp_list->sid, fds)) {
			/* if it is the server socket... */
			if (tmp_list->sid == s) {
				if ((ns = accept(s, (struct sockaddr *)&sock_in, &len)) < 0) {
					scePrintf("accept failed. (%d)", sceEENetErrno);
					ERR_STOP;
				}
				scePrintf("connected from %s:%d\n",
					  inet_ntoa(sock_in.sin_addr), ntohs(sock_in.sin_port));
				/* add echo socket */
				add_node_to_list(ns);
				FD_SET(ns, fds0);

			/* not the server socket, so must be a client
			   connection */
			} else {
				n = recv(tmp_list->sid, buf, sizeof(buf), 0);
				if (n == 0) {
					scePrintf("Connection closed.\n");
					tmps = tmp_list->sid;
					tmp_list = tmp_list->next;
					FD_CLR(tmps, fds0);
					remove_node_from_list(tmps);
					sceEENetClose(tmps);
					continue;
				}
				if (n < 0) {
					scePrintf("recv failed. (%d)\n", sceEENetErrno);
					ERR_STOP;
				}
				if (n != (r = send(tmp_list->sid, buf, n, 0))) {
					scePrintf("send %d->%d\n", n, r);
					if (r < 0) {
						scePrintf("send failed. (%d)\n", sceEENetErrno);
						ERR_STOP;
					}
				}
			}
		}
		tmp_list = tmp_list->next;
	}

}

static void echo_server(void)
{
	int ret;
	struct sockaddr_in sock_in;
	int s;
	fd_set fds;
	fd_set fds0;
	struct timeval tv;
	FD_LIST *tmp_list;

	/* Lower priority level so that eenetctl can catch up. */
	ChangeThreadPriority(GetThreadId(), 120);

	/* displays information on the network device in use */
	/* this function comes from ../util/ifinfo/ */
	sceEENetShowAllIfinfo();

	if (0 > (s = socket(AF_INET, SOCK_STREAM, 0))) {
		scePrintf("socket failed (%d)", sceEENetErrno);
		ERR_STOP;
	}
	
	memset(&sock_in, 0, sizeof(sock_in));
	sock_in.sin_family = AF_INET;
	sock_in.sin_port = htons(SERVER_PORT);

	if (0 > bind(s, (struct sockaddr *)&sock_in, sizeof(sock_in))) {
		scePrintf("bind failed. (%d)\n", sceEENetErrno);
		shutdown(s, SHUT_RDWR);
		ERR_STOP;
	}
	if (0 > listen(s, 5)) {
		scePrintf("listen failed (%d)\n", sceEENetErrno);
		shutdown(s, SHUT_RDWR);
		ERR_STOP;
	}

	scePrintf("starting echo server on port %d\n", SERVER_PORT);

	FD_ZERO(&fds0);
	FD_SET(s, &fds0);
	add_node_to_list(s);
	for ( ; ; ) {
		/* must be updated after each iteration */
		fds = fds0;
		tmp_list = fd_list;
		tv.tv_sec = 0;  /* seconds */
		tv.tv_usec = 0; /* microseconds */

		ret = select(FD_SETSIZE, &fds, (fd_set *)NULL, (fd_set *)NULL, &tv);
		if (ret == 0) {
			/* processing */
			continue;
		}
		if (ret < 0) {
			scePrintf("select failed. (%d)\n", sceEENetErrno);
			sceEENetClose(s);
			ERR_STOP;
		}
		/* go thru the list of sockets that activity is reported
		   on and process the data has necessary. */
		processSocketData(tmp_list, s, &fds, &fds0);

	}

	shutdown(s, SHUT_RDWR);
	
	ChangeThreadPriority(GetThreadId(), 1);
}

int	main(int argc, char **argv)
{
	void *libeenet_pool;
	int ret;

#ifdef TCPDUMP
	char stackbuf[8192] __attribute__((aligned(64)));
	char *filebuf;
	sceEENetTcpdumpParam_t daemonp;
#endif

	if ((sema_id = create_sema(0, 255)) < 0) {
		scePrintf("create_sema() failed.\n");
		ERR_STOP;
	}

	/************************************************************
	 * initialization
	 ************************************************************/

	/* init sif stuff */
	sceSifInitRpc(0);

	/* load iop modules */
	while ((ent_devm_mid = sceSifLoadModule(IOP_MOD_ENT_DEVM, 0, NULL)) < 0) {
		scePrintf("ent_devm.irx cannot be loaded.\n");
	}
	while (sceSifLoadModule(IOP_MOD_NETCNF, sizeof(NETCNF_ARG), NETCNF_ARG) < 0) {
		scePrintf("netcnf.irx cannot be loaded.\n");
	}
	while ((eenetctl_mid = sceSifLoadModule(IOP_MOD_EENETCTL, 0, NULL)) < 0) {
		scePrintf("eenetctl.irx cannot be loaded.\n");
	}
	while ((ent_cnf_mid = sceSifLoadModule(IOP_MOD_ENT_CNF, 0, NULL)) < 0) {
		scePrintf("ent_cnf.irx cannot be loaded.\n");
	}
#if defined( HDD_ETHERNET ) || defined( HDD_ETHERNET_WITH_PPPOE )
	while (sceSifLoadModule(IOP_MOD_DEV9, 0, NULL) < 0) {
		scePrintf("dev9.irx cannot be loaded.\n");
	}
	while ((ent_smap_mid = sceSifLoadModule(IOP_MOD_ENT_SMAP, 0, NULL)) < 0) {
		scePrintf("ent_smap.irx cannot be loaded.\n");
	}
	PreparePowerOff();
#endif
#if defined( USB_ETHERNET ) || defined( USB_ETHERNET_WITH_PPPOE )
	while (sceSifLoadModule(IOP_MOD_USBD, 0, NULL) < 0) {
		scePrintf("usbd.irx cannot be loaded.\n");
	}
	while ((ent_eth_mid = sceSifLoadModule(IOP_MOD_ENT_ETH, 0, NULL)) < 0) {
		scePrintf("ent_eth.irx cannot be loaded.\n");
	}
#endif
#if defined( MODEM )
	while ((ent_ppp_mid = sceSifLoadModule(IOP_MOD_ENT_PPP, 0, NULL)) < 0) {
		scePrintf("ent_ppp.irx cannot be loaded.\n");
	}
	while ((modem_mid = sceSifLoadModule(IOP_MOD_MODEMDRV, sizeof(MODEMDRV_ARG), MODEMDRV_ARG)) < 0) {
		scePrintf("%s cannot be loaded.\n", IOP_MOD_MODEMDRV);
	}
#endif

	/* initialize eenet library */
	if ((libeenet_pool = memalign(64, EENET_MEMSIZE)) == NULL) {
		scePrintf("memalign() failed.\n");
		ERR_STOP;
	}

	if ((ret = sceEENetInit(libeenet_pool, EENET_MEMSIZE, EENET_TPL, 8192, EENET_APP_PRIO)) < 0) {
		scePrintf("sceEENetInit() failed.\n");
		ERR_STOP;
	}

#ifdef DEBUG
	sceEENetSetLogLevel(EENET_LOG_DEBUG);
#endif

#ifdef UP_INTERFACE_AUTO
	if ((ret = sceEENetCtlInit(8192, EENET_APP_PRIO, 8192, EENET_APP_PRIO, 8192, EENET_APP_PRIO, 1, 0)) < 0) {
#else
	if ((ret = sceEENetCtlInit(8192, EENET_APP_PRIO, 8192, EENET_APP_PRIO, 8192, EENET_APP_PRIO, 1, 1)) < 0) {
#endif
		scePrintf("sceEENetCtlInit() failed.\n");
		ERR_STOP;
	}
	if ((ret = sceEENetCtlRegisterEventHandler(event_handler)) < 0) {
		scePrintf("sceEENetCtlRegisterEventHandler() failed.\n");
		ERR_STOP;
	}

	if ((ret = ent_cnf_init()) < 0) {
		scePrintf("ent_cnf_init() failed.\n");
		ERR_STOP;
	}

	/* register device to eenet */
#if defined( HDD_ETHERNET ) || defined( HDD_ETHERNET_WITH_PPPOE )
	if ((ret = sceEENetDeviceSMAPReg(8192, 8192)) < 0) {
		scePrintf("ret of sceEENetDeviceSMAPReg() = %d\n", ret);
		ERR_STOP;
	}
#endif
#if defined( USB_ETHERNET ) || defined( USB_ETHERNET_WITH_PPPOE )
	if ((ret = sceEENetDeviceETHReg(8192, 8192)) < 0) {
		scePrintf("ret of sceEENetDeviceETHReg() = %d\n", ret);
		ERR_STOP;
	}
#endif
#if defined( MODEM )
	if ((ret = sceEENetDevicePPPReg(8192, 8192)) < 0) {
		scePrintf("ret of sceEENetDevicePPPReg() = %d\n", ret);
		ERR_STOP;
	}
#endif

	/* wait until target interface is attached. */
	WaitEvent(Ev_Attach);

#ifdef TCPDUMP
#if 1   /* use buffering */
	memset(&daemonp, 0, sizeof(sceEENetTcpdumpParam_t));
	filebuf = malloc(128*1024);
	if ((ret = sceEENetTcpdump(EENET_IFNAME, "host0:./tcpdump.log", filebuf, 128*1024, 1, NULL, 0, 0, stackbuf, 8192, EENET_APP_PRIO, &daemonp)) < 0) {
#else
	if ((ret = sceEENetTcpdump(EENET_IFNAME, "host0:./tcpdump.log", NULL, 0, 1, NULL, 0, 0, stackbuf, 8192, EENET_APP_PRIO, &daemonp)) < 0) {
#endif
		scePrintf("sceEENetTcpdump() failed.\n");
	}
#endif

	if ((ret = ent_cnf_load_config(NET_DB, USR_CONF_NAME)) < 0) {
		scePrintf("ent_cnf_load_config() failed.\n");
		ERR_STOP;
	}
	if ((ret = ent_cnf_set_config()) < 0) {
		scePrintf("ent_cnf_set_config() failed.\n");
		ERR_STOP;
	}

#ifndef UP_INTERFACE_AUTO
	/* up interface manually */
	if ((ret = sceEENetCtlUpInterface(EENET_IFNAME)) < 0) {
		scePrintf("sceEENetCtlUpInterface() failed.\n");
		ERR_STOP;
	}
#endif
	/* wait for interface initialization completed */
	WaitEvent(Ev_UpCompleted);

	/************************************************************
	 * main function
	 ************************************************************/

	scePrintf("starting the server.\n");

	echo_server();

	scePrintf("done.\n");

	sceEENetFreeThreadinfo(GetThreadId());

	/************************************************************
	 * termination
	 ************************************************************/

	/* down interface */
	if ((ret = sceEENetCtlDownInterface(EENET_IFNAME)) < 0) {
		scePrintf("sceEENetCtlDownInterface() failed.\n");
		ERR_STOP;
	}
	WaitEvent(Ev_DownCompleted);

#ifdef TCPDUMP
	if (daemonp.tid > 0) {
		sceEENetTcpdumpTerm(&daemonp);
	}
	free(filebuf);
#endif

#if defined( HDD_ETHERNET ) || defined( HDD_ETHERNET_WITH_PPPOE )
	if ((ret = sceEENetDeviceSMAPUnreg()) < 0) {
		scePrintf("sceEENetDeviceSMAPUnreg() failed.\n");
		ERR_STOP;
	}
#endif
#if defined( USB_ETHERNET ) || defined( USB_ETHERNET_WITH_PPPOE )
	if ((ret = sceEENetDeviceETHUnreg()) < 0) {
		scePrintf("sceEENetDeviceETHUnreg() failed.\n");
		ERR_STOP;
	}
#endif
#if defined( MODEM )
	if ((ret = sceEENetDevicePPPUnreg()) < 0) {
		scePrintf("sceEENetDevicePPPUnreg() failed.\n");
		ERR_STOP;
	}
#endif
	WaitEvent(Ev_DetachCompleted);
	if ((ret = sceEENetCtlUnregisterEventHandler(event_handler)) < 0) {
		scePrintf("sceEENetCtlUnRegisterEventHandler() failed.\n");
		ERR_STOP;
	}
	if ((ret = sceEENetCtlTerm()) < 0) {
		scePrintf("sceEENetCtlTerm() failed.\n");
		ERR_STOP;
	}
	if ((ret = sceEENetTerm()) < 0) {
		scePrintf("sceEENetTerm() failed.\n");
		ERR_STOP;
	}
	free(libeenet_pool);

#if defined( HDD_ETHERNET ) || defined( HDD_ETHERNET_WITH_PPPOE )
	sceSifStopModule(ent_smap_mid, 0, NULL, &ret);
	sceSifUnloadModule(ent_smap_mid);
#endif
#if defined( USB_ETHERNET ) || defined( USB_ETHERNET_WITH_PPPOE )
	sceSifStopModule(ent_eth_mid, 0, NULL, &ret);
	sceSifUnloadModule(ent_eth_mid);
#endif
#if defined( MODEM )
	sceSifStopModule(modem_mid, 0, NULL, &ret);
	sceSifUnloadModule(modem_mid);

	sceSifStopModule(ent_ppp_mid, 0, NULL, &ret);
	sceSifUnloadModule(ent_ppp_mid);
#endif

	sceSifStopModule(ent_cnf_mid, 0, NULL, &ret);
	sceSifUnloadModule(ent_cnf_mid);

	sceSifStopModule(eenetctl_mid, 0, NULL, &ret);
	sceSifUnloadModule(eenetctl_mid);

	sceSifStopModule(ent_devm_mid, 0, NULL, &ret);
	sceSifUnloadModule(ent_devm_mid);

	DeleteSema(sema_id);

	return 0;
}
