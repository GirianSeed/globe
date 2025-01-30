/* SCE CONFIDENTIAL
"PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/*
 *              Emotion Engine Library Sample Program
 *
 *                        - http_proxy -
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
 *       1.0.0     Aug,28,2002    komaki              first version
 */

#if 0
#define USB_ETHERNET
#endif
#if 0
#define USB_ETHERNET_WITH_PPPOE
#endif
#if 1
#define HDD_ETHERNET
#endif
#if 0
#define HDD_ETHERNET_WITH_PPPOE
#endif
#if 0
#define	MODEM
#endif

#if 1
#define	UP_INTERFACE_AUTO
#endif

#include <sys/types.h>
#include <eekernel.h>
#if defined(USE_ERX)
#include <malloc.h>
#include <liberx.h>
#include <libgcc_export.h>
#include <libc_export.h>
#endif 
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <eeregs.h>
#include <sifdev.h>
#include <sifrpc.h>
#include <libmrpc.h>
#include <libnet.h>
#include <errno.h>
#if defined(HDD_ETHERNET) || defined(HDD_ETHERNET_WITH_PPPOE)
#include <poweroff.h>
#endif
#include <http_proxy.h>

#if defined(USE_ERX)
#define LIBERX_THREAD_PRIORITY      10
#define LIBERX_MEMORY_SIZE          (8*1024*1024)
#define LIBERX_MAX_MODULES          32

static int libcdvd_erxid, ntg_isck_erxid, libhttp_erxid;
static int base64_erxid, libqp_erxid;
static int netcnfif_erxid;
static int libinsck_erxid;
static int libmrpc_erxid;
static int libnet_erxid;
static int libnetif_erxid;
#endif

// --------- DEFINE FOR IOP MODULES --------
#define MAIN_MOD_ROOT     "host0:/usr/local/sce/iop/modules/"
#define NETICON_ROOT      "host0:/usr/local/sce/conf/neticon/japanese/"
#define IOP_MOD_INET      MAIN_MOD_ROOT "inet.irx"
#define IOP_MOD_AN986     MAIN_MOD_ROOT "an986.irx"
#define IOP_MOD_USBD      MAIN_MOD_ROOT "usbd.irx"
#define IOP_MOD_NETCNF    MAIN_MOD_ROOT "netcnf.irx"
#define IOP_MOD_INETCTL   MAIN_MOD_ROOT "inetctl.irx"
#define IOP_MOD_MSIFRPC   MAIN_MOD_ROOT "msifrpc.irx"
#define IOP_MOD_DEV9      MAIN_MOD_ROOT "dev9.irx"
#define IOP_MOD_SMAP      MAIN_MOD_ROOT "smap.irx"
#define IOP_MOD_PPP       MAIN_MOD_ROOT "ppp.irx"
#define IOP_MOD_PPPOE     MAIN_MOD_ROOT "pppoe.irx"
#define IOP_MOD_LIBNET    MAIN_MOD_ROOT "libnet.irx"
#define	IOP_MOD_MODEMDRV  ""
#define NET_DB            "host0:/usr/local/sce/conf/net/net.db"
#define INETCTL_ARG       "-no_auto" "\0" "-no_decode"
#define NETCNF_ICON       NETICON_ROOT "SYS_NET.ICO"
#define NETCNF_ICONSYS    NETICON_ROOT "icon.sys"
#define NETCNF_ARG        "icon=" NETCNF_ICON "\0" "iconsys=" NETCNF_ICONSYS
#define	MODEMDRV_ARG      ""

#if defined(USE_ERX)
#define EE_MOD_ROOT       "host0:/usr/local/sce/ee/modules/"
#define EE_MOD_LIBCDVD    EE_MOD_ROOT "libcdvd.erx"
#define EE_MOD_NTG_ISCK   EE_MOD_ROOT "ntg_isck.erx"
#define EE_MOD_LIBHTTP    EE_MOD_ROOT "libhttp.erx"
#define EE_MOD_NETCNFIF   EE_MOD_ROOT "netcnfif.erx"
#define EE_MOD_LIBINSCK   EE_MOD_ROOT "libinsck.erx"
#define EE_MOD_LIBMRPC    EE_MOD_ROOT "libmrpc.erx"
#define EE_MOD_LIBNET     EE_MOD_ROOT "libnet.erx"
#define EE_MOD_LIBNETIF   EE_MOD_ROOT "libnetif.erx"
#define EE_MOD_BASE64     EE_MOD_ROOT "base64.erx"
#define EE_MOD_LIBQP      EE_MOD_ROOT "libqp.erx"
#endif

#if defined(USB_ETHERNET)
#define USR_CONF_NAME     "Combination4"
#elif defined(USB_ETHERNET_WITH_PPPOE)
#define USR_CONF_NAME     "Combination5"
#elif defined(HDD_ETHERNET)
#define USR_CONF_NAME     "Combination6"
#elif defined(HDD_ETHERNET_WITH_PPPOE)
#define USR_CONF_NAME     "Combination7"
#elif defined(MODEM)
#define USR_CONF_NAME     ""
#endif

