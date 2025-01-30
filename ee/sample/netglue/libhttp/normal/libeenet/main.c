/* SCE CONFIDENTIAL
"PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/*
 *              Emotion Engine Library Sample Program
 *
 *                         - http_test -
 *                        Version 1.0.0
 *
 *      Copyright (C) 2001-2002 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                          <main.c>
 *                       <main functions>
 *
 *       Version   Date           Design              Log
 *  --------------------------------------------------------------------
 *       1.0.0     Sep,19,2002    komaki              first version
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
#if defined(USE_ERX)
#include <eemodules.h>
#include <liberx.h>
#include <libgcc_export.h>
#include <libc_export.h>
#endif 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <errno.h>
#include <sifrpc.h>
#include <sifdev.h>
#include <iopmodules.h>
#if defined(HDD_ETHERNET) || defined(HDD_ETHERNET_WITH_PPPOE)
#include <poweroff.h>
#endif

#include <libeenet.h>
#include <eenetctl.h>
#include <ent_cnf.h>

#include <http_test.h>

#if defined(USE_ERX)
#define LIBERX_THREAD_PRIORITY      10
#define LIBERX_MEMORY_SIZE          (8*1024*1024)
#define LIBERX_MAX_MODULES          32

static int libcdvd_erxid, libscf_erxid, ntg_ent_erxid, libhttp_erxid;
static int base64_erxid, libqp_erxid;
static int libeenet_erxid;
static int eenetctl_erxid;
static int ent_cnf_erxid;
#ifdef TCPDUMP
static int ent_dump_erxid;
#endif
#endif

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
#if defined(USE_ERX)
static int ent_smap_erxid;
#endif
#endif
#if defined(USB_ETHERNET) || defined(USB_ETHERNET_WITH_PPPOE)
#define EENET_IFNAME "eth0"
static int ent_eth_mid;
#if defined(USE_ERX)
static int ent_eth_erxid;
#endif
#endif
#if defined(MODEM)
#define EENET_IFNAME "ppp0"
static int ent_ppp_mid;
#if defined(USE_ERX)
static int ent_ppp_erxid;
#endif
static int modem_mid;
#endif

#define ERR_STOP while(1)

static int sema_id;
static int event_flag = 0;
#define Ev_Attach         0x01
#define Ev_UpCompleted    0x02
#define Ev_DownCompleted  0x04
#define Ev_DetachCompleted 0x08

#define WaitEvent(event) \
	while(1){ \
		WaitSema(sema_id); \
		if(event_flag & (event)) \
			break; \
	} \

#if defined(USE_ERX)
int InitErxEnvironment(void *pool, int pool_size);
#endif

static void event_handler(const char *ifname, int af, int type){

	scePrintf("event_handler: event happened. af = %d, type = %d\n",
		af, type);

	switch(type){
	case sceEENETCTL_IEV_Attach:
		if(strcmp(ifname, EENET_IFNAME))
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
		if(strcmp(ifname, EENET_IFNAME))
			break;
		event_flag |= Ev_DetachCompleted;
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
#if defined(USE_ERX)
	void *liberx_pool;
#endif
	void *libeenet_pool;
	int  ret;
#ifdef TCPDUMP
	char stackbuf[8192] __attribute__((aligned(64)));
	char *filebuf = NULL;
	sceEENetTcpdumpParam_t daemonp;
#endif

#if defined(USE_ERX)
	/* init erx library */
	if((liberx_pool = memalign(256, LIBERX_MEMORY_SIZE)) == NULL){
		scePrintf("memalign() failed.\n");
		ERR_STOP;
	}
	if(InitErxEnvironment(liberx_pool, LIBERX_MEMORY_SIZE) < 0){
		scePrintf("InitErxEnvironment() failed.\n");
		ERR_STOP;
	}

	/* load libcdvd/libscf/base64/libqp/libhttp erx */
	while((libcdvd_erxid = sceErxLoadStartModuleFile(EE_MOD_LIBCDVD, 0, NULL, 0, NULL, &ret)) < 0)
		scePrintf("libcdvd.erx cannot be loaded.\n");
	while((libscf_erxid = sceErxLoadStartModuleFile(EE_MOD_LIBSCF, 0, NULL, 0, NULL, &ret)) < 0)
		scePrintf("libscf.erx cannot be loaded.\n");
	while((libeenet_erxid = sceErxLoadStartModuleFile(EE_MOD_LIBEENET, 0, NULL, 0, NULL, &ret)) < 0)
		scePrintf("libeenet.erx cannot be loaded.\n");
	while((ntg_ent_erxid = sceErxLoadStartModuleFile(EE_MOD_NTG_ENT, 0, NULL, 0, NULL, &ret)) < 0)
		scePrintf("ntg_ent.erx cannot be loaded.\n");
	while((base64_erxid = sceErxLoadStartModuleFile(EE_MOD_BASE64, 0, NULL, 0, NULL, &ret)) < 0)
		scePrintf("base64.erx cannot be loaded.\n");
	while((libqp_erxid = sceErxLoadStartModuleFile(EE_MOD_LIBQP, 0, NULL, 0, NULL, &ret)) < 0)
		scePrintf("libqp.erx cannot be loaded.\n");
	while((libhttp_erxid = sceErxLoadStartModuleFile(EE_MOD_LIBHTTP, 0, NULL, 0, NULL, &ret)) < 0)
		scePrintf("libhttp.erx cannot be loaded.\n");
