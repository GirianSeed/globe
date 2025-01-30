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
 *       1.0.0     Sep,19,2000    munekis     first version
 *       1.1.0     Dec,22,2000    komaki      remake about if event
 *       1.2.0     Jan,11,2001    komaki      load inetctl.irx
 *       1.3.0     Oct,18,2001    ksh         down if & improvement
 *  --------------------------------------------------------------------
 *       1.4.0     2002/01/29     ksh         remake
 *       1.5.0     2003/04/02     ksh         USB to HDD Ethernet etc.
 *                 2003/04/15     ksh         char net_buf[]
 *                 2003/04/28     ksh         add erx handling
 *                 2003/04/30     ksh         add libmc2 handling
 */

/**********************************************************************/
#define	sceLibnetDisableAliases

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

#if 0
#define USE_MC2
#endif

#if 1
#define UP_INTERFACE_AUTO
#endif

#include <eekernel.h>
#if defined( USE_MC2 )
#include <libdbc.h>
#include <libmc2.h>
#include <netcnfif.h>
#endif
#if defined( USE_ERX )
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
#define	IOP_MOD_NETCNFIF  MAIN_MOD_ROOT "netcnfif.irx"
#define IOP_MOD_INETLOG   SCETOP "iop/util/inet/inetlog.irx"
#define	IOP_MOD_MODEMDRV  ""
#define	NET_DB            SCETOP "conf/net/net.db"
#define INET_ARG          "debug=18"
#define	INETCTL_ARG       "-no_auto" "\0" "-no_decode"
#define	LIBNET_ARG        "-verbose"
#define	NETCNF_ICON       SETAPP_ROOT "SYS_NET.ICO"
#define	NETCNF_ICONSYS    SETAPP_ROOT "icon.sys"
#define	NETCNF_ARG        "icon=" NETCNF_ICON "\0" "iconsys=" NETCNF_ICONSYS
#define	MODEMDRV_ARG      ""

#if defined( USE_ERX )
#define	EE_MOD_ROOT       SCETOP "ee/modules/"
#define	EE_MOD_LIBCDVD    EE_MOD_ROOT "libcdvd.erx"
#define	EE_MOD_LIBSCF     EE_MOD_ROOT "libscf.erx"
#define	EE_MOD_LIBMRPC    EE_MOD_ROOT "libmrpc.erx"
#define	EE_MOD_LIBNET     EE_MOD_ROOT "libnet.erx"
#define	EE_MOD_LIBNETIF   EE_MOD_ROOT "libnetif.erx"
#define	EE_MOD_NETCNFIF   EE_MOD_ROOT "netcnfif.erx"
#define	EE_MOD_NETIFMC2   EE_MOD_ROOT "netifmc2.erx"

#define LIBERX_THREAD_PRIORITY	( 10 )
#define LIBERX_MEMORY_SIZE	( 8 * 1024 * 1024 )
#define LIBERX_MAX_MODULES	( 32 )
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

#if defined( USE_MC2 )
#define	IOP_MOD_SIO2MAN   MAIN_MOD_ROOT "sio2man.irx"
#define	IOP_MOD_SIO2D     MAIN_MOD_ROOT "sio2d.irx"
#define	IOP_MOD_DBCMAN    MAIN_MOD_ROOT "dbcman.irx"
#define	IOP_MOD_MC2       MAIN_MOD_ROOT "mc2/mc2_d.irx"

#define	EE_MOD_LIBDBC     EE_MOD_ROOT "libdbc.erx"
#define	EE_MOD_LIBMC2     EE_MOD_ROOT "libmc2.erx"

#undef NET_DB
#undef USR_CONF_NAME
#define	NET_DB            "mc0:"
#define USR_CONF_NAME     "Combination1"
#endif

/**********************************************************************/
#define	PRIVATE	static
#define	PUBLIC
#define ERR_STOP	for( ; ; )

#define NETBUFSIZE	( 2048 )
#define BUFSIZE	( 1460 )

