/* SCE CONFIDENTIAL
 "PlayStation 2" Programmer Tool Runtime Library Release 3.0
 */
/* 
 *        Emotion Engine (I/O Processor) Library Sample Program
 *
 *                    - <libinsck http_get> -
 *
 *                         Version 1.30
 *                           Shift-JIS
 *
 *      Copyright (C) 2001-2003 Sony Computer Entertainment Inc.
 *                        All Rights Reserved.
 *
 *                            <http.c>
 *              <main function of libinsck http_get>
 *
 *       Version        Date            Design      Log
 *  --------------------------------------------------------------------
 *       1.00           May,17,2001     komaki      first version
 *       1.10           Oct,19,2001     ksh         down if & improvement
 *       1.20           Nov,28,2001     mka         use SHUT_RDWR for shutdown
 *       1.30           Apr,02,2003     ksh         USB to HDD Ethernet etc.
 *       1.40           2003/04/28      ksh         add erx handling
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
#if defined(USE_ERX)
#include <liberx.h>
#include <libgcc_export.h>
#include <libc_export.h>
#endif
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
#define LIBNET_ARG        "-verbose"
#define	NETCNF_ICON       SETAPP_ROOT "SYS_NET.ICO"
#define	NETCNF_ICONSYS    SETAPP_ROOT "icon.sys"
#define	NETCNF_ARG        "icon=" NETCNF_ICON "\0" "iconsys=" NETCNF_ICONSYS
#define	MODEMDRV_ARG      ""

#if defined(USE_ERX)
#define	EE_MOD_ROOT       SCETOP "ee/modules/"
#define	EE_MOD_LIBCDVD    EE_MOD_ROOT "libcdvd.erx"
#define	EE_MOD_LIBSCF     EE_MOD_ROOT "libscf.erx"
#define	EE_MOD_LIBMRPC    EE_MOD_ROOT "libmrpc.erx"
#define	EE_MOD_LIBNET     EE_MOD_ROOT "libnet.erx"
#define	EE_MOD_LIBNETIF   EE_MOD_ROOT "libnetif.erx"
#define	EE_MOD_LIBINSCK   EE_MOD_ROOT "libinsck.erx"
#define	EE_MOD_NETCNFIF   EE_MOD_ROOT "netcnfif.erx"
#define	EE_MOD_NETIFMC2   EE_MOD_ROOT "netifmc2.erx"

#define	LIBERX_THREAD_PRIORITY	( 10 )
#define	LIBERX_MEMORY_SIZE	( 8 * 1024 * 1024 )
#define	LIBERX_MAX_MODULES	( 32 )
#endif

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

#define REQUEST_GET1 "GET / HTTP/1.1\r\nHOST: "
#define REQUEST_GET2 "\r\n\r\n"

PRIVATE	u_int rpc_buffer[NETBUFSIZE]  __attribute__((aligned(64)));
PRIVATE	char server_name[128];


PRIVATE int 	LoadModule( const char *module, int args, const char *argp )
{
	int id;
	
	while ( ( id = sceSifLoadModule( module, args, argp ) ) < 0 ) {
		scePrintf( "%s cannot be loaded.\n", module );
	}
	return( id );
}

#if defined(USE_ERX)
PRIVATE	int 	LoadErxModule( const char *module, int args, const char *argp )
{
	int 	id;
	int 	result;

	while ( ( id = sceErxLoadStartModuleFile( module, 0, NULL, args, argp, &result ) ) < 0 ) {
		scePrintf( "%s cannot be loaded.\n", module );
	}

	return( id );
}

PRIVATE	int 	InitErxEnvironment( void *pool, int pool_size )
{
	int 	res;

	res = sceErxInit( LIBERX_THREAD_PRIORITY, pool, pool_size, LIBERX_MAX_MODULES );
	scePrintf( "sceErxInit: %d\n", res );
	res = sceErxRegisterLibraryEntries( sceLibgccCommonGetErxEntries() );
	scePrintf( "sceErxExportLibgccCommon\n" );
	res = sceErxRegisterLibraryEntries( sceLibcStdioGetErxEntries() );
	scePrintf( "sceErxExportLibcStdio\n" );
	res = sceErxRegisterLibraryEntries( sceLibcStdlibGetErxEntries() );
	scePrintf( "sceErxExportLibcStdlib\n" );
	res = sceErxRegisterLibraryEntries( sceLibcStringGetErxEntries() );
	scePrintf( "sceErxExportLibcString\n" );
	res = sceErxRegisterLibraryEntries( sceLibcMallocGetErxEntries() );
	scePrintf( "sceErxExportLibcMalloc\n" );
	res = sceErxRegisterLibraryEntries( sceLibcErrnoGetErxEntries() );
	scePrintf( "sceErxExportLibcErrno\n" );

	return( 0 );
}
#endif

int	main( int argc, char* argv[] )
{
#if defined(USE_ERX)
	void	*liberx_pool;
#endif
	int  tcp_len, ret, fd, total_len = 0;
	char get_cmd[128];
	char buffer0[BUFSIZE];
	int if_id;
	struct sceInetAddress myaddr;
	sceSifMClientData cd;
	struct sockaddr_in sin;
	u_int temp;
	int i, s;

	if(argc != 2){
		scePrintf("usage: %s <URL>\n", argv[0]);
		ERR_STOP;
	}
	strcpy(server_name, argv[1]);


	/***************************************************************
		Preparation
	***************************************************************/