#endif

	if((ret = http_test_init(argc, argv)) < 0){
		scePrintf("http_test_init() failed.\n");
		ERR_STOP;
	}

	if((sema_id = create_sema(0, 255)) < 0){
		scePrintf("create_sema() failed.\n");
		ERR_STOP;
	}

	/************************************************************
	 * initialization
	 ************************************************************/

	/* init sif stuff */
	sceSifInitRpc(0);

	/* load iop modules */
	while((ent_devm_mid = sceSifLoadModule(IOP_MOD_ENT_DEVM, 0, NULL)) < 0)
		scePrintf("ent_devm.irx cannot be loaded.\n");
	while(sceSifLoadModule(IOP_MOD_NETCNF, sizeof(NETCNF_ARG), NETCNF_ARG) < 0)
		scePrintf("netcnf.irx cannot be loaded.\n");
	while((eenetctl_mid = sceSifLoadModule(IOP_MOD_EENETCTL, 0, NULL)) < 0)
		scePrintf("eenetctl.irx cannot be loaded.\n");
	while((ent_cnf_mid = sceSifLoadModule(IOP_MOD_ENT_CNF, 0, NULL)) < 0)
		scePrintf("ent_cnf.irx cannot be loaded.\n");
#if defined( HDD_ETHERNET ) || defined( HDD_ETHERNET_WITH_PPPOE )
	while(sceSifLoadModule(IOP_MOD_DEV9, 0, NULL) < 0)
		scePrintf("dev9.irx cannot be loaded.\n");
	while((ent_smap_mid = sceSifLoadModule(IOP_MOD_ENT_SMAP, 0, NULL)) < 0)
		scePrintf("ent_smap.irx cannot be loaded.\n");
	PreparePowerOff();
#endif
#if defined( USB_ETHERNET ) || defined( USB_ETHERNET_WITH_PPPOE )
	while(sceSifLoadModule(IOP_MOD_USBD, 0, NULL) < 0)
		scePrintf("usbd.irx cannot be loaded.\n");
	while((ent_eth_mid = sceSifLoadModule(IOP_MOD_ENT_ETH, 0, NULL)) < 0)
		scePrintf("ent_eth.irx cannot be loaded.\n");
#endif
#if defined( MODEM )
	while((ent_ppp_mid = sceSifLoadModule(IOP_MOD_ENT_PPP, 0, NULL)) < 0)
		scePrintf("ent_ppp.irx cannot be loaded.\n");
	while((modem_mid = sceSifLoadModule(IOP_MOD_MODEMDRV, sizeof(MODEMDRV_ARG), MODEMDRV_ARG)) < 0)
		scePrintf("%s cannot be loaded.\n", IOP_MOD_MODEMDRV);
#endif

#if defined(USE_ERX)
	/* load eenetctl/ent_cnf erx */
	while((eenetctl_erxid = sceErxLoadStartModuleFile(EE_MOD_EENETCTL, 0, NULL, 0, NULL, &ret)) < 0)
		scePrintf("eenetctl.erx cannot be loaded.\n");
	while((ent_cnf_erxid = sceErxLoadStartModuleFile(EE_MOD_ENT_CNF, 0, NULL, 0, NULL, &ret)) < 0)
		scePrintf("ent_cnf.erx cannot be loaded.\n");
#ifdef TCPDUMP
	while((ent_dump_erxid = sceErxLoadStartModuleFile(EE_MOD_ENT_DUMP, 0, NULL, 0, NULL, &ret)) < 0)
		scePrintf("ent_dump.erx cannot be loaded.\n");
#endif
#endif

	/* initialize eenet library */
	if((libeenet_pool = memalign(64, EENET_MEMSIZE)) == NULL){
		scePrintf("memalign() failed.\n");
		ERR_STOP;
	}
	if((ret = sceEENetInit(libeenet_pool, EENET_MEMSIZE, EENET_TPL, 8192, EENET_APP_PRIO)) < 0){
		scePrintf("sceEENetInit() failed.\n");
		ERR_STOP;
	}