#define REQUEST_GET1	"GET / HTTP/1.1\r\nHOST: "
#define REQUEST_GET2	"\r\n\r\n"


PRIVATE	int 	LoadModule( const char *module, int args, const char *argp )
{
	int 	id;
	
	while ( ( id = sceSifLoadModule( module, args, argp ) ) < 0 ) {
		scePrintf( "%s cannot be loaded.\n", module );
	}

	return( id );
}

#if defined( USE_ERX )
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
	res = sceErxRegisterLibraryEntries( sceLibcStringGetErxEntries() );
	scePrintf( "sceErxExportLibcString\n" );
	res = sceErxRegisterLibraryEntries( sceLibcMallocGetErxEntries() );
	scePrintf( "sceErxExportLibcMalloc\n" );

	return( 0 );
}
#endif

int 	main( int argc, char *argv[] )
{
#if defined( USE_MC2 )
	int 	mc2_tid;
	int 	old_priority;
#endif
#if defined( USE_ERX )
	void	*liberx_pool;
#endif
	static u_char	net_buf[ NETBUFSIZE ] __attribute__( ( aligned( 64 ) ) );
	sceSifMClientData	cd;
	sceInetAddress_t	myaddr;
	sceInetParam_t	param;
	const char	*server_name;
	char	myname[ 128 ];
	char	myptrname[ 128 ];
	char	get_cmd[ 128 ];
	char	buffer[ BUFSIZE ];
	int 	tcpid;
	int 	fd;
	int 	total_len = 0;
	int 	flags;
	int 	if_id[ sceLIBNET_MAX_INTERFACE ];
	int 	i;
	int 	ret;

	if ( argc != 2 ) {
		scePrintf( "usage: %s <URL>\n", argv[ 0 ] );
		ERR_STOP;
	}
	server_name = argv[ 1 ];


	/***************************************************************
		Preparation
	***************************************************************/
#if defined( USE_ERX )
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
	LoadModule( IOP_MOD_LIBNET, sizeof( LIBNET_ARG ), LIBNET_ARG );
	LoadModule( IOP_MOD_NETCNFIF, 0, NULL );

#if defined( HDD_ETHERNET ) || defined( HDD_ETHERNET_WITH_PPPOE )
	PreparePowerOff();
#endif

#if defined( USE_MC2 )
	LoadModule( IOP_MOD_SIO2MAN, 0, NULL );
	LoadModule( IOP_MOD_SIO2D, 0, NULL );
	LoadModule( IOP_MOD_DBCMAN, 0, NULL );
	LoadModule( IOP_MOD_MC2, 0, NULL );
#endif

#if defined( USE_ERX )
	LoadErxModule( EE_MOD_LIBMRPC, 0, NULL );
	LoadErxModule( EE_MOD_LIBNET, 0, NULL );
	LoadErxModule( EE_MOD_NETCNFIF, 0, NULL );
	LoadErxModule( EE_MOD_LIBNETIF, 0, NULL );
#endif

#if defined( USE_ERX ) && defined( USE_MC2 )
	LoadErxModule( EE_MOD_LIBDBC, 0, NULL );
	LoadErxModule( EE_MOD_LIBMC2, 0, NULL );
	LoadErxModule( EE_MOD_NETIFMC2, 0, NULL );
#endif

	/***************************************************************
		Initialize Libnet
	***************************************************************/
	sceSifMInitRpc( 0 );
	ret = sceLibnetInitialize( &cd, sizeof( net_buf ), sceLIBNET_STACKSIZE, sceLIBNET_PRIORITY );
	if ( ret < 0 ) {
		scePrintf( "sceLibnetInitialize() failed.(%d)\n", ret );
		ERR_STOP;
	}
	ret = sceLibnetRegisterHandler( &cd, net_buf );
	if ( ret < 0 ) {
		scePrintf( "sceLibnetRegisterHandler() failed.(%d)\n", ret );
		ERR_STOP;
	}

#if defined( USE_MC2 )
	sceDbcInit();
	sceMc2Init( 0 );
	sceNetcnfifInit();
	old_priority = ChangeThreadPriority( GetThreadId(), 121 );
	mc2_tid = sceNetcnfifCreateMc2Thread( 120 );
	if ( mc2_tid < 0 ) {
		scePrintf( "sceNetcnfifCreateMc2Thread() failed.(%d)\n", mc2_tid );
	}
#endif

#ifdef  UP_INTERFACE_AUTO
#if defined( USE_MC2 )
	scePrintf( "up interface auto with libmc2\n" );
	ret = load_set_conf_extra( &cd, net_buf, NET_DB, USR_CONF_NAME, sceLIBNETF_AUTO_UPIF | sceLIBNETF_DECODE );
#else
	scePrintf( "up interface auto\n" );
	ret = load_set_conf_extra( &cd, net_buf, NET_DB, USR_CONF_NAME, sceLIBNETF_AUTO_UPIF );
#endif
	if ( ret < 0 ) {
		scePrintf( "load_set_conf_extra() failed.(%d)\n", ret );
		ERR_STOP;
	}
	ret = sceLibnetWaitGetAddress( &cd, net_buf, if_id, sceLIBNET_MAX_INTERFACE, &myaddr, sceLIBNETF_AUTO_UPIF );
	if ( ret < 0 ) {
		scePrintf( "sceLibnetWaitGetAddress() failed.(%d, %d)\n", ret, if_id[ 0 ] );
		ERR_STOP;
	}

#else
#if defined( USE_MC2 )
	scePrintf( "up interface no auto with libmc2\n" );
	ret = load_set_conf_extra( &cd, net_buf, NET_DB, USR_CONF_NAME, sceLIBNETF_DECODE );
#else
	scePrintf( "up interface no auto\n" );
	ret = load_set_conf_extra( &cd, net_buf, NET_DB, USR_CONF_NAME, 0 );
#endif
	if ( ret < 0 ) {
		scePrintf( "load_set_conf_extra() failed.(%d)\n", ret );
		ERR_STOP;
	}
	ret = sceLibnetWaitGetInterfaceID( &cd, net_buf, if_id, sceLIBNET_MAX_INTERFACE );
	if ( ret < 0 ) {
		scePrintf( "sceLibnetWaitGetInterfaceID() failed.(%d, %d)\n", ret, if_id[ 0 ] );
		ERR_STOP;
	}
	ret = sceInetCtlUpInterface( &cd, net_buf, 0 );
	if ( ret < 0 ) {
		scePrintf( "sceInetCtlUpInterface() failed.(%d)\n", ret );
		ERR_STOP;
	}
	ret = sceLibnetWaitGetAddress( &cd, net_buf, if_id, sceLIBNET_MAX_INTERFACE, &myaddr, 0 );
	if ( ret < 0 ) {
		scePrintf( "sceLibnetWaitGetAddress() failed.(%d, %d)\n", ret, if_id[ 0 ] );
		ERR_STOP;
	}

#endif  /*** UP_INTERFACE_AUTO ***/

#if defined( USE_MC2 )
	ret = sceNetcnfifDeleteMc2Thread( mc2_tid );
	if ( ret < 0 ) {
		scePrintf( "sceNetcnfifDeleteMc2Thread() failed.(%d)\n", ret );
	}
	ChangeThreadPriority( GetThreadId(), old_priority );
	sceNetcnfifTerm();
	sceMc2End();
	sceDbcEnd();
#endif
	
	for ( i = 0; i < sceLIBNET_MAX_INTERFACE; i++ ) {
		if ( if_id[ i ] == 0 ) {
			break;
		}
		scePrintf( "interface: %d\n", if_id[ i ] );
	}


	/***************************************************************
		Display IP address
	***************************************************************/
	ret = sceInetAddress2String( &cd, net_buf, myptrname, 128, &myaddr );
	if ( ret < 0 ) {
		scePrintf( "sceInetAddress2String() failed.(%d)\n", ret );
		ERR_STOP;
	}
	ret = sceInetAddress2Name( &cd, net_buf, 0, myname, 128, &myaddr, 0, 0 );
	if ( ret < 0 ) {
		scePrintf( "My address: %s\n", myptrname );
	}
	else {
		scePrintf( "My address: %s (%s)\n", myname, myptrname );
	}


	/***************************************************************
		Main
	***************************************************************/
	memset( &param, 0, sizeof( sceInetParam_t ) );
	param.type = sceINETT_CONNECT;
	param.local_port = sceINETP_AUTO;
	ret = sceInetName2Address( &cd, net_buf, 0, &param.remote_addr, server_name, 0, 0 );
	if ( ret < 0 ) {
		scePrintf( "%s, %d, sceInetName2Address : DNS failed : %s, ret = %d\n", __FILE__, __LINE__, server_name, ret );
		ERR_STOP;
	}
	param.remote_port = 80;

	ret = sceInetAddress2String( &cd, net_buf, buffer, 128, &param.remote_addr );
	if ( ret < 0 ) {
		scePrintf( "sceInetAddress2String() failed.(%d)\n", ret );
		ERR_STOP;
	}
	scePrintf( "retrieved address for %s = %s\n", server_name, buffer );

	tcpid = sceInetCreate( &cd, net_buf, &param );
	if ( tcpid < 0 ) {
		scePrintf( "%s, %d, sceInetCreate() failed.(%d)\n", __FILE__, __LINE__, tcpid );
		ERR_STOP;
	}

	ret = sceInetOpen( &cd, net_buf, tcpid, -1 );
	if ( ret < 0 ) {
		scePrintf( "sceInetOpen() failed.(%d)\n", ret );
		goto end;
	}

	strcpy( get_cmd, REQUEST_GET1 );
	strcat( get_cmd, server_name );
	strcat( get_cmd, REQUEST_GET2 );

	flags = 0;
	ret = sceInetSend( &cd, net_buf, tcpid, get_cmd, strlen( get_cmd ), &flags, -1 );
	if ( ret < 0 ) {
		scePrintf( "sceInetSend() failed.(%d)\n", ret );
		goto end;
	}

	fd = sceOpen( "host0:index.html", SCE_RDWR | SCE_CREAT | SCE_TRUNC );
	if ( fd < 0 ) {
		scePrintf( "failed to open index.html\n" );
		goto end;
	}

	for ( ; ; ) {
		flags = 0;
		ret = sceInetRecv( &cd, net_buf, tcpid, buffer, BUFSIZE, &flags, -1 );
		if ( ret < 0 ) {
			scePrintf( "sceInetRecv() failed.(%d)\n", ret );
			break;
		}
		if ( ret > 0 ) {
			sceWrite( fd, buffer, ret );
			total_len += ret;
			scePrintf( "ret = %d, total_len = %d\n", ret, total_len );
		}
		if ( flags & sceINETF_FIN ) {
			break;
		}
	}

	sceClose( fd );

end:
	ret = sceInetClose( &cd, net_buf, tcpid, -1 );
	if ( ret < 0 ) {
		scePrintf( "sceInetClose() failed.(%d)\n", ret );
	}
	else {
		scePrintf( "sceInetClose() done.\n" );
	}

	scePrintf( "done.\n" );


	/***************************************************************
		Terminate Libnet
	***************************************************************/
	ret = sceInetCtlDownInterface( &cd, net_buf, 0 );
	if ( ret < 0 ) {
		scePrintf( "sceInetCtlDownInterface() failed.(%d)\n", ret );
		ERR_STOP;
	}
	ret = sceLibnetUnregisterHandler( &cd, net_buf );
	if ( ret < 0 ) {
		scePrintf( "sceLibnetUnregisterHandler() failed.(%d)\n", ret );
		ERR_STOP;
	}
	ret = sceLibnetTerminate( &cd );
	if ( ret < 0 ) {
		scePrintf( "sceLibnetTerminate() failed.(%d)\n", ret );
		ERR_STOP;
	}
	sceSifMExitRpc();

	return( 0 );
}

/*** End of file ***/