#define ERR_STOP	while(1)

#define NETBUFSIZE	512
#define RPCSIZE		(NETBUFSIZE * 4)
#define BUFSIZE		1460
#define STACKSIZE	0x4000
#define PRIO		63

#if defined(USE_ERX)
int InitErxEnvironment(void *pool, int pool_size);
#endif

static u_int rpc_buffer[NETBUFSIZE]  __attribute__((aligned(64)));

int main(int argc, char **argv){
#if defined(USE_ERX)
	void *liberx_pool;
#endif
	int ret;
	char buffer0[BUFSIZE];
	int if_id;
	struct sceInetAddress myaddr;
	sceSifMClientData cd;

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

	/* load libcdvd/netcnfif/libnet/libnetif/libmrpc/libinsck/ntg_isck/base64/libqp/libhttp erx */
	while((libcdvd_erxid = sceErxLoadStartModuleFile(EE_MOD_LIBCDVD, 0, NULL, 0, NULL, &ret)) < 0)
		scePrintf("libcdvd.erx cannot be loaded.\n");
	while((netcnfif_erxid = sceErxLoadStartModuleFile(EE_MOD_NETCNFIF, 0, NULL, 0, NULL, &ret)) < 0)
		scePrintf("netcnfif.erx cannot be loaded.\n");
	while((libmrpc_erxid = sceErxLoadStartModuleFile(EE_MOD_LIBMRPC, 0, NULL, 0, NULL, &ret)) < 0)
		scePrintf("libmrpc.erx cannot be loaded.\n");
	while((libnet_erxid = sceErxLoadStartModuleFile(EE_MOD_LIBNET, 0, NULL, 0, NULL, &ret)) < 0)
		scePrintf("libnet.erx cannot be loaded.\n");
	while((libnetif_erxid = sceErxLoadStartModuleFile(EE_MOD_LIBNETIF, 0, NULL, 0, NULL, &ret)) < 0)
		scePrintf("libnetif.erx cannot be loaded.\n");
	while((libinsck_erxid = sceErxLoadStartModuleFile(EE_MOD_LIBINSCK, 0, NULL, 0, NULL, &ret)) < 0)
		scePrintf("libinsck.erx cannot be loaded.\n");
	while((ntg_isck_erxid = sceErxLoadStartModuleFile(EE_MOD_NTG_ISCK, 0, NULL, 0, NULL, &ret)) < 0)
		scePrintf("ntg_isck.erx cannot be loaded.\n");
	while((base64_erxid = sceErxLoadStartModuleFile(EE_MOD_BASE64, 0, NULL, 0, NULL, &ret)) < 0)
		scePrintf("base64.erx cannot be loaded.\n");
	while((libqp_erxid = sceErxLoadStartModuleFile(EE_MOD_LIBQP, 0, NULL, 0, NULL, &ret)) < 0)
		scePrintf("libqp.erx cannot be loaded.\n");
	while((libhttp_erxid = sceErxLoadStartModuleFile(EE_MOD_LIBHTTP, 0, NULL, 0, NULL, &ret)) < 0)
		scePrintf("libhttp.erx cannot be loaded.\n");
#endif

	if((ret = http_proxy_init(argc, argv)) < 0){
		scePrintf("http_proxy_init() failed.\n");
		ERR_STOP;
	}

	// init sif stuff
	sceSifInitRpc(0);

	// network stuff
	while(sceSifLoadModule(IOP_MOD_INET, 0, NULL) < 0)
		scePrintf("inet.irx cannot be loaded.\n");

	while(sceSifLoadModule(IOP_MOD_NETCNF, sizeof(NETCNF_ARG), NETCNF_ARG) < 0)
		scePrintf("netcnf.irx cannot be loaded.\n");

	while(sceSifLoadModule(IOP_MOD_INETCTL, sizeof(INETCTL_ARG), INETCTL_ARG) < 0)
		scePrintf("inetctl.irx cannot be loaded.\n");

#if defined(USB_ETHERNET) || defined(USB_ETHERNET_WITH_PPPOE)
	while( sceSifLoadModule(IOP_MOD_USBD, 0, NULL) < 0)
		scePrintf("usbd.irx cannot be loaded.\n");

	while( sceSifLoadModule(IOP_MOD_AN986, 0, NULL) < 0)
		scePrintf("an986.irx cannot be loaded.\n");
#elif defined(HDD_ETHERNET) || defined(HDD_ETHERNET_WITH_PPPOE)
	while( sceSifLoadModule(IOP_MOD_DEV9, 0, NULL) < 0)
		scePrintf("dev9.irx cannot be loaded.\n");

	while( sceSifLoadModule(IOP_MOD_SMAP, 0, NULL) < 0)
		scePrintf("smap.irx cannot be loaded.\n");
#endif
#if defined(USB_ETHERNET_WITH_PPPOE) || defined(HDD_ETHERNET_WITH_PPPOE)
	while( sceSifLoadModule(IOP_MOD_PPP, 0, NULL) < 0)
		scePrintf("ppp.irx cannot be loaded.\n");

	while( sceSifLoadModule(IOP_MOD_PPPOE, 0, NULL) < 0)
		scePrintf("pppoe.irx cannot be loaded.\n");
#endif

	while( sceSifLoadModule(IOP_MOD_MSIFRPC, 0, NULL) < 0)
		scePrintf("msifrpc.irx cannot be loaded.\n");

	while( sceSifLoadModule(IOP_MOD_LIBNET, 0, NULL) < 0)
		scePrintf("libnet.irx cannot be loaded.\n");

#if defined(HDD_ETHERNET) || defined(HDD_ETHERNET_WITH_PPPOE)
	PreparePowerOff();
#endif
	sceSifMInitRpc(0);
	libnet_init(&cd, 2048, 8192, 32);

	if((ret = reg_handler(&cd, rpc_buffer)) < 0){
		scePrintf("reg_handler() failed.\n");
		ERR_STOP;
	}
#ifdef	UP_INTERFACE_AUTO
	scePrintf("up interface auto\n");
	if((ret = load_set_conf(&cd, rpc_buffer, NET_DB, USR_CONF_NAME)) < 0){
		scePrintf("load_set_conf() failed.\n");
		ERR_STOP;
	}
	if((ret = wait_get_addr(&cd, rpc_buffer, &if_id, &myaddr)) < 0){
		scePrintf("wait_get_addr() failed.\n");
		ERR_STOP;
	}
#else
	scePrintf("up interface no auto\n");
	if((ret = load_set_conf_only(&cd, rpc_buffer, NET_DB, USR_CONF_NAME)) < 0){
		scePrintf("load_set_conf_only() failed.\n");
		ERR_STOP;
	}

	if((ret = get_interface_id(&cd, rpc_buffer, &if_id)) < 0){
		scePrintf("get_interface_id()\n");
		ERR_STOP;
	}
	up_interface(&cd, rpc_buffer, if_id);

	if((ret = wait_get_addr_only(&cd, rpc_buffer, &if_id, &myaddr)) < 0){
		scePrintf("wait_get_addr_only() failed.\n");
		ERR_STOP;
	}
#endif	/*** UP_INTERFACE_AUTO ***/
	if((ret = sceInetAddress2String(&cd, rpc_buffer, buffer0, 128,
			&myaddr)) != sceINETE_OK){
		scePrintf("invalid addr\n");
		ERR_STOP;
	}
	scePrintf("My address: %s\n", buffer0);

	ChangeThreadPriority(GetThreadId(), 127);

	if ((ret = http_proxy()) < 0){
		scePrintf("http_proxy() failed\n");
		ERR_STOP;
	}

	/***************************************************************
		Terminate libnet
	***************************************************************/	
	down_interface(&cd, rpc_buffer, 0);
	if((ret = unreg_handler(&cd, rpc_buffer)) < 0){
		scePrintf("unreg_handler() failed.\n");
		ERR_STOP;
	}
	libnet_term(&cd);

#if defined(USE_ERX)
	if(0 <= sceErxStopModule(libhttp_erxid, 0, NULL, &ret))
		sceErxUnloadModule(libhttp_erxid);
	if(0 <= sceErxStopModule(libqp_erxid, 0, NULL, &ret))
		sceErxUnloadModule(libqp_erxid);
	if(0 <= sceErxStopModule(base64_erxid, 0, NULL, &ret))
		sceErxUnloadModule(base64_erxid);
	if(0 <= sceErxStopModule(ntg_isck_erxid, 0, NULL, &ret))
		sceErxUnloadModule(ntg_isck_erxid);
	if(0 <= sceErxStopModule(libinsck_erxid, 0, NULL, &ret))
		sceErxUnloadModule(libinsck_erxid);
	if(0 <= sceErxStopModule(libnetif_erxid, 0, NULL, &ret))
		sceErxUnloadModule(libnetif_erxid);
	if(0 <= sceErxStopModule(libnet_erxid, 0, NULL, &ret))
		sceErxUnloadModule(libnet_erxid);
	if(0 <= sceErxStopModule(libmrpc_erxid, 0, NULL, &ret))
		sceErxUnloadModule(libmrpc_erxid);
	if(0 <= sceErxStopModule(netcnfif_erxid, 0, NULL, &ret))
		sceErxUnloadModule(netcnfif_erxid);
	if(0 <= sceErxStopModule(libcdvd_erxid, 0, NULL, &ret))
		sceErxUnloadModule(libcdvd_erxid);
	free(liberx_pool);
#endif

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
	res = sceErxRegisterLibraryEntries(sceLibcErrnoGetErxEntries());
	scePrintf("sceErxExportLibcErrno:   %08X\n", res);

	return 0;
}
#endif