#ifdef DEBUG
	sceEENetSetLogLevel(EENET_LOG_DEBUG);
#endif

#ifdef UP_INTERFACE_AUTO
	if((ret = sceEENetCtlInit(8192, EENET_APP_PRIO, 8192, EENET_APP_PRIO, 8192, EENET_APP_PRIO, 1, 0)) < 0){
#else
	if((ret = sceEENetCtlInit(8192, EENET_APP_PRIO, 8192, EENET_APP_PRIO, 8192, EENET_APP_PRIO, 1, 1)) < 0){
#endif
		scePrintf("sceEENetCtlInit() failed.\n");
		ERR_STOP;
	}
	if((ret = sceEENetCtlRegisterEventHandler(event_handler)) < 0){
		scePrintf("sceEENetCtlRegisterEventHandler() failed.\n");
		ERR_STOP;
	}

	if((ret = ent_cnf_init()) < 0){
		scePrintf("ent_cnf_init() failed.\n");
		ERR_STOP;
	}

	/* register device to eenet */
#if defined( HDD_ETHERNET ) || defined( HDD_ETHERNET_WITH_PPPOE )
#if defined(USE_ERX)
	while((ent_smap_erxid = sceErxLoadStartModuleFile(EE_MOD_ENT_SMAP, 0, NULL, 0, NULL, &ret)) < 0)
		scePrintf("ent_smap.erx cannot be loaded.\n");
#endif
	if((ret = sceEENetDeviceSMAPReg(8192, 8192)) < 0){
		scePrintf("ret of sceEENetDeviceSMAPReg() = %d\n", ret);
		ERR_STOP;
	}
#endif
#if defined( USB_ETHERNET ) || defined( USB_ETHERNET_WITH_PPPOE )
#if defined(USE_ERX)
	while((ent_eth_erxid = sceErxLoadStartModuleFile(EE_MOD_ENT_ETH, 0, NULL, 0, NULL, &ret)) < 0)
		scePrintf("ent_eth.erx cannot be loaded.\n");
#endif
	if((ret = sceEENetDeviceETHReg(8192, 8192)) < 0){
		scePrintf("ret of sceEENetDeviceETHReg() = %d\n", ret);
		ERR_STOP;
	}
#endif
#if defined( MODEM )
#if defined(USE_ERX)
	while((ent_ppp_erxid = sceErxLoadStartModuleFile(EE_MOD_ENT_PPP, 0, NULL, 0, NULL, &ret)) < 0)
		scePrintf("ent_ppp.erx cannot be loaded.\n");
#endif
	if((ret = sceEENetDevicePPPReg(8192, 8192)) < 0){
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
	if((ret = sceEENetTcpdump(EENET_IFNAME, "host0:./tcpdump.log", filebuf, 128*1024, 1, NULL, 0, 0, stackbuf, 8192, EENET_APP_PRIO, &daemonp)) < 0){
#else
	if((ret = sceEENetTcpdump(EENET_IFNAME, "host0:./tcpdump.log", NULL, 0, 1, NULL, 0, 0, stackbuf, 8192, EENET_APP_PRIO, &daemonp)) < 0){
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
	if((ret = sceEENetCtlUpInterface(EENET_IFNAME)) < 0){
		scePrintf("sceEENetCtlUpInterface() failed.\n");
		ERR_STOP;
	}
#endif
	/* wait for interface initialization completed */
	WaitEvent(Ev_UpCompleted);

	ChangeThreadPriority(GetThreadId(), 127);

	/************************************************************
	 * main function
	 ************************************************************/

	if((ret = http_test()) < 0){
		scePrintf("http_test() failed.\n");
		ERR_STOP;
	}

	/************************************************************
	 * termination
	 ************************************************************/

#ifdef TCPDUMP
	if (daemonp.tid > 0)
		sceEENetTcpdumpTerm(&daemonp);
	if (filebuf)
		free(filebuf);
#endif

	/* down interface */
	if((ret = sceEENetCtlDownInterface(EENET_IFNAME)) < 0){
		scePrintf("sceEENetCtlDownInterface() failed.\n");
		ERR_STOP;
	}
	WaitEvent(Ev_DownCompleted);

#if defined( HDD_ETHERNET ) || defined( HDD_ETHERNET_WITH_PPPOE )
	if((ret = sceEENetDeviceSMAPUnreg()) < 0) {
		scePrintf("sceEENetDeviceSMAPUnreg() failed.\n");
		ERR_STOP;
	}
#if defined(USE_ERX)
	if(0 <= sceErxStopModule(ent_smap_erxid, 0, NULL, &ret))
		sceErxUnloadModule(ent_smap_erxid);
#endif
#endif
#if defined( USB_ETHERNET ) || defined( USB_ETHERNET_WITH_PPPOE )
	if((ret = sceEENetDeviceETHUnreg()) < 0) {
		scePrintf("sceEENetDeviceETHUnreg() failed.\n");
		ERR_STOP;
	}
#if defined(USE_ERX)
	if(0 <= sceErxStopModule(ent_eth_erxid, 0, NULL, &ret))
		sceErxUnloadModule(ent_eth_erxid);
#endif
#endif
#if defined( MODEM )
	if((ret = sceEENetDevicePPPUnreg()) < 0) {
		scePrintf("sceEENetDevicePPPUnreg() failed.\n");
		ERR_STOP;
	}
#if defined(USE_ERX)
	if(0 <= sceErxStopModule(ent_ppp_erxid, 0, NULL, &ret))
		sceErxUnloadModule(ent_ppp_erxid);
#endif
#endif
	WaitEvent(Ev_DetachCompleted);
	if((ret = sceEENetCtlUnregisterEventHandler(event_handler)) < 0){
		scePrintf("sceEENetCtlUnRegisterEventHandler() failed.\n");
		ERR_STOP;
	}
	if((ret = sceEENetCtlTerm()) < 0) {
		scePrintf("sceEENetCtlTerm() failed.\n");
		ERR_STOP;
	}
	if((ret = sceEENetTerm()) < 0) {
		scePrintf("sceEENetTerm() failed.\n");
		ERR_STOP;
	}
	free(libeenet_pool);
#if defined(USE_ERX)
#ifdef TCPDUMP
	if(0 <= sceErxStopModule(ent_dump_erxid, 0, NULL, &ret))
		sceErxUnloadModule(ent_dump_erxid);
#endif
	if(0 <= sceErxStopModule(ent_cnf_erxid, 0, NULL, &ret))
		sceErxUnloadModule(ent_cnf_erxid);
	if(0 <= sceErxStopModule(eenetctl_erxid, 0, NULL, &ret))
		sceErxUnloadModule(eenetctl_erxid);
#endif

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

#if defined(USE_ERX)
	if(0 <= sceErxStopModule(libhttp_erxid, 0, NULL, &ret))
		sceErxUnloadModule(libhttp_erxid);
	if(0 <= sceErxStopModule(libqp_erxid, 0, NULL, &ret))
		sceErxUnloadModule(libqp_erxid);
	if(0 <= sceErxStopModule(base64_erxid, 0, NULL, &ret))
		sceErxUnloadModule(base64_erxid);
	if(0 <= sceErxStopModule(ntg_ent_erxid, 0, NULL, &ret))
		sceErxUnloadModule(ntg_ent_erxid);
	if(0 <= sceErxStopModule(libeenet_erxid, 0, NULL, &ret))
		sceErxUnloadModule(libeenet_erxid);
	if(0 <= sceErxStopModule(libscf_erxid, 0, NULL, &ret))
		sceErxUnloadModule(libscf_erxid);
	if(0 <= sceErxStopModule(libcdvd_erxid, 0, NULL, &ret))
		sceErxUnloadModule(libcdvd_erxid);
	free(liberx_pool);
#endif

	DeleteSema(sema_id);

	return 0;
}

#if defined(USE_ERX)
int InitErxEnvironment(
	void *pool,
	int pool_size
	)
{
	int res;

	res = sceErxInit(LIBERX_THREAD_PRIORITY, pool,
		pool_size, LIBERX_MAX_MODULES);
	scePrintf("sceErxInit: %d\n", res);

	/* export libgcc function */
	res = sceErxRegisterLibraryEntries(sceLibgccCommonGetErxEntries());
	scePrintf("sceErxExportLibgccCommon:   %08X\n", res);
	res = sceErxRegisterLibraryEntries(sceLibcStdioGetErxEntries());
	scePrintf("sceErxExportLibcStdio:   %08X\n", res);
	res = sceErxRegisterLibraryEntries(sceLibcStringGetErxEntries());
	scePrintf("sceErxExportLibcString:   %08X\n", res);
	res = sceErxRegisterLibraryEntries(sceLibcStdlibGetErxEntries());
	scePrintf("sceErxExportLibcStdlib:   %08X\n", res);
	res = sceErxRegisterLibraryEntries(sceLibcMallocGetErxEntries());
	scePrintf("sceErxExportLibcMalloc:   %08X\n", res);

	return 0;
}
#endif
