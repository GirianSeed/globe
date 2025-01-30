/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5.5
 */
/* 
 *        Emotion Engine (I/O Processor) Library Sample Program
 *
 *                     - <libeenet http_get2> -
 *
 *                         Version 1.00
 *                           Shift-JIS
 *
 *      Copyright (C) 2001 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                            <http.c>
 *              <main function of libeenet http_get2>
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.00           Apr,23,2002     komaki      first version
 */

#if 1
#define HDD_ETHERNET
#endif
#if 0
#define HDD_ETHERNET_WITH_PPPOE
#endif
#if 0
#define HDD_ETHERNET_INET6
#endif

#if 1
#define TCPDUMP
#endif

#if 0
#define UP_INTERFACE_AUTO
#endif

#include <sys/types.h>
#include <eekernel.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sifrpc.h>
#include <sifcmd.h>
#include <sifdev.h>
#include <iopmodules.h>
#if defined(HDD_ETHERNET) || defined(HDD_ETHERNET_WITH_PPPOE) || defined(HDD_ETHERNET_INET6)
#include <poweroff.h>
#endif

#include <libeenet.h>
#include <eenetctl.h>
#include <ent_smap.h>
#include <ent_cnf.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

//defines for eenet 
#define EENET_MEMSIZE	512*1024	/* 512KB */
#define SIFCMD_NUM 2
#define ENT_SMAP_NAME "smap0"

static sceSifCmdData cmddata[SIFCMD_NUM];
static int eenetctl_mid;
static int ent_cnf_mid;
static int ent_devm_mid;
static int ent_smap_mid;

#define ERR_STOP while(1)
#define BUFSIZE 1460
#define REQUEST_GET1 "GET / HTTP/1.1\r\nHOST: "
#define REQUEST_GET2 "\r\n\r\n"

static int sema_id;
static int event_flag = 0;
#define Ev_Attach         0x01
#define Ev_UpCompleted    0x02
#define Ev_DownCompleted  0x04

#define WaitEvent(event) \
	while(1){ \
		WaitSema(sema_id); \
		if(event_flag & (event)) \
			break; \
	} \

static void event_handler(const char *ifname, int af, int type){

	scePrintf("event_handler: event happened. af = %d, type = %d\n",
		af, type);

	switch(type){
	case sceEENETCTL_IEV_Attach:
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
	}

	return;
}

static int create_sema(int init_count, int max_count){
	struct SemaParam sema_param;
	int sema_id;

	memset(&sema_param, 0, sizeof(struct SemaParam));
	sema_param.initCount = init_count;
	sema_param.maxCount = max_count;
	sema_id = CreateSema(&sema_param);

	return sema_id;
}

