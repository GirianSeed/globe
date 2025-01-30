/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/* 
 *                          Libnet Library
 *
 *      Copyright (C) 2000-2003 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *       Version      Date        Design      Log
 *  --------------------------------------------------------------------
 *       1.1.0        2002/01/24  ksh         remake
 *       1.2.0        2003/04/02  ksh         USB to HDD Ethernet etc.
 */

/**********************************************************************/
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
#define	IOP_MOD_LIBNET    MAIN_MOD_ROOT "libnet.irx"
#define IOP_MOD_INETLOG   SCETOP "iop/util/inet/inetlog.irx"
#define	IOP_MOD_MODEMDRV  ""
#define	NET_DB            SCETOP "conf/net/net.db"
#define INET_ARG          "debug=18"
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
#include "main.h"
#include "comm.h"

#define PRIVATE static
#define PUBLIC
#define ERR_STOP        for ( ; ; )

PRIVATE	u_int net_buf[NETBUFSIZE] __attribute__((aligned(64)));
PRIVATE	int th_num = 0;
PUBLIC	int passive_sema;
PUBLIC	int fin_sema;
PUBLIC	struct connection_data connection[MAX_CONNECTION];


PRIVATE int 	LoadModule( const char *module, int args, const char *argp )
{
	int 	id;
	
	while ( ( id = sceSifLoadModule( module, args, argp ) ) < 0 ) {
		scePrintf( "%s cannot be loaded.\n", module );
	}

	return( id );
}

PRIVATE	void	clean_up( void *data )
{
	int i, ret;

	while(1) {
		WaitSema(fin_sema);

		for(i = 0; i < MAX_CONNECTION; i++){
			if(connection[i].fin){
				if((ret = DeleteThread(connection[i].tid)) < 0)
					continue;
				free(connection[i].stack);
				connection[i].exist = 0;
				connection[i].fin = 0;
			}
		}
	}

	return;
}

PRIVATE	int 	search_sid( void )
{
	int i;
	for(i = 0; i < MAX_CONNECTION; i++){
		if(connection[i].exist == 0)
			return i;
	}
	return -1;
}

PRIVATE	int 	start_thread( void *func, char *stackp, int size, int prio, void *argp )
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
	return tid;
}

int 	main( int argc, char* argv[] )
{
	struct SemaParam semap;
	sceSifMClientData cd;
	int ret, i;
	int if_id;
	struct sceInetAddress myaddr;
	char myname[128];
	char myptrname[128];
	char *clean_up_stack;
	int sid;
	int tid;

	if(argc != 2){
		scePrintf("usage: %s <th_num>\n", argv[0]);
		ERR_STOP;
	}
	
	if ((th_num = atoi(argv[1])) <= 0){
		scePrintf("th_num = %d\n", th_num);
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
	for(i = 0; i < MAX_CONNECTION; i++)
		memset(&connection[i], 0, sizeof(struct connection_data));

	semap.initCount = th_num;
	semap.maxCount = th_num;
	passive_sema = CreateSema(&semap);
	semap.initCount = 0;
	semap.maxCount = 1;
	fin_sema = CreateSema(&semap);

	/* thread for clean up each conneciton thread*/
	if((clean_up_stack = (char *)memalign(16, STACKSIZE)) == NULL){
		scePrintf("malloc failed.\n");
		ERR_STOP;
	}
	start_thread(clean_up, clean_up_stack, STACKSIZE, PRIO, NULL);

	while(1) {
		WaitSema(passive_sema);
		if((sid = search_sid()) < 0)
			scePrintf("invalid sid %d\n", sid);
		connection[sid].sid = sid;
		connection[sid].exist = 1;
		if((connection[sid].stack = (char *)memalign(16, STACKSIZE)) == NULL){
			scePrintf("malloc failed.\n");
			ERR_STOP;
		}
		scePrintf("main(): sid %d, stack = 0x%x\n", sid, (u_int)connection[sid].stack);
		tid = start_thread(load_testd, connection[sid].stack, STACKSIZE, PRIO, &connection[sid].sid);
		connection[sid].tid = tid;
	}


	/***************************************************************
		Terminate libnet
	***************************************************************/
	down_interface(&cd, net_buf, 0);
	if((ret = unreg_handler(&cd, net_buf)) < 0){
		scePrintf("unreg_handler() failed.\n");
		ERR_STOP;
	}
	libnet_term(&cd);
	sceSifMExitRpc();

	return 0;

}

/*** End of file ***/