#if defined(USE_ERX)
	liberx_pool = memalign( 256, LIBERX_MEMORY_SIZE );
	if ( liberx_pool == NULL ) {
		scePrintf( "memalign() failed.\n" );
		ERR_STOP;
	}
	if ( InitErxEnvironment( liberx_pool, LIBERX_MEMORY_SIZE ) < 0 ) {
		scePrintf( "InitErxEnvironment() failed.\n" );
		ERR_STOP;
	}
	LoadErxModule( EE_MOD_LIBCDVD, 0, NULL );
	LoadErxModule( EE_MOD_LIBSCF, 0, NULL );
#endif

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

#if defined(USE_ERX)
	LoadErxModule( EE_MOD_LIBMRPC, 0, NULL );
	LoadErxModule( EE_MOD_LIBNET, 0, NULL );
	LoadErxModule( EE_MOD_LIBINSCK, 0, NULL );
	LoadErxModule( EE_MOD_NETCNFIF, 0, NULL );
	LoadErxModule( EE_MOD_LIBNETIF, 0, NULL );
#endif

	/***************************************************************
		Initialize libnet
	***************************************************************/
	sceSifMInitRpc(0);
	libnet_init(&cd, 2048, 8192, 32);
	if((ret = reg_handler(&cd, rpc_buffer)) < 0){
		scePrintf("reg_handler() failed.\n");
		ERR_STOP;
	}

#ifdef  UP_INTERFACE_AUTO
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
	up_interface(&cd, rpc_buffer, 0);

	if((ret = wait_get_addr_only(&cd, rpc_buffer, &if_id, &myaddr)) < 0){
		scePrintf("wait_get_addr_only() failed.\n");
		ERR_STOP;
	}
#endif  /*** UP_INTERFACE_AUTO ***/


	/***************************************************************
		Display IP address
	***************************************************************/
	if((ret = sceInetAddress2String(&cd, rpc_buffer, buffer0, 128, &myaddr)) != sceINETE_OK){
		scePrintf("invalid addr\n");
		ERR_STOP;
	}
	scePrintf("My address: %s\n", buffer0);


	/***************************************************************
		Main
	***************************************************************/
	if((u_int)-1 != (temp = inet_addr(server_name))){
		sin.sin_family = AF_INET;
		sin.sin_addr.s_addr = temp;
	}else{
		struct hostent *hp = NULL;

		if(NULL == (hp = gethostbyname(server_name))){
			scePrintf("unknown host %s\n", server_name);
			ERR_STOP;
		}
		sin.sin_family = hp->h_addrtype;
		bcopy(hp->h_addr, (caddr_t)&sin.sin_addr, hp->h_length);
	}
	sin.sin_port = htons(80);

	for(i = 0; i < 5; i++){
		if(0 > (s = socket(AF_INET, SOCK_STREAM, 0))){
			scePrintf("socket() failed (errno=%d)\n", errno);
			ERR_STOP;
		}
		if(0 == (ret = connect(s, (struct sockaddr *)&sin,
				sizeof(sin))))
			goto ok;
		shutdown(s, SHUT_RDWR);
		if(errno != ECONNREFUSED){
			scePrintf("connect() failed (errno=%d)\n", errno);
			ERR_STOP;
		}
	}
	scePrintf("too many refused\n");
	ERR_STOP;
ok:
	strcpy(get_cmd, REQUEST_GET1);
	strcat(get_cmd, server_name);
	strcat(get_cmd, REQUEST_GET2);

	tcp_len = strlen(get_cmd);
	if(tcp_len != (ret = send(s, get_cmd, tcp_len, 0))){
		scePrintf("send() failed %d->%d (errno=%d)\n",
			tcp_len, ret, errno);
		ERR_STOP;
	}
	if(0 > (fd = sceOpen("host0:index.html",
			SCE_RDWR | SCE_CREAT | SCE_TRUNC))){
		scePrintf("failed to open index.html\n");
		ERR_STOP;
	}
	while(0 < (tcp_len = recv(s, buffer0, sizeof(buffer0), 0))){
		sceWrite(fd, buffer0, tcp_len);
		total_len += tcp_len;
		scePrintf("tcp_len = %d, total_len = %d\n", tcp_len, total_len);
	} 
	sceClose(fd);
	shutdown(s, SHUT_RDWR);
	sceInsockTerminate(0);

	down_interface(&cd, rpc_buffer, if_id);

	scePrintf("done.\n");


	/***************************************************************
		Terminate libnet
	***************************************************************/
	down_interface(&cd, rpc_buffer, 0);
	if((ret = unreg_handler(&cd, rpc_buffer)) < 0){
		scePrintf("unreg_handler() failed.\n");
		ERR_STOP;
	}
	libnet_term(&cd);
	sceSifMExitRpc();

	return 0;
}

/*** End of file ***/

