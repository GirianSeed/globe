/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 2.5.3
 */
/* 
 *        Emotion Engine (I/O Processor) Library Sample Program
 *
 *                   - <libinsck echo_server> -
 *
 *                         Version 1.20
 *                           Shift-JIS
 *
 *      Copyright (C) 2001 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                            <main.c>
 *              <main function of libinsck echo_server>
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.00           May,17,2001     komaki      first version
 *       1.10           Oct,19,2001     ksh         down if
 *       1.20           Nov,28,2001     mka         use SHUT_RDWR for shutdown
 *                                                  and add free stack area
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
#include <errno.h>
#include <libinsck.h>

#define	PRIVATE	static
#define	PUBLIC
#define ERR_STOP	for ( ; ; )

#define NETBUFSIZE	( 512 )
#define RPCSIZE		( NETBUFSIZE * 4 )
#define BUFSIZE		( 1460 )
#define STACKSIZE	( 0x4000 )
#define PRIO		( 63 )

typedef struct xthread {
	char stack[STACKSIZE];
	struct xthread *next;
	int tid;
	void *parameter;
} XTHREAD;

PRIVATE	u_int net_buf[NETBUFSIZE] __attribute__((aligned(64)));
PRIVATE int exit_event, exit_sema;
PRIVATE XTHREAD *exit_list = NULL;

PRIVATE void	LoadModule( const char *module, int args, const char *argp )
{
	while ( sceSifLoadModule( module, args, argp ) < 0 ) {
		scePrintf( "%s cannot be loaded.\n", module );
	}
}

PRIVATE void	do_exit( XTHREAD *p )
{
	WaitSema(exit_sema);
	p->next = exit_list;
	exit_list = p;
	SignalSema(exit_sema);

	SignalSema(exit_event);
	SleepThread();
}

PRIVATE	void	echo_thread( void *arg )
{
	XTHREAD *p = arg;
	int s, n, r;
	char buf[BUFSIZ];

	s = (int)p->parameter;
	while(0 < (n = recv(s, buf, sizeof(buf), 0)))
		if(n != (r = send(s, buf, n, 0))){
			scePrintf("send %d->%d\n", n, r);
			if(0 > r){
				scePrintf("send: errno=%d\n", sceInsockErrno);
				break;
			}
		}
	if(0 > n)
		scePrintf("recv: errno=%d\n", sceInsockErrno);
	shutdown(s, SHUT_RDWR);
	scePrintf("connetion closed\n");
	sceInsockTerminate(0);
	do_exit(p);
}

PRIVATE void	clean_thread( void *arg )
{
	XTHREAD *p;
	int r;

	while(0 <= (r = WaitSema(exit_event))){
		if(0 > (r = WaitSema(exit_sema)))
			break;
		while(NULL != (p = exit_list)){
			exit_list = p->next;
			TerminateThread(p->tid);
			DeleteThread(p->tid);
			free(p);
		}
		SignalSema(exit_sema);
	}
	scePrintf("Waitsema() failed. (%d)\n", r);
	SleepThread();
}

PRIVATE XTHREAD *start_thread( void (*func)(void *arg), void *parameter )
{
	struct ThreadParam tparam;
	XTHREAD *p;
	int r;

	if((p = (XTHREAD *)memalign(16, sizeof(*p))) == NULL){
		scePrintf("memalign failed.\n");
		ERR_STOP;
	}
	tparam.entry = func;
	tparam.stack = p->stack;
	tparam.stackSize = STACKSIZE;
	tparam.gpReg = &_gp;
	tparam.initPriority = PRIO;
	if(0 >= (p->tid = CreateThread(&tparam))){
		scePrintf("CreateThread() failed (%d)\n", p->tid);
		free(p);
		ERR_STOP;
	}
	p->parameter = parameter;
	if(0 > (r = StartThread(p->tid, p))){
		DeleteThread(p->tid);
		scePrintf("StartThread() failed (%d)\n", p->tid);
		free(p);
		ERR_STOP;
	}
	return(p);
}

PRIVATE int	create_sema( int init_count, int max_count )
{
	struct SemaParam sparam;
	int sid;

	memset(&sparam, 0, sizeof(sparam));
	sparam.initCount = init_count;
	sparam.maxCount = max_count;
	if(0 > (sid = CreateSema(&sparam))){
		scePrintf("CreateSema() failed (%d)\n", sid);
		ERR_STOP;
	}
	return(sid);
}

PRIVATE	void	echo_server( void )
{
	struct sockaddr_in sin;
	int s, ns, len;
	XTHREAD *pclean;

	if(0 > (s = socket(AF_INET, SOCK_STREAM, 0))){
		scePrintf("socket: errno=%d\n", sceInsockErrno);
		ERR_STOP;
	}

	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(7);

	if(0 > bind(s, (struct sockaddr *)&sin, sizeof(sin))){
		scePrintf("bind: errno=%d\n", sceInsockErrno);
		shutdown(s, SHUT_RDWR);
		ERR_STOP;
	}
	if(0 > listen(s, 5)){
		scePrintf("listen: errno=%d\n", sceInsockErrno);
		shutdown(s, SHUT_RDWR);
		ERR_STOP;
	}

	exit_event = create_sema(0, 256);
	exit_sema = create_sema(1, 1);
	pclean = start_thread(clean_thread, NULL);

	len = sizeof(sin);
	while(0 <= (ns = accept(s, (struct sockaddr *)&sin, &len))){
		scePrintf("connected from %s:%d\n",
			inet_ntoa(sin.sin_addr), ntohs(sin.sin_port));
		start_thread(echo_thread, (void *)ns);
	}
	if(0 > ns){
		scePrintf("accept: errno=%d\n", sceInsockErrno);
		ERR_STOP;
	}

	TerminateThread(pclean->tid);
	DeleteThread(pclean->tid);
	free(pclean);
	DeleteSema(exit_event);
	DeleteSema(exit_sema);
	shutdown(s, SHUT_RDWR);
	sceInsockTerminate(0);
}

int	main( int argc, char* argv[] )
{
	sceSifMClientData cd;
	int if_id;
	struct sceInetAddress myaddr;
	char myptrname[128];
	int ret;

	if(argc != 1){
		scePrintf("usage: %s\n", argv[0]);
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
	if((ret = sceInetAddress2String(&cd, net_buf, myptrname, sizeof(myptrname), &myaddr)) != sceINETE_OK){
		scePrintf("invalid addr\n");
		ERR_STOP;
	}
	scePrintf("My address: %s\n", myptrname);


	/***************************************************************
		Main
	***************************************************************/
	echo_server();


	/***************************************************************
		Terminate libnet
	***************************************************************/
	down_interface(&cd, net_buf, 0);
	if((ret = unreg_handler(&cd, net_buf)) < 0){
		scePrintf("unreg_handler() failed.\n");
		ERR_STOP;
	}
	libnet_term(&cd);

	return 0;
}

/*** End of file ***/

