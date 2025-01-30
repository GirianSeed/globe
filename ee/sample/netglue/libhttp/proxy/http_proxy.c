/* SCE CONFIDENTIAL
"PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
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
 *                        <http_proxy.c>
 *                       <main functions>
 *
 *       Version   Date           Design              Log
 *  --------------------------------------------------------------------
 *       1.0.0     Oct,22,2001    komaki              first version
 */

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
#define	MODEM
#endif

#if 1
#define	UP_INTERFACE_AUTO
#endif

#include <sys/types.h>
#include <eekernel.h>
#include <stdlib.h>
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

#include <libhttp.h>

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

static u_int rpc_buffer[NETBUFSIZE]  __attribute__((aligned(64)));

static int wait_eot;
static void eot_cb(sceHTTPClient_t *p, int rtn, void *uopt){
	printf("<<End of Transaction (%d)>>\n", rtn);
	wait_eot = 1;
}

int main(int argc, char **argv){
	sceHTTPClient_t *client;
	sceHTTPParsedURI_t *p;
	sceHTTPParsedURI_t *proxy;
	sceHTTPResponse_t *response;
	sceHTTPHeaderList_t *list;
	int method;
	int i;
	int ret;
	char buffer0[BUFSIZE];
	int if_id;
	struct sceInetAddress myaddr;
	sceSifMClientData cd;

	if(argc < 3){
		printf("usage: http_proxy.elf url proxy\n");
		ERR_STOP;
	}

	if((ret = sceHTTPInit()) < 0){
		printf("sceHTTPInit() failed.\n");
		ERR_STOP;
	}

	method = sceHTTPM_GET;

	p = sceHTTPParseURI(argv[1], (sceHTTPParseURI_FILENAME
					| sceHTTPParseURI_SEARCHPART));
	if(!p){
		printf("bad URI\n");
		ERR_STOP;
	}

	proxy = sceHTTPParseURI(argv[2], 0);
	if(!p){
		printf("bad Proxy\n");
		ERR_STOP;
	}

	// init sif stuff
	sceSifInitRpc(0);

	// network stuff
	while(sceSifLoadModule(IOP_MOD_INET, 0, NULL) < 0)
		printf("inet.irx cannot be loaded.\n");

	while(sceSifLoadModule(IOP_MOD_NETCNF, sizeof(NETCNF_ARG), NETCNF_ARG) < 0)
		printf("netcnf.irx cannot be loaded.\n");

	while(sceSifLoadModule(IOP_MOD_INETCTL, sizeof(INETCTL_ARG), INETCTL_ARG) < 0)
		printf("inetctl.irx cannot be loaded.\n");

#if defined(USB_ETHERNET) || defined(USB_ETHERNET_WITH_PPPOE)
	while( sceSifLoadModule(IOP_MOD_USBD, 0, NULL) < 0)
		printf("usbd.irx cannot be loaded.\n");

	while( sceSifLoadModule(IOP_MOD_AN986, 0, NULL) < 0)
		printf("an986.irx cannot be loaded.\n");
#elif defined(HDD_ETHERNET) || defined(HDD_ETHERNET_WITH_PPPOE)
	while( sceSifLoadModule(IOP_MOD_DEV9, 0, NULL) < 0)
		printf("dev9.irx cannot be loaded.\n");

	while( sceSifLoadModule(IOP_MOD_SMAP, 0, NULL) < 0)
		printf("smap.irx cannot be loaded.\n");
#endif
#if defined(USB_ETHERNET_WITH_PPPOE) || defined(HDD_ETHERNET_WITH_PPPOE)
	while( sceSifLoadModule(IOP_MOD_PPP, 0, NULL) < 0)
		printf("ppp.irx cannot be loaded.\n");

	while( sceSifLoadModule(IOP_MOD_PPPOE, 0, NULL) < 0)
		printf("pppoe.irx cannot be loaded.\n");
#endif

	while( sceSifLoadModule(IOP_MOD_MSIFRPC, 0, NULL) < 0)
		printf("msifrpc.irx cannot be loaded.\n");

	while( sceSifLoadModule(IOP_MOD_LIBNET, 0, NULL) < 0)
		printf("libnet.irx cannot be loaded.\n");

#if defined(HDD_ETHERNET) || defined(HDD_ETHERNET_WITH_PPPOE)
	PreparePowerOff();
#endif
	sceSifMInitRpc(0);
	libnet_init(&cd, 2048, 8192, 32);

	if((ret = reg_handler(&cd, rpc_buffer)) < 0){
		printf("reg_handler() failed.\n");
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
		printf("invalid addr\n");
		ERR_STOP;
	}
	printf("My address: %s\n", buffer0);

	ChangeThreadPriority(GetThreadId(), 127);

	if ((client = sceHTTPCreate()) == NULL){
		printf("sceHTTPCreate failed\n");
		ERR_STOP;
	}

	if (sceHTTPSetOption(client, sceHTTPO_ParsedURI, p) < 0){
		printf("sceHTTPSetOption failed\n");
		ERR_STOP;
	}
	if (sceHTTPSetOption(client, sceHTTPO_ProxyURI, proxy) < 0){
		printf("sceHTTPSetOption failed\n");
		ERR_STOP;
	}
	if (sceHTTPSetOption(client, sceHTTPO_Method, method) < 0){
		printf("sceHTTPSetOption failed\n");
		ERR_STOP;
	}
	if (sceHTTPSetOption(client, sceHTTPO_EndOfTransactionCB, eot_cb, NULL) < 0){
		printf("sceHTTPSetOption failed\n");
		ERR_STOP;
	}
	if (sceHTTPSetOption(client, sceHTTPO_BlockingMode, 0) < 0){
		printf("sceHTTPSetOption failed\n");
		ERR_STOP;
	}
	if (sceHTTPOpen(client) < 0){
		printf("sceHTTPOpen failed\n");
		ERR_STOP;
	}
	if (sceHTTPRequest(client) < 0){
		printf("sceHTTPRequest failed\n");
		ERR_STOP;
	}

	/* wait callback */
	while (wait_eot == 0)
		;

	/* show response */
	if ((response = sceHTTPGetResponse(client)) == NULL){
		printf("sceHTTPGetResponse failed\n");
		ERR_STOP;
	}
	printf("<<Response>>\n");
	printf("Status-line: Protocol=%s Version=%d Revison=%d Code=%d(%s,%s)\n",
		(response->server_prot == sceHTTPProt_HTTP ? "http" : "https"),
		response->http_ver, response->http_rev,
		response->code, response->reason,
		sceHTTPErrorString(response->code));

	for (list = response->headers; list; list = sceHTTPNextHeader(list))
		printf("%s: %s\n", list->name, list->value);

	printf("<<Body length=%d>>\n", (int)response->length);
	for(i = 0; i < response->length; i++)
		printf("%c", response->entity[i]);
	printf("<<End of Body>>\n");

	if (sceHTTPClose(client) < 0){
		printf("sceHTTPClose failed\n");
		ERR_STOP;
	}
	if (sceHTTPDestroy(client) < 0){
		printf("sceHTTPDestroy failed\n");
		ERR_STOP;
	}
	if (sceHTTPFreeURI(p)){
		printf("sceHTTPFreeURI failed\n");
		ERR_STOP;
	}
	if (sceHTTPFreeURI(proxy) < 0){
		printf("sceHTTPFreeURI failed\n");
		ERR_STOP;
	}

	if (sceHTTPTerminate()){
		printf("sceHTTPTerminate failed\n");
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

	return 0;
}