int	main(int argc, char **argv){
	int  tcp_len, ret, fd, total_len = 0;
	char get_cmd[128];
	char buffer0[BUFSIZE];
	char server_name[128];
	int s;
	struct addrinfo hints, *res, *res0;
	const char *cause = NULL;
#ifdef TCPDUMP
	char stackbuf[8192] __attribute__((aligned(64)));
	char *filebuf;
	sceEENetTcpdumpParam_t daemonp;
#endif

	if(argc != 2){
		scePrintf("usage: %s <URL>\n", argv[0]);
		ERR_STOP;
	}
	strcpy(server_name, argv[1]);

	if((sema_id = create_sema(0, 1)) < 0){
		scePrintf("create_sema() failed.\n");
		ERR_STOP;
	}

	/************************************************************
	 * initialization
	 ************************************************************/

	/* init sif stuff */
	sceSifInitRpc(0);

	/* initialize eenet library */
	if((ret = sceEENetInit(EENET_MEMSIZE, 1, 0, 8192, 32)) < 0){
		scePrintf("sceEENetInit() failed.\n");
		ERR_STOP;
	}

	while(sceSifLoadModule(IOP_MOD_NETCNF, sizeof(NETCNF_ARG), NETCNF_ARG) < 0)
		scePrintf("netcnf.irx cannot be loaded.\n");
#ifdef UP_INTERFACE_AUTO
	if((ret = sceEENetCtlInit(8192, 32, 8192, 32, 8192, 32, 1, 0)) < 0){
#else
	if((ret = sceEENetCtlInit(8192, 32, 8192, 32, 8192, 32, 1, 1)) < 0){
#endif
		scePrintf("sceEENetCtlInit() failed.\n");
		ERR_STOP;
	}
	while((eenetctl_mid = sceSifLoadModule(IOP_MOD_EENETCTL, 0, NULL)) < 0)
		scePrintf("eenetctl.irx cannot be loaded.\n");
	if((ret = sceEENetCtlRegisterEventHandler(event_handler)) < 0){
		scePrintf("sceEENetCtlRegisterEventHandler() failed.\n");
		ERR_STOP;
	}

	while((ent_cnf_mid = sceSifLoadModule(IOP_MOD_ENT_CNF, 0, NULL)) < 0)
		scePrintf("ent_cnf.irx cannot be loaded.\n");
	if((ret = ent_cnf_init()) < 0){
		scePrintf("ent_cnf_init() failed.\n");
		ERR_STOP;
	}

	while((ent_devm_mid = sceSifLoadModule(IOP_MOD_ENT_DEVM, 0, NULL)) < 0)
		scePrintf("ent_devm.irx cannot be loaded.\n");

	/* register ee sifcmd data */
	DI();
	sceSifSetCmdBuffer(cmddata, SIFCMD_NUM);
	EI();

	/* allocate and register iop sifcmd data */
	if((ret = ent_cnf_set_sifcmdbuffer(SIFCMD_NUM)) < 0){
		scePrintf("ent_cnf_set_sifcmdbuffer() failed.\n");
		ERR_STOP;
	}

	/* register device to eenet */
	if((ret = sceEENetDeviceSMAPReg(8192, 8192)) < 0){
		scePrintf("ret of sceEENetDeviceSMAPReg() = %d\n", ret);
		ERR_STOP;
	}

	/* load iop modules */
	while(sceSifLoadModule(IOP_MOD_DEV9, 0, NULL) < 0)
		scePrintf("dev9.irx cannot be loaded.\n");
	while((ent_smap_mid = sceSifLoadModule(IOP_MOD_ENT_SMAP, 0, NULL)) < 0)
		scePrintf("ent_smap.irx cannot be loaded.\n");
#if defined( HDD_ETHERNET ) || defined( HDD_ETHERNET_WITH_PPPOE ) || defined( HDD_ETHERNET_INET6 )
	PreparePowerOff();
#endif

	/* wait until target interface is attached. */
	WaitEvent(Ev_Attach);

#ifdef TCPDUMP
#if 1   /* use buffering */
	memset(&daemonp, 0, sizeof(sceEENetTcpdumpParam_t));
	filebuf = malloc(128*1024);
	if((ret = sceEENetTcpdump(ENT_SMAP_NAME, "host0:./tcpdump.log", filebuf, 128*1024, 1, NULL, 0, 0, stackbuf, 8192, 8, &daemonp)) < 0){
#else
	if((ret = sceEENetTcpdump(ent_smap_NAME, "host0:./tcpdump.log", NULL, 0, 1, NULL, 0, 0, stackbuf, 8192, 8, &daemonp)) < 0){
#endif
		scePrintf("sceEENetTcpdump() failed.\n");
	}
#endif

	if((ret = ent_cnf_load_config(NET_DB, USR_CONF_NAME)) < 0){
		scePrintf("ent_cnf_load_config() failed.\n");
		ERR_STOP;
	}
	if((ret = ent_cnf_set_config()) < 0){
		scePrintf("ent_cnf_set_config() failed.\n");
		ERR_STOP;
	}

#ifndef UP_INTERFACE_AUTO
	/* up interface manually */
	if((ret = sceEENetCtlUpInterface(ENT_SMAP_NAME)) < 0){
		scePrintf("sceEENetCtlUpInterface() failed.\n");
		ERR_STOP;
	}
#endif
	/* wait for interface initialization completed */
	WaitEvent(Ev_UpCompleted);

	/************************************************************
	 * main function
	 ************************************************************/

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = PF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	ret = getaddrinfo(server_name, "80", &hints, &res0);
	if (ret) {
		scePrintf("getaddrinfo() failed.\n");
		goto termination;
	}
	s = -1;
	for (res = res0; res; res = res->ai_next) {
		s = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if (s < 0) {
			cause = "socket()";
			continue;
		}

		if (connect(s, res->ai_addr, res->ai_addrlen) < 0) {
			cause = "connect()";
			sceEENetClose(s);
			s = -1;
			continue;
		}

		break;  /* okay we got one */
	}
	if (s < 0) {
		scePrintf("%s failed (errno=%d)\n", cause, sceEENetErrno);
		freeaddrinfo(res0);
		goto termination;
	}
	freeaddrinfo(res0);

	strcpy(get_cmd, REQUEST_GET1);
	strcat(get_cmd, server_name);
	strcat(get_cmd, REQUEST_GET2);

	tcp_len = strlen(get_cmd);
	if(tcp_len != (ret = send(s, get_cmd, tcp_len, 0))){
		scePrintf("send() failed %d->%d (errno=%d)\n",
			tcp_len, ret, sceEENetErrno);
		goto termination;
	}
	if(0 > (fd = sceOpen("host0:index.html",
			SCE_RDWR | SCE_CREAT | SCE_TRUNC))){
		scePrintf("failed to open index.html\n");
		goto termination;
	}
	while(0 < (tcp_len = recv(s, buffer0, sizeof(buffer0), 0))){
		sceWrite(fd, buffer0, tcp_len);
		total_len += tcp_len;
		scePrintf("tcp_len = %d, total_len = %d\n", tcp_len, total_len);
	} 
	sceClose(fd);
	sceEENetClose(s);

	scePrintf("done.\n");

	/************************************************************
	 * termination
	 ************************************************************/

termination:
#ifdef TCPDUMP
	if (daemonp.tid > 0)
		sceEENetTcpdumpTerm(&daemonp);
	free(filebuf);
#endif

	/* down interface */
	if((ret = sceEENetCtlDownInterface(ENT_SMAP_NAME)) < 0){
		scePrintf("sceEENetCtlDownInterface() failed.\n");
		ERR_STOP;
	}
	WaitEvent(Ev_DownCompleted);

	sceSifStopModule(ent_smap_mid, 0, NULL, &ret);
	sceSifUnloadModule(ent_smap_mid);

	if((ret = sceEENetDeviceSMAPUnreg()) < 0) {
		scePrintf("sceEENetDeviceSMAPUnreg() failed.\n");
		ERR_STOP;
	}

	sceSifStopModule(ent_devm_mid, 0, NULL, &ret);
	sceSifUnloadModule(ent_devm_mid);

	sceSifStopModule(ent_cnf_mid, 0, NULL, &ret);
	sceSifUnloadModule(ent_cnf_mid);

	if((ret = sceEENetCtlUnregisterEventHandler(event_handler)) < 0){
		scePrintf("sceEENetCtlUnRegisterEventHandler() failed.\n");
		ERR_STOP;
	}

	sceSifStopModule(eenetctl_mid, 0, NULL, &ret);
	sceSifUnloadModule(eenetctl_mid);

	if((ret = sceEENetCtlTerm()) < 0) {
		scePrintf("sceEENetCtlTerm() failed.\n");
		ERR_STOP;
	}

	if((ret = sceEENetTerm()) < 0) {
		scePrintf("sceEENetTerm() failed.\n");
		ERR_STOP;
	}

	return 0;
}
